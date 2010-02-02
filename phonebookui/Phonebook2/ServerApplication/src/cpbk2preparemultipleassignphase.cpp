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


#include "CPbk2PrepareMultipleAssignPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "Pbk2ContactAssignerFactory.h"
#include "TPbk2AssignNoteService.h"
#include "MPbk2SelectFieldProperty.h"
#include "Pbk2ServerApp.hrh"
#include "CPbk2KeyEventDealer.h"
#include <TPbk2DestructionIndicator.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2ServerApp.rsg>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkFieldType.h>


using namespace Pbk2ContactRelocator;

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement( 0 );

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
    _LIT( KPanicText, "CPbk2PrepareMultipleAssignPhase" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::CPbk2PrepareMultipleAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareMultipleAssignPhase::CPbk2PrepareMultipleAssignPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          MPbk2SelectFieldProperty* aSelectFieldProperty,
          TUint& aNoteFlags ) :
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iSelectFieldProperty( aSelectFieldProperty ),
            iNoteFlags( aNoteFlags )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::~CPbk2PrepareMultipleAssignPhase
// --------------------------------------------------------------------------
//
CPbk2PrepareMultipleAssignPhase::~CPbk2PrepareMultipleAssignPhase()
    {
    Cancel();
    delete iContactRelocator;
    delete iResults;
    delete iDealer;
    delete iRetrieveOperation;
    delete iContactLinks;
    delete iEntriesToRelocate;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2PrepareMultipleAssignPhase::ConstructL
        ( const MVPbkContactLinkArray* aContactLinks )
    {
    iDealer = CPbk2KeyEventDealer::NewL( *this );

    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( *aContactLinks, *iContactLinks );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2PrepareMultipleAssignPhase* CPbk2PrepareMultipleAssignPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          const MVPbkContactLinkArray* aContactLinks,
          MPbk2SelectFieldProperty* aSelectFieldProperty,
          TUint& aNoteFlags )
    {
    CPbk2PrepareMultipleAssignPhase* self =
        new ( ELeave ) CPbk2PrepareMultipleAssignPhase
            ( aObserver, aSelectFieldProperty, aNoteFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::LaunchServicePhaseL()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::CancelServicePhase()
    {
    delete iContactRelocator;
    iContactRelocator = NULL;

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2PrepareMultipleAssignPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2PrepareMultipleAssignPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareMultipleAssignPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2PrepareMultipleAssignPhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2PrepareMultipleAssignPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RunL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RunL()
    {
    PrepareForAssignL();

    if ( iEntriesToRelocate && iEntriesToRelocate->Count() > 0 )
        {
        if ( iEntriesToRelocate->Count() == 1 )
            {
            RetrieveContactL();
            }
        else
            {
            RelocateContactsL();
            }
        }
    else
        {
        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2PrepareMultipleAssignPhase::RunError( TInt aError )
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
void CPbk2PrepareMultipleAssignPhase::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::ContactRelocatedL
        ( MVPbkStoreContact* aRelocatedContact )
    {
    CleanupDeletePushL( aRelocatedContact );
    MVPbkContactLink* link = aRelocatedContact->CreateLinkLC();

    AppendResultL( link );

    CleanupStack::Pop(); // link
    CleanupStack::PopAndDestroy(); // aRelocatedContact
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    delete aContact;

    if ( aReason == KErrCancel )
        {
        if ( iResults && iResults->Count() > 0 )
            {
            // Continue with other contacts
            iObserver.NextPhase( *this );
            }
        else
            {
            // User canceled and there are no other contacts
            TPbk2AssignNoteService noteService;
            TRAP_IGNORE( noteService.ShowInformationNoteL
                ( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED ) );

            iObserver.PhaseCanceled( *this );
            }
        }
    else
        {
        iObserver.PhaseError( *this, aReason );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::ContactsRelocationFailed
        ( TInt aReason, CVPbkContactLinkArray* aContacts )
    {
    delete aContacts;

    if ( aReason == KErrCancel )
        {
        if ( iResults && iResults->Count() > 0 )
            {
            // Continue with other contacts
            iObserver.NextPhase( *this );
            }
        else
            {
            // User canceled and there are no other contacts
            TPbk2AssignNoteService noteService;
            TRAP_IGNORE( noteService.ShowInformationNoteL
                ( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED ) );

            iObserver.PhaseCanceled( *this );
            }
        }
    else
        {
        // Other errors
        iObserver.PhaseError( *this, aReason );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RelocationProcessComplete()
    {
    iObserver.NextPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    TRAPD( err, RelocateContactL( aContact ) ); // takes ownership
    if ( err != KErrNone )
        {
        // RelocateContactL will leave with KErrDied if this was
        // destroyed
        if ( err != KErrDied )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareMultipleAssignPhase::Pbk2ProcessKeyEventL
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
// CPbk2PrepareMultipleAssignPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *CEikonEnv::Static()->EikAppUi() );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iEntriesToRelocate->At( KFirstElement ), *this );
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RelocateContactL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RelocateContactL
        ( MVPbkStoreContact* aStoreContact )
    {
    CleanupDeletePushL( aStoreContact );

    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator( &thisDestroyed, iDestroyedPtr );

    CleanupStack::Pop( aStoreContact );

    // Asynchronously relocate the contact
    if ( iContactRelocator->RelocateContactL
        ( aStoreContact, *this, EPbk2DisplayStoreDoesNotSupportQuery,
          CPbk2ContactRelocator::EPbk2RelocatorExistingContact ) )
        {
        if ( thisDestroyed )
            {
            // The calling code excepts us to leave with KErrDied if
            // this was destroyed
            User::Leave( KErrDied );
            }

        // User accepted relocation
        if ( !iContactRelocator->IsPhoneMemoryInConfigurationL() )
            {
            // Note is shown only if phone memory is not in configuration
            iNoteFlags = KPbk2NoteFlagOneContactRelocated;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::RelocateContactsL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::RelocateContactsL()
    {
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator( &thisDestroyed, iDestroyedPtr );

    // Asynchronously relocate contacts
    TBool res = iContactRelocator->RelocateContactsL( iEntriesToRelocate,
        *this, EPbk2DisplayStoreDoesNotSupportQuery );

    if ( !thisDestroyed )
        {
        iEntriesToRelocate = NULL;  // ownership was taken

        if ( res )
            {
            // User accepted relocation
            if ( !iContactRelocator->IsPhoneMemoryInConfigurationL() )
                {
                // Note is shown only if phone memory is not in configuration
                iNoteFlags = KPbk2NoteFlagSeveralContactsRelocated;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::PrepareForAssignL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::PrepareForAssignL()
    {
    if ( iContactLinks )
        {
        const TInt count = iContactLinks->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            MVPbkContactLink* link = iContactLinks->At(i).CloneLC();
            if ( SupportsFieldL( link->ContactStore().
                 StoreProperties().Uri() ) )
                {
                AppendResultL( link );
                }
            else
                {
                SendToRelocationL( link );
                }
            CleanupStack::Pop(); // iContactLinks->At(i).CloneLC()
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::AppendResultL
        ( MVPbkContactLink* aContactLink )
    {
    if ( aContactLink )
        {
        if ( !iResults )
            {
            iResults = CVPbkContactLinkArray::NewL();
            }

        iResults->AppendL( aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::SendToRelocationL
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::SendToRelocationL
        ( MVPbkContactLink* aContactLink )
    {
    if ( aContactLink )
        {
        if ( !iEntriesToRelocate )
            {
            iEntriesToRelocate = CVPbkContactLinkArray::NewL();
            }

        iEntriesToRelocate->AppendL( aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::SupportsFieldL
// --------------------------------------------------------------------------
//
TBool CPbk2PrepareMultipleAssignPhase::SupportsFieldL
        ( TVPbkContactStoreUriPtr aStoreUri )
    {
    __ASSERT_DEBUG( iSelectFieldProperty->SelectedFieldType(),
        Panic( ENullPointer ) );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    MVPbkContactStore* store =
        appUi.ApplicationServices().ContactManager().ContactStoresL().
            Find( aStoreUri );

    const MVPbkFieldTypeList& fieldTypeList =
            store->StoreProperties().SupportedFields();

    TBool result = EFalse;
    if ( fieldTypeList.ContainsSame
            ( *iSelectFieldProperty->SelectedFieldType() ) )
        {
        result = ETrue;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2PrepareMultipleAssignPhase::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2PrepareMultipleAssignPhase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
