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
*		Used in contact name formatting.
*
*/

// INCLUDES
#include "PbkViewContactFieldDataAdapter.h"
#include "PbkEng.hrh"
#include <cntviewbase.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicInvalidFieldType =1,
    EPanicInvalidFieldId,
    EPanicPreCond_Constructor
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "PbkViewContactFieldDataAdapter");
    User::Panic(KPanicText,aReason);
    }

}  // namespace



// ================= MEMBER FUNCTIONS =======================

// TPbkViewContactFieldDataArray::TFieldData

inline TPbkViewContactFieldDataArray::TFieldData::TFieldData() :
    iFieldId(EPbkFieldIdNone)
    {
    }

inline void TPbkViewContactFieldDataArray::TFieldData::Set
        (TPbkFieldId aFieldId, const TDesC& aText)
    {
    iFieldId = aFieldId;
    iText.Set(aText);
    }

TStorageType TPbkViewContactFieldDataArray::TFieldData::PbkFieldType() const
    {
    return KStorageTypeText;
    }

TPbkFieldId TPbkViewContactFieldDataArray::TFieldData::PbkFieldId() const
    {
    return iFieldId;
    }

TPtrC TPbkViewContactFieldDataArray::TFieldData::PbkFieldText() const
    {
    return iText;
    }

TTime TPbkViewContactFieldDataArray::TFieldData::PbkFieldTime() const
    {
    Panic(EPanicInvalidFieldType);
    return Time::NullTTime();
    }


// TPbkViewContactFieldDataArray
EXPORT_C TPbkViewContactFieldDataArray::TPbkViewContactFieldDataArray
        (const CViewContact& aViewContact, 
        const RContactViewSortOrder& aSortOrder) :
    iViewContact(aViewContact), iSortOrder(aSortOrder)
    {
    __ASSERT_ALWAYS(aViewContact.FieldCount() == aSortOrder.Count(),
        Panic(EPanicPreCond_Constructor));
    }

EXPORT_C TPbkViewContactFieldDataArray::~TPbkViewContactFieldDataArray()
    {
    }

TInt TPbkViewContactFieldDataArray::PbkFieldCount() const
    {
    return (iViewContact.FieldCount());
    }

MPbkFieldData& TPbkViewContactFieldDataArray::PbkFieldAt(TInt aIndex)
    {
    iField.Set(FieldId(aIndex), iViewContact.Field(aIndex));
    return iField;
    }

const MPbkFieldData& TPbkViewContactFieldDataArray::PbkFieldAt(TInt aIndex) const
    {
    iField.Set(FieldId(aIndex), iViewContact.Field(aIndex));
    return iField;
    }

TPbkFieldId TPbkViewContactFieldDataArray::FieldId(TInt aIndex) const
    {
    switch (iSortOrder[aIndex].iUid)
        {
        case KUidContactFieldTopContactValue:
            {
            // Top Contact is a new attribute of Phonebook added by ECE, as it's not a
            // field, just return EPbkFieldIdNone here.
            return EPbkFieldIdNone;
            }
        case KUidContactFieldFamilyNameValue:
            {
            return EPbkFieldIdLastName;
            }
        case KUidContactFieldGivenNameValue:
            {
            return EPbkFieldIdFirstName;
            }
        case KUidContactFieldCompanyNameValue:
            {
            return EPbkFieldIdCompanyName;
            }
        case KUidContactFieldFamilyNamePronunciationValue:
            {
            return EPbkFieldIdLastNameReading;
            }
        case KUidContactFieldGivenNamePronunciationValue:
            {
            return EPbkFieldIdFirstNameReading;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidFieldId));
            return EPbkFieldIdNone;
            }
        }
    }


// End of File
