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
* Description:  A store specific field property array
*
*/



// INCLUDE FILES
#include <CPbk2StoreSpecificFieldPropertyArray.h>

// From Phonebook2
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2FieldProperty.h>

// From Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// Unnamed namespace for local definitions
namespace
    {
    #ifdef _DEBUG
        enum TPanicCode
            {
            EPanic_At_OOB = 1
            };
            
        void Panic( TPanicCode aPanic )
            {
            _LIT( KPanicCat, "CPbk2StoreSpecificFieldPropertyArray" );
            User::Panic( KPanicCat(), aPanic );
            }
    #endif // _DEBUG    
    }
    
// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::CPbk2StoreSpecificFieldPropertyArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2StoreSpecificFieldPropertyArray::CPbk2StoreSpecificFieldPropertyArray(
        MPbk2FieldPropertyArray& aFieldProperties)
        : iFieldProperties(aFieldProperties)
    {
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2StoreSpecificFieldPropertyArray::ConstructL(
        CPbk2StorePropertyArray& aStoreProperties,
        const MVPbkFieldTypeList& aFieldTypes,
        const TVPbkContactStoreUriPtr& aStoreUri)
    {
    const CPbk2StoreProperty* prop = aStoreProperties.FindProperty(aStoreUri);
    if (prop)
        {
        iSpecificFieldProperties = 
            prop->CreateStoreSpecificFieldPropertiesL(aFieldTypes);
        }
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreSpecificFieldPropertyArray* 
        CPbk2StoreSpecificFieldPropertyArray::NewL(
            MPbk2FieldPropertyArray& aFieldProperties,
            CPbk2StorePropertyArray& aStoreProperties,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactStore& aStore)
    {
    CPbk2StoreSpecificFieldPropertyArray* self = 
        new( ELeave ) CPbk2StoreSpecificFieldPropertyArray(aFieldProperties);
    CleanupStack::PushL( self );
    self->ConstructL(
        aStoreProperties,
        aFieldTypes,
        aStore.StoreProperties().Uri());
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CPbk2StoreSpecificFieldPropertyArray::~CPbk2StoreSpecificFieldPropertyArray()
    {
    delete iSpecificFieldProperties;
    }


// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::FindProperty
// -----------------------------------------------------------------------------
//
const MPbk2FieldProperty* CPbk2StoreSpecificFieldPropertyArray::FindProperty(
        const MVPbkFieldType& aFieldType) const
    {
    const MPbk2FieldProperty* prop = NULL;
    if (iSpecificFieldProperties)
        {
        // First looking from store specific properties.
        prop = iSpecificFieldProperties->FindProperty(aFieldType);
        }
                
    if (!prop)
        {
        // Not found from store specific propertis. Looking from base 
        // properties.
        return iFieldProperties.FindProperty(aFieldType);
        }
    return prop;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::Count
// -----------------------------------------------------------------------------
//
TInt CPbk2StoreSpecificFieldPropertyArray::Count() const
    {
    return iFieldProperties.Count();
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::At
// -----------------------------------------------------------------------------
//
const MPbk2FieldProperty& CPbk2StoreSpecificFieldPropertyArray::At(
        TInt aIndex) const
    {
    __ASSERT_DEBUG( iFieldProperties.Count() > aIndex && 
                    aIndex >= 0, Panic( EPanic_At_OOB ) );
    if ( !iSpecificFieldProperties )
        {
        return iFieldProperties.At(aIndex);
        }
    else
        {
        const MPbk2FieldProperty& baseProp = iFieldProperties.At( aIndex );
        return *FindProperty( baseProp.FieldType() );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::FieldPropertyArrayExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2StoreSpecificFieldPropertyArray::FieldPropertyArrayExtension(
     TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2FieldPropertyArrayExtension2Uid )
        {
        return static_cast<MPbk2FieldPropertyArray2*>( this );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreSpecificFieldPropertyArray::FindProperty
// -----------------------------------------------------------------------------
//
const MPbk2FieldProperty* CPbk2StoreSpecificFieldPropertyArray::FindProperty(
                    const MVPbkFieldType& aFieldType,
                    const TDesC& aName ) const
    {
    const MPbk2FieldProperty* prop = NULL;
    if (iSpecificFieldProperties)
        {
        // First looking from store specific properties.
        MPbk2FieldPropertyArray2* ext = reinterpret_cast<MPbk2FieldPropertyArray2*>(
            iSpecificFieldProperties->FieldPropertyArrayExtension(
                KMPbk2FieldPropertyArrayExtension2Uid ) );
        
        prop = ext->FindProperty(aFieldType, aName);
        }
                
    if (!prop)
        {
        // Not found from store specific propertis. Looking from base 
        // properties.
        MPbk2FieldPropertyArray2* ext = reinterpret_cast<MPbk2FieldPropertyArray2*>(
            iFieldProperties.FieldPropertyArrayExtension(
                KMPbk2FieldPropertyArrayExtension2Uid ) );
        
        prop = ext->FindProperty(aFieldType, aName);
        }
    return prop;
    }
//  End of File  
