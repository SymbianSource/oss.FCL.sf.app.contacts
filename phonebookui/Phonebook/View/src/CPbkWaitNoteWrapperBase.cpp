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
*       Provides methods for a wait note wrapper.
*
*/


// INCLUDE FILES
#include <CPbkWaitNoteWrapperBase.h>
#include <eikprogi.h>
#include <AknWaitDialog.h>
#include <MPbkBackgroundProcess.h>
#include <PbkDebug.h>

#ifdef __WINS__
// Disable warning from CPbkWaitNoteWrapperBase constructor
#pragma warning( disable : 4355 )
#endif

namespace {

#ifdef _DEBUG
// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_ExecuteL_MPbkBackgroundProcess = 1
    };

// ================= LOCAL FUNCTIONS =======================
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText,"CPbkWaitNoteWrapperBase");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG
}


// ================= MEMBER FUNCTIONS =======================

// CPbkWaitNoteWrapperBase::CIdleCallback
inline CPbkWaitNoteWrapperBase::CIdleCallback::CIdleCallback
        (CPbkWaitNoteWrapperBase& aWaitNoteWrapper) :
    CActive(EPriorityIdle),
    iWaitNoteWrapper(aWaitNoteWrapper)
    {
    CActiveScheduler::Add(this);
    }

CPbkWaitNoteWrapperBase::CIdleCallback::~CIdleCallback()
    {
    Cancel();
    }

void CPbkWaitNoteWrapperBase::CIdleCallback::IssueRequest()
    {
    TRequestStatus* sp = &iStatus;
    User::RequestComplete(sp, KErrNone);
    SetActive();
    }

void CPbkWaitNoteWrapperBase::CIdleCallback::DoCancel()
    {
    }

void CPbkWaitNoteWrapperBase::CIdleCallback::RunL()
    {
    iWaitNoteWrapper.IdleRunL();
    }

TInt CPbkWaitNoteWrapperBase::CIdleCallback::RunError(TInt aError)
    {
    return iWaitNoteWrapper.IdleRunError(aError);
    }


// CPbkWaitNoteWrapperBase::TNoteParams
EXPORT_C CPbkWaitNoteWrapperBase::TNoteParams::TNoteParams() :
    iVisibilityDelayOff(EFalse),
    iTone(CAknNoteDialog::ENoTone),
    iText(NULL),
    iObserver(NULL)
    {
	}

// CPbkWaitNoteWrapperBase
CPbkWaitNoteWrapperBase::CPbkWaitNoteWrapperBase() :
    iIdleCallback(*this), iNoteOpen(EFalse)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::CPbkWaitNoteWrapperBase(0x%x)"),this);
    }

CPbkWaitNoteWrapperBase::~CPbkWaitNoteWrapperBase()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::~CPbkWaitNoteWrapperBase(0x%x)"),this);

	// Wait dialog gets deleted automatically, but delete it by force
	// if it somehow still exists
	if (iWaitDialog)
		{
		delete iWaitDialog;
		iWaitDialog=NULL;
		}
    Cancel();
    }

EXPORT_C void CPbkWaitNoteWrapperBase::DisplayL
        (TInt aResId, 
        const TNoteParams& aNoteParams /*=TNoteParams()*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkWaitNoteWrapperBase::DisplayL(0x%x,%d)"),
        this, aResId);

    Cancel();
    iObserver = aNoteParams.iObserver;
    iWaitDialog = CreateDialogL
        (reinterpret_cast<CEikDialog**>(&iWaitDialog),
		aNoteParams.iVisibilityDelayOff);
    iWaitDialog->SetCallback(this);
    iWaitDialog->SetTone(aNoteParams.iTone);
    iWaitDialog->PrepareLC(aResId);
    if (aNoteParams.iText)
        {
        iWaitDialog->SetTextL(*aNoteParams.iText);
        }
	iNoteOpen = ETrue;
    iWaitDialog->RunLD();
    }

EXPORT_C void CPbkWaitNoteWrapperBase::Execute
        (MPbkBackgroundProcess& aProcess)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkWaitNoteWrapperBase::Execute(0x%x,0x%x)"),
        this, &aProcess);
    __ASSERT_DEBUG(iWaitDialog && !iBackgroundProcess,
        Panic(EPanicPreCond_ExecuteL_MPbkBackgroundProcess));

    iBackgroundProcess = &aProcess;
    // Request IdleRunL call
    iIdleCallback.IssueRequest();
    }

EXPORT_C void CPbkWaitNoteWrapperBase::ExecuteL
        (MPbkBackgroundProcess& aProcess,
        TInt aResId, 
        const TNoteParams& aNoteParams /*=TNoteParams()*/)
    {
    DisplayL(aResId,aNoteParams);
    Execute(aProcess);
    }

EXPORT_C TBool CPbkWaitNoteWrapperBase::IsNoteClosed() const
    {
    return (!iNoteOpen);
    }

EXPORT_C void CPbkWaitNoteWrapperBase::SetProgressDialogCallback
        (MProgressDialogCallback& aCallback)
    {
    iDialogCallbackObserver = &aCallback;
    }

void CPbkWaitNoteWrapperBase::Cancel()
    {
    iIdleCallback.Cancel();
    ProcessCanceled();
    }

void CPbkWaitNoteWrapperBase::IdleRunL()
    {
    if (!iBackgroundProcess)
        {
        return;
        }

    if (ShowProgressInfo() && !iProgressInfoControl)
        {
		if (iNoteOpen)
			{
			iProgressInfoControl = iWaitDialog->GetProgressInfoL();
			}
        }
    if (iProgressInfoControl)
        {
        const TInt numberOfSteps = iBackgroundProcess->TotalNumberOfSteps();
        if (numberOfSteps > 0 && 
            iProgressInfoControl->Info().iFinalValue != numberOfSteps)
            {
            iProgressInfoControl->SetFinalValue(numberOfSteps);
            }
        }

    if (iBackgroundProcess->IsProcessDone())
        {
        ProcessFinished();
        }
    else if (!iNoteOpen)
        {
        ProcessCanceled();
        }
    else
        {
        // Do one step of processing
        iBackgroundProcess->StepL();
        // Update progress info
        if (iProgressInfoControl)
            {
            iProgressInfoControl->IncrementAndDraw(1);
            }
        // Request next cycle
        iIdleCallback.IssueRequest();
        }
    }

TInt CPbkWaitNoteWrapperBase::IdleRunError
        (TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::IdleRunError(0x%x,%d)"),this,aError);

    TInt translatedError = aError;
    if (iBackgroundProcess)
        {
        translatedError = iBackgroundProcess->HandleStepError(aError);
        }

    if (translatedError == KErrNone)
        {
        // Continue processing
        iIdleCallback.IssueRequest();
        }
    else
        {
        if (iBackgroundProcess)
            {
            MPbkBackgroundProcess& process = *iBackgroundProcess;
            iBackgroundProcess = NULL;
            KillNote();
            NotifyObserver(process);
            }
        }
 
    return translatedError;
    }

void CPbkWaitNoteWrapperBase::DialogDismissedL
        (TInt aButtonId)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::DialogDismissedL(0x%x)"),this);
    
    if (iDialogCallbackObserver)
        {
        iDialogCallbackObserver->DialogDismissedL(aButtonId);
        }

	iNoteOpen = EFalse;
    ProcessCanceled();
    }

/**
 * Closes wait note gracefully.
 */
void CPbkWaitNoteWrapperBase::CloseNote()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::CloseNote(0x%x)"),this);

    if (iWaitDialog)
        {
        iWaitDialog->SetCallback(NULL);
        TRAPD(err,iWaitDialog->ProcessFinishedL());
        if (err != KErrNone)
            {
            // Graceful close was unsuccesful -> stop
			// being nice and kill the dialog
            KillNote();
            }
		iNoteOpen = EFalse;
        }
	}

/**
 * Closes wait note immediately.
 */
void CPbkWaitNoteWrapperBase::KillNote()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::KillNote(0x%x)"),this);

    if (iWaitDialog)
        {
        iWaitDialog->SetCallback(NULL);
		delete iWaitDialog;
		iWaitDialog=NULL;
        }

	iNoteOpen = EFalse;
    }

void CPbkWaitNoteWrapperBase::ProcessFinished()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::ProcessFinished(0x%x)"),this);

    if (iBackgroundProcess)
        {
        MPbkBackgroundProcess& process = *iBackgroundProcess;
        iBackgroundProcess = NULL;
        process.ProcessFinished();
        CloseNote();
        NotifyObserver(process);
		}
    }

void CPbkWaitNoteWrapperBase::ProcessCanceled()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkWaitNoteWrapperBase::ProcessCanceled(0x%x)"),this);

    if (iBackgroundProcess)
        {
        MPbkBackgroundProcess& process = *iBackgroundProcess;
        iBackgroundProcess = NULL;
        process.ProcessCanceled();
		// Do not kill the note, if it already has been done
		if (iNoteOpen)
			{
			KillNote();
			}
        NotifyObserver(process);
        }
    }

/**
 * Notifies observer about process completion.
 */
void CPbkWaitNoteWrapperBase::NotifyObserver(MPbkBackgroundProcess& aProcess)
    {
    if (iObserver)
        {
        MPbkProcessObserver& observer = *iObserver;
        iObserver = NULL;
        observer.ProcessFinished(aProcess);
        }
    }

//  End of File  
