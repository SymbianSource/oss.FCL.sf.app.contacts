/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact attribute assigner.
*
*/


#include "CPbk2ContactAttributeAssigner.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include "MPbk2UiServiceObserver.h"
#include "Pbk2ContactAssignerFactory.h"
#include "TPbk2TitlePaneOperator.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppStoreManager.h"
#include "MPbk2ServicePhase.h"
#include "CPbk2ContactFetchPhase.h"
#include "CPbk2AssignAttributePhase.h"
#include "CPbk2AttributeAddressSelectPhase.h"
#include "CPbk2PrepareAttributeAssignPhase.h"
#include <CPbk2FetchDlg.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2DialogEliminator.h>
#include <CPbk2StoreManager.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkPublicUid.h>
#include <VPbkFieldTypeSelectors.rsg>
#include <VPbkFieldTypeSelectorFactory.h>
#include <VPbkContactViewFilterBuilder.h>

// System includes
#include <AiwContactAssignDataTypes.h>
#include <featmgr.h>

using namespace AiwContactAssign;


/// Unnamed namespace for local definitions
namespace {

/**
 * Sets title pane.
 *
 * @param aDataRetriever    Server message data retriever.
 * @param aFlags            Assign flags.
 * @param aMessage          Server message.
 */
void SetTitlePaneL
        ( TPbk2ServerMessageDataRetriever& aDataRetriever, TUint aFlags,
          const RMessage2& aMessage )
    {
    HBufC* titlePaneText = NULL;
    if ( !( aFlags & AiwContactAssign::EUseProviderTitle ) )
        {
        titlePaneText = aDataRetriever.GetTitlePaneTextL( aMessage );
        }
    TPbk2TitlePaneOperator titlePaneOperator;
    titlePaneOperator.SetTitlePaneL( titlePaneText ); // takes ownership
    }

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicLogicPhaseCompletion
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ContactAttributeAssigner");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::CPbk2ContactAttributeAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactAttributeAssigner::CPbk2ContactAttributeAssigner
        ( MPbk2UiServiceObserver& aObserver ) :
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::~CPbk2ContactAttributeAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactAttributeAssigner::~CPbk2ContactAttributeAssigner()
    {
    delete iFetchPhase;
    delete iAddressSelectPhase;
    delete iAssignAttributePhase;

    // iPrepareAssignPhase need to be last deleted phase, because
    // it owns relocator which close stores.
    delete iPrepareAssignPhase;
    delete iStoreUris;
    delete iPreselectedContacts;
    delete iStoreManager;
    delete iContactViewFilter;
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactAttributeAssigner* CPbk2ContactAttributeAssigner::NewL
        ( const RMessage2& aFetchCompleteMessage,
          MPbk2UiServiceObserver& aObserver )
    {
    CPbk2ContactAttributeAssigner* self =
        new ( ELeave ) CPbk2ContactAttributeAssigner( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFetchCompleteMessage );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::ConstructL( const RMessage2& aMessage )
    {
    iEikEnv = CEikonEnv::Static();

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    // Retrieve data from the client-server message
    TPbk2ServerMessageDataRetriever dataRetriever;
    TUint flags = dataRetriever.SingleContactAssignFlagsL( aMessage );

    // Set title pane
    SetTitlePaneL( dataRetriever, flags, aMessage );

    iAttributeData = dataRetriever.GetAttributeDataL( aMessage );
    iRemoveAttribute =
        dataRetriever.GetAttributeRemovalIndicatorValueL( aMessage );
    iPreselectedContacts = dataRetriever.GetPreselectedContactLinksL
        ( aMessage, appUi.ApplicationServices().ContactManager() );
    iStoreUris = dataRetriever.GetContactStoreUriArrayL
        ( aMessage, appUi.ApplicationServices().StoreConfiguration(),
          iPreselectedContacts, iUseDeviceConfig );

    // Drop all non-writable stores
    iStoreManager = CPbk2ServerAppStoreManager::NewL
        ( appUi.StoreManager(),
          appUi.ApplicationServices().ContactManager() );
    iStoreManager->RemoveReadOnlyStoreUris
        ( *iStoreUris, appUi.ApplicationServices().StoreProperties() );

    // Retrieve view filter
    if ( iAttributeData.iAttributeUid ==
             TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
        {
        // Speedial specific filter
        FeatureManager::InitializeLibL();
        
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            iContactViewFilter = VPbkFieldTypeSelectorFactory::
                BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    appUi.ApplicationServices().ContactManager().FieldTypes() );
            }
        else
            {
            iContactViewFilter = CVPbkFieldTypeSelector::NewL( 
                    appUi.ApplicationServices().ContactManager().FieldTypes() );
            }
        
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
                                  *iContactViewFilter, 
                                  EVPbkContactViewFilterPhoneNumber, 
                                  appUi.ApplicationServices().ContactManager() );
        
        FeatureManager::UnInitializeLib();
        }
    
    if ( iContactViewFilter == NULL )
        {
        iContactViewFilter =
            dataRetriever.GetContactViewFilterForAttributeAssignL
                ( aMessage,
                  appUi.ApplicationServices().ContactManager().FieldTypes() );
        }
    
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::LaunchServiceL()
    {
    if ( !iStoreUris->Count() > 0 )
        {
        // All the stores are read-only or non-accessible
        iObserver.ServiceError( KErrArgument );
        }
    else
        {
        // Open stores
        iStoreManager->LoadAndOpenContactStoresL
            ( *iStoreUris, *this, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::CancelService
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::CancelService()
    {
    if ( iAssignAttributePhase )
        {
        iAssignAttributePhase->CancelServicePhase();
        }

    if ( iAddressSelectPhase )
        {
        iAddressSelectPhase->CancelServicePhase();
        }

    if ( iPrepareAssignPhase )
        {
        iPrepareAssignPhase->CancelServicePhase();
        }

    if ( iFetchPhase )
        {
        iFetchPhase->CancelServicePhase();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::AcceptDelayedContactsL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::ExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::ExitServiceL( TInt aExitCommandId )
    {
    if ( iAssignAttributePhase )
        {
        iAssignAttributePhase->RequestCancelL( aExitCommandId );
        }

    if ( iAddressSelectPhase )
        {
        iAddressSelectPhase->RequestCancelL( aExitCommandId );
        }

    if ( iPrepareAssignPhase )
        {
        iPrepareAssignPhase->RequestCancelL( aExitCommandId );
        }

    if ( iFetchPhase )
        {
        iFetchPhase->RequestCancelL( aExitCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::ServiceResults
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::ServiceResults(TServiceResults* aResults) const
    {
    if ( iAssignAttributePhase )
        {
        aResults->iLinkArray = iAssignAttributePhase->Results();
        aResults->iExtraData = iAssignAttributePhase->ExtraResultData();
        aResults->iFieldContent = iAssignAttributePhase->FieldContent();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::NextPhase
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::NextPhase
        ( MPbk2ServicePhase& aPhase )
    {
    TInt err = KErrNone;

    if ( &aPhase == iFetchPhase )
        {
        MVPbkContactLinkArray* results = iFetchPhase->Results();
        __ASSERT_DEBUG( results && results->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchPrepareAttributeAssignPhaseL( results ) );
        }
    else if ( &aPhase == iPrepareAssignPhase )
        {
        MVPbkContactLinkArray* results = iPrepareAssignPhase->Results();
        __ASSERT_DEBUG( results && results->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchAddressSelectPhaseL( results ) );
        }
    else if ( &aPhase == iAddressSelectPhase )
        {
        MVPbkContactLinkArray* results = iAddressSelectPhase->Results();
        __ASSERT_DEBUG( results && results->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchAssignPhaseL( results ) );
        }
    else if ( &aPhase == iAssignAttributePhase )
        {
        iObserver.ServiceComplete();
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPanicLogicPhaseCompletion ) );
        }

    if ( err != KErrNone )
        {
        iObserver.ServiceError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PreviousPhase
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::PreviousPhase
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    // Not supported in attribute assign
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PhaseCanceled
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::PhaseCanceled
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceCanceled();
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PhaseAborted
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::PhaseAborted
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceAborted();
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PhaseError
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::PhaseError
        ( MPbk2ServicePhase& /*aPhase*/, TInt aErrorCode )
    {
    iObserver.ServiceError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PhaseOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2ContactAttributeAssigner::PhaseOkToExit
        ( MPbk2ServicePhase& /*aPhase*/, TInt aCommandId )
    {
    // Deny exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iObserver.CompleteExitMessage( aCommandId );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::PhaseAccept
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::PhaseAccept
        ( MPbk2ServicePhase& /*aPhase*/, const TDesC8& /*aMarkedEntries*/,
          const TDesC8& /*aLinkData*/ )
    {
    // Not supported in assign
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    aStartupMonitor.DeregisterEvents( *this );

    TBool fetchViewNeeded = ETrue;

    // Fetch view is not needed when removing attribute or when
    // there are preselected contacts
    if ( iRemoveAttribute )
        {
        fetchViewNeeded = EFalse;
        }
    else if ( iPreselectedContacts && iPreselectedContacts->Count() > 0 )
        {
        fetchViewNeeded = EFalse;
        }

    if ( fetchViewNeeded )
        {
        LaunchFetchPhaseL();
        }
    else
        {
        if ( iStoreManager->ContactsAvailableL( *iPreselectedContacts ) )
            {
            LaunchPrepareAttributeAssignPhaseL( iPreselectedContacts );
            }
        else
            {
            iObserver.ServiceError( KErrNotSupported );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::StartupCanceled
// --------------------------------------------------------------------------
//
void  CPbk2ContactAttributeAssigner::StartupCanceled( TInt aErrorCode )
    {
    if ( aErrorCode == KErrCancel )
        {
        iObserver.ServiceCanceled();
        }
    else
        {
        iObserver.ServiceError( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::LaunchFetchPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::LaunchFetchPhaseL()
    {
    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_SINGLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbk2FetchDlg::EFetchSingle;

    delete iFetchPhase;
    iFetchPhase = NULL;
    iFetchPhase = CPbk2ContactFetchPhase::NewL
        ( *this, *iStoreManager, params, ETrue,
          MPbk2FetchDlgObserver::EFetchYes, iContactViewFilter,
          iUseDeviceConfig, iStoreUris );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    // Change layout to fetch's layout
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );

    iFetchPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::LaunchPrepareAttributeAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::LaunchPrepareAttributeAssignPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iPrepareAssignPhase;
    iPrepareAssignPhase = NULL;

    iPrepareAssignPhase = CPbk2PrepareAttributeAssignPhase::NewL
        ( *this, aContactLinks, iAttributeData, iRemoveAttribute );
        
 	CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    // Change layout back to regular layout after fetch
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutEmpty );

    iPrepareAssignPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::LaunchAddressSelectPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::LaunchAddressSelectPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iAddressSelectPhase;
    iAddressSelectPhase = NULL;
    iAddressSelectPhase = CPbk2AttributeAddressSelectPhase::NewL
        ( *this, aContactLinks, iAttributeData, EFalse );

    iAddressSelectPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::LaunchAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ContactAttributeAssigner::LaunchAssignPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iAssignAttributePhase;
    iAssignAttributePhase = NULL;

    iAssignAttributePhase = CPbk2AssignAttributePhase::NewL
        ( *this, aContactLinks, iAttributeData, iRemoveAttribute );

    iAssignAttributePhase->LaunchServicePhaseL();
    }

// End of File
