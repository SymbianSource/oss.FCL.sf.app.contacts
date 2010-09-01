/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A sorted field collection for phonebook2 fields
*
*/



// INCLUDE FILES
#include "CPbk2PresentationContactFieldCollection.h"

// From Phonebook2
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyGroup.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldPropertyArray2.h>
#include <CPbk2PresentationContactField.h>

// From Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldUriData.h>

// ============================= LOCAL FUNCTIONS ============================

namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EGetStoreContactIndex_PreCond,
    EGetStoreContactIndex_PostCond,
    EPanic_FieldAtLC_OOB
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2PresentationContactFieldCollection");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// --------------------------------------------------------------------------
// CompareFields
// Returns:
// 1) zero, if the two objects are equal.
// 2) a negative value, if the first object is less than the second.
// 3) a positive value, if the first object is greater than the second.
// --------------------------------------------------------------------------
//
TInt CompareFields(const CPbk2PresentationContactField&  aFirst,
    const CPbk2PresentationContactField& aSecond)
    {
    return aFirst.FieldProperty().OrderingItem() - 
           aSecond.FieldProperty().OrderingItem();           
    }

// --------------------------------------------------------------------------
// SupportedFieldTypes
// Returns the supported field type list of the parent contact
// of the collection
// --------------------------------------------------------------------------
//
inline const MVPbkFieldTypeList& SupportedFieldTypes(
    MVPbkStoreContactFieldCollection& aFieldCollection)
    {
    return aFieldCollection.ParentStoreContact().
        ContactStore().StoreProperties().SupportedFields();
    }
    
// --------------------------------------------------------------------------
// MatchProperty
// --------------------------------------------------------------------------
//
inline const MPbk2FieldProperty* MatchProperty
        (const MPbk2FieldPropertyArray& aFieldProperties,
        const MVPbkStoreContactField& aField, TInt aMaxMatchPriority,
        const TDesC& aName)
    {
    for (TInt i = 0; i < aMaxMatchPriority; ++i)
        {
        const MVPbkFieldType* type = aField.MatchFieldType(i);
        if (type)
            {
            TVPbkFieldVersitProperty prop;
            prop.SetName(EVPbkVersitNameIMPP);
            if (type->Matches(prop, 0))
                {
                TPtrC name = MVPbkContactFieldUriData::Cast(aField.FieldData()).
                        Scheme();
                if (name == KNullDesC)
                    {
                    name.Set(aName);
                    }
                 
                 MPbk2FieldPropertyArray2* fieldPropertyExtension =
                    reinterpret_cast<MPbk2FieldPropertyArray2*>(
                        const_cast<MPbk2FieldPropertyArray&>( aFieldProperties ).
                            FieldPropertyArrayExtension(
                                KMPbk2FieldPropertyArrayExtension2Uid ) );    
                return fieldPropertyExtension->FindProperty(*type, name);
                }
            return aFieldProperties.FindProperty(*type);
            }
        }
    return NULL;
    }
    
}

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::
//  CPbk2PresentationContactFieldCollection
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
inline CPbk2PresentationContactFieldCollection::
    CPbk2PresentationContactFieldCollection(
        const MPbk2FieldPropertyArray& aFieldProperties,
        MVPbkStoreContactFieldCollection& aFieldCollection,
        MVPbkStoreContact& aParentContact)
        :   iFieldProperties(aFieldProperties),
            iFieldCollection(aFieldCollection),
            iParentContact(aParentContact)
    {
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
inline void CPbk2PresentationContactFieldCollection::ConstructL()
    {
    const TInt count = iFieldCollection.FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkStoreContactField& field = iFieldCollection.FieldAt(i);
        AddFieldMappingL(field, KNullDesC);
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2PresentationContactFieldCollection* 
    CPbk2PresentationContactFieldCollection::NewL(
        const MPbk2FieldPropertyArray& aFieldProperties,
        MVPbkStoreContactFieldCollection& aFieldCollection,
        MVPbkStoreContact& aParentContact)
    {
    CPbk2PresentationContactFieldCollection* self = 
        new( ELeave ) CPbk2PresentationContactFieldCollection(aFieldProperties, 
        aFieldCollection, aParentContact);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPbk2PresentationContactFieldCollection::
    ~CPbk2PresentationContactFieldCollection()
    {
    iFields.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::At
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2PresentationContactField& 
    CPbk2PresentationContactFieldCollection::At(TInt aIndex) const
    {
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ContainsField
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2PresentationContactFieldCollection::ContainsField(
    const MPbk2FieldProperty& aProperty) const
    {
    const TInt count = iFields.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFields[i]->FieldProperty().IsSame(aProperty))
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ContainsFieldFromGroup
// --------------------------------------------------------------------------
//
EXPORT_C TBool 
    CPbk2PresentationContactFieldCollection::ContainsFieldFromGroup( 
        const CPbk2FieldPropertyGroup& aPropertyGroup) const
    {
    const TInt count = aPropertyGroup.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (ContainsField(aPropertyGroup.At(i)) != KErrNotFound)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FindFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2PresentationContactFieldCollection::FindFieldIndex(
    const MVPbkStoreContactField& aField) const
    {
    const TInt count = iFields.Count();
    for(TInt i = 0; i < count; ++i)
        {
        if (iFields[i]->IsSame(aField))
            {
            return i;
            }
        }
    return KErrNotFound;
    }
            
// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::AddFieldMappingL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactFieldCollection::AddFieldMappingL(
    const MVPbkStoreContactField& aField, const TDesC& aName)
    {    
    TLinearOrder<CPbk2PresentationContactField> compareFunc(CompareFields);
    const TInt maxMatchPriority = 
        SupportedFieldTypes(iFieldCollection).MaxMatchPriority();
    const MPbk2FieldProperty* prop = 
        MatchProperty(iFieldProperties, aField, maxMatchPriority, aName);
    if (prop)
        {
        CPbk2PresentationContactField* pbk2Field = 
            CPbk2PresentationContactField::NewLC(*prop, aField, iParentContact);
        iFields.InsertInOrderAllowRepeatsL(pbk2Field, compareFunc);
        CleanupStack::Pop(); // pbk2Field
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::RemoveFieldMapping
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactFieldCollection::RemoveFieldMapping(TInt aIndex)
    {
    if (aIndex >= 0 && aIndex < iFields.Count())
        {
        delete iFields[aIndex];
        iFields.Remove(aIndex);
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ResetFieldMappings
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactFieldCollection::ResetFieldMappings()
    {
    iFields.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::StoreIndexOfField
// --------------------------------------------------------------------------
//  
EXPORT_C TInt CPbk2PresentationContactFieldCollection::StoreIndexOfField(
    TInt aFieldIndex) const
    {
    __ASSERT_DEBUG(aFieldIndex >= 0 && aFieldIndex < iFields.Count(),
        Panic(EGetStoreContactIndex_PreCond));
    const TInt count = iFieldCollection.FieldCount();
    MVPbkStoreContactField* field = &iFields[aFieldIndex]->StoreField();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldCollection.FieldAt(i).IsSame(*field))
            {
            return i;
            }
        }
    __ASSERT_DEBUG(EFalse, Panic(EGetStoreContactIndex_PostCond));
    return KErrNotFound;
    }
    
// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldIndexOfStoreField
// --------------------------------------------------------------------------
//  
EXPORT_C TInt CPbk2PresentationContactFieldCollection::FieldIndexOfStoreField(
    TInt aStoreFieldIndex) const
    { 
    __ASSERT_DEBUG(aStoreFieldIndex >= 0 && 
            aStoreFieldIndex < iFieldCollection.FieldCount(),
        Panic(EGetStoreContactIndex_PreCond));
   
    const TInt count = iFields.Count();
    const MVPbkStoreContactField& collectionField = iFieldCollection.FieldAt( aStoreFieldIndex );
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkStoreContactField& field = iFields[i]->StoreField();
        if ( field.IsSame( collectionField ) )
            {
            return i;
            }
        }
    __ASSERT_DEBUG(EFalse, Panic(EGetStoreContactIndex_PostCond));
    return KErrNotFound;
    }
    

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact&
        CPbk2PresentationContactFieldCollection::ParentContact() const
    {
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldCount
// --------------------------------------------------------------------------
//
TInt CPbk2PresentationContactFieldCollection::FieldCount() const
    {
    return iFields.Count();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldAt
// --------------------------------------------------------------------------
//
const MVPbkStoreContactField&
        CPbk2PresentationContactFieldCollection::FieldAt(TInt aIndex) const
    {
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldAt
// --------------------------------------------------------------------------
//
MVPbkStoreContactField& CPbk2PresentationContactFieldCollection::FieldAt
        (TInt aIndex)
    {
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldAtLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField*
        CPbk2PresentationContactFieldCollection::FieldAtLC(TInt aIndex) const
    {
    __ASSERT_DEBUG( iFields.Count() > aIndex && 
                    aIndex >= 0, Panic( EPanic_FieldAtLC_OOB ) );

    TInt storeIndex = StoreIndexOfField(aIndex);
    const MVPbkStoreContactField& field =
        iFieldCollection.FieldAt(storeIndex);
                
    MVPbkStoreContactField* pbk2Field = CPbk2PresentationContactField::NewLC
        (iFields[aIndex]->FieldProperty(), field, iParentContact);
    return pbk2Field;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ParentStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact& 
    CPbk2PresentationContactFieldCollection::ParentStoreContact() const
    {
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::ParentStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2PresentationContactFieldCollection::RetrieveField(
        const MVPbkContactLink& aContactLink) const
    {
    return iFieldCollection.RetrieveField(aContactLink);
    }
        
//  End of File  
