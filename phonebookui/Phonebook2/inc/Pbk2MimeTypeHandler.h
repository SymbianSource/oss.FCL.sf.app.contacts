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
* Description:  Phonebook 2 MIME type handler.
*
*/


#ifndef PBK2MIMETYPEHANDLER_H
#define PBK2MIMETYPEHANDLER_H

//  INCLUDES
#include <e32std.h>

//  CLASS DECLARATION

/**
 * Phonebook 2 MIME type mapper.
 * Responsible for mapping MIME type string into a MIME type enumeration.
 */
class Pbk2MimeTypeHandler
    {
    public: // Data types

        enum TPbk2SupportedMIMETypes
            {
            EMimeTypeNotSupported = 0,
            EMimeTypeImage,
            EMimeTypeAudio,
            EMimeTypeVideo
            };

    public: // Interface

        /**
         * Maps MIME type in given string to one of the enumerations
         * defined in TPbk2SupportedMIMETypes.
         *
         * @param aMimeTypeString   8-bit descriptor containing a MIME type.
         * @return  Phonebook 2 MIME type enumeration.
         */
        IMPORT_C static TInt MapMimeTypeL(
                const TDesC8& aMimeTypeString );

        /**
         * Maps MIME type in given string to one of the enumerations
         * defined in TPbk2SupportedMIMETypes.
         *
         * @param aMimeTypeString   Descriptor containing a MIME type.
         * @return  Phonebook 2 MIME type enumeration.
         */
        IMPORT_C static TInt MapMimeTypeL(
                const TDesC& aMimeTypeString );
    };

#endif // PBK2MIMETYPEHANDLER_H

// End of File
