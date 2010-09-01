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
* Description:  Field Type Parameters
*
*/


// INCLUDES
#include "TVPbkFieldTypeParameters.h"
#include <barsread.h>
#include "VPbkError.h"

// LOCAL
namespace
    {
    const TUint KLastFiveBits = 0x1fu;
    const TInt KFiveBits = 5;
    /**
     * Calculates the 32-bit parameter word number from aParam. Masks out-of-
     * range values off aParam.
     */
    inline TInt IndexOf(TVPbkFieldTypeParameter aParam)
        {
        return TInt((TUint(aParam) & ((KVPbkMaxVersitParams - 1) ^ KLastFiveBits)) 
               >> KFiveBits );
        }

    /**
     * Calculates the 32-bit parameter word number from aParam.
     * Checks aParam for validity and panics with aIndexErrorCode if out of range.
     */
    inline TInt CheckedIndexOf
            (TVPbkFieldTypeParameter aParam,
            VPbkError::TPanicCode aIndexErrorCode)
        {
        __ASSERT_ALWAYS( !(TUint(aParam) & (KMaxTUint32 - (KVPbkMaxVersitParams - 1))),
            VPbkError::Panic(aIndexErrorCode) );
        (void)aIndexErrorCode;
        return IndexOf(aParam);
        }

    /**
     * Calculates the 32-bit parameter bit from aParam.
     */
    inline TUint32 BitOf(TVPbkFieldTypeParameter aParam)
        {
        return (1u << (TUint(aParam) & KLastFiveBits));
        }
    }

EXPORT_C void TVPbkFieldTypeParameters::ReadFromResource
        (TResourceReader& aResReader)
    {
    Reset();
    AddFromResource(aResReader);
    }

EXPORT_C void TVPbkFieldTypeParameters::DoAdd(TVPbkFieldTypeParameter aParam)
    {
    iParams[ CheckedIndexOf(aParam, VPbkError::EInvalidTypeParameter) ] 
        |= BitOf(aParam);
    }

EXPORT_C void TVPbkFieldTypeParameters::DoAddAll
        (const TVPbkFieldTypeParameters& aParams)
    {
    for (TInt i=0; i < KBufSize; ++i)
        {
        iParams[i] |= aParams.iParams[i];
        }
    }

EXPORT_C void TVPbkFieldTypeParameters::DoRemove(TVPbkFieldTypeParameter aParam)
    {
    iParams[ CheckedIndexOf(aParam, VPbkError::EInvalidTypeParameter) ] &= 
        ~BitOf(aParam);
    }

EXPORT_C void TVPbkFieldTypeParameters::DoRemoveAll
        (const TVPbkFieldTypeParameters& aParams)
    {
    for (TInt i=0; i < KBufSize; ++i)
        {
        iParams[i] &= ~(aParams.iParams[i]);
        }
    }

EXPORT_C void TVPbkFieldTypeParameters::Reset()
    {
    for (TInt i=0; i < KBufSize; ++i)
        {
        iParams[i] = 0;
        }
    }

EXPORT_C TBool TVPbkFieldTypeParameters::Contains(TVPbkFieldTypeParameter aParam) const
    {
    return ((iParams[IndexOf(aParam)] & BitOf(aParam)) != 0u);
    }

EXPORT_C TBool TVPbkFieldTypeParameters::ContainsAll
        (const TVPbkFieldTypeParameters& aOtherParams) const
    {
    for (TInt i=0; i < KBufSize; ++i)
        {
        const TUint other = aOtherParams.iParams[i];
        if ( (iParams[i] & other) ^ other )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

EXPORT_C TBool TVPbkFieldTypeParameters::ContainsNone
        (const TVPbkFieldTypeParameters& aOtherParams) const
    {
    for (TInt i=0; i < KBufSize; ++i)
        {
        const TUint other = aOtherParams.iParams[i];
        if (iParams[i] & other)
            {
            return EFalse;
            }
        }
    return ETrue;
    }

void TVPbkFieldTypeParameters::AddFromResource
        (TResourceReader& aResReader)
    {
    TInt count = aResReader.ReadInt8();
    while (count-- > 0)
        {
        const TVPbkFieldTypeParameter param = 
            static_cast<TVPbkFieldTypeParameter>(aResReader.ReadInt8());
        iParams[ CheckedIndexOf(param, VPbkError::EInvalidTypeParameterResource) ] 
            |= BitOf(param);
        }
    }

TUint32* TVPbkFieldTypeParameters::FieldTypeParameters()
    {
    return iParams;
    }

TInt TVPbkFieldTypeParameters::Size()
    {
    return KBufSize;
    }

// End of file
