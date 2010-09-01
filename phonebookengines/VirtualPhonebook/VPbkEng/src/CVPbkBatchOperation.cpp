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
* Description:  An implementation for operations that needs to do a big task
*                and report steps to observers. Can be used e.g
*                in store domain implementation to handle multiple store
*                operations
*
*/


#include "CVPbkBatchOperation.h"

CVPbkBatchOperation::CVPbkBatchOperation(
        MVPbkBatchOperationObserver& aObserver) :
    iObserver(aObserver)
    {
    }

CVPbkBatchOperation::~CVPbkBatchOperation()
    {
    }

EXPORT_C CVPbkBatchOperation* CVPbkBatchOperation::NewLC(
        MVPbkBatchOperationObserver& aObserver)
    {
    CVPbkBatchOperation* self = new(ELeave) CVPbkBatchOperation(aObserver);
    CleanupStack::PushL(self);
    return self;
	}

void CVPbkBatchOperation::HandleZeroSuboperations()
    {
    // Called from the base class if there were no suboperations
    iObserver.OperationComplete( *this );
    }
    
void CVPbkBatchOperation::StepComplete(MVPbkContactOperationBase& /*aOperation*/, TInt aStepSize)
    {
    iObserver.StepComplete(*this, aStepSize);
    }

TBool CVPbkBatchOperation::StepFailed(MVPbkContactOperationBase& /*aOperation*/, TInt aStepSize, TInt aError)
    {
    return iObserver.StepFailed(*this, aStepSize, aError);
    }

void CVPbkBatchOperation::OperationComplete(MVPbkContactOperationBase& /*aOperation*/)
    {
    ++iCompleteCount;

    if (iCompleteCount >= SubOperationCount())
        {
        iObserver.OperationComplete(*this);
        }
    }

// End of File
