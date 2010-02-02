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
*		A class for asyncronous phonebook name lookup
*
*/


// INCLUDE FILES

// Phonebook debug
#include    <PbkDebug.h>

// Phonebook engine
#include    "CPbkContactEngine.h"
#include    "CPbkContactItem.h"
#include    "CPbkFieldsInfo.h"
#include    <MPbkContactNameFormat.h>

// Phonebook copy
#include    "CPbkNameLookup.h"


// MODULE DATA STRUCTURES


// ================= MEMBER FUNCTIONS =======================

inline CPbkNameLookup::CPbkNameLookup
        (CPbkContactEngine& aEngine,
        MPbkNameLookupObserver& aObserver) :
    CActive(EPriorityIdle),
    iEngine(aEngine),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CPbkNameLookup* CPbkNameLookup::NewL
        (CPbkContactEngine& aEngine,
        MPbkNameLookupObserver& aObserver)
    {
    return new(ELeave) CPbkNameLookup(aEngine, aObserver);
    }

void CPbkNameLookup::IssueNameLookupL
        (CContactIdArray* aContactIds,
        CPbkContactItem& aEntry)
    {
    Cancel();
    delete iContactIds;
    iContactIds = aContactIds;

    iEntry = &aEntry;

    IssueRequest();
    }

CPbkNameLookup::~CPbkNameLookup()
    {
    Cancel();
    delete iContactIds;
    }

void CPbkNameLookup::DoCancel()
    {
    }

void CPbkNameLookup::RunL()
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkNameLookup::RunL start"));

    TBool result = EFalse;
    // Load the contact item to display information about it
    const TInt index = iContactIds->Count()-1;
    const TContactItemId cid = (*iContactIds)[index];
    CPbkContactItem* item = iEngine.ReadContactLC(cid);
    if (item)
        {
        // comparison is based only to the last name
        HBufC* contactTitle = item->GetContactTitleOrNullL();
        if (contactTitle)
            {
            CleanupStack::PushL(contactTitle);
            HBufC* title = iEntry->GetContactTitleOrNullL();
            if (title)
                {
                CleanupStack::PushL(title);

                if (!title->CompareF(*contactTitle))
                    {
                    iObserver.NameLookupCompleted(item->Id());
                    result = ETrue;
                    }

                CleanupStack::PopAndDestroy(title);
                }
            CleanupStack::PopAndDestroy(contactTitle);
            }            
        }

    CleanupStack::PopAndDestroy(item);

    if (!result)
        {
        iContactIds->Remove(iContactIds->Count()-1);
        IssueRequest();
        }

    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkNameLookup::RunL end"));
    }

TInt CPbkNameLookup::RunError
        (TInt aError)
    {
    iObserver.NameLookupFailed(aError);
    return KErrNone;
    }

void CPbkNameLookup::IssueRequest()
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkNameLookup::IssueRequest start"));

    if (iContactIds->Count() > 0)
        {
        TRequestStatus* sp = &iStatus;
        User::RequestComplete(sp, KErrNone);
        SetActive();
        }
    else
        {
        iObserver.NameLookupCompleted(KNullContactId);
        }

    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkNameLookup::IssueRequest start"));
    }

//  End of File  
