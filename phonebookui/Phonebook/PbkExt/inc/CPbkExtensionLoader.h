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
*       Loads the extension dll and provides access to extension factory.
*
*/


#ifndef __CPbkExtensionLoader_H__
#define __CPbkExtensionLoader_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLRATIONS
class MPbkExtensionFactory;

// CLASS DECLARATION

/**
 * Loads the extension dll and provides access to extension factory
 * implemented in the DLL.
 */
class CPbkExtensionLoader : public CBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         * Loads the ECom extension dll that has UID aUid.
         */
        static CPbkExtensionLoader* NewL(TUid aUid);

        /**
         * Destructor.
         */
        ~CPbkExtensionLoader();

    public: // Interface
        /**
         * Returns the loaded extension factory.
		 * May be NULL if some kind of error happened when loading.
         */
        MPbkExtensionFactory* ExtensionFactory();

    private: // Implementation
        CPbkExtensionLoader();
        void LoadExtensionDllL(TUid aUid);

    private: // Data
        /// Own: loaded extension factory.
        MPbkExtensionFactory* iFactory;
    };

#endif // __CPbkExtensionLoader_H__

// End of File
