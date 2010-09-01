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
* Description: 
*     Field property array for Phonebook 2 field types.
*
*/


#include "CPbk2FieldPropertyArray.h"

// Phonebook 2
#include "Pbk2PresentationUtils.h"
#include "CPbk2FieldProperty.h"
#include "CPbk2FieldOrderingManager.h"
#include <CPbk2ServiceManager.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2Presentation.rsg>
#include <CPbk2FieldPropertyGroup.h>
#include <Pbk2DataCaging.hrh>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkEng.rsg>

// System includes
#include <barsc.h>
#include <barsread.h>

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::CPbk2FieldPropertyArray
// ---------------------------------------------------------------------------
//
inline CPbk2FieldPropertyArray::CPbk2FieldPropertyArray()
    {
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CPbk2FieldPropertyArray::ConstructL(
        const MVPbkFieldTypeList& aSupportedFieldTypeList, RFs& aRFs,
        CPbk2ServiceManager* aServMan)
    {
    RPbk2LocalizedResourceFile resFile( &aRFs );
    resFile.OpenLC(KPbk2RomFileDrive,
                   KDC_RESOURCE_FILES_DIR, 
                   Pbk2PresentationUtils::PresentationResourceFile());

    // Read field type presentation order information
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( 
            R_PBK2_FIELDTYPE_PRESENTATION_ORDERS ) );
    CPbk2FieldOrderingManager* orderings = 
            CPbk2FieldOrderingManager::NewL( reader );
    CleanupStack::PopAndDestroy(); // R_PBK2_FIELDTYPE_PRESENTATION_ORDERS
    CleanupStack::PushL( orderings );
    
    // Read field property resource strucutre array
    reader.SetBuffer( resFile.AllocReadLC( R_PHONEBOOK_FIELD_PROPERTIES ) );
    ReadFieldPropertiesL( reader, aSupportedFieldTypeList, *orderings );
    
    MultiplyImppFieldPropertyL( aServMan, aRFs );
    
    // orderings R_PHONEBOOK_FIELD_PROPERTIES, resFile
    CleanupStack::PopAndDestroy(3); 
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldPropertyArray* CPbk2FieldPropertyArray::NewL
        (const MVPbkFieldTypeList& aSupportedFieldTypeList, RFs& aRFs )
    {
    CPbk2FieldPropertyArray* self = new(ELeave) CPbk2FieldPropertyArray;
    CleanupStack::PushL(self);
    self->ConstructL( aSupportedFieldTypeList, aRFs, NULL );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldPropertyArray* CPbk2FieldPropertyArray::NewL
        (const MVPbkFieldTypeList& aSupportedFieldTypeList, RFs& aRFs,
         CPbk2ServiceManager* aServMan)
    {
    CPbk2FieldPropertyArray* self = new(ELeave) CPbk2FieldPropertyArray;
    CleanupStack::PushL(self);
    self->ConstructL( aSupportedFieldTypeList, aRFs, aServMan );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldPropertyArray* CPbk2FieldPropertyArray::NewL(
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        TResourceReader& aReader,
        RFs& aRFs )
    {
    CPbk2FieldPropertyArray* self = new(ELeave) CPbk2FieldPropertyArray;
    CleanupStack::PushL(self);
    CPbk2FieldOrderingManager* orderings = 
        self->CreateFieldOrderingsLC( aRFs );
    self->ReadFieldPropertiesL( aReader, aSupportedFieldTypeList, *orderings );
    CleanupStack::PopAndDestroy( orderings );

    self->MultiplyImppFieldPropertyL( NULL, aRFs );

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::~CPbk2FieldPropertyArray
// ---------------------------------------------------------------------------
//
CPbk2FieldPropertyArray::~CPbk2FieldPropertyArray()
    {
    iFieldProperties.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::FindProperty
// ---------------------------------------------------------------------------
//
const MPbk2FieldProperty* CPbk2FieldPropertyArray::FindProperty
        (const MVPbkFieldType& aFieldType) const
    {
    const CPbk2FieldProperty* result = NULL;
    const TInt count = iFieldProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const CPbk2FieldProperty* property = iFieldProperties[i];
        if (property->FieldType().IsSame(aFieldType))
            {
            result = property;
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::Count
// ---------------------------------------------------------------------------
//
TInt CPbk2FieldPropertyArray::Count() const
    {
    return iFieldProperties.Count();
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::At
// ---------------------------------------------------------------------------
//
const MPbk2FieldProperty& CPbk2FieldPropertyArray::At
        (TInt aIndex) const
    {
    return *iFieldProperties[aIndex];
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::FieldPropertyArrayExtension
// ---------------------------------------------------------------------------
//
TAny* CPbk2FieldPropertyArray::FieldPropertyArrayExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2FieldPropertyArrayExtension2Uid )
        {
        return static_cast<MPbk2FieldPropertyArray2*>( this );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::FindProperty
// ---------------------------------------------------------------------------
//
const MPbk2FieldProperty* CPbk2FieldPropertyArray::FindProperty
        (const MVPbkFieldType& aFieldType, const TDesC& aName) const
    {
    const CPbk2FieldProperty* result = NULL;
    const TInt count = iFieldProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const CPbk2FieldProperty* property = iFieldProperties[i];
        if (property->FieldType().IsSame(aFieldType))
            {
            if (!aName.CompareF(property->XSpName()))
                {
                result = property;
                break;
                }
            else if (property->XSpName() == KNullDesC)
                {
                result = property; //if field has name but there is not
                                   //the proper property take the one with empty name
                }
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::ReadFieldPropertiesL
// ---------------------------------------------------------------------------
//
void CPbk2FieldPropertyArray::ReadFieldPropertiesL(
        TResourceReader& aReader,
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        CPbk2FieldOrderingManager& aOrderingManager )
    {
    // aSupportedFieldTypeList is a list of the supported field type of the used store
    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2FieldProperty* property = CPbk2FieldProperty::NewLC( aReader, 
            aSupportedFieldTypeList, aOrderingManager );
        TBool doNotAdd = EFalse;
        if (property->IsSupported())
            {
			// Non-Japanese variants use all the other properties but
			// Japanese reading fields
			const MVPbkFieldType* lastNameReading = 
				aSupportedFieldTypeList.Find( 
					R_VPBK_FIELD_TYPE_LASTNAMEREADING );
			const MVPbkFieldType* firstNameReading = 
				aSupportedFieldTypeList.Find( 
					R_VPBK_FIELD_TYPE_FIRSTNAMEREADING );
			const MVPbkFieldType& type = property->FieldType();
			if ( ( lastNameReading && 
				   lastNameReading->IsSame( type ) ) ||
				   firstNameReading &&
				   firstNameReading->IsSame( type ) )
				{
				doNotAdd = ETrue;
				}
            }
        else
            {
            doNotAdd = ETrue;
            }

        if (doNotAdd)
            {
            CleanupStack::PopAndDestroy(property);
            }
        else
            {
            iFieldProperties.AppendL(property);
            CleanupStack::Pop(property);
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::CreateFieldOrderingsLC
// ---------------------------------------------------------------------------
//
CPbk2FieldOrderingManager* CPbk2FieldPropertyArray::CreateFieldOrderingsLC( 
        RFs& aRFs )
    {
    RPbk2LocalizedResourceFile resFile( &aRFs );
    resFile.OpenLC(KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile());

    // Read field type presentation order information
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( 
            R_PBK2_FIELDTYPE_PRESENTATION_ORDERS ) );
    CPbk2FieldOrderingManager* orderings = 
            CPbk2FieldOrderingManager::NewL( reader );
    // R_PBK2_FIELDTYPE_PRESENTATION_ORDERS, resFile
    CleanupStack::PopAndDestroy(2); 
    CleanupStack::PushL( orderings );
    return orderings;
    }
    
// ---------------------------------------------------------------------------
// CPbk2FieldPropertyArray::MultiplyImppFieldPropertyL
// ---------------------------------------------------------------------------
//
void CPbk2FieldPropertyArray::MultiplyImppFieldPropertyL(
        CPbk2ServiceManager* aServMan, RFs& aRFs )
    {
    TVPbkFieldVersitProperty prop;
    prop.SetName(EVPbkVersitNameIMPP);
    TInt i = 0;
    for (; i < iFieldProperties.Count(); i++)
        {
        if (iFieldProperties[i]->FieldType().Matches(prop, 0))
            {
            break;
            }
        }
    if (i == iFieldProperties.Count())
        {
        return;
        }
    CPbk2FieldProperty* imppProp = (CPbk2FieldProperty*)iFieldProperties[i];
    
    CPbk2ServiceManager* servMan = aServMan;
    if ( !aServMan )
        {
        servMan = CPbk2ServiceManager::NewL( aRFs );
        CleanupStack::PushL(servMan);
        }
    
    //Below call to refresh the brand data.
    //RefreshDataL is needed because currently branding server
    //does not notify any brand updates (e.g. installation of
    //new brand packages). When notifications in future are
    //provided, this function can be removed from Phonebook.
    servMan->RefreshDataL();
    const CPbk2ServiceManager::RServicesArray& services = servMan->Services(); 
    
    for (TInt j = 0; j < services.Count(); j++)
        {
        CPbk2FieldProperty* prop = new (ELeave) CPbk2FieldProperty;
        CleanupStack::PushL(prop);
        prop->iFieldType = imppProp->iFieldType;
        prop->iFlags = imppProp->iFlags;
        if (!(services[j].iFlags & CPbk2ServiceManager::EInstalled))
            {
            prop->iFlags &= ~KPbk2FieldFlagTemplateField;
            }
        prop->iMaxLength = imppProp->iMaxLength;
        prop->iMultiplicity = imppProp->iMultiplicity;
        prop->iEditMode = imppProp->iEditMode;
        prop->iDefaultCase = imppProp->iDefaultCase;
        prop->iCtrlType = imppProp->iCtrlType;
        prop->iOrderingItem = imppProp->iOrderingItem + j;
        prop->iAddItemOrdering = imppProp->iAddItemOrdering + j;
        prop->iGroupId = imppProp->iGroupId;
        prop->iLocation = imppProp->iLocation;
        prop->iDefaultLabel = services[j].iDisplayName.AllocL();
        prop->iXSpName = services[j].iName.AllocL();
        if (services[j].iBitmapId)
            {
            TUid uid;
            uid.iUid = KPbk2ServManId;
            prop->iIconId = TPbk2IconId(uid, services[j].iBitmapId);
            }
        else
            {
            prop->iIconId = imppProp->iIconId;
            }
        iFieldProperties.InsertL(prop, i);
        CleanupStack::Pop(prop);
        }

    imppProp->iFlags &= ~(KPbk2FieldFlagTemplateField |
            KPbk2FieldFlagUserCanAddField);
    for (i = 0; i < iFieldProperties.Count(); i++)
        {
        CPbk2FieldProperty* fProp = iFieldProperties[i];
        if (!fProp->FieldType().Matches(prop, 0))
            {
            if (fProp->iOrderingItem > imppProp->iOrderingItem)
                {
                fProp->iOrderingItem += services.Count();
                }
            if (fProp->iAddItemOrdering > imppProp->iAddItemOrdering)
                {
                fProp->iAddItemOrdering += services.Count();
                }
            }
        }
    imppProp->iOrderingItem += services.Count();
    imppProp->iAddItemOrdering += services.Count();
    
    if ( !aServMan )
        {
        CleanupStack::PopAndDestroy(servMan);
        }
    }

// End of File
