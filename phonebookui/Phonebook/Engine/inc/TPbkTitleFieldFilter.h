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
*      Generic contact title field filter.
*
*/


#ifndef     __TPbkTitleFieldFilter_H__
#define     __TPbkTitleFieldFilter_H__

// INCLUDES
#include "MPbkFieldData.h"
#include "PbkContactNameFormattingUtil.h"
#include <cntview.h>


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Title data entry for TPbkTitleFieldFilter.
 * @see TPbkTitleFieldFilter
 */
NONSHARABLE_CLASS(TPbkTitleFieldData) : 
        public MPbkFieldData
    {
    public:  // Constructors
        TPbkTitleFieldData();

    public:  // New functions
        void Set(TPbkFieldId aFieldId, const TDesC& aText);

    public: // from MPbkFieldData
        TStorageType PbkFieldType() const;
        TPbkFieldId PbkFieldId() const;
        TPtrC PbkFieldText() const;
        TTime PbkFieldTime() const;

    private:  // Data
        TPbkFieldId iFieldId;
        TPtrC iFieldText;
    };


/**
 * Contact title name field filter.
 */
template<TInt MaxTitleFields> 
class TPbkTitleFieldFilter :
        public MPbkFieldDataArray
    {
    public:  // Constructor
        TPbkTitleFieldFilter();

    public: // New functions
        virtual void Set(const MPbkFieldDataArray& aFieldData,
                const RContactViewSortOrder& aSortOrder);

    protected: // New functions
        TInt Count() const;
        void Add(TPbkFieldId aFieldId, const TDesC& aText);
        void Reset();

    public:  // from MPbkFieldDataArray
        TInt PbkFieldCount() const;
        MPbkFieldData& PbkFieldAt(TInt aIndex);
        const MPbkFieldData& PbkFieldAt(TInt aIndex) const;

    private:  // Data
        TInt iCount;
        TFixedArray<TPbkTitleFieldData, MaxTitleFields> iFields;
    };

template<TInt MaxTitleFields>
TPbkTitleFieldFilter<MaxTitleFields>::TPbkTitleFieldFilter() :
    iCount(0)
    {
    }

template<TInt MaxTitleFields> inline
TInt TPbkTitleFieldFilter<MaxTitleFields>::Count() const
    {
    return iCount;
    }

template<TInt MaxTitleFields> inline
void TPbkTitleFieldFilter<MaxTitleFields>::Add(TPbkFieldId aFieldId, const TDesC& aText)
    {
    iFields.At(iCount++).Set(aFieldId, aText);
    }

template<TInt MaxTitleFields> inline
void TPbkTitleFieldFilter<MaxTitleFields>::Reset()
    {
    iCount = 0;
    }


/**
 * The standard western name formatting filter implementation.
 */
template<TInt MaxTitleFields>
void TPbkTitleFieldFilter<MaxTitleFields>::Set
        (const MPbkFieldDataArray& aFieldData,
        const RContactViewSortOrder& aSortOrder)
    {
    Reset();

    /* Add the fields in the sort order to the array */
    for (TInt i = 0; i < aSortOrder.Count(); ++i)
        {
        const TPbkFieldId fieldId = PbkContactNameFormattingUtil::GetPbkFieldIdAt(aSortOrder, i);
        if (fieldId == EPbkFieldIdLastName || 
            fieldId == EPbkFieldIdFirstName) 
            {
            TPtrC fieldText = PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId);
            if (fieldText.Length() > 0)
                {
                Add(fieldId, fieldText);
                }
            }
        }

    // Company name if no last name and first name
    if (Count() == 0)
        {
        const TPbkFieldId fieldId = EPbkFieldIdCompanyName;
        TPtrC fieldText = PbkContactNameFormattingUtil::FieldText(aFieldData, fieldId);
        if (fieldText.Length() > 0)
            {
            Add(fieldId, fieldText);
            }
        }
    }

template<TInt MaxTitleFields>
TInt TPbkTitleFieldFilter<MaxTitleFields>::PbkFieldCount() const
    {
    return iCount;
    }

template<TInt MaxTitleFields>
MPbkFieldData& TPbkTitleFieldFilter<MaxTitleFields>::PbkFieldAt(TInt aIndex)
    {
    // TFixedArray will check that aIndex is valid
    return iFields.At(aIndex);
    }

template<TInt MaxTitleFields>
const MPbkFieldData& TPbkTitleFieldFilter<MaxTitleFields>::PbkFieldAt(TInt aIndex) const
    {
    // TFixedArray will check that aIndex is valid
    return iFields.At(aIndex);
    }

#endif // __TPbkTitleFieldFilter_H__

// End of File
