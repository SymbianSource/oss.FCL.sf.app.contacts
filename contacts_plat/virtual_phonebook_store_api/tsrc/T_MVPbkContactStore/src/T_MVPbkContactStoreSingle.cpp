/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 * NOTE! When running individual test cases, memory leak might be occured.
 * Check first that those leeks are not from ECom components (see documentation
 * REComSession::FinalClose()). VPbk does not use FinalClose() like it should
 * be used 'cause its only valid in debug builds.
 */

//  CLASS HEADER
#include "T_MVPbkContactStoreSingle.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <barsc.h>
#include <barsread.h>
#include <s32mem.h>
#include <ECom/ECom.h>
#include <bautils.h>

//  INTERNAL INCLUDES
#include <CVPbkContactManager.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <TVPbkFieldTypeMapping.h>
#include <CVPbkSortOrder.h>
#include <MVPbkContactView.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactStoreInfo.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactBookmark.h>
#include <CVPbkFieldTypeRefsList.h>
#include <TVPbkWordParserCallbackParam.h>
#include <VPbkEng.rsg>
#include <VPbkContactViewFilterBuilder.h>
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactBookmarkCollection.h>

#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldTypeIterator.h>

#include "T_ContactViewObserver.h"
#include "T_TestSuiteUtilities.h"
#include "VPbkFieldType.hrh"
#include <cntdb.h>
#include <EUnitMacros.h>
#include <barsc.h>

#include <s32mem.h>

// LOCAL
namespace 
    {
#if defined(__WINS__)
    _LIT( KResFile, "z:\\resource\\T_MVPbkContactStore.rsc" );
#else    
    _LIT( KResFile, "c:\\resource\\T_MVPbkContactStore.rsc" );
#endif // defined(__WINS__)    
	const TInt KFirstIndex = 0;
	const TInt KMaxNumOfStoreContacts = 6;
    _LIT( KFirstNameFieldValue, "Firstname value" );
    _LIT( KLastNameFieldValue, "Lastname value" );
    _LIT( KFNameFieldValuex, "xxxFirstname" );
    _LIT( KLNameFieldValuex, "xxxLastname" );
    _LIT( KAlphabet, "abcdefghijklmnopqrstuvwxyz" );
    _LIT( KFirstNameLabel, "First name" );
    _LIT( KFirstNameNewLabel, "1st name" );
    _LIT( KTelNoValue, "555-1234567890" );
    
    _LIT( KGroupLabel, "Grouplabel" );
    
    _LIT( KFindString, "a" );
    _LIT( KFindStrings, "al" );
    
    MVPbkStoreContactField* CreateFieldTypeL(
            MVPbkStoreContact& aStoreContact, 
            const MVPbkFieldType& aFieldType )
        {
        MVPbkStoreContactField* field = aStoreContact.CreateFieldLC( aFieldType );
        CleanupStack::Pop(); // field
        return field;
        }

    void StopScheduler( CActiveSchedulerWait& aScheduler )
        {
        if( aScheduler.IsStarted() )
            {
            aScheduler.AsyncStop();
            }
        }

    } // namespace

// CALLBACK FUNCTIONS

void T_MVPbkContactStoreSingle::StoreReady( MVPbkContactStore& aContactStore )
    {
    EUNIT_ASSERT( &iStoreList->At( 0 ) == &aContactStore );
    
    // Event coming straight from the store, so no OpenComplete() expected
    if( iNoOpenCompleteFlag ) 
        {
        iNoOpenCompleteFlag = EFalse;
        iLastStoreError = KErrNone;
        iLastStoreEvent = EStoreReady;
        if( iExpectedStoreEvent == iLastStoreEvent )
        	{
	        iExpectedStoreResponseFound = ETrue;
        	}
        }
    StopScheduler( iStoreScheduler );
    }

void T_MVPbkContactStoreSingle::StoreUnavailable(
        MVPbkContactStore& aContactStore, TInt aReason )
    {
    EUNIT_ASSERT( &iStoreList->At( 0 ) == &aContactStore );
    
    // Event coming straight from the store, so no OpenComplete() expected
    if( iNoOpenCompleteFlag )
        {
        iNoOpenCompleteFlag = EFalse;
        iLastStoreError = aReason;
        iLastStoreEvent = EOpenError;
        iExpectedStoreResponseFound = ETrue;
        StopScheduler( iStoreScheduler );
        }
    }
    
void T_MVPbkContactStoreSingle::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
	{
	}
    

void T_MVPbkContactStoreSingle::OpenComplete()
    {
    iLastStoreError = KErrNone;
    iLastStoreEvent = EOpenCompleted;
    if( iExpectedStoreEvent == iLastStoreEvent )
    	{
        iExpectedStoreResponseFound = ETrue;
    	}
    StopScheduler( iStoreScheduler );
    }

void T_MVPbkContactStoreSingle::ContactOperationCompleted(
        TContactOpResult aResult )
    {
	iLastContactOpError = KErrNone;
	iLastContactOpEvent = EContactOperationCompleted;
	if( aResult.iOpCode == EContactReadAndLock || 
	     aResult.iOpCode == EContactRead )
	    {
	    iStoreContact = aResult.iStoreContact;
	    }
	    
    if( iExpectedContactOpEvent == iLastContactOpEvent )
    	{
        iExpectedContactOpResponseFound = ETrue;
    	}
	StopScheduler( iContactScheduler );
    }

void T_MVPbkContactStoreSingle::ContactOperationFailed(
        TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    iLastContactOpError = aErrorCode;
    iLastContactOpEvent = EContactOperationFailed;
    iExpectedContactOpResponseFound = ETrue;
	StopScheduler( iContactScheduler );
    }

void T_MVPbkContactStoreSingle::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    iStoreContact = aContact;
    iLastSingleContactOpError = KErrNone;
    iLastSingleContactOpEvent = ESingleContactOperationCompleted;
    CActiveScheduler::Stop();
    }

void T_MVPbkContactStoreSingle::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iLastSingleContactOpError = aError;
    iLastSingleContactOpEvent = ESingleContactOperationFailed;
    CActiveScheduler::Stop();
    }

void T_MVPbkContactStoreSingle::ContactViewReady(
        MVPbkContactViewBase& /*aView*/ )
    {
    iLastViewError = KErrNone;
    iLastViewEvent = EContactViewReady;
    
    if( iExpectedViewEvent == iLastViewEvent )
    	{
        iExpectedViewResponseFound = ETrue;
    	}
    
    ++iCurViewEventCount;
    if ( iExpectedViewEventCount == iCurViewEventCount )
        {
        iExpectedViewEventCount = 1;
        iCurViewEventCount = 0;
        StopScheduler( iViewScheduler );
        }
    }

void T_MVPbkContactStoreSingle::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    iLastViewError = KErrNone;
    iLastViewEvent = EContactViewUnavailable;
    if( iExpectedViewEvent == iLastViewEvent )
    	{
        iExpectedViewResponseFound = ETrue;
    	}
    
    ++iCurViewEventCount;
    if ( iExpectedViewEventCount == iCurViewEventCount )
        {
        iExpectedViewEventCount = 1;
        iCurViewEventCount = 0;
        StopScheduler( iViewScheduler );
        }
    }

void T_MVPbkContactStoreSingle::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
        const MVPbkContactLink& /*aLink*/ )
    {
    iLastViewError = KErrNone;
    iLastViewEvent = EContactAddedToView;
    if( iExpectedViewEvent == iLastViewEvent )
    	{
        iExpectedViewResponseFound = ETrue;
    	}
    
    ++iCurViewEventCount;
    if ( iExpectedViewEventCount == iCurViewEventCount )
        {
        iExpectedViewEventCount = 1;
        iCurViewEventCount = 0;
        StopScheduler( iViewScheduler );
        }
    }

void T_MVPbkContactStoreSingle::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
        const MVPbkContactLink& /*aLink*/ )
    {
    iLastViewError = KErrNone;
    iLastViewEvent = EContactRemovedFromView;
    if( iExpectedViewEvent == iLastViewEvent )
    	{
        iExpectedViewResponseFound = ETrue;
    	}
    
    ++iCurViewEventCount;
    if ( iExpectedViewEventCount == iCurViewEventCount )
        {
        iExpectedViewEventCount = 1;
        iCurViewEventCount = 0;
        StopScheduler( iViewScheduler );
        }
    }

void T_MVPbkContactStoreSingle::ContactViewError(
        MVPbkContactViewBase& /*aView*/, TInt aError,
        TBool /*aErrorNotified*/ )
    {
    iLastViewError = aError;
    iLastViewEvent = EContactViewError;
    iExpectedViewResponseFound = ETrue;
    StopScheduler(iViewScheduler);
    }

void T_MVPbkContactStoreSingle::StepComplete(
        MVPbkContactOperationBase& /*aOperation*/, TInt aStepSize )
    {
    iStepCounter += aStepSize;
    iLastBatchOpError = KErrNone;
    iLastBatchOpEvent = EStepComplete;
    }

TBool T_MVPbkContactStoreSingle::StepFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aStepSize*/,
        TInt aError )
    {
    iLastBatchOpError = aError;
    iLastBatchOpEvent = EStepFailed;
    CActiveScheduler::Stop();
    return EFalse;
    }

void T_MVPbkContactStoreSingle::OperationComplete(
        MVPbkContactOperationBase& /*aOperation*/ )
    {
    iLastBatchOpError = KErrNone;
    iLastBatchOpEvent = EOperationComplete;
    CActiveScheduler::Stop();
    }

void T_MVPbkContactStoreSingle::FindCompleteL(
        MVPbkContactLinkArray* aResults )
    {
    iContactLinks = aResults;
    iLastFindError = KErrNone;
    iLastFindEvent = EFindComplete;
    CActiveScheduler::Stop();
    }

void T_MVPbkContactStoreSingle::FindFailed( TInt aError )
    {
    iContactLinks = NULL;
    iLastFindError = aError;
    iLastFindEvent = EFindComplete;
    CActiveScheduler::Stop();
    }

// from MVPbkContactFindFromStoresObserver
void T_MVPbkContactStoreSingle::FindFromStoreSucceededL( 
        MVPbkContactStore& /*aStore*/,
        MVPbkContactLinkArray* aResultsFromStore )
    {
    // Must not return NULL
    EUNIT_ASSERT( aResultsFromStore );
    CleanupDeletePushL( aResultsFromStore );
    
    if( !iFindResults )
        {
        iFindResults = CVPbkContactLinkArray::NewL();
        }
    
    const TInt count = aResultsFromStore->Count();
    for( TInt i = 0; i < count; ++i )
        {
        iFindResults->AppendL( aResultsFromStore->At( i ).CloneLC() );
        CleanupStack::Pop(); // new link
        }
    CleanupStack::PopAndDestroy(); // aResultsFromStore
    iLastFindError = KErrNone;
    }

// from MVPbkContactFindFromStoresObserver
void T_MVPbkContactStoreSingle::FindFromStoreFailed(
        MVPbkContactStore& /*aStore*/, TInt aError )
    {
    iLastFindError = aError;
    }

// from MVPbkContactFindFromStoresObserver
void T_MVPbkContactStoreSingle::FindFromStoresOperationComplete()
    {
    // This is always called from find, and only one time.
    
    // In case search did not result in a success nor failure, e.g.
    // search with empty search terms.
    if( !iFindResults )
        {
        iFindResults = CVPbkContactLinkArray::NewL();
        }

    iLastFindEvent = EFindWithParserComplete;
    CActiveScheduler::Stop();
    }

TInt VPbkParseDataL( TAny* aParam )
    {
    TVPbkWordParserCallbackParam* param = 
        static_cast< TVPbkWordParserCallbackParam* >( aParam );
    const TText* ptr = param->iStringToParse->Ptr();
	const TText* end = ptr + param->iStringToParse->Length();
	const TText* startOfWord = NULL;
	FOREVER
		{
		// Other than alpha digit are separators
		if( ptr == end || !TChar( *ptr ).IsAlphaDigit() )
			{
			if( startOfWord )
				{
				TPtrC addWord( startOfWord, ptr - startOfWord );
				param->iWordArray->AppendL( addWord );
				startOfWord=NULL;
				}
			if( ptr == end )
			    {
			    break;
			    }
			}
		else if ( !startOfWord )
		    {
		    startOfWord = ptr;
		    }
		ptr++;
		}
	return KErrNone;
    }
    
    
// CONSTRUCTION

T_MVPbkContactStoreSingle* T_MVPbkContactStoreSingle::NewL(
        const TVPbkContactStoreUriPtr& aUri )
    {
    T_MVPbkContactStoreSingle* self = T_MVPbkContactStoreSingle::NewLC( aUri ); 
    CleanupStack::Pop();

    return self;
    }

T_MVPbkContactStoreSingle* T_MVPbkContactStoreSingle::NewLC(
        const TVPbkContactStoreUriPtr& aUri )
    {
    T_MVPbkContactStoreSingle* self = 
        new( ELeave ) T_MVPbkContactStoreSingle();
    CleanupStack::PushL( self );

	self->ConstructL( aUri );

    return self;
    }

T_MVPbkContactStoreSingle::~T_MVPbkContactStoreSingle()
    {
    delete iFindStrings;
    iFs.Close();
    delete iUriToTest;
    
    Teardown();
    }

T_MVPbkContactStoreSingle::T_MVPbkContactStoreSingle()
:   iNoOpenCompleteFlag( EFalse ),
    iStoreUnavailable( EFalse ),
    iExpectedViewEventCount( 1 ) // default: 1 event will stop the scheduler
    {
    }

void T_MVPbkContactStoreSingle::ConstructL(
        const TVPbkContactStoreUriPtr& aUri )
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    iUriToTest = aUri.UriDes().AllocL();
    User::LeaveIfError( iFs.Connect() );
    TPtrC uri = iUriToTest->Des();
    EUNIT_PRINT( uri );
    
    // Use FinalClose() here to avoid resource and handle leaks for test
    // case scope. However, this does not eliminate DLL scope leaks that
    // may be present.
    REComSession::FinalClose();
    }

//  METHODS

void T_MVPbkContactStoreSingle::BaseSetupL(  )
    {
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
    uriArray->AppendL( TVPbkContactStoreUriPtr( *iUriToTest ) );
    iContactManager = CVPbkContactManager::NewL( *uriArray, &iFs );
    CleanupStack::PopAndDestroy( uriArray );

    iNoOpenCompleteFlag = EFalse;
    ResetTestSuiteEvents();
    }

void T_MVPbkContactStoreSingle::SetupStoreL()
    {
    BaseSetupL();
    ReplaceStoreL();
    }

void T_MVPbkContactStoreSingle::SetupViewL()
    {
    SetupStoreL();
    CreateContactViewL( iContactView, R_SORTORDER_LASTNAME_FIRSTNAME );
    }

void T_MVPbkContactStoreSingle::SetupDeleteContactsL()
    {
    SetupViewL();
    DeleteAllContactsL();
    }

void T_MVPbkContactStoreSingle::SetupDeleteContactsForMatchL()
    {
    SetupDeleteContactsL();

    // Find a telephone number field type supported by the current store
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iFs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( iFs, path );
    TResourceReader reader;
    HBufC8* selectorBuffer = 
        resFile.AllocReadLC( R_VPBK_PHONE_NUMBER_SELECTOR );
    reader.SetBuffer( selectorBuffer );

    const MVPbkFieldTypeList& supportedFields = 
        iContactStore->StoreProperties().SupportedFields();
    CVPbkFieldTypeSelector* fieldTypeSelector =
        CVPbkFieldTypeSelector::NewL( reader, supportedFields );
    CleanupStack::PushL( fieldTypeSelector );
    CVPbkFieldTypeIterator* fieldTypeIterator =
        CVPbkFieldTypeIterator::NewLC( *fieldTypeSelector, supportedFields );

    const MVPbkFieldType* supportedFieldType = NULL;
    if( fieldTypeIterator->HasNext() )
        {
        supportedFieldType = fieldTypeIterator->Next();
        }

    if( supportedFieldType )
        {
        TInt addedFieldCount( 0 );
        // Create new contact with specified telephone number field
        MVPbkStoreContact* storeContact = iContactStore->CreateNewContactLC();
        EUNIT_ASSERT_EQUALS( storeContact->Fields().FieldCount(), 0 );

        addedFieldCount += AddValueToContactFieldL(
            *storeContact, *supportedFieldType, KTelNoValue );
        addedFieldCount += AddContactFieldsAndSaveL(
            *storeContact, KFNameFieldValuex, KLNameFieldValuex );
        EUNIT_ASSERT( storeContact->Fields().FieldCount() >= addedFieldCount );
        CleanupStack::PopAndDestroy(); // storeContact
        
        iTestSupportedByStore = ETrue;
        }
    CleanupStack::PopAndDestroy( 4 ); // fieldTypeIterator, fieldTypeSelector,
                                      // selectorBuffer, resFile
    }

void T_MVPbkContactStoreSingle::SetupStoreWithContactsL()
    {
    // TODO: Is the contact store expected to already have some contacts?
    SetupStoreL();
    CreateContactViewL( iContactView, R_SORTORDER_LASTNAME_FIRSTNAME );
    TInt add = KMaxNumOfStoreContacts - iContactView->ContactCountL();
    if( add > 0 )
        {
        PopulateContactStoreL( add );
        }
    }

void T_MVPbkContactStoreSingle::SetupStoreWithContactsAndObserverL()
    {
    SetupStoreWithContactsL();

    iViewObserver = new( ELeave ) T_ContactViewObserver;
    iContactView->AddObserverL( *iViewObserver );
    }

void T_MVPbkContactStoreSingle::SetupStoreForFindL()
    {
    SetupDeleteContactsL();    
    TInt add = KMaxNumOfStoreContacts - iContactView->ContactCountL();
    if( add > 0 )
        {
        PopulateContactStoreL( add );
        EUNIT_ASSERT_EQUALS( iContactView->ContactCountL(), add );
        }
    }

void T_MVPbkContactStoreSingle::SetupStoresForSelectiveFindL()
	{
	SetupStoreWithContactsL();
    }

void T_MVPbkContactStoreSingle::SetupStoreForFindWithIdentifierLastL()
    {
    SetupDeleteContactsL();    
    TInt add = KMaxNumOfStoreContacts - iContactView->ContactCountL();
    if( add > 0 )
        {
        PopulateContactStoreL( add, EFalse );
        EUNIT_ASSERT_EQUALS( iContactView->ContactCountL(), add );
        }
    }

void T_MVPbkContactStoreSingle::SetupStoreForFindFailL()
    {
    // First setup store to ensure that all stores are available 
    // before making open all.
    SetupStoreL();
    iStoreList->CloseAll( *this );
    iStoreList->OpenAllL( *this );
    // Don't wait opencomplete
    }

void T_MVPbkContactStoreSingle::SetupBigStoreL()
    {
    SetupStoreL();
    CreateContactViewL( iContactView, R_SORTORDER_LASTNAME_FIRSTNAME );
    const TInt maxPopulation = 20;
    TInt add = maxPopulation - iContactView->ContactCountL();
    if( add > 0 )
        {
        PopulateContactStoreL( add );
        }
    }

void T_MVPbkContactStoreSingle::SetupStoreAndTwoViewsWithContactsL()
    {
    SetupStoreWithContactsL();

    CreateContactViewL( iContactView2, R_SORTORDER_LASTNAME_FIRSTNAME );
    }
    
void T_MVPbkContactStoreSingle::SetupCreateFindStringsL()
    {
    SetupStoreWithContactsL();
    
    iFindStrings = new(ELeave)CDesCArrayFlat( 2 );
    }    

void T_MVPbkContactStoreSingle::SetupCreateFindStrings2L()
    {
    SetupViewL();
    
    iFindStrings = new(ELeave)CDesCArrayFlat( 2 );
    }        

void T_MVPbkContactStoreSingle::Teardown()
    {
    delete iFindStrings;
    iFindStrings = NULL;
    
    delete iFindResults;
    iFindResults = NULL;

    delete iContactLinks;
    iContactLinks = NULL;

    iStoreList = NULL;

    delete iBaseView;
    iBaseView = NULL;

    delete iStoreContact;
    iStoreContact = NULL;

    if( iViewObserver && iContactView )
        {
        iContactView->RemoveObserver( *iViewObserver );
        }
    delete iContactView;
    iContactView = NULL;

    delete iContactView2;
    iContactView2 = NULL;

    if( iContactStore )
        {
        iContactStore->Close( *this );
        }
    iContactStore = NULL;

    delete iContactManager;
    iContactManager = NULL;

    iTestSupportedByStore = EFalse;

    delete iViewObserver;
    iViewObserver = NULL;
    }

// TESTS

void T_MVPbkContactStoreSingle::TestGetStoreListL()
    {
    iStoreList = &iContactManager->ContactStoresL();
    EUNIT_ASSERT_EQUALS_DESC( iStoreList->Count(), 1,
        "ContactStoreList count was not 1" );
    }

void T_MVPbkContactStoreSingle::TestOpenStoreL(  )
    {
    iStoreList = &iContactManager->ContactStoresL();
	EUNIT_ASSERT_EQUALS_DESC( iStoreList->Count(), 1,
        "ContactStoreList count was not 1" );
	
    // Try to open non existing db. 
    iStoreList->OpenAllL( *this );

    ResetTestSuiteEvents();
	iExpectedStoreEvent = EOpenCompleted;
    StartStoreSchedulerAndDecideToLeaveL();
    
    EUNIT_ASSERT_EQUALS( iLastStoreError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastStoreEvent, iExpectedStoreEvent );

    iStoreList->CloseAll( *this );
    }

void T_MVPbkContactStoreSingle::TestReplaceStoreL(  )
    {
	ReplaceStoreL();
    }

void T_MVPbkContactStoreSingle::TestCreateContactViewL()
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
        R_SINGLE_CONTACT_VIEW );
    viewDef->SetUriL( iUriToTest->Des() );
    
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_SORTORDER_LASTNAME_FIRSTNAME );

    iBaseView = iContactManager->CreateContactViewLC(
        *this, *viewDef, *sortOrder );
    CleanupStack::Pop(); // iBaseView
    EUNIT_ASSERT( iBaseView );
    
    ResetTestSuiteEvents();
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
    }

void T_MVPbkContactStoreSingle::TestCreateLocalContactViewL()
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
        R_SINGLE_CONTACT_VIEW );
    viewDef->SetUriL( iUriToTest->Des() );
    viewDef->SetSharing( EVPbkLocalView );
    
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_SORTORDER_LASTNAME_FIRSTNAME );

    TRAPD( res, 
        {
        iBaseView = iContactManager->CreateContactViewLC(
            *this, *viewDef, *sortOrder );
        CleanupStack::Pop(); // iBaseView
        });
        
    if ( res == KErrNone )
        {
        EUNIT_ASSERT( iBaseView );
        
        ResetTestSuiteEvents();
        iExpectedViewEvent = EContactViewReady;
        StartViewSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
        CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef    
        }
    else if ( res != KErrNotSupported )
        {
        // If store doesn't support local view then test was ok.
        User::Leave( res );
        }
    }
    
void T_MVPbkContactStoreSingle::TestCreateUnsortedContactViewL()
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
        R_SINGLE_CONTACT_VIEW );
    viewDef->SetUriL( iUriToTest->Des() );
    viewDef->SetSortPolicy( EVPbkUnsortedContactView );
    // Create a shared view with test name so that it doesn't
    // conflict with S60 shared views.
    _LIT( KViewName, "T_MVPbkContactStoreSingle_View" );
    viewDef->SetNameL( KViewName );
    
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_SORTORDER_LASTNAME_FIRSTNAME );

	// Create view
    TRAPD( res,
        {
        iBaseView = iContactManager->CreateContactViewLC(
                *this, *viewDef, *sortOrder );
        CleanupStack::Pop(); // iBaseView
        });
    
    // If the store doesn't support unsorted view then it leaves
    // KErrArgument.
    if ( res != KErrArgument )
        {
        // Leave if other error, e.g. KErrNoMemory
        User::LeaveIfError( res );
        EUNIT_ASSERT( res == KErrNone && iBaseView );
        
	    ResetTestSuiteEvents();
	    iExpectedViewEvent = EContactViewReady;
	    StartViewSchedulerAndDecideToLeaveL();
	    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
	    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
	    
	    TInt contactCount = iBaseView->ContactCountL();
	    // Test out of scope index
	    EUNIT_ASSERT_SPECIFIC_LEAVE( 
	    	iBaseView->ContactAtL( contactCount ), KErrArgument );
	    EUNIT_ASSERT_SPECIFIC_LEAVE( 
	    	iBaseView->CreateLinkLC( contactCount ), KErrArgument );
        }
	        
    CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
    }

void T_MVPbkContactStoreSingle::TestCreateSharedGroupViewL()
    {
    if ( iContactStore->StoreProperties().SupportsContactGroups() )
        {
        CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
            R_SINGLE_CONTACT_VIEW );
        viewDef->SetUriL( iUriToTest->Des() );
        viewDef->SetType( EVPbkGroupsView );
        viewDef->SetSharing( EVPbkSharedView );
        
        CVPbkSortOrder* sortOrder = CreateSortOrderLC(
            R_SORTORDER_GENERIC_LABEL );

        iBaseView = iContactManager->CreateContactViewLC(
            *this, *viewDef, *sortOrder );
        CleanupStack::Pop(); // iBaseView
        EUNIT_ASSERT( iBaseView );
        
        ResetTestSuiteEvents();
        iExpectedViewEvent = EContactViewReady;
        StartViewSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
        CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
        }
    }

void T_MVPbkContactStoreSingle::TestCreateLocalGroupViewL()
    {
    if ( iContactStore->StoreProperties().SupportsContactGroups() )
        {
        CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
            R_SINGLE_CONTACT_VIEW );
        viewDef->SetUriL( iUriToTest->Des() );
        viewDef->SetType( EVPbkGroupsView );
        viewDef->SetSharing( EVPbkLocalView );
        
        CVPbkSortOrder* sortOrder = CreateSortOrderLC(
            R_SORTORDER_GENERIC_LABEL );

        iBaseView = iContactManager->CreateContactViewLC(
            *this, *viewDef, *sortOrder );
        CleanupStack::Pop(); // iBaseView
        EUNIT_ASSERT( iBaseView );
        
        ResetTestSuiteEvents();
        iExpectedViewEvent = EContactViewReady;
        StartViewSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
        CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
        }
    }

void T_MVPbkContactStoreSingle::TestCreateContactViewWithSelectorL()
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
        R_SINGLE_CONTACT_VIEW );
    viewDef->SetUriL( iUriToTest->Des() );
    
    // Find a telephone number field type supported by the current store
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iFs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( iFs, path );
    
    TResourceReader reader;
    HBufC8* selectorBuffer = 
        resFile.AllocReadLC( R_VPBK_PHONE_NUMBER_SELECTOR );
    reader.SetBuffer( selectorBuffer );

    const MVPbkFieldTypeList& 
        supportedFields = iContactStore->StoreProperties().SupportedFields();
    CVPbkFieldTypeSelector* 
    	filter = CVPbkFieldTypeSelector::NewL( reader, supportedFields );
    CleanupStack::PushL( filter );
    viewDef->SetFieldTypeFilterL( filter );
    CleanupStack::PopAndDestroy( filter );
        
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_SORTORDER_LASTNAME_FIRSTNAME );

    iBaseView = iContactManager->CreateContactViewLC(
        *this, *viewDef, *sortOrder );
    CleanupStack::Pop(); // iBaseView
    EUNIT_ASSERT( iBaseView );
    
    ResetTestSuiteEvents();
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    CleanupStack::PopAndDestroy( 4 ); // sortOrder, selectorBuffer, resFile, viewDef
    }

void T_MVPbkContactStoreSingle::TestCreateContactViewWithSelector2L()
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC(
        R_SINGLE_CONTACT_VIEW );
    viewDef->SetUriL( iUriToTest->Des() );
    
    // Find a telephone number field type supported by the current store
    const MVPbkFieldTypeList& 
        supportedFields = iContactStore->StoreProperties().SupportedFields();
    CVPbkFieldTypeSelector* 
    	filter = CVPbkFieldTypeSelector::NewL( supportedFields );
    	
    EUNIT_ASSERT_SPECIFIC_LEAVE( 
    	VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    		*filter, EVPbkContactViewFilterNone, *iContactManager ), KErrNotSupported );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterMobileNumber, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterEmail, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterPhoneNumber, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterFaxNumber, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterRingingTone, *iContactManager );

    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterVideoNumber, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterPagerNumber, *iContactManager );
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
    	*filter, EVPbkContactViewFilterDTMF, *iContactManager );
    
    CleanupStack::PushL( filter );
    viewDef->SetFieldTypeFilterL( filter );
    CleanupStack::PopAndDestroy( filter );
        
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_SORTORDER_LASTNAME_FIRSTNAME );

    iBaseView = iContactManager->CreateContactViewLC(
        *this, *viewDef, *sortOrder );
    CleanupStack::Pop(); // iBaseView
    EUNIT_ASSERT( iBaseView );
    
    ResetTestSuiteEvents();
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
    }
    
void T_MVPbkContactStoreSingle::TestCreateContactL(  )
    {
    TInt expectedCount = iContactStore->StoreInfo().NumberOfContactsL();

    MVPbkStoreContact* storeContact = iContactStore->CreateNewContactLC();
    EUNIT_ASSERT( storeContact );
    AddContactFieldsAndSaveL( *storeContact, KFirstNameFieldValue, 
        KLastNameFieldValue );
    CleanupStack::PopAndDestroy(); // storeContact
    ++expectedCount;
    
    storeContact = iContactStore->CreateNewContactLC();
    EUNIT_ASSERT( storeContact );
    AddContactFieldsAndSaveL( *storeContact, KFirstNameFieldValue, 
        KLastNameFieldValue );
    CleanupStack::PopAndDestroy(); // storeContact
    storeContact = NULL;
    ++expectedCount;
    
    storeContact = iContactStore->CreateNewContactLC();
    EUNIT_ASSERT( storeContact );
    AddContactFieldsAndSaveL( *storeContact, KFirstNameFieldValue, 
        KLastNameFieldValue );
    CleanupStack::PopAndDestroy(); // storeContact
    storeContact = NULL;
    ++expectedCount;
    
    EUNIT_ASSERT_EQUALS( expectedCount, 
            iContactStore->StoreInfo().NumberOfContactsL() );
    }

void T_MVPbkContactStoreSingle::TestChangeContactViewSortOrderL()
    {
    CVPbkSortOrder* newSortOrder = 
        CreateSortOrderLC( R_SORTORDER_FIRSTNAME_LASTNAME );
    TRAPD( res, iContactView->ChangeSortOrderL( *newSortOrder ) );
    // KErrArgument is ok if the view is an existing shared view and
    // newSortOrder is not the same as the shared view's order.
    if ( res != KErrArgument )
        {
        User::LeaveIfError( res );

        ResetTestSuiteEvents();
        iExpectedViewEvent = EContactViewReady;
        StartViewSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );

        const MVPbkFieldTypeList& sortOrderFields = iContactView->SortOrder();
        TInt fieldTypeCount = sortOrderFields.FieldTypeCount();

        for ( TInt i = 0; i < fieldTypeCount; ++i )
            {
            EUNIT_ASSERT( sortOrderFields.FieldTypeAt( i ).IsSame( 
                newSortOrder->FieldTypeAt( i ) ) );
            }
        }
    CleanupStack::PopAndDestroy( newSortOrder );
    }

void T_MVPbkContactStoreSingle::TestRefreshContactViewL()
    {
    iContactView->RefreshL();

    ResetTestSuiteEvents();
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    }

void T_MVPbkContactStoreSingle::TestCreateContactViewLinkL()
    {
    // TODO: Why 1 and not 0 or something else?
    TInt linkIndex = 1;

    MVPbkContactLink* link = iContactView->CreateLinkLC( linkIndex );
    EUNIT_ASSERT_EQUALS( iContactView->IndexOfLinkL( *link ), linkIndex );

    CleanupStack::PopAndDestroy(); // link
    }

void T_MVPbkContactStoreSingle::TestCreateContactLinkNotCommitedL()
    {
    MVPbkStoreContact* contact = iContactStore->CreateNewContactLC();
    MVPbkContactLink* link = contact->CreateLinkLC();
    EUNIT_ASSERT( !link );
    CleanupStack::PopAndDestroy(); // contact
    }

void T_MVPbkContactStoreSingle::TestLinkRefersToL()
    {
    TInt linkIndex = 1;

    MVPbkContactLink* link = iContactView->CreateLinkLC( linkIndex );
    const MVPbkViewContact& contact = iContactView->ContactAtL( linkIndex );
    EUNIT_ASSERT( link->RefersTo( contact ) );

    CleanupStack::PopAndDestroy(); // link
    }    

void T_MVPbkContactStoreSingle::TestCreateContactViewBookmarkL()
    {
    // TODO: Why 1 and not 0 or something else?
    TInt bookmarkIndex = 1;

    MVPbkContactBookmark* bookmark = 
        iContactView->CreateBookmarkLC( bookmarkIndex );
    EUNIT_ASSERT_EQUALS(
        iContactView->IndexOfBookmarkL( *bookmark ), bookmarkIndex );

    CleanupStack::PopAndDestroy(); // bookmark
    }

void T_MVPbkContactStoreSingle::TestAddContactViewObserverL()
    {
    iViewObserver = new( ELeave ) T_ContactViewObserver;

    DoTestAddContactViewObserverL( *iViewObserver );
    }

void T_MVPbkContactStoreSingle::TestAddContactViewObserverTwiceL()
    {
    iViewObserver = new( ELeave ) T_ContactViewObserver;

    DoTestAddContactViewObserverL( *iViewObserver );

    // Add same observer again. Contact view should handle this ok.
    DoTestAddContactViewObserverL( *iViewObserver );
    }

void T_MVPbkContactStoreSingle::DoTestAddContactViewObserverL(
        T_ContactViewObserver& aObserver )
    {
    iContactView->AddObserverL( aObserver );

    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( aObserver.iLastError, KErrNone );
    EUNIT_ASSERT_EQUALS(
        aObserver.iLastEvent, T_ContactViewObserver::EContactViewReady );
    }

void T_MVPbkContactStoreSingle::TestRemoveContactViewObserverL()
    {
    iContactView->RemoveObserver( *iViewObserver );
    }

void T_MVPbkContactStoreSingle::TestRemoveNonexistentContactViewObserverL()
    {
    // The observer has not been added in the setup method
    iViewObserver = new( ELeave ) T_ContactViewObserver;

    iContactView->RemoveObserver( *iViewObserver );
    }

void T_MVPbkContactStoreSingle::TestCommitMultipleContactsL()
    {
    TInt cntCount( iContactView->ContactCountL() );
    RArray<MVPbkStoreContact*> contacts;
    CleanupClosePushL( contacts );

    // Read store contacts from view
    iContactView->ContactAtL( 0 ).ReadAndLockL( *this );
    
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );

    MVPbkStoreContact* cnt1 = iStoreContact;
    iStoreContact = NULL;
    cnt1->PushL();
    contacts.AppendL( cnt1 );

    iContactView->ContactAtL( 2 ).ReadAndLockL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );
    
    MVPbkStoreContact* cnt2 = iStoreContact;
    iStoreContact = NULL;
    CleanupDeletePushL( cnt2 );
    contacts.AppendL( cnt2 );

    MVPbkContactOperationBase* op = 
        iContactManager->CommitContactsL( contacts.Array(), *this );
    EUNIT_ASSERT( op );
    CleanupDeletePushL( op );
    
    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL(); 
    EUNIT_ASSERT_EQUALS( iLastBatchOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastBatchOpEvent, EOperationComplete );
    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy( 3 ); // cnt2, cnt1, contacts
    }

void T_MVPbkContactStoreSingle::TestContactRetrieverL()
    {
    // Create contact retriever with contact link
    const MVPbkViewContact& viewContact = iContactView->ContactAtL( 1 );
    MVPbkContactLink* cntLink = viewContact.CreateLinkLC();
    MVPbkContactOperationBase* op = 
        iContactManager->RetrieveContactL( *cntLink, *this );
    EUNIT_ASSERT( op );
    CleanupDeletePushL( op );

    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastSingleContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastSingleContactOpEvent, ESingleContactOperationCompleted );
    EUNIT_ASSERT( iStoreContact );
    EUNIT_ASSERT( iStoreContact->IsSame( viewContact ) );

    CleanupStack::PopAndDestroy(); // op

    // Compare links
    MVPbkContactLink* storeLnk = iStoreContact->CreateLinkLC();
    EUNIT_ASSERT( storeLnk );
    EUNIT_ASSERT( &storeLnk->ContactStore() == &cntLink->ContactStore() );
    CleanupStack::PopAndDestroy( 2 ); // storeLnk, cntLink
    delete iStoreContact; iStoreContact = NULL;

    // Create contact retriever from stream
    const MVPbkViewContact& contact = iContactView->ContactAtL( 1 );
    MVPbkContactLink* lnk = contact.CreateLinkLC();

    const MVPbkStreamable* streamableLnk = lnk->Streamable();
    if ( streamableLnk )
        {
        TInt size(streamableLnk->ExternalizedSize());
	    HBufC8* buffer = HBufC8::NewLC( size );
        TPtr8 bufferPtr( buffer->Des() );

        RDesWriteStream writeStream( bufferPtr );
        writeStream.PushL();
        streamableLnk->ExternalizeL( writeStream );
        writeStream.CommitL();
        
        RDesReadStream readStream(bufferPtr);
        readStream.PushL();
        MVPbkContactLinkArray* linkArray = 
            iContactManager->CreateLinksLC( readStream );
        
        EUNIT_ASSERT_EQUALS( linkArray->Count(), 1 );
        EUNIT_ASSERT( linkArray->At( 0 ).IsSame( *lnk ) );
        EUNIT_ASSERT_EQUALS( linkArray->Find( *lnk ), 0 );
        
        // linkArray, readStream, writeStream, buffer
        CleanupStack::PopAndDestroy(4);
        
        HBufC8* packed = lnk->PackLC();
        linkArray = iContactManager->CreateLinksLC( *packed );
            
        EUNIT_ASSERT_EQUALS( linkArray->Count(), 1 );
        EUNIT_ASSERT( linkArray->At( 0 ).IsSame( *lnk ) );
        EUNIT_ASSERT_EQUALS( linkArray->Find( *lnk ), 0 );
    
        CleanupStack::PopAndDestroy( 2 ); // linkArray, packed
        }
    CleanupStack::PopAndDestroy(); // lnk
    }

void T_MVPbkContactStoreSingle::TestMatchFullPhoneNumberL()
    {
    if( iTestSupportedByStore )
        {
        DoTestMatchFullPhoneNumberL();
        }
    else
        {
        EUNIT_PRINT( _L( "This test is not supported by the store (%S)" ), iUriToTest );
        }
    }

void T_MVPbkContactStoreSingle::DoTestMatchFullPhoneNumberL()
    {
    // 1. full number search
    _LIT( KMatchNo, "555-1234567890" );  
    MVPbkContactOperationBase* op =  iContactManager->MatchPhoneNumberL(
        KMatchNo, KMatchNo().Length(), *this );
    EUNIT_ASSERT( op );
    CleanupDeletePushL( op );

    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );
    EUNIT_ASSERT( iContactLinks->Count() >= 1 ); // At least one should be found
    CleanupStack::PopAndDestroy(); // op
    }

void T_MVPbkContactStoreSingle::TestMatchPartialPhoneNumberL()
    {
    if( iTestSupportedByStore )
        {
        DoTestMatchPartialPhoneNumberL();
        }
    else
        {
        EUNIT_PRINT( _L( "This test is not supported by the store (%S)" ), iUriToTest );
        }
    }

void T_MVPbkContactStoreSingle::DoTestMatchPartialPhoneNumberL()
    {
    // 2. partial number search
    _LIT( KMatchNo1, "234567890" );
    MVPbkContactOperationBase* op = iContactManager->MatchPhoneNumberL(
        KMatchNo1, KMatchNo1().Length(), *this );
    CleanupDeletePushL( op );

    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );
    EUNIT_ASSERT( iContactLinks->Count() >= 1 );
    CleanupStack::PopAndDestroy(); // op
    }

void T_MVPbkContactStoreSingle::TestNoMatchPhoneNumberL()
    {
    if( iTestSupportedByStore )
        {
        DoTestNoMatchPhoneNumberL();
        }
    else
        {
        EUNIT_PRINT( _L( "This test is not supported by the store (%S)" ), iUriToTest );
        }
    }

void T_MVPbkContactStoreSingle::DoTestNoMatchPhoneNumberL()
    {
    // No match
    _LIT( KNoMatch, "-1111111" );
    MVPbkContactOperationBase* op = iContactManager->MatchPhoneNumberL(
        KNoMatch, KNoMatch().Length(), *this );
    CleanupDeletePushL( op );

    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );
    EUNIT_ASSERT_EQUALS( iContactLinks->Count(), 0 );
    CleanupStack::PopAndDestroy(); // op
    }

void T_MVPbkContactStoreSingle::TestMatchPhoneNumber_FailL()  
    {
    // Make match before all stores are open, 
    // so before opencompleted has been called.
    _LIT( KMatchNo1, "234567890" );

	// KLastNameFieldValue
	MVPbkContactOperationBase* op = iContactManager->MatchPhoneNumberL(
                KMatchNo1, KMatchNo1().Length(), *this );
    CleanupDeletePushL(op);
    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    // Make sure that there still are an empty result array
    EUNIT_ASSERT_EQUALS( iContactLinks && iContactLinks->Count(), 0 );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );
    CleanupStack::PopAndDestroy(); // op
    }

void T_MVPbkContactStoreSingle::TestContactOperationsL()
    {
    // ReadL
    const MVPbkViewContact& viewCnt1 = iContactView->ContactAtL( 1 );
    viewCnt1.ReadL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );
    EUNIT_ASSERT( iStoreContact->IsSame( viewCnt1 ) );
    EUNIT_ASSERT( viewCnt1.IsSame( *iStoreContact ) );

    // CommitL failing
    iStoreContact->CommitL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationFailed;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrAccessDenied );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );

    // IsSame
    MVPbkStoreContact* storeCnt = iStoreContact;
    iStoreContact = NULL;
    CleanupDeletePushL( storeCnt );

    const MVPbkViewContact& sameCnt = iContactView->ContactAtL( 1 );
    sameCnt.ReadL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact->IsSame( *storeCnt ) );
    CleanupStack::PopAndDestroy(); // storeCnt

    // LockL
    iStoreContact->LockL( *this );
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );

    // CommitL succeed
    iStoreContact->CommitL( *this );
    
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    
    delete iStoreContact;
    iStoreContact = NULL;

    // ReadAndLockL
    const MVPbkViewContact& viewCnt0 = iContactView->ContactAtL( 0 );
    viewCnt0.ReadAndLockL( *this );
    
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );
    EUNIT_ASSERT( iStoreContact->IsSame( viewCnt0 ) );
    EUNIT_ASSERT( &iStoreContact->ParentStore() == iContactStore );
    
    TInt fldCount( iStoreContact->Fields().FieldCount() );
    TVPbkFieldTypeMapping typeMapping = CreateFieldTypeMappingL( R_TEL_NO_TYPE );
    const MVPbkFieldType* fieldType = typeMapping.FindMatch( iContactManager->FieldTypes() );
    EUNIT_ASSERT( fieldType );
    fldCount += AddValueToContactFieldL( *iStoreContact, *fieldType, KTelNoValue );
    EUNIT_ASSERT_EQUALS( iStoreContact->Fields().FieldCount(), fldCount );

    TVPbkFieldTypeMapping typeMapping1 = CreateFieldTypeMappingL( R_NON_VERSIT_TYPE );
    const MVPbkFieldType* nonVersitFieldType = 
        typeMapping1.FindMatch( iContactManager->FieldTypes() );
    EUNIT_ASSERT( nonVersitFieldType );

    if ( IsFieldTypeSupported( *nonVersitFieldType ) )
        {
        fldCount += AddValueToContactFieldL( *iStoreContact, *nonVersitFieldType, KTelNoValue );
        EUNIT_ASSERT_EQUALS( iStoreContact->Fields().FieldCount(), fldCount );
        }

    // RemoveField
    iStoreContact->RemoveField( iStoreContact->Fields().FieldCount() - 1 );
    EUNIT_ASSERT_EQUALS( iStoreContact->Fields().FieldCount(), ( fldCount - 1 ) );
    // RemoveAllFields
    iStoreContact->RemoveAllFields();
    EUNIT_ASSERT_EQUALS( iStoreContact->Fields().FieldCount(), 0 );

    delete iStoreContact;
    iStoreContact = NULL;

    viewCnt0.ReadL( *this );
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );
    
    // Group
    EUNIT_ASSERT( iStoreContact->Group() == NULL );

    // DeleteL
    TInt cntCount( iContactView->ContactCountL() );
    iStoreContact->DeleteL( *this );
    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
	iExpectedViewEvent = EContactRemovedFromView;
    StartContactSchedulerAndDecideToLeaveL();
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent);
    EUNIT_ASSERT_EQUALS( iContactView->ContactCountL(), --cntCount );

    const MVPbkViewContact& contact = iContactView->ContactAtL( 0 );
    contact.DeleteL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
	iExpectedViewEvent = EContactRemovedFromView;
    StartContactSchedulerAndDecideToLeaveL();
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent);
    EUNIT_ASSERT_EQUALS( iContactView->ContactCountL(), --cntCount );
      
    // ParentView
    EUNIT_ASSERT( &contact.ParentView() == iContactView );
    }

void T_MVPbkContactStoreSingle::TestDeleteMultipleContactsL()
    {
    DeleteAllContactsL();
    }

void T_MVPbkContactStoreSingle::TestContactFieldOperationsL()
    {
    const MVPbkViewContact& contact = iContactView->ContactAtL( 0 );
    contact.ReadAndLockL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    EUNIT_ASSERT( iStoreContact );
    EUNIT_ASSERT( contact.IsSame( *iStoreContact ) );
    EUNIT_ASSERT( static_cast<const MVPbkBaseContact*>( 
        &contact.Fields().ParentContact() ) == &contact );

    TVPbkFieldTypeMapping typeMapping = CreateFieldTypeMappingL( R_TEL_NO_TYPE );
    const MVPbkFieldType* fieldType = typeMapping.FindMatch(iContactManager->FieldTypes());
    EUNIT_ASSERT( fieldType );
    TInt fieldCount(
    	static_cast<const MVPbkStoreContact&> 
    		(*iStoreContact).Fields().FieldCount());
    fieldCount += AddValueToContactFieldL( *iStoreContact, *fieldType, KTelNoValue );
    EUNIT_ASSERT_EQUALS( iStoreContact->Fields().FieldCount(), fieldCount );

    MVPbkStoreContactField& field = iStoreContact->Fields().FieldAt( 0 );
    if ( field.SupportsLabel() )
        {
        EUNIT_ASSERT_EQUALS( field.FieldLabel().Compare( KFirstNameLabel ), 0 );

        field.SetFieldLabelL( KFirstNameNewLabel() );
        EUNIT_ASSERT_EQUALS( field.FieldLabel().Compare( KFirstNameNewLabel ), 0 );
        }
    
    // Test IsSame for fields 
    // IsSame work only for fields from same contact instance
    const TInt viewContactFieldCount = contact.Fields().FieldCount();
    const TInt storeContactFieldCount = iStoreContact->Fields().FieldCount();
    for ( TInt i = 0; i < viewContactFieldCount; ++i )
        {
        const MVPbkBaseContactField& field = contact.Fields().FieldAt(i);
        for ( TInt j = 0; j < storeContactFieldCount; ++j)
            {
            // Non of the fields must match because view contact and store
            // contact is different instances
            EUNIT_ASSERT( !field.IsSame( iStoreContact->Fields().FieldAt(j)) );
            }
        }
    for ( TInt i = 0; i < storeContactFieldCount; ++i )
        {
        const MVPbkStoreContactField& field = iStoreContact->Fields().FieldAt(i);
        for ( TInt j = 0; j < viewContactFieldCount; ++j)
            {
            // Non of the fields must match because view contact and store
            // contact is different instances
            EUNIT_ASSERT( !field.IsSame( contact.Fields().FieldAt(j)) );
            }
        }
    
    // IsSame is kind of useless for view contacts but test it anyway.
    EUNIT_ASSERT( contact.Fields().FieldAt(0).IsSame( 
        contact.Fields().FieldAt(0) ) );
    EUNIT_ASSERT( iStoreContact->Fields().FieldAt(0).IsSame( 
        iStoreContact->Fields().FieldAt(0) ) );
    MVPbkStoreContactField* copy = iStoreContact->Fields().FieldAtLC(0);
    EUNIT_ASSERT( copy->IsSame( iStoreContact->Fields().FieldAt(0) ) );
    CleanupStack::PopAndDestroy(); // copy
    
    // Test cloning
    MVPbkStoreContactField* clone = field.CloneLC();
    EUNIT_ASSERT( clone != &field );
    EUNIT_ASSERT( clone->IsSame( field ) );
    CleanupStack::PopAndDestroy(); // clone

    iStoreContact->CommitL(*this);

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    }

void T_MVPbkContactStoreSingle::DoTestFindL(
        const TDesC& aString,
        const MVPbkFieldTypeList& aFieldTypes,
        const TInt aExpectedMatchCount )
    {
    MVPbkContactOperationBase* op = iContactManager->FindL(
        aString, 
        aFieldTypes,
        *this );
    CleanupDeletePushL( op );
    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );
    EUNIT_ASSERT( iContactLinks );
    TInt count = iContactLinks->Count();
    EUNIT_ASSERT_EQUALS( count, aExpectedMatchCount );
    CleanupStack::PopAndDestroy(); // op
    }

void T_MVPbkContactStoreSingle::TestFindNoMatchEmptyFieldTypesL()
    {
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );
    EUNIT_ASSERT_EQUALS( types->FieldTypeCount(), 0 );

	DoTestFindL( KLastNameFieldValue, *types, 0 );

    CleanupStack::PopAndDestroy( types );
    }

void T_MVPbkContactStoreSingle::TestFindMatchLastNameL()
    {
    DoTestFindL( KLastNameFieldValue, iContactManager->FieldTypes(),
        KMaxNumOfStoreContacts );
    }

void T_MVPbkContactStoreSingle::TestFindMatchFirstLastNameL()
    {
    _LIT( KFirstLastName, "aLastname" );
    DoTestFindL( KFirstLastName, iContactManager->FieldTypes(), 1 );
    }

void T_MVPbkContactStoreSingle::TestFindNoMatchL()
    {
    _LIT( KNoMatchString, "no match" );
    DoTestFindL( KNoMatchString, iContactManager->FieldTypes(), 0 );
    }

void T_MVPbkContactStoreSingle::TestFind_FailL()
    {
    // Make find before all stores are open, 
    // so before opencompleted has been called.

	// KLastNameFieldValue
	MVPbkContactOperationBase* op = iContactManager->FindL(
                KLastNameFieldValue, 
                iContactManager->FieldTypes(),
                *this);
    CleanupDeletePushL(op);
    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    // When stores are not ready find should complete with empty result
    // array
    EUNIT_ASSERT( iContactLinks );
    EUNIT_ASSERT_EQUALS( iContactLinks->Count(), 0 );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindComplete );  
    CleanupStack::PopAndDestroy(); // op  
    }

// TODO: Clean up (and refactor?) the next four FindLWithParser functions
void T_MVPbkContactStoreSingle::TestFindWithParserEmptyFieldTypesL()
    {
    // Create an array for search strings
    const TInt granularity = 2;
    CDesCArrayFlat* searchStrings = 
        new( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( searchStrings );
    // Create a list for fieldtype references
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );

    // 1) Test with empty field type list -> result should be
    // empty linkarray
	MVPbkContactOperationBase* op = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ) );
    CleanupDeletePushL(op);
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindWithParserComplete );
    EUNIT_ASSERT( iFindResults );
    // The count should be 0, because we gave the empty fieldtype list.
    EUNIT_ASSERT_EQUALS( iFindResults->Count(), 0 )
    CleanupStack::PopAndDestroy(); // op
    delete iFindResults; 
    iFindResults = NULL;
    ResetTestSuiteEvents();

    CleanupStack::PopAndDestroy( 2, searchStrings );
    }

void T_MVPbkContactStoreSingle::TestFindWithParserL()
    {
    // Create an array for search strings
    const TInt granularity = 2;
    CDesCArrayFlat* searchStrings = 
        new( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( searchStrings );
    // Create a list for fieldtype references
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );

    // 2) Test using first, last, and company name as fieldtypes and
    // search strings that should match.
    // Assume that setup and PopulateContactStoreL have inserted contact
    // whose lastname is "aLastName value".
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME ) );
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME ) );
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_COMPANYNAME ) );
    searchStrings->AppendL( _L("a") );
    searchStrings->AppendL( _L("val") );
    MVPbkContactOperationBase* op = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ) );
    CleanupDeletePushL(op);
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindWithParserComplete );
    EUNIT_ASSERT( iFindResults );
    // contact whose last name is "aLastName value" should match
    EUNIT_ASSERT_EQUALS( iFindResults->Count(), 1 )
    CleanupStack::PopAndDestroy(); // op
    delete iFindResults; 
    iFindResults = NULL;
    ResetTestSuiteEvents();
    searchStrings->Reset();

    CleanupStack::PopAndDestroy( 2, searchStrings );
    }

void T_MVPbkContactStoreSingle::TestFindWithParserNoMatchL()
    {
    // Create an array for search strings
    const TInt granularity = 2;
    CDesCArrayFlat* searchStrings = 
        new( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( searchStrings );
    // Create a list for fieldtype references
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );

    // 3) Test using first, last, and company name as fieldtypes and
    // search strings that shouldn't match
    // Assume that setup and PopulateContactStoreL have inserted contact
    // whose lastname is "aLastName value".
    searchStrings->AppendL( _L("aK") );
    searchStrings->AppendL( _L("val") );
    
    MVPbkContactOperationBase* op = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ) );
    CleanupDeletePushL(op);
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindWithParserComplete );
    EUNIT_ASSERT( iFindResults );
    // contact whose last name is "aLastName value" should match
    EUNIT_ASSERT_EQUALS( iFindResults->Count(), 0 )
    CleanupStack::PopAndDestroy(); // op

    CleanupStack::PopAndDestroy( 2, searchStrings );
    }

void T_MVPbkContactStoreSingle::TestFindWithSelectiveStoreSearchL()
    {
    // Create an array for search strings
    const TInt granularity = 2;
    CDesCArrayFlat* searchStrings = 
        new( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( searchStrings );
    
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );

    
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME ) );
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME ) );
    types->AppendL( 
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_COMPANYNAME ) );
    
	searchStrings->AppendL( _L("a") );
    searchStrings->AppendL( _L("val") );
    
    CDesC16ArrayFlat* aUriArray = new ( ELeave ) CDesCArrayFlat( 52 );
	CleanupStack::PushL(aUriArray);
	
	aUriArray->AppendL(iUriToTest->Des());
   	MVPbkContactOperationBase* op = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ), *aUriArray );
    
    CleanupDeletePushL(op);
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindWithParserComplete );
	// should find value
    EUNIT_ASSERT_EQUALS( iFindResults->Count(), 1 );
    
    delete iFindResults;
    iFindResults = NULL;
    
    //Now test in different database with selective search
    CDesC16ArrayFlat* aUriArrayNew = new ( ELeave ) CDesCArrayFlat( 52 );
	CleanupStack::PushL(aUriArrayNew);
    
  	TRAPD( err, iDatabaseNew = CContactDatabase::OpenL(_L("c:testStore.cdb")));
    if( err == KErrNotFound )
        {
        iDatabaseNew = CContactDatabase::CreateL( _L("c:testStore.cdb") );
        }
 
     // Append database in the array for selective search
    aUriArrayNew->AppendL(_L("c:testStore.cdb"));
    
    //Selective Search
    MVPbkContactOperationBase* op1 = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ), *aUriArrayNew );
    
    CleanupDeletePushL(op1);
    StartSchedulerAndDecideToLeaveL();
    CleanupStack::PopAndDestroy();
   	EUNIT_ASSERT_EQUALS( iLastFindError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastFindEvent, EFindWithParserComplete );
    // should not find value
    EUNIT_ASSERT_EQUALS( iFindResults->Count(), 0 );
    
    ResetTestSuiteEvents();
    searchStrings->Reset();
    delete iDatabaseNew ;
    iDatabaseNew = NULL;
    CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy(2);
    
    }

void T_MVPbkContactStoreSingle::TestCancelFindL()
    {
    // Create an array for search strings
    const TInt granularity = 2;
    CDesCArrayFlat* searchStrings = 
        new( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( searchStrings );
    // Create a list for fieldtype references
    CVPbkFieldTypeRefsList* types = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( types );

    // 4) Test canceling
    MVPbkContactOperationBase* op = iContactManager->FindL(
        *searchStrings, *types, *this, TCallBack( &VPbkParseDataL ) );
    delete op;

    CleanupStack::PopAndDestroy( 2, searchStrings );
    }

void T_MVPbkContactStoreSingle::TestCopyContactsL()
	{
	CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
    TInt count( iContactView->ContactCountL() );

	for( TInt i = 0; i < count; ++i )
		{
		linkArray->AppendL( iContactView->CreateLinkLC( i ) );
		CleanupStack::Pop(); // link
		}
		
	// Creates duplicates of contacts, because store param is NULL
	MVPbkContactOperationBase* op = iContactManager->CopyContactsL(
        *linkArray, NULL, *this );
    CleanupDeletePushL( op );
    EUNIT_ASSERT( op );
    ResetTestSuiteEvents();
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastBatchOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastBatchOpEvent, EOperationComplete );
    // Contacts where duplicated to the same store...
    EUNIT_ASSERT_EQUALS( ( count * 2 ), iContactView->ContactCountL() );
    
    CleanupStack::PopAndDestroy(2); // op, linkArray
	}
	
void T_MVPbkContactStoreSingle::TestGroupsL()
	{
	if( iContactStore->StoreProperties().SupportsContactGroups() )
	    {
	    // CreateNewContactGroupLC
        MVPbkContactGroup* group = iContactStore->CreateNewContactGroupLC();
    	EUNIT_ASSERT( group );
    	
    	// GroupLabel & SetGroupLabel
    	EUNIT_ASSERT( group->GroupLabel().Compare( KGroupLabel ) != 0 );
    	group->SetGroupLabelL( KNullDesC() );
    	EUNIT_ASSERT_EQUALS( group->GroupLabel().Compare( KNullDesC ), 0 );
    	
    	group->SetGroupLabelL( KGroupLabel() );
    	EUNIT_ASSERT_EQUALS( group->GroupLabel().Compare( KGroupLabel ), 0 );
    	
    	// AddContactL
    	TInt contactCount( iContactView->ContactCountL() );
    	for( TInt i = 0; i < contactCount; ++i )
    		{
    		group->AddContactL( *iContactView->CreateLinkLC( i ) );
    		CleanupStack::PopAndDestroy(); // link
    		}
    		
    	// ItemsContainedLC
    	EUNIT_ASSERT_EQUALS( group->ItemsContainedLC()->Count(), contactCount );
    	CleanupStack::PopAndDestroy(); // items contained
    	
    	// RemoveContactL
    	if( contactCount > 0 ) // Always should be (> 0)
    		{
    		group->RemoveContactL( *iContactView->CreateLinkLC( 0 ) );
    		CleanupStack::PopAndDestroy(); // link
    		EUNIT_ASSERT_EQUALS(
    		    group->ItemsContainedLC()->Count(), ( contactCount - 1 ) );
    		CleanupStack::PopAndDestroy(); // items contained		
    		}
    		
    	// CommitL
    	group->CommitL( *this );
    	ResetTestSuiteEvents();
    	iExpectedContactOpEvent = EContactOperationCompleted;
        StartContactSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
        CleanupStack::PopAndDestroy(); // group
	    }
	else
	    {
	    EUNIT_ASSERT_SPECIFIC_LEAVE( iContactStore->CreateNewContactGroupLC(),
	        KErrNotSupported );
	    }
	}

void T_MVPbkContactStoreSingle::TestStoreInfoL()
    {
    const MVPbkContactStoreInfo& info = iContactStore->StoreInfo();
    const MVPbkContactStoreProperties& props = 
        iContactStore->StoreProperties();
    
    // Maximum number of contacts.    
    TInt maxNumOfContacts = info.MaxNumberOfContactsL();
    EUNIT_ASSERT( maxNumOfContacts == KVPbkStoreInfoUnlimitedNumber ||
        maxNumOfContacts > 0 );
    
    // Number of contacts
    TInt numOfContacts = info.NumberOfContactsL();
    if( !props.ReadOnly() )
        {
        EUNIT_ASSERT_EQUALS( numOfContacts, 0 );
        const TInt addContacts = 3;
        PopulateContactStoreL( addContacts );
        numOfContacts = info.NumberOfContactsL();
        EUNIT_ASSERT_EQUALS( numOfContacts, addContacts );
        }
    
    // Reserved memory
    EUNIT_ASSERT( info.ReservedMemoryL() >= 0 );
    // Free memory
    EUNIT_ASSERT( info.FreeMemoryL() >= 0 );
    
    // Maximum number of groups
    TInt maxNumberOfGroups = info.MaxNumberOfGroupsL();
    if( props.SupportsContactGroups() )
        {
        EUNIT_ASSERT( maxNumberOfGroups == KVPbkStoreInfoUnlimitedNumber ||
            maxNumberOfGroups > 0 );
        }
    else
        {
        EUNIT_ASSERT_EQUALS( maxNumberOfGroups, 0 ); 
        }
    
    // Number of groups
    TInt numberOfGroups = info.NumberOfGroupsL();
    EUNIT_ASSERT_EQUALS( numberOfGroups, 0 );
    if( props.SupportsContactGroups() )
        {
        MVPbkContactGroup* group = iContactStore->CreateNewContactGroupLC();
    	group->SetGroupLabelL( KGroupLabel() );
    	
    	// AddContactL
    	TInt contactCount( iContactView->ContactCountL() );
    	for( TInt i = 0; i < contactCount; ++i )
    		{
    		group->AddContactL( *iContactView->CreateLinkLC( i ) );
    		CleanupStack::PopAndDestroy(); // link
    		}
    		
    	// CommitL
    	group->CommitL( *this );
    	ResetTestSuiteEvents();
    	iExpectedContactOpEvent = EContactOperationCompleted;
        StartContactSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
        CleanupStack::PopAndDestroy(); // group

        ++numberOfGroups;
        EUNIT_ASSERT_EQUALS( info.NumberOfGroupsL(), numberOfGroups );
        // This is for contact model. Test that adding a group didn't
        // affect on number of contacts
        TInt numOfContactsAfterGroup = info.NumberOfContactsL(); 
        EUNIT_ASSERT_EQUALS( numOfContacts, numOfContactsAfterGroup );
        }
    }

void T_MVPbkContactStoreSingle::TestViewContactExpansionL()
    {
    const MVPbkViewContact& viewContact = iContactView->ContactAtL( 0 );

    MVPbkExpandable* expandable = viewContact.Expandable();

    // Expect the expandable to be NULL as the view is not a group
    EUNIT_ASSERT( !expandable );
    }

void T_MVPbkContactStoreSingle::TestIsSameViewContactWithContactStoreL()
    {
    DoTestIsSameViewContactWithTwoContactViewsL(
        0, 0, ETrue, ETrue, iContactStore );
    }

void T_MVPbkContactStoreSingle::TestIsSameViewContactWithNullStoreL()
    {
    DoTestIsSameViewContactWithTwoContactViewsL(
        0, 0, EFalse, EFalse, NULL );
    }

void T_MVPbkContactStoreSingle::TestIsNotSameViewContactWithContactStoreL()
    {
    DoTestIsSameViewContactWithTwoContactViewsL(
        0, 1, EFalse, EFalse, iContactStore );
    }

void T_MVPbkContactStoreSingle::TestIsNotSameViewContactWithNullStoreL()
    {
    DoTestIsSameViewContactWithTwoContactViewsL(
        0, 1, EFalse, EFalse, NULL );
    }

void T_MVPbkContactStoreSingle::DoTestIsSameViewContactWithTwoContactViewsL(
        const TInt aFirstContactIndex,
        const TInt aSecondContactIndex,
        const TBool aExpectedResultFirstComparison,
        const TBool aExpectedResultSecondComparison,
        MVPbkContactStore* aContactStore )
    {
    EUNIT_ASSERT_EQUALS_DESC(
        iContactView->ContactCountL(),
        iContactView2->ContactCountL(),
        "Setup did not initialize the two views to the same contact count" );

    const MVPbkViewContact& viewContact1 =
        iContactView->ContactAtL( aFirstContactIndex );
    const MVPbkViewContact& viewContact2 =
        iContactView2->ContactAtL( aSecondContactIndex );

    EUNIT_ASSERT_EQUALS(
        viewContact1.IsSame( viewContact2, aContactStore ),
        aExpectedResultFirstComparison );
    EUNIT_ASSERT_EQUALS(
        viewContact2.IsSame( viewContact1, aContactStore ),
        aExpectedResultSecondComparison );
    }

void T_MVPbkContactStoreSingle::TestIsSameViewContactL()
    {
    // TODO: Refactor with the above
    EUNIT_ASSERT_EQUALS_DESC(
        iContactView->ContactCountL(),
        iContactView2->ContactCountL(),
        "Setup did not initialize the two views to the same contact count" );

    const MVPbkViewContact& viewContact1 = iContactView->ContactAtL( 0 );
    const MVPbkViewContact& viewContact2 = iContactView2->ContactAtL( 0 );

    EUNIT_ASSERT( viewContact1.IsSame( viewContact2 ) );
    EUNIT_ASSERT( viewContact2.IsSame( viewContact1 ) );
    }

void T_MVPbkContactStoreSingle::TestIsSameStoreContactWithContactStoreL()
    {
    DoTestIsSameStoreContactWithContactStoreL( 0, iContactStore, ETrue );
    }

void T_MVPbkContactStoreSingle::TestIsNotSameStoreContactWithContactStoreL()
    {
    DoTestIsSameStoreContactWithContactStoreL( 0, NULL, EFalse );
    }

void T_MVPbkContactStoreSingle::DoTestIsSameStoreContactWithContactStoreL(
        const TInt aViewContactIndex,
        MVPbkContactStore* aContactStore,
        TBool aExpectedResult )
    {
    const MVPbkViewContact& viewContact =
        iContactView->ContactAtL( aViewContactIndex );
    viewContact.ReadL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS_DESC( iLastContactOpError, KErrNone,
        "Error reading a contact" );
    EUNIT_ASSERT_EQUALS_DESC( iLastContactOpEvent, iExpectedContactOpEvent,
        "Unexpected contact operation event while reading a contact" );
    EUNIT_ASSERT( iStoreContact );
    // TODO: Is this necessary? If yes, modify to suit here. (Jarno)
    //EUNIT_ASSERT( iStoreContact->IsSame( viewContact ) );
    EUNIT_ASSERT_EQUALS(
        viewContact.IsSame( *iStoreContact, aContactStore ),
        aExpectedResult );
    }

// TODO: This test goes to test CContact::IsSame(), why?
void T_MVPbkContactStoreSingle::TestIsSameStoreContactL()
    {
    DoTestIsSameStoreContactL( 0, ETrue );
    }

// TODO: This does not currently have any active tests
void T_MVPbkContactStoreSingle::TestIsNotSameStoreContactL()
    {
    // TODO: Is it even possible to test this? (Jarno)
    DoTestIsSameStoreContactL( 0, EFalse );
    }

void T_MVPbkContactStoreSingle::DoTestIsSameStoreContactL(
        const TInt aViewContactIndex,
        TBool aExpectedResult )
    {
    const MVPbkViewContact& viewContact =
        iContactView->ContactAtL( aViewContactIndex );
    viewContact.ReadL( *this );

    ResetTestSuiteEvents();
	iExpectedContactOpEvent = EContactOperationCompleted;
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS_DESC( iLastContactOpError, KErrNone,
        "Error reading a contact" );
    EUNIT_ASSERT_EQUALS_DESC( iLastContactOpEvent, iExpectedContactOpEvent,
        "Unexpected contact operation event while reading a contact" );
    EUNIT_ASSERT( iStoreContact );
    // TODO: Is this necessary? If yes, modify to suit here. (Jarno)
    //EUNIT_ASSERT( iStoreContact->IsSame( viewContact ) );
    EUNIT_ASSERT_EQUALS(
        viewContact.IsSame( *iStoreContact ),
        aExpectedResult );
    }

void T_MVPbkContactStoreSingle::TestMatchViewContactStoreL()
    {
    const MVPbkViewContact& viewContact = iContactView->ContactAtL( 0 );
    EUNIT_ASSERT( viewContact.MatchContactStore( *iUriToTest ) );
    }

void T_MVPbkContactStoreSingle::TestFailMatchViewContactStoreL()
    {
    const MVPbkViewContact& viewContact = iContactView->ContactAtL( 0 );
    
    _LIT( KInvalidStoreUri, "cntdb://c:foo.cdb");
    EUNIT_ASSERT( !viewContact.MatchContactStore( KInvalidStoreUri ) );
    }

void T_MVPbkContactStoreSingle::TestMatchViewContactStoreDomainL()
    {
    DoTestMatchViewContactStoreDomainL( 0, *iUriToTest, ETrue );
    }

void T_MVPbkContactStoreSingle::TestFailMatchViewContactStoreDomainL()
    {
    _LIT( KInvalidStoreDomainUri, "averell://c:foo.cdb");
    DoTestMatchViewContactStoreDomainL( 0, KInvalidStoreDomainUri, EFalse );
    }

void T_MVPbkContactStoreSingle::DoTestMatchViewContactStoreDomainL(
        const TInt aViewContactIndex,
        const TDesC& aStoreUri,
        TBool aExpectedResult )
    {
    const MVPbkViewContact& viewContact =
        iContactView->ContactAtL( aViewContactIndex );

    TVPbkContactStoreUriPtr uriPtr( aStoreUri );
    const TPtrC storeDomain = uriPtr.Component(
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType );

    EUNIT_ASSERT_EQUALS(
        viewContact.MatchContactStoreDomain( storeDomain ), aExpectedResult )
    }

void T_MVPbkContactStoreSingle::TestCreateViewContactBookmarkL()
    {
    const TInt contactIndex = 0;
    const MVPbkViewContact& viewContact = 
        iContactView->ContactAtL( contactIndex );

    MVPbkContactBookmark* bookmark = viewContact.CreateBookmarkLC();
    EUNIT_ASSERT_DESC( bookmark, "Creating the bookmark failed" );
    EUNIT_ASSERT_EQUALS_DESC(
        iContactView->IndexOfBookmarkL( *bookmark ), contactIndex,
        "The bookmark and the view contact indexes were not the same" );
    CleanupStack::PopAndDestroy(); // bookmark
    }

void T_MVPbkContactStoreSingle::TestCreateFilteredViewL()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
    
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );    
 
    CleanupStack::PopAndDestroy( 1 ); // findView
    }
    
void T_MVPbkContactStoreSingle::TestCreateFilteredView2L()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Filtered view must support further filtering
    EUNIT_ASSERT( findView->ViewFiltering() );
    
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent ); 
    
    CleanupStack::PopAndDestroy( 2 ); // findView, refine
    }    
    
void T_MVPbkContactStoreSingle::TestCreateFilteredView3L()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    MVPbkContactViewBase* findView2 = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    CleanupStack::PopAndDestroy( 2 ); // findView
    }
    
void T_MVPbkContactStoreSingle::TestCreateFilteredView4L()
    {
    RContactBookmarkCollection bookmarks;
    // Save a bookmark for one contact
    MVPbkContactBookmark* mark = iContactView->ContactAtL(
        KMaxNumOfStoreContacts - 1).CreateBookmarkLC();
    bookmarks.AppendL( mark );
    CleanupStack::Pop();
    CleanupClosePushL( bookmarks );
    
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, &bookmarks );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent ); 
    TInt contactCount = findView->ContactCountL();
    // Must be at least 1 contact because we set always included contact.
    EUNIT_ASSERT( contactCount > 0 );
    for ( TInt i = 0; i < contactCount; ++i )
        {
        EUNIT_ASSERT( findView->IndexOfBookmarkL( *mark ) != KErrNotFound );
        }
        
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Filtered view must support further filtering
    EUNIT_ASSERT( findView->ViewFiltering() );
    
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, &bookmarks );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent ); 
    
    contactCount = refineView->ContactCountL();
    // Must be at least 1 contact because we set always included contact.
    EUNIT_ASSERT( contactCount > 0 );
    for ( TInt i = 0; i < contactCount; ++i )
        {
        EUNIT_ASSERT( refineView->IndexOfBookmarkL( *mark ) != KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( 3, &bookmarks); // findView, refine
    }
    
void T_MVPbkContactStoreSingle::TestDeleteContactsFromFilteredViewL()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Filtered view must support further filtering
    EUNIT_ASSERT( findView->ViewFiltering() );
    
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    TInt before( refineView->ContactCountL() );    
    TestDeleteMultipleContactsL();
    TInt after(refineView->ContactCountL());
    EUNIT_ASSERT( before >= after );
    
    CleanupStack::PopAndDestroy( 2 ); // findView, refine
    }
    
void T_MVPbkContactStoreSingle::TestCreateContactAfterFilteringL()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Filtered view must support further filtering
    EUNIT_ASSERT( findView->ViewFiltering() );
    
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();

    TInt before( refineView->ContactCountL() );
    PopulateContactStoreL( 20, ETrue );
    TInt after( refineView->ContactCountL() );
    EUNIT_ASSERT( before <= after );    
    
    // Free contacts database from test contacts
    DeleteAllContactsL();
    CleanupStack::PopAndDestroy( 2 ); // findView, refine    
    }
    
void T_MVPbkContactStoreSingle::TestFilteredViewL()
    {
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    
    if ( findView->ContactCountL() > 0 )
        {
        const MVPbkViewContact* contact = &findView->ContactAtL( 0 );
        EUNIT_ASSERT( contact );
        
        MVPbkContactLink* link = findView->CreateLinkLC( 0 );
        EUNIT_ASSERT( link );        
        TInt indexOfLink( findView->IndexOfLinkL( *link ) );
        EUNIT_ASSERT( indexOfLink == 0 );
        CleanupStack::PopAndDestroy(); //link
        
        findView->AddObserverL( *this );
        findView->RemoveObserver( *this );
        findView->AddObserverL( *this );
        
        MVPbkContactBookmark* bookmark = findView->CreateBookmarkLC( 0 );
        EUNIT_ASSERT( bookmark );
        TInt indexOfBookmark( findView->IndexOfBookmarkL( *bookmark ) );
        EUNIT_ASSERT( indexOfBookmark == 0 );
        CleanupStack::PopAndDestroy();
        }
    
    
    CleanupStack::PopAndDestroy( 1 ); // findView
    }

void T_MVPbkContactStoreSingle::TestUpdateFilterL()
    {
    RContactBookmarkCollection bookmarks;
    // Save a bookmark for one contact
    MVPbkContactBookmark* mark = iContactView->ContactAtL(
        KMaxNumOfStoreContacts - 1).CreateBookmarkLC();
    bookmarks.AppendL( mark );
    CleanupStack::Pop();
    CleanupClosePushL( bookmarks );
    
    // First assert that the view supports filtering
    EUNIT_ASSERT( iContactView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iContactView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, &bookmarks );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent ); 
    TInt contactCount = findView->ContactCountL();
    // Must be at least 1 contact because we set always included contact.
    EUNIT_ASSERT( contactCount > 0 );
    EUNIT_ASSERT( findView->IndexOfBookmarkL( *mark ) != KErrNotFound );
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, &bookmarks );
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent ); 
    
    contactCount = refineView->ContactCountL();
    // Must be at least 1 contact because we set always included contact.
    EUNIT_ASSERT( contactCount > 0 );
    EUNIT_ASSERT( refineView->IndexOfBookmarkL( *mark ) != KErrNotFound );
    
    // Test updating, Update must be called for all views in the stack.
    // Filtered view must support further filtering
    EUNIT_ASSERT( findView->ViewFiltering() );
    EUNIT_ASSERT( refineView->ViewFiltering() );
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindString );
    findView->ViewFiltering()->UpdateFilterL( *iFindStrings, NULL );
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    refineView->ViewFiltering()->UpdateFilterL( *iFindStrings, NULL );
    
    /// Wait findView and refineView to update itself
    iExpectedViewEvent = EContactViewReady;
    iExpectedViewEventCount = 2;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    // KFindString is "a" and there is only one contact whose name begins
    // with according to setup "aLastname value".
    EUNIT_ASSERT( findView->ContactCountL() == 1 );
    // KFindString is "al" and there is only one contact whose name begins
    // with according to setup "aLastname value".
    EUNIT_ASSERT( refineView->ContactCountL() == 1 );
    
    CleanupStack::PopAndDestroy( 3, &bookmarks); // findView, refine
    }
    
// HELPER FUNCTIONS

void T_MVPbkContactStoreSingle::ReplaceStoreL()
    {
    iStoreList = &iContactManager->ContactStoresL();
    // Create new one.
    iContactStore = &iStoreList->At( KFirstIndex );
    iContactStore->ReplaceL( *this );
    // Flag to notice that we dont wait OpenComplete event
    // but StoreReady or StoreError event
    iNoOpenCompleteFlag = ETrue;
    ResetTestSuiteEvents();
	iExpectedStoreEvent = EStoreReady;
    StartStoreSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastStoreError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastStoreEvent, iExpectedStoreEvent );
    }

void T_MVPbkContactStoreSingle::CreateContactViewL(
        MVPbkContactView*& aContactView, 
	    TInt aSortOrderResId,
	    TInt aViewDefResId )
    {
    CVPbkContactViewDefinition* viewDef = CreateViewDefLC( aViewDefResId );
    CVPbkSortOrder* sortOrder = CreateSortOrderLC( aSortOrderResId );
    aContactView = iContactStore->CreateViewLC( *viewDef, *this, *sortOrder );
    CleanupStack::Pop(); // aContactView;
    CleanupStack::PopAndDestroy( 2 ); // sortOrder, viewDef
    
    ResetTestSuiteEvents();
    iExpectedViewEvent = EContactViewReady;
    StartViewSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
    }

CVPbkContactViewDefinition* T_MVPbkContactStoreSingle::CreateViewDefLC(
        TInt aResId )
	{
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iFs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( iFs, path );
    
    TResourceReader reader;
    HBufC8* buffer = resFile.AllocReadLC( aResId );
    reader.SetBuffer( buffer );

    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewL( reader );
    CleanupStack::PopAndDestroy( 2 ); //  buffer, resFile    
    CleanupStack::PushL( viewDef );
    return viewDef;	
	}

void T_MVPbkContactStoreSingle::PopulateContactStoreL(
        const TInt aAmount,
        const TBool aBeginWithIdentifier )
    {
    const TInt alphabetCount = KAlphabet().Length();
    
    for ( TInt i = 0; i < aAmount; ++i )
        {
        TInt remainder = i % alphabetCount;
        HBufC* firstName = HBufC::NewLC( KFirstNameFieldValue().Length() + 1 );
        TPtr ptr( firstName->Des() );
        if( aBeginWithIdentifier )
            {
            ptr.Append( KAlphabet()[ remainder ] );
            ptr.Append( KFirstNameFieldValue );
            }
        else
            {
            ptr.Append( KFirstNameFieldValue );
            ptr.Append( KAlphabet()[ remainder ] );
            }
        
        HBufC* lastName = HBufC::NewLC( KLastNameFieldValue().Length() + 1 );
        ptr.Set( lastName->Des() );
        if( aBeginWithIdentifier )
            {
            ptr.Append( KAlphabet()[ remainder ] );
            ptr.Append( KLastNameFieldValue );
            }
        else
            {
            ptr.Append( KLastNameFieldValue );
            ptr.Append( KAlphabet()[ remainder ] );
            }
        MVPbkStoreContact* storeContact = iContactStore->CreateNewContactLC();
        AddContactFieldsAndSaveL( *storeContact, *firstName, *lastName );
        CleanupStack::PopAndDestroy( 3 ); // firstName, lastName, storeContact
        }
    }

TInt T_MVPbkContactStoreSingle::AddContactFieldsAndSaveL( 
        MVPbkStoreContact& aStoreContact,
        const TDesC& aFName,
        const TDesC& aLName )
    {
    TInt addedFieldCount( 0 );
    // Create fieldtype FIRSTNAME
    TVPbkFieldTypeMapping firstNameFieldType = CreateFieldTypeMappingL( R_FIRST_NAME_TYPE );
    const MVPbkFieldType* fieldType = 
        firstNameFieldType.FindMatch( iContactManager->FieldTypes() );
    EUNIT_ASSERT( fieldType );
    // Add value to the field
    addedFieldCount += AddValueToContactFieldL( aStoreContact, *fieldType, aFName );

    // Create fieldtype LASTNAME
    TVPbkFieldTypeMapping lastNameFieldType = CreateFieldTypeMappingL( R_LAST_NAME_TYPE );
    const MVPbkFieldType* lfieldType = 
        lastNameFieldType.FindMatch( iContactManager->FieldTypes() );
    EUNIT_ASSERT( lfieldType );
    // Add value to the field
    addedFieldCount += AddValueToContactFieldL( aStoreContact, *lfieldType, aLName );
    
    // Save modifications to the storecontact
    aStoreContact.CommitL( *this );  
    ResetTestSuiteEvents();
    iExpectedContactOpEvent = EContactOperationCompleted;    
    if( iContactView )
        {
        iExpectedViewEvent = EContactAddedToView;
        StartViewSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastViewError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastViewEvent, iExpectedViewEvent );
        }
    StartContactSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT_EQUALS( iLastContactOpError, KErrNone );
    EUNIT_ASSERT_EQUALS( iLastContactOpEvent, iExpectedContactOpEvent );
    
    return addedFieldCount;
    }

TInt T_MVPbkContactStoreSingle::AddValueToContactFieldL(
        MVPbkStoreContact& aStoreContact, 
        const MVPbkFieldType& aFieldType,
        const TDesC& aValue )
    {
    TInt err( KErrNone );
    TInt fldCount( aStoreContact.Fields().FieldCount() );
    
    MVPbkStoreContactField* field = NULL; 
    TRAP( err, field = CreateFieldTypeL( aStoreContact, aFieldType ) );
    if ( err == KErrNotSupported )
        {
        // Does not support fieldtype
        // Return 0 to indicate that the field was not added.
        return 0;
        }
    User::LeaveIfError( err );
    CleanupDeletePushL( field );
    
    EUNIT_ASSERT( field->FieldData().DataType() == EVPbkFieldStorageTypeText );
    MVPbkContactFieldTextData::Cast( field->FieldData() ).SetTextL( aValue );
    aStoreContact.AddFieldL( field );
    CleanupStack::Pop(); // field
    EUNIT_ASSERT( aStoreContact.Fields().FieldCount() == ++fldCount );
    return 1; // Return 1 to indicate that the field was added succesfully
    }

TVPbkFieldTypeMapping T_MVPbkContactStoreSingle::CreateFieldTypeMappingL( const TInt aResId )
    {
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iFs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( iFs, path );
    
    TResourceReader reader;
    HBufC8* buffer = resFile.AllocReadLC( aResId );
    reader.SetBuffer(buffer);

    TVPbkFieldTypeMapping result(reader);
    CleanupStack::PopAndDestroy( 2 ); // buffer, resFile

    return result;
    }

void T_MVPbkContactStoreSingle::DeleteAllContactsL()
    {
    const MVPbkContactStoreProperties& props = iContactStore->StoreProperties();
    TInt cntCount( iContactView->ContactCountL() );
    if ( !props.ReadOnly() && cntCount > 0 )
        {
        CVPbkContactLinkArray* contactLnks = CVPbkContactLinkArray::NewLC();
        for ( TInt i = 0; i < cntCount; ++i )
            {
            MVPbkContactLink* lnk = 
                iContactView->ContactAtL( i ).CreateLinkLC();
            contactLnks->AppendL( lnk );
            CleanupStack::Pop(); // lnk
            }

		EUNIT_ASSERT(contactLnks->Count() == cntCount);
        // Delete all contacts
        MVPbkContactOperationBase* op = 
            iContactManager->DeleteContactsL( *contactLnks, *this );
        CleanupDeletePushL( op );
        EUNIT_ASSERT( op );

        iStepCounter = 0;
        ResetTestSuiteEvents();
        StartSchedulerAndDecideToLeaveL();
        EUNIT_ASSERT_EQUALS( iLastBatchOpError, KErrNone );
        EUNIT_ASSERT_EQUALS( iLastBatchOpEvent, EOperationComplete );
        EUNIT_ASSERT( iStepCounter == cntCount )
        EUNIT_ASSERT( iContactView->ContactCountL() == 0 );

        CleanupStack::PopAndDestroy(); // op
        CleanupStack::PopAndDestroy(contactLnks);
        }
    }

TBool T_MVPbkContactStoreSingle::IsFieldTypeSupported( 
    const MVPbkFieldType& aFieldType )
    {
    const MVPbkFieldTypeList& supportedFields = 
        iContactStore->StoreProperties().SupportedFields();
    return supportedFields.ContainsSame( aFieldType );
    }

CVPbkSortOrder* T_MVPbkContactStoreSingle::CreateSortOrderLC( TInt aResId )
    {
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iFs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( iFs, path );
    TResourceReader reader;
    HBufC8* buffer = resFile.AllocReadLC( aResId );
    reader.SetBuffer(buffer);

    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( reader, iContactManager->FieldTypes() );
    CleanupStack::PopAndDestroy( 2 ); // buffer, resFile
    CleanupStack::PushL( sortOrder );
    return sortOrder;
    }

void T_MVPbkContactStoreSingle::StartSchedulerAndDecideToLeaveL()
    {
    CActiveScheduler::Start();
    if ( iLastSingleContactOpError == KErrNoMemory ||
         iLastBatchOpError == KErrNoMemory ||
         iLastFindError == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }
    }

void T_MVPbkContactStoreSingle::StartViewSchedulerAndDecideToLeaveL()
    {
    while (!iExpectedViewResponseFound)
    	{	
	    iViewScheduler.Start();
	    if ( iLastViewError == KErrNoMemory )
	        {
	        User::Leave( KErrNoMemory );
	        }
    	}
    iExpectedViewResponseFound = EFalse;
    }
    
void T_MVPbkContactStoreSingle::StartStoreSchedulerAndDecideToLeaveL()
	{
    while (!iExpectedStoreResponseFound)
    	{	
	    iStoreScheduler.Start();
	    if ( iLastStoreError == KErrNoMemory )
	        {
	        User::Leave( KErrNoMemory );
	        }
    	}	
    iExpectedStoreResponseFound = EFalse;
	}
	
void T_MVPbkContactStoreSingle::StartContactSchedulerAndDecideToLeaveL()
	{
    while (!iExpectedContactOpResponseFound)
    	{	
	    iContactScheduler.Start();
	    if ( iLastContactOpError == KErrNoMemory )
	        {
	        User::Leave( KErrNoMemory );
	        }
    	}	
    iExpectedContactOpResponseFound = EFalse;
	}

void T_MVPbkContactStoreSingle::ResetTestSuiteEvents()
    {
    // Last events
    iLastViewEvent = ENullEvent;
    iLastContactOpEvent = ENullEvent;
    iLastStoreEvent = ENullEvent;
    iLastSingleContactOpEvent = ENullEvent;
    iLastBatchOpEvent = ENullEvent;
    iLastFindEvent = ENullEvent;
    
    // Last errors
    iLastViewError = KErrNone;
    iLastContactOpError = KErrNone;
    iLastStoreError = KErrNone;
    iLastSingleContactOpError = KErrNone;
    iLastBatchOpError = KErrNone;
    iLastFindError = KErrNone;
    
    // expected events
    iExpectedViewEvent = ENullEvent;
    iExpectedStoreEvent = ENullEvent;
    iExpectedContactOpEvent = ENullEvent;
    
    // expected event found
    iExpectedViewResponseFound = EFalse;
    iExpectedStoreResponseFound = EFalse;
    iExpectedContactOpResponseFound = EFalse;
    
    // scheduler is stopped after 1 iExpectedViewEvent. Test function
    // can change the value
    iExpectedViewEventCount = 1;
    iCurViewEventCount = 0;
    }
    
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    T_MVPbkContactStoreSingle,
    "VPbkApiTester::MVPbkContactStoreSingle",
    "MODULE" )

EUNIT_TEST(
    "Get store list from contact manager",
    "CVPbkContactManager",
    "ContactStoresL",
    "FUNCTIONALITY",
    BaseSetupL, TestGetStoreListL, Teardown )

EUNIT_TEST(
    "Replace contact store",
    "MVPbkContactStore",
    "ReplaceL",
    "FUNCTIONALITY",
    BaseSetupL, TestReplaceStoreL, Teardown )

EUNIT_TEST(
    "Open/close store",
    "MVPbkContactStoreList",
    "OpenAllL/CloseAll",
    "FUNCTIONALITY",
    BaseSetupL, TestOpenStoreL, Teardown )

EUNIT_TEST(
    "Create shared view",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateContactViewL, Teardown )

EUNIT_TEST(
    "Create local view",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateLocalContactViewL, Teardown )

EUNIT_TEST(
    "Create unsorted view",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateUnsortedContactViewL, Teardown )

EUNIT_TEST(
    "Create shared group view",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateSharedGroupViewL, Teardown )

EUNIT_TEST(
    "Create local group view",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateLocalGroupViewL, Teardown )

EUNIT_TEST(
    "Create view with selector",
    "CVPbkContactManager",
    "CreateContactViewLC",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateContactViewWithSelectorL, Teardown )

EUNIT_TEST(
    "Create view with selector",
    "VPbkContactViewFilterBuilder",
    "BuildContactViewFilterL",
    "FUNCTIONALITY",
    SetupStoreL, TestCreateContactViewWithSelector2L, Teardown )

EUNIT_TEST(
    "Create contacts",
    "MVPbkContactStore",
    "CreateContactLC",
    "FUNCTIONALITY",
    SetupViewL, TestCreateContactL, Teardown )

EUNIT_TEST(
    "Change contact view sort order",
    "MVPbkContactView",
    "ChangeSortOrderL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestChangeContactViewSortOrderL, Teardown )

EUNIT_TEST(
    "Refresh contact view",
    "MVPbkContactView",
    "RefreshL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestRefreshContactViewL, Teardown )

EUNIT_TEST(
    "Create contact view link",
    "MVPbkContactView",
    "CreateLinkLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCreateContactViewLinkL, Teardown )

EUNIT_TEST(
    "Create contact link from not commited contact",
    "MVPbkContactStore",
    "CreateLinkLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCreateContactLinkNotCommitedL, Teardown )

EUNIT_TEST(
    "Link refers to contact",
    "CContactLink",
    "RefersTo",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestLinkRefersToL, Teardown )

EUNIT_TEST(
    "Create contact view bookmark",
    "MVPbkContactView",
    "",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCreateContactViewBookmarkL, Teardown )

EUNIT_TEST(
    "Add contact view observer",
    "MVPbkContactView",
    "CreateBookmarkLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestAddContactViewObserverL, Teardown )

EUNIT_TEST(
    "Add contact view observer twice",
    "MVPbkContactView",
    "AddObserverL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestAddContactViewObserverTwiceL, Teardown )

EUNIT_TEST(
    "Remove contact view observer",
    "MVPbkContactView",
    "RemoveObserver",
    "FUNCTIONALITY",
    SetupStoreWithContactsAndObserverL, TestRemoveContactViewObserverL, Teardown )

EUNIT_TEST(
    "Remove nonexistent contact view observer",
    "MVPbkContactView",
    "RemoveObserver",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestRemoveNonexistentContactViewObserverL, Teardown )

EUNIT_TEST(
    "Commit multiple contacts",
    "MVPbkContactManager",
    "CommitContactsL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCommitMultipleContactsL, Teardown )

EUNIT_TEST(
    "Create contact retriever",
    "CVPbkContactManager",
    "RetrieveContactL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestContactRetrieverL, Teardown )

EUNIT_TEST(
    "Contact operations (read/edit/delete)",
    "MVPbkViewContact",
    "ContactAtL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestContactOperationsL, Teardown )

EUNIT_TEST(
    "Delete multiple contacts",
    "MVPbkContactManager",
    "DeleteContactsL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestDeleteMultipleContactsL, Teardown )

EUNIT_TEST(
    "Delete multiple contacts(large db)",
    "MVPbkContactManager",
    "DeleteContactsL",
    "FUNCTIONALITY",
    SetupBigStoreL, TestDeleteMultipleContactsL, Teardown )

EUNIT_TEST(
    "Match full phone number from store",
    "MVPbkContactManager",
    "MatchPhoneNumberL",
    "FUNCTIONALITY",
    SetupDeleteContactsForMatchL, TestMatchFullPhoneNumberL, Teardown )

EUNIT_TEST(
    "Match partial phone number from store",
    "MVPbkContactManager",
    "MatchPhoneNumberL",
    "FUNCTIONALITY",
    SetupDeleteContactsForMatchL, TestMatchPartialPhoneNumberL, Teardown )

EUNIT_TEST(
    "No match phone number from store",
    "MVPbkContactManager",
    "MatchPhoneNumberL",
    "FUNCTIONALITY",
    SetupDeleteContactsForMatchL, TestNoMatchPhoneNumberL, Teardown )

EUNIT_TEST(
    "Fail-Match phone number from store",
    "MVPbkContactManager",
    "MatchPhoneNumberL",
    "FUNCTIONALITY",
    SetupStoreForFindFailL, TestMatchPhoneNumber_FailL, Teardown )    

EUNIT_TEST(
    "Storecontact field operations",
    "MVPbkStoreContactField",
    "CloneLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestContactFieldOperationsL, Teardown )

EUNIT_TEST(
    "Find no match (empty field types)",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindNoMatchEmptyFieldTypesL, Teardown )

EUNIT_TEST(
    "Find last name matches",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindWithIdentifierLastL, TestFindMatchLastNameL, Teardown )

EUNIT_TEST(
    "Find first last name match",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindMatchFirstLastNameL, Teardown )

EUNIT_TEST(
    "Find no match",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindNoMatchL, Teardown )

EUNIT_TEST(
    "FindL fail cases",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindFailL, TestFind_FailL, Teardown )

EUNIT_TEST(
    "FindL with parser (empty field types)",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindWithParserEmptyFieldTypesL, Teardown )

EUNIT_TEST(
    "FindL with parser",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindWithParserL, Teardown )

EUNIT_TEST(
    "FindL with parser (no match)",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestFindWithParserNoMatchL, Teardown )
    
EUNIT_TEST(
    "FindL with parser using Selective Search",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoresForSelectiveFindL, TestFindWithSelectiveStoreSearchL, Teardown )

EUNIT_TEST(
    "Cancel find",
    "CVPbkContactManager",
    "FindL",
    "FUNCTIONALITY",
    SetupStoreForFindL, TestCancelFindL, Teardown )

EUNIT_TEST(
    "Copy contacts to store",
    "CVPbkContactManager",
    "CopyContactsL",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCopyContactsL, Teardown )
    
EUNIT_TEST(
    "Group contact operations",
    "MVPbkContactStore",
    "CreateNewContactGroupLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestGroupsL, Teardown )

EUNIT_TEST(
    "Store info",
    "MVPbkContactStore",
    "StoreInfo",
    "FUNCTIONALITY",
    SetupDeleteContactsL, TestStoreInfoL, Teardown )

// TODO: Another test for view groups (or group views?) where it is not NULL
EUNIT_TEST(
    "Nonexistent view contact expansion",
    "MVPbkViewContact",
    "Expandable",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestViewContactExpansionL, Teardown )

EUNIT_TEST(
    "The same view contacts with contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkViewContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreAndTwoViewsWithContactsL, TestIsSameViewContactWithContactStoreL, Teardown )

EUNIT_TEST(
    "The same view contacts with null contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkViewContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreAndTwoViewsWithContactsL, TestIsSameViewContactWithNullStoreL, Teardown )

EUNIT_TEST(
    "Not the same view contacts with contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkViewContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreAndTwoViewsWithContactsL, TestIsNotSameViewContactWithContactStoreL, Teardown )

EUNIT_TEST(
    "Not the same view contacts with null contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkViewContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreAndTwoViewsWithContactsL, TestIsNotSameViewContactWithNullStoreL, Teardown )

EUNIT_TEST(
    "The same view contacts",
    "MVPbkViewContact",
    "IsSame(MVPbkViewContact)",
    "FUNCTIONALITY",
    SetupStoreAndTwoViewsWithContactsL, TestIsSameViewContactL, Teardown )

EUNIT_TEST(
    "The same view and store contact with contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkStoreContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestIsSameStoreContactWithContactStoreL, Teardown )

EUNIT_TEST(
    "Not the same view and store contact with contact store",
    "MVPbkViewContact",
    "IsSame(MVPbkStoreContact, MVPbkContactStore)",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestIsNotSameStoreContactWithContactStoreL, Teardown )

EUNIT_TEST(
    "The same view and store contact",
    "MVPbkViewContact",
    "IsSame(MVPbkStoreContact)",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestIsSameStoreContactL, Teardown )

EUNIT_TEST(
    "Match view contact store",
    "MVPbkViewContact",
    "MatchContactStore",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestMatchViewContactStoreL, Teardown )

EUNIT_TEST(
    "Fail match view contact store",
    "MVPbkViewContact",
    "MatchContactStore",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestFailMatchViewContactStoreL, Teardown )

EUNIT_TEST(
    "Match view contact store domain",
    "MVPbkViewContact",
    "MatchContactStoreDomain",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestMatchViewContactStoreDomainL, Teardown )

EUNIT_TEST(
    "Fail match view contact store domain",
    "MVPbkViewContact",
    "MatchContactStoreDomain",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestFailMatchViewContactStoreDomainL, Teardown )

EUNIT_TEST(
    "Create view contact bookmark",
    "MVPbkViewContact",
    "CreateBookmarkLC",
    "FUNCTIONALITY",
    SetupStoreWithContactsL, TestCreateViewContactBookmarkL, Teardown )
    
EUNIT_TEST(
    "Create filtered view",
    "MVPbkContactViewFiltering",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestCreateFilteredViewL, Teardown )  
    
EUNIT_TEST(
    "Create filtered view",
    "MVPbkContactViewFiltering",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestCreateFilteredView2L, Teardown )

EUNIT_TEST(
    "Create filtered view",
    "MVPbkContactViewFiltering",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestCreateFilteredView3L, Teardown )

EUNIT_TEST(
    "Create filtered view with always included contacts",
    "MVPbkContactViewFiltering",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestCreateFilteredView4L, Teardown )

EUNIT_TEST(
    "Delete contacts from filtered view.",
    "MVPbkContactViewFiltering",
    "DeleteContactsL",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestDeleteContactsFromFilteredViewL, Teardown )
    
EUNIT_TEST(
    "Create contact after view is filtered.",
    "MVPbkContactViewFiltering",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupCreateFindStrings2L, TestCreateContactAfterFilteringL, Teardown )    
    
EUNIT_TEST(
    "Other functions from filtered view.",
    "MVPbkContactViewFiltering",
    "AddObserverL/RemoveObserver",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestFilteredViewL, Teardown )

EUNIT_TEST(
    "Update filtered view with always included contacts",
    "MVPbkContactViewFiltering",
    "UpdateFilterL",
    "FUNCTIONALITY",
    SetupCreateFindStringsL, TestUpdateFilterL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
