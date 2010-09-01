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
* Description: 
*
*/


#include "CVPbkVCardFieldTypeProperty.h"
#include <TVPbkFieldVersitProperty.h>

#include <barsread.h>

// class TVPbkVCardIndexedProperty
void TVPbkVCardIndexedProperty::InitializeL
        ( TResourceReader& aResReader,
          VPbkEngUtils::CTextStore& aTextStore )
    {
    iIndex = aResReader.ReadInt32();
    iProperty.InitializeL( aResReader, aTextStore );
    }

TInt TVPbkVCardIndexedProperty::Index() const
    {
    return iIndex;
    }

TVPbkVCardFieldTypeMapping TVPbkVCardIndexedProperty::Property() const
    {
    return iProperty;
    }

const MVPbkFieldType* TVPbkVCardIndexedProperty::FindMatch(
                            const MVPbkFieldTypeList& aFieldTypeList) const
    {
    return iProperty.FindMatch( aFieldTypeList );
    }

// class CVPbkVCardFieldTypeProperty
CVPbkVCardFieldTypeProperty* CVPbkVCardFieldTypeProperty::NewLC
    ( TResourceReader& aResReader,
      VPbkEngUtils::CTextStore& aTextStore )
    {
    CVPbkVCardFieldTypeProperty* self = new (ELeave) CVPbkVCardFieldTypeProperty();
    CleanupStack::PushL( self );
    self->ConstructL( aResReader, aTextStore );
    return self;
    }

CVPbkVCardFieldTypeProperty::~CVPbkVCardFieldTypeProperty()
    {
    delete iName;
    iIndexedProperties.Close();
    }

const TVPbkVCardIndexedProperty&
CVPbkVCardFieldTypeProperty::IndexedPropertyAt( TInt aIndex ) const
    {
    return iIndexedProperties[ aIndex ];
    }

const TDesC8& CVPbkVCardFieldTypeProperty::Name() const
    {
    return *iName;
    }

TVPbkVersitStorageType CVPbkVCardFieldTypeProperty::StorageType() const
    {
    return iStorageType;
    }

TInt CVPbkVCardFieldTypeProperty::Count() const
    {
    return iIndexedProperties.Count();
    }

CVPbkVCardFieldTypeProperty::CVPbkVCardFieldTypeProperty()
    {
    }

void CVPbkVCardFieldTypeProperty::ConstructL
    ( TResourceReader& aResReader, VPbkEngUtils::CTextStore& aTextStore )
    {
    // VCARD_FIELDTYPE_PROPERTY::name
    HBufC* name16 = aResReader.ReadHBufCL();
    CleanupStack::PushL(name16);
    HBufC8* name = HBufC8::NewLC(name16->Length());
    TPtr8 namePtr = name->Des();
    namePtr.Copy(*name16);
    iName = name;
    CleanupStack::Pop(); // name
    CleanupStack::PopAndDestroy(); // name16

    // VCARD_FIELDTYPE_PROPERTY::versitStorageType
    iStorageType = static_cast< TVPbkVersitStorageType >( aResReader.ReadInt8() );
    TInt indexedPropertyCount( aResReader.ReadInt16() );
    while( indexedPropertyCount-- > 0 )
        {
        TVPbkVCardIndexedProperty property;
        property.InitializeL( aResReader, aTextStore );
        User::LeaveIfError( iIndexedProperties.Append( property ) );
        }
    }

// End of file
