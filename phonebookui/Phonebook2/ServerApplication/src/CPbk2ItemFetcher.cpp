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
* Description:  Phonebook 2 application server item fetcher.
*
*/


#include "CPbk2ItemFetcher.h"

// Phonebook 2
#include "MPbk2UiServiceObserver.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppAppUi.h"
#include "TPbk2TitlePaneOperator.h"
#include "CPbk2ServerAppStoreManager.h"
#include "CPbk2ContactFetchPhase.h"
#include "CPbk2AddressSelectPhase.h"
#include <CPbk2StoreManager.h>
#include <Pbk2UIControls.rsg>
#include <TPbk2AddressSelectParams.h>
#include <Pbk2IPCPackage.h>
#include <MPbk2ApplicationServices.h>
#include "CPbk2CommAddressSelectPhase.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <TVPbkContactStoreUriPtr.h>

// System includes
#include <avkon.rsg>
#include <spsettings.h>
#include <spproperty.h>

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

struct TMap
    {
    TAiwCommAddressSelectType aiwType;
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selectorType;
    };

const TMap KAiwMapTable[] = {
    { EAiwCommEmpty,            VPbkFieldTypeSelectorFactory::EEmptySelector },
    { EAiwCommVoiceCall,        VPbkFieldTypeSelectorFactory::EVoiceCallSelector },
    { EAiwCommUniEditor,        VPbkFieldTypeSelectorFactory::EUniEditorSelector },
    { EAiwCommEmailEditor,      VPbkFieldTypeSelectorFactory::EEmailEditorSelector },
    { EAiwCommInstantMessaging, VPbkFieldTypeSelectorFactory::EInstantMessagingSelector },
    { EAiwCommVOIPCall,         VPbkFieldTypeSelectorFactory::EVOIPCallSelector },
    { EAiwCommURL,              VPbkFieldTypeSelectorFactory::EURLSelector },
    { EAiwCommVideoCall,        VPbkFieldTypeSelectorFactory::EVideoCallSelector }
};
const TInt KAiwMapTableLength = sizeof( KAiwMapTable ) / sizeof( TMap ); 

VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
    MapToCommSelectorType( TAiwCommAddressSelectType aAiwType )
    {
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selector =
        VPbkFieldTypeSelectorFactory::EEmptySelector;
    for( TInt i = 0; i < KAiwMapTableLength; i++ )
        {
        if( aAiwType == KAiwMapTable[i].aiwType)
            {
            selector = KAiwMapTable[i].selectorType;
            break;
            }
        }
    
    return selector;
    }

/**
 * Builds contact view filter based on selected address select type.
 *
 * @param aType                 Address select type.
 * @param aFieldTypeList        Master field type list.
 * @param aShowEmailInSendMsg   Indicates whether email fields are valid
 *                              for message sending purposes (UniEditor).
 * @return  Contact view filter.
 */
CVPbkFieldTypeSelector* BuildViewFilterL(
        TAiwAddressSelectType aType,
        const MVPbkFieldTypeList& aFieldTypeList,
        TBool aShowEmailInSendMsg )
    {
    CVPbkFieldTypeSelector* selector = NULL;
    
    switch( aType )
        {
        case EAiwMMSSelect:
            {
            // Do not use VPbkFieldTypeSelectorFactory's EUniEditorSelector
            // filter here, as it's performance is not good.
            // For view filtering needs, we need to use a fast filter.
            if ( aShowEmailInSendMsg )
                {
                selector = VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL
                    ( VPbkFieldTypeSelectorFactory::EMmsAddressSelector,
                    aFieldTypeList );
                }
            else
                {
                selector = VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL
                    ( VPbkFieldTypeSelectorFactory::EPhoneNumberSelector,
                    aFieldTypeList );
                }
            break;
            }
        case EAiwEMailSelect:
            {
            selector = VPbkFieldTypeSelectorFactory::
                BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EEmailEditorSelector,
                    aFieldTypeList );
            break;
            }        
        case EAiwVideoNumberSelect:
            {
            selector = VPbkFieldTypeSelectorFactory::
                BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVideoCallSelector,
                    aFieldTypeList );
            break;
            }
        case EAiwVOIPSelect:
            {
            selector = VPbkFieldTypeSelectorFactory::
                BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    aFieldTypeList );
            break;
            }
        default:
            {
            break;
            }
        }
    
    return selector;
    }

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicLogicPhaseCompletion = 1
    };

void Panic( TPanicCode aReason )
    {
    _LIT( KPanicText, "CPbk2ItemFetcher" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ItemFetcher::CPbk2ItemFetcher
// --------------------------------------------------------------------------
//
CPbk2ItemFetcher::CPbk2ItemFetcher
        ( MPbk2UiServiceObserver& aObserver,
          TPbk2FetchType aFetchType ) :
            iObserver( aObserver ),
            iFetchType( aFetchType )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::CPbk2ItemFetcher()") );
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::~CPbk2ItemFetcher
// --------------------------------------------------------------------------
//
CPbk2ItemFetcher::~CPbk2ItemFetcher()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::~CPbk2ItemFetcher()") );

    delete iStoreUris;
    delete iContactFetchStoreUris;
    delete iPreselectedContacts;
    delete iContactViewFilter;
    iDefaultPriorities.Close();
    delete iStoreManager;
    delete iFetchPhase;
    delete iAddressSelectPhase;
    delete iCommAddressSelectPhase;
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::NewL
// --------------------------------------------------------------------------
//
CPbk2ItemFetcher* CPbk2ItemFetcher::NewL
        ( const RMessage2& aFetchCompleteMessage,
          MPbk2UiServiceObserver& aObserver,
          TPbk2FetchType aFetchType )
    {
    CPbk2ItemFetcher* self =
        new ( ELeave ) CPbk2ItemFetcher( aObserver, aFetchType );
    CleanupStack::PushL( self );
    self->ConstructL( aFetchCompleteMessage );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ItemFetcher::ConstructL( const RMessage2& aMessage )
    {
    // Retrieve data from the client-server message
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>(*CEikonEnv::Static()->EikAppUi());

    TPbk2ServerMessageDataRetriever dataRetriever;
    TUint flags = dataRetriever.FetchFlagsL( aMessage );

    // Set title pane
    SetTitlePaneL( dataRetriever, flags, aMessage );

    iPreselectedContacts = dataRetriever.GetPreselectedContactLinksL
        ( aMessage, appUi.ApplicationServices().ContactManager() );
    iStoreUris = dataRetriever.GetContactStoreUriArrayL
        ( aMessage, appUi.ApplicationServices().StoreConfiguration(),
          iPreselectedContacts, iUseDeviceConfig );
    
    // Contact fetch store uris, not including xSP stores
    iContactFetchStoreUris = CVPbkContactStoreUriArray::NewL();
    iContactFetchStoreUris->AppendAllL(*iStoreUris); 
    
    TAiwCommAddressSelectType selectorType = 
        dataRetriever.GetCommAddressSelectTypeL( aMessage );
    iCommAddressSelectType = MapToCommSelectorType( selectorType );
    
    iAddressSelectType = dataRetriever.GetAddressSelectTypeL( aMessage );
    
    // Create contact filters
    if ( iCommAddressSelectType != VPbkFieldTypeSelectorFactory::EEmptySelector )
        {
        iContactViewFilter = VPbkFieldTypeSelectorFactory::
        	BuildContactActionTypeSelectorL( iCommAddressSelectType,
                appUi.ApplicationServices().ContactManager().FieldTypes() );
        }
    // Phonebook specific filters take prefference over the
    // filter specified by client
    else
        {
        iContactViewFilter = BuildViewFilterL( iAddressSelectType,
                appUi.ApplicationServices().ContactManager().FieldTypes(),
                appUi.ApplicationServices().LocallyVariatedFeatureEnabled
                ( EVPbkLVShowEmailInSendMsg ) );
        
        // Filter specified by client
        if ( iContactViewFilter == NULL )
            {
            iContactViewFilter = dataRetriever.GetContactViewFilterL
                ( aMessage, appUi.ApplicationServices().ContactManager().
                  FieldTypes() );
            }
        }

    iFetchResId = dataRetriever.GetFetchDialogResourceL( aMessage );

    iStoreManager = CPbk2ServerAppStoreManager::NewL
        ( appUi.StoreManager(),
          appUi.ApplicationServices().ContactManager() );

    if ( iCommAddressSelectType != VPbkFieldTypeSelectorFactory::EEmptySelector )
        {
        GetDefaultPriorities( flags );
        }
    else
        {
        iDefaultPriorities = dataRetriever.FetchDefaultPrioritiesL( aMessage );
        }
    
    if ( iCommAddressSelectType != VPbkFieldTypeSelectorFactory::EEmptySelector )
        {
        // add also xSP stores to the store list
        AddXSPStoresL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::LaunchServiceL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::LaunchServiceL()") );

    iStoreManager->LoadAndOpenContactStoresL( *iStoreUris, *this, EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::CancelService
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::CancelService()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::CancelService()") );

    if ( iFetchPhase )
        {
        iFetchPhase->CancelServicePhase();
        }

    if ( iAddressSelectPhase )
        {
        iAddressSelectPhase->CancelServicePhase();
        }

   if ( iCommAddressSelectPhase )
        {
        iCommAddressSelectPhase->CancelServicePhase();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::AcceptDelayedContactsL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::AcceptDelayedContactsL
        ( const TDesC8& aContactLinkBuffer )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::AcceptDelayedContactsL()") );

    if ( iAddressSelectPhase )
        {
        iAddressSelectPhase->AcceptDelayedL( aContactLinkBuffer );
        }

    if ( iCommAddressSelectPhase )
        {
        iCommAddressSelectPhase->AcceptDelayedL( aContactLinkBuffer );
        }

    if ( iFetchPhase )
        {
        iFetchPhase->AcceptDelayedL( aContactLinkBuffer );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::ExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::ExitServiceL( TInt aExitCommandId )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::ExitServiceL(%d)"), aExitCommandId );

    if ( iAddressSelectPhase )
        {
        iAddressSelectPhase->RequestCancelL( aExitCommandId );

        // Report fetch aborting and close application
        iObserver.ServiceAborted();
        }

    if ( iCommAddressSelectPhase )
        {
        iCommAddressSelectPhase->RequestCancelL( aExitCommandId );

        // Report fetch aborting and close application
        iObserver.ServiceAborted();
        }

    if ( iFetchPhase )
        {
        iFetchPhase->RequestCancelL( aExitCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::ServiceResults
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::ServiceResults(TServiceResults* aResults) const
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::ServiceResults()") );

    if ( iAddressSelectPhase )
        {
        aResults->iLinkArray = iAddressSelectPhase->Results();
        aResults->iExtraData = iAddressSelectPhase->ExtraResultData();
        aResults->iFieldContent = NULL;
        }
        
    if ( iCommAddressSelectPhase )
        {
        aResults->iLinkArray = iCommAddressSelectPhase->Results();
        aResults->iExtraData = iCommAddressSelectPhase->ExtraResultData();
        aResults->iFieldContent = iCommAddressSelectPhase->FieldContent();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::NextPhase
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::NextPhase( MPbk2ServicePhase& aPhase )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::NextPhase()") );

    if ( &aPhase == iFetchPhase )
        {
        MVPbkContactLinkArray* fetchResult = iFetchPhase->Results();
        __ASSERT_DEBUG( fetchResult && fetchResult->Count() > 0,
            Panic( EPanicLogicPhaseCompletion ) );

        TRAPD( err, LaunchAddressSelectPhaseL( *fetchResult ) );
        if ( err != KErrNone )
            {
            iObserver.ServiceError( err );
            }
        }
    else if ( &aPhase == iAddressSelectPhase || 
              &aPhase == iCommAddressSelectPhase )
        {
        // Address select was the last phase
        iObserver.ServiceComplete();
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPanicLogicPhaseCompletion ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PreviousPhase
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::PreviousPhase( MPbk2ServicePhase& /*aPhase*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PhaseCanceled
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::PhaseCanceled( MPbk2ServicePhase& aPhase )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::PhaseCanceled()") );

    // We cannot cancel the phase completely if the cancellation
    // was done from address phase and fetch phase dialog still exists and
    // we are dealing with singleitemfetch
    if ( ( ( &aPhase != iAddressSelectPhase ) && 
         ( &aPhase != iCommAddressSelectPhase ) ) || 
         !iFetchPhase ||
         iFetchType == EMultipleItemFetch )
        {
        iObserver.ServiceCanceled();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PhaseAborted
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::PhaseAborted( MPbk2ServicePhase& /*aPhase*/ )
    {
    iObserver.ServiceAborted();
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PhaseError
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::PhaseError
        ( MPbk2ServicePhase& /*aPhase*/, TInt aErrorCode )
    {
    iObserver.ServiceError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PhaseOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2ItemFetcher::PhaseOkToExit
        ( MPbk2ServicePhase& /*aPhase*/, TInt aCommandId )
    {
    // Deny exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iObserver.CompleteExitMessage( aCommandId );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::PhaseAccept
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::PhaseAccept
        ( MPbk2ServicePhase& /*aPhase*/, const TDesC8& aMarkedEntries,
          const TDesC8& aLinkData )
    {
    iObserver.CompleteAcceptMsg( aMarkedEntries, aLinkData );
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::ContactUiReadyL()") );

    aStartupMonitor.DeregisterEvents( *this );

    if ( !iPreselectedContacts || iPreselectedContacts->Count() == 0 )
        {
        LaunchFetchPhaseL();
        }
    else
        {
        if ( iStoreManager->ContactsAvailableL( *iPreselectedContacts ) )
            {
            LaunchAddressSelectPhaseL( *iPreselectedContacts );
            }
        else
            {
            iObserver.ServiceError( KErrNotSupported );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::StartupCanceled
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::StartupCanceled( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::StartupCanceled()"), aErrorCode );

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
// CPbk2ItemFetcher::LaunchFetchPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::LaunchFetchPhaseL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::LaunchFetchPhaseL()") );

    // By default fetch cannot exit
    TBool fetchOkToExit = EFalse;

    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_SINGLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbk2FetchDlg::EFetchSingle;

    if ( iFetchType == ECallItemFetch )
        {
        params.iResId = R_PBK2_CALL_ITEM_FETCH_DLG;
        params.iFlags = CPbk2FetchDlg::EFetchCallItem;
        }
    else if ( iFetchType == EMultipleItemFetch )
        {
        params.iResId = R_PBK2_MULTIPLE_ENTRY_FETCH_DLG;
        params.iFlags = CPbk2FetchDlg::EFetchMultiple;

        // In multiple entry fetch, fetch can exit
        fetchOkToExit = ETrue;
        }

    delete iFetchPhase;
    iFetchPhase = NULL;
    iFetchPhase = CPbk2ContactFetchPhase::NewL
        ( *this, *iStoreManager, params, fetchOkToExit,
          MPbk2FetchDlgObserver::EFetchDelayed, iContactViewFilter,
          iUseDeviceConfig, iContactFetchStoreUris );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout to fetch's layout
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );

    iFetchPhase->LaunchServicePhaseL();
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::LaunchAddressSelectPhaseL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::LaunchAddressSelectPhaseL
        ( MVPbkContactLinkArray& aContactLinks )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2ItemFetcher::LaunchAddressSelectPhaseL()") );

    // By default RSK is 'Back'
    TBool rskBack = ETrue;

    // RSK is cancel in case of multiple item fetch
    // or if there are preselected contacts
    if ( iFetchType == EMultipleItemFetch )
        {
        rskBack = EFalse;
        }
    else if ( iPreselectedContacts && iPreselectedContacts->Count() != 0 )
        {
        rskBack = EFalse;
        }

    delete iAddressSelectPhase;
    iAddressSelectPhase = NULL;
    delete iCommAddressSelectPhase;
    iCommAddressSelectPhase = NULL;
    
    if ( iCommAddressSelectType != VPbkFieldTypeSelectorFactory::EEmptySelector )
        {
        iCommAddressSelectPhase = CPbk2CommAddressSelectPhase::NewL
            ( *this, aContactLinks.At( 0 ), iDefaultPriorities,
              *iContactViewFilter, iCommAddressSelectType, rskBack );

        iCommAddressSelectPhase->LaunchServicePhaseL();
        }
    else
        {
        iAddressSelectPhase = CPbk2AddressSelectPhase::NewL
            ( *this, aContactLinks, iDefaultPriorities,
            iAddressSelectType, rskBack );

        iAddressSelectPhase->LaunchServicePhaseL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::AddXSPStoresL
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::AddXSPStoresL()
    {
    RIdArray ids;
    CleanupClosePushL( ids );
    CSPSettings* settings = CSPSettings::NewL();
    CleanupStack::PushL( settings ); 
    TInt error = settings->FindServiceIdsL( ids );  
    const TInt count = ids.Count();

    for( TInt i = 0; i < count; i++ )
        {
        CSPProperty* property = CSPProperty::NewLC();
        error = settings->FindPropertyL( ids[i], EPropertyContactStoreId,
            *property );
        if( KErrNone == error )
            {
            RBuf uri;
            uri.Create( KSPMaxDesLength );
            CleanupClosePushL( uri );  
            property->GetValue( uri );
            TVPbkContactStoreUriPtr uriPtr( uri );
            iStoreUris->AppendL( uriPtr );
            CleanupStack::PopAndDestroy();  //uri
            }
        CleanupStack::PopAndDestroy();  //property
        }
    CleanupStack::PopAndDestroy( 2 );  //settings, ids
    }

// --------------------------------------------------------------------------
// CPbk2ItemFetcher::GetDefaultPriorities
// --------------------------------------------------------------------------
//
void CPbk2ItemFetcher::GetDefaultPriorities( TUint aFlags )
    {
    if ( !( aFlags & ::EDoNotUseDefaultField ) )
        {
        switch ( iCommAddressSelectType )
            {
            // Errors during Append() operation are ignored, in this case 
            // address select dialog will be shown instead of immediate return
            // of the default field.
            case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypePhoneNumber );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeSms );
                iDefaultPriorities.Append( EVPbkDefaultTypeMms );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeVideoNumber );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeEmail );
                break;    
                }
            case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeVoIP );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EPocSelector:
                {
                iDefaultPriorities.Append( EVPbkDefaultTypePOC );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EURLSelector :
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeOpenLink );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector :
                {
                iDefaultPriorities.Append( EVPbkDefaultTypeChat );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }
// End of File
