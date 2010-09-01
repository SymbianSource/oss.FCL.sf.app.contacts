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


#include "CPbk2SingleAssignDataPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "MPbk2ContactAssigner.h"
#include "Pbk2ContactAssignerFactory.h"
#include "CPbk2ContactEditorOperator.h"
#include "MPbk2SelectFieldProperty.h"
#include "TPbk2AssignNoteService.h"
#include "Pbk2ServerApp.hrh"
#include <MPbk2DialogEliminator.h>
#include <TPbk2DestructionIndicator.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2ServerApp.rsg>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::CPbk2SingleAssignDataPhase
// --------------------------------------------------------------------------
//
CPbk2SingleAssignDataPhase::CPbk2SingleAssignDataPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkStoreContact* aStoreContact,
          MPbk2SelectFieldProperty* aSelectedFieldProperty,
          TInt aSelectedFieldIndex, HBufC* aDataBuffer, TInt aMimeType,
          TCoeHelpContext aHelpContext,
          TUint aAssignFlags, TUint aNoteFlags ) :
            iObserver( aObserver ),
            iStoreContact( aStoreContact ),
            iSelectedFieldIndex( aSelectedFieldIndex ),
            iSelectedFieldProperty( aSelectedFieldProperty ),
            iDataBuffer( aDataBuffer ),
            iMimeType( aMimeType ),
            iHelpContext( aHelpContext ),
            iAssignFlags( aAssignFlags ),
            iNoteFlags( aNoteFlags ),
            iEikEnv( CEikonEnv::Static() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::~CPbk2SingleAssignDataPhase
// --------------------------------------------------------------------------
//
CPbk2SingleAssignDataPhase::~CPbk2SingleAssignDataPhase()
    {
    if ( iContactEditorEliminator )
        {
        iContactEditorEliminator->ForceExit();
        }
    delete iResults;
    delete iContactDataAssigner;
    delete iStoreContact;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SingleAssignDataPhase* CPbk2SingleAssignDataPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkStoreContact* aStoreContact,
          MPbk2SelectFieldProperty* aSelectedFieldProperty,
          TInt aSelectedFieldIndex, HBufC* aDataBuffer, TInt aMimeType,
          TCoeHelpContext aHelpContext,
          TUint aAssignFlags, TUint aNoteFlags )
    {
    CPbk2SingleAssignDataPhase* self =
        new ( ELeave ) CPbk2SingleAssignDataPhase
            ( aObserver, aStoreContact, aSelectedFieldProperty,
              aSelectedFieldIndex, aDataBuffer, aMimeType, aHelpContext,
              aAssignFlags, aNoteFlags );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::LaunchServicePhaseL()
    {
        DoAssignDataL();
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::CancelServicePhase()
    {
    if ( iContactEditorEliminator )
        {
        // Changes won't be saved here.
        iContactEditorEliminator->ForceExit();
        }
    // Notify observer to complete its message.
    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::RequestCancelL( TInt aExitCommandId )
    {
    if ( iContactEditorEliminator )
        {
        // Do not notify observer about abortion here, we must let
        // contact editor to do it. Otherwise the changes would not be saved.
        iContactEditorEliminator->RequestExitL( aExitCommandId );
        }
    else
        {
        if ( aExitCommandId == EEikBidCancel )
            {
            iObserver.PhaseAborted( *this );
            }
        else
            {
            iObserver.PhaseCanceled( *this );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SingleAssignDataPhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SingleAssignDataPhase::ExtraResultData() const
    {
    return iSelectedFieldIndex;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SingleAssignDataPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SingleAssignDataPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::AssignComplete
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::AssignComplete
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt aIndex )
    {
    // aIndex is KErrNotSupported from CPbk2ContactEmptyDataAssigner
    // but iSelectedFieldIndex may still be set previously.
    if (aIndex == KErrNotSupported)
    {
        aIndex = iSelectedFieldIndex;
    }

    TRAPD( err, HandleContactDataAssignedL( aIndex ) );
    if ( err != KErrNone )
        {
        // HandleContactDataAssignedL will leave with KErrDied if
        // this was destroyed, then we will not inform observer
        // since iObserver is no more. Syslib's CBaActiveScheduler leaves
        // with KLeaveExit (in certain situations, e.g. pressing End key
        // with delete-dialog open when assigning empty contact.
        if ( err != KErrDied && err != KLeaveExit )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::AssignFailed
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::AssignFailed
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt aErrorCode )
    {
    iObserver.PhaseError( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    TInt err = KErrNone;

    if ( aResult.iOpCode == EContactLock )
        {
        TRAP( err, DoAssignDataL() );
        }
    else if ( aResult.iOpCode == EContactCommit )
        {
        TRAP( err, FinalizeL( iStoreContact ) );
        }

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    // Locking or committing the contact failed
    iObserver.PhaseError( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ContactEditingComplete
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::ContactEditingComplete
        ( MVPbkStoreContact* aEditedContact )
    {
    delete iStoreContact;
    iStoreContact = aEditedContact;

    TRAPD( err, FinalizeL( iStoreContact ) );

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::ContactEditingDeletedContact
        ( MVPbkStoreContact* aEditedContact )
    {
    delete iStoreContact;
    iStoreContact = aEditedContact;

    // Reset note flags
    iNoteFlags |= KPbk2NoteFlagContactDeleted;

    // Finalize, but with no results to append
    TRAPD( err, FinalizeL( NULL ) );

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::ContactEditingAborted()
    {
    iObserver.PhaseAborted( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2SingleAssignDataPhase::OkToExitL( TInt aCommandId )
    {
    return iObserver.PhaseOkToExit( *this, aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::DoAssignDataL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::DoAssignDataL()
    {
    const MVPbkFieldType* fieldType = NULL;
    MVPbkStoreContactField* field = NULL;
    if ( iSelectedFieldProperty )
        {
        fieldType = iSelectedFieldProperty->SelectedFieldType();
        if ( iSelectedFieldIndex >= KErrNone )
            {
            field = &iStoreContact->Fields().FieldAt( iSelectedFieldIndex );
            }
        }

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    delete iContactDataAssigner;
    iContactDataAssigner = NULL;
    iContactDataAssigner =
        Pbk2ContactAssignerFactory::CreateContactDataAssignerL
            ( *this, iMimeType, fieldType,
              appUi.ApplicationServices().ContactManager(),
              appUi.ApplicationServices().FieldProperties() );

    iContactDataAssigner->AssignDataL
        ( *iStoreContact, field, fieldType, iDataBuffer );
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::HandleContactDataAssignedL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::HandleContactDataAssignedL( TInt aIndex )
    {
    TInt index = aIndex;

    // If desired, open editor
    if ( !( iAssignFlags & EDoNotOpenEditor ) )
        {
        if ( iAssignFlags & ECreateNewContact )
            {
            // If we created a new contact, set focus to first field
            index = KErrNotFound;
            }

        TBool thisDestroyed = EFalse;
        iDestroyedPtr = &thisDestroyed;
        TPbk2DestructionIndicator indicator
            ( &thisDestroyed, iDestroyedPtr );

        CPbk2ContactEditorOperator* editorOperator =
            CPbk2ContactEditorOperator::NewLC( *this, *this );
        iContactEditorEliminator = editorOperator;
        iContactEditorEliminator->ResetWhenDestroyed
            ( &iContactEditorEliminator );

        editorOperator->OpenEditorL(
            iStoreContact, index, iAssignFlags, iHelpContext );

        // index is updated when editor is closed.
        iSelectedFieldIndex = index;

        CleanupStack::PopAndDestroy(); // operator

        if ( thisDestroyed )
            {
            // The calling code excepts us to leave with KErrDied if
            // this was destroyed
            User::Leave( KErrDied );
            }
        }
    else
        {
        iStoreContact->CommitL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::AppendResultL
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
// CPbk2SingleAssignDataPhase::DisplayNotesL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::DisplayNotesL()
    {
    TPbk2AssignNoteService noteService;

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    if ( !(iNoteFlags & KPbk2NoteFlagContactDeleted) 
           &&!(iAssignFlags & EskipInfoNotes) )
        {
        // Show note if new contact was created
        if ( iNoteFlags & KPbk2NoteFlagOneContactCreated )
            {
            if ( IsContactInSelectedMemoryL( *iStoreContact ) )
                {
                noteService.ShowInformationNoteL
                    ( R_QTN_PHOB_NOTE_CONTACT_CREATED );
                }
            else
                {
                TVPbkContactStoreUriPtr contactUri =
                    iStoreContact->ParentStore().StoreProperties().Uri();

                CPbk2StorePropertyArray& array =
                    appUi.ApplicationServices().StoreProperties();
                noteService.ShowInformationNoteL
                    ( R_QTN_PHOB_NOTE_CONTACT_HIDDEN, contactUri, array );
                }
            }
        // Show note if existing contact was updated
        else
            {
            if ( iNoteFlags & KPbk2NoteFlagOneContactRelocated )
                {
                CPbk2StorePropertyArray& array =
                    appUi.ApplicationServices().StoreProperties();
                TVPbkContactStoreUriPtr store
                    ( VPbkContactStoreUris::DefaultCntDbUri() );

                noteService.ShowInformationNoteL
                    ( R_QTN_PHOB_NOTE_CONTACT_HIDDEN, store, array );
                }
            else
                {
                noteService.ShowInformationNoteL
                    ( R_QTN_PHOB_NOTE_CONTACT_UPDATED );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::IsContactInSelectedMemoryL
// --------------------------------------------------------------------------
//
TBool CPbk2SingleAssignDataPhase::IsContactInSelectedMemoryL
        ( MVPbkStoreContact& aContact )
    {
    TBool result( EFalse );

    TVPbkContactStoreUriPtr contactUri =
        aContact.ParentStore().StoreProperties().Uri();

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    CVPbkContactStoreUriArray* currentConfig =
        appUi.ApplicationServices().StoreConfiguration().
            CurrentConfigurationL();
    CleanupStack::PushL( currentConfig );

    if ( currentConfig->IsIncluded( contactUri ) )
        {
        result = ETrue;
        }

    CleanupStack::PopAndDestroy( currentConfig );
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SingleAssignDataPhase::FinalizeL
// --------------------------------------------------------------------------
//
void CPbk2SingleAssignDataPhase::FinalizeL
        ( const MVPbkStoreContact* aStoreContact )
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
        
    AppendResultL( aStoreContact );
    DisplayNotesL();    
     
    if ( !thisDestroyed )
        {
        iObserver.NextPhase( *this );
        }    
    }

// End of File
