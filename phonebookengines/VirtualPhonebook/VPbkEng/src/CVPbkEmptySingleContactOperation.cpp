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
* Description:  Virtual Phonebook Empty single contact operation.
*
*/


#include "CVPbkEmptySingleContactOperation.h"
#include <MVPbkSingleContactOperationObserver.h>

CVPbkEmptySingleContactOperation::CVPbkEmptySingleContactOperation(
        MVPbkSingleContactOperationObserver& aObserver, TInt aErrorCode) :
    CActive(CActive::EPriorityIdle),
    iObserver(aObserver),
    iErrorCode( aErrorCode )
    {
    CActiveScheduler::Add(this);
    }

CVPbkEmptySingleContactOperation::~CVPbkEmptySingleContactOperation()
    {
    Cancel();
    }

CVPbkEmptySingleContactOperation* CVPbkEmptySingleContactOperation::NewL(
        MVPbkSingleContactOperationObserver& aObserver, TInt aErrorCode)
    {
    CVPbkEmptySingleContactOperation* self = 
        new(ELeave) CVPbkEmptySingleContactOperation( aObserver, aErrorCode );
    return self;
	}

void CVPbkEmptySingleContactOperation::StartL()
    {
    IssueRequest();
    }

void CVPbkEmptySingleContactOperation::Cancel()
    {
    CActive::Cancel();
    }

void CVPbkEmptySingleContactOperation::RunL()
    {
    if ( iErrorCode == KErrNone )
        {
        iObserver.VPbkSingleContactOperationComplete( *this, NULL );
        }
    else
        {
        iObserver.VPbkSingleContactOperationFailed( *this, iErrorCode );
        }
    }

void CVPbkEmptySingleContactOperation::DoCancel()
    {
    }

void CVPbkEmptySingleContactOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }


// End of File
