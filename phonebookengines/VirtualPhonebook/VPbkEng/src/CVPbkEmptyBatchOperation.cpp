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
* Description:  Virtual Phonebook Empty batch operation.
*
*/


#include "CVPbkEmptyBatchOperation.h"

CVPbkEmptyBatchOperation::CVPbkEmptyBatchOperation(
        MVPbkBatchOperationObserver& aObserver) :
    CActive(CActive::EPriorityIdle),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CVPbkEmptyBatchOperation::~CVPbkEmptyBatchOperation()
    {
    Cancel();
    }

EXPORT_C CVPbkEmptyBatchOperation* CVPbkEmptyBatchOperation::NewLC(
        MVPbkBatchOperationObserver& aObserver)
    {
    CVPbkEmptyBatchOperation* self = new(ELeave) CVPbkEmptyBatchOperation(aObserver);
    CleanupStack::PushL(self);
    return self;
	}

void CVPbkEmptyBatchOperation::StartL()
    {
    IssueRequest();
    }

void CVPbkEmptyBatchOperation::Cancel()
    {
    CActive::Cancel();
    }

void CVPbkEmptyBatchOperation::RunL()
    {
    iObserver.OperationComplete(*this);
    }

void CVPbkEmptyBatchOperation::DoCancel()
    {
    }

void CVPbkEmptyBatchOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }


// End of File
