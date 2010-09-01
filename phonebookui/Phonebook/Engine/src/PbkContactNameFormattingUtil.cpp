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
*		Phonebook contact name formatting base
*
*/


// INCLUDE FILES
#include "PbkContactNameFormattingUtil.h"
#include <cntviewbase.h>
#include <PbkEngUtils.h>
#include "MPbkFieldData.h"


// ==================== MEMBER FUNCTIONS ====================

TPtrC PbkContactNameFormattingUtil::FieldText
        (const MPbkFieldDataArray& aContactData, 
        TPbkFieldId aFieldId)
    {
    TPtrC result;
    const TInt count = aContactData.PbkFieldCount();
    for (TInt i=0; i < count; ++i)
        {
        const MPbkFieldData& field = aContactData.PbkFieldAt(i);
        if (field.PbkFieldId()==aFieldId && 
            field.PbkFieldType()==KStorageTypeText)
            {
            TPtrC text(field.PbkFieldText());
            if (!PbkEngUtils::IsEmptyOrAllSpaces(text))
                {
                result.Set(text);
                }
            break;
            }
        }
    return result;
    }

/**
 * Returns true if first character of aText is space.
 * @precond aText.Length() > 0
 */
TBool PbkContactNameFormattingUtil::FirstCharIsSpace(const TDesC& aText)
    {
    return (TChar(aText[0]).IsSpace());
    }

/**
 * Returns true if last character of aText is space.
 * @precond aText.Length() > 0
 */
TBool PbkContactNameFormattingUtil::LastCharIsSpace(const TDesC& aText)
    {
    return (TChar(aText[aText.Length()-1]).IsSpace());
    }

/**
 * Gets the phonebook field id corresponding to aIndex in the sort order array.
 */
TPbkFieldId PbkContactNameFormattingUtil::GetPbkFieldIdAt
        (const RContactViewSortOrder& aSortOrder, 
        TInt aIndex)
    {
    switch (aSortOrder[aIndex].iUid)
        {
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
        case KUidContactFieldGivenNamePronunciationValue:
            {
            return EPbkFieldIdFirstNameReading;
            }
        case KUidContactFieldFamilyNamePronunciationValue:
            {
            return EPbkFieldIdLastNameReading;
            }
        default:
            {
            return EPbkFieldIdNone;
            }
        }
    }

//  End of File 
