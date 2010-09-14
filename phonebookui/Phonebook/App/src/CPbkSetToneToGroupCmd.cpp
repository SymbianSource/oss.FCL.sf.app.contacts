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
*          Provides phonebook assign ringing tone to group members command methods.
*
*/


// INCLUDE FILES
#include "CPbkSetToneToGroupCmd.h"
#include <StringLoader.h>
#include <eikprogi.h>
#include <AknNoteWrappers.h>
#include <CPbkContactEngine.h>
#include <cntitem.h>
#include <phonebook.rsg>
#include <pbkview.rsg>
#include <CPbkSetToneToGroup.h>
#include <MPbkCommandObserver.h>


#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1,
    EPanicLogic_ConstructL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSetToneToGroupCmd");
    User::Panic(KPanicText,aReason);
    }
#endif

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkSetToneToGroupCmd::CPbkSetToneToGroupCmd
        (CPbkContactEngine& aEngine) :
    CActive(EPriorityIdle),
    iEngine(aEngine),
    iDialogDismissed(ETrue) // there is no dialog at this point
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::CPbkSetToneToGroupCmd(0x%x)"), 
        this);
    
    CActiveScheduler::Add(this);
    }

inline void CPbkSetToneToGroupCmd::ConstructL
        (TContactItemId aGroupId,
        const TDesC& aRingingToneName)
    {
    CContactGroup* group = iEngine.ReadContactGroupL(aGroupId);
    CleanupStack::PushL(group);
    const CContactIdArray* groupContainsIds = group->ItemsContained();

    //Logic:
    __ASSERT_DEBUG(groupContainsIds, Panic(EPanicLogic_ConstructL));

    iSetToneToGroupProcess = CPbkSetToneToGroup::NewLC
		(iEngine, *groupContainsIds, aRingingToneName);
    CleanupStack::Pop(iSetToneToGroupProcess);

    CleanupStack::PopAndDestroy(); // group
    }

CPbkSetToneToGroupCmd* CPbkSetToneToGroupCmd::NewL
        (CPbkContactEngine& aEngine,
        TContactItemId aGroupId,
        const TDesC& aRingingToneName)
    {
    CPbkSetToneToGroupCmd* self = new(ELeave) CPbkSetToneToGroupCmd(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aGroupId, aRingingToneName);
    CleanupStack::Pop(self);
    return self;
    }

void CPbkSetToneToGroupCmd::ResetWhenDestroyed
        (CPbkSetToneToGroupCmd** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

CPbkSetToneToGroupCmd::~CPbkSetToneToGroupCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::~CPbkSetToneToGroupCmd(0x%x)"), 
        this);

    Cancel();
    DeleteProgressNote();
    delete iSetToneToGroupProcess;

    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    iDestroyed = ETrue;
    }

void CPbkSetToneToGroupCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::ExecuteLD(0x%x)"), 
        this);
    CleanupStack::PushL(this);

    // delete previous note    
    DeleteProgressNote();

    // Start tone setting process. ProcessFinished() will be called when execution
    // is finished.
    CAknProgressDialog* progressDialog = new(ELeave) CAknProgressDialog(
            reinterpret_cast<CEikDialog**>(NULL), ETrue);
    progressDialog->PrepareLC(R_QTN_PHOB_WNOTE_SET_RTONE);
    iProgressDlgInfo = progressDialog->GetProgressInfoL();
    iProgressDlgInfo->SetFinalValue(iSetToneToGroupProcess->TotalNumberOfSteps());
    progressDialog->SetCallback(this);
    iDialogDismissed = EFalse;
    progressDialog->RunLD();

    iProgressDialog = progressDialog;

    // issue request for entry assignation
    IssueRequest();

    CleanupStack::Pop(); // this
    }

void CPbkSetToneToGroupCmd::AddObserver
        (MPbkCommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

void CPbkSetToneToGroupCmd::DoCancel()
    {
    }

void CPbkSetToneToGroupCmd::RunL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::StepL()"), 
        this);

    if (!iSetToneToGroupProcess->IsProcessDone() && !iDialogDismissed)
        {
        // process one step
        iSetToneToGroupProcess->StepL();
        ++iSetCount;
        
        // Incrementing progress of the process
        iProgressDlgInfo->SetAndDraw(iSetCount);

        // issue request to delete next item
        IssueRequest();
        }
    else
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::RunL process completed start"));

        // show process completion note
        TParse toneName;
        toneName.Set(iSetToneToGroupProcess->RingingToneFile(), NULL, NULL);
    
        HBufC* noteText = NULL;
        if (toneName.Name().Length() == 0)
            {
            HBufC* defaultName = CCoeEnv::Static()->AllocReadResourceLC(R_QTN_PHOP_SELI_DEFAULT_RTONE);
            noteText = StringLoader::LoadL(R_QTN_PHOB_NOTE_RTONE_SET_TO_GRP, *defaultName);
            CleanupStack::PopAndDestroy(); // defaultName
            CleanupStack::PushL(noteText);
            }
        else
            {
            noteText = StringLoader::LoadLC(R_QTN_PHOB_NOTE_RTONE_SET_TO_GRP, toneName.Name());
            }

        CAknConfirmationNote* noteDlg = new (ELeave) CAknConfirmationNote;
        noteDlg->ExecuteLD(*noteText);
        CleanupStack::PopAndDestroy(); // notetext

        // process is completed, all entries have been set
        ProcessFinished(*iSetToneToGroupProcess);

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::RunL process completed end"));
        }

    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::RunL end")); 
    }

TInt CPbkSetToneToGroupCmd::RunError
        (TInt aError)
    {
    TInt result = iSetToneToGroupProcess->HandleStepError(aError);
    Cancel();
    ProcessFinished(*iSetToneToGroupProcess);
    return result;    
    }
        
void CPbkSetToneToGroupCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::ProcessFinished(0x%x)"), 
        this);

    DeleteProgressNote();
    }

void CPbkSetToneToGroupCmd::DeleteProgressNote()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::DeleteProgressNote start"));

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

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::DeleteProgressNote end"));
    }

void CPbkSetToneToGroupCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkSetToneToGroupCmd::DialogDismissedL
        (TInt /*aButtonId*/)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroupCmd::DialogDismissedL"));
    iDialogDismissed = ETrue;
    
    // notify command owner that the command has finished
    iCommandObserver->CommandFinished(*this);
    }

//  End of File  
