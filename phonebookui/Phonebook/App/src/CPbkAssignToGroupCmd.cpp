/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



// INCLUDE FILES
#include "CPbkAssignToGroupCmd.h"
#include <eikprogi.h>
#include <StringLoader.h>
#include <phonebook.rsg>
#include <CPbkAssignToGroup.h>
#include <CPbkContactViewListControl.h>
#include <MPbkCommandObserver.h>
#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAssignToGroupCmd");
    User::Panic(KPanicText,aReason);
    }
#endif

}


// ================= MEMBER FUNCTIONS =======================

inline CPbkAssignToGroupCmd::CPbkAssignToGroupCmd
        (CPbkContactEngine& aEngine,
        CPbkContactViewListControl& aUiControl) :
    CActive(EPriorityIdle),
    iEngine(aEngine),
    iUiControl(aUiControl),
    iFocusId(KNullContactId),
    iDialogDismissed(ETrue) // there is no dialog at this point
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::CPbkAssignToGroupCmd(0x%x)"), 
        this);

    CActiveScheduler::Add(this);
    }

inline void CPbkAssignToGroupCmd::ConstructL
        (const CContactIdArray& aContacts,
        TContactItemId aGroupId)
    {
    iAssignToGroupProcess = CPbkAssignToGroup::NewLC(iEngine, aContacts, aGroupId);
    CleanupStack::Pop(iAssignToGroupProcess);
    }

CPbkAssignToGroupCmd* CPbkAssignToGroupCmd::NewL
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        CPbkContactViewListControl& aUiControl,
        TContactItemId aGroupId)
    {
    CPbkAssignToGroupCmd* self = new(ELeave) CPbkAssignToGroupCmd(aEngine, aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts, aGroupId);
    CleanupStack::Pop(self);
    return self;
    }

void CPbkAssignToGroupCmd::ResetWhenDestroyed
        (CPbkAssignToGroupCmd** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

CPbkAssignToGroupCmd::~CPbkAssignToGroupCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::~CPbkAssignToGroupCmd(0x%x)"), 
        this);

    Cancel();

    if ( !iRedButtonPressed )
        {
        // Unblank and redraw UI control
        iUiControl.SetBlank(EFalse);
        }

    DeleteProgressNote();
    delete iAssignToGroupProcess;

    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    iDestroyed = ETrue;
    }

void CPbkAssignToGroupCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::ExecuteLD(0x%x)"), 
        this);
    CleanupStack::PushL(this);

    // save focus 
    iFocusId = iUiControl.FocusedContactIdL();

    // Blank UI control to avoid flicker
    iUiControl.SetBlank(ETrue);

    // delete previous note    
    DeleteProgressNote();

    // Start assignation process. ProcessFinished() will be called when execution
    // is finished.
    CAknProgressDialog* progressDialog = new(ELeave) CAknProgressDialog(
            reinterpret_cast<CEikDialog**>(NULL), ETrue);
    progressDialog->PrepareLC(R_QTN_PHOB_WNOTE_ADD_TO_GROUP);
    iProgressDlgInfo = progressDialog->GetProgressInfoL();
    iProgressDlgInfo->SetFinalValue(iAssignToGroupProcess->TotalNumberOfSteps());
    progressDialog->SetCallback(this);
    iDialogDismissed = EFalse;
    progressDialog->RunLD();

    iProgressDialog = progressDialog;

    // issue request for entry assignation
    IssueRequest();

    CleanupStack::Pop(this);
    }

void CPbkAssignToGroupCmd::AddObserver
        (MPbkCommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }
   
void CPbkAssignToGroupCmd::DoCancel()
    {
    }

void CPbkAssignToGroupCmd::RunL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::StepL()"), 
        this);

    if (!iAssignToGroupProcess->IsProcessDone() && !iDialogDismissed)
        {
        // process one step
        iAssignToGroupProcess->StepL();
        ++iAssignedCount;
        
        // Incrementing progress of the process
        iProgressDlgInfo->SetAndDraw(iAssignedCount);

        // issue request to delete next item
        IssueRequest();
        }
    else
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroupCmd::RunL process completed start"));

        // process is completed, all entries have been assigned
        ProcessFinished(*iAssignToGroupProcess);

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroupCmd::RunL process completed end"));
        }

    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::RunL end")); 
    }

TInt CPbkAssignToGroupCmd::RunError
        (TInt aError)
    {
    TInt result = iAssignToGroupProcess->HandleStepError(aError);
    Cancel();
    ProcessFinished(*iAssignToGroupProcess);
    return result;
    }
     
void CPbkAssignToGroupCmd::ProcessFinished
        (MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroupCmd::ProcessFinished(0x%x)"), 
        this);

    if ( !iRedButtonPressed )
        {
        // Clear listbox selections
        iUiControl.HandleMarkableListUpdateAfterCommandExecution();

        // restore focus 
        if (iFocusId != KNullContactId)
            {
            TInt index = -1;
            TRAP_IGNORE(index = iUiControl.FindContactIdL(iFocusId));
            if (index >= 0)
                {
                iUiControl.SetCurrentItemIndex(index);
                }
            }

        // Unblank and redraw UI control
        iUiControl.SetBlank(EFalse);
        iUiControl.DrawNow();
        }

    DeleteProgressNote();
    }

void CPbkAssignToGroupCmd::DeleteProgressNote()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroupCmd::DeleteProgressNote start"));

    if (iProgressDialog && !iDialogDismissed)
        {
        // deletes the dialog
        TRAPD(err, iProgressDialog->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iProgressDialog;
            }
        iProgressDialog = NULL;
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroupCmd::DeleteProgressNote end"));
    }

void CPbkAssignToGroupCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    } 

void CPbkAssignToGroupCmd::DialogDismissedL( TInt aButtonId )
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroupCmd::DialogDismissedL"));
    iDialogDismissed = ETrue;
    if ( aButtonId == KErrNotFound )
        {
        iRedButtonPressed = ETrue;        
        }
    
    // notify command owner that the command has finished
    iCommandObserver->CommandFinished(*this);
    }

//  End of File  
