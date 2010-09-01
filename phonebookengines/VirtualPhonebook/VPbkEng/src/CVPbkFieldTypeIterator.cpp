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
* Description:  Field type iterator
*
*/


// INCLUDES
#include "CVPbkFieldTypeIterator.h"
#include "MVPbkFieldType.h"
#include "CVPbkFieldTypeSelector.h"


inline CVPbkFieldTypeIterator::CVPbkFieldTypeIterator(
        const CVPbkFieldTypeSelector& aIterationCriteria, 
        const MVPbkFieldTypeList& aFieldTypeList) :
    iIterationCriteria(aIterationCriteria),
    iFieldTypeList(aFieldTypeList)
    {
    }

EXPORT_C CVPbkFieldTypeIterator* CVPbkFieldTypeIterator::NewLC(
        const CVPbkFieldTypeSelector& aIterationCriteria, 
        const MVPbkFieldTypeList& aFieldTypeList)
    {
    CVPbkFieldTypeIterator* self = new(ELeave) CVPbkFieldTypeIterator(
            aIterationCriteria, aFieldTypeList);
    CleanupStack::PushL(self);
    return self;
    }

CVPbkFieldTypeIterator::~CVPbkFieldTypeIterator()
    {
    }

const MVPbkFieldType* CVPbkFieldTypeIterator::Next()
    {
    const MVPbkFieldType* nextType = NULL;
    // advance to next field and return it, otherwise NULL
    const TInt typeCount = iFieldTypeList.FieldTypeCount();
    for (TInt i = iIteratorIndex; i < typeCount; ++i)
        {
        const MVPbkFieldType& type = iFieldTypeList.FieldTypeAt(i);
        if (iIterationCriteria.IsFieldTypeIncluded(type))
            {
            // next field found
            nextType = &type;
            iIteratorIndex = i+1;
            break;
            }
        }
    return nextType;
    }

TBool CVPbkFieldTypeIterator::HasNext() const
    {
    TBool ret = EFalse;
    // detect whether there is just a field in the remaining fields
    const TInt typeCount = iFieldTypeList.FieldTypeCount();
    for (TInt i = iIteratorIndex; i < typeCount; ++i)
        {
        const MVPbkFieldType& type = iFieldTypeList.FieldTypeAt(i);
        if (iIterationCriteria.IsFieldTypeIncluded(type))
            {
            // field type matches
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// end of file

