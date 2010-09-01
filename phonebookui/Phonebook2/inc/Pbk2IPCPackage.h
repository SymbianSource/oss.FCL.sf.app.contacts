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
* Description:  Phonebook 2 generic IPC package.
*
*/


#ifndef PBK2IPCPACKAGE_H
#define PBK2IPCPACKAGE_H

//  INCLUDES
#include <e32std.h>

//  FORWARD DECLARATIONS
class RDesWriteStream;
class RDesReadStream;

//  CLASS DECLARATION

/**
 * Phonebook 2 generic IPC package.
 * Utility class for IPC transfer of buffers.
 * Responsible for externalizing and internalizing buffers.
 * The buffer may consists of multiple subbuffers.
 * Before the buffer is written to the stream, it will be converted to
 * 8-bit format, if necessary.
 */
class Pbk2IPCPackage
    {
    public: //Interface

        /**
         * Writes buffer's data to a stream.
         *
         * @param aBuffer       The buffer to write.
         * @param aWriteStream  The stream to write to.
         */
        IMPORT_C static void ExternalizeL(
                HBufC8*& aBuffer,
                RDesWriteStream& aWriteStream );

        /**
         * Writes buffer's data to a stream.
         *
         * @param aBuffer       The buffer to write.
         * @param aWriteStream  The stream to write to.
         */
        IMPORT_C static void ExternalizeL(
                HBufC*& aBuffer,
                RDesWriteStream& aWriteStream );

        /**
         * Reads data from the stream.
         *
         * @param aBuffer       Buffer to write the data.
         * @param aReadStream   The stream to read from.
         */
        IMPORT_C static void InternalizeL(
                HBufC8*& aBuffer,
                RDesReadStream& aReadStream );

        /**
         * Reads data from the stream.
         *
         * @param aBuffer       Buffer to write the data.
         * @param aReadStream   The stream to read from.
         */
        IMPORT_C static void InternalizeL(
                HBufC*& aBuffer,
                RDesReadStream& aReadStream );

        /**
         * Reads data from the specific position of the stream.
         *
         * @param aBuffer       Buffer to write the data.
         * @param aReadStream   The stream to read from.
         * @param aPosition     The position where to read.
         */
        IMPORT_C static void InternalizeL(
                HBufC8*& aBuffer,
                RDesReadStream& aReadStream,
                const TInt aPosition );

        /**
         * Reads data from the specific position of the stream.
         *
         * @param aBuffer       Buffer to write the data.
         * @param aReadStream   The stream to read from.
         * @param aPosition     The position where to read.
         */
        IMPORT_C static void InternalizeL(
                HBufC*& aBuffer,
                RDesReadStream& aReadStream,
                const TInt aPosition );

        /**
         * Counts buffer's size in the package.
         *
         * @param aBuffer   Buffer to inspect.
         * @return  Size of the buffer.
         */
        IMPORT_C static TInt CountPackageSize(
                HBufC8* aBuffer );

        /**
         * Counts buffer's size in the package.
         *
         * @param aBuffer   Buffer to inspect.
         * @return  Size of the buffer.
         */
        IMPORT_C static TInt CountPackageSize(
                HBufC* aBuffer );

    private: // Implementation
        static void FindPositionL(
            RDesReadStream& aReadStream,
            const TInt aPosition );

    private: // Data types

        /// Buffer type
        enum TBuffer
            {
            EBuffer8Bit,    // 8-bit buffer
            EBuffer16Bit    // 16-bit buffer
            };
    };

#endif // PBK2IPCPACKAGE_H

// End of File
