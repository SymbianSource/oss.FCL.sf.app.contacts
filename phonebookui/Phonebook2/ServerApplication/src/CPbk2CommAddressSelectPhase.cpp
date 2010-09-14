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
* Description:  Phonebook 2 server app address select phase.
*
*/

#include "CPbk2CommAddressSelectPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2KeyEventDealer.h"
#include <MPbk2DialogEliminator.h>
#include <CPbk2AddressSelect.h>
#include <TPbk2AddressSelectParams.h>
#include <MPbk2ApplicationServices.h>
#include <CPbk2PresenceIconInfo.h>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>
#include <CPbk2StoreManager.h> 

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <CVPbkxSPContacts.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldFilter.h>
#include <vpbkeng.rsg>

// System includes
#include <barsread.h>
#include <avkon.rsg>
#include <contactpresencefactory.h>
#include <mcontactpresence.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlists.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_LaunchServicePhaseL = 1,
    EPanicPreCond_ContactOperationComplete,
    EPanicPreCond_RetrieveContactL,
    EPanicPreCond_DoSelectAddressesL
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2CommAddressSelectPhase");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// Separator between service name and user's id in the service name returned
// in presence icon info.
_LIT( KServiceNameSeparator, ":");

const TInt KMaxXspServiceCount = 5;
/**
 * Copies a link array to another.
 *
 * @param aSourceLinkArray    Link array which is copied
 * @param aTargetLinkArray    Links are copied to this
 */
void CopyContactLinksL( const MVPbkContactLinkArray& aSourceLinkArray,
        CVPbkContactLinkArray& aTargetLinkArray )
    {
    const TInt count = aSourceLinkArray.Count();
    for ( TInt i(0); i < count; ++i )
        {
        const MVPbkContactLink& contactLink = aSourceLinkArray.At(i);
        aTargetLinkArray.AppendL( contactLink.CloneLC() );
        CleanupStack::Pop(); // link
        }
    }

struct TMap
    {
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selector;
    TInt resId;
    };

const TMap KFieldMapTable[] = {
    { VPbkFieldTypeSelectorFactory::EVoiceCallSelector, R_PBK2_CALL_ITEM_NUMBER_SELECT },
    { VPbkFieldTypeSelectorFactory::EUniEditorSelector, R_PBK2_GENERIC_ADDRESS_SELECT },
    { VPbkFieldTypeSelectorFactory::EEmailEditorSelector, R_PBK2_EMAIL_ADDRESS_SELECT },
    { VPbkFieldTypeSelectorFactory::EInstantMessagingSelector, R_PBK2_GENERIC_ADDRESS_SELECT },
    { VPbkFieldTypeSelectorFactory::EVOIPCallSelector, R_PBK2_VOIP_ADDRESS_SELECT },
    { VPbkFieldTypeSelectorFactory::EURLSelector, R_PBK2_URL_ADDRESS_SELECT },
    { VPbkFieldTypeSelectorFactory::EVideoCallSelector, R_PBK2_VIDEO_NUMBER_SELECT },
    { VPbkFieldTypeSelectorFactory::EPocSelector, R_PBK2_POC_ADDRESS_SELECT }
};
const TInt KFieldMapTableLength = sizeof( KFieldMapTable ) / sizeof( TMap );

TInt AddressSelectResourceId
    ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aType )
    {
    TInt resourceId = KErrNotFound;
    for( TInt i = 0; i < KFieldMapTableLength; i++ )
        {
        if( aType == KFieldMapTable[i].selector )
            {
            resourceId = KFieldMapTable[i].resId;
            break;
            }
        }

    return resourceId;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::CPbk2CommAddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2CommAddressSelectPhase::CPbk2CommAddressSelectPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          RVPbkContactFieldDefaultPriorities& aPriorities,
          TBool aRskBack,
          CVPbkFieldTypeSelector& aFieldTypeSelector,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
                aCommSelector  ) :
            iObserver( aObserver ),
            iPriorities( aPriorities ),
            iRskBack( aRskBack ),
            iFieldTypeSelector ( aFieldTypeSelector ),
            iState( EInitialState ),
            iCommMethod( aCommSelector )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::~CPbk2CommAddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2CommAddressSelectPhase::~CPbk2CommAddressSelectPhase()
    {
    if ( iAddressSelectEliminator != NULL )
        {
        iAddressSelectEliminator->ForceExit();
        }
    delete iStoreContact;
    delete iRetrieveOperation;
    delete iContactLink;
    delete iResults;
    delete iDealer;
    delete ixSPManager;
    delete ixSPContactOperation;
    delete ixSPContactsArray;
    ixSPStoreContactsArray.ResetAndDestroy();
    delete iFieldContent;
    if( iContactPresence )
        {
        iContactPresence->Close();
        iContactPresence = NULL;
        }
    iPresenceIconArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CommAddressSelectPhase::ConstructL
    ( const MVPbkContactLink& aContactLink )
    {
    iEikenv = CEikonEnv::Static();

    // Copy contact link provided by the client
    MVPbkContactLink* link = aContactLink.CloneLC();
    User::LeaveIfNull( link );
    CleanupStack::Pop(); //link
    iContactLink = link;

    iDealer = CPbk2KeyEventDealer::NewL( *this );

    // Map TVPbkFieldTypeSelector to resource id
    iResourceId = AddressSelectResourceId( iCommMethod );
    if ( iResourceId == KErrNotFound )
        {
        User::LeaveIfError( KErrArgument );
        }

    // Create xSP contacts manager
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>(*CEikonEnv::Static()->EikAppUi());

    ixSPManager = CVPbkxSPContacts::NewL(
         appUi.ApplicationServices().ContactManager() );
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2CommAddressSelectPhase* CPbk2CommAddressSelectPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          const MVPbkContactLink& aContactLink,
          RVPbkContactFieldDefaultPriorities& aPriorities,
          CVPbkFieldTypeSelector& aFieldTypeSelector,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
            aCommSelector,
          TBool aRskBack )
    {
    CPbk2CommAddressSelectPhase* self = new ( ELeave )
        CPbk2CommAddressSelectPhase( aObserver, aPriorities, aRskBack,
            aFieldTypeSelector, aCommSelector );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLink );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::LaunchServicePhaseL()
    {
    __ASSERT_DEBUG( iState == EInitialState, 
            Panic( EPanicPreCond_LaunchServicePhaseL ) );
    
    RetrieveContactL();
    
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::CancelServicePhase()
    {
    if ( iAddressSelectEliminator != NULL )
        {
        iAddressSelectEliminator->ForceExit();
        }

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    if ( iContactPresence )
    	{
		iContactPresence->CancelAll();
    	}

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::RequestCancelL( TInt aExitCommandId )
    {
    if ( iAddressSelectEliminator != NULL )
        {
        iAddressSelectEliminator->RequestExitL( aExitCommandId );
        }

    // Withdraw our key event agent so that it does not react to
    // app shutter's escape key event simulation
    delete iDealer;
    iDealer = NULL;

   if ( aExitCommandId == EEikBidCancel )
        {
        iObserver.PhaseAborted( *this );
        }
    else
        {
        iObserver.PhaseCanceled( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to accept
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to deny
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2CommAddressSelectPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2CommAddressSelectPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2CommAddressSelectPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2CommAddressSelectPhase::FieldContent() const
    {
    return iFieldContent;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    __ASSERT_DEBUG( iState == EInitialState || iState == ExSPLinksRetrieved ||
            iState == ExSPContactRetrieved, 
            Panic( EPanicPreCond_ContactOperationComplete ) );
    
    if ( iState == EInitialState )
        {
        iState = EMainContactRetrieved;
        // Contact retrieval complete, take contact ownership
        delete iStoreContact;
        iStoreContact = aContact;
        StartLoadingxSPContactLinks();
        }
    else if ( iState == ExSPLinksRetrieved || iState == ExSPContactRetrieved )
        {
        iState = ExSPContactRetrieved;
        
        // xSP contact retrieval complete. Ignore errors, just try next one
        // or go to next state.
        /*TInt err = */ixSPStoreContactsArray.Append( aContact );

        // Start retrieving next xSP contact, if there are some contacts left
        if ( ixSPContactsArray->Count() > 0 )
            {
            RetrieveContact();
            }
        else
            {
            iState = ExSPContactsRetrieved;
            StartLoadingPresenceIconInfo();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2CommAddressSelectPhase::Pbk2ProcessKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret = EFalse;

    if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        iObserver.PhaseOkToExit( *this, EEikBidCancel );
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::VPbkOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::VPbkOperationFailed(
    MVPbkContactOperationBase* /*aOperation*/, TInt /*aError*/ )
    {
    // Ignore an error, xSP info can be omitted. Just run address select dialog
    DoSelectAddresses();
    }
// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::VPbkOperationResultCompleted
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::VPbkOperationResultCompleted(
    MVPbkContactOperationBase* /*aOperation*/,
    MVPbkContactLinkArray* aArray )
    {
    iState = ExSPLinksRetrieved;
    if ( !aArray || aArray->Count() == 0 )
        {
        StartLoadingPresenceIconInfo();
        }
    else
        {
		StartLoadingxSPContacts( *aArray );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ReceiveIconInfoL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::ReceiveIconInfoL(
            const TDesC8& /*aPackedLink*/,
            const TDesC8& /*aBrandId*/,
            const TDesC8& /*aElementId*/ )
    {
    // nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ReceiveIconFileL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::ReceiveIconFileL(
            const TDesC8& aBrandId,
            const TDesC8& aElementId,
            CFbsBitmap* aBrandedBitmap,
            CFbsBitmap* aMask )
    {
	iState = EPresenceIconRetrieved;
	
    // icon file received, save it to icon info array
    TInt count = iPresenceIconArray.Count();    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPresenceIconArray[i]->BrandId().CompareF( aBrandId ) == 0 &&
             iPresenceIconArray[i]->ElementId().CompareF( aElementId ) == 0 )
            {
            if ( !iPresenceIconArray[i]->IconBitmap() )
                {
                iPresenceIconArray[i]->SetBitmap( aBrandedBitmap, aMask );
                break;
                }            
            }
        }

    // check if all icon files received
    iState = EPresenceIconsRetrieved;
    for ( TInt j = 0; j < count && iState == EPresenceIconsRetrieved; j++ )
        {
        if ( iPresenceIconArray[j]->IconBitmap() == NULL ||
             iPresenceIconArray[j]->IconBitmapMask() == NULL )
            {
            // Not all retrieved, still in state  EPresenceIconRetrieved.
            iState = EPresenceIconRetrieved;
            }
        }

    // start address fetch dialog, if it waits for presence icons
    if ( iState == EPresenceIconsRetrieved )
        {
        DoSelectAddresses();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::PresenceSubscribeError
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::PresenceSubscribeError(
            const TDesC8& /*aPackedLink*/,
            TInt /*aStatus*/ )
    {
    // nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ErrorOccured
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::ErrorOccured(
            TInt /*aOpId*/,
            TInt /*aStatus*/ )
    {
    if (iState == EPresenceIconInfoRetrieved || 
        iState == EPresenceIconRetrieved )
        {
        iPresenceIconArray.ResetAndDestroy();
        DoSelectAddresses();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ReceiveIconInfosL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::ReceiveIconInfosL(
            const TDesC8& aPackedLink,
            RPointerArray <MContactPresenceInfo>& aInfoArray,
            TInt aOpId )
    {
    iState = EPresenceIconInfoRetrieved;
    // Must be TRAPped here because presence framework ignores the leave
    // and this instance will be jammed if ReceiveIconInfosL leaves.
    TRAPD( res, HandleReceiveIconInfosL( aPackedLink, aInfoArray, aOpId ) );
    if ( res != KErrNone )
        {
        DoSelectAddresses();
        }
    
    }
// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StoreReady(
    MVPbkContactStore& /*aContactStore*/ ) 
    {
    // not interested
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/,
    TInt /*aReason*/ ) 
    {
    // not interested
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::HandleStoreEventL(
     MVPbkContactStore& /*aContactStore*/,
     TVPbkContactStoreEvent aEvent ) 
    {
    if ( aEvent.iContactLink != NULL && iStoreContact != NULL )
        {
        if ( aEvent.iContactLink->RefersTo( *iStoreContact ) )
            {
            CancelServicePhase();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::HandleReceiveIconInfosL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::HandleReceiveIconInfosL(
        const TDesC8& /*aPackedLink*/,
        RPointerArray<MContactPresenceInfo>& aInfoArray, TInt /*aOpId*/)
    {    
    // service specific icons received
    TInt count = aInfoArray.Count();

    // calculate and set preferred icon size
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
    iContactPresence->SetPresenceIconSize( size );

    for ( TInt i = 0; i < count; i++ )
        {
        if ( aInfoArray[i]->BrandId().Length() > 0 &&
             aInfoArray[i]->ElementId().Length() > 0 &&
             aInfoArray[i]->ServiceName().Length() > 0 )
            {
            TInt serviceSeparatorPos = aInfoArray[i]->ServiceName().
                FindF( KServiceNameSeparator );
            if ( serviceSeparatorPos > 0 )
                {
                TInt nameIdLength = aInfoArray[i]->ServiceName().Length() -
                    serviceSeparatorPos - 1;
                // save icon info to our own array
                CPbk2PresenceIconInfo* iconInfo = CPbk2PresenceIconInfo::NewL(
                     aInfoArray[i]->BrandId(),
                     aInfoArray[i]->ElementId(),
                     aInfoArray[i]->ServiceName().Right( nameIdLength ) );

                CleanupStack::PushL( iconInfo );
                iPresenceIconArray.AppendL( iconInfo ); // ownership is taken
                CleanupStack::Pop( iconInfo );

                // start retrieving icon file
                TInt opId = iContactPresence->GetPresenceIconFileL(
                    aInfoArray[i]->BrandId(), aInfoArray[i]->ElementId() );
                }
            }
        }
    
    if ( iPresenceIconArray.Count() == 0 )
        {
        DoSelectAddresses();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::RetrieveContact
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::RetrieveContact()
    {
    TRAPD( res, RetrieveContactL() );
    if ( res != KErrNone )
        {
        DoSelectAddresses();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::RetrieveContactL()
    {
    __ASSERT_DEBUG( iState == EInitialState || iState == ExSPLinksRetrieved ||
            iState == ExSPContactRetrieved, 
            Panic( EPanicPreCond_RetrieveContactL ) );

    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *iEikenv->EikAppUi() );

    // Fetch one contact at a time if service cancellation is not
    // commanded.
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    if ( iState == EInitialState )
        {
        iRetrieveOperation = appUi.ApplicationServices().ContactManager().
            RetrieveContactL( *iContactLink, *this );
        }
    else if ( iState == ExSPLinksRetrieved || iState == ExSPContactRetrieved  )
        {
        iRetrieveOperation = appUi.ApplicationServices().ContactManager().
            RetrieveContactL( ixSPContactsArray->At( 0 ), *this );
        ixSPContactsArray->Delete( 0 );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::DoSelectAddresses
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::DoSelectAddresses()
	{
	TRAPD( res, DoSelectAddressesL() );
	if ( res != KErrNone )
		{
		iState = EAddressSelectError;
		iObserver.PhaseError( *this, res );
		}
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::DoSelectAddressesL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::DoSelectAddressesL()
    {
    __ASSERT_DEBUG(iStoreContact, Panic(EPanicPreCond_DoSelectAddressesL));

    FilterXspContactsL();
    
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, iResourceId );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    TPbk2AddressSelectParams params
        ( *iStoreContact, appUi.ApplicationServices().ContactManager(),
          appUi.ApplicationServices().NameFormatter(),
          appUi.ApplicationServices().FieldProperties(),
          reader, iTitleResId );

    params.iCommMethod = iCommMethod;

    // If default priorities are set use defaults directly.
    params.SetDefaultPriorities( iPriorities );

    // Launch call directly using default values.
    params.SetUseDefaultDirectly( ETrue );

    TArray<MVPbkStoreContact*> storeContactsArray = 
            ixSPStoreContactsArray.Array();
    TArray<CPbk2PresenceIconInfo*> presenceIconsArray = 
            iPresenceIconArray.Array();
    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(
        params, iFieldTypeSelector, &storeContactsArray,
        &presenceIconsArray );

    // Correct CBA buttons
    TInt correctedCba = CorrectRSK( iResourceId );
    if ( correctedCba > KErrNone )
        {
        addressSelect->SetCba( correctedCba );
        }

    // Execute
    iAddressSelectEliminator = addressSelect;
    iAddressSelectEliminator->ResetWhenDestroyed
        ( &iAddressSelectEliminator );
    
    appUi.StoreManager().RegisterStoreEventsL( *this );     
    MVPbkStoreContactField* resultField = addressSelect->ExecuteLD();
    iState = EAddressSelectDone;
    appUi.StoreManager().DeregisterStoreEvents( *this ); 
	
    CleanupStack::PopAndDestroy(); // reader

    if ( resultField )
        {
        CleanupDeletePushL(resultField);

        // Save field content
        MVPbkContactFieldData& fieldData = resultField->FieldData();
        TVPbkFieldStorageType type = fieldData.DataType();
        if ( type == EVPbkFieldStorageTypeText )
            {
            MVPbkContactFieldTextData& textData =
                MVPbkContactFieldTextData::Cast( fieldData );
            TPtrC ptr = textData.Text();
            delete iFieldContent;
            iFieldContent = NULL;
            iFieldContent = HBufC::NewL( ptr.Length() );
            iFieldContent->Des().Copy( ptr );
            }
        else if ( type == EVPbkFieldStorageTypeUri )
            {
            MVPbkContactFieldUriData& textData =
                MVPbkContactFieldUriData::Cast( fieldData );
            TPtrC ptr = textData.Uri();
            delete iFieldContent;
            iFieldContent = NULL;
            iFieldContent = HBufC::NewL( ptr.Length() );
            iFieldContent->Des().Copy( ptr );
            }

        // Append field to the result array
        AppendResultL( resultField );
        CleanupStack::PopAndDestroy(); // resultField
        iObserver.NextPhase( *this );
        }
    else
        {
        // Normal cancellation and there is no results
        // to be sent to caller
        iObserver.PhaseCanceled( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::CorrectRSK
// --------------------------------------------------------------------------
//
inline TInt CPbk2CommAddressSelectPhase::CorrectRSK
        ( TInt aAddressSelectResourceId )
    {
    TInt result = KErrNone;

    if ( iRskBack )
        {
        switch ( aAddressSelectResourceId )
            {
            case R_PBK2_GENERIC_ADDRESS_SELECT:
            case R_PBK2_PHONE_NUMBER_SELECT:
            case R_PBK2_VIDEO_NUMBER_SELECT:
            case R_PBK2_EMAIL_ADDRESS_SELECT:
            case R_PBK2_EMAIL_OVER_SMS_ADDRESS_SELECT:
            case R_PBK2_MMS_ADDRESS_SELECT:
            case R_PBK2_POC_ADDRESS_SELECT:
            case R_PBK2_VOIP_ADDRESS_SELECT:
            case R_PBK2_DTMF_PHONE_NUMBER_SELECT:
            case R_PBK2_THUMBNAIL_SELECT:
            case R_PBK2_CALL_ITEM_NUMBER_SELECT:
                {
                result = R_AVKON_SOFTKEYS_SELECT_BACK__SELECT;
                break;
                }

            default:
                {
                // Do nothing
                break;
                }
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::AppendResultL
        ( const MVPbkStoreContactField* aField )
    {
    if ( aField )
        {
        // Add the contact link to the result array
        MVPbkContactLink* link = aField->CreateLinkLC();
        if ( link )
            {
            if ( !iResults )
                {
                iResults = CVPbkContactLinkArray::NewL();
                }

            CleanupStack::Pop(); // aField->CreateLinkLC()

            iResults->AppendL( link );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::GetImppFieldDataL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::GetImppFieldDataL( CDesC16ArrayFlat& aXspIdArray, MVPbkStoreContact& aContact )
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );
        
    const MVPbkFieldType* type = appUi.ApplicationServices().ContactManager().
            FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    
    CVPbkBaseContactFieldTypeIterator* itr =
            CVPbkBaseContactFieldTypeIterator::NewLC( *type,
                aContact.Fields() );
    
    while ( itr->HasNext() )
        {
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldUriData& uri =
                MVPbkContactFieldUriData::Cast(field->FieldData() );
        HBufC* data = uri.Uri().AllocL();
        CleanupStack::PushL(data);
        aXspIdArray.AppendL( data->Des() ); 
        CleanupStack::PopAndDestroy(data); //data
        }
    CleanupStack::PopAndDestroy(itr); //itr
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::FilterXspContactsL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::FilterXspContactsL()
    {
    TInt xspContactCount = ixSPStoreContactsArray.Count();
    
    if ( xspContactCount > 0 )
        {
        for ( TInt i = xspContactCount - 1; i >= 0; --i )
            {
            MVPbkStoreContact* contact = ixSPStoreContactsArray[i];
            if ( !IsMatchL( *contact, *iStoreContact ) )
                 {
                 ixSPStoreContactsArray.Remove(i);
                 delete contact;
                 }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::IsMatchL
// --------------------------------------------------------------------------
//
TBool CPbk2CommAddressSelectPhase::IsMatchL( MVPbkStoreContact& aXspContact, 
        MVPbkStoreContact& aStoreContact )
    {
    TBool result = EFalse;
    
    CDesCArrayFlat* storeContactXspIdArray = new ( ELeave ) CDesCArrayFlat( KMaxXspServiceCount );
    CleanupStack::PushL( storeContactXspIdArray );
    GetImppFieldDataL( *storeContactXspIdArray, aStoreContact );
    
    CDesCArrayFlat* xspContcatXspIdArray = new ( ELeave ) CDesCArrayFlat( KMaxXspServiceCount );
    CleanupStack::PushL( xspContcatXspIdArray );
    GetImppFieldDataL( *xspContcatXspIdArray, aXspContact );
    
    for ( TInt i = 0; i < storeContactXspIdArray->MdcaCount(); i++ )
        {
        TPtrC data = storeContactXspIdArray->MdcaPoint( i );
        for ( TInt j = 0; j < xspContcatXspIdArray->MdcaCount(); j++ )
            {
            TPtrC data1 = xspContcatXspIdArray->MdcaPoint( j );
            if ( data1.CompareF( data ) == 0 )
                 {
                 result = ETrue;	
                 break;
                 }
            }
        }
    CleanupStack::PopAndDestroy(); // xspContcatXspIdArray
    CleanupStack::PopAndDestroy(); // storeContactXspIdArray
    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingxSPContactLinks
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingxSPContactLinks()
    {
    TRAPD( res, StartLoadingxSPContactLinksL() );
    if (res != KErrNone)
        {
        DoSelectAddresses();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingxSPContactLinksL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingxSPContactLinksL()
    {
    delete ixSPContactOperation;
    ixSPContactOperation = NULL;
    ixSPContactOperation = ixSPManager->GetxSPContactLinksL(
            *iStoreContact, *this, *this);
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingxSPContacts
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingxSPContacts(
        MVPbkContactLinkArray& aArray )
	{
	TRAPD( res, StartLoadingxSPContactsL( aArray ) );
	if ( res != KErrNone )
		{
		DoSelectAddresses();
		}
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingxSPContactsL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingxSPContactsL( 
        MVPbkContactLinkArray& aArray )
	{
	// Take a own copy of supplied contact links
    if (!ixSPContactsArray)
        {
        ixSPContactsArray = CVPbkContactLinkArray::NewL();
        }
    ixSPStoreContactsArray.ResetAndDestroy();
    
	CopyContactLinksL( aArray, *ixSPContactsArray );
	RetrieveContactL();
	}


// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingPresenceIconInfo
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingPresenceIconInfo()
	{
	TRAPD( res, StartLoadingPresenceIconInfoL());
	if ( res != KErrNone )
		{
		// In error case continue like there are no presence icons.
        DoSelectAddresses();
		}
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::StartLoadingPresenceIconInfoL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::StartLoadingPresenceIconInfoL()
	{
	const TInt oneAddress = 1;
	if ( ( iCommMethod == 
			VPbkFieldTypeSelectorFactory::EInstantMessagingSelector ||
		   iCommMethod ==
				   VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) &&
		   NumOfAddressesL() > oneAddress )
		{
        GetPresenceInfoL();
		}
	else
		{
        DoSelectAddresses();
        }
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::NumOfAddressesL
// --------------------------------------------------------------------------
//
TInt CPbk2CommAddressSelectPhase::NumOfAddressesL( 
		MVPbkStoreContact& aStoreContact )
	{
	CVPbkFieldFilter::TConfig config( aStoreContact.Fields(),
			&iFieldTypeSelector );
	CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL( config );
	TInt result = fieldFilter->FieldCount();
	delete fieldFilter;
	return result;
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::NumOfAddressesL
// --------------------------------------------------------------------------
//
TInt CPbk2CommAddressSelectPhase::NumOfAddressesL()
	{
	TInt numOfAddresses = 0; 
	if ( iStoreContact )
		{
		numOfAddresses += NumOfAddressesL( *iStoreContact );
		}
		
	const TInt count = ixSPStoreContactsArray.Count();
	for ( TInt i = 0; i < count; ++i )
		{
		numOfAddresses += NumOfAddressesL( *ixSPStoreContactsArray[i] );
		}
	return numOfAddresses;
	}

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::GetPresenceInfoL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::GetPresenceInfoL()
	{
	if ( !iContactPresence )
		{
		iContactPresence = TContactPresenceFactory::NewContactPresenceL(
				*this );
		}
	// ...and start presence icons retrieving
	HBufC8* packedLink = iContactLink->PackLC();
	// Operation id no needed because CancelAll is used.
	/*TInt opId =*/iContactPresence->GetPresenceInfoL( *packedLink );
	CleanupStack::PopAndDestroy( packedLink );
	}

// End of File
