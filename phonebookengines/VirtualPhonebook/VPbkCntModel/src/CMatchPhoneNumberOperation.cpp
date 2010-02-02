/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook Contact model phone number match operation.
*
*/
 

#include "CMatchPhoneNumberOperation.h"
#include "CContactLink.h"
#include "CContactStore.h"
#include <cntdb.h>

#include <MVPbkContactFindObserver.h>

namespace VPbkCntModel {

CMatchPhoneNumberOperation::CMatchPhoneNumberOperation(
        CContactStore& aContactStore,
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver) :
    CActive(CActive::EPriorityIdle),
    iContactStore(aContactStore),
    iObserver(aObserver),
    iMaxMatchDigits(aMaxMatchDigits)
    {
    CActiveScheduler::Add(this);
    }

inline void CMatchPhoneNumberOperation::ConstructL(const TDesC& aPhoneNumber)
    {
    iPhoneNumber = aPhoneNumber.AllocL();
    iResults = CVPbkContactLinkArray::NewL();
    }

CMatchPhoneNumberOperation* CMatchPhoneNumberOperation::NewL(
        CContactStore& aContactStore,
        const TDesC& aPhoneNumber,
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver)
    {
	CMatchPhoneNumberOperation* self = new(ELeave) CMatchPhoneNumberOperation(
        aContactStore, aMaxMatchDigits, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aPhoneNumber);
    CleanupStack::Pop(self);
	return self;
	}

CMatchPhoneNumberOperation::~CMatchPhoneNumberOperation()
    {
    Cancel();

    delete iPhoneNumber;
    delete iContactIds;
    delete iResults;
    }

void CMatchPhoneNumberOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CMatchPhoneNumberOperation::DoCancel()
    {
    iState = ECancelled;
    if (!IsActive())
        {
        IssueRequest();
        }
    }

void CMatchPhoneNumberOperation::RunL()
    {
    switch (iState)
        {
        case EMatch:
            {
            iContactIds = iContactStore.NativeDatabase().
                MatchPhoneNumberL(*iPhoneNumber, iMaxMatchDigits);
            iState = EBuildLinks;
            IssueRequest();
            break;
            }
        case EBuildLinks:
            {
            if (iContactIds->Count() > 0)
                {
                CContactLink* link = CContactLink::NewLC(iContactStore, (*iContactIds)[0]);
                iContactIds->Remove(0);
                iResults->AppendL(link);
                CleanupStack::Pop(link); // link
                IssueRequest();
                }
            else
                {
                delete iContactIds;
                iContactIds = NULL;
                iState = EComplete;
                IssueRequest();
                }
            break;
            }
        case EComplete:
            {
            // takes ownership of results
            CVPbkContactLinkArray* results = iResults;            
            iResults = NULL;
            iObserver.FindCompleteL(results);
            break;
            }
        case ECancelled:
            {
            iObserver.FindFailed(KErrCancel);
            break;
            }
        }

    }

TInt CMatchPhoneNumberOperation::RunError(TInt aError)
    {
    iObserver.FindFailed(aError);
    return KErrNone;
    }

void CMatchPhoneNumberOperation::StartL()
    {
    IssueRequest();
	}

void CMatchPhoneNumberOperation::Cancel()
    {
    CActive::Cancel();
	}

} // namespace VPbkCntModel
