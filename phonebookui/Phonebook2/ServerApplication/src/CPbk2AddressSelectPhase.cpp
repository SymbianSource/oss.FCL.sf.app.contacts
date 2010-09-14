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

#include "CPbk2AddressSelectPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2KeyEventDealer.h"
#include <MPbk2DialogEliminator.h>
#include <CPbk2AddressSelect.h>
#include <TPbk2AddressSelectParams.h>
#include <MPbk2ApplicationServices.h>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>
#include <CPbk2StoreManager.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>

// System includes
#include <barsread.h>
#include <avkon.rsg>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;

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

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::CPbk2AddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2AddressSelectPhase::CPbk2AddressSelectPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          RVPbkContactFieldDefaultPriorities& aPriorities,
          TBool aRskBack ) :
            iObserver( aObserver ),
            iPriorities( aPriorities ),
            iRskBack( aRskBack )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::~CPbk2AddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2AddressSelectPhase::~CPbk2AddressSelectPhase()
    {
    if ( iAddressSelectEliminator )
        {
        iAddressSelectEliminator->ForceExit();
        }
    delete iStoreContact;
    delete iRetrieveOperation;
    delete iContactLinks;
    delete iResults;
    delete iDealer;
    if ( iThisPtrDestroyed )
        {
        *iThisPtrDestroyed = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AddressSelectPhase::ConstructL
        ( MVPbkContactLinkArray& aContactLinks,
          TAiwAddressSelectType aAddressSelectType )
    {
    iEikenv = CEikonEnv::Static();

    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( aContactLinks, *iContactLinks );

    iDealer = CPbk2KeyEventDealer::NewL( *this );

    iResourceId = AddressSelectResourceId( aAddressSelectType );
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2AddressSelectPhase* CPbk2AddressSelectPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray& aContactLinks,
          RVPbkContactFieldDefaultPriorities& aPriorities,
          TAiwAddressSelectType aAddressSelectType,
          TBool aRskBack )
    {
    CPbk2AddressSelectPhase* self =
        new ( ELeave ) CPbk2AddressSelectPhase
            ( aObserver, aPriorities, aRskBack );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks, aAddressSelectType );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::LaunchServicePhaseL()
    {
    // Start by retrieving first contact
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::CancelServicePhase()
    {
    if ( iAddressSelectEliminator )
        {
        iAddressSelectEliminator->ForceExit();
        }

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    // Reset and destroy contact links
    iContactLinks->ResetAndDestroy();

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::RequestCancelL( TInt aExitCommandId )
    {
    if ( iAddressSelectEliminator )
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
// CPbk2AddressSelectPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to accept
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to deny
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2AddressSelectPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2AddressSelectPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2AddressSelectPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2AddressSelectPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    // Run the address select
    TRAPD( err, DoSelectAddressesL() );

    if ( err != KErrNone )
        {
        // Deregister store events if something went wrong.
        CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
            ( *iEikenv->EikAppUi() );
        appUi.StoreManager().DeregisterStoreEvents( *this );

        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2AddressSelectPhase::Pbk2ProcessKeyEventL
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
// CPbk2AddressSelectPhase::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::StoreReady(
    MVPbkContactStore& /*aContactStore*/ )
    {
    // not interested
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/,
    TInt /*aReason*/ )
    {
    // not interested
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::HandleStoreEventL(
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
// CPbk2AddressSelectPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    // Fetch one contact at a time if service cancellation is not
    // commanded.
    if ( iContactLinks && iContactLinks->Count() > 0 )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        iRetrieveOperation = appUi.ApplicationServices().ContactManager().
            RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
        }
    else
        {
        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::DoSelectAddressesL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::DoSelectAddressesL()
    {
    // If field has already given use it prior to default field.
    MVPbkStoreContactField* field = CheckIfFieldAlreadySelectedLC();

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, iResourceId );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    TPbk2AddressSelectParams params
        ( *iStoreContact, appUi.ApplicationServices().ContactManager(),
          appUi.ApplicationServices().NameFormatter(),
          appUi.ApplicationServices().FieldProperties(),
          reader, iTitleResId );

    // If default priorities are set use defaults directly.
    params.SetDefaultPriorities( iPriorities );

    if ( field )
        {
        params.SetFocusedField( field );
        // If we have field focused we don't launch call directly using
        // default number. The number selection dialog should be shown.
        params.SetUseDefaultDirectly( EFalse );
        }
    else
        {
        // Launch call directly using default values.
        params.SetUseDefaultDirectly( ETrue );
        }

    CPbk2AddressSelect* addressSelect =
        CPbk2AddressSelect::NewL( params );

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
    
    TBool amIDestroyed( EFalse );
    iThisPtrDestroyed = &amIDestroyed;    
    
    MVPbkStoreContactField* resultField = addressSelect->ExecuteLD();
    appUi.StoreManager().DeregisterStoreEvents( *this );
    CleanupStack::PopAndDestroy(); // reader

    if ( amIDestroyed )
        {        
        return;
        }
    
    //Reset
    iThisPtrDestroyed = NULL;
    if ( resultField )
        {
        CleanupDeletePushL(resultField);
        AddFieldToResultsAndContinueL(resultField);
        CleanupStack::PopAndDestroy(); // resultField
        }
    else
        {
        if ( !iContactLinks->Count() && !iResults )
            {
            // Normal cancellation and there is no results
            // to be sent to caller
            iObserver.PhaseCanceled( *this );
            }
        else
            {
            // Although contact does not contain any selectable fields
            // address  fetching is continued because there is more
            // contacts left or some valid addresses are already selected
            RetrieveContactL();
            }
        }

    if ( field )
        {
        CleanupStack::PopAndDestroy();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::CorrectRSK
// --------------------------------------------------------------------------
//
inline TInt CPbk2AddressSelectPhase::CorrectRSK
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
// CPbk2AddressSelectPhase::CheckIfFieldAlreadySelectedLC
// --------------------------------------------------------------------------
//
inline MVPbkStoreContactField*
        CPbk2AddressSelectPhase::CheckIfFieldAlreadySelectedLC()
    {
    const MVPbkContactLink& contactLink = iContactLinks->At( KFirstElement );
    MVPbkStoreContactFieldCollection& fields = iStoreContact->Fields();
    MVPbkStoreContactField* field = fields.RetrieveField(contactLink);
    iContactLinks->Delete( KFirstElement );
    if ( field )
        {
        // Use CloneLC to avoid unwanted behaviour. Field collection might
        // mess up the field reference.
        return field->CloneLC();
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::AddFieldToResultsL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::AddFieldToResultsAndContinueL
        ( MVPbkStoreContactField* aField )
    {
    // Add the results
    AppendResultL( aField );

    // Continue with next contact
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelectPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelectPhase::AppendResultL
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
// CPbk2AddressSelectPhase::AddressSelectResourceId
// --------------------------------------------------------------------------
//
inline TInt CPbk2AddressSelectPhase::AddressSelectResourceId
        ( TAiwAddressSelectType aType )
    {
    TInt result = KErrNotFound;
    iTitleResId = 0;    // default is 0
    switch ( aType )
        {
        case EAiwAllItemsSelect:
            {
            result = R_PBK2_GENERIC_ADDRESS_SELECT;
            break;
            }

        case EAiwPhoneNumberSelect:
            {
            result = R_PBK2_PHONE_NUMBER_SELECT;
            break;
            }

        case EAiwCallItemSelect:
            {
            result = R_PBK2_CALL_ITEM_NUMBER_SELECT;
            iTitleResId = R_QTN_PHOB_QTL_CALL_TO_NAME;
            break;
            }

        case EAiwVideoNumberSelect:
            {
            result = R_PBK2_VIDEO_NUMBER_SELECT;
            break;
            }

        case EAiwPOCNumberSelect:
            {
            result =  R_PBK2_POC_ADDRESS_SELECT;
            break;
            }

        case EAiwEMailSelect:
            {
            result = R_PBK2_EMAIL_ADDRESS_SELECT;
            break;
            }

        case EAiwMMSSelect:
            {
            CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
                ( *iEikenv->EikAppUi() );

            if (appUi.ApplicationServices().
                LocallyVariatedFeatureEnabled( EVPbkLVShowEmailInSendMsg ))
                {
                // Email fields included
                result = R_PBK2_MMS_ADDRESS_SELECT;
                }
            else
                {
                result = R_PBK2_SMS_ADDRESS_SELECT;
                }
            break;
            }

        case EAiwSMSEMailSelect:
            {
            result = R_PBK2_EMAIL_OVER_SMS_ADDRESS_SELECT;
            break;
            }

        case EAiwVOIPSelect:
            {
            result = R_PBK2_VOIP_ADDRESS_SELECT;
            break;
            }

        case EAiwVoIPItemSelect:
            {
            result = R_PBK2_VOIP_ITEM_ADDRESS_SELECT;
            iTitleResId = R_QTN_PHOB_QTL_CALL_TO_NAME;
            break;
            }

        case EAiwSIPSelect:
            {
            result = R_PBK2_SIP_ADDRESS_SELECT;
            break;
            }

        case EAiwSIPMSISDNSelect:
            {
            result = R_PBK2_SIP_MSISDN_ADDRESS_SELECT;
            break;
            }

        case EAiwThumbnailSelect:
            {
            result = R_PBK2_THUMBNAIL_SELECT;
            break;
            }

        case EAiwDTMFPhoneNumberSelect:
            {
            result = R_PBK2_DTMF_PHONE_NUMBER_SELECT;
            break;
            };

        default:
            {
            // Do nothing
            break;
            }
        }

    return result;
    }

// End of File
