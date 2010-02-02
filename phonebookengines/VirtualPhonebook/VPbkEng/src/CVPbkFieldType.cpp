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
* Description:  Field type
*
*/


// INCLUDES
#include "CVPbkFieldType.h"

// Virtual Phonebook
#include "TVPbkFieldVersitProperty.h"
#include "VPbkUtil.h"

// System includes
#include <barsc.h>
#include <barsread.h>
#include <badesca.h>


// --------------------------------------------------------------------------
// CVPbkFieldType::CVPbkFieldType
// --------------------------------------------------------------------------
//
inline CVPbkFieldType::CVPbkFieldType()
    {
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::~CVPbkFieldType
// --------------------------------------------------------------------------
//
CVPbkFieldType::~CVPbkFieldType()
    {
    iVersitProperties.Close();
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::NewLC
// --------------------------------------------------------------------------
//
CVPbkFieldType* CVPbkFieldType::NewLC(
        RResourceFile& aResFile,
        TResourceReader& aResReader, 
        VPbkEngUtils::CTextStore& aTextStore)
    {
    CVPbkFieldType* self = new(ELeave) CVPbkFieldType;
    CleanupStack::PushL(self);
    self->ConstructL(aResFile, aResReader, aTextStore);
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkFieldType::ConstructL(
        RResourceFile& aResFile,
        TResourceReader& aResReader, 
        VPbkEngUtils::CTextStore& aTextStore)
    {
    // aResReader points to a VPBK_FIELD_TYPE_LINK element
    iFieldTypeResId = aResReader.ReadInt32();
    HBufC8* fieldTypeBuf = aResFile.AllocReadLC(iFieldTypeResId);
    TResourceReader resReader;
    resReader.SetBuffer(fieldTypeBuf);
    
    // VPBK_FIELD_TYPE::versitProperties[]
    TInt propertyCount = resReader.ReadInt8();
    while (propertyCount-- > 0)
        {
        User::LeaveIfError( 
            iVersitProperties.Append(
                TVPbkFieldVersitProperty(resReader)) );
        TVPbkFieldVersitProperty& property = 
            iVersitProperties[iVersitProperties.Count()-1];
        if (property.ExtensionName().Length() > 0)
            {
            // Store the extension name to the text store and set the property
            // pointer to the stored version
            property.SetExtensionName(aTextStore.AddL(property.ExtensionName()));
            }
        }

    // VPBK_FIELD_TYPE::excludedParameters[]
    iExcludedTypeParameters.ReadFromResource(resReader);

    // VPBK_FIELD_TYPE::nonVersitType
    iNonVersitType = 
        static_cast<TVPbkNonVersitFieldType>(resReader.ReadUint8());

    CleanupStack::PopAndDestroy(); // fieldTypeResId buffer
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::VersitProperties
// --------------------------------------------------------------------------
//
TArray<TVPbkFieldVersitProperty> CVPbkFieldType::VersitProperties() const
    {
    return iVersitProperties.Array();
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::ExcludedParameters
// --------------------------------------------------------------------------
//
const TVPbkFieldTypeParameters& CVPbkFieldType::ExcludedParameters() const
    {
    return iExcludedTypeParameters;
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::NonVersitType
// --------------------------------------------------------------------------
//
TVPbkNonVersitFieldType CVPbkFieldType::NonVersitType() const
    {
    return iNonVersitType;
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::IsSame
// --------------------------------------------------------------------------
//
TBool CVPbkFieldType::IsSame(const MVPbkFieldType& aOtherType) const
    {
    // Each field type is a unique instance so object identities 
    // (== this pointers in C++) can be compared directly.
    return (static_cast<const MVPbkFieldType*>(this) == &aOtherType);
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::Matches
// --------------------------------------------------------------------------
//
TBool CVPbkFieldType::Matches
        (const TVPbkFieldVersitProperty& aMatchProperty,
        TInt aMatchPriority) const
    {
    return (
        aMatchPriority < iVersitProperties.Count() &&
        iVersitProperties[aMatchPriority].Matches(aMatchProperty) &&
        aMatchProperty.Parameters().ContainsNone(iExcludedTypeParameters) );
    }

// --------------------------------------------------------------------------
// CVPbkFieldType::FieldTypeResId
// --------------------------------------------------------------------------
//
TInt CVPbkFieldType::FieldTypeResId() const
    {
    return iFieldTypeResId;
    }


namespace VPbkFieldTypeList {

// --------------------------------------------------------------------------
// IsSame
// --------------------------------------------------------------------------
//
EXPORT_C TBool IsSame( const MVPbkFieldTypeList& aLeft, 
        const MVPbkFieldTypeList& aRight )
    {
    TBool result = EFalse;
    const TInt count = aLeft.FieldTypeCount();
    if ( count == aRight.FieldTypeCount() )
        {
        result = ETrue;
        for ( TInt i = 0; i < count && result; ++i )
            {
            result = aLeft.FieldTypeAt(i).IsSame( aRight.FieldTypeAt(i) );
            }
        }
    return result;
    }
}
// End of File
