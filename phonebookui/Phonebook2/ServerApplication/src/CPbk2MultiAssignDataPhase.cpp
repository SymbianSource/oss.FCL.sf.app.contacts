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
* Description:  Phonebook 2 server app contact data assign phase.
*
*/


#include "CPbk2MultiAssignDataPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "Pbk2ServerApp.hrh"
#include "MPbk2ContactAssigner.h"
#include "Pbk2ContactAssignerFactory.h"
#include "MPbk2SelectFieldProperty.h"
#include "TPbk2AssignNoteService.h"
#include "Pbk2AssignValidateField.h"
#include "CPbk2KeyEventDealer.h"
#include <CPbk2StoreConfiguration.h>
#include <TPbk2DestructionIndicator.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContact.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2ServerApp.rsg>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;
const TInt KGranularity = 4;

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
// CPbk2MultiAssignDataPhase::CPbk2MultiAssignDataPhase
// --------------------------------------------------------------------------
//
CPbk2MultiAssignDataPhase::CPbk2MultiAssignDataPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          MPbk2SelectFieldProperty* aSelectedFieldProperty,
          HBufC* aDataBuffer, TInt aMimeType, TUint aNoteFlags ) :
            iObserver( aObserver ),
            iSelectedFieldProperty( aSelectedFieldProperty ),
            iDataBuffer( aDataBuffer ),
            iMimeType( aMimeType ),
            iNoteFlags( aNoteFlags )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::~CPbk2MultiAssignDataPhase
// --------------------------------------------------------------------------
//
CPbk2MultiAssignDataPhase::~CPbk2MultiAssignDataPhase()
    {
    delete iResults;
    delete iContactDataAssigner;
    delete iContactLinks;
    delete iRetrieveOperation;
    delete iCommitOperation;
    delete iStoreContact;
    delete iDealer;
    if ( iStoreContacts )
        {
        iStoreContacts->ResetAndDestroy();
        }
    delete iStoreContacts;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2MultiAssignDataPhase::ConstructL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    iEikenv = CEikonEnv::Static();

    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( *aContactLinks, *iContactLinks );

    iDealer = CPbk2KeyEventDealer::NewL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2MultiAssignDataPhase* CPbk2MultiAssignDataPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          MPbk2SelectFieldProperty* aSelectedFieldProperty,
          HBufC* aDataBuffer, TInt aMimeType, TUint aNoteFlags )
    {
    CPbk2MultiAssignDataPhase* self =
        new ( ELeave ) CPbk2MultiAssignDataPhase
            ( aObserver, aSelectedFieldProperty,
              aDataBuffer, aMimeType, aNoteFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::LaunchServicePhaseL()
    {
    ContinueL();
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::CancelServicePhase()
    {
    iOperationCancelled = ETrue;
    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::RequestCancelL( TInt aExitCommandId )
    {
    // Withdraw our key event agent so that it does not react to
    // app shutter's escape key event simulation
    delete iDealer;
    iDealer = NULL;

    iOperationCancelled = ETrue;

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
// CPbk2MultiAssignDataPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2MultiAssignDataPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2MultiAssignDataPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2MultiAssignDataPhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2MultiAssignDataPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::AssignComplete
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AssignComplete
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt /*aIndex*/ )
    {
    MVPbkStoreContact* storeContact = iStoreContact;
    iStoreContact = NULL;
    TRAPD( err, AppendResultAndContinueL( storeContact ) );
    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::AssignFailed
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AssignFailed
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt aErrorCode )
    {
    iObserver.PhaseError( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == EContactLock )
        {
        TRAPD( err, AssignDataL() );

        if ( err != KErrNone && err != KErrDied )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
     // If the contact operation failed, the iStoreContact is expected to be deleted,
     // otherwise it will lead to memory leak.
    if ( iStoreContact )
        {
        delete iStoreContact;
        iStoreContact = NULL;
        }

    // if iContactsProcessed > 0, the note (how many contacts had been processed) needs to be shown.
    if ( (iContactLinks->Count() > 0) || (iContactsProcessed > 0) )
        {
        ContinueL();
        }
    else
        {
        iObserver.PhaseError( *this, aErrorCode );	
        }

    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    iStoreContact = aContact;
    TRAPD( err, iStoreContact->LockL( *this ) );
    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::StepComplete
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2MultiAssignDataPhase::StepFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/,
          TInt aError )
    {
    iObserver.PhaseError( *this, aError );

    // Do not continue
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::OperationComplete
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::OperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    // Commit done, we can finish now
    Finalize();
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2MultiAssignDataPhase::Pbk2ProcessKeyEventL
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
// CPbk2MultiAssignDataPhase::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AssignDataL()
    {
    const MVPbkFieldType* fieldType = NULL;
    if ( iSelectedFieldProperty )
        {
        fieldType = iSelectedFieldProperty->SelectedFieldType();
        }

    if ( fieldType )
        {
        MVPbkStoreContactField* contactField = NULL;
        if ( SelectContactFieldL( contactField ) )
            {
            CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
                ( *iEikenv->EikAppUi() );

            delete iContactDataAssigner;
            iContactDataAssigner = NULL;
            iContactDataAssigner =
                Pbk2ContactAssignerFactory::CreateContactDataAssignerL
                    ( *this, iMimeType, fieldType,
                      appUi.ApplicationServices().ContactManager(),
                      appUi.ApplicationServices().FieldProperties() );

            iContactDataAssigner->AssignDataL
                ( *iStoreContact, contactField, fieldType, iDataBuffer );
            }
        else
            {
            ContinueL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::SelectContactFieldL
// --------------------------------------------------------------------------
//
TBool CPbk2MultiAssignDataPhase::SelectContactFieldL
        ( MVPbkStoreContactField*& aField )
    {
    TBool ret = ETrue;

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator( &thisDestroyed, iDestroyedPtr );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    CPbk2PresentationContact* contact =
        CPbk2PresentationContact::NewL( *iStoreContact,
            appUi.ApplicationServices().FieldProperties() );
    CleanupStack::PushL( contact );

    // Check is the field type already used up in the contact
    TInt fieldIndex = KErrNotSupported;
    TInt result =
        Pbk2AssignValidateField::ValidateFieldTypeUsageInContactL
            ( *contact, *iSelectedFieldProperty->SelectedFieldType(),
              appUi.ApplicationServices().ContactManager().FsSession(),
              fieldIndex );

    switch( result )
        {
        case KErrAlreadyExists:
            {
            // Contact already has the one and only suitable contact
            // field filled up, show replace detail query
            TPbk2AssignNoteService noteService;
            TInt queryResult = noteService.ShowReplaceDetailQueryL
                ( appUi.ApplicationServices().NameFormatter(),
                  *iStoreContact );
            if ( !queryResult )
                {
                ret = EFalse;
                }
            else if ( !thisDestroyed )
                {
                // Set contact field to existing field so it gets replaced
                aField = &iStoreContact->Fields().FieldAt( fieldIndex );
                }
            break;
            }
        case KErrPathNotFound:
            {
            // Set contact field to existing field so it gets replaced
            aField = &iStoreContact->Fields().FieldAt( fieldIndex );
            break;
            }
        case KErrNotFound:      // FALLTHROUGH
        case KErrNone:
            {
            // Contact does not have the field type used, or does have
            // it used but supports more fields of that type.
            // Return value stays at ETrue.
            break;
            }
        default:
            {
            // If result was any other error code
            if ( result < KErrNone )
                {
                TPbk2AssignNoteService noteService;
                noteService.ShowCanNotAddDetailNoteL
                    ( appUi.ApplicationServices().NameFormatter(),
                      *iStoreContact );
                ret = EFalse;
                }
            break;
            }
        };

    CleanupStack::PopAndDestroy( contact );

    if ( thisDestroyed || iOperationCancelled )
        {
        // The calling code excepts us to leave with KErrDied if
        // this was destroyed
        User::Leave( KErrDied );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AppendResultL
        ( MVPbkStoreContact* aStoreContact )
    {
    if ( aStoreContact )
        {
        CleanupDeletePushL( aStoreContact );

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

        if ( !iStoreContacts )
            {
            iStoreContacts = new ( ELeave )
                CArrayPtrFlat<MVPbkStoreContact>( KGranularity );
            }

        // Add contact to the array of store contacts
        // to be commited later
        CleanupStack::Pop(); // aStoreContact
        iStoreContacts->AppendL( aStoreContact );   // takes ownership
        ++iContactsProcessed;
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::DisplayNotesL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::DisplayNotesL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );
    CPbk2StorePropertyArray& array =
        appUi.ApplicationServices().StoreProperties();

    TPbk2AssignNoteService noteService;
    noteService.ShowDetailAddedNoteL( iContactsProcessed );

    TVPbkContactStoreUriPtr store
        ( VPbkContactStoreUris::DefaultCntDbUri() );
    if ( iNoteFlags & KPbk2NoteFlagOneContactRelocated )
        {
        noteService.ShowInformationNoteL
            ( R_QTN_PHOB_NOTE_CONTACT_HIDDEN, store, array );
        }
    else if ( iNoteFlags & KPbk2NoteFlagSeveralContactsRelocated )
        {
        noteService.ShowInformationNoteL
            ( R_QTN_PHOB_NOTE_CONTACTS_HIDDEN, store, array );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::AppendResultAndContinueL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::AppendResultAndContinueL
        ( MVPbkStoreContact* aContact )
    {
    AppendResultL( aContact );
    ContinueL();
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    // Retrieve one contact at a time
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
    iContactLinks->Delete( KFirstElement );
    }
// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::CommitContactsL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::CommitContactsL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikenv->EikAppUi() );

    // All contacts have been processed, commit changes
    delete iCommitOperation;
    iCommitOperation = NULL;
    iCommitOperation = appUi.ApplicationServices().ContactManager().
        CommitContactsL( iStoreContacts->Array(), *this );
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::ContinueL
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::ContinueL()
    {
    if ( iContactLinks->Count() > 0 )
        {
        RetrieveContactL();
        }
    else if ( iStoreContacts && iStoreContacts->Count() > 0 )
        {
        CommitContactsL();
        }
    else
        {
        Finalize();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MultiAssignDataPhase::Finalize
// --------------------------------------------------------------------------
//
void CPbk2MultiAssignDataPhase::Finalize()
    {
    // Safe to ignore the error, it concerns only showing notes
    TRAP_IGNORE( DisplayNotesL(); );

    iObserver.NextPhase( *this );
    }

// End of File
