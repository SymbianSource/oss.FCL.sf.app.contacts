/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Flash File System (FFS) check helper class.
*
*/


#ifndef __CPBK2FFSCHECK_H__
#define __CPBK2FFSCHECK_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <f32file.h>

#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class CCoeEnv;

// CLASS DECLARATION

/**
 * Flash File System (FFS) check helper class.
 */
class CPbk2DriveSpaceCheck : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aFs open file server session, if not given 
		 * it will be created.
         */
        IMPORT_C static CPbk2DriveSpaceCheck* NewL( RFs& aFs );
        
        /**
         * Destructor.
         */
        ~CPbk2DriveSpaceCheck();

    public: // New functions
        /**
         * Checks critical drive space level. If SetStore is not called before 
         * using this it checks the FFS critical level. If SetStore is called
         * and store is not located to any drive, this will always pass.
         * If critical level is reached, this will leave KErrDiskFull.
         */
        IMPORT_C void DriveSpaceCheckL();

        /**
         * Sets current drive to be checked, if given store is in any drive
         * @param aUriPtr Store Uri that is checked is it in any drive
         */
        IMPORT_C void SetStore( const TVPbkContactStoreUriPtr aUriPtr );
        
        /**
         * Sets current drive to be checked, if given store is in any drive
         * @param aFilePath A drive name or file path
         */
        IMPORT_C void SetStore( const TDesC& aFilePath );

    private:  // Implementation
        CPbk2DriveSpaceCheck( RFs& aFs );
        
    private:    // Data
        /// Not Owned: CONE environment
        RFs& iFs;
        /// Own: drive where store is located if set with SetStore
        //       see TDriveNumber of f32file.h
        TInt iDrive;        
        /// Own: Store is located to some drive
        TBool iStoreIsInDrive;
        };

#endif // __CPBK2FFSCHECK_H__
            
// End of File
