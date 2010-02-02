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


#include <RVPbkContactFieldDefaultPriorities.h>
#include <VPbkError.h>
#include <s32strm.h>
#include <s32mem.h>

// Priority array static data
namespace KPriorityArrayStatic
    {
    const TUint8 KArrayVersionNumber = 1;
    }

namespace KPriorityArrayExternalizeSizes
    {
    const TInt KVersionNumberSize = 1;
    const TInt KElementCountSize = 1;
    const TInt KElementSize = 1;
    }

namespace {

TInt DoCalculateBufferSize(const RVPbkContactFieldDefaultPriorities& aPriorities)
    {
    TInt bufferSize = KPriorityArrayExternalizeSizes::KVersionNumberSize;
    bufferSize += KPriorityArrayExternalizeSizes::KElementCountSize;
    bufferSize += aPriorities.Count() * KPriorityArrayExternalizeSizes::KElementSize;
    return bufferSize;
    }

void DoFillBuffer(RWriteStream& aWriteStream,
        const RVPbkContactFieldDefaultPriorities& aPriorities)
    {
    // Write version number
    aWriteStream.WriteUint8L(KPriorityArrayStatic::KArrayVersionNumber);
    // Write array element count to the stream
    aWriteStream.WriteUint8L(aPriorities.Count());

    // Write each element to the stream
    for (TInt i = 0; i < aPriorities.Count(); ++i)
        {
        aWriteStream.WriteUint8L(aPriorities.At(i));
        }
    }    

} /// unnamed namespace


/**
 * Default priority array pack format 1:
 * stream := VersionNumber ElementCount Element*
 * VersionNumber := TInt8
 * ElementCount := TInt8
 * Element := TInt8
 */
EXPORT_C HBufC8* RVPbkContactFieldDefaultPriorities::ExternalizeLC() const
    {
    const TInt bufferSize = DoCalculateBufferSize(*this);
    HBufC8* buffer = HBufC8::NewLC(bufferSize);
    TPtr8 bufPtr(buffer->Des());
    RDesWriteStream writeStream(bufPtr);
    writeStream.PushL();
    DoFillBuffer(writeStream, *this);
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);
    return buffer;
    }

EXPORT_C void RVPbkContactFieldDefaultPriorities::InternalizeL
        (const TDesC8& aBuffer)
    {
    iDefaultTypes.Reset();
    
    RDesReadStream readStream(aBuffer);
    readStream.PushL();

    // Version number
    const TInt vsn = readStream.ReadUint8L();
    __ASSERT_ALWAYS(vsn == KPriorityArrayStatic::KArrayVersionNumber,
            VPbkError::Panic(VPbkError::EInvalidTypeParameter));

    // Read count of elements
    TInt count = readStream.ReadUint8L();

    // Read elements
    for (TInt i = 0; i < count; ++i)
        {
        TVPbkDefaultType type = (TVPbkDefaultType) readStream.ReadUint8L();
        User::LeaveIfError(iDefaultTypes.Append(type));
        }

    CleanupStack::PopAndDestroy(&readStream);
    }

// End of File
