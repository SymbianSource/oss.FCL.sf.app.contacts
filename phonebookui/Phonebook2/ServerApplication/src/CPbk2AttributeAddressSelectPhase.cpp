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
* Description:  Phonebook 2 server app attribute address select phase.
*
*/

#include "CPbk2AttributeAddressSelectPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2KeyEventDealer.h"
#include <MPbk2DialogEliminator.h>
#include <CPbk2AddressSelect.h>
#include <TPbk2AddressSelectParams.h>
#include <MPbk2ApplicationServices.h>
#include <pbk2uicontrols.rsg>
#include <CPbk2StoreManager.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <CVPbkFieldTypeSelector.h>
#include <vpbkfieldtypeselectors.rsg>
#include <VPbkContactViewFilterBuilder.h>

// System includes
#include <barsread.h>
#include <avkon.rsg>
#include <featmgr.h>

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
// CPbk2AttributeAddressSelectPhase::CPbk2AttributeAddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2AttributeAddressSelectPhase::CPbk2AttributeAddressSelectPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          TPbk2AttributeAssignData aAttributeData,
          TBool aRskBack ) :
            iObserver( aObserver ),
            iAttributeData( aAttributeData ),
            iRskBack( aRskBack )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::~CPbk2AttributeAddressSelectPhase
// --------------------------------------------------------------------------
//
CPbk2AttributeAddressSelectPhase::~CPbk2AttributeAddressSelectPhase()
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

    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AttributeAddressSelectPhase::ConstructL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    iEikEnv = CEikonEnv::Static();

    iContactLinks = CVPbkContactLinkArray::NewL();
    if ( aContactLinks )
        {
        CopyContactLinksL( *aContactLinks, *iContactLinks );
        }
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2AttributeAddressSelectPhase* CPbk2AttributeAddressSelectPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          TPbk2AttributeAssignData aAttributeData,
          TBool aRskBack )
    {
    CPbk2AttributeAddressSelectPhase* self =
        new ( ELeave ) CPbk2AttributeAddressSelectPhase
            ( aObserver, aAttributeData, aRskBack );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::LaunchServicePhaseL()
    {
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::CancelServicePhase()
    {
    if ( iAddressSelectEliminator )
        {
        iAddressSelectEliminator->ForceExit();
        }

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2AttributeAddressSelectPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to accept
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to deny
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2AttributeAddressSelectPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2AttributeAddressSelectPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2AttributeAddressSelectPhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2AttributeAddressSelectPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );
    TInt err( KErrNone );
    
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    TRAP( err, SelectAddressesL() );       
  
    if ( err != KErrNone )
        {
		// Deregister store events if something went wrong. 
        appUi.StoreManager().DeregisterStoreEvents( *this );
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2AttributeAddressSelectPhase::Pbk2ProcessKeyEventL
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
// CPbk2AttributeAddressSelectPhase::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::StoreReady(
    MVPbkContactStore& /*aContactStore*/ )
    {
    // not interested
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/,
    TInt /*aReason*/ )
    {
    // not interested
    }
    
// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::StoreUnavailable
// --------------------------------------------------------------------------
//    
void CPbk2AttributeAddressSelectPhase::HandleStoreEventL(
     MVPbkContactStore& /*aContactStore*/,
     TVPbkContactStoreEvent aEvent )
    {
    if ( aEvent.iContactLink != NULL && iStoreContact != NULL )
        {
        if ( aEvent.iContactLink->RefersTo( *iStoreContact ) )
            {
            RDebug::Print(_L("----CPbk2AttributeAddressSelectPhase::HandleStoreEventL() - dismiss dialog"));
            // dismiss address selection dialog since contact was changed
            if ( iAddressSelectEliminator )
                {
                iAddressSelectEliminator->ForceExit();
                }
            
            // observer will be notified in SelectAddressesL() after closing 
            // address selection dialog
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *iEikEnv->EikAppUi() );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::SelectAddressesL
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::SelectAddressesL()
    {
    MVPbkStoreContactField* field = CheckIfFieldAlreadySelectedL();

    if ( !field )
        {
        TInt resourceId = AddressSelectResourceId( iAttributeData );
        // Field was not already selected --> querying it
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC( reader, resourceId );

        CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
            ( *iEikEnv->EikAppUi() );

        TPbk2AddressSelectParams params
            ( *iStoreContact, appUi.ApplicationServices().ContactManager(),
              appUi.ApplicationServices().NameFormatter(),
              appUi.ApplicationServices().FieldProperties(),
              reader, R_QTN_PHOB_TITLE_SELECT_DEFAULT );
        params.SetTitleToIncludeContactName( EFalse );

        CVPbkFieldTypeSelector* fieldTypeSelector = NULL;
        
        // In speed dial, for voip fields we build a fieldselector 
        // from vpbkfieldtypeselectorfactory. This makes sure the
        // right voip fields are shown in the address select dialog
        if ( iAttributeData.iAttributeUid ==
                 TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
            {
            // Speedial specific filter
            if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
                {
                fieldTypeSelector = VPbkFieldTypeSelectorFactory::
                    BuildContactActionTypeSelectorL(
                        VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                        appUi.ApplicationServices().ContactManager().FieldTypes() );
                CleanupStack::PushL( fieldTypeSelector );
                }
            else
                {
                fieldTypeSelector = CVPbkFieldTypeSelector::NewL( 
                        appUi.ApplicationServices().ContactManager().FieldTypes() );
                CleanupStack::PushL( fieldTypeSelector );
                }
            
            VPbkContactViewFilterBuilder::BuildContactViewFilterL( 
                                      *fieldTypeSelector, 
                                      EVPbkContactViewFilterPhoneNumber, 
                                      appUi.ApplicationServices().ContactManager() );
            }
        
        // Run address select
        CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(
                params,
                *fieldTypeSelector,
                NULL,
                NULL );
        
        // Correct CBA buttons
        TInt correctedCba = CorrectRSK( resourceId );
        if ( correctedCba > KErrNone )
            {
            addressSelect->SetCba( correctedCba );
            }

        // Execute
        iAddressSelectEliminator = addressSelect;
        iAddressSelectEliminator->ResetWhenDestroyed
            ( &iAddressSelectEliminator );
        appUi.StoreManager().RegisterStoreEventsL( *this );
        field = addressSelect->ExecuteLD();
        appUi.StoreManager().DeregisterStoreEvents( *this );
        if ( fieldTypeSelector )
			{
			CleanupStack::PopAndDestroy( 2 ); // fieldTypeSelector, reader
			}
		else
			{
			CleanupStack::PopAndDestroy(); // reader
			}
		
        }

    if ( field )
        {
        CleanupDeletePushL( field );
        AppendResultL( field );
        CleanupStack::PopAndDestroy( field );
        iObserver.NextPhase( *this );
        }
    else
        {
        iObserver.PhaseCanceled( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::CheckIfFieldAlreadySelectedL
// --------------------------------------------------------------------------
//
inline MVPbkStoreContactField*
        CPbk2AttributeAddressSelectPhase::CheckIfFieldAlreadySelectedL()
    {
    MVPbkStoreContactField* ret = NULL;

    const MVPbkContactLink& contactLink = iContactLinks->At( KFirstElement );
    MVPbkStoreContactFieldCollection& fields = iStoreContact->Fields();
    MVPbkStoreContactField* field = fields.RetrieveField( contactLink );
    if ( field )
        {
        ret = field->CloneLC();
        CleanupStack::Pop(); // field->CloneLC()
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::AddressSelectResourceId
// --------------------------------------------------------------------------
//
inline TInt CPbk2AttributeAddressSelectPhase::AddressSelectResourceId
        ( TPbk2AttributeAssignData aAttributeData )
    {
    TInt result = KErrNotFound;

    if ( aAttributeData.iAttributeUid ==
         TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
        {
        result = R_PBK2_PHONE_NUMBER_SELECT;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AttributeAddressSelectPhase::CorrectRSK
// --------------------------------------------------------------------------
//
inline TInt CPbk2AttributeAddressSelectPhase::CorrectRSK
        ( TInt aAddressSelectResourceId )
    {
    TInt result = KErrNone;

    if ( iRskBack )
        {
        switch ( aAddressSelectResourceId )
            {
            case R_PBK2_PHONE_NUMBER_SELECT:
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
// CPbk2AttributeAddressSelectPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2AttributeAddressSelectPhase::AppendResultL
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

// End of File
