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
* Description:  Phonebook 2 assign speed dial command object.
*
*/


#include "CPbk2AssignSpeedDialCmd.h"

// Phonebook 2
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactDetailsControl.h>
#include <CPbk2ContactRelocator.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactIdConverter.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkBaseContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <SpdiaControl.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanic_NullPointer
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AssignSpeedDialCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::CPbk2AssignSpeedDialCmd
// --------------------------------------------------------------------------
//
CPbk2AssignSpeedDialCmd::CPbk2AssignSpeedDialCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            iUiControl( &aUiControl )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::~CPbk2AssignSpeedDialCmd
// --------------------------------------------------------------------------
//
CPbk2AssignSpeedDialCmd::~CPbk2AssignSpeedDialCmd()
    {
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    
    delete iIdConverter;
    delete iOperation;
    delete iStoreContact;
    delete iContactRelocator;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignSpeedDialCmd* CPbk2AssignSpeedDialCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPbk2AssignSpeedDialCmd* self = new ( ELeave ) CPbk2AssignSpeedDialCmd
        ( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ConstructL()
    {
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );

    iAppServices = CPbk2ApplicationServices::InstanceL();

    TVPbkContactStoreUriPtr uri(VPbkContactStoreUris::DefaultCntDbUri());
    MVPbkContactStore* defaultStore =
        iAppServices->ContactManager().ContactStoresL().Find(uri);
    User::LeaveIfNull(defaultStore);
    iIdConverter = CVPbkContactIdConverter::NewL(*defaultStore);
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    if (iUiControl)
        {
        MVPbkContactLink* link = iUiControl->FocusedContactL()->CreateLinkLC();
        iOperation =
            iAppServices->ContactManager().RetrieveContactL(*link, *this);
        CleanupStack::PopAndDestroy(); // link
        }
    else
        {
        FinishCommand( KErrNotFound );
        }
    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ResetUiControl
        ( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if (aResult.iOpCode == EContactLock)
        {
        TRAPD( err,iStoreContact->CommitL( *this ) );
        if ( err != KErrNone )
            {
            // Speed dial has showed the error note if any, just exit
            FinishCommand( err );
            }
        }
    else if ( aResult.iOpCode == EContactCommit )
        {
        FinishCommand( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    FinishCommand(aErrorCode);
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    delete iOperation;
    iOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;

    TBool contactNeedsRelocation = EFalse;
    TRAPD(err, contactNeedsRelocation = RelocateContactL());

    if (!contactNeedsRelocation)
        {
        TRAP(err, AssignSpeedDialL());
        if (err != KErrNone)
            {
            FinishCommand(err);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    delete iOperation;
    iOperation = NULL;

    FinishCommand( aError );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ContactRelocatedL
        ( MVPbkStoreContact* aRelocatedContact )
    {
    iStoreContact = aRelocatedContact; // take ownership
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    // Take the contact back
    iStoreContact = aContact;
    if ( aReason == KErrCancel )
        {
        // No error note is to be shown to the user when she
        // manually cancels the relocation process, therefore
        // the error code must be converted
        aReason = KErrNone;
        }
    FinishCommand( aReason );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::RelocationProcessComplete()
    {
    TRAPD( res, AssignSpeedDialL() );
    if ( res != KErrNone )
        {
        FinishCommand( res );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::FinishCommand
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::FinishCommand( TInt aError )
    {
    if (aError == KErrNone && iUiControl)
        {
        // UI control has to reload its contact,
        // so lets call SetFocusedContactL
        TRAP(aError, iUiControl->SetFocusedContactL(*iStoreContact));
        }
    if (iUiControl)
        {
        iUiControl->UpdateAfterCommandExecution();
        }
    iCommandObserver->CommandFinished(*this);
    // Speed dial has showed the error note if any, just exit
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::RelocateContactL
// --------------------------------------------------------------------------
//
inline TBool CPbk2AssignSpeedDialCmd::RelocateContactL()
    {
    TBool ret = EFalse;

    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    if ( !iContactRelocator->IsPhoneMemoryContact( *iStoreContact ) &&
        iContactRelocator->IsPhoneMemoryInConfigurationL() )
        {
        // Asynchronously relocate contact
        iContactRelocator->RelocateContactL( iStoreContact, *this,
            Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery,
            CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
        iStoreContact = NULL;    // Ownership was taken
        ret = ETrue;             // Indicate that contact needs relocating
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::AssignSpeedDialL
// --------------------------------------------------------------------------
//
void CPbk2AssignSpeedDialCmd::AssignSpeedDialL()
    {
    if (!iUiControl)
        {
        User::Leave(KErrNotFound);
        }

    MVPbkStoreContactField* field = FocusedFieldLC( *iStoreContact );
    TInt fieldIndex = iIdConverter->PbkEngFieldIndexL( *field );
    CleanupStack::PopAndDestroy(); // field
    MVPbkContactLink* link =
            iStoreContact->CreateLinkLC();

    CSpdiaControl* speedDialDlg = CSpdiaControl::NewL();

    // No need to check the return value
    speedDialDlg->ExecuteLD(iIdConverter->LinkToIdentifier
        (*link), fieldIndex);
    CleanupStack::PopAndDestroy(); // link

    __ASSERT_DEBUG( iStoreContact, Panic(EPanic_NullPointer));
    // This is done to get the "contact changed" -event from the store.
    // Not really needed otherwise because the CSpdiaControl commits the
    // contact but event is not sent by the store.
    iStoreContact->LockL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSpeedDialCmd::FocusedFieldLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2AssignSpeedDialCmd::FocusedFieldLC
        ( MVPbkStoreContact& aStoreContact )
    {
    TInt index = iUiControl->FocusedFieldIndex();
    if ( index != KErrNotFound )
        {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.
        return aStoreContact.Fields().FieldAtLC( index );
        }
    return NULL;
    }

// End of File
