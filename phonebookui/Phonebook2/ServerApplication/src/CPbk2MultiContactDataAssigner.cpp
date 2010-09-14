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
* Description:  Phonebook 2 multiple contact data assigner.
*
*/


#include "CPbk2MultiContactDataAssigner.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include "MPbk2UiServiceObserver.h"
#include "CPbk2SelectMultiplePropertyPhase.h"
#include "CPbk2ContactFetchPhase.h"
#include "CPbk2MultiAssignDataPhase.h"
#include "CPbk2PrepareMultipleAssignPhase.h"
#include "TPbk2TitlePaneOperator.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppStoreManager.h"
#include "MPbk2SelectFieldProperty.h"
#include <CPbk2FetchDlg.h>
#include <MPbk2ApplicationServices.h>
#include <pbk2uicontrols.rsg>
#include <pbk2serverapp.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicLogicPhaseCompletion
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2MultiContactDataAssigner");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

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

} /// namespace


// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::CPbk2MultiContactDataAssigner
// --------------------------------------------------------------------------
//
CPbk2MultiContactDataAssigner::CPbk2MultiContactDataAssigner
        ( MPbk2UiServiceObserver& aObserver ) :
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::~CPbk2MultiContactDataAssigner
// --------------------------------------------------------------------------
//
CPbk2MultiContactDataAssigner::~CPbk2MultiContactDataAssigner()
    {
    delete iSelectPropertyPhase;
    delete iFetchPhase;
    delete iPrepareAssignPhase;
    delete iAssignDataPhase;
    delete iStoreUris;
    delete iDataBuffer;
    delete iAddressSelectFilterBuffer;
    delete iPreselectedContacts;
    delete iSelectFieldProperty;
    delete iContactViewFilter;
    delete iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2MultiContactDataAssigner* CPbk2MultiContactDataAssigner::NewL(
        const RMessage2& aFetchCompleteMessage,
        MPbk2UiServiceObserver& aObserver )
    {
    CPbk2MultiContactDataAssigner* self =
        new ( ELeave ) CPbk2MultiContactDataAssigner( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFetchCompleteMessage );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::ConstructL( const RMessage2& aMessage )
    {
    // Retrieve data from the client-server message
    TPbk2ServerMessageDataRetriever dataRetriever;
    TUint flags = dataRetriever.MultipleContactAssignFlagsL( aMessage );

    // Set title pane
    SetTitlePaneL( dataRetriever, flags, aMessage );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    iPreselectedContacts = dataRetriever.GetPreselectedContactLinksL
        ( aMessage, appUi.ApplicationServices().ContactManager() );
    iStoreUris = dataRetriever.GetContactStoreUriArrayL
        ( aMessage, appUi.ApplicationServices().StoreConfiguration(),
          iPreselectedContacts, iUseDeviceConfig );
    iAddressSelectFilterBuffer =
        dataRetriever.GetAddressSelectFilterBufferL( aMessage );
    iDataBuffer = dataRetriever.GetDataBufferL( aMessage );
    iMimeType = dataRetriever.GetMimeTypeL( aMessage );
    
    // check if the client has special orientation setting to make sure keeping 
    // the same orientation with the client.
    TInt clientOrientationType = dataRetriever.GetOrietationTypeL( aMessage );
    if( clientOrientationType == CAknAppUi::EAppUiOrientationLandscape )
        {
        appUi.SetOrientationL( CAknAppUi::EAppUiOrientationLandscape );
        }
    else if( clientOrientationType == CAknAppUi::EAppUiOrientationPortrait )
        {
        appUi.SetOrientationL( CAknAppUi::EAppUiOrientationPortrait );
        }

    // Drop all non-writable stores
    iStoreManager = CPbk2ServerAppStoreManager::NewL
        ( appUi.StoreManager(),
          appUi.ApplicationServices().ContactManager() );
    iStoreManager->RemoveReadOnlyStoreUris
        ( *iStoreUris, appUi.ApplicationServices().StoreProperties() );

    // Retrieve view filter
    iContactViewFilter =
        dataRetriever.GetContactViewFilterForAttributeAssignL
            ( aMessage,
              appUi.ApplicationServices().ContactManager().FieldTypes() );
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::LaunchServiceL()
    {
    // Open stores
    iStoreManager->LoadAndOpenContactStoresL( *iStoreUris, *this, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::CancelService
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::CancelService()
    {
    if ( iAssignDataPhase )
        {
        iAssignDataPhase->CancelServicePhase();
        }

    if ( iPrepareAssignPhase )
        {
        iPrepareAssignPhase->CancelServicePhase();
        }

    if ( iSelectPropertyPhase )
        {
        iSelectPropertyPhase->CancelServicePhase();
        }

    if ( iFetchPhase )
        {
        iFetchPhase->CancelServicePhase();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::AcceptDelayedContactsL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::DenyDelayedContactsL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::ExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::ExitServiceL( TInt aExitCommandId )
    {
    if ( iAssignDataPhase )
        {
        iAssignDataPhase->RequestCancelL( aExitCommandId );
        }

    if ( iPrepareAssignPhase )
        {
        iPrepareAssignPhase->RequestCancelL( aExitCommandId );
        }

    if ( iSelectPropertyPhase )
        {
        iSelectPropertyPhase->RequestCancelL( aExitCommandId );
        }

    if ( iFetchPhase )
        {
        iFetchPhase->RequestCancelL( aExitCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::ServiceResults
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::ServiceResults(TServiceResults* aResults) const
    {
    if ( iAssignDataPhase )
        {
        aResults->iLinkArray = iAssignDataPhase->Results();
        aResults->iExtraData = iAssignDataPhase->ExtraResultData();
        aResults->iFieldContent = iAssignDataPhase->FieldContent();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::NextPhase
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::NextPhase
        ( MPbk2ServicePhase& aPhase )
    {
    TInt err = KErrNone;

    if ( &aPhase == iFetchPhase )
        {
        MVPbkContactLinkArray* fetchResults = iFetchPhase->Results();
        __ASSERT_DEBUG( fetchResults && fetchResults->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchSelectMultiPropertyPhaseL( fetchResults ) );
        }
    else if ( &aPhase == iSelectPropertyPhase )
        {
        MVPbkContactLinkArray* selectPropertyResults
            = iSelectPropertyPhase->Results();
        __ASSERT_DEBUG( selectPropertyResults &&
            selectPropertyResults->Count() > 0,
                Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchPrepareMultiAssignPhaseL( selectPropertyResults ) );
        }
    else if ( &aPhase == iPrepareAssignPhase )
        {
        MVPbkContactLinkArray* prepareResults
            = iPrepareAssignPhase->Results();
        __ASSERT_DEBUG( prepareResults &&
            prepareResults->Count() > 0,
                Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchAssignPhaseL( prepareResults ) );
        }
    else if ( &aPhase == iAssignDataPhase )
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
// CPbk2MultiContactDataAssigner::PreviousPhase
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::PreviousPhase
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    // Not supported in multiple assign
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::PhaseCanceled
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::PhaseCanceled
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceCanceled();
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::PhaseAborted
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::PhaseAborted
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceAborted();
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::PhaseError
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::PhaseError
        ( MPbk2ServicePhase& /*aPhase*/, TInt aErrorCode )
    {
    iObserver.ServiceError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::PhaseOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2MultiContactDataAssigner::PhaseOkToExit
        ( MPbk2ServicePhase& /*aPhase*/, TInt aCommandId )
    {
    // Deny exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iObserver.CompleteExitMessage( aCommandId );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::PhaseAccept
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::PhaseAccept
        ( MPbk2ServicePhase& /*aPhase*/, const TDesC8& /*aMarkedEntries*/,
          const TDesC8& /*aLinkData*/ )
    {
    // Not supported in assign
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    aStartupMonitor.DeregisterEvents( *this );
    if ( iPreselectedContacts && iPreselectedContacts->Count() > 0 )
        {
        if ( iStoreManager->ContactsAvailableL( *iPreselectedContacts ) )
            {
            LaunchSelectMultiPropertyPhaseL( iPreselectedContacts );
            }
        else
            {
            iObserver.ServiceError( KErrNotSupported );
            }
        }
    else
        {
        LaunchFetchPhaseL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::StartupCanceled
// --------------------------------------------------------------------------
//
void  CPbk2MultiContactDataAssigner::StartupCanceled( TInt aErrorCode )
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
// CPbk2MultiContactDataAssigner::LaunchFetchPhaseL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::LaunchFetchPhaseL()
    {
    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_MULTIPLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbk2FetchDlg::EFetchMultiple;

    delete iFetchPhase;
    iFetchPhase = NULL;
    iFetchPhase = CPbk2ContactFetchPhase::NewL
        ( *this, *iStoreManager, params, ETrue,
          MPbk2FetchDlgObserver::EFetchYes, iContactViewFilter,
          iUseDeviceConfig, iStoreUris );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout to fetch's layout
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );

    iFetchPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::LaunchSelectMultiPropertyPhaseL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::LaunchSelectMultiPropertyPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iSelectPropertyPhase;
    iSelectPropertyPhase = NULL;

    // Prepare parameters by reseting them
    delete iSelectFieldProperty;
    iSelectFieldProperty = NULL;

    iSelectPropertyPhase = CPbk2SelectMultiplePropertyPhase::NewL
        ( *this, aContactLinks, *iAddressSelectFilterBuffer,
          iSelectFieldProperty );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout back to regular layout after fetch
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutEmpty );

    iSelectPropertyPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::LaunchPrepareMultiAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::LaunchPrepareMultiAssignPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iPrepareAssignPhase;
    iPrepareAssignPhase = NULL;

    iPrepareAssignPhase = CPbk2PrepareMultipleAssignPhase::NewL
        ( *this, aContactLinks, iSelectFieldProperty,
          iInformationNoteFlags );

    iPrepareAssignPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2MultiContactDataAssigner::LaunchAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2MultiContactDataAssigner::LaunchAssignPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iAssignDataPhase;
    iAssignDataPhase = NULL;

    iAssignDataPhase = CPbk2MultiAssignDataPhase::NewL
        ( *this, aContactLinks, iSelectFieldProperty,
          iDataBuffer, iMimeType, iInformationNoteFlags );

    // In case of the assign phase it is important to delete the
    // previous phases so that exit and cancel methods work correctly
    delete iPrepareAssignPhase;
    iPrepareAssignPhase = NULL;
    delete iSelectPropertyPhase;
    iSelectPropertyPhase = NULL;
    delete iFetchPhase;
    iFetchPhase = NULL;

    iAssignDataPhase->LaunchServicePhaseL();
    }

// End of File
