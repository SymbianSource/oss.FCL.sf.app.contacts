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
* Description:  Implements Phonebook 2 generic IPC package.
*
*/


// Phonebook 2
#include "Pbk2IPCPackage.h"

// System includes
#include <barsread.h>
#include <s32mem.h>

// --------------------------------------------------------------------------
// Package syntax
// 1. Bit enumeration (EBuffer8Bit or EBuffer16Bit)
// 2. Length of the buffer
// 3. Data
// --------------------------------------------------------------------------

/// Unnamed namespace for local definitions
namespace {

const TInt KFormatSlotSize( 4 + 4 );
const TInt KConvert16To8 = 2;

enum TPanicCode
    {
    EInvalidArgument
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "Pbk2IPCPackage" );
    User::Panic( KPanicText, aReason );
    }

} /// namespace

// --------------------------------------------------------------------------
// Pbk2IPCPackage::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::ExternalizeL
        ( HBufC8*& aBuffer, RDesWriteStream& aWriteStream )
    {
    if ( aBuffer )
        {
        aWriteStream.WriteUint32L( EBuffer8Bit );
        aWriteStream.WriteUint32L( aBuffer->Length() );
        aWriteStream.WriteL( *aBuffer );
        }
    else
        {
        aWriteStream.WriteUint32L( EBuffer8Bit );
        aWriteStream.WriteUint32L( 0 );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::ExternalizeL
        ( HBufC*& aBuffer, RDesWriteStream& aWriteStream )
    {
    if ( aBuffer )
        {
        aWriteStream.WriteUint32L( EBuffer16Bit );
        aWriteStream.WriteUint32L( aBuffer->Length() );
        aWriteStream.WriteL( *aBuffer );
        }
    else
        {
        aWriteStream.WriteUint32L( EBuffer16Bit );
        aWriteStream.WriteUint32L( 0 );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::InternalizeL
        ( HBufC8*& aBuffer, RDesReadStream& aReadStream )
    {
    delete aBuffer;
    aBuffer = NULL;

    // Need to read, even we don't needed
    TInt bit( aReadStream.ReadUint32L() );
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC8::NewL( length );
        TPtr8 ptr = aBuffer->Des();
        aReadStream.ReadL( ptr, length );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::InternalizeL
        ( HBufC*& aBuffer, RDesReadStream& aReadStream )
    {
    delete aBuffer;
    aBuffer = NULL;

    // Need to read, even we don't needed
    TInt bit( aReadStream.ReadUint32L() );
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC::NewL( length );
        TPtr ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::InternalizeL
        ( HBufC8*& aBuffer, RDesReadStream& aReadStream,
          const TInt aPosition )
    {
    delete aBuffer;
    aBuffer = NULL;

    // Read the non relevant parts of the package
    FindPositionL( aReadStream, aPosition );

    // Need to read, even we don't needed
    TInt bit( aReadStream.ReadUint32L() );
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC8::NewL( length );
        TPtr8 ptr = aBuffer->Des();
        aReadStream.ReadL( ptr, length );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2IPCPackage::InternalizeL
        ( HBufC*& aBuffer, RDesReadStream& aReadStream,
          const TInt aPosition )
    {
    delete aBuffer;
    aBuffer = NULL;

    // Read the non relevant parts of the package
    FindPositionL( aReadStream, aPosition );

    // Need to read, even we don't needed
    TInt bit( aReadStream.ReadUint32L() );
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC::NewL( length );
        TPtr ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::CountPackageSize
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2IPCPackage::CountPackageSize( HBufC8* aBuffer )
    {
    TInt ret = KFormatSlotSize;

    if ( aBuffer )
        {
        // See package syntax
        ret = aBuffer->Length() + KFormatSlotSize;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::CountPackageSize
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2IPCPackage::CountPackageSize( HBufC* aBuffer )
    {
    TInt ret = KFormatSlotSize;
    if ( aBuffer )
        {
        // See package syntax
        ret = aBuffer->Length()*KConvert16To8 + KFormatSlotSize;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2IPCPackage::FindPositionL
// Reads the non relevant parts of the package.
// @param aReadStream   The stream to read from.
// @param aPosition     The position where to read.
// --------------------------------------------------------------------------
//
void Pbk2IPCPackage::FindPositionL
        ( RDesReadStream& aReadStream, const TInt aPosition )
    {
    for ( TInt i=0; i < aPosition; ++i )
        {
        TInt bit = aReadStream.ReadUint32L();
        switch ( bit )
            {
            case EBuffer8Bit:
                {
                TInt dummyLength = aReadStream.ReadUint32L();
                aReadStream.ReadL(dummyLength);
                break;
                }
            case EBuffer16Bit:
                {
                TInt dummyLength = aReadStream.ReadUint32L()*KConvert16To8;
                aReadStream.ReadL(dummyLength);
                break;
                }
            default:
                {
                Panic( EInvalidArgument );
                }
            };
        }
    }

// End of File
