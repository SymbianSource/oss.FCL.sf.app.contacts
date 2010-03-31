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
* Description:  Phonebook 2 single contact data assigner.
*
*/


#include "CPbk2SingleContactDataAssigner.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include "MPbk2UiServiceObserver.h"
#include "TPbk2TitlePaneOperator.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppStoreManager.h"
#include "CPbk2SelectSinglePropertyPhase.h"
#include "CPbk2ContactFetchPhase.h"
#include "CPbk2SingleAssignDataPhase.h"
#include "CPbk2SelectCreateNewPropertyPhase.h"
#include "CPbk2PrepareSingleAssignPhase.h"
#include "MPbk2SelectFieldProperty.h"
#include <Pbk2UIControls.rsg>
#include <Pbk2ServerApp.rsg>
#include <CPbk2StoreManager.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ApplicationServices.h>
#include "TPbk2AssignNoteService.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContact.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    ELogicPreselectedContacts,
    EPanicLogicPhaseCompletion
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2SingleContactDataAssigner" );
    User::Panic( KPanicText, aReason );
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
// CPbk2SingleContactDataAssigner::CPbk2SingleContactDataAssigner
// --------------------------------------------------------------------------
//
CPbk2SingleContactDataAssigner::CPbk2SingleContactDataAssigner
        ( MPbk2UiServiceObserver& aObserver ) :
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::~CPbk2SingleContactDataAssigner
// --------------------------------------------------------------------------
//
CPbk2SingleContactDataAssigner::~CPbk2SingleContactDataAssigner()
    {
    delete iSelectPropertyPhase;
    delete iFetchPhase;
    delete iPrepareAssignPhase;
    delete iAssignDataPhase;
    delete iStoreUris;
    delete iDataBuffer;
    delete iAddressSelectFilterBuffer;
    delete iPreselectedContact;
    delete iSelectFieldProperty;
    delete iContactViewFilter;
    delete iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2SingleContactDataAssigner* CPbk2SingleContactDataAssigner::NewL
        ( const RMessage2& aFetchCompleteMessage,
          MPbk2UiServiceObserver& aObserver )
    {
    CPbk2SingleContactDataAssigner* self =
        new(ELeave) CPbk2SingleContactDataAssigner( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFetchCompleteMessage );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::ConstructL( const RMessage2& aMessage )
    {
    // Retrieve data from the client-server message
    TPbk2ServerMessageDataRetriever dataRetriever;
    iAssignFlags = dataRetriever.SingleContactAssignFlagsL( aMessage );

    // Set title pane
    SetTitlePaneL( dataRetriever, iAssignFlags, aMessage );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    iPreselectedContact = dataRetriever.GetPreselectedContactLinksL
        ( aMessage, appUi.ApplicationServices().ContactManager() );
    iStoreUris = dataRetriever.GetContactStoreUriArrayL
        ( aMessage, appUi.ApplicationServices().StoreConfiguration(),
          iPreselectedContact, iUseDeviceConfig );

    // Assert that there aren't multiple preselected contacts
    __ASSERT_DEBUG( ( !iPreselectedContact ||
        iPreselectedContact->Count() <= 1 ),
        Panic( ELogicPreselectedContacts ) );

    iAddressSelectFilterBuffer =
        dataRetriever.GetAddressSelectFilterBufferL( aMessage );
    iDataBuffer = dataRetriever.GetDataBufferL( aMessage );
    iMimeType = dataRetriever.GetMimeTypeL( aMessage );
    iFieldIndex = dataRetriever.GetFocusIndexL( aMessage );

    iEditorHelpContext = dataRetriever.GetEditorHelpContextL( aMessage );

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
// CPbk2SingleContactDataAssigner::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchServiceL()
    {
    // Open stores
    iStoreManager->LoadAndOpenContactStoresL( *iStoreUris, *this, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::CancelService
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::CancelService()
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
// CPbk2SingleContactDataAssigner::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::AcceptDelayedContactsL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::ExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::ExitServiceL( TInt aExitCommandId )
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
// CPbk2SingleContactDataAssigner::ServiceResults
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::ServiceResults(TServiceResults* aResults) const
    {
    if ( iAssignDataPhase )
        {
        aResults->iLinkArray = iAssignDataPhase->Results();
        aResults->iExtraData = iAssignDataPhase->ExtraResultData();
        aResults->iFieldContent = iAssignDataPhase->FieldContent();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::NextPhase
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::NextPhase
        ( MPbk2ServicePhase& aPhase )
    {
    TInt err = KErrNone;

    if ( &aPhase == iFetchPhase )
        {
        MVPbkContactLinkArray* fetchResult = iFetchPhase->Results();
        __ASSERT_DEBUG( fetchResult && fetchResult->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchSelectSinglePropertyPhaseL( fetchResult ) );
        }
    else if ( &aPhase == iSelectPropertyPhase )
        {
        TRAP( err, LaunchPrepareSingleAssignPhaseL
            ( iSelectPropertyPhase->TakeStoreContact() ) );
        }
    else if ( &aPhase == iPrepareAssignPhase )
        {
        TRAP( err, LaunchAssignPhaseL
            ( iPrepareAssignPhase->TakeStoreContact() ) );
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
// CPbk2SingleContactDataAssigner::PreviousPhase
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::PreviousPhase
        ( MPbk2ServicePhase& aPhase )
    {
    TInt err = KErrNone;

    if ( &aPhase == iPrepareAssignPhase )
        {
        MVPbkContactLinkArray* resultLinks = aPhase.Results();
        __ASSERT_DEBUG( resultLinks && resultLinks->Count() == 1,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAP( err, LaunchSelectSinglePropertyPhaseL( resultLinks ) );
        }

    if ( err != KErrNone )
        {
        iObserver.ServiceError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::PhaseCanceled
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::PhaseCanceled
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceCanceled();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::PhaseAborted
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::PhaseAborted
        ( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceAborted();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::PhaseError
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::PhaseError
        ( MPbk2ServicePhase& /*aPhase*/, TInt aErrorCode )
    {
    iObserver.ServiceError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::PhaseOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2SingleContactDataAssigner::PhaseOkToExit
        ( MPbk2ServicePhase& /*aPhase*/, TInt aCommandId )
    {
    // Deny exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iObserver.CompleteExitMessage( aCommandId );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::PhaseAccept
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::PhaseAccept
        ( MPbk2ServicePhase& /*aPhase*/, const TDesC8& /*aMarkedEntries*/,
          const TDesC8& /*aLinkData*/ )
    {
    // Not supported in assign
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    aStartupMonitor.DeregisterEvents( *this );
 
   if ( iAssignFlags & ECreateNewContact )
        {        
        CVPbkContactStoreUriArray* validStores =
            iStoreManager->CurrentlyValidStoresL();
        CleanupStack::PushL( validStores );

        CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
            ( *CEikonEnv::Static()->EikAppUi() );

        // Saving store URI
        TVPbkContactStoreUriPtr savingStoreUri =
            appUi.ApplicationServices().StoreConfiguration().
                DefaultSavingStoreL();

        TPbk2AssignNoteService noteService;
        if ( validStores->IsIncluded ( savingStoreUri ) )
            {
            LaunchSelectCreateNewPropertyPhaseL( savingStoreUri );
            }
        else if ( noteService.ShowCreateNewToPhoneQueryL() )
            {
            savingStoreUri.Set( 
                TVPbkContactStoreUriPtr( 
                    VPbkContactStoreUris::DefaultCntDbUri() ) );
            LaunchSelectCreateNewPropertyPhaseL( savingStoreUri );
            }
        else
            {
            // Canceled
            iObserver.ServiceCanceled();
            }

        CleanupStack::PopAndDestroy( validStores );
        }
    else
        {
        if ( iPreselectedContact && iPreselectedContact->Count() == 1 )
            {
            if ( iStoreManager->ContactsAvailableL( *iPreselectedContact ) )
                {
                LaunchSelectSinglePropertyPhaseL( iPreselectedContact );
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
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::StartupCanceled
// --------------------------------------------------------------------------
//
void  CPbk2SingleContactDataAssigner::StartupCanceled( TInt aErrorCode )
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
// CPbk2SingleContactDataAssigner::LaunchFetchPhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchFetchPhaseL()
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
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout to fetch's layout
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );

    iFetchPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::LaunchSelectSinglePropertyPhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchSelectSinglePropertyPhaseL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    delete iSelectPropertyPhase;
    iSelectPropertyPhase = NULL;

    // Prepare parameters by reseting them
    delete iSelectFieldProperty;
    iSelectFieldProperty = NULL;
    iSelectFieldResult = KErrNotReady;

    iSelectPropertyPhase = CPbk2SelectSinglePropertyPhase::NewL
        ( *this, aContactLinks, iAddressSelectFilterBuffer,
          iSelectFieldProperty, iSelectFieldResult );

    iRelocationFlags = CPbk2ContactRelocator::EPbk2RelocatorExistingContact;

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout back to regular
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutEmpty );

    iSelectPropertyPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::LaunchSelectCreateNewPropertyPhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchSelectCreateNewPropertyPhaseL(
        TVPbkContactStoreUriPtr  savingStoreUri )
    {
    delete iSelectPropertyPhase;
    iSelectPropertyPhase = NULL;

    // Prepare parameters by reseting them
    delete iSelectFieldProperty;
    iSelectFieldProperty = NULL;
    iSelectFieldResult = KErrNotReady;

    iSelectPropertyPhase = CPbk2SelectCreateNewPropertyPhase::NewL
        ( savingStoreUri, *this, iAddressSelectFilterBuffer,  
          iSelectFieldProperty, iSelectFieldResult, iInformationNoteFlags );

    iRelocationFlags = CPbk2ContactRelocator::EPbk2RelocatorNewContact;

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout back to regular
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutEmpty );

    iSelectPropertyPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::LaunchPrepareSingleAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchPrepareSingleAssignPhaseL
        ( MVPbkStoreContact* aStoreContact )
    {
    delete iPrepareAssignPhase;
    iPrepareAssignPhase = NULL;

    CleanupStack::PushL( aStoreContact );
    iPrepareAssignPhase = CPbk2PrepareSingleAssignPhase::NewL
        ( *this, aStoreContact, iSelectFieldProperty,
          iSelectFieldResult, iFieldIndex, iInformationNoteFlags,
          iRelocationFlags );
    CleanupStack::Pop( aStoreContact );

    iPrepareAssignPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2SingleContactDataAssigner::LaunchAssignPhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleContactDataAssigner::LaunchAssignPhaseL
        ( MVPbkStoreContact* aStoreContact )
    {
    delete iAssignDataPhase;
    iAssignDataPhase = NULL;

    CleanupStack::PushL( aStoreContact );
    iAssignDataPhase = CPbk2SingleAssignDataPhase::NewL
        ( *this, aStoreContact, iSelectFieldProperty,
          iFieldIndex, iDataBuffer, iMimeType,
          iEditorHelpContext, iAssignFlags, iInformationNoteFlags );
    CleanupStack::Pop( aStoreContact );

    // In case of the assign phase it is important to delete the
    // previous phases so that exit and cancel methods work correctly
    delete iPrepareAssignPhase;
    iPrepareAssignPhase = NULL;
    delete iSelectPropertyPhase;
    iSelectPropertyPhase = NULL;
    delete iFetchPhase;
    iFetchPhase = NULL;

    // If need to open the editor, then change StatuspaneLayout to 
    // CPbk2ServerAppAppUi::EStatusPaneLayoutUsual
    if (!(iAssignFlags & EDoNotOpenEditor))
        {
        CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
            ( *CEikonEnv::Static()->EikAppUi() );
        appUi.ChangeStatuspaneLayoutL(CPbk2ServerAppAppUi::EStatusPaneLayoutUsual);
        }

    iAssignDataPhase->LaunchServicePhaseL();
    }

// End of File
