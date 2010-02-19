/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 delete mycard command.
*
*/


// INCLUDE FILES
#include "cpbk2deletemycardcmd.h"
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <Pbk2ExNamesListRes.rsg>

// Phonebook2
#include <CPbk2ApplicationServices.h>
#include <MPbk2CommandObserver.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2GeneralConfirmationQuery.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    ERunL_InvalidState = 1,
    EPanicPreCond_Null_Pointer,
    EPanicInvalidArray
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2DeleteMyCardCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG
}

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::CPbk2DeleteMyCardCmd
// --------------------------------------------------------------------------
//
inline CPbk2DeleteMyCardCmd::CPbk2DeleteMyCardCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteMyCardCmd::CPbk2DeleteMyCardCmd(0x%x)"), this);

    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::~CPbk2DeleteMyCardCmd
// --------------------------------------------------------------------------
//
CPbk2DeleteMyCardCmd::~CPbk2DeleteMyCardCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteMyCardCmd::~CPbk2DeleteMyCardCmd(0x%x)"), this);

    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand(NULL);
        }

    delete iContactsArray;
    delete iDeleteOperation;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2DeleteMyCardCmd::ConstructL(
    const MVPbkContactLink& aLink )
    {
    iContactsArray = CVPbkContactLinkArray::NewL();
    iContactsArray->AppendL( aLink.CloneLC() );
    CleanupStack::Pop(); // link

    iAppServices = CPbk2ApplicationServices::InstanceL();

    // register command to ui control
    iUiControl->RegisterCommand(this);
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2DeleteMyCardCmd* CPbk2DeleteMyCardCmd::NewL(
    MPbk2ContactUiControl& aUiControl, 
    const MVPbkContactLink& aLink )
    {
    CPbk2DeleteMyCardCmd* self = new(ELeave) CPbk2DeleteMyCardCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL( aLink );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteMyCardCmd::ExecuteLD(0x%x)"), this);

    IssueRequest( EConfirming );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::ResetUiControl(
    MPbk2ContactUiControl& aUiControl )
    {
    if( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteMyCardCmd::RunL(0x%x) start"), this);

    switch ( iState )
        {
        case EConfirming:
            {
            ConfirmDeletionL();
            break;
            }
        case EStarting:
            {
            DeleteContactL();
            break;
            }
        case EStopping:
            {
            ProcessDismissed( KErrNone );
            break;
            }
        case ECanceling:
            {
            ProcessDismissed( KErrCancel );
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(ERunL_InvalidState));
            break;
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2DeleteMyCardCmd::RunL end"));
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2DeleteMyCardCmd::RunError( TInt aError )
    {
    ProcessDismissed( aError );
    return aError;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::StepComplete(
    MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    // ignored
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2DeleteMyCardCmd::StepFailed(
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aStepSize*/,
    TInt aError )
    {
    ProcessDismissed( aError );
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::OperationComplete(
    MVPbkContactOperationBase& aOperation )
    {
    if ( &aOperation == iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;

        IssueRequest( EStopping );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::ProcessDismissed( TInt aCancelCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteMyCardCmd::ProcessDismissed()"));

    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));

    Cancel();
    delete iDeleteOperation;
    iDeleteOperation = NULL;

    if ( aCancelCode != KErrCancel && iUiControl)
        {
        // Don't remove list markings if user canceled
        iUiControl->SetBlank(EFalse);
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::DeleteContactL
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::DeleteContactL()
    {
    iDeleteOperation = iAppServices->
        ContactManager().DeleteContactsL( *iContactsArray, *this );

    if( iUiControl )
        {
        // Blank UI control to avoid flicker
        iUiControl->SetBlank( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
inline void CPbk2DeleteMyCardCmd::IssueRequest( TProcessState aState )
    {
    iState = aState;
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteMyCardCmd::ConfirmDeletionL
// --------------------------------------------------------------------------
//
void CPbk2DeleteMyCardCmd::ConfirmDeletionL()
    {
    HBufC* prompt = 
        StringLoader::LoadLC( R_QTN_PHOB_MY_CARD_CLEAR_CONFIRM );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if( dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt ) )
        {
        // Continue with starting the deletion
        IssueRequest( EStarting );
        }
    else
        {
        IssueRequest( ECanceling );
        }
    CleanupStack::PopAndDestroy( prompt );
    }

//  End of File
