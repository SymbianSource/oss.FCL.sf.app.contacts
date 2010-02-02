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
*     Implements Virtual Phonebook generic IPC package.
*
*/



#ifndef VPBKENGUTILS_VPBKIPCPACKAGE_H
#define VPBKENGUTILS_VPBKIPCPACKAGE_H

//  INCLUDES
#include <e32std.h>

//  FORWARD DECLARATIONS
class RDesWriteStream;
class RDesReadStream;

namespace VPbkEngUtils {

//  CLASS DECLARATION
class VPbkIPCPackage
    {
    public: //Interface
        /**
         * Writes buffer's data to a stream.
         *
         * @param aBuffer       The buffer to write.
         * @param aWriteStream  The stream to write to.
         */
        IMPORT_C static void ExternalizeL(
                const TDesC8* aBuffer,
                RDesWriteStream& aWriteStream );
        
        /**
         * Writes buffer's data to a stream.
         *
         * @param aBuffer       The buffer to write.
         * @param aWriteStream  The stream to write to.
         */
        IMPORT_C static void ExternalizeL(
                const TDesC* aBuffer,
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
         * @param aBuffer, ownership not transfered
         * @return  Size of the buffer.
         */
        IMPORT_C static TInt CountPackageSize(
                const TDesC8* aBuffer );

        /**
         * Counts buffer's size in the package.
         *
         * @param aBuffer, ownership not transfered
         * @return  Size of the buffer.
         */
        IMPORT_C static TInt CountPackageSize(
                const TDesC* aBuffer );
    };

} /// namespace

#endif // VPBKENGUTILS_VPBKIPCPACKAGE_H

// End of File
