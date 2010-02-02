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
* Description:  Field filter
*
*/

// INCLUDES
#include "CVPbkFieldFilter.h"
#include <MVPbkContactFieldSelector.h>
#include <MVPbkContactFieldOrdering.h>
#include <MVPbkStoreContactField.h>
// LOCAL
namespace {

/**
 * Swap two RArray objects.
 */
void Swap(RArray<TInt>& aLhs, RArray<TInt>& aRhs)
    {
    RArray<TInt> tmp = aLhs;
    aLhs = aRhs;
    aRhs = tmp;
    }

/**
 * TKey implementation for sorting the field mapping array.
 */
NONSHARABLE_CLASS(TMappingSortKey) : public TKey
    {
    public:
        TMappingSortKey(
        	const MVPbkBaseContactFieldCollection& aBaseFields,
            const RArray<TInt>& aFieldMapping,
            MVPbkContactFieldOrdering& aFieldOrdering);

    public:  // from TKey
    	TInt Compare(TInt aLeft, TInt aRight) const;

    private:  // Data
    	///Ref: base fields
        const MVPbkBaseContactFieldCollection& iBaseFields;
        ///Ref: field mapping  
        const RArray<TInt>& iFieldMapping;
        ///Ref: field ordering
        MVPbkContactFieldOrdering& iFieldOrdering;
    };

/**
 * TSwap implementation for sorting the field mapping array.
 */
NONSHARABLE_CLASS(TMappingSwap) : public TSwap
    {
    public:
        TMappingSwap(RArray<TInt>& aFieldMapping);

    public:  // from TSwap
    	void Swap(TInt aLeft, TInt aRight) const;

    private:  // Data
    	///Ref: field mapping
        RArray<TInt>& iFieldMapping;
    };

// TMappingSortKey implementation
inline TMappingSortKey::TMappingSortKey(
		const MVPbkBaseContactFieldCollection& aBaseFields,
        const RArray<TInt>& aFieldMapping,
        MVPbkContactFieldOrdering& aFieldOrdering) :
    iBaseFields ( aBaseFields ),
    iFieldMapping ( aFieldMapping ),
    iFieldOrdering ( aFieldOrdering )
    {
    }

TInt TMappingSortKey::Compare(
		TInt aLeft,
		TInt aRight) const
    {
    const MVPbkBaseContactField& lhs = iBaseFields.FieldAt(iFieldMapping[aLeft]);
    const MVPbkBaseContactField& rhs = iBaseFields.FieldAt(iFieldMapping[aRight]);
    return iFieldOrdering.CompareFields(lhs, rhs);
    }

// TMappingSwap implementation
inline TMappingSwap::TMappingSwap(
		RArray<TInt>& aFieldMapping) :
    iFieldMapping ( aFieldMapping )
    {
    }

void TMappingSwap::Swap(
		TInt aLeft,
		TInt aRight) const
    {
    const TInt tmp = iFieldMapping[aLeft];
    iFieldMapping[aLeft] = iFieldMapping[aRight];
    iFieldMapping[aRight] = tmp;
    }

}  // unnamed namespace


// CVPbkFieldFilter implementation
inline CVPbkFieldFilter::CVPbkFieldFilter()
    {
    }

EXPORT_C CVPbkFieldFilter* CVPbkFieldFilter::NewL(
		const TConfig& aConfig)
    {
    CVPbkFieldFilter* self = new(ELeave) CVPbkFieldFilter;
    CleanupStack::PushL(self);
    self->ResetL(aConfig);
    CleanupStack::Pop(self);
    return self;
    }

CVPbkFieldFilter::~CVPbkFieldFilter()
    {
    iFieldMapping.Close();
    }

EXPORT_C void CVPbkFieldFilter::ResetL(
		const TConfig& aConfig)
    {
    RArray<TInt> fieldMapping;
    CleanupClosePushL(fieldMapping);

	// loop through all the store contact fields
    const TInt fieldCount = aConfig.iBaseFields.FieldCount();
    for (TInt fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
        {
        MVPbkStoreContactField& fieldCandidate = 
            	const_cast<MVPbkStoreContactFieldCollection*>(
            			&aConfig.iBaseFields)->FieldAt(fieldIndex);
        if (!aConfig.iFieldSelector || 
            aConfig.iFieldSelector->IsFieldIncluded(fieldCandidate))
            {
            // if the selector chooses the fields then it is added
            // to the field mapping, this cannot leave due to ReserveL
            // allocation earlier in this function
            User::LeaveIfError(fieldMapping.Append(fieldIndex));
            }
        }

    // sort fields
    if (aConfig.iFieldOrdering)
        {
        TMappingSortKey sortKey(
                aConfig.iBaseFields, 
        		fieldMapping, 
                *aConfig.iFieldOrdering);
        TMappingSwap swap(fieldMapping);
        User::QuickSort(fieldMapping.Count(), sortKey, swap);
        }

	// swap members 
    iBaseFields = &aConfig.iBaseFields;
    Swap(iFieldMapping, fieldMapping);
    CleanupStack::PopAndDestroy(); // fieldMapping
    }

EXPORT_C TInt CVPbkFieldFilter::FindField(
		const MVPbkBaseContactField& aContactField) const
    {
    TInt result = KErrNotFound;

    const TInt count = FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkBaseContactField& field = FieldAt(i);
        if (field.IsSame(aContactField))
            {
            result = i;
            break;
            }
        }

    return result;
    }

MVPbkBaseContact& CVPbkFieldFilter::ParentContact() const
    {
    return iBaseFields->ParentContact();
    }

TInt CVPbkFieldFilter::FieldCount() const
    {
    return iFieldMapping.Count();
    }

const MVPbkStoreContactField& CVPbkFieldFilter::FieldAt(
		TInt aIndex) const
    {
    return iBaseFields->FieldAt(iFieldMapping[aIndex]);
    }

MVPbkStoreContactField& CVPbkFieldFilter::FieldAt(
		TInt aIndex) 
    {
    return iBaseFields->FieldAt(iFieldMapping[aIndex]);
    }

MVPbkStoreContactField* CVPbkFieldFilter::FieldAtLC(
		TInt aIndex) const
    {
    return iBaseFields->FieldAtLC(iFieldMapping[aIndex]);
    }

MVPbkStoreContact& CVPbkFieldFilter::ParentStoreContact() const
    {
    return iBaseFields->ParentStoreContact();
    }
    
MVPbkStoreContactField* CVPbkFieldFilter::RetrieveField(
        const MVPbkContactLink& aContactLink) const
    {
    return iBaseFields->RetrieveField(aContactLink);
    }

// End of File
