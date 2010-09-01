/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Field type list for mapping vCard field types to 
                 generic field type list.
*
*/


#include "CVPbkVCardFieldTypeList.h"
#include "CVPbkVCardFieldTypeProperty.h"

inline CVPbkVCardFieldTypeList::CVPbkVCardFieldTypeList()
    {
    }

inline void CVPbkVCardFieldTypeList::ConstructL(
        const TArray<CVPbkVCardFieldTypeProperty*> aArray,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    TInt propertyCount(aArray.Count());
    for ( TInt n = 0; n < propertyCount; ++n )
        {
        CVPbkVCardFieldTypeProperty* property = aArray[ n ];
        TInt indexedPropertyCount( property->Count() );
        for ( TInt i = 0; i < indexedPropertyCount; ++i )
            {
            const TVPbkVCardIndexedProperty& indexedProperty = 
                property->IndexedPropertyAt( i );
            const MVPbkFieldType* vcardType = 
                                    indexedProperty.FindMatch( aMasterFieldTypeList );
            if ( vcardType )
                {
                iFieldTypes.AppendL( vcardType );
                const TInt matchPriorities = vcardType->VersitProperties().Count();
                if (matchPriorities > iMaxMatchPriority)
                    {
                    iMaxMatchPriority = matchPriorities - 1;
                    }
                }
            }
        }
    }

CVPbkVCardFieldTypeList* CVPbkVCardFieldTypeList::NewLC(
        const TArray<CVPbkVCardFieldTypeProperty*> aArray,
        const MVPbkFieldTypeList& aMasterFieldTypeList)
    {
    CVPbkVCardFieldTypeList* self = new(ELeave)CVPbkVCardFieldTypeList();
    CleanupStack::PushL( self );
    self->ConstructL( aArray, aMasterFieldTypeList );
    return self;
    }

CVPbkVCardFieldTypeList::~CVPbkVCardFieldTypeList()
    {
    iFieldTypes.Close();
    }

TInt CVPbkVCardFieldTypeList::FieldTypeCount() const
    {
    return iFieldTypes.Count();
    }

const MVPbkFieldType& CVPbkVCardFieldTypeList::FieldTypeAt(TInt aIndex) const
    {
    return *iFieldTypes[ aIndex ];
    }

TBool CVPbkVCardFieldTypeList::ContainsSame(const MVPbkFieldType& aFieldType) const
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

TInt CVPbkVCardFieldTypeList::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

const MVPbkFieldType* CVPbkVCardFieldTypeList::FindMatch
    (const TVPbkFieldVersitProperty& aMatchProperty,
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

const MVPbkFieldType* CVPbkVCardFieldTypeList::FindMatch
    (TVPbkNonVersitFieldType aNonVersitType) const
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

const MVPbkFieldType* CVPbkVCardFieldTypeList::Find(
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

// End of file
