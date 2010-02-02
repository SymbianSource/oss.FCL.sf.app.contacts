/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CXSPSCANNER_H__
#define __CXSPSCANNER_H__

//  INCLUDES
#include <s32file.h>

// FORWARD DECLARATIONS
class CxSPLoader;

// TYPEDEFS
typedef CArrayPtrFlat<CxSPLoader> CxSPArray;

// CLASS DECLARATION

/**
 * Scanns for ECom extensions.
 */
class CxSPScanner : public CBase
    {
    public: // Constructor and destructor

        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @param aExtensions Array of extensions
         * @return New instance of this class.
         */
        static CxSPScanner* NewL(CxSPArray& aExtensions);

        /**
         * Destructor.
         */
        ~CxSPScanner();

    public: // Interface
        /**
         * Scans a directory and appends found extensions to iExtensions.
         */
        void ScanL();

    private: // Implementation
        CxSPScanner(CxSPArray& aExtensions);

    private: // Data
        /// Ref: Array of extensions that this scanner builds.
        CxSPArray& iExtensions;
    };

#endif // __CXSPSCANNER_H__

// End of File
