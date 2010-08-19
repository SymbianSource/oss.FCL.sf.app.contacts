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
* Description:  Phonebook 2 application server entry fetcher.
*
*/


#include "CPbk2EntryFetcher.h"

// Phonebook 2
#include "MPbk2UiServiceObserver.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppAppUi.h"
#include "TPbk2TitlePaneOperator.h"
#include "CPbk2ServerAppStoreManager.h"
#include "MPbk2ServicePhase.h"
#include "CPbk2ContactFetchPhase.h"
#include <CPbk2StoreManager.h>
#include <Pbk2IPCPackage.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactManager.h>

// System includes
#include <AiwContactSelectionDataTypes.h>

// Debug
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Sets title pane.
 *
 * @param aDataRetriever    Server message data retriever.
 * @param aFlags            Fetch flags.
 * @param aMessage          Server message.
 */
void SetTitlePaneL
        ( TPbk2ServerMessageDataRetriever& aDataRetriever, TUint aFlags,
          const RMessage2& aMessage )
    {
    HBufC* titlePaneText = NULL;
    if ( !( aFlags & ::EUseProviderTitle ) )
        {
        titlePaneText = aDataRetriever.GetTitlePaneTextL( aMessage );
        }
    TPbk2TitlePaneOperator titlePaneOperator;
    titlePaneOperator.SetTitlePaneL( titlePaneText ); // takes ownership
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::CPbk2EntryFetcher
// --------------------------------------------------------------------------
//
CPbk2EntryFetcher::CPbk2EntryFetcher
        ( MPbk2UiServiceObserver& aObserver,
            TPbk2FetchType aFetchType ) :
            iObserver( aObserver ),
            iFetchType( aFetchType )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::CPbk2EntryFetcher()") );
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::~CPbk2EntryFetcher
// --------------------------------------------------------------------------
//
CPbk2EntryFetcher::~CPbk2EntryFetcher()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::~CPbk2EntryFetcher()") );

    delete iStoreUris;
    delete iPreselectedContacts;
    delete iContactViewFilter;
    delete iStoreManager;
    delete iFetchPhase;
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2EntryFetcher::ConstructL( const RMessage2& aMessage )
    {
    // Retrieve data from the client-server message
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    TPbk2ServerMessageDataRetriever dataRetriever;
    TUint flags = dataRetriever.FetchFlagsL( aMessage );

    // Set title pane
    SetTitlePaneL( dataRetriever, flags, aMessage );

    iPreselectedContacts = dataRetriever.GetPreselectedContactLinksL
        ( aMessage, appUi.ApplicationServices().ContactManager() );
    iStoreUris = dataRetriever.GetContactStoreUriArrayL
        ( aMessage, appUi.ApplicationServices().StoreConfiguration(),
          iPreselectedContacts, iUseDeviceConfig );
    iContactViewFilter = dataRetriever.GetContactViewFilterL
        ( aMessage, appUi.ApplicationServices().ContactManager().
            FieldTypes() );
    iFetchResId = dataRetriever.GetFetchDialogResourceL( aMessage );

    iStoreManager = CPbk2ServerAppStoreManager::NewL
        ( appUi.StoreManager(),
          appUi.ApplicationServices().ContactManager() );
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::NewL
// --------------------------------------------------------------------------
//
CPbk2EntryFetcher* CPbk2EntryFetcher::NewL(
        const RMessage2& aFetchCompleteMessage,
        MPbk2UiServiceObserver& aObserver,
        TPbk2FetchType aFetchType )
    {
    CPbk2EntryFetcher* self =
        new ( ELeave ) CPbk2EntryFetcher( aObserver, aFetchType );
    CleanupStack::PushL( self );
    self->ConstructL( aFetchCompleteMessage );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::LaunchServiceL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::LaunchServiceL()") );

    iStoreManager->LoadAndOpenContactStoresL( *iStoreUris, *this, EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::CancelService
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::CancelService()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::CancelService()") );

    if ( iFetchPhase )
        {
        iFetchPhase->CancelServicePhase();
        }
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::AcceptDelayedContactsL
        ( const TDesC8& aContactLinkBuffer )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::AcceptDelayedContactsL()") );

    if ( iFetchPhase )
        {
        iFetchPhase->AcceptDelayedL( aContactLinkBuffer );
        }
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::DenyDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::DenyDelayedContactsL
        ( const TDesC8& aContactLinkBuffer )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::DenyDelayedContactsL()") );

    if ( iFetchPhase )
        {
        iFetchPhase->DenyDelayedL( aContactLinkBuffer );
        }
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::ExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::ExitServiceL( TInt aExitCommandId )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::ExitServiceL(%d)"), aExitCommandId );

    if ( iFetchPhase )
        {
        iFetchPhase->RequestCancelL( aExitCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::ServiceResults
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::ServiceResults(TServiceResults* aResults) const
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::ServiceResults()") );

    if ( iFetchPhase )
        {
        aResults->iLinkArray = iFetchPhase->Results();
        aResults->iExtraData = iFetchPhase->ExtraResultData();
        aResults->iFieldContent = iFetchPhase->FieldContent();
        }
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::NextPhase
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::NextPhase( MPbk2ServicePhase& /*aPhase*/ )
    {
    // There is no other phase than fetch, we are done
    iObserver.ServiceComplete();
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PreviousPhase
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::PreviousPhase( MPbk2ServicePhase& /*aPhase*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PhaseCanceled
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::PhaseCanceled( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceCanceled();
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PhaseAborted
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::PhaseAborted( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceAborted();
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PhaseError
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::PhaseError
        ( MPbk2ServicePhase& /*aPhase*/, TInt aErrorCode )
    {
    iObserver.ServiceError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PhaseOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2EntryFetcher::PhaseOkToExit
        ( MPbk2ServicePhase& /*aPhase*/, TInt aCommandId )
    {
    // Deny exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iObserver.CompleteExitMessage( aCommandId );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::PhaseAccept
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::PhaseAccept
        ( MPbk2ServicePhase& /*aPhase*/, const TDesC8& aMarkedEntries,
          const TDesC8& aLinkData )
    {
    iObserver.CompleteAcceptMsg( aMarkedEntries, aLinkData );
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::ContactUiReadyL()") );

    aStartupMonitor.DeregisterEvents( *this );

    LaunchFetchPhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetcher::StartupCanceled
// --------------------------------------------------------------------------
//
void  CPbk2EntryFetcher::StartupCanceled( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2EntryFetcher::StartupCanceled(%d)"), aErrorCode );

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
// CPbk2EntryFetcher::LaunchFetchPhaseL
// --------------------------------------------------------------------------
//
void CPbk2EntryFetcher::LaunchFetchPhaseL()
    {
    CPbk2FetchDlg::TParams params;

    params.iResId = iFetchResId;
    params.iMarkedEntries = iPreselectedContacts;
    params.iFlags = CPbk2FetchDlg::EFetchSingle;
    if ( iFetchType == EMultipleEntryFetch )
        {
        params.iFlags = CPbk2FetchDlg::EFetchMultiple;
        }

    delete iFetchPhase;
    iFetchPhase = NULL;
    iFetchPhase = CPbk2ContactFetchPhase::NewL
        ( *this, *iStoreManager, params, ETrue,
          MPbk2FetchDlgObserver::EFetchDelayed, iContactViewFilter,
          iUseDeviceConfig, iStoreUris );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout to fetch's layout
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );

    iFetchPhase->LaunchServicePhaseL();
    }

// End of File
