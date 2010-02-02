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
* Description:  Virtual Phonebook contact field iterator interface.
*
*/


// INCLUDES
#include "CVPbkContactFieldIterator.h"
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkFieldType.h>

// -----------------------------------------------------------------------------
// CFielfByTypeIterator definition
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
NONSHARABLE_CLASS( CFieldByTypeIterator ): public CBase
    {
public:
    CFieldByTypeIterator(
        const MVPbkFieldType& aFieldType,
        TypeOfCollection& aFields );

    TypeOfField* Next();
    TBool HasNext();
private:
    const MVPbkFieldType& iFieldType;
    TypeOfCollection& iFields;
    TInt iIteratorIndex;
    };

// -----------------------------------------------------------------------------
// CFieldByTypeIterator<TypeOfCollection, TypeOfField>::CFieldByTypeIterator
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
CFieldByTypeIterator<TypeOfCollection, TypeOfField>::CFieldByTypeIterator(
    const MVPbkFieldType& aFieldType, TypeOfCollection& aFields)
    :   iFieldType( aFieldType ),
        iFields( aFields )
    {
    }

// -----------------------------------------------------------------------------
// CFieldByTypeIterator<TypeOfCollection, TypeOfField>::Next
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
TypeOfField* CFieldByTypeIterator<TypeOfCollection, TypeOfField>::Next()
    {
    TypeOfField* nextField = NULL;
    // advance to next field and return it, otherwise NULL
    const TInt fieldCount = iFields.FieldCount();
    for (TInt i = iIteratorIndex; i < fieldCount; ++i)
        {
        TypeOfField& field = iFields.FieldAt(i);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && fieldType->IsSame( iFieldType ) )
            {
            // next field found
            nextField = &field;
            iIteratorIndex = i+1;
            break;
            }
        }

    return nextField;
    }

// -----------------------------------------------------------------------------
// CFieldByTypeIterator<TypeOfCollection, TypeOfField>::HasNext
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
TBool CFieldByTypeIterator<TypeOfCollection, TypeOfField>::HasNext()
    {
    TBool ret = EFalse;
    // detect whether there is just a field in the remaining fields
    const TInt fieldCount = iFields.FieldCount();
    for (TInt i = iIteratorIndex; i < fieldCount; ++i)
        {
        TypeOfField& field = iFields.FieldAt(i);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && fieldType->IsSame( iFieldType ) )
            {
            // field type matches
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CFieldByTypeListIterator definition
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
NONSHARABLE_CLASS( CFieldByTypeListIterator ): public CBase
    {
public:
    CFieldByTypeListIterator(
        const MVPbkFieldTypeList& aFieldTypeList,
        TypeOfCollection& aFields );

    TypeOfField* Next();
    TBool HasNext();
private:
    const MVPbkFieldTypeList& iFieldTypeList;
    TypeOfCollection& iFields;
    TInt iIteratorIndex;
    };

// -----------------------------------------------------------------------------
// CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::
// CFieldByTypeListIterator
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::
CFieldByTypeListIterator(
        const MVPbkFieldTypeList& aFieldTypeList,
        TypeOfCollection& aFields)
    :   iFieldTypeList( aFieldTypeList ),
        iFields( aFields )
    {
    }

// -----------------------------------------------------------------------------
// CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::Next
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
TypeOfField* CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::Next()
    {
    TypeOfField* nextField = NULL;
    // advance to next field and return it, otherwise NULL
    const TInt fieldCount = iFields.FieldCount();
    for (TInt i = iIteratorIndex; i < fieldCount; ++i)
        {
        TypeOfField& field = iFields.FieldAt(i);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && iFieldTypeList.ContainsSame( *fieldType ) )
            {
            // next field found
            nextField = &field;
            iIteratorIndex = i+1;
            break;
            }
        }

    return nextField;
    }

// -----------------------------------------------------------------------------
// CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::HasNext
// -----------------------------------------------------------------------------
//
template<typename TypeOfCollection, typename TypeOfField>
TBool CFieldByTypeListIterator<TypeOfCollection, TypeOfField>::HasNext()
    {
    TBool ret = EFalse;
    // detect whether there is just a field in the remaining fields
    const TInt fieldCount = iFields.FieldCount();
    for (TInt i = iIteratorIndex; i < fieldCount; ++i)
        {
        TypeOfField& field = iFields.FieldAt(i);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && iFieldTypeList.ContainsSame( *fieldType ) )
            {
            // field type matches
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// Iterator for MVPbkStoreContactFieldCollection by Type
// -----------------------------------------------------------------------------
//
NONSHARABLE_CLASS(CStoreFieldByTypeIterator)
    :   public CFieldByTypeIterator<
            MVPbkStoreContactFieldCollection,
            MVPbkStoreContactField>
    {
public:
    CStoreFieldByTypeIterator(
        const MVPbkFieldType& aFieldType,
        MVPbkStoreContactFieldCollection& aFields );
    };

CStoreFieldByTypeIterator::CStoreFieldByTypeIterator(
        const MVPbkFieldType& aFieldType,
        MVPbkStoreContactFieldCollection& aFields )
    :   CFieldByTypeIterator( aFieldType, aFields )
    {
    }

// -----------------------------------------------------------------------------
// Iterator for const MVPbkBaseContactFieldCollection by Type
// -----------------------------------------------------------------------------
//
NONSHARABLE_CLASS(CBaseFieldByTypeIterator)
    :   public CFieldByTypeIterator<
            const MVPbkBaseContactFieldCollection,
            const MVPbkBaseContactField>
    {
public:
    CBaseFieldByTypeIterator(
        const MVPbkFieldType& aFieldType,
        const MVPbkBaseContactFieldCollection& aFields );
    };

CBaseFieldByTypeIterator::CBaseFieldByTypeIterator(
        const MVPbkFieldType& aFieldType,
        const MVPbkBaseContactFieldCollection& aFields )
    :   CFieldByTypeIterator( aFieldType, aFields )
    {
    }

// -----------------------------------------------------------------------------
// Iterator for const MVPbkBaseContactFieldCollection by TypeList
// -----------------------------------------------------------------------------
//
NONSHARABLE_CLASS(CBaseFieldByTypeListIterator)
    :   public CFieldByTypeListIterator<
            const MVPbkBaseContactFieldCollection,
            const MVPbkBaseContactField>
    {
public:
    CBaseFieldByTypeListIterator(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aFields );
    };

CBaseFieldByTypeListIterator::CBaseFieldByTypeListIterator(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aFields )
    :   CFieldByTypeListIterator( aFieldTypeList, aFields )
    {
    }

// -----------------------------------------------------------------------------
// Implementation for CVPbkBaseContactFieldTypeIterator class
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeIterator::ConstructL
// -----------------------------------------------------------------------------
//
void CVPbkBaseContactFieldTypeIterator::ConstructL(
        const MVPbkFieldType& aFieldType,
        const MVPbkBaseContactFieldCollection& aStoreFields )
    {
    iIterator = new (ELeave ) CBaseFieldByTypeIterator( aFieldType, aStoreFields );
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeIterator::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkBaseContactFieldTypeIterator* CVPbkBaseContactFieldTypeIterator::NewLC(
        const MVPbkFieldType& aFieldType,
        const MVPbkBaseContactFieldCollection& aFields)
    {
    CVPbkBaseContactFieldTypeIterator* self =
        new(ELeave) CVPbkBaseContactFieldTypeIterator;
    CleanupStack::PushL(self);
    self->ConstructL( aFieldType, aFields  );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeIterator::~CVPbkBaseContactFieldTypeIterator
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeIterator::~CVPbkBaseContactFieldTypeIterator()
    {
    delete iIterator;
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeIterator::Next
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactField* CVPbkBaseContactFieldTypeIterator::Next() const
    {
    return iIterator->Next();
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeIterator::HasNext
// -----------------------------------------------------------------------------
//
TBool CVPbkBaseContactFieldTypeIterator::HasNext() const
    {
    return iIterator->HasNext();
    }

// -----------------------------------------------------------------------------
// Implementation for CVPbkBaseContactFieldTypeListIterator class
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeListIterator::ConstructL
// -----------------------------------------------------------------------------
//
void CVPbkBaseContactFieldTypeListIterator::ConstructL(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aStoreFields )
    {
    iIterator = new (ELeave ) CBaseFieldByTypeListIterator( aFieldTypeList, aStoreFields );
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeListIterator::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkBaseContactFieldTypeListIterator* CVPbkBaseContactFieldTypeListIterator::NewLC(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aFields)
    {
    CVPbkBaseContactFieldTypeListIterator* self =
        new(ELeave) CVPbkBaseContactFieldTypeListIterator;
    CleanupStack::PushL(self);
    self->ConstructL( aFieldTypeList, aFields  );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeListIterator::~CVPbkBaseContactFieldTypeListIterator
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeListIterator::~CVPbkBaseContactFieldTypeListIterator()
    {
    delete iIterator;
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeListIterator::Next
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactField* CVPbkBaseContactFieldTypeListIterator::Next() const
    {
    return iIterator->Next();
    }

// -----------------------------------------------------------------------------
// CVPbkBaseContactFieldTypeListIterator::HasNext
// -----------------------------------------------------------------------------
//
TBool CVPbkBaseContactFieldTypeListIterator::HasNext() const
    {
    return iIterator->HasNext();
    }

// -----------------------------------------------------------------------------
// Implementation for CVPbkContactFieldTypeIterator class
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::ConstructL
// -----------------------------------------------------------------------------
//
void CVPbkContactFieldTypeIterator::ConstructL(
        const MVPbkFieldType& aFieldType,
        MVPbkStoreContactFieldCollection& aStoreFields )
    {
    iIterator = new (ELeave ) CStoreFieldByTypeIterator( aFieldType, aStoreFields );
    }

// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkContactFieldTypeIterator* CVPbkContactFieldTypeIterator::NewLC(
        const MVPbkFieldType& aFieldType,
        MVPbkStoreContactFieldCollection& aFields)
    {
    CVPbkContactFieldTypeIterator* self =
        new(ELeave) CVPbkContactFieldTypeIterator;
    CleanupStack::PushL(self);
    self->ConstructL( aFieldType, aFields  );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::~CVPbkContactFieldTypeIterator
// -----------------------------------------------------------------------------
//
CVPbkContactFieldTypeIterator::~CVPbkContactFieldTypeIterator()
    {
    delete iIterator;
    }

// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::Next
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactField* CVPbkContactFieldTypeIterator::Next() const
    {
    return iIterator->Next();
    }

// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::Next
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* CVPbkContactFieldTypeIterator::Next()
    {
    return iIterator->Next();
    }

// -----------------------------------------------------------------------------
// CVPbkContactFieldTypeIterator::HasNext
// -----------------------------------------------------------------------------
//
TBool CVPbkContactFieldTypeIterator::HasNext() const
    {
    return iIterator->HasNext();
    }

// end of file

