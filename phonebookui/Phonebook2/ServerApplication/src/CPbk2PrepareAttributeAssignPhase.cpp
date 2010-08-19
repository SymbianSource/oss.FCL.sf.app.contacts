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
* Description:  Phonebook 2 server app prepare single assign phase.
*
*/


#include "CPbk2PrepareAttributeAssignPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2KeyEventDealer.h"
#include <TPbk2StoreContactAnalyzer.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <TVPbkStoreContactAnalyzer.h>
#include <VPbkFieldTypeSelectorFactory.h>

// System includes
#include <featmgr.h>

using namespace Pbk2ContactRelocator;

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

#ifdef _DEBUG

enum TPanicCode
    {
    ENullPointer,
    ELogicRelocation
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2PrepareAttributeAssignPhase" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::CPbk2PrepareAttributeAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareAttributeAssignPhase::CPbk2PrepareAttributeAssignPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          TPbk2AttributeAssignData aAttributeData,
            TBool aRemoveAttribute ) :
            iObserver( aObserver ),
            iAttributeData( aAttributeData ),
            iRemoveAttribute( aRemoveAttribute )
    {
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::~CPbk2PrepareAttributeAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareAttributeAssignPhase::~CPbk2PrepareAttributeAssignPhase()
    {
    delete iStoreContact;
    delete iContactRelocator;
    delete iRetrieveOperation;
    delete iDealer;
    delete iContactLinks;
    delete iResults;
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2PrepareAttributeAssignPhase::ConstructL
        ( MVPbkContactLinkArray& aContactLinks )
    {
    iEikenv = CEikonEnv::Static();

    iContactLinks = CVPbkContactLinkArray::NewL();
    CopyContactLinksL( aContactLinks, *iContactLinks );

    iDealer = CPbk2KeyEventDealer::NewL( *this );
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2PrepareAttributeAssignPhase* CPbk2PrepareAttributeAssignPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          TPbk2AttributeAssignData aAttributeData,
          TBool aRemoveAttribute )
    {
    CPbk2PrepareAttributeAssignPhase* self =
        new ( ELeave ) CPbk2PrepareAttributeAssignPhase
            ( aObserver, aAttributeData, aRemoveAttribute );
    CleanupStack::PushL( self );
    self->ConstructL( *aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::LaunchServicePhaseL()
    {
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::CancelServicePhase()
    {
    delete iContactRelocator;
    iContactRelocator = NULL;

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::RequestCancelL( TInt aExitCommandId )
    {
    delete iContactRelocator;
    iContactRelocator = NULL;

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
// CPbk2PrepareAttributeAssignPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2PrepareAttributeAssignPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareAttributeAssignPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2PrepareAttributeAssignPhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2PrepareAttributeAssignPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    TRAPD( err, PrepareAssignL() );
    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::ContactRelocatedL
        ( MVPbkStoreContact* aRelocatedContact )
    {
    delete iStoreContact;
    iStoreContact = aRelocatedContact;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    delete aContact;

    if ( aReason != KErrCancel )
        {
        iObserver.PhaseError( *this, aReason );
        }
    else
        {
        iObserver.PhaseCanceled( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // The code should never enter here
    __ASSERT_DEBUG( EFalse, Panic( ELogicRelocation ) );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::RelocationProcessComplete()
    {
    TRAPD( err, AppendResultL( iStoreContact ) );

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    else
        {
        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareAttributeAssignPhase::Pbk2ProcessKeyEventL
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
// CPbk2PrepareAttributeAssignPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *iEikenv->EikAppUi() );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::PrepareAssignL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::PrepareAssignL()
    {
    TBool contactIsValid = ValidateContactL();

    if ( contactIsValid )
        {
        // If contact does not need relocation, append the original
        // given link to results array (important not to create a new link
        // from retrieved contact since it will lose field link information)
        AppendResultL( iContactLinks->At( KFirstElement ) );

        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ValidateContactL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareAttributeAssignPhase::ValidateContactL()
    {
    TBool ret = ETrue;

    if ( !ContactMeetsPreconditionsL() )
        {
        ret = EFalse;
        iObserver.PhaseError( *this, KErrArgument );
        }

    if ( ret && ContactNeedsRelocation() )
        {
        RelocateContactL();
        ret = EFalse;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ContactMeetsPreconditionsL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareAttributeAssignPhase::ContactMeetsPreconditionsL()
    {
    TBool ret = ETrue;

    if ( iAttributeData.iAttributeUid ==
         TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
        {
        CPbk2ServerAppAppUi& appUi =
            static_cast<CPbk2ServerAppAppUi&>
                ( *iEikenv->EikAppUi() );

        // The contact must have a phone number field
        TPbk2StoreContactAnalyzer analyzer
            ( appUi.ApplicationServices().ContactManager(), iStoreContact );
        
        TVPbkStoreContactAnalyzer vpbkAnalyzer
            ( appUi.ApplicationServices().ContactManager(), iStoreContact ); 
        
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            if ( ( KErrNotFound == analyzer.HasFieldL( R_PHONEBOOK2_SIP_MSISDN_SELECTOR ) ) 
              // use vpbk to check whether the installed service can be assigned	
              && ( KErrNotFound == vpbkAnalyzer.HasFieldL( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) ) )
                {
                ret = EFalse;
                }
            }
        else
            {
            if ( KErrNotFound ==
                    analyzer.HasFieldL( R_PHONEBOOK2_PHONENUMBER_SELECTOR ) )
                {
                ret = EFalse;
                }
            }


        if ( ret && iRemoveAttribute )
            {
            // In case of attribute removal we must also check that
            // the given contact link is a field link

            if ( !iStoreContact->Fields().RetrieveField
                    ( iContactLinks->At( KFirstElement ) ) )
                {
                ret = EFalse;
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::ContactNeedsRelocation
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareAttributeAssignPhase::ContactNeedsRelocation()
    {
    TBool ret = EFalse;

    if ( iAttributeData.iAttributeUid ==
         TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
        {
        CPbk2ServerAppAppUi& appUi =
            static_cast<CPbk2ServerAppAppUi&>
                ( *iEikenv->EikAppUi() );

        if ( !iStoreContact->ParentStore().StoreProperties().
              SupportsSpeedDials() )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::RelocateContactL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::RelocateContactL()
    {
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;

    // Asynchronously relocate the contact
    iContactRelocator->RelocateContactL
        ( contact, *this, EPbk2DisplayStoreDoesNotSupportQuery,
          CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::AppendResultL
        ( const MVPbkStoreContact* aStoreContact )
    {
    if ( aStoreContact )
        {
        // Add the contact link to the result array
        MVPbkContactLink* link = aStoreContact->CreateLinkLC();
        if ( link )
            {
            if ( !iResults )
                {
                iResults = CVPbkContactLinkArray::NewL();
                }

            CleanupStack::Pop(); // aStoreContact->CreateLinkLC()

            iResults->AppendL( link );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareAttributeAssignPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2PrepareAttributeAssignPhase::AppendResultL
        ( const MVPbkContactLink& aContactLink )
    {
    MVPbkContactLink* link = aContactLink.CloneLC();

    if ( !iResults )
        {
        iResults = CVPbkContactLinkArray::NewL();
        }

    CleanupStack::Pop(); // aContactLink->CloneLC()
    iResults->AppendL( link );
    }

// End of File
