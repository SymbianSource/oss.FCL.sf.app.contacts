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


//  CLASS HEADER
#include "T_MVPbkContactStoreComposite.h"

#include <CVPbkContactManager.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkSortOrder.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactViewFiltering.h>
#include <TVPbkFieldTypeMapping.h>
#include <TVPbkContactStoreUriPtr.h>

#include <T_MVPbkContactStore.rsg>

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <barsc.h>
#include <barsread.h>
#include <s32mem.h>
#include <bautils.h>

//  INTERNAL INCLUDES

// LOCAL
namespace 
    {
#if defined(__WINS__)
    _LIT( KResFile, "z:\\resource\\T_MVPbkContactStore.rsc" );
#else
    _LIT( KResFile, "c:\\resource\\T_MVPbkContactStore.rsc" );
#endif // defined(__WINS__)    
    _LIT( KFNameFieldValue, "Bbbfirstname" );
    _LIT( KLNameFieldValue, "Ccclastname" ); 
    _LIT( KTelNo, "555-1239876543" );

    _LIT( KFNameFieldValue2, "Aaafirstname" );
    _LIT( KLNameFieldValue2, "Bbblastname" );
    _LIT( KTelNo2, "+358959876543" );

    _LIT( KFNameFieldValue3, "Cccfirstname" );
    _LIT( KLNameFieldValue3, "Aaalastname" );
    _LIT( KTelNo3, "04051245652" );

    void StopScheduler( CActiveSchedulerWait& aScheduler )
        {
        if ( aScheduler.IsStarted() )
            {
            aScheduler.AsyncStop();
            }
        }
        
    _LIT( KFindString, "a" );
    _LIT( KFindStrings, "aa" );

    } // namespace

// CALLBACK

void T_MVPbkContactStoreComposite::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    if (iStoreScheduler.IsStarted())
    	{    	
	    iLastError = KErrNone;
	    iLastEvent = EStoreReady;
	    // Event coming straight from the store, so no OpenComplete() expected
	    if ( iFlag ) 
	        {
	        iFlag = EFalse;
	        if (iExpectedStoreEvent == iLastEvent)
	        	{
		        iExpectedStoreResponseFound = ETrue;
	        	}
	        StopScheduler(iStoreScheduler);
	        }
    	}
    }

void T_MVPbkContactStoreComposite::StoreUnavailable
                            (MVPbkContactStore& /*aContactStore*/, TInt aReason)
    {
    if (iStoreScheduler.IsStarted())
    	{    	
	    iLastError = aReason;
	    iLastEvent = EOpenError;
	    
	    // Event coming straight from the store, so no OpenComplete() expected
	    if ( iFlag ) 
	        {
	        iFlag = EFalse;
	        iExpectedStoreResponseFound = ETrue;
 	       StopScheduler(iStoreScheduler);
	        }
    	}
    }
    
void T_MVPbkContactStoreComposite::HandleStoreEventL(
                MVPbkContactStore& /*aContactStore*/, 
                TVPbkContactStoreEvent /*aStoreEvent*/)
	{
	}

void T_MVPbkContactStoreComposite::OpenComplete()
    {
    if (iStoreScheduler.IsStarted())
    	{    	
	    iLastError = KErrNone;
	    iLastEvent = EOpenComplete;
        if (iExpectedStoreEvent == iLastEvent)
        	{
	        iExpectedStoreResponseFound = ETrue;
        	}
	    StopScheduler(iStoreScheduler);
    	}
    }

void T_MVPbkContactStoreComposite::ContactViewReady(MVPbkContactViewBase& /*aView*/)
    {
    if ( iViewScheduler.IsStarted() )
        {
        iLastError = KErrNone;
        iLastEvent = EContactViewReady;
        if (iExpectedViewEvent == iLastEvent)
        	{
	        iExpectedViewResponseFound = ETrue;
        	}
        StopScheduler(iViewScheduler);
        }
    }

void T_MVPbkContactStoreComposite::ContactViewUnavailable(MVPbkContactViewBase& /*aView*/)
    {
    if ( iViewScheduler.IsStarted() )
        {
        iLastError = KErrNone;
        iLastEvent = EContactViewUnavailable;
        if (iExpectedViewEvent == iLastEvent)
        	{
	        iExpectedViewResponseFound = ETrue;
        	}
        StopScheduler(iViewScheduler);
        }
    }

void T_MVPbkContactStoreComposite::ContactAddedToView
        (MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, const MVPbkContactLink& /*aLink*/)
    {
    if ( iViewScheduler.IsStarted() )
        {
        iLastError = KErrNone;
        iLastEvent = EContactAddedToView;
        if (iExpectedViewEvent == iLastEvent)
        	{
	        iExpectedViewResponseFound = ETrue;
        	}
        StopScheduler(iViewScheduler);
        }
    }

void T_MVPbkContactStoreComposite::ContactRemovedFromView
        (MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, const MVPbkContactLink& /*aLink*/)
    {
    if ( iViewScheduler.IsStarted() )
        {
        iLastError = KErrNone;
        iLastEvent = EContactRemovedFromView;
        if (iExpectedViewEvent == iLastEvent)
        	{
	        iExpectedViewResponseFound = ETrue;
        	}
        StopScheduler(iViewScheduler);
        }
    }

void T_MVPbkContactStoreComposite::ContactViewError
        (MVPbkContactViewBase& /*aView*/, TInt aError, TBool /*aErrorNotified*/)
    {
    if ( iViewScheduler.IsStarted() )
        {
        iLastError = aError;
        iLastEvent = EContactViewError;
        iExpectedViewResponseFound = ETrue;
        StopScheduler(iViewScheduler);
        }
    }

void T_MVPbkContactStoreComposite::StepComplete(MVPbkContactOperationBase& /*aOperation*/,
                                  TInt /*aStepSize*/)
    {
    iLastError = KErrNone;
    iLastEvent = EStepComplete;
    }

TBool T_MVPbkContactStoreComposite::StepFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aStepSize*/,
        TInt aError)
    {
    iLastError = aError;
    iLastEvent = EStepFailed;
    CActiveScheduler::Stop();
    return EFalse;
    }

void T_MVPbkContactStoreComposite::OperationComplete(MVPbkContactOperationBase& /*aOperation*/)
    {
    iLastError = KErrNone;
    iLastEvent = EOperationComplete;
    CActiveScheduler::Stop();
    }

void T_MVPbkContactStoreComposite::ContactOperationCompleted(TContactOpResult aResult)
    {
    if (iContactScheduler.IsStarted())
    	{
		iLastError = KErrNone;
		iLastEvent = EContactOperationCompleted;
		if ( aResult.iOpCode == EContactReadAndLock || 
		     aResult.iOpCode == EContactRead )
		    {
		    iStoreContact = aResult.iStoreContact;
		    }
		    
        if (iExpectedContactEvent == iLastEvent)
        	{
	        iExpectedContactResponseFound = ETrue;
        	}
		StopScheduler(iContactScheduler);
    	}
    }

void T_MVPbkContactStoreComposite::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    iLastError = aErrorCode;
    iLastEvent = EContactOperationFailed;
    iExpectedContactResponseFound = ETrue;
	StopScheduler(iContactScheduler);
    }

// CONSTRUCTION
T_MVPbkContactStoreComposite* T_MVPbkContactStoreComposite::NewL(const RArray<TVPbkContactStoreUriPtr>& aUriList)
    {
    T_MVPbkContactStoreComposite* self = T_MVPbkContactStoreComposite::NewLC(aUriList); 
    CleanupStack::Pop();

    return self;
    }

T_MVPbkContactStoreComposite* 
T_MVPbkContactStoreComposite::NewLC(const RArray<TVPbkContactStoreUriPtr>& aUriList)
    {
    T_MVPbkContactStoreComposite* self = 
        new( ELeave ) T_MVPbkContactStoreComposite();
    CleanupStack::PushL( self );

	self->ConstructL(aUriList); 

    return self;
    }

// Destructor (virtual by CBase)
T_MVPbkContactStoreComposite::~T_MVPbkContactStoreComposite()
    {
    delete iFindStrings;
    delete iUriArray;
    delete iStorePaths;
    iFs.Close();
    }

// Default constructor
T_MVPbkContactStoreComposite::T_MVPbkContactStoreComposite()
: iFlag(EFalse)
    {
    }

// Second phase construct
void T_MVPbkContactStoreComposite::ConstructL(
        const RArray<TVPbkContactStoreUriPtr>& aUriList)
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    User::LeaveIfError( iFs.Connect() );
    TInt count( aUriList.Count() );
    iStorePaths = new (ELeave) CDesCArrayFlat( count );
    iUriArray = CVPbkContactStoreUriArray::NewL();
    for ( TInt i = 0; i < count; ++i )
        {
        iStorePaths->AppendL( aUriList[ i ].UriDes() );
        iUriArray->AppendL( TVPbkContactStoreUriPtr(iStorePaths->MdcaPoint( i ) ));
        }
    }


//  METHODS

void T_MVPbkContactStoreComposite::SetupL(  )   
    {
    iFlag = EFalse;
    iContactManager = CVPbkContactManager::NewL( *iUriArray, &iFs );
    iExpectedStoreResponseFound = EFalse;
    iExpectedContactResponseFound = EFalse;    
    }

void T_MVPbkContactStoreComposite::SetupEmptyL()
    {
    iFlag = EFalse;
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
    iContactManager = CVPbkContactManager::NewL( *uriArray, &iFs );
    CleanupStack::PopAndDestroy(uriArray);
    }

void T_MVPbkContactStoreComposite::SetupStoreL()
    {
    SetupL();

    ReplaceAllStoresL();

    PopulateStoresL();
    }

void T_MVPbkContactStoreComposite::SetupViewL(TInt aViewDefResId, TInt aSortOrderResId)
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
    CleanupClosePushL(resFile);
    resFile.OpenL( iFs, path );
    TResourceReader reader;
    HBufC8* buffer = resFile.AllocReadLC(aViewDefResId);
    reader.SetBuffer(buffer);

    CVPbkContactViewDefinition* def = CVPbkContactViewDefinition::NewL(reader);
    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PushL(def);

    CVPbkSortOrder* sortOrder = CreateSortOrderLC( aSortOrderResId );

    iView = iContactManager->CreateContactViewLC( *this, *def, *sortOrder );
    CleanupStack::Pop(); // iView
    EUNIT_ASSERT( iView );
    CleanupStack::PopAndDestroy(3); // sortOrder, def, resFile

    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == iExpectedViewEvent );
    }

void T_MVPbkContactStoreComposite::SetupOrderedFlatCompositeViewL()
    {
    SetupStoreL();
    
    SetupCreateFindStringsL();

    SetupViewL(R_ORDERED_FLAT_COMPOSITE_CONTACT_VIEW, R_SORTORDER_LASTNAME_FIRSTNAME);
    }

void T_MVPbkContactStoreComposite::SetupOrderedComplexCompositeViewL()
    {
    SetupStoreL();
    
    SetupCreateFindStringsL();

    SetupViewL(R_ORDERED_COMPLEX_COMPOSITE_CONTACT_VIEW, R_SORTORDER_LASTNAME_FIRSTNAME);
    }

void T_MVPbkContactStoreComposite::SetupSortedFlatCompositeViewL()
    {
    SetupStoreL();
    
    SetupCreateFindStringsL();

    SetupViewL(R_SORTED_FLAT_COMPOSITE_CONTACT_VIEW, R_SORTORDER_LASTNAME_FIRSTNAME);
    }

void T_MVPbkContactStoreComposite::SetupSortedComplexCompositeViewL()
    {
    SetupStoreL();
    
    SetupCreateFindStringsL();

    SetupViewL(R_SORTED_COMPLEX_COMPOSITE_CONTACT_VIEW, R_SORTORDER_LASTNAME_FIRSTNAME);
    }

void T_MVPbkContactStoreComposite::SetupStoreWithContactsL()
    {
    SetupStoreL();

    PopulateStoresL();
    }

void T_MVPbkContactStoreComposite::SetupCreateFindStringsL()
    {
    iFindStrings = new(ELeave)CDesCArrayFlat( 2 );
    }

void T_MVPbkContactStoreComposite::Teardown(  )
    {
    delete iFindStrings;
    iFindStrings = NULL;
    
    delete iStoreContact; iStoreContact = NULL;
    delete iView; iView = NULL;
    if ( iStoreList )
        {
        iStoreList->CloseAll( *this );
        }
    iStoreList = NULL;
    delete iContactManager; iContactManager = NULL;
    }


// TESTS

void T_MVPbkContactStoreComposite::TestGetStoreListL()
    {
    iStoreList = &iContactManager->ContactStoresL();
    EUNIT_ASSERT( iStoreList->Count() == iUriArray->Count() );
    }

void T_MVPbkContactStoreComposite::TestLoadContactStoreL()
    {
    const TInt count = iUriArray->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iContactManager->LoadContactStoreL( (*iUriArray)[i] );
        }
    iStoreList = &iContactManager->ContactStoresL();
    EUNIT_ASSERT( iStoreList->Count() == count );
    }

void T_MVPbkContactStoreComposite::TestOpeningAllStoresL(  )
    {
    iStoreList = &iContactManager->ContactStoresL();
    TInt storeCount ( iStoreList->Count() );
    EUNIT_ASSERT_EQUALS_DESC( storeCount, 4, "Store count check failed." );

    // Try to open all stores.
    iStoreList->OpenAllL( *this );

    // We have to get EOpenComplete event.
    StartStoreSchedulerAndDecideToLeaveL(EOpenComplete);
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == iExpectedStoreEvent );
    }

void T_MVPbkContactStoreComposite::TestViewContactCountL()
    {
    // There is at least 3 * storecount contacts available
    EUNIT_ASSERT( iView->ContactCountL() >= ( iUriArray->Count() * 3 ) );
    }

void T_MVPbkContactStoreComposite::TestCommitMultipleContactsL()
    {
    TInt cntCount( iView->ContactCountL() );
    RArray<MVPbkStoreContact*> storeCnts;
    CleanupClosePushL( storeCnts );
    
    iView->ContactAtL( 0 ).ReadAndLockL( *this );

    StartContactSchedulerAndDecideToLeaveL(EContactOperationCompleted);
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == iExpectedContactEvent );
    iExpectedContactResponseFound = EFalse;

    MVPbkStoreContact* cnt1 = iStoreContact;
    iStoreContact = NULL;
    CleanupDeletePushL( cnt1 );
    storeCnts.Append( cnt1 );

    // get last contact from view
    iView->ContactAtL( cntCount - 1 ).ReadAndLockL( *this ); 

    StartContactSchedulerAndDecideToLeaveL(EContactOperationCompleted);
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == iExpectedContactEvent );
    iExpectedContactResponseFound = EFalse;

    MVPbkStoreContact* cnt2 = iStoreContact;
    iStoreContact = NULL;
    CleanupDeletePushL( cnt2 );
    storeCnts.Append( cnt2 );
        
    MVPbkContactOperationBase* op = 
        iContactManager->CommitContactsL( storeCnts.Array(), *this );
    CleanupDeletePushL( op );

    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EOperationComplete );
    EUNIT_ASSERT( iView->ContactCountL() == cntCount );

    CleanupStack::PopAndDestroy( 4 ); // op, cnt2, cnt1, storeCnts
    }

void T_MVPbkContactStoreComposite::TestDeleteMultipleContactsL()
    {
    TInt deletecount( 2 );
    TInt cntCount( iView->ContactCountL() );
    CVPbkContactLinkArray* contactLinks = CVPbkContactLinkArray::NewLC();

    for ( TInt i = 0; i < deletecount; ++i )
        {
        MVPbkContactLink* lnk = iView->ContactAtL( i ).CreateLinkLC();
        contactLinks->AppendL( lnk );
        CleanupStack::Pop(); // lnk
        }

    MVPbkContactOperationBase* op = 
        iContactManager->DeleteContactsL( *contactLinks, *this);
    CleanupDeletePushL( op );
    
    // TODO: This part of VPBkEng under construction...
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EOperationComplete );
    /*
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EContactViewUnavailable );
    StartSchedulerAndDecideToLeaveL();
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EContactViewReady );
    */
    
    EUNIT_ASSERT( iView->ContactCountL() == ( cntCount - deletecount ) );

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // contactLnks
    }
    
void T_MVPbkContactStoreComposite::TestCreateFilteredViewL()
    {
    // First check that the view supports filtering
    EUNIT_ASSERT( iView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
       
    MVPbkContactViewBase* findView = 
        iView->ViewFiltering()->CreateFilteredViewLC( *this, *iFindStrings, 
            NULL );
    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);
    
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EContactViewReady );    
    
    CleanupStack::PopAndDestroy( 1 ); // findView
    }
    
void T_MVPbkContactStoreComposite::TestCreateFilteredView2L()
    {
    // First check that the view supports filtering
    EUNIT_ASSERT( iView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
    
    // First character
    MVPbkContactViewBase* findView = 
        iView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL ); 
    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Second character
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);    
    
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == EContactViewReady );
    
    CleanupStack::PopAndDestroy( 2 ); // findView, refineView
    }
        
void T_MVPbkContactStoreComposite::TestDeleteContactsFromFilteredViewL()
    {
    // First check that the view supports filtering
    EUNIT_ASSERT( iView->ViewFiltering() );
    
    iFindStrings->AppendL( KFindString );
    
    // First character
    MVPbkContactViewBase* findView = 
        iView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);
    
    iFindStrings->Reset();
    iFindStrings->AppendL( KFindStrings );
    
    // Second character
    MVPbkContactViewBase* refineView = 
        findView->ViewFiltering()->CreateFilteredViewLC( *this, 
            *iFindStrings, NULL );
    StartViewSchedulerAndDecideToLeaveL(EContactViewReady);    
        
    TestDeleteMultipleContactsL();
            
    CleanupStack::PopAndDestroy( 2 ); // findView, refineView        
    }

// HELP FUNCTIONS

void T_MVPbkContactStoreComposite::ReplaceAllStoresL()
    {
    iStoreList = &iContactManager->ContactStoresL();
    TInt count ( iStoreList->Count() );
    
    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkContactStore& contactStore = iStoreList->At( i );
        contactStore.ReplaceL( *this );
	    
	    // Flag to notice that we dont wait OpenComplete event
	    // but StoreReady or StoreError event
	    iFlag = ETrue;
	    
        StartStoreSchedulerAndDecideToLeaveL(EStoreReady);
        EUNIT_ASSERT( iLastError == KErrNone );
        EUNIT_ASSERT( iLastEvent == iExpectedStoreEvent );
        }
    }

CVPbkSortOrder* T_MVPbkContactStoreComposite::CreateSortOrderLC( TInt aResId )
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

void T_MVPbkContactStoreComposite::PopulateStoresL()
    {
    TInt storeCount( iStoreList->Count() );
    for ( TInt i = 0; i < storeCount; ++i )
        {
        MVPbkContactStore* store = &iStoreList->At( i );
        MVPbkStoreContact* storeContact = store->CreateNewContactLC();
        AddContactFieldsAndSaveL( *storeContact, KFNameFieldValue, KLNameFieldValue, KTelNo );
        CleanupStack::PopAndDestroy(); // storeContact
        storeContact = store->CreateNewContactLC();
        AddContactFieldsAndSaveL( *storeContact, KFNameFieldValue2, KLNameFieldValue2, KTelNo2 );
        CleanupStack::PopAndDestroy(); // storeContact
        storeContact = store->CreateNewContactLC();
        AddContactFieldsAndSaveL( *storeContact, KFNameFieldValue3, KLNameFieldValue3, KTelNo3 );
        CleanupStack::PopAndDestroy(); // storeContact
        }
    }

void T_MVPbkContactStoreComposite::AddContactFieldsAndSaveL( 
    MVPbkStoreContact& aStoreContact, const TDesC& aFName, const TDesC& aLName, const TDesC& aTelNo )
    {
    AddFieldToContact( aStoreContact, R_FIRST_NAME_TYPE, aFName );
    AddFieldToContact( aStoreContact, R_LAST_NAME_TYPE, aLName );
    AddFieldToContact( aStoreContact, R_TEL_NO_TYPE, aTelNo );
   
    // Save modifications to the storecontact
    aStoreContact.CommitL( *this );
    StartContactSchedulerAndDecideToLeaveL(EContactOperationCompleted);
    EUNIT_ASSERT( iLastError == KErrNone );
    EUNIT_ASSERT( iLastEvent == iExpectedContactEvent );
    iExpectedContactResponseFound = EFalse;
    if ( iView ) // If no view, there will be no events to it
        {
        StartViewSchedulerAndDecideToLeaveL(EContactAddedToView);
        EUNIT_ASSERT( iLastError == KErrNone );
        EUNIT_ASSERT( iLastEvent == iExpectedViewEvent );
        }
    }

void T_MVPbkContactStoreComposite::AddFieldToContact( MVPbkStoreContact& aStoreContact, TInt aResId, const TDesC& aValue )
    {
    TInt fldCount( aStoreContact.Fields().FieldCount() );
    // Create fieldtype
    TVPbkFieldTypeMapping fieldTypeMapping = CreateFieldTypeMappingL( aResId );
    const MVPbkFieldType* fieldType = 
        fieldTypeMapping.FindMatch( iContactManager->FieldTypes() );
    EUNIT_ASSERT( fieldType );
    // Add value to the field
    AddValueToContactFieldL( aStoreContact, *fieldType, aValue );
    EUNIT_ASSERT( aStoreContact.Fields().FieldCount() == ++fldCount );
    }

void T_MVPbkContactStoreComposite::AddValueToContactFieldL( MVPbkStoreContact& aStoreContact, 
    const MVPbkFieldType& aFieldType, const TDesC& aValue )
    {
    MVPbkStoreContactField* field = aStoreContact.CreateFieldLC( aFieldType );
    EUNIT_ASSERT( field->FieldData().DataType() == EVPbkFieldStorageTypeText );
    MVPbkContactFieldTextData::Cast( field->FieldData() ).SetTextL( aValue );
    aStoreContact.AddFieldL( field );
    CleanupStack::Pop();    
    }

TVPbkFieldTypeMapping T_MVPbkContactStoreComposite::CreateFieldTypeMappingL( const TInt aResId )
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

void T_MVPbkContactStoreComposite::StartSchedulerAndDecideToLeaveL()
    {
    CActiveScheduler::Start();
    if ( iLastError == KErrNoMemory )
        {
        User::Leave( iLastError );
        }
    }
    
void T_MVPbkContactStoreComposite::StartStoreSchedulerAndDecideToLeaveL(TInt aExpectedEvent)
	{
	iExpectedStoreEvent = aExpectedEvent;
    while (!iExpectedStoreResponseFound)
    	{	
	    iStoreScheduler.Start();
	    if ( iLastError == KErrNoMemory )
	        {
	        User::Leave( iLastError );
	        }
    	}	
    iExpectedStoreResponseFound = EFalse;
	}
    
void T_MVPbkContactStoreComposite::StartContactSchedulerAndDecideToLeaveL(TInt aExpectedEvent)
	{
	iExpectedContactEvent = aExpectedEvent;
    while (!iExpectedContactResponseFound)
    	{	
	    iContactScheduler.Start();
	    if ( iLastError == KErrNoMemory )
	        {
	        User::Leave( iLastError );
	        }
    	}	
    iExpectedContactResponseFound = EFalse;
	}


void T_MVPbkContactStoreComposite::StartViewSchedulerAndDecideToLeaveL(TInt aExpectedEvent)
    {
    iExpectedViewEvent = aExpectedEvent;
    while (!iExpectedViewResponseFound)
    	{	
	    iViewScheduler.Start();
	    if ( iLastError == KErrNoMemory )
	        {
	        User::Leave( iLastError );
	        }
    	}
    iExpectedViewResponseFound = EFalse;
    }	
	
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    T_MVPbkContactStoreComposite,
    "VPbkApiTester::MVPbkContactStoreComposite",
    "MODULE" )

EUNIT_TEST(
    "Retrieving stores from contact manager",
    "MVPbkContactStoreList",
    "ContactStoresL",
    "FUNCTIONALITY",
    SetupL, TestGetStoreListL, Teardown )

EUNIT_TEST(
    "Loading stores dynamically",
    "CVPbkContactManager",
    "LoadContactStoreL",
    "FUNCTIONALITY",
    SetupL, TestLoadContactStoreL, Teardown )

EUNIT_TEST(
    "Opening all stores",
    "MVPbkContactStoreList",
    "OpenAllL",
    "FUNCTIONALITY",
    SetupStoreL, TestOpeningAllStoresL, Teardown )

EUNIT_TEST(
    "View's contact count",
    "MVPbkContactViewBase",
    "ContactCountL",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestViewContactCountL, Teardown )

EUNIT_TEST(
    "View's contact count",
    "MVPbkContactViewBase",
    "ContactCountL",
    "FUNCTIONALITY",
    SetupOrderedComplexCompositeViewL, TestViewContactCountL, Teardown )

EUNIT_TEST(
    "View's contact count",
    "MVPbkContactViewBase",
    "ContactCountL",
    "FUNCTIONALITY",
    SetupSortedFlatCompositeViewL, TestViewContactCountL, Teardown )

EUNIT_TEST(
    "View's contact count",
    "MVPbkContactViewBase",
    "ContactCountL",
    "FUNCTIONALITY",
    SetupSortedComplexCompositeViewL, TestViewContactCountL, Teardown )

EUNIT_TEST(
    "Commit of multiple contacts",
    "CVPbkContactManager",
    "CommitContactsL",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestCommitMultipleContactsL, Teardown )

EUNIT_TEST(
    "Deletion of multiple contacts",
    "CVPbkContactManager",
    "DeleteContactsL",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestDeleteMultipleContactsL, Teardown )
    
EUNIT_TEST(
    "Create filtered view. One character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupSortedFlatCompositeViewL, TestCreateFilteredViewL, Teardown )    

EUNIT_TEST(
    "Create filtered view. One character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestCreateFilteredViewL, Teardown )        
    
EUNIT_TEST(
    "Create filtered view. One character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupSortedComplexCompositeViewL, TestCreateFilteredViewL, Teardown )    

EUNIT_TEST(
    "Create filtered view. One character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupOrderedComplexCompositeViewL, TestCreateFilteredViewL, Teardown )   

EUNIT_TEST(
    "Create filtered view. Two character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupSortedFlatCompositeViewL, TestCreateFilteredView2L, Teardown )    

EUNIT_TEST(
    "Create filtered view. Two character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestCreateFilteredView2L, Teardown )        
    
EUNIT_TEST(
    "Create filtered view. Two character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupSortedComplexCompositeViewL, TestCreateFilteredView2L, Teardown )    

EUNIT_TEST(
    "Create filtered view. Two character.",
    "MVPbkContactViewBase",
    "CreateFilteredViewLC",
    "FUNCTIONALITY",
    SetupOrderedComplexCompositeViewL, TestCreateFilteredView2L, Teardown ) 
    
EUNIT_TEST(
    "Delete contacts from filtered view.",
    "CVPbkContactManager",
    "DeleteContactsL",
    "FUNCTIONALITY",
    SetupOrderedFlatCompositeViewL, TestDeleteContactsFromFilteredViewL, Teardown )                 
    
EUNIT_END_TEST_TABLE

//  END OF FILE
