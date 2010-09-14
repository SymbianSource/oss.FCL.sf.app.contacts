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
*           Methods for Phonebook contact deletion query
*
*/


// INCLUDE FILES
#include "CPbkDeleteContactQuery.h"
#include <pbkview.rsg>
#include <StringLoader.h>
#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>

namespace {

// LOCAL CONSTANTS
enum TPanicCode
    {
    CPbkDeleteContactQuery_RunLD =1
    };

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkDeleteContactQuery");
    User::Panic(KPanicText,aReason);
    }

#endif

}  // namespace


// ==================== MEMBER FUNCTIONS ====================

inline CPbkDeleteContactQuery::CPbkDeleteContactQuery()
    {
    }

EXPORT_C CPbkDeleteContactQuery* CPbkDeleteContactQuery::NewLC()
    {
    CPbkDeleteContactQuery* self = new(ELeave) CPbkDeleteContactQuery;
    self->PrepareLC(R_PBK_DELETE_QUERY);
    return self;
    }

CPbkDeleteContactQuery::~CPbkDeleteContactQuery()
    {
    }

EXPORT_C TInt CPbkDeleteContactQuery::RunLD(const CPbkContactItem& aContact)
    {
    CreatePromptL(aContact);
    return CAknQueryDialog::RunLD();
    }

EXPORT_C TInt CPbkDeleteContactQuery::RunLD
        (CPbkContactEngine& aEngine, TContactItemId aContactId)
    {
    CPbkContactItem* item = aEngine.ReadContactLC(aContactId);
    CreatePromptL(*item);
    CleanupStack::PopAndDestroy(item);
    return CAknQueryDialog::RunLD();
    }

EXPORT_C TInt CPbkDeleteContactQuery::RunLD
        (CPbkContactEngine& aEngine, const CContactIdArray& aContactIds)
    {
    const TInt count = aContactIds.Count();

    __ASSERT_DEBUG(count >= 0, Panic(CPbkDeleteContactQuery_RunLD));

    if (count == 0)
        {
        return 0;
        }
    else if (count == 1)
        {
        return RunLD(aEngine, aContactIds[0]);
        }
    else 
        {
        CreatePromptL(aContactIds);
        return CAknQueryDialog::RunLD();
        }
    }

void CPbkDeleteContactQuery::CreatePromptL(const CPbkContactItem& aContact)
    {
    HBufC* name = aContact.GetContactTitleL();
    CleanupStack::PushL(name);
    HBufC* prompt = StringLoader::LoadLC(R_QTN_QUERY_COMMON_CONF_DELETE, *name);
    CAknQueryDialog::SetPromptL(*prompt);
    CleanupStack::PopAndDestroy(2, name);
    }

void CPbkDeleteContactQuery::CreatePromptL(const CContactIdArray& aContactIds)
    {
    HBufC* prompt = StringLoader::LoadLC
        (R_QTN_PHOB_QUERY_DELETE_N_ENTRIES, aContactIds.Count());
    CAknQueryDialog::SetPromptL(*prompt);
    CleanupStack::PopAndDestroy(prompt);
    }

// End of File
