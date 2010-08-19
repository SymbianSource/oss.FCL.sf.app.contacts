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


#include "CPbk2PrepareSingleAssignPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "Pbk2ContactAssignerFactory.h"
#include "TPbk2AssignNoteService.h"
#include "MPbk2SelectFieldProperty.h"
#include "Pbk2ServerApp.hrh"
#include "CPbk2KeyEventDealer.h"
#include <TPbk2DestructionIndicator.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2ServerApp.rsg>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>


using namespace Pbk2ContactRelocator;

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    ENullPointer,
    ELogicRelocation
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2PrepareSingleAssignPhase" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::CPbk2PrepareSingleAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareSingleAssignPhase::CPbk2PrepareSingleAssignPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkStoreContact* aStoreContact,
          MPbk2SelectFieldProperty* aSelectFieldProperty,
          TInt aSelectFieldResult, TInt& aFieldIndex, TUint& aNoteFlags,
          TUint aRelocationFlags ) :
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iStoreContact( aStoreContact ),
            iSelectFieldResult( aSelectFieldResult ),
            iSelectFieldProperty( aSelectFieldProperty ),
            iFieldIndex( aFieldIndex ),
            iNoteFlags( aNoteFlags ),
            iRelocationFlags( aRelocationFlags )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::~CPbk2PrepareSingleAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareSingleAssignPhase::~CPbk2PrepareSingleAssignPhase()
    {
    Cancel();
    delete iContactRelocator;
    delete iStoreContact;
    delete iResults;
    delete iDealer;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2PrepareSingleAssignPhase::ConstructL()
    {
    iEikEnv = CEikonEnv::Static();

    iDealer = CPbk2KeyEventDealer::NewL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2PrepareSingleAssignPhase* CPbk2PrepareSingleAssignPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkStoreContact* aStoreContact,
          MPbk2SelectFieldProperty* aSelectFieldProperty,
          TInt aSelectFieldResult, TInt& aFieldIndex, TUint& aNoteFlags,
          TUint aRelocationFlags  )
    {
    CPbk2PrepareSingleAssignPhase* self =
        new ( ELeave ) CPbk2PrepareSingleAssignPhase
            ( aObserver, aStoreContact, aSelectFieldProperty,
              aSelectFieldResult, aFieldIndex, aNoteFlags,
              aRelocationFlags );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::LaunchServicePhaseL()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::CancelServicePhase()
    {
    delete iContactRelocator;
    iContactRelocator = NULL;

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2PrepareSingleAssignPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2PrepareSingleAssignPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareSingleAssignPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2PrepareSingleAssignPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2PrepareSingleAssignPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::RunL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::RunL()
    {
    PrepareForAssignL();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareSingleAssignPhase::RunError( TInt aError )
    {
    if ( aError != KErrDied )
        {
        // If this is destroyed it is not safe to use iObserver anymore
        iObserver.PhaseError( *this, aError );
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::ContactRelocatedL
        ( MVPbkStoreContact* aRelocatedContact )
    {
    delete iStoreContact;
    iStoreContact = aRelocatedContact;

    AppendResultL( iStoreContact );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    delete aContact;

    if ( aReason != KErrCancel )
        {
        iObserver.PhaseError( *this, aReason );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // The code should never enter here
    __ASSERT_DEBUG( EFalse, Panic( ELogicRelocation ) );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::RelocationProcessComplete()
    {
    // Next phase is select single property
    iObserver.PreviousPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareSingleAssignPhase::Pbk2ProcessKeyEventL
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
// CPbk2PrepareSingleAssignPhase::RelocateContactL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareSingleAssignPhase::RelocateContactL
        ( Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aPolicy,
          TBool& aThisDestroyed )
    {
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;

    // Asynchronously relocate the contact
    TBool relocate = iContactRelocator->RelocateContactL
        ( contact, *this, aPolicy, iRelocationFlags );

    if ( !aThisDestroyed )
        {
        if ( !relocate )
            {
            // Cancel
            TPbk2AssignNoteService noteService;
            noteService.ShowInformationNoteL
                ( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED );
            }
        else
            {
            // User accepted relocation
            if ( !iContactRelocator->IsPhoneMemoryInConfigurationL() )
                {
                // Note is shown only if phone memory is not in configuration
                iNoteFlags = KPbk2NoteFlagOneContactRelocated;
                }
            }
        }

    return relocate;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::QueryForDataReplaceL
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareSingleAssignPhase::QueryForDataReplaceL()
    {
    TInt ret = KErrNone;

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator
        ( &thisDestroyed, iDestroyedPtr );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    // Query for data replace
    TPbk2AssignNoteService noteService;
    TInt queryResult = noteService.ShowReplaceDetailQueryL
        ( appUi.ApplicationServices().NameFormatter(), *iStoreContact );


    if ( !thisDestroyed && !queryResult )
        {
        // User didn't agree to replace data
        if ( IsContactFromPhoneMemoryL( *iStoreContact ) )
            {
            TPbk2AssignNoteService noteService;
            noteService.ShowInformationNoteL
                ( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED );

            ret = KErrCancel;
            }
        else
            {
            TPbk2ContactRelocationQueryPolicy relocationPolicy =
                EPbk2DisplayBasicQuery;

            if ( RelocateContactL( relocationPolicy, thisDestroyed ) )
                {
                ret = KErrNotReady;
                }
            else
                {
                ret = KErrCancel;
                }
            }
        }


    if ( thisDestroyed )
        {
        ret = KErrDied;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::QueryForRelocationL
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareSingleAssignPhase::QueryForRelocationL()
    {
    TInt ret = KErrCancel;

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator
        ( &thisDestroyed, iDestroyedPtr );

    if ( iSelectFieldProperty->SelectedFieldType() )
        {
        CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
            ( *iEikEnv->EikAppUi() );

        TPbk2AssignNoteService noteService;
        noteService.ShowCanNotAddDetailNoteL
            ( appUi.ApplicationServices().NameFormatter(), *iStoreContact );
        }

    if ( IsContactFromPhoneMemoryL( *iStoreContact ) )
        {
        TPbk2AssignNoteService noteService;
        noteService.ShowInformationNoteL
            ( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED );
        }
    else
        {
        TPbk2ContactRelocationQueryPolicy relocationPolicy =
            EPbk2DisplayStoreDoesNotSupportQuery;

        if ( RelocateContactL( relocationPolicy, thisDestroyed ) )
            {
            ret = KErrNotReady;
            }
        }

    if ( thisDestroyed )
        {
        ret = KErrDied;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::PrepareForAssignL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::PrepareForAssignL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    switch( iSelectFieldResult )
        {
        case KErrNone:
            // leave iFieldIndex intact.
            {
            iObserver.NextPhase( *this );
            break;
            }
        case KErrNotFound:
            {
            iFieldIndex = KErrNotFound;
            iObserver.NextPhase( *this );
            break;
            }
        case KErrPathNotFound:
            {
            TInt fieldIndex = iSelectFieldProperty->SelectedFieldIndex();
            if ( fieldIndex > KErrNotFound )
                {
                iFieldIndex = fieldIndex;
                }
            iObserver.NextPhase( *this );
            break;
            }
        case KErrAlreadyExists:
            {
            TInt result = QueryForDataReplaceL();
            if ( result == KErrNone )
                {
                iFieldIndex = iSelectFieldProperty->SelectedFieldIndex();
                iObserver.NextPhase( *this );
                }
            else if ( result == KErrCancel )
                {
                iObserver.PhaseCanceled( *this );
                }
            else if ( result == KErrNotReady )
                {
                // Contact relocation took place, do not inform observer yet
                }
            else if ( result == KErrDied )
                {
                // This has been destroyed, do nothing
                }
            else
                {
                iObserver.PhaseError( *this, result );
                }
            break;
            }
        case KErrNotSupported:
            {
            TInt result = QueryForRelocationL();
            if ( result == KErrCancel )
                {
                iObserver.PhaseCanceled( *this );
                }
            else if ( result == KErrNotReady )
                {
                // Contact relocation took place, do not inform observer yet
                }
            else if ( result == KErrDied )
                {
                // This has been destroyed, do nothing
                }
            else
                {
                iObserver.PhaseError( *this, result );
                }
            break;
            }
        case KErrAbort:
            {
            iObserver.PhaseAborted( *this );
            break;
            }
        case KErrCancel: // FALLTHROUGH
        default:
            {
            iObserver.PhaseCanceled( *this );
            }
        };
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::IsContactFromPhoneMemoryL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareSingleAssignPhase::IsContactFromPhoneMemoryL
        ( const MVPbkStoreContact& aStoreContact )
    {
    TBool result = EFalse;

    TVPbkContactStoreUriPtr phoneMemoryUri
        ( VPbkContactStoreUris::DefaultCntDbUri() );

    TVPbkContactStoreUriPtr contactUri =
        aStoreContact.ParentStore().StoreProperties().Uri();

    if ( contactUri.Compare(
            phoneMemoryUri,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
        {
        result = ETrue;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareSingleAssignPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::AppendResultL
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
// CPbk2PrepareSingleAssignPhase::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2PrepareSingleAssignPhase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
