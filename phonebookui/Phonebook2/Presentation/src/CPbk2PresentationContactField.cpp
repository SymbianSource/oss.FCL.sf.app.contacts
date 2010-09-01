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
* Description:  A phonebook2 presentation level contact field
*
*/



// INCLUDE FILES
#include "CPbk2PresentationContactField.h"
#include "CPbk2PresentationContactFieldTextData.h"


#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MPbk2FieldProperty.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkFieldType.h>

namespace{

const TInt KStaticLength = 35;

} // namespace

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::CPbk2PresentationContactField
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2PresentationContactField::CPbk2PresentationContactField(
    const MPbk2FieldProperty& aFieldProperty,
    MVPbkStoreContact& aParentContact)
    :   iFieldProperty(aFieldProperty),
        iParentContact(aParentContact),
        iDynamicVisibility( ETrue )
    {
    }


// --------------------------------------------------------------------------
// CPbk2PresentationContactField::~CPbk2PresentationContactField
// --------------------------------------------------------------------------
//
CPbk2PresentationContactField::~CPbk2PresentationContactField()
    {
    delete iData;
    delete iStoreField;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2PresentationContactField* CPbk2PresentationContactField::NewL
        (const MPbk2FieldProperty& aFieldProperty,
        const MVPbkStoreContactField& aStoreField,
        MVPbkStoreContact& aParentContact)
    {
    CPbk2PresentationContactField* self = 
        NewLC(aFieldProperty, aStoreField, aParentContact);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2PresentationContactField* CPbk2PresentationContactField::NewLC
    (const MPbk2FieldProperty& aFieldProperty,
    const MVPbkStoreContactField& aStoreField,
    MVPbkStoreContact& aParentContact)
    {
    CPbk2PresentationContactField* self = 
        new(ELeave) CPbk2PresentationContactField(aFieldProperty, 
        aParentContact);
    CleanupStack::PushL(self);
    self->ConstructL(aStoreField);
    return self;
    }
    
// --------------------------------------------------------------------------
// CPbk2PresentationContactField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactField::ConstructL
        (const MVPbkStoreContactField& aStoreField)
    {
    // Make a permanent copy from the field reference
    iStoreField = aStoreField.CloneLC();
    CleanupStack::Pop(); // iStoreField

    if ( FieldData().DataType() == EVPbkFieldStorageTypeText )
        {        
        MVPbkContactFieldTextData& textData = 
            MVPbkContactFieldTextData::Cast( iStoreField->FieldData() );
        iData = 
            CPbk2PresentationContactFieldTextData::NewL( 
                textData, iFieldProperty, MaxDataLength() );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2PresentationContactField::MaxDataLength
// --------------------------------------------------------------------------
//  
EXPORT_C TInt CPbk2PresentationContactField::MaxDataLength() const
    {
    TInt maxLength = KVPbkUnlimitedFieldLength;
    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData& textData = 
                MVPbkContactFieldTextData::Cast(iStoreField->FieldData());
            maxLength = textData.MaxLength();    
            break;
            }
        case EVPbkFieldStorageTypeBinary:
            {
            const MVPbkContactFieldBinaryData& binaryData = 
                MVPbkContactFieldBinaryData::Cast(iStoreField->FieldData());
            maxLength = binaryData.MaxLength();
            break;
            }
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            }
        }
    
    TInt staticMaxLength = iFieldProperty.MaxLength();
    if (maxLength == KVPbkUnlimitedFieldLength)
        {
        maxLength = staticMaxLength;
        }
    else
        {
        maxLength = Min(staticMaxLength, maxLength);
        }
    return maxLength;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::IsEmpty
// --------------------------------------------------------------------------
//    
EXPORT_C TBool CPbk2PresentationContactField::IsEmpty() const
    {
    return iStoreField->FieldData().IsEmpty();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::IsEditable
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2PresentationContactField::IsEditable() const
    {
    return !(iFieldProperty.Flags() & KPbk2FieldFlagDisableEdit);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::IsRemovable
// --------------------------------------------------------------------------
//    
EXPORT_C TBool CPbk2PresentationContactField::IsRemovable() const
    {
    return !(iFieldProperty.Flags() & KPbk2FieldFlagCanNotBeRemoved);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::IsVisibleInDetailsView
// --------------------------------------------------------------------------
//    
EXPORT_C TBool CPbk2PresentationContactField::IsVisibleInDetailsView() const
    {
    return !(iFieldProperty.Flags() & KPbk2FieldFlagHideFromDetailsView);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::SetDynamicVisibility
// --------------------------------------------------------------------------
// 
EXPORT_C void CPbk2PresentationContactField::SetDynamicVisibility(
    TBool aVisibility )
    {
    iDynamicVisibility = aVisibility;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::SetDynamicVisibility
// --------------------------------------------------------------------------
// 
EXPORT_C TBool CPbk2PresentationContactField::DynamicVisibility()
    {
    return iDynamicVisibility;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CPbk2PresentationContactField::ParentObject() const
    {
    return iStoreField->ParentObject();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact& CPbk2PresentationContactField::ParentContact() const
    {
    return iStoreField->ParentContact();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::ParentContact
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CPbk2PresentationContactField::MatchFieldType(
    TInt aMatchPriority) const
    {
    return iStoreField->MatchFieldType(aMatchPriority);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::BestMatchingFieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType* 
        CPbk2PresentationContactField::BestMatchingFieldType() const
    {
    return iStoreField->BestMatchingFieldType();
    }
    
// --------------------------------------------------------------------------
// CPbk2PresentationContactField::FieldData
// --------------------------------------------------------------------------
//
const MVPbkContactFieldData& CPbk2PresentationContactField::FieldData() const
    {
    
    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            if ( iData )
                {
                return *iData;
                }            
            else
                {
                return iStoreField->FieldData();
                }
            }
        case EVPbkFieldStorageTypeBinary: // FALLTHROUGH
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            return iStoreField->FieldData();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::IsSame
// --------------------------------------------------------------------------
//
TBool CPbk2PresentationContactField::IsSame(
    const MVPbkBaseContactField& aOther) const
    {
    const CPbk2PresentationContactField* field = 
        dynamic_cast<const CPbk2PresentationContactField*>(&aOther);
    if (field)
        {
        return iStoreField->IsSame(*field->iStoreField);
        }
    return iStoreField->IsSame(aOther);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::SupportsLabel
// --------------------------------------------------------------------------
//
TBool CPbk2PresentationContactField::SupportsLabel() const
    {
    TVPbkFieldVersitProperty prop;
    prop.SetName(EVPbkVersitNameIMPP);
    if (iStoreField->BestMatchingFieldType()->Matches(prop, 0))
        {
        return EFalse;
        }
    return iStoreField->SupportsLabel();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::FieldLabel
// --------------------------------------------------------------------------
//
TPtrC CPbk2PresentationContactField::FieldLabel() const
    {
    if (iStoreField->ContactStore().StoreProperties().SupportsFieldLabels()
            && iStoreField->FieldLabel() != KNullDesC)
        {
        return iStoreField->FieldLabel();
        }
    else if (iFieldProperty.DefaultLabel() == KNullDesC)
        {
        TVPbkFieldVersitProperty prop;
        prop.SetName(EVPbkVersitNameIMPP);
        if (iStoreField->BestMatchingFieldType()->Matches(prop, 0))
            {
            return (MVPbkContactFieldUriData::Cast(iStoreField->FieldData())).
                    Scheme();
            }
        }
    return iFieldProperty.DefaultLabel();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::SetFieldLabelL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactField::SetFieldLabelL(const TDesC& aText)
    {
    iStoreField->SetFieldLabelL(aText);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::MaxLabelLength
// --------------------------------------------------------------------------
//
TInt CPbk2PresentationContactField::MaxLabelLength() const
    {
    TInt storeLength = iStoreField->MaxLabelLength();
    if (storeLength == KVPbkUnlimitedLabelLength)
        {
        return KStaticLength;
        }
    else
        {
        return Min(KStaticLength, storeLength);
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2PresentationContactField::FieldData
// --------------------------------------------------------------------------
//
MVPbkContactFieldData& CPbk2PresentationContactField::FieldData()
    {
    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            if ( iData )
                {
                return *iData;
                }            
            else
                {
                return iStoreField->FieldData();
                }
            }
        case EVPbkFieldStorageTypeBinary: // FALLTHROUGH
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            return iStoreField->FieldData();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::CloneLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2PresentationContactField::CloneLC() const
    {
    CPbk2PresentationContactField* copy = CPbk2PresentationContactField::NewLC
        (iFieldProperty, *iStoreField,iParentContact);
    return copy;
    }    
  
// --------------------------------------------------------------------------
// CPbk2PresentationContactField::CreateLinkLC
// --------------------------------------------------------------------------
//  
MVPbkContactLink* CPbk2PresentationContactField::CreateLinkLC() const
    {
    return iStoreField->CreateLinkLC();
    }
  
//  End of File  
