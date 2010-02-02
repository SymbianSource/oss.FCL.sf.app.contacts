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
*       Provides methods for Generic Phonebook view state.
*
*/


// INCLUDE FILES
#include "CPbkViewState.h"
#include <s32mem.h>
#include "PbkUID.h"
#include <CPbkFieldData.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    { 
    EPanicInvalidDataType = 1,
    EPanicInvalidMarkedContactCount
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkViewState");
    User::Panic(KPanicText, aReason);
    }
} // namespace

// CONSTANTS
const TUid CPbkViewState::KUid = { KPbkViewStateUid };


// MODULE DATA STRUCTURES

/**
 * Stream buffer which counts number of bytes written to it.
 */
NONSHARABLE_CLASS(TCalcLengthStreamBuf) : 
        public MStreamBuf
    {
    public:  // Interface
		/**
		 * Constructor.
		 */
        inline TCalcLengthStreamBuf()
            : iBytesWritten(0)
            {
            }

		/**
		 * Returns the number bytes written.
		 */
        inline TInt BytesWritten() const
            {
            return iBytesWritten;
            }

		/**
		 * Resets the bytes written count.
		 */
        inline void Reset()
            {
            iBytesWritten = 0;
            }

    private:  // from MStreamBuf
	    void DoWriteL(const TAny* /*aPtr*/,TInt aLength)
            {
            // Just count the bytes
            iBytesWritten += aLength;
            }

    private:  // Data
        /// Ref: count of bytes written to this streambuf
        TInt iBytesWritten;
    };


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkViewState* CPbkViewState::NewL()
    {
    CPbkViewState* self = new(ELeave) CPbkViewState;
    return self;
    }

EXPORT_C CPbkViewState* CPbkViewState::NewLC()
    {
    CPbkViewState* self = new(ELeave) CPbkViewState;
    CleanupStack::PushL(self);
    return self;
    }

EXPORT_C CPbkViewState* CPbkViewState::NewL(RReadStream& aStream)
    {
    CPbkViewState* self = CPbkViewState::NewLC(aStream);
    CleanupStack::Pop();  // self
    return self;
    }

EXPORT_C CPbkViewState* CPbkViewState::NewLC(RReadStream& aStream)
    {
    CPbkViewState* self = new(ELeave) CPbkViewState;
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    return self;
    }

EXPORT_C CPbkViewState* CPbkViewState::NewL(const TDesC8& aBuf)
    {
    CPbkViewState* self = CPbkViewState::NewLC(aBuf);
    CleanupStack::Pop();  // self
    return self;
    }

EXPORT_C CPbkViewState* CPbkViewState::NewLC(const TDesC8& aBuf)
    {
    CPbkViewState* self = new(ELeave) CPbkViewState;
    CleanupStack::PushL(self);
    self->UnpackL(aBuf);
    return self;
    }

CPbkViewState::~CPbkViewState()
    {
    delete iMarkedContactIds;
    if (iFieldDataArray)
        {
        iFieldDataArray->ResetAndDestroy();
        delete iFieldDataArray;
        }
    }

EXPORT_C TUid CPbkViewState::Uid()
    {
    return KUid;
    }

EXPORT_C TContactItemId CPbkViewState::FocusedContactId() const 
    { 
    return iFocusedContactId; 
    }

EXPORT_C TContactItemId CPbkViewState::TopContactId() const 
    { 
    return iTopContactId; 
    }

EXPORT_C const CContactIdArray* CPbkViewState::MarkedContactIds() const 
    { 
    return iMarkedContactIds; 
    }

EXPORT_C CContactIdArray* CPbkViewState::MarkedContactIds() 
    { 
    return iMarkedContactIds; 
    }

EXPORT_C TInt CPbkViewState::FocusedFieldIndex() const 
    { 
    return iFocusedFieldIndex; 
    }

EXPORT_C TInt CPbkViewState::TopFieldIndex() const 
    { 
    return iTopFieldIndex; 
    }

EXPORT_C CPbkFieldDataArray* CPbkViewState::FieldDataArray() const 
    { 
    return iFieldDataArray; 
    }

EXPORT_C TContactItemId CPbkViewState::ParentContactId() const
    {
    return iParentContactId;
    }

EXPORT_C TUint CPbkViewState::Flags() const
    {
    return iFlags;
    }

EXPORT_C void CPbkViewState::SetFocusedContactId(TContactItemId aId) 
    { 
    iFocusedContactId=aId; 
    }

EXPORT_C void CPbkViewState::SetTopContactId(TContactItemId aId) 
    { 
    iTopContactId=aId; 
    }

EXPORT_C void CPbkViewState::SetMarkedContactIds(CContactIdArray* aArray)
    {
    delete iMarkedContactIds;
    iMarkedContactIds = aArray;
    }

EXPORT_C void CPbkViewState::SetFocusedFieldIndex(TInt aIndex) 
    { 
    iFocusedFieldIndex=aIndex; 
    }

EXPORT_C void CPbkViewState::SetTopFieldIndex(TInt aIndex) 
    { 
    iTopFieldIndex=aIndex; 
    }

EXPORT_C void CPbkViewState::SetFieldDataArray
        (CPbkFieldDataArray* aFieldDataArray)
    {
    if (iFieldDataArray)
        {
        iFieldDataArray->ResetAndDestroy();
        delete iFieldDataArray;
        }
    iFieldDataArray = aFieldDataArray;
    }

EXPORT_C void CPbkViewState::Reset()
    {
    iFocusedContactId = KNullContactId;
    iTopContactId = KNullContactId;
    iFocusedFieldIndex = -1;
    iTopFieldIndex = -1;
    delete iMarkedContactIds;
    iMarkedContactIds = NULL;
    if (iFieldDataArray)
        {
        iFieldDataArray->ResetAndDestroy();
        delete iFieldDataArray;
        iFieldDataArray = NULL;
        }
    iParentContactId = KNullContactId;
    }

EXPORT_C void CPbkViewState::SetParentContactId
        (TContactItemId aParentContactId)
    {
    iParentContactId = aParentContactId;
    }

EXPORT_C void CPbkViewState::SetFlags
        (TUint aFlags)
    {
    iFlags = aFlags;
    }

/*EXPORT_C*/ void CPbkViewState::MergeViewState(CPbkViewState& aOtherState)
    {
    if (aOtherState.iFocusedContactId != KNullContactId)
        {
        iFocusedContactId = aOtherState.iFocusedContactId;
        }
    if (aOtherState.iTopContactId != KNullContactId)
        {
        iTopContactId = aOtherState.iTopContactId;
        }
    if (aOtherState.iFocusedFieldIndex >= 0)
        {
        iFocusedFieldIndex = aOtherState.iFocusedFieldIndex;
        }
    if (aOtherState.iTopFieldIndex >= 0)
        {
        iTopFieldIndex = aOtherState.iTopFieldIndex;
        }
    if (aOtherState.iMarkedContactIds && aOtherState.iMarkedContactIds->Count() > 0)
        {
        delete iMarkedContactIds;
        iMarkedContactIds = aOtherState.iMarkedContactIds;
        aOtherState.iMarkedContactIds = NULL;
        }
    if (aOtherState.iFieldDataArray && aOtherState.iFieldDataArray->Count() > 0)
        {
        delete iFieldDataArray;
        iFieldDataArray = aOtherState.iFieldDataArray;
        aOtherState.iFieldDataArray = NULL;
        }
    if (aOtherState.iParentContactId != KNullContactId)
        {
        iParentContactId = aOtherState.iParentContactId;
        }
    }

EXPORT_C HBufC8* CPbkViewState::PackL() const
    {
    HBufC8* buf = PackLC();
    CleanupStack::Pop();  // buf
    return buf;
    }

EXPORT_C HBufC8* CPbkViewState::PackLC() const
    {
    // Calculate needed buffer size using TCalcLengthStreamBuf
    TCalcLengthStreamBuf countBuf;
        {
        RWriteStream countStream(&countBuf);
        // this can't leave because we are using TCalcLengthStreamBuf
        ExternalizeL(countStream); 
        }

    // Allocate a buffer and a stream
    HBufC8* buf = HBufC8::NewLC(countBuf.BytesWritten());
    TPtr8 bufPtr = buf->Des();
    RDesWriteStream stream(bufPtr);
    stream.PushL();

    // Write this object to buf through stream
    ExternalizeL(stream);

    stream.CommitL();
    CleanupStack::PopAndDestroy();  // stream
    return buf;
    }

EXPORT_C void CPbkViewState::UnpackL(const TDesC8& aPack)
    {
    RDesReadStream stream(aPack);
    stream.PushL();
    InternalizeL(stream);
    CleanupStack::PopAndDestroy();  // stream
    }

EXPORT_C void CPbkViewState::ExternalizeL(RWriteStream& aStream) const
    {
    if (iFocusedContactId != KNullContactId)
        {
        aStream.WriteInt8L(EFocusedContactId);
        aStream.WriteInt32L(iFocusedContactId);
        }

    if (iTopContactId != KNullContactId)
        {
        aStream.WriteInt8L(ETopContactId);
        aStream.WriteInt32L(iTopContactId);
        }

    const TInt count = iMarkedContactIds ? iMarkedContactIds->Count() : 0;
    if (count > 0)
        {
        aStream.WriteInt8L(EMarkedContactIds);
        aStream.WriteInt32L(count);
        for (TInt i=0; i < count; ++i)
            {
            aStream.WriteInt32L((*iMarkedContactIds)[i]);
            }
        }

    if (iFocusedFieldIndex >= 0)
        {
        aStream.WriteInt8L(EFocusedFieldIndex);
        aStream.WriteInt32L(iFocusedFieldIndex);
        }

    if (iTopFieldIndex >= 0)
        {
        aStream.WriteInt8L(ETopFieldIndex);
        aStream.WriteInt32L(iTopFieldIndex);
        }

    if (iFieldDataArray && iFieldDataArray->Count() > 0)
        {
        aStream.WriteInt8L(EFieldDataArray);
        iFieldDataArray->ExternalizeL(aStream);
        }

    if (iParentContactId != KNullContactId)
        {
        aStream.WriteInt8L(EParentContactId);
        aStream.WriteInt32L(iParentContactId);
        }

    if (iFlags != ENullFlags)
        {
        aStream.WriteInt8L(EFlags);
        aStream.WriteUint32L(iFlags);
        }

    aStream.WriteInt8L(EEnd);
    }

EXPORT_C void CPbkViewState::InternalizeL(RReadStream& aStream)
    {
    Reset();

    TBool eof = EFalse;
    while (!eof)
        {
        TInt type = EEnd;
        TRAPD(err, type = aStream.ReadInt8L());
        switch (err)
            {
            case KErrEof:
				{
                // End of stream, exit loop like EEnd opcode was read.
                type = EEnd;
                break;
				}
            case KErrNone:
				{
                break;
				}
            default:
				{
                // Some other error than EOF occured.
                User::Leave(err);
                break;
				}
            }

        switch (type)
            {
            case EFocusedContactId:
				{
                iFocusedContactId = aStream.ReadInt32L();
                break;
				}

            case ETopContactId:
				{
                iTopContactId = aStream.ReadInt32L();
                break;
				}

            case EMarkedContactIds:
                {
                TInt count = aStream.ReadInt32L();
                __ASSERT_ALWAYS(count > 0, Panic(EPanicInvalidMarkedContactCount));
                iMarkedContactIds = CContactIdArray::NewL();
                while (count-- > 0)
                    {
                    iMarkedContactIds->AddL(aStream.ReadInt32L());
                    }
                break;
                }

            case EFocusedFieldIndex:
				{
                iFocusedFieldIndex = aStream.ReadInt32L();
                break;
				}

            case ETopFieldIndex:
				{
                iTopFieldIndex = aStream.ReadInt32L();
                break;
				}

            case EFieldDataArray:
				{
                iFieldDataArray = CPbkFieldDataArray::NewL(aStream);
                break;
				}
            case EParentContactId:
                {
                iParentContactId = aStream.ReadInt32L();
                break;
                }

            case EFlags:
                {
                iFlags = aStream.ReadUint32L();
                break;
                }

            case EEnd:
				{
                eof = ETrue;
                break;
				}

            default:
				{
                Panic(EPanicInvalidDataType);
                break;
				}
            }
        }
    }

EXPORT_C TBool CPbkViewState::operator==(const CPbkViewState& aRhs) const
    {
    return iFocusedContactId == aRhs.iFocusedContactId &&
            iTopContactId == aRhs.iTopContactId &&
            iFocusedFieldIndex == aRhs.iFocusedFieldIndex &&
            iTopFieldIndex == aRhs.iTopFieldIndex &&
            iParentContactId == aRhs.iParentContactId; 
    }

/**
 * C++ default constructor.
 */
CPbkViewState::CPbkViewState() :
    iFocusedContactId(KNullContactId),
    iTopContactId(KNullContactId),
    iFocusedFieldIndex(-1),
    iTopFieldIndex(-1),
    iMarkedContactIds(NULL),
    iFieldDataArray(NULL),
    iParentContactId(KNullContactId)
    {
    }


//  End of File  
