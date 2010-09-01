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
*      Japanese contact title field filter.
*
*/


#ifndef     __TPbkJapanTitleFieldFilter_H__
#define     __TPbkJapanTitleFieldFilter_H__

// INCLUDES
#include "TPbkTitleFieldFilter.h"
#include "PbkContactNameFormattingUtil.h"


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Japanese contact title name field filter.
 * @see TPbkTitleFieldFilter
 */
template<TInt MaxTitleFields>
class TPbkJapanTitleFieldFilter :
        public TPbkTitleFieldFilter<MaxTitleFields>
    {
    public:
        TPbkJapanTitleFieldFilter();

    public: // from TPbkTitleFieldFilter
        void Set(const MPbkFieldDataArray& aFieldData);

    };


template<TInt MaxTitleFields>
TPbkJapanTitleFieldFilter<MaxTitleFields>::TPbkJapanTitleFieldFilter() :
    TPbkTitleFieldFilter<MaxTitleFields>()
    {
    }


template<TInt MaxTitleFields>
void TPbkJapanTitleFieldFilter<MaxTitleFields>::Set
        (const MPbkFieldDataArray& aFieldData)
    {
    this->Reset();

    // Last name
    TPbkFieldId fieldId = EPbkFieldIdLastName;
    TPtrC fieldText = PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId);
    if (fieldText.Length() > 0)
        {
        this->Add(fieldId, fieldText);
        }

    // First name
    fieldId = EPbkFieldIdFirstName;
    fieldText.Set(PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId));
    if (fieldText.Length() > 0)
        {
        this->Add(fieldId, fieldText);
        }

    // Last & first name reading
    if (this->Count() == 0)
        {
        // last name reading
        fieldId = EPbkFieldIdLastNameReading;
        fieldText.Set(PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId));
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

    // Company name if no last name and first name
    if (this->Count() == 0)
        {
        fieldId = EPbkFieldIdCompanyName;
        fieldText.Set(PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId));
        if (fieldText.Length() > 0)
            {
            this->Add(fieldId, fieldText);
            }
        }
    }


#endif // __TPbkJapanTitleFieldFilter_H__

// End of File
