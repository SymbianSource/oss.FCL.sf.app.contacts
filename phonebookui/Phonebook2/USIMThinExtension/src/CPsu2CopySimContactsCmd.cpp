/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 copy "new SIM" contacts to phone memory.
*
* NOTE: CPsu2CopySimContactsCmd contains a lot of similar code to this class.
* Reason is that there is no clear place to share common SIM related code between 
* USIMThinExtension.dll and USIMExtension.dll. This is a potential place for
* future refactoring. 
*
*/


// INCLUDE FILES
#include "CPsu2CopySimContactsCmd.h"

// Phonebook 2
#include <MPbk2CommandObserver.h>
#include <MPbk2StartupMonitor.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <TPbk2CopyContactsResults.h>
#include <Phonebook2PublicPSKeys.h>
#include <Phonebook2PrivatePSKeys.h>
#include <Pbk2UID.h>
#include <Pbk2USimThinUIRes.rsg>
/// Use R_PBK2_GENERAL_CONFIRMATION_QUERY from UI controls
/// As a core components UiControl's resource file is loaded in core Pbk2
#include <Pbk2UIControls.rsg>
/// Use R_GENERAL_COPY_PROGRESS_NOTE from Commands
/// As a core components Command's resource file is loaded in core Pbk2
#include <Pbk2Commands.rsg>
#include <Pbk2DataCaging.hrh>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2StoreValidityInformer.h>

// Virtual Phonebook
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkSimStoreFactory.h>
#include <MVPbkSimStateInformation.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactView.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactCopier.h>

// System includes
#include <AknQueryDialog.h>
#include <e32property.h>
#include <StringLoader.h>
#include <e32cmn.h>
#include <e32capability.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

// CONSTANTS
const TInt KSimNotChecked = 0;
const TInt KSimChecked = 1;
_LIT( KPsu2ThinUiExtResourceFile, "Pbk2USimThinUIRes.rsc" );


#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_ValidStoreConfigurationL,
    EStorePropertyNotFound_CreateSimViewsL,
    ENullTarget_CopyContactsL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPsu2CopySimContactsCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/// Tasks
enum TNextTask
    {
    ECheckCopyPrerequisites,
    EOpenSimStores,
    ECheckSimStorePrerequisites,
    ELaunchNewSIMInsertedQuery,
    ECreateSimViews,
    ECopyContacts,
    EShowResults,
    EComplete
    };

} /// namespace


// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CPsu2CopySimContactsCmd
// --------------------------------------------------------------------------
//
CPsu2CopySimContactsCmd::CPsu2CopySimContactsCmd
        ( MPbk2StartupMonitor& aStartupMonitor ) :
            CActive( EPriorityStandard ),
            iStartupMonitor( aStartupMonitor ),
            iResourceFile( *CCoeEnv::Static() )
    {
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::~CPsu2CopySimContactsCmd
// --------------------------------------------------------------------------
//
CPsu2CopySimContactsCmd::~CPsu2CopySimContactsCmd()
    {
    Cancel();
    delete iDecorator;
    delete iCopyOperation;
    delete iCopier;
    delete iCopiedContacts;
    delete iValidSourceStoreUris;
    iSourceViews.ResetAndDestroy();
    CloseStores();
    delete iSourceContactLinks;
    iSourceStores.Close();
    iResourceFile.Close();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPsu2CopySimContactsCmd* CPsu2CopySimContactsCmd::NewL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    CPsu2CopySimContactsCmd* self =
        new( ELeave ) CPsu2CopySimContactsCmd( aStartupMonitor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ConstructL()
    {
    CActiveScheduler::Add(this);

    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
        ( R_GENERAL_COPY_PROGRESS_NOTE, EFalse );
    iDecorator->SetObserver( *this );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CopySimContactsCmd::RunL res %d task %d"),
        iStatus.Int(), iNextTask);

    if ( iStatus.Int() == KErrNone )
        {
        switch ( iNextTask )
            {
            case ECheckCopyPrerequisites:
                {
                CheckCopyPrerequisitesL();
                break;
                }
            case EOpenSimStores:
                {
                OpenSimStoresL();
                break;
                }
            case ECheckSimStorePrerequisites:
                {
                CheckSimStorePrerequisitesL();
                break;
                }
            case ELaunchNewSIMInsertedQuery:
                {
                LaunchNewSIMInsertedQueryL();
                break;
                }
            case ECreateSimViews:
                {
                CreateSimViewsL();
                break;
                }
            case ECopyContacts:
                {
                CopyContactsL();
                break;
                }
            case EShowResults:
                {
                ShowResultsL();
                break;
                }
            case EComplete: // FALLTHROUGH
            default:
                {
                CompleteL();
                break;
                }
            }
        }
    else
        {
        CompleteWithError( iStatus.Int() );
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::DoCancel()
    {
    // Nothing to do here
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2CopySimContactsCmd::RunError( TInt aError )
    {
    iStartupMonitor.HandleStartupFailed( aError );
    iCommandObserver->CommandFinished(*this);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ExecuteLD()
    {
    iNextTask = ECheckCopyPrerequisites;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ResetUiControl
        ( MPbk2ContactUiControl& /*aUiControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::StoreReady
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::StoreReady
        ( MVPbkContactStore& /*aContactStore*/)
    {
    ++iCounter;
    if ( iSourceStores.Count() == iCounter )
        {
        iNextTask = ECheckSimStorePrerequisites;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::StoreUnavailable
        ( MVPbkContactStore& aContactStore,  TInt /*aReason*/ )
    {
    // Remove URI
    iValidSourceStoreUris->Remove( aContactStore.StoreProperties().Uri() );

    // Remove store
    const TInt count = iSourceStores.Count();
    TBool storeRemoved = EFalse;
    for ( TInt i = 0; i < count && !storeRemoved; ++i )
        {
        if ( iSourceStores[i] == &aContactStore )
            {
            iSourceStores[i]->Close( *this ); 
            iSourceStores.Remove( i );
            storeRemoved = ETrue;
            }
        }

    // If all source stores failed to open -> complete command
    if ( iSourceStores.Count() == 0 )
        {
        iNextTask = EComplete;
        IssueRequest();
        }
    // All available stores are open
    else if ( iSourceStores.Count() == iCounter )
        {
        iNextTask = ECheckSimStorePrerequisites;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/)
    {
    ++iCounter;
    if ( iSourceViews.Count() == iCounter )
        {
        iNextTask = ECopyContacts;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Nothing to be done here, wait view ready events
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt aError,
          TBool /*aErrorNotified*/ )
    {
    IssueRequest( aError );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::StepComplete
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aStepSize )
    {
    iCopiedSuccessfully += aStepSize;
    iDecorator->ProcessAdvance( aStepSize );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPsu2CopySimContactsCmd::StepFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/,
          TInt aError )
    {
    CCoeEnv::Static()->HandleError(aError);
    if( aError == KErrDiskFull )
    	{
    	// When disk get full during copy, show the results
        Cancel();
        delete iCopyOperation;
        iCopyOperation = NULL;
        iNextTask = EShowResults;
        IssueRequest();
    	}
    else
    	{
        IssueRequest( aError );
    	}
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::OperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    // Decorator calls ProcessDismissed
    iDecorator->ProcessStopped();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    Cancel();
    delete iCopyOperation;
    iCopyOperation = NULL;
    iNextTask = EShowResults;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::IssueRequest()
    {
    IssueRequest( KErrNone );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::IssueRequest( TInt aResult )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aResult );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CheckCopyPrerequisitesL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CheckCopyPrerequisitesL()
    {
    if ( ValidStoreConfigurationL() && 
         NewSimCardForPhonebook2AppL() )
        {
        iNextTask = EOpenSimStores;
        }
    else
        {
        iNextTask = EComplete;
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::OpenSimStoresL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::OpenSimStoresL()
    {
    // Load and open source stores to contact manager
    const TInt count = iValidSourceStoreUris->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            LoadContactStoreL( (*iValidSourceStoreUris)[i] );
        MVPbkContactStore* source =
            Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                ContactStoresL().Find( (*iValidSourceStoreUris)[i] );
        iSourceStores.AppendL( source );
        source->OpenL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CheckSimStorePrerequisitesL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CheckSimStorePrerequisitesL()
    {
    // If there are no contacts to copy then do nothing
    TInt sourceContactCount = 0;
    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        sourceContactCount +=
            iSourceStores[i]->StoreInfo().NumberOfContactsL();
        }

    iNextTask = EComplete;
    if ( sourceContactCount > 0 )
        {
        iNextTask = ELaunchNewSIMInsertedQuery;
        }
    else
    	{
    	SetNewSimCardKeyL();    	
    	}
    
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::LaunchNewSIMInsertedQueryL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::LaunchNewSIMInsertedQueryL()
    {
    // Open resource as late as possible so that it's not opened in start-up
    // if not needed
    iResourceFile.OpenL( KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPsu2ThinUiExtResourceFile );

    HBufC* text = StringLoader::LoadLC( R_QTN_SIMP_NOTE_NEW_SIM_COPY );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt ret = dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *text );
    if ( iAvkonAppUi->IsForeground() )
    	{
    	// Despite of user answer set P&S key
        SetNewSimCardKeyL();
        SetShowSIMContactsL( ret );        
        iNextTask = ret ? ECreateSimViews : EComplete;        
    	}
    else
    	{
	    iNextTask = EComplete;
    	}

    CleanupStack::PopAndDestroy( text );
    IssueRequest();
    }


// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::SetShowSIMContactsL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::SetShowSIMContactsL( TBool aShow )
    {
    const TDesC& adnUri = VPbkContactStoreUris::SimGlobalAdnUri();
    
    CVPbkContactStoreUriArray* uriArray = 
        Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration().
            CurrentConfigurationL();
    CleanupStack::PushL( uriArray );

    if ( iAvkonAppUi->IsForeground() )
    	{
	    if ( aShow )
	        {
	        if ( !uriArray->IsIncluded(adnUri) )
	            {
	            // Show SIM contact also in names list:
	            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration().
	                AddContactStoreURIL( adnUri );
	            }
	        }
	    else
	        {
	        if ( uriArray->IsIncluded(adnUri) )
	            {
	            // Do not show SIM contacts:
	            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration().
	                RemoveContactStoreURIL( adnUri );
	            }
	        }

	    iNextTask = EComplete;
    	}

    CleanupStack::PopAndDestroy( uriArray );
    }


// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CreateSimViewsL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CreateSimViewsL()
    {
    // Set views ready counter to zero
    iCounter = 0;

    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Find store property
        const CPbk2StoreProperty* prop =
            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties().
                FindProperty( iSourceStores[i]->StoreProperties().Uri() );
        __ASSERT_DEBUG( prop,
            Panic( EStorePropertyNotFound_CreateSimViewsL ) );
        // Get Virtual Phonebook view definition
        // There is only one view defintion for SIM stores
        // -> take the first one
        const CVPbkContactViewDefinition& viewDef =
            (prop->Views())[0]->ViewDefinition();
        MVPbkContactView* view = iSourceStores[i]->CreateViewLC(
            viewDef,
            *this,
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                SortOrderManager().SortOrder() );
        iSourceViews.AppendL( view );
        CleanupStack::Pop(); // view
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CopyContactsL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CopyContactsL()
    {
    if ( !iCopier )
        {
        iCopier = CVPbkContactCopier::NewL
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
        }
    
    // Create an array for source contact links
    if ( !iSourceContactLinks )
        {
        iSourceContactLinks = CVPbkContactLinkArray::NewL();
        }

    if ( !iCopiedContacts )
        {
        iCopiedContacts = CVPbkContactLinkArray::NewL();
        }
    
    // Create links to the contacts that are copied
    const TInt viewCount = iSourceViews.Count();
    for ( TInt i = 0; i < viewCount; ++i )
        {
        const TInt contactCount = iSourceViews[i]->ContactCountL();
        for( TInt j = 0; j < contactCount; ++j )
            {
            MVPbkContactLink* link =
                iSourceViews[i]->ContactAtL(j).CreateLinkLC();
            iSourceContactLinks->AppendL( link );
            CleanupStack::Pop(); // link
            }
        }

    // Views can be destroyed now because links have been created
    iSourceViews.ResetAndDestroy();

    // Get target store
    MVPbkContactStore* targetStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            ContactStoresL().Find( VPbkContactStoreUris::DefaultCntDbUri() );
    // Target store must exist as cheked in ValidStoreConfigurationL
    __ASSERT_DEBUG( targetStore, Panic( ENullTarget_CopyContactsL ));

    // Start copying
    iCopiedContacts->ResetAndDestroy();
    iCopyOperation = iCopier->CopyContactsL(
        CVPbkContactCopier::EVPbkUsePlatformSpecificDuplicatePolicy,
        *iSourceContactLinks, targetStore, *iCopiedContacts, *this );


    // Show progress note
    iDecorator->ProcessStartedL( iSourceContactLinks->Count() );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ShowResultsL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::ShowResultsL()
    {
    TPbk2CopyContactsResults results(
        iCopiedSuccessfully, iSourceContactLinks->Count() );
    results.ShowNoteL();

    iNextTask = EComplete;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CompleteL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CompleteL()
    {
    iStartupMonitor.HandleStartupComplete();
    iCommandObserver->CommandFinished( *this );

    if (iAvkonAppUi->IsForeground())
        {
        PublishOpenCompleteL();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::PublishOpenCompleteL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::PublishOpenCompleteL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CopySimContactsCmd::PublishOpenCompleteL") );

    TInt err = RProperty::Define( TUid::Uid( KPbk2PSUidPublic ),
                                  KPhonebookOpenCompleted, RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    RProperty prop;
    CleanupClosePushL( prop );
    User::LeaveIfError(prop.Attach( TUid::Uid( KPbk2PSUidPublic ),
                                    KPhonebookOpenCompleted )); 
    TInt value( EPhonebookClosed );
    err = prop.Get( value );
    if ( err == KErrNone && value != EPhonebookOpenCompleted)
        {
        // Only publish once per phonebook opening
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("publish EPhonebookOpenCompleted") );
        err = prop.Set( EPhonebookOpenCompleted );
        }
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy(&prop);
    }


// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CompleteWithError
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CompleteWithError( TInt aError )
    {
    iStartupMonitor.HandleStartupFailed( aError );
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::ValidStoreConfigurationL
// --------------------------------------------------------------------------
//
TBool CPsu2CopySimContactsCmd::ValidStoreConfigurationL()
    {
    __ASSERT_DEBUG( !iValidSourceStoreUris,
        Panic(EPreCond_ValidStoreConfigurationL));

    CVPbkContactStoreUriArray* currentConfig =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().CurrentConfigurationL();
    CleanupStack::PushL( currentConfig );
    CVPbkContactStoreUriArray* validConfig =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreValidityInformer().CurrentlyValidStoresL();
    CleanupStack::PushL( validConfig );

    // ADN/SDN is valid source if it's not in current configuration
    iValidSourceStoreUris = CVPbkContactStoreUriArray::NewL();
    const TDesC& adnUri = VPbkContactStoreUris::SimGlobalAdnUri();
    const TDesC& sdnUri = VPbkContactStoreUris::SimGlobalSdnUri();
    if ( !currentConfig->IsIncluded( adnUri ))
        {
        iValidSourceStoreUris->AppendL( adnUri );
        }
    if ( !currentConfig->IsIncluded( sdnUri ))
        {
        iValidSourceStoreUris->AppendL( sdnUri );
        }
    
    // No sim store needs to be handled in this command, 
    // directly set the flag.
    if ( iValidSourceStoreUris->Count() == 0 )
    	{
    	SetNewSimCardKeyL();    	
    	}

    TBool result = EFalse;
    // If there are source stores and contact model is valid
    // then configuration is valid
    if ( iValidSourceStoreUris->Count() > 0 &&
         validConfig->IsIncluded(
            VPbkContactStoreUris::DefaultCntDbUri() ))
        {
        result = ETrue;
        }

    CleanupStack::PopAndDestroy( 2 );
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::NewSimCardForPhonebook2AppL
// --------------------------------------------------------------------------
//
TBool CPsu2CopySimContactsCmd::NewSimCardForPhonebook2AppL()
    {
    MVPbkSimStateInformation* simStateInfo =
        VPbkSimStoreFactory::GetSimStateInformationL();
    CleanupDeletePushL( simStateInfo );
    TBool newSimCard = simStateInfo->NewSimCardL();
    CleanupStack::PopAndDestroy(); // simStateInfo

    if ( newSimCard )
        {
        RProperty queryProperty;
        TInt queryDone = KSimNotChecked;
        TInt result = queryProperty.Get(
            TUid::Uid( KPbk2UID3 ), KPhonebookSimCopyAsked, queryDone );
        queryProperty.Close();
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2CopySimContactsCmd::NewSimCardForPhonebook2AppL res %d status %d"),
            result, queryDone);
        // If result is KErrNone it means that the key has been set
        // and query has been asked before, KErrNotFound means that key
        // is not set and query hasn't been asked
        if ( result == KErrNotFound )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CloseStores
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::CloseStores()
    {
    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSourceStores[i]->Close( *this );
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::SetNewSimCardKeyL
// --------------------------------------------------------------------------
//
void CPsu2CopySimContactsCmd::SetNewSimCardKeyL()
    {
    RProperty queryProperty;
    CleanupClosePushL( queryProperty );

    // Check if the key has been defined
    TInt queryDone = KSimNotChecked;
    TInt result = queryProperty.Get(
        TUid::Uid( KPbk2UID3 ), KPhonebookSimCopyAsked, queryDone );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CopySimContactsCmd::SetNewSimCardKeyL res %d status %d"),
        result, queryDone);

    if ( result == KErrNotFound )
        {
        _LIT_SECURITY_POLICY_C1( newSimCardQueryReadPolicy,
            ECapabilityReadUserData );
        _LIT_SECURITY_POLICY_S0( newSimCardQueryWritePolicy, KPbk2UID3 );

        // Define a property for query done flag
        User::LeaveIfError( queryProperty.Define(
            TUid::Uid( KPbk2UID3 ),
            KPhonebookSimCopyAsked,
            RProperty::EInt,
            newSimCardQueryReadPolicy,
            newSimCardQueryWritePolicy ));
        }

    // Set the value to KSimChecked
    User::LeaveIfError( queryProperty.Set(
        TUid::Uid( KPbk2UID3 ),
        KPhonebookSimCopyAsked,
        KSimChecked ));

    CleanupStack::PopAndDestroy(); // queryProperty
    }

//  End of File
