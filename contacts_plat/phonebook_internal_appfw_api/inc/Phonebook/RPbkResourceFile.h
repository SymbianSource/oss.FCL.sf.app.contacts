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
*       Resource file loader for loading localized versions of resource files.
*
*/


#ifndef __RPbkResourceFile_H__
#define __RPbkResourceFile_H__

// INCLUDES
#include <e32std.h>
#include <ConeResLoader.h>

/**
 * Helper class for loading Phonebook MMC UI resources.
 */
class RPbkResourceFile
	{
	public: // Construction and destruction
        /**
         * Standard C++ constructor.
         * @param aEnv CoeEnv that will be used when opening the resource file.
         */
        IMPORT_C RPbkResourceFile(CCoeEnv& aEnv);

        /**
         * Standard C++ destructor.
         */
        IMPORT_C ~RPbkResourceFile();

    public: // interface
        /**
         * Opens the MMC UI dll resource file for use. If already opened
         * does not do anything.
         *
         * @param aFileName File name of the resources.
         */
        IMPORT_C void OpenL(const TDesC& aFileName);

        /**
         * Closes the MMC UI dll resource file.
         * You must always remember to close the resource file when finished
         * using it.
         */
        IMPORT_C void Close();
        
    private:  // Data
        /// Own: Resource loader
        RConeResourceLoader iLoader;
        /// Is the resource file opened.
        TBool iOpen;

	};

#endif // __RPbkResourceFile_H__

// End of File
