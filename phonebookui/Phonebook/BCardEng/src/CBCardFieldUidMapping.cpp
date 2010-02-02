/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*    vCard data field <-> UID mapping class.
*
*/


// INCLUDE FILES
#include "CBCardFieldUidMapping.h"
#include <barsread.h>


// MODULE DATA STRUCTURES

struct CBCardFieldUidMapping::TMappingImpl
    {
    // Data
    const TDesC8* iName;
    TInt iFieldIndex;
    TUid iUid;

    /// Default constructor
    TMappingImpl();

    /// Conversion constructor from TVcardField
    TMappingImpl(const TVcardField& aVcardField);

    /**
     * Converts this object to a CBCardFieldUidMapping::TMapping.
     */
    operator CBCardFieldUidMapping::TMapping() const;

    // Array operations
    static TInt CompareName(const TMappingImpl& aLhs, const TMappingImpl& aRhs);
    static TInt CompareIndex(const TMappingImpl& aLhs, const TMappingImpl& aRhs);
    static TInt CompareNameAndIndex(const TMappingImpl& aLhs, const TMappingImpl& aRhs);
    };


// ==================== MEMBER FUNCTIONS ====================

inline CBCardFieldUidMapping::TMappingImpl::TMappingImpl()
    {
    // No need to init anything
    }

inline CBCardFieldUidMapping::TMappingImpl::TMappingImpl
        (const TVcardField& aVcardField) :
    iName(&aVcardField.iName), 
    iFieldIndex(aVcardField.iFieldIndex)
    {
    }

inline CBCardFieldUidMapping::TMappingImpl::operator CBCardFieldUidMapping::TMapping() const
    {
    TMapping result;
    result.iVcardField.iName.Set(*iName);
    result.iVcardField.iFieldIndex = iFieldIndex;
    result.iUid = iUid;
    return result;
    }

inline TInt CBCardFieldUidMapping::TMappingImpl::CompareName
        (const TMappingImpl& aLhs, const TMappingImpl& aRhs)
    {
    return (aLhs.iName->CompareF(*aRhs.iName));
    }

inline TInt CBCardFieldUidMapping::TMappingImpl::CompareIndex
        (const TMappingImpl& aLhs, const TMappingImpl& aRhs)
    {
    return (aLhs.iFieldIndex - aRhs.iFieldIndex);
    }

TInt CBCardFieldUidMapping::TMappingImpl::CompareNameAndIndex
        (const TMappingImpl& aLhs, const TMappingImpl& aRhs)
    {
    TInt result = CompareName(aLhs,aRhs);
    if (result == 0)
        {
        // If names are equal compare indexes
        result = CompareIndex(aLhs,aRhs);
        }
    return result;
    }

inline CBCardFieldUidMapping::CBCardFieldUidMapping()
    {
    }

inline void CBCardFieldUidMapping::ConstructL
        (TResourceReader& aReader)
    {
    // Read the array
    TInt count = aReader.ReadInt16();
    while (--count >= 0)
        {
        HBufC* name16 = aReader.ReadHBufCL();
        CleanupStack::PushL(name16);
        HBufC8* name = HBufC8::NewLC(name16->Length());
        TPtr8 namePtr = name->Des();
        namePtr.Copy(*name16);

        TMappingImpl newMapping;
        newMapping.iName  = name;

        newMapping.iFieldIndex = aReader.ReadInt32();
        newMapping.iUid   = TUid::Uid(aReader.ReadInt32());
        User::LeaveIfError(iMappings.Append(newMapping));
        CleanupStack::Pop(name);
        CleanupStack::PopAndDestroy(name16);
        }

    // Sort the array by (name,index)
    iMappings.Sort(TLinearOrder<TMappingImpl>(TMappingImpl::CompareNameAndIndex));
    }

CBCardFieldUidMapping* CBCardFieldUidMapping::NewL(TResourceReader& aReader)
    {
    CBCardFieldUidMapping* self = new(ELeave) CBCardFieldUidMapping;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;
    }

CBCardFieldUidMapping::~CBCardFieldUidMapping()
    {
    for (TInt i=iMappings.Count()-1; i >= 0; --i)
        {
        // The cast below is safe, see ConstructL()
        delete ((HBufC8*)(iMappings[i].iName));
        }
    iMappings.Close();
    }

TInt CBCardFieldUidMapping::Count() const
    {
    return iMappings.Count();
    }

CBCardFieldUidMapping::TMapping CBCardFieldUidMapping::operator[]
        (TInt aIndex) const
    {
    return iMappings[aIndex];
    }

TInt CBCardFieldUidMapping::Find
        (const TVcardField& aVcardField) const
    {
    TMappingImpl fieldToFind(aVcardField);
    return iMappings.FindInOrder
        (fieldToFind, 
        TLinearOrder<TMappingImpl>(TMappingImpl::CompareNameAndIndex));
    }

TInt CBCardFieldUidMapping::Find
        (const TUid& aUid) const
    {
    // Inline find loop instead of RArray::Find for speed
    const TInt count = iMappings.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iMappings[i].iUid == aUid)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

TPtrC8 CBCardFieldUidMapping::FindName
        (const TUid& aUid) const
    {
    TPtrC8 result;
    const TInt index = Find(aUid);
    if (index != KErrNotFound)
        {
        result.Set(*iMappings[index].iName);
        }
    
    return result;
    }

TInt CBCardFieldUidMapping::FindIndex
        (const TUid& aUid) const
    {
    TInt index = Find(aUid);
    if (index == KErrNotFound)
        {
        return KErrNotFound;
        }
    else
        {
        return iMappings[index].iFieldIndex;
        }
    }


// End of File
