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
*       Loads a polymorphic extension dll and provides access to 
*       extension factory.
*
*/


#ifndef CLogsExtensionLoader_H
#define CLogsExtensionLoader_H


//  INCLUDES
#include <e32base.h>

// FORWARD DECLRATIONS
class MLogsExtensionFactory;

// CLASS DECLARATION

/**
 * Loads a polymorphic extension dll and provides access to 
 * extension factory.
 */
class CLogsExtensionLoader : public CBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new CLogsExtensionLoader.
         */
        static CLogsExtensionLoader* NewL();

        /**
         * Destructor.
         */
        ~CLogsExtensionLoader();

    public: // Interface
        /**
         * Returns extension factory. Ownership is changed for the
         * caller. Second call will return NULL.
         *
         * @return extension factory
         */
        MLogsExtensionFactory& ExtensionFactory();

    private:
        CLogsExtensionLoader();
        void ConstructL();
        TUidType UidType();
        void LoadExtensionDllL();

    private: // Data
        /// Own: the test dll.
        RLibrary iDll;
        /// Ref/Own: loaded extension factory
        MLogsExtensionFactory* iFactory;

    };

#endif
            

// End of File
