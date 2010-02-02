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
* Description:  List of field types
*
*/


// INCLUDES
#include "CVPbkFieldTypeList.h"

// Virtual Phonebook
#include "VPbkUtil.h"

// System includes
#include <barsc.h>
#include <barsread.h>
#include <badesca.h>

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::CVPbkFieldTypeList
// --------------------------------------------------------------------------
//
inline CVPbkFieldTypeList::CVPbkFieldTypeList()
    {
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::~CVPbkFieldTypeList
// --------------------------------------------------------------------------
//
CVPbkFieldTypeList::~CVPbkFieldTypeList()
    {
    iFieldTypes.ResetAndDestroy();
    delete iTextStore;
    }


// --------------------------------------------------------------------------
// CVPbkFieldTypeList::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkFieldTypeList::ConstructL(
        RResourceFile& aResFile,
        TResourceReader& aResReader )
    {
    iTextStore = new(ELeave) VPbkEngUtils::CTextStore;
    TInt count = aResReader.ReadInt16();
    while (count-- > 0)
        {
        CVPbkFieldType* type = CVPbkFieldType::NewLC(aResFile, 
                aResReader, *iTextStore);
        User::LeaveIfError(iFieldTypes.Append(type));
        CleanupStack::Pop(type);

        const TInt matchPriorities = type->VersitProperties().Count();
        if (matchPriorities > iMaxMatchPriority)
            {
            iMaxMatchPriority = matchPriorities - 1;
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeList* CVPbkFieldTypeList::NewL(
        RResourceFile& aResFile,
        TResourceReader& aResReader )
    {
    CVPbkFieldTypeList* self = new ( ELeave ) CVPbkFieldTypeList;
    CleanupStack::PushL( self );
    self->ConstructL( aResFile, aResReader );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::FieldTypeCount
// --------------------------------------------------------------------------
//
TInt CVPbkFieldTypeList::FieldTypeCount() const
    {
    return iFieldTypes.Count();
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::FieldTypeAt
// --------------------------------------------------------------------------
//
const MVPbkFieldType& CVPbkFieldTypeList::FieldTypeAt(TInt aIndex) const
    {
    return *iFieldTypes[aIndex];
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::ContainsSame
// --------------------------------------------------------------------------
//
TBool CVPbkFieldTypeList::ContainsSame(const MVPbkFieldType& aFieldType) const
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
// CVPbkFieldTypeList::MaxMatchPriority
// --------------------------------------------------------------------------
//
TInt CVPbkFieldTypeList::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeList::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeList::FindMatch
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
// CVPbkFieldTypeList::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeList::FindMatch
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
// CVPbkFieldTypeList::Find
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CVPbkFieldTypeList::Find(
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

// End of File
