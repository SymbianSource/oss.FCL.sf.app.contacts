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
* Description:  A sort order that is needed in view creation.
*
*/


#include "CVPbkSortOrder.h"
#include <barsread.h>
#include "TVPbkFieldTypeMapping.h"

#include <VPbkError.h>


inline CVPbkSortOrder::CVPbkSortOrder() :
    iMaxSortOrderLength(-1)
    {
    }

inline void CVPbkSortOrder::ConstructL
        (TResourceReader& aReader, const MVPbkFieldTypeList& aMasterFieldTypeList)
    {
    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        TVPbkFieldTypeMapping mapping(aReader);
        const MVPbkFieldType* fieldType = mapping.FindMatch(aMasterFieldTypeList);
        if (!fieldType)
            {
            User::Leave(KErrNotFound);
            }
        
        const TInt matchPriorities = fieldType->VersitProperties().Count();
        if (matchPriorities > iMaxMatchPriority)
            {
            iMaxMatchPriority = matchPriorities - 1;
            }

        User::LeaveIfError(iSortOrder.Append(fieldType));
        }
    }

inline void CVPbkSortOrder::ConstructL(const MVPbkFieldTypeList& aSortOrder)
    {
    const TInt count = aSortOrder.FieldTypeCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType& fieldType = aSortOrder.FieldTypeAt(i);
        User::LeaveIfError(iSortOrder.Append(&fieldType));
        }
    iMaxMatchPriority = aSortOrder.MaxMatchPriority();
    }

EXPORT_C CVPbkSortOrder* CVPbkSortOrder::NewL
        (TResourceReader& aReader, const MVPbkFieldTypeList& aMasterFieldTypeList)
    {
    CVPbkSortOrder* self = new(ELeave) CVPbkSortOrder;
    CleanupStack::PushL(self);
    self->ConstructL(aReader, aMasterFieldTypeList);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CVPbkSortOrder* CVPbkSortOrder::NewL(const MVPbkFieldTypeList& aSortOrder)
    {
    CVPbkSortOrder* self = new(ELeave) CVPbkSortOrder;
    CleanupStack::PushL(self);
    self->ConstructL(aSortOrder);
    CleanupStack::Pop(self);
    return self;    
    }

CVPbkSortOrder::~CVPbkSortOrder()
    {
    iSortOrder.Close();
    }

EXPORT_C void CVPbkSortOrder::SetReserveL(TInt aSortOrderLength)
    {
    __ASSERT_ALWAYS(aSortOrderLength > 0, VPbkError::Panic(VPbkError::EInvalidLength));
    iMaxSortOrderLength = aSortOrderLength;

    RPointerArray<MVPbkFieldType> sortOrder(iMaxSortOrderLength);
    CleanupClosePushL( sortOrder );
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count && i < iMaxSortOrderLength; ++i)
        {
        User::LeaveIfError(sortOrder.Append(iSortOrder[i]));
        }
    iSortOrder.Close();
    CleanupStack::Pop( &sortOrder );
    iSortOrder = sortOrder;
    iSortOrder.GranularCompress();
    }

EXPORT_C void CVPbkSortOrder::Set(const MVPbkFieldTypeList& aSortOrder)
    {
    __ASSERT_ALWAYS(iMaxSortOrderLength > 0, VPbkError::Panic(VPbkError::EInvalidLength));
    TInt i;

    // First remove all entries in iSortOrder. Reset so we preserve array's memory
    for (i = iSortOrder.Count() - 1; i >= 0; --i)
        {
        iSortOrder.Remove(i);
        }

    const TInt count = aSortOrder.FieldTypeCount();
    for (i = 0; i < count; ++i)
        {
        iSortOrder.Append(&aSortOrder.FieldTypeAt(i));
        }
    }

TInt CVPbkSortOrder::FieldTypeCount() const
    {
    return iSortOrder.Count();
    }

const MVPbkFieldType& CVPbkSortOrder::FieldTypeAt(TInt aIndex) const
    {
    return *iSortOrder[aIndex];
    }

TBool CVPbkSortOrder::ContainsSame(const MVPbkFieldType& aFieldType) const
    {
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType* matchingType = iSortOrder[i];
        if (matchingType && matchingType->IsSame(aFieldType))
            {
            return ETrue;
            }
        }
    return EFalse;
    }

TInt CVPbkSortOrder::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

const MVPbkFieldType* CVPbkSortOrder::FindMatch
        (const TVPbkFieldVersitProperty& aMatchProperty, TInt aMatchPriority) const
    {
    const MVPbkFieldType* result = NULL;
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType* fieldType = iSortOrder[i];
        if (fieldType && fieldType->Matches(aMatchProperty, aMatchPriority))
            {
            result = fieldType;
            break;
            }
        }
    return result;
    }

const MVPbkFieldType* CVPbkSortOrder::FindMatch(TVPbkNonVersitFieldType aNonVersitType) const
    {
    const MVPbkFieldType* result = NULL;
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType* fieldType = iSortOrder[i];
        if (fieldType && fieldType->NonVersitType() == aNonVersitType)
            {
            result = fieldType;
            break;
            }
        }
    return result;
    }

const MVPbkFieldType* CVPbkSortOrder::Find(
        TInt aFieldTypeResId) const
    {
    const MVPbkFieldType* fieldType = NULL;
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iSortOrder[i]->FieldTypeResId() == aFieldTypeResId)
            {
            fieldType = iSortOrder[i];
            break;
            }
        }
    return fieldType;
    }

// end of file
