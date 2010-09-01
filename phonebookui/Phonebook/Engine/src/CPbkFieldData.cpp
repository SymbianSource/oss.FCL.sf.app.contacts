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
*       Classes for storing Phonebook contact data in a contact database 
*
*/


// INCLUDE FILES
#include <CPbkFieldData.h>
#include <f32file.h>    // TParsePtr

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KDefaultArrayGranurality = 8;

enum TPanicCode
    {
    EPanicInvalidType = 1
    };

// ==================== LOCAL FUNCTIONS ====================

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkFieldData");
    User::Panic(KPanicText, aReason);
    }

/**
 * Writes aTime to aStream.
 */
RWriteStream& operator<<(RWriteStream& aStream, const TTime& aTime)
    {
    const TInt64 time(aTime.Int64());
    aStream.WriteInt32L(I64HIGH(time));
    aStream.WriteInt32L(I64LOW(time));
    return aStream;
    }

/**
 * Reads aTime from aStream.
 */
RReadStream& operator>>(RReadStream& aStream, TTime& aTime)
{
  TUint h = aStream.ReadInt32L();
  TUint l = aStream.ReadInt32L();
  aTime = MAKE_TINT64(h,l);
  return aStream;
}

}  // namespace


// ==================== MEMBER FUNCTIONS ====================

inline CPbkFieldData::CPbkFieldData()
    {
    // CBase::operator new(TLeave) resets member data
    }

EXPORT_C CPbkFieldData* CPbkFieldData::NewL
        (TPbkFieldId aFieldId, const TDesC& aText)
    {
    CPbkFieldData* self = new(ELeave) CPbkFieldData;
    CleanupStack::PushL(self);
	self->ConstructL(aFieldId, aText);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkFieldData* CPbkFieldData::NewL
        (TPbkFieldId aFieldId, const TTime& aTime)
    {
    CPbkFieldData* self = new(ELeave) CPbkFieldData;
    CleanupStack::PushL(self);
	self->ConstructL(aFieldId, aTime);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkFieldData* CPbkFieldData::NewL(RReadStream& aStream)
    {
    CPbkFieldData* self = new(ELeave) CPbkFieldData;
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CPbkFieldData::ConstructL
		(TPbkFieldId aFieldId, const TDesC& aText)
    {
    iType = ETypeText;
    iFieldId = aFieldId;
    if (aText.Length() > 0)
        {
        iText = aText.AllocL();
        }
    }

void CPbkFieldData::ConstructL
		(TPbkFieldId aFieldId, const TTime& aTime)
    {
    iType = ETypeTime;
    iFieldId = aFieldId;
    iTime = aTime;
    }

void CPbkFieldData::ConstructL(RReadStream& aStream)
    {
	InternalizeL(aStream);
    }


CPbkFieldData::~CPbkFieldData()
    {
    delete iText;
    }

EXPORT_C const TDesC& CPbkFieldData::Text() const
    {
    __ASSERT_ALWAYS(iType==ETypeText, Panic(EPanicInvalidType));

    if (iText)
        {
        return *iText;
        }
    else
        {
        return KNullDesC;
        }
    }

EXPORT_C const TTime& CPbkFieldData::Time() const
    {
    __ASSERT_ALWAYS(iType==ETypeTime, Panic(EPanicInvalidType));
    return iTime;
    }

EXPORT_C TStorageType CPbkFieldData::PbkFieldType() const
    {
    return iType;
    }

EXPORT_C TPbkFieldId CPbkFieldData::PbkFieldId() const
    {
    return iFieldId;
    }

EXPORT_C TPtrC CPbkFieldData::PbkFieldText() const
    {
    return Text();
    }

EXPORT_C TTime CPbkFieldData::PbkFieldTime() const
    {
    return Time();
    }

EXPORT_C void CPbkFieldData::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt8L(iType);
    aStream.WriteInt32L(iFieldId);
    switch (iType)
        {
        case ETypeText:
            aStream.WriteInt32L(iText->Length());
            aStream << *iText;
            break;
        case ETypeTime:
            aStream << iTime;
            break;
        }
    }

EXPORT_C void CPbkFieldData::InternalizeL(RReadStream& aStream)
    {
    if (iType == ETypeText)
        {
        delete iText;
        iText = NULL;
        }
    iType = static_cast<TType>(aStream.ReadInt8L());
    iFieldId = aStream.ReadInt32L();
    switch (iType)
        {
        case ETypeText:
            iText = HBufC::NewL(aStream, aStream.ReadInt32L());
            break;
        case ETypeTime:
            aStream >> iTime;
            break;
        }
    }

inline CPbkFieldDataArray::CPbkFieldDataArray() :
        CArrayPtrFlat<CPbkFieldData>(KDefaultArrayGranurality)
    { 
    }

EXPORT_C CPbkFieldDataArray* CPbkFieldDataArray::NewL(RReadStream& aStream)
    {
    CPbkFieldDataArray* self = new(ELeave) CPbkFieldDataArray;
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkFieldDataArray* CPbkFieldDataArray::NewL()
    {
	return new(ELeave) CPbkFieldDataArray;
    }

EXPORT_C void CPbkFieldDataArray::ResetAndDestroyPushL()
    {
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy,this));
    }

EXPORT_C void CPbkFieldDataArray::ExternalizeL(RWriteStream& aStream) const
    {
    const TInt count = Count();
    aStream.WriteInt32L(count);
    for (TInt i=0; i < count; ++i)
        {
        At(i)->ExternalizeL(aStream);
        }
    }

EXPORT_C void CPbkFieldDataArray::InternalizeL(RReadStream& aStream)
    {
    ResetAndDestroy();
    TInt count = aStream.ReadInt32L();
    SetReserveL(count);
    while (count-- > 0)
        {
        // AppendL() cannot leave because of the SetReserveL() call above
        AppendL(CPbkFieldData::NewL(aStream));
        }
    }

TInt CPbkFieldDataArray::PbkFieldCount() const
    {
    return Count();
    }

MPbkFieldData& CPbkFieldDataArray::PbkFieldAt(TInt aIndex)
    {
    return *At(aIndex);
    }

const MPbkFieldData& CPbkFieldDataArray::PbkFieldAt(TInt aIndex) const
    {
    return *At(aIndex);
    }

void CPbkFieldDataArray::CleanupResetAndDestroy(TAny* aSelf)
    {
    CPbkFieldDataArray* self = static_cast<CPbkFieldDataArray*>(aSelf);
    self->ResetAndDestroy();
    delete self;
    }

// End of File
