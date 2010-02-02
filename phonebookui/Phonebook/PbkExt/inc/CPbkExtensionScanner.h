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
*       Scanner that finds extensions.
*
*/


#ifndef __CPbkExtensionScanner_H__
#define __CPbkExtensionScanner_H__

//  INCLUDES
#include <f32file.h>

// FORWARD DECLARATIONS
class CPbkExtensionLoader;

// TYPEDEFS
typedef CArrayPtrFlat<CPbkExtensionLoader> CPbkExtensionArray;

// CLASS DECLARATION

/**
 * Scanns for ECom extensions.
 */
class CPbkExtensionScanner : public CBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         */
        static CPbkExtensionScanner* NewLC(CPbkExtensionArray& aExtensions);

        /**
         * Destructor.
         */
        ~CPbkExtensionScanner();

    public: // Interface
        /**
         * Scans a directory and appends found extensions to iExtensions.
         */
        void ScanL();

    private: // Implementation
        CPbkExtensionScanner(CPbkExtensionArray& aExtensions);

    private: // Data
        /// Ref: Array of extensions that this scanner builds.
        CPbkExtensionArray& iExtensions;
    };

#endif // __CPbkExtensionScanner_H__

// End of File
