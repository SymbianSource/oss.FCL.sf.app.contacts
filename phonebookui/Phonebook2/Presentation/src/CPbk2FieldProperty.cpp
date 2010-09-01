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
* Description:  Phonebook 2 field property.
*
*/


#include "CPbk2FieldProperty.h"

// From Phonebook 2
#include "CPbk2FieldOrderingManager.h"

// From Virtual Phonebook
#include <MVPbkFieldType.h>

// System includes
#include <barsread.h>
#include <coemain.h>


// --------------------------------------------------------------------------
// CPbk2FieldProperty::CPbk2FieldProperty
// --------------------------------------------------------------------------
//    
CPbk2FieldProperty::CPbk2FieldProperty()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::~CPbk2FieldProperty
// --------------------------------------------------------------------------
//    
CPbk2FieldProperty::~CPbk2FieldProperty()
    {
    delete iAddItemText;
    delete iDefaultLabel;
    delete iXSpName;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::ConstructL
// --------------------------------------------------------------------------
//    
inline void CPbk2FieldProperty::ConstructL
        ( TResourceReader& aReader,
          const MVPbkFieldTypeList& aSupportedFieldTypeList,
          CPbk2FieldOrderingManager& aFieldOrderingManager )
    {
    aReader.ReadInt8(); // read version number
    const TInt fieldTypeResId = aReader.ReadInt32();

    iFieldType = aSupportedFieldTypeList.Find( fieldTypeResId );
    iMultiplicity = aReader.ReadInt8();
    iMaxLength = aReader.ReadInt16();
    iEditMode = aReader.ReadInt8();
    iDefaultCase = aReader.ReadInt8();
    iIconId = TPbk2IconId( aReader );
    iCtrlType = aReader.ReadInt8();
    iFlags = aReader.ReadUint32();    
    iAddItemText = aReader.ReadHBufCL();    
    iLocation = aReader.ReadInt8();
    iGroupId =  aReader.ReadInt8();
    iDefaultLabel = aReader.ReadHBufCL();
    iXSpName = NULL;

    if ( iFieldType )
        {
        // If field type is supported get ordering items
        iOrderingItem =
            aFieldOrderingManager.OrderingItem( *iFieldType );
        iAddItemOrdering =
            aFieldOrderingManager.AddItemOrdering( *iFieldType );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::NewLC
// --------------------------------------------------------------------------
//    
CPbk2FieldProperty* CPbk2FieldProperty::NewLC
        ( TResourceReader& aReader,
          const MVPbkFieldTypeList& aSupportedFieldTypeList,
          CPbk2FieldOrderingManager& aFieldOrderingManager )
    {
    CPbk2FieldProperty* self = new( ELeave ) CPbk2FieldProperty;
    CleanupStack::PushL( self );
    self->ConstructL( aReader, aSupportedFieldTypeList, 
        aFieldOrderingManager );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::IsSupported
// --------------------------------------------------------------------------
//    
TBool CPbk2FieldProperty::IsSupported() const
    {
    TBool ret = EFalse;

    if ( iFieldType )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::IsSame
// --------------------------------------------------------------------------
//
TBool CPbk2FieldProperty::IsSame( const MPbk2FieldProperty& aOther) const
    {
    MPbk2FieldProperty2* fieldPropertyExtension =
        reinterpret_cast<MPbk2FieldProperty2*>( const_cast<MPbk2FieldProperty&>
            ( aOther ).FieldPropertyExtension(
                KMPbk2FieldPropertyExtension2Uid ) );
    TBool xSpNameIsSame = ETrue;
    if ( fieldPropertyExtension != NULL )
        {
        if ( fieldPropertyExtension->XSpName().CompareF( XSpName() ) != 0 )
            {
            xSpNameIsSame = EFalse;
            }
        }
    
    return ( aOther.FieldType().IsSame( *iFieldType ) && xSpNameIsSame );
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::FieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType& CPbk2FieldProperty::FieldType() const
    {
    return *iFieldType;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::Multiplicity
// --------------------------------------------------------------------------
//    
TPbk2FieldMultiplicity CPbk2FieldProperty::Multiplicity() const
    {
    return static_cast<TPbk2FieldMultiplicity>(iMultiplicity);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::MaxLength
// --------------------------------------------------------------------------
//
TInt CPbk2FieldProperty::MaxLength() const
    {
    return iMaxLength;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::EditMode
// --------------------------------------------------------------------------
//
TPbk2FieldEditMode CPbk2FieldProperty::EditMode() const
    {
    return static_cast<TPbk2FieldEditMode>(iEditMode);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::DefaultCase
// --------------------------------------------------------------------------
//
TPbk2FieldDefaultCase CPbk2FieldProperty::DefaultCase() const
    {
    return static_cast<TPbk2FieldDefaultCase>(iDefaultCase);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::IconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2FieldProperty::IconId() const
    {
    return iIconId;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::CtrlType
// --------------------------------------------------------------------------
//
TPbk2FieldCtrlType CPbk2FieldProperty::CtrlType() const
    {
    return static_cast<TPbk2FieldCtrlType>(iCtrlType);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::Flags
// --------------------------------------------------------------------------
//
TUint CPbk2FieldProperty::Flags() const
    {
    return iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::OrderingItem
// --------------------------------------------------------------------------
//
TInt CPbk2FieldProperty::OrderingItem() const
    {
    return iOrderingItem;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::AddItemOrdering
// --------------------------------------------------------------------------
//
TInt CPbk2FieldProperty::AddItemOrdering() const
    {
    return iAddItemOrdering;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::AddItemText
// --------------------------------------------------------------------------
//
const TDesC& CPbk2FieldProperty::AddItemText() const
    {
    if (iAddItemText)
        {
        return *iAddItemText;
        }
   else if (iDefaultLabel)
        {
        return *iDefaultLabel;
        }
   else
        {
        return KNullDesC;
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::Location
// --------------------------------------------------------------------------
//
TPbk2FieldLocation CPbk2FieldProperty::Location() const
    {
    return static_cast<TPbk2FieldLocation>(iLocation);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::GroupId
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPbk2FieldProperty::GroupId() const
    {
    return static_cast<TPbk2FieldGroupId>(iGroupId);
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::DefaultLabel
// --------------------------------------------------------------------------
//
const TDesC& CPbk2FieldProperty::DefaultLabel() const
    {
    return iDefaultLabel ? *iDefaultLabel : KNullDesC();
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::FieldPropertyExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2FieldProperty::FieldPropertyExtension( TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2FieldPropertyExtension2Uid )
        {
        return static_cast<MPbk2FieldProperty2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FieldProperty::XSpName
// --------------------------------------------------------------------------
//
const TDesC& CPbk2FieldProperty::XSpName() const
    {
    return iXSpName ? *iXSpName : KNullDesC();
    }

// End of file
