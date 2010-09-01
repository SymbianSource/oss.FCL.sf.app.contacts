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
* Description:  An list for fieldtype references. Implements 
*                MVPbkFieldTypeList so can be used e.g as parameter to Find
*                operation.
*
*/


// INCLUDES
#include "CVPbkFieldTypeRefsList.h"
#include <barsread.h>
#include "TVPbkFieldVersitProperty.h"


// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::CVPbkFieldTypeRefsList
// --------------------------------------------------------------------------
//
inline CVPbkFieldTypeRefsList::CVPbkFieldTypeRefsList()
    {
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::~CVPbkFieldTypeRefsList
// --------------------------------------------------------------------------
//
CVPbkFieldTypeRefsList::~CVPbkFieldTypeRefsList()
    {
    iFieldTypes.Reset();
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeRefsList* CVPbkFieldTypeRefsList::NewL()
    {
    CVPbkFieldTypeRefsList* self = new(ELeave) CVPbkFieldTypeRefsList;
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::AppendL
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkFieldTypeRefsList::AppendL(const MVPbkFieldType& aFieldType)
    {
    const TInt matchPriorities = aFieldType.VersitProperties().Count();
    if (matchPriorities > iMaxMatchPriority)
        {
        iMaxMatchPriority = matchPriorities - 1;
        }

    User::LeaveIfError(iFieldTypes.Append(&aFieldType));
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::Remove
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkFieldTypeRefsList::Remove(const MVPbkFieldType& aFieldType)
    {
    TInt index = iFieldTypes.Find(&aFieldType);
    if (index != KErrNotFound)
        {
        iFieldTypes.Remove(index);
        iMaxMatchPriority = 0;
        TInt fieldTypeCount = iFieldTypes.Count();
        for (TInt i = 0; i < fieldTypeCount; ++i)
            {
            const TInt matchPriorities = FieldTypeAt(i).VersitProperties().Count();
            if (matchPriorities > iMaxMatchPriority)
                {
                iMaxMatchPriority = matchPriorities - 1;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::Reset
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkFieldTypeRefsList::Reset()
    {
    // Reset but don't destroy
    iFieldTypes.Reset();
    }
    
// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::FieldTypeCount
// --------------------------------------------------------------------------
//
TInt CVPbkFieldTypeRefsList::FieldTypeCount() const
    {
    return iFieldTypes.Count();
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::FieldTypeAt
// --------------------------------------------------------------------------
//
const MVPbkFieldType& CVPbkFieldTypeRefsList::FieldTypeAt(TInt aIndex) const
    {
    return *iFieldTypes[aIndex];
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::ContainsSame
// --------------------------------------------------------------------------
//
TBool CVPbkFieldTypeRefsList::ContainsSame(const MVPbkFieldType& aFieldType) const
    {
    const TInt count = iFieldTypes.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iFieldTypes[i]->IsSame(aFieldType))
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::MaxMatchPriority
// --------------------------------------------------------------------------
//
TInt CVPbkFieldTypeRefsList::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeRefsList::FindMatch
        (const TVPbkFieldVersitProperty& aMatchProperty,
        TInt aMatchPriority) const
    {
    const TInt count = iFieldTypes.Count();
    for (TInt i=0; i < count; ++i)
        {
        const MVPbkFieldType& ft = *iFieldTypes[i];
        if (ft.Matches(aMatchProperty, aMatchPriority))
            {
            return &ft;
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeRefsList::FindMatch
        (TVPbkNonVersitFieldType aNonVersitType) const
    {
    const TInt count = iFieldTypes.Count();
    for (TInt i=0; i < count; ++i)
        {
        const MVPbkFieldType& ft = *iFieldTypes[i];
        if (ft.NonVersitType() == aNonVersitType)
            {
            return &ft;
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeRefsList::Find
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeRefsList::Find(
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

// end of file

