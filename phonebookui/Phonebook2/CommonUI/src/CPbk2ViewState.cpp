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
* Description:  Phonebook 2 view state.
*
*/


// INCLUDE FILES
#include <CPbk2ViewState.h>

// Phonebook 2
#include <Pbk2UID.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>

// System includes
#include <s32mem.h>


const TUid CPbk2ViewState::KUid = { KPbk2ViewStateUid };

/// Unnamed namespace for local definitions
namespace {

const TUint8 KPbk2ViewStateVersionNumber = 1;

/**
 * Stream buffer which counts number of bytes written to it.
 */
class TCalcLengthStreamBuf : public MStreamBuf
    {
    public: // Construction

        /**
         * Constructor.
         */
        inline TCalcLengthStreamBuf() :
                iBytesWritten( 0 )
            {
            }

        /**
         * Returns the number of bytes written.
         *
         * @return  Number of bytes written.
         */
        inline TInt BytesWritten() const
            {
            return iBytesWritten;
            }

    private: // From MStreamBuf

        void DoWriteL( const TAny* /*aPtr*/, TInt aLength )
            {
            // Just count the bytes
            iBytesWritten += aLength;
            }

    private: // Data
        /// Ref: Count of bytes written to this streambuf
        TInt iBytesWritten;
    };

enum TPanicCode
    {
    EPanicInvalidVersion = 1,
    EPanicInvalidDataType,
    EPanicInvalidMarkedContactCount
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ViewState");
    User::Panic(KPanicText, aReason);
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ViewState::CPbk2ViewState
// --------------------------------------------------------------------------
//
CPbk2ViewState::CPbk2ViewState() :
        iFocusedFieldIndex( KErrNotFound ),
        iTopFieldIndex( KErrNotFound ),
        iFocusedPropertiesIndex( KErrNotFound ),
        iTopPropertiesIndex( KErrNotFound )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::~CPbk2ViewState
// --------------------------------------------------------------------------
//
CPbk2ViewState::~CPbk2ViewState()
    {
    delete iFocusedContact;
    delete iTopContact;
    delete iParentContact;
    delete iArray;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState* CPbk2ViewState::NewL()
    {
    CPbk2ViewState* self = new ( ELeave ) CPbk2ViewState;
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState* CPbk2ViewState::NewLC()
    {
    CPbk2ViewState* self = new ( ELeave ) CPbk2ViewState;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState* CPbk2ViewState::NewL( const TDesC8& aBuf )
    {
    CPbk2ViewState* self = CPbk2ViewState::NewLC( aBuf );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState* CPbk2ViewState::NewLC( const TDesC8& aBuf )
    {
    CPbk2ViewState* self = new ( ELeave ) CPbk2ViewState;
    CleanupStack::PushL( self );
    self->ConstructL( aBuf );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ViewState::ConstructL( const TDesC8& aBuf )
    {
    UnpackL( aBuf );
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::Uid
// --------------------------------------------------------------------------
//
EXPORT_C TUid CPbk2ViewState::Uid()
    {
    return KUid;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::FocusedContact
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkContactLink* CPbk2ViewState::FocusedContact() const
    {
    return iFocusedContact;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TakeFocusedContact
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLink* CPbk2ViewState::TakeFocusedContact()
    {
    MVPbkContactLink* result = iFocusedContact;
    iFocusedContact = NULL;
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TopContact
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkContactLink* CPbk2ViewState::TopContact() const
    {
    return iTopContact;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TakeTopContact
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLink* CPbk2ViewState::TakeTopContact()
    {
    MVPbkContactLink* result = iTopContact;
    iTopContact = NULL;
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::ParentContact
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkContactLink* CPbk2ViewState::ParentContact() const
    {
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TakeParentContact
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLink* CPbk2ViewState::TakeParentContact()
    {
    MVPbkContactLink* result = iParentContact;
    iParentContact = NULL;
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::MarkedContacts
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkContactLinkArray* CPbk2ViewState::MarkedContacts() const
    {
    return iArray;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TakeMarkedContacts
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLinkArray* CPbk2ViewState::TakeMarkedContacts()
    {
    MVPbkContactLinkArray* result = iArray;
    iArray = NULL;
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ViewState::FocusedFieldIndex() const
    {
    return iFocusedFieldIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TopFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ViewState::TopFieldIndex() const
    {
    return iTopFieldIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::FocusedPropertiesIndex
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ViewState::FocusedPropertiesIndex() const
    {
    return iFocusedPropertiesIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::TopPropertiesIndex
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ViewState::TopPropertiesIndex() const
    {
    return iTopPropertiesIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::Flags
// --------------------------------------------------------------------------
//
EXPORT_C TUint CPbk2ViewState::Flags() const
    {
    return iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetFocusedContact
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetFocusedContact
        ( MVPbkContactLink* aContact )
    {
    delete iFocusedContact;
    iFocusedContact = aContact;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetTopContact
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetTopContact( MVPbkContactLink* aTopContact )
    {
    delete iTopContact;
    iTopContact = aTopContact;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetParentContact
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetParentContact
        ( MVPbkContactLink* aParentContact )
    {
    delete iParentContact;
    iParentContact = aParentContact;
    }


// --------------------------------------------------------------------------
// CPbk2ViewState::SetMarkedContacts
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetMarkedContacts
        ( MVPbkContactLinkArray* aArray )
    {
    delete iArray;
    iArray = aArray;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetFocusedFieldIndex( TInt aIndex )
    {
    iFocusedFieldIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetTopFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetTopFieldIndex( TInt aIndex )
    {
    iTopFieldIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetFocusedPropertiesIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetFocusedPropertiesIndex( TInt aIndex )
    {
    iFocusedPropertiesIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetTopPropertiesIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetTopPropertiesIndex( TInt aIndex )
    {
    iTopPropertiesIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::Reset
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::Reset()
    {
    iFocusedFieldIndex = KErrNotFound;
    iTopFieldIndex = KErrNotFound;
    iFocusedPropertiesIndex = KErrNotFound;
    iTopPropertiesIndex = KErrNotFound;
    delete iFocusedContact;
    iFocusedContact = NULL;
    delete iTopContact;
    iTopContact = NULL;
    delete iArray;
    iArray = NULL;
    iFlags = 0;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::SetFlags
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::SetFlags( TUint aFlags )
    {
    iFlags = aFlags;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::PackL
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPbk2ViewState::PackL() const
    {
    HBufC8* buf = PackLC();
    CleanupStack::Pop(); // buf
    return buf;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::PackLC
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPbk2ViewState::PackLC() const
    {
    // Calculate needed buffer size using TCalcLengthStreamBuf
    TCalcLengthStreamBuf countBuf;     
    RWriteStream countStream( &countBuf );
    // This can't leave because we are using TCalcLengthStreamBuf
    ExternalizeL( countStream );

    // Allocate a buffer and a stream
    HBufC8* buf = HBufC8::NewLC( countBuf.BytesWritten() );
    TPtr8 bufPtr = buf->Des();
    RDesWriteStream stream( bufPtr );
    stream.PushL();

    // Write this object to buf through stream
    ExternalizeL( stream );

    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream
    return buf;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::UnpackL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewState::UnpackL( const TDesC8& aPack )
    {
    RDesReadStream stream( aPack );
    stream.PushL();
    InternalizeL( stream );
    CleanupStack::PopAndDestroy(); // stream
    }


// --------------------------------------------------------------------------
// CPbk2ViewState::operator==
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ViewState::operator==( const CPbk2ViewState& aRhs ) const
    {
    return iFocusedFieldIndex == aRhs.iFocusedFieldIndex &&
           iTopFieldIndex == aRhs.iTopFieldIndex &&
           iFocusedPropertiesIndex == aRhs.iFocusedPropertiesIndex &&
           iTopPropertiesIndex == aRhs.iTopPropertiesIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::ExternalizeL
// --------------------------------------------------------------------------
//
void CPbk2ViewState::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint8L( KPbk2ViewStateVersionNumber );

    if (iFocusedContact)
        {
        aStream.WriteInt8L(EFocusedContact);
        HBufC8* buf = iFocusedContact->PackLC();
        aStream.WriteUint16L(buf->Length());
        aStream.WriteL(*buf);
        CleanupStack::PopAndDestroy(); // buf
        }

    if (iTopContact)
        {
        aStream.WriteInt8L(ETopContact);
        HBufC8* buf = iTopContact->PackLC();
        aStream.WriteUint16L(buf->Length());
        aStream.WriteL(*buf);
        CleanupStack::PopAndDestroy(); // buf
        }

    if (iParentContact)
        {
        aStream.WriteInt8L(EParentContact);
        HBufC8* buf = iParentContact->PackLC();
        aStream.WriteUint16L(buf->Length());
        aStream.WriteL(*buf);
        CleanupStack::PopAndDestroy(); // buf
        }

    if (iArray)
        {
        aStream.WriteInt8L(EMarkedContacts);
        HBufC8* buf = iArray->PackLC();
        aStream.WriteUint16L(buf->Length());
        aStream.WriteL(*buf);
        CleanupStack::PopAndDestroy(); // buf
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

    if (iFocusedPropertiesIndex >= 0)
        {
        aStream.WriteInt8L(EFocusedPropertiesIndex);
        aStream.WriteInt32L(iFocusedPropertiesIndex);
        }

    if (iTopPropertiesIndex >= 0)
        {
        aStream.WriteInt8L(ETopPropertiesIndex);
        aStream.WriteInt32L(iTopPropertiesIndex);
        }

    if (iFlags != ENullFlags)
        {
        aStream.WriteInt8L(EFlags);
        aStream.WriteUint32L(iFlags);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewState::InternalizeL
// --------------------------------------------------------------------------
//
void CPbk2ViewState::InternalizeL( RReadStream& aStream )
    {
    Reset();

    const TUint versionNumber = aStream.ReadUint8L();
    __ASSERT_ALWAYS(versionNumber == KPbk2ViewStateVersionNumber,
                    Panic(EPanicInvalidVersion));

    CVPbkContactManager* contactManager =
        &Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager();

    TBool eof = EFalse;
    while (!eof)
        {
        TInt type = EEnd;
        TRAPD(err, type = aStream.ReadInt8L());
        switch (err)
            {
            case KErrEof:
                {
                // End of stream, exit loop like EEnd opcode was read
                type = EEnd;
                break;
                }
            case KErrNone:
                {
                break;
                }
            default:
                {
                // Some other error than EOF occured
                User::Leave(err);
                break;
                }
            }

        switch (type)
            {
            case EFocusedContact:
                {
                TInt length = aStream.ReadUint16L();
                HBufC8* buf = HBufC8::NewLC(length);
                TPtr8 bufPtr = buf->Des();
                aStream.ReadL(bufPtr, length);
                MVPbkContactLinkArray* links =
                    contactManager->CreateLinksLC( *buf );
                if ( links && links->Count() > 0 )
                    {
                    iFocusedContact = links->At(0).CloneLC();
                    CleanupStack::Pop();
                    }
                CleanupStack::PopAndDestroy(2); // links, buf
                break;
                }

            case ETopContact:
                {
                TInt length = aStream.ReadUint16L();
                HBufC8* buf = HBufC8::NewLC(length);
                TPtr8 bufPtr = buf->Des();
                aStream.ReadL(bufPtr, length);
                MVPbkContactLinkArray* links =
                    contactManager->CreateLinksLC( *buf );
                if ( links && links->Count() > 0 )
                    {
                    iTopContact = links->At(0).CloneLC();
                    CleanupStack::Pop();
                    }
                CleanupStack::PopAndDestroy(2); // links, buf
                break;
                }

            case EParentContact:
                {
                TInt length = aStream.ReadUint16L();
                HBufC8* buf = HBufC8::NewLC(length);
                TPtr8 bufPtr = buf->Des();
                aStream.ReadL(bufPtr, length);
                MVPbkContactLinkArray* links =
                    contactManager->CreateLinksLC( *buf );
                if ( links && links->Count() > 0 )
                    {
                    iParentContact = links->At(0).CloneLC();
                    CleanupStack::Pop();
                    }
                CleanupStack::PopAndDestroy(2); // links, buf
                break;
                }

            case EMarkedContacts:
                {
                TInt length = aStream.ReadUint16L();
                HBufC8* buf = HBufC8::NewLC(length);
                TPtr8 bufPtr = buf->Des();
                aStream.ReadL(bufPtr, length);
                iArray = contactManager->CreateLinksLC( *buf );
                CleanupStack::Pop();
                CleanupStack::PopAndDestroy(); // buf
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
            
            case EFocusedPropertiesIndex:
                {
                iFocusedPropertiesIndex = aStream.ReadInt32L();
                break;
                }

            case ETopPropertiesIndex:
                {
                iTopPropertiesIndex = aStream.ReadInt32L();
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

//  End of File
