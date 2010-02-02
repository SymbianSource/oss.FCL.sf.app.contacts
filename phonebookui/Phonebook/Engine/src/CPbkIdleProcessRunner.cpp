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
*      Executes a MPbkBackgroundProcess in an idle loop.
*
*/


// INCLUDE FILES
#include "CPbkIdleProcessRunner.h"
#include <MPbkBackgroundProcess.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkIdleProcessRunner::CPbkIdleProcessRunner
        (TInt aPriority) :
    iRunner(aPriority)
    {
    }

CPbkIdleProcessRunner* CPbkIdleProcessRunner::NewL(TInt aPriority)
    {
    CPbkIdleProcessRunner* self = new(ELeave) CPbkIdleProcessRunner(aPriority);
    return self;
    }

CPbkIdleProcessRunner::~CPbkIdleProcessRunner()
    {
    }

void CPbkIdleProcessRunner::Execute
        (MPbkBackgroundProcess& aProcess, MPbkProcessObserver* aObserver)
    {
    iRunner.Start(aProcess,aObserver);
    }

void CPbkIdleProcessRunner::SynchronousExecuteL
        (MPbkBackgroundProcess& aProcess)
    {
    iRunner.RunSyncL(aProcess);
    }

void CPbkIdleProcessRunner::Cancel()
    {
    iRunner.Stop();
    }

CPbkIdleProcessRunner::CRunner::CRunner(TInt aPriority) :
    CActive(aPriority)
    {
    CActiveScheduler::Add(this);
    }

CPbkIdleProcessRunner::CRunner::~CRunner()
    {
    Stop();
    }

void CPbkIdleProcessRunner::CRunner::Start
        (MPbkBackgroundProcess& aProcess, 
        MPbkProcessObserver* aObserver)
    {
    Stop();
    iProcess = &aProcess;
    iObserver = aObserver;
    iProcessError = NULL;
    IssueRequest();
    }

void CPbkIdleProcessRunner::CRunner::RunSyncL(MPbkBackgroundProcess& aProcess)
    {
    Stop();
    iProcess = &aProcess;
    iObserver = NULL;
    TInt error = KErrNone;
    iProcessError = &error;

    IssueRequest();
    iActiveWait.Start();

    if (error != KErrNone)
        {
        User::Leave(error);
        }
    }

void CPbkIdleProcessRunner::CRunner::Stop()
    {
    Cancel();
    ProcessCanceled();
    }

void CPbkIdleProcessRunner::CRunner::DoCancel()
    {
    ProcessCanceled();
    }

void CPbkIdleProcessRunner::CRunner::RunL()
    {
    if (!iProcess)
        {
        return;
        }

    if (iProcess->IsProcessDone())
        {
        ProcessFinished();
        }
    else
        {
        iProcess->StepL();
        IssueRequest();
        }
    }

/**
 * Handles leaves in RunL().
 */
TInt CPbkIdleProcessRunner::CRunner::RunError(TInt aError)
    {
    TInt translatedError = aError;
    if (iProcess)
        {
        translatedError = iProcess->HandleStepError(aError);
        }
    if (iProcessError)
        {
        *iProcessError = translatedError;
        }

    if (translatedError == KErrNone)
        {
        // Continue processing
        IssueRequest();
        }
    else
        {
        MPbkBackgroundProcess* process = iProcess;
        iProcess = NULL;
        Finalize(process);
        }
 
    return translatedError;
    }

void CPbkIdleProcessRunner::CRunner::ProcessFinished()
    {
    MPbkBackgroundProcess* process = iProcess;
    iProcess = NULL;
    if (process)
        {
        process->ProcessFinished();
        }
    Finalize(process);
    }

void CPbkIdleProcessRunner::CRunner::ProcessCanceled()
    {
    MPbkBackgroundProcess* process = iProcess;
    iProcess = NULL;
    if (process)
        {
        process->ProcessCanceled();
        }
    Finalize(process);
    }

/**
 * Notifies observer about process completion.
 */
void CPbkIdleProcessRunner::CRunner::Finalize
        (MPbkBackgroundProcess* aProcess)
    {
    if (iActiveWait.IsStarted())
        {
        iActiveWait.AsyncStop();
        }
    if (iObserver && aProcess)
        {
        MPbkProcessObserver* observer = iObserver;
        iObserver = NULL;
        observer->ProcessFinished(*aProcess);
        }
    }

/**
 * Issues a request for RunL() to be called from idle.
 */
void CPbkIdleProcessRunner::CRunner::IssueRequest()
    {
    TRequestStatus* sp = &iStatus;
    User::RequestComplete(sp, KErrNone);
    SetActive();
    }


//  End of File  
