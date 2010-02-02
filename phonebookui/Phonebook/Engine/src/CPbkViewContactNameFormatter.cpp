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
*	CViewContact contact name formatter.
*
*/


// INCLUDES
#include <CPbkViewContactNameFormatter.h>
#include <cntviewbase.h>
#include "PbkViewContactFieldDataAdapter.h"
#include <MPbkContactNameFormat.h>


namespace {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_ContactTitleL =1,
    EPanicPostCond_ReserveBufferL,
    EPanicInvalidContactType
    };

/// Minimum size for the name formatting buffer
const TInt KMinNameBufSize = 32;


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkViewContactNameFormatter");
    User::Panic(KPanicText,aReason);
    }

#endif

}  // namespace



// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkViewContactNameFormatter* CPbkViewContactNameFormatter::NewL
        (MPbkContactNameFormat& aNameFormatter)
    {
    CPbkViewContactNameFormatter* self = 
        new(ELeave) CPbkViewContactNameFormatter(aNameFormatter);
    return self;
    }

CPbkViewContactNameFormatter::CPbkViewContactNameFormatter
        (MPbkContactNameFormat& aNameFormatter) :
    iNameFormatter(aNameFormatter)
    {
    }

CPbkViewContactNameFormatter::~CPbkViewContactNameFormatter()
    {
    delete iNameBuf;
    }

/**
 * Allocates iNameBuf to be able hold at least aLength characters.
 *
 * @return a TPtr pointing to the buffer
 * @precond aSize>=0
 * @postcond result.MaxLength()>=aLength && result.Length()==0
 */
TPtr CPbkViewContactNameFormatter::ReserveBufferL(TInt aLength) const
    {
    const TInt bufLength = Max(aLength,KMinNameBufSize);
    if (iNameBuf)
        {
        TPtr bufPtr(iNameBuf->Des());
        if (bufPtr.MaxLength() < bufLength)
            {
            iNameBuf = iNameBuf->ReAllocL(bufLength);
            }
        }
    else
        {
        iNameBuf = HBufC::NewL(bufLength);
        }

    TPtr result(iNameBuf->Des());
    result.Zero();
    __ASSERT_DEBUG(result.MaxLength()>=aLength && result.Length()==0, 
        Panic(EPanicPostCond_ReserveBufferL));
    return result;
    }

EXPORT_C const TDesC& CPbkViewContactNameFormatter::ContactTitleL
        (const CViewContact& aViewContact, 
        const RContactViewSortOrder& aSortOrder) const
    {
    __ASSERT_DEBUG(aViewContact.FieldCount() == aSortOrder.Count(),
        Panic(EPanicPreCond_ContactTitleL));

    switch (aViewContact.ContactType())
        {
        case CViewContact::EContactItem:
            {
            TPbkViewContactFieldDataArray fieldData(aViewContact,aSortOrder);
            const TInt nameLen = iNameFormatter.ContactListTitleLength(fieldData);
            TPtr bufPtr(ReserveBufferL(nameLen));
            iNameFormatter.GetContactListTitle(fieldData,bufPtr);
            break;
            }

        case CViewContact::EGroup:
            {
            TPtrC title;
            const TInt fieldCount = aSortOrder.Count();
            for (TInt i=0; i < fieldCount; ++i)
                {
                if (aSortOrder[i] == KUidContactFieldTemplateLabel)
                    {
                    title.Set(aViewContact.Field(i));
                    break;
                    }
                }
            TPtr bufPtr(ReserveBufferL(title.Length()));
            bufPtr.Copy(title);
            break;
            }

        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidContactType));
            return KNullDesC;
            }
        }

    return (*iNameBuf);
    }


// End of File
