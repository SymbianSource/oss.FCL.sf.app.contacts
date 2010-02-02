/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include "TContactFieldDataImpl.h"
#include <VPbkError.h>
#include <cntdef.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "VPbkCntModelUid.h"




namespace VPbkCntModel {

_LIT(KColon, ":");

// TContactFieldTextData
TBool TContactFieldTextData::IsEmpty() const
    {
    return !iCntModelField->Storage()->IsFull();
    }

void TContactFieldTextData::CopyL(const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS(DataType() == aFieldData.DataType(), 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    SetTextL(MVPbkContactFieldTextData::Cast(aFieldData).Text());
    }

TPtrC TContactFieldTextData::Text() const
    {
    return iCntModelField->TextStorage()->Text();
    }

void TContactFieldTextData::SetTextL(const TDesC& aText)
    {
    iCntModelField->TextStorage()->SetTextL(aText);
    }

TInt TContactFieldTextData::MaxLength() const
    {
    return KVPbkUnlimitedFieldLength;
    }

// TContactFieldBinaryData
TBool TContactFieldBinaryData::IsEmpty() const
    {
    return !iCntModelField->Storage()->IsFull();
    }

void TContactFieldBinaryData::CopyL(const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS(DataType() == aFieldData.DataType(), 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    SetBinaryDataL(MVPbkContactFieldBinaryData::Cast(aFieldData).BinaryData());
    }

TPtrC8 TContactFieldBinaryData::BinaryData() const
    {
    TPtrC8 result;
    HBufC8* buf = iCntModelField->StoreStorage()->Thing();
    if (buf)
        {
        result.Set(*buf);
        }
    return result;
    }

void TContactFieldBinaryData::SetBinaryDataL(const TDesC8& aBinaryData)
    {
    iCntModelField->StoreStorage()->SetThingL(aBinaryData);
    }

// TContactFieldDateTimeData
TBool TContactFieldDateTimeData::IsEmpty() const
    {
    return !iCntModelField->Storage()->IsFull();
    }

void TContactFieldDateTimeData::CopyL(const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS(DataType() == aFieldData.DataType(), 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    SetDateTime(MVPbkContactFieldDateTimeData::Cast(aFieldData).DateTime());
    }

TTime TContactFieldDateTimeData::DateTime() const
    {
    return iCntModelField->DateTimeStorage()->Time();
    }

void TContactFieldDateTimeData::SetDateTime(const TTime& aTime)
    {
    iCntModelField->DateTimeStorage()->SetTime(aTime);
    }

// TContactFieldUriData
TBool TContactFieldUriData::IsEmpty() const
    {
    return !iCntModelField->Storage()->IsFull();
    }

void TContactFieldUriData::CopyL(const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS(DataType() == aFieldData.DataType(), 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    SetUriL(MVPbkContactFieldUriData::Cast(aFieldData).Uri());
    }

TPtrC TContactFieldUriData::Scheme() const
    {
    TPtrC ptr = iCntModelField->TextStorage()->Text();
    if (!ptr.Length())
        {
        return ptr;
        }
    TInt pos = ptr.Find(KColon);
    __ASSERT_DEBUG(pos > 0, 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    return ptr.Left(pos);
    }

TPtrC TContactFieldUriData::Text() const
    {
    TPtrC ptr = iCntModelField->TextStorage()->Text();
    if (!ptr.Length())
        {
        return ptr;
        }
    TInt pos = ptr.Find(KColon);
    __ASSERT_DEBUG(pos > 0 && pos + 1 < ptr.Length(), 
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch));
    return ptr.Mid(++pos);
    }

TPtrC TContactFieldUriData::Uri() const
    {
    return iCntModelField->TextStorage()->Text();
    }

// aUri: URI as specified in RFC 3986
// For example:
//   ftp://ftp.is.co.za/rfc/rfc1808.txt
//   ldap://[2001:db8::7]/c=GB?objectClass?one
//   mailto:John.Doe@example.com
//   tel:+1-816-555-1212
void TContactFieldUriData::SetUriL(const TDesC& aUri)
    {
    if (aUri.Length())
        {
        // skip th validate if field type is IMPP, because IMPP is not URI,
        // scheme of URI is taken from sXP service name, which might contains
        // special characters
        TFieldType fType = {KUidContactFieldIMPPValue};
        if (!iCntModelField->ContentType().ContainsFieldType(fType))
            {
            if ( !Validate(aUri, ETrue) )
               {
               User::Leave(KErrNotSupported);
               }
            }
        
        }
    iCntModelField->TextStorage()->SetTextL(aUri);
    }

// aScheme: scheme part of URI aText: rest of URI
// For example:
//   aScheme:                  aText:
//   ftp                       //ftp.is.co.za/rfc/rfc1808.txt
//   ldap                      //[2001:db8::7]/c=GB?objectClass?one
//   mailto                    John.Doe@example.com
//   tel                       +1-816-555-1212
void TContactFieldUriData::SetUriL(const TDesC& aScheme, const TDesC& aText)
    {
    if (!aText.Length())
        {
        if (aScheme.Length())
            {
            User::Leave(KErrCorrupt);
            }
        }
    if (aScheme.Length())
        {
        /*
        skip th validate if field type is IMPP, because IMPP is not URI,
        scheme of URI is taken from sXP service name, which might contains
        special characters
        */
        TFieldType fType = {KUidContactFieldIMPPValue};
        if (!iCntModelField->ContentType().ContainsFieldType(fType))
            {
            if ( !Validate(aScheme, EFalse))
               {
               User::Leave(KErrCorrupt);
               }
            }
        HBufC* buf = HBufC::NewLC(aScheme.Length() + KColon().Length() +
                aText.Length());
        TPtr ptr(NULL, 0);
        ptr.Set(buf->Des());
        ptr = aScheme;
        ptr.Append(KColon);
        ptr.Append(aText);
        iCntModelField->TextStorage()->SetTextL(*buf);
        CleanupStack::PopAndDestroy(buf);
        }
    else
        {
        if (aText.Length())
            {
            User::Leave(KErrCorrupt);
            }
        iCntModelField->TextStorage()->SetTextL(KNullDesC);
        }
    }

TInt TContactFieldUriData::MaxLength() const
    {
    return KVPbkUnlimitedFieldLength;
    }

TBool TContactFieldUriData::Validate(const TDesC& aUri, TBool aFullUri)
    {
    if (!((TChar)aUri[0]).IsAlpha())
        {
        return EFalse;
        }
    for (TInt i = 1; i < aUri.Length(); i++)
        {
        TChar chr = aUri[i];
        if ((chr == ':') && aFullUri)
            {
            return ETrue;
            }
        if (!chr.IsAlphaDigit() && (chr != '+') && (chr != '-') && (chr != '.'))
            {
            return EFalse;
            }
        }
    if (aFullUri)
        {
        return EFalse;
        }
    return ETrue;
    }

}  // namespace VPbkCntModel

