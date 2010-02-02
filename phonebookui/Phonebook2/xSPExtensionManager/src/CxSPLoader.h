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
*       Loads the extension DLL and provides access to extension factory.
*
*/


#ifndef __CXSPLOADER_H__
#define __CXSPLOADER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLRATIONS
class MxSPFactory;

// CLASS DECLARATION

/**
 * Loads the extension DLL and provides access to extension factory
 * implemented in the DLL.
 */
class CxSPLoader : public CBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         * Loads the ECom extension DLL that has UID aUid.
         *
         * @param aUid UID of the DLL to be loaded
         * @return New instance of this class
         */
        static CxSPLoader* NewL( TUid aUid );

        /**
         * Destructor.
         */
        ~CxSPLoader();

    public: // Interface
        /**
         * Returns the loaded extension factory.
		 * May be NULL if some kind of error happened when loading.
		 *
		 * @return The loaded DLL's extension factory
         */
        MxSPFactory* ExtensionFactory() const;

    private: // Implementation

    	/**
    	 * Default constructor
    	 */
        CxSPLoader();

        /**
         * Loads the extension DLL with given Uid
         *
         * @param aUid The Uid of extension to be loaded
         */
        void LoadExtensionDllL( TUid aUid );

    private: // Data
        /// Own: loaded extension factory.
        MxSPFactory* iFactory;
    };

#endif // __CXSPLOADER_H__

// End of File
