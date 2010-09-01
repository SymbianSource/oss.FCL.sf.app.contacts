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
* Description:  Phonebook 2 localixed resource file.
*
*/


#ifndef RPBK2LOCALIZEDRESOURCEFILE_H
#define RPBK2LOCALIZEDRESOURCEFILE_H

#include <barsc.h>

class CCoeEnv;

/**
 * Phonebook 2 localixed resource file.
 */
class RPbk2LocalizedResourceFile : public RResourceFile
    {
    public: // Construction and destruction

        /**
         * Constructor. Using this constructor the resource file
         * is added to the CCoeEnv resource files when opened.
         * Use this from code that always run in process that
         * has a valid environment (CCoeEnv::Static).
         *
         * @param aCoeEnv   Reference to Coe environment.
         */
        IMPORT_C RPbk2LocalizedResourceFile(
                CCoeEnv& aCoeEnv );

        /**
         * Constructor. Opens an own file system session
         * for reading the resource file. Prefer using
         * the constructors take the session as input.
         *
         * @param aFs an open file system session
         */
        IMPORT_C RPbk2LocalizedResourceFile( RFs* aFs = NULL );

    public: // Interface

        /**
         * Finds and opens the localized version of the given resource file
         * and leaves the file to the cleanup stack for closing
         * by using CleanupStack::PopAndDestroy.
         *
         * @param aDrive        Drive letter for the resource file,
         *                      must also include the colon.
         *                      For example "z:".
         * @param aPath         Path to the file name.
         * @param aFileName     Resource file name.
         * @see BaflUtils::NearestLanguageFile
         */
        IMPORT_C void OpenLC(
                const TDesC& aDrive,
                const TDesC& aPath,
                const TDesC& aFileName );

        /**
         * Finds and opens the localized version of the given resource file.
         *
         * @param aDrive        Drive letter for the resource file, must also
                                include the colon. For example "z:".
         * @param aPath         Path to the file name.
         * @param aFileName     Resource file name.
         * @see BaflUtils::NearestLanguageFile
         */
        IMPORT_C void OpenL(
                const TDesC& aDrive,
                const TDesC& aPath,
                const TDesC& aFileName );

        /**
         * Finds and opens the localized version of the given resource file.
         *
         * @param aFullPath     Full path to the resource file.
         * @see BaflUtils::NearestLanguageFile
         */
        IMPORT_C void OpenL(
                const TDesC& aFullPath );

        /**
         * Closes the resource file.
         */
        IMPORT_C void Close();

        /**
         * Returns the offset for this resource file.
         *
         * @return  Offset for this resource file.
         */
        IMPORT_C TInt Offset();

    private:    // implementation
        void EnsureOpenFileSystemSessionL();
        
    private: // Data
        /// Ref: An open file server session
        RFs* iFs;
        /// Own: a file system session in case the client didn't gave one.
        RFs iOwnFs;
        /// Ref: Control environment
        CCoeEnv* iCoeEnv;
        /// Own: Offset
        TInt iOffset;
    };

#endif // RPBK2LOCALIZEDRESOURCEFILE_H

// End of File
