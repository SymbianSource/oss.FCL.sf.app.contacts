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
#include <Pbk2UIControls.rsg>
#include <Pbk2Commands.rsg>
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
#include <VPbkEng.rsg>

// System includes
#include <barsread.h>
#include <avkon.rsg>
#include <contactpresencefactory.h>
#include <mcontactpresence.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlists.h>

/// Unnamed namespace for local definitions
namespace {

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
            iCommMethod( aCommSelector ),
            iContactRetrieved( EFalse )
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

    // Create contact presence manager if chat fields should be shown
    if ( ( iCommMethod ==
            VPbkFieldTypeSelectorFactory::EInstantMessagingSelector ) ||
            ( iCommMethod ==
                        VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) )
            
        {
        iContactPresence = TContactPresenceFactory::NewContactPresenceL(
            *this );
        // ...and start presence icons retrieving
        HBufC8* packedLink = iContactLink->PackLC();
        TInt opId = iContactPresence->GetPresenceInfoL( *packedLink );
        CleanupStack::PopAndDestroy( packedLink );
        }
    else
        {
        iPresenceIconsRetrieved = ETrue;
        }
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
    // Start by retrieving first contact
    iState = EMainContactRetrieving;
    iContactRetrieved = ETrue;
    RetrieveContactL();
    
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    // Change layout back to regular
    appUi.ChangeStatuspaneLayoutL(
        CPbk2ServerAppAppUi::EStatusPaneLayoutUsual );
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
    if ( iState == EMainContactRetrieving )
        {
        // Contact retrieval complete, take contact ownership
        delete iStoreContact;
        iStoreContact = aContact;

        // Start fetching xSP contact links
        iState = ExSPLinksRetrieving;

        TRAPD( err,
            ixSPContactOperation = ixSPManager->GetxSPContactLinksL(
                *iStoreContact, *this, *this );
             );

        if ( err != KErrNone )
            {
            // Run the address select dialog for the main contact at least
            TRAPD( err, DoSelectAddressesL() );
            if ( err != KErrNone )
                {
                iObserver.PhaseError( *this, err );
                }
            iState = EDialogWaitsUserInput;
            }
        }

    if ( iState == ExSPContactsRetrieving )
        {
        // xSP contact retrieval complete
        TInt err = ixSPStoreContactsArray.Append( aContact );

        // If error, stop retrieving xSP contacts and show address select
        // dialog.

        // Start retrieving next xSP contact, if there are some contacts left
        if ( ixSPContactsArray->Count() != 0 && err == KErrNone )
            {
            TRAP( err, RetrieveContactL(); );
            if ( err != KErrNone )
                {
                iState = EDialogWaitsUserInput;
                }
            }
        else
            {
            FilterXspContactsL();            
            // Run the address select, if no more contacts to retrieve
            TRAP( err, DoSelectAddressesL() );
            if ( err != KErrNone )
                {
                iObserver.PhaseError( *this, err );
                }
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
    TRAPD( err, DoSelectAddressesL() );
    if ( err != KErrNone )
       {
       iObserver.PhaseError( *this, err );
       }

    iState = EDialogWaitsUserInput;
    }
// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::VPbkOperationResultCompleted
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::VPbkOperationResultCompleted(
    MVPbkContactOperationBase* /*aOperation*/,
    MVPbkContactLinkArray* aArray )
    {
    if ( aArray == NULL || aArray->Count() == 0 )
        {
        // No xSP contacts, run address select dialog
        TRAPD( err, DoSelectAddressesL() );
        if ( err != KErrNone )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    else
        {
        TRAPD( err,
            // Take a own copy of supplied contact links
            ixSPContactsArray = CVPbkContactLinkArray::NewL();
            CopyContactLinksL( *aArray, *ixSPContactsArray );

            // Start fetching xSP contacts
            iState = ExSPContactsRetrieving;
            RetrieveContactL();
            );

        // If error happened, show address select dialog
        if ( err != KErrNone )
            {
            TRAP( err, DoSelectAddressesL() );
            if ( err != KErrNone )
                {
                iObserver.PhaseError( *this, err );
                }
            }

        ixSPStoreContactsArray.ResetAndDestroy();
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
    if ( iPresenceIconsRetrieved )
        {
        // icons retrieving failed before, no need to take care about
        // presence icons anymore
        return;
        }

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
    iPresenceIconsRetrieved = ETrue;
    for ( TInt j = 0; j < count && iPresenceIconsRetrieved; j++ )
        {
        if ( iPresenceIconArray[j]->IconBitmap() == NULL ||
             iPresenceIconArray[j]->IconBitmapMask() == NULL )
            {
            iPresenceIconsRetrieved = EFalse;
            }
        }

    // start address fetch dialog, if it waits for presence icons
    if ( iPresenceIconsRetrieved && iState == EWaitForPresenceIcons )
        {
        TRAPD( err, DoSelectAddressesL() );
        if ( err != KErrNone )
            {
            iObserver.PhaseError( *this, err );
            }
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
    // Incase of several errors from contact precense avoid
    // multiple launching of the select dialog
    // TODO: How should the errors be hanlded?
    if (!iPresenceIconsRetrieved)
        {
        // Error occured, destroy presence icon array
        iPresenceIconsRetrieved = ETrue;
        iPresenceIconArray.ResetAndDestroy();
        //Needn't to launch address fetch dialog if contact had been retrieved,
        //because it will start this dialog after retrieve the contact,
        //avoid multiple launching of the select dialog
        if ( !iContactRetrieved )
            {
            // Launch the address fetch dialog if it's waiting for presence icons.
            TRAPD( err, DoSelectAddressesL() );
            if ( err != KErrNone )
                {
                iObserver.PhaseError( *this, err );
                }
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::ReceiveIconInfosL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::ReceiveIconInfosL(
            const TDesC8& /*aPackedLink*/,
            RPointerArray <MContactPresenceInfo>& aInfoArray,
            TInt /*aOpId*/ )
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
        iPresenceIconsRetrieved = ETrue;
        }
    if ( iPresenceIconsRetrieved && iState == EWaitForPresenceIcons )
        {
        TRAPD( err, DoSelectAddressesL() );
        if ( err != KErrNone )
            {
            iObserver.PhaseError( *this, err );
            }
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
// CPbk2CommAddressSelectPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *iEikenv->EikAppUi() );

    // Fetch one contact at a time if service cancellation is not
    // commanded.
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    if ( iState == EMainContactRetrieving )
        {
        iRetrieveOperation = appUi.ApplicationServices().ContactManager().
            RetrieveContactL( *iContactLink, *this );
        }
    else if ( iState == ExSPContactsRetrieving )
        {
        iRetrieveOperation = appUi.ApplicationServices().ContactManager().
            RetrieveContactL( ixSPContactsArray->At( 0 ), *this );
        ixSPContactsArray->Delete( 0 );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommAddressSelectPhase::DoSelectAddressesL
// --------------------------------------------------------------------------
//
void CPbk2CommAddressSelectPhase::DoSelectAddressesL()
    {
    
    if ( !iStoreContact )
        {
        return;
        }
    
    if ( !iPresenceIconsRetrieved )
        {
        // wait for presence icons
        iState = EWaitForPresenceIcons;
        return;
        }

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

    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(
        params, iFieldTypeSelector, &ixSPStoreContactsArray.Array(),
        &iPresenceIconArray.Array() );

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
TBool CPbk2CommAddressSelectPhase::IsMatchL( MVPbkStoreContact& aXspContact, MVPbkStoreContact& aStoreContact )
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
// End of File
