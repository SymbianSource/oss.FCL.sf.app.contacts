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
* Description:  Phonebook 2 assign defaults command.
*
*/


// INCLUDE FILES
#include "CPbk2AssignDefaultsCmd.h"

// Phonebook 2
#include <MPbk2CommandObserver.h>
#include <CPbk2MemoryEntryDefaultsDlg.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <coemain.h>
#include <eikenv.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstContact = 0;
const TInt KOneContact = 1;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_ConstructL = 1,
    EPanicPreCond_LockContactL,
    EPanicPreCond_Null_Pointer
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AssignDefaultsCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::CPbk2AssignDefaultsCmd
// --------------------------------------------------------------------------
//
CPbk2AssignDefaultsCmd::CPbk2AssignDefaultsCmd( 
        MPbk2ContactUiControl& aUiControl ) :
    iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AssignDefaultsCmd::CPbk2AssignDefaultsCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::~CPbk2AssignDefaultsCmd
// --------------------------------------------------------------------------
//
CPbk2AssignDefaultsCmd::~CPbk2AssignDefaultsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AssignDefaultsCmd::~CPbk2AssignDefaultsCmd(0x%x)"),
        this);

    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    delete iContactRetriever;
    delete iContactLinkArray;
    delete iContact;
    delete iContactRelocator;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignDefaultsCmd* CPbk2AssignDefaultsCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPbk2AssignDefaultsCmd* self =
        new ( ELeave ) CPbk2AssignDefaultsCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AssignDefaultsCmd::ConstructL()
    {
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );

    iAppServices = CPbk2ApplicationServices::InstanceL();

    iContactLinkArray = iUiControl->SelectedContactsOrFocusedContactL();
    iUiControl->RegisterCommand( this );

    __ASSERT_DEBUG( iContactLinkArray &&
        iContactLinkArray->Count() == KOneContact,
            Panic( EPanicPostCond_ConstructL ) );
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AssignDefaultsCmd::ExecuteLD(0x%x)"), this);

    const TInt linkCount = iContactLinkArray->Count();

    // It is possible to assing defaults to only one contact at a time
    if ( linkCount == KOneContact )
        {
        CleanupStack::PushL( this );
        iContactRetriever = iAppServices->ContactManager().
            RetrieveContactL( iContactLinkArray->At( KFirstContact ),
                *this );
        CleanupStack::Pop( this );
        }
    else
        {
        // Notify command owner that the command has finished
        ProcessFinished( KErrArgument );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::AddObserver
        (MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == & aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::VPbkSingleContactOperationComplete
        (MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    if ( aContact )
        {
        iContact = aContact;

        TBool contactNeedsRelocation = EFalse;
        TInt err = KErrNone;
        TRAP( err, contactNeedsRelocation = RelocateContactL() );
        if (err != KErrNone)
            {
            ProcessFinished( err );
            }

        if ( !contactNeedsRelocation )
            {
            TRAP( err, LockContactL() );
            if ( err != KErrNone )
                {
                ProcessFinished( err );
                }
            }
        }
    else
        {
        ProcessFinished( KErrGeneral );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::VPbkSingleContactOperationFailed
        (MVPbkContactOperationBase& /*aOperation*/, TInt aError)
    {
    ProcessFinished( aError );
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ContactOperationCompleted
        (TContactOpResult aResult)
    {
    if (aResult.iOpCode == EContactLock)
        {
        TRAPD( err, LaunchAssignDialogL() );
        if ( err != KErrNone )
            {
            ProcessFinished( err );
            }
        }
    else if (aResult.iOpCode == EContactCommit)
        {
        // Now, its time to exit the command
        ProcessFinished( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode,
          TBool /*aErrorNotified*/ )
    {
    ProcessFinished( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ContactRelocatedL
        (MVPbkStoreContact* aRelocatedContact)
    {
    iContact = aRelocatedContact; // take ownership
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ContactRelocationFailed
        (TInt aReason, MVPbkStoreContact* aContact)
    {
    // Take the contact back
    iContact = aContact;
    if ( aReason == KErrCancel )
        {
        // No error note is to be shown to the user when she
        // manually cancels the relocation process, therefore
        // the error code must be converted
        aReason = KErrNone;
        }    
    ProcessFinished( aReason );
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::RelocationProcessComplete()
    {
    TRAPD( res, LockContactL() );
    if ( res != KErrNone )
        {
        ProcessFinished( res );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::LockContactL
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::LockContactL()
    {
    __ASSERT_DEBUG(iContact, Panic(EPanicPreCond_LockContactL));

    // We must lock the contact for changes
    iContact->LockL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::LaunchAssignDialogL
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::LaunchAssignDialogL()
    {
    CPbk2PresentationContact* presentation =
        CPbk2PresentationContact::NewL( *iContact,
        iAppServices->FieldProperties() );
    CleanupStack::PushL(presentation);

    // Launch the dialog
    CPbk2MemoryEntryDefaultsDlg* dlg =
        CPbk2MemoryEntryDefaultsDlg::NewL(*presentation,
        iAppServices->ContactManager());
    dlg->ExecuteLD();

    CleanupStack::PopAndDestroy(presentation); // presentation

    // Next, commit contact
    iContact->CommitL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::RelocateContactL
// --------------------------------------------------------------------------
//
inline TBool CPbk2AssignDefaultsCmd::RelocateContactL()
    {
    TBool ret = EFalse;

    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    if ( !iContactRelocator->IsPhoneMemoryContact( *iContact ) &&
        iContactRelocator->IsPhoneMemoryInConfigurationL() )
        {
        // Asynchronously relocate contact
        iContactRelocator->RelocateContactL( iContact, *this,
            Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery,
            CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
        iContact = NULL;    // Ownership was taken
        ret = ETrue;        // Indicate that contact needs relocating
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AssignDefaultsCmd::ProcessFinished
// --------------------------------------------------------------------------
//
void CPbk2AssignDefaultsCmd::ProcessFinished( TInt aError )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AssignDefaultsCmd::ProcessFinished()"));
        
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));        

    if ( aError == KErrNone && iUiControl)
        {
        // UI control has to reload its contact,
        // so lets call SetFocusedContactL
        TRAP( aError, iUiControl->SetFocusedContactL( *iContact ) );
        iUiControl->UpdateAfterCommandExecution();
        }

    if ( aError != KErrNone )
        {
        CCoeEnv::Static()->HandleError( aError );
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished( *this );
    }

//  End of File
