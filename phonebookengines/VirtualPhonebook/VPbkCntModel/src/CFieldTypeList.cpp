/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Field type list for mapping contact model view sort order 
                 to generic field type list.
*
*/


#include "CFieldTypeList.h"
#include "CFieldTypeMap.h"

#include <cntview.h>

namespace VPbkCntModel {

CFieldTypeList::CFieldTypeList()
    {
    }

inline void CFieldTypeList::ConstructL(
        const RContactViewSortOrder& aSortOrder,
        const CFieldTypeMap& aFieldTypeMap)
    {
    const TInt count = aSortOrder.Count();
    const TInt maxMatchPriority = aFieldTypeMap.MaxMatchPriority();

    for (TInt i = 0; i < count; ++i)
        {
        for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; ++matchPriority)
            {
            const MVPbkFieldType* genericType = 
                aFieldTypeMap.FindByCntModelFieldType(aSortOrder[i], matchPriority);
            if (genericType)
                {
                User::LeaveIfError(iFieldTypes.Append(genericType));
                break; // break the innermost for loop
                }
            }
        }
    }

CFieldTypeList* CFieldTypeList::NewLC(
        const RContactViewSortOrder& aSortOrder,
        const CFieldTypeMap& aFieldTypeMap)
    {
	CFieldTypeList* self = new(ELeave) CFieldTypeList;
    CleanupStack::PushL(self);
    self->ConstructL(aSortOrder, aFieldTypeMap);
    return self;
	}

CFieldTypeList::~CFieldTypeList()
    {
    iFieldTypes.Close();
    }

TInt CFieldTypeList::FieldTypeCount() const
    {
    return iFieldTypes.Count();
    }

const MVPbkFieldType& CFieldTypeList::FieldTypeAt(TInt aIndex) const
    {
    return *iFieldTypes[aIndex];
    }

TBool CFieldTypeList::ContainsSame(
        const MVPbkFieldType& aFieldType) const
    {
    TBool result = EFalse;

    const TInt count = iFieldTypes.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldTypes[i]->IsSame(aFieldType))
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }

TInt CFieldTypeList::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

const MVPbkFieldType* CFieldTypeList::FindMatch(
        const TVPbkFieldVersitProperty& aMatchProperty,
        TInt aMatchPriority) const
    {
    const MVPbkFieldType* result = NULL;

    const TInt count = iFieldTypes.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldTypes[i]->Matches(aMatchProperty, aMatchPriority))
            {
            result = iFieldTypes[i];
            break;
            }
        }
    return result;
    }

const MVPbkFieldType* CFieldTypeList::FindMatch(
        TVPbkNonVersitFieldType aNonVersitType) const
    {
    const MVPbkFieldType* result = NULL;

    const TInt count = iFieldTypes.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldTypes[i]->NonVersitType() == aNonVersitType)
            {
            result = iFieldTypes[i];
            break;
            }
        }
    return result;
    }

const MVPbkFieldType* CFieldTypeList::Find(
        TInt aFieldTypeResId) const
    {
    const MVPbkFieldType* fieldType = NULL;
    const TInt count = iFieldTypes.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldTypes[i]->FieldTypeResId() == aFieldTypeResId)
            {
            fieldType = iFieldTypes[i];
            break;
            }
        }
    return fieldType;
    }

} // namespace VPbkCntModel

// end of file
