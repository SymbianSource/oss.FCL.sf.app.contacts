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
*      Japanese contact field filter for find functionality.
*
*/


#ifndef     __TPbkJapanFindFieldFilter_H__
#define     __TPbkJapanFindFieldFilter_H__

// INCLUDES
#include "TPbkTitleFieldFilter.h"
#include "PbkContactNameFormattingUtil.h"

// CLASS DECLARATION

/**
 * Japanese contact title name field filter.
 * @see TPbkTitleFieldFilter
 */
template<TInt MaxTitleFields>
class TPbkJapanFindFieldFilter :
        public TPbkTitleFieldFilter<MaxTitleFields>
    {
    public:
        TPbkJapanFindFieldFilter();

    public: // from TPbkTitleFieldFilter
        void Set(const MPbkFieldDataArray& aFieldData);

    };


template<TInt MaxTitleFields>
TPbkJapanFindFieldFilter<MaxTitleFields>::TPbkJapanFindFieldFilter() :
    TPbkTitleFieldFilter<MaxTitleFields>()
    {
    }

/**
 * Only add reading fields. This is for secondary find,
 * and used only if title field find was not successful.
 */
template<TInt MaxTitleFields>
void TPbkJapanFindFieldFilter<MaxTitleFields>::Set
        (const MPbkFieldDataArray& aFieldData)
    {
    this->Reset();

    // Last name reading
    TPbkFieldId fieldId = EPbkFieldIdLastNameReading;
    TPtrC fieldText = PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId);
    if (fieldText.Length() > 0)
        {
        this->Add(fieldId, fieldText);
        }

    // First name reading
    fieldId = EPbkFieldIdFirstNameReading;
    fieldText.Set(PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId));
    if (fieldText.Length() > 0)
        {
        this->Add(fieldId, fieldText);
        }
    }

#endif // __TPbkJapanFindFieldFilter_H__

// End of File
