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
*     Flash File System (FFS) check helper class.
*
*/


#ifndef __CPbkFFSCheck_H__
#define __CPbkFFSCheck_H__

//  INCLUDES
#include <e32base.h>    // CBase

// FORWARD DECLARATIONS
class CCoeEnv;
class MPbkCommandHandler;


// CLASS DECLARATION

/**
 * Flash File System (FFS) check helper class.
 */
class CPbkFFSCheck : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aCoeEnv CONE environment
         */
        IMPORT_C static CPbkFFSCheck* NewL(CCoeEnv* aCoeEnv=NULL);
        
        /**
         * Destructor.
         */
        ~CPbkFFSCheck();

    public: // New functions
        /**
         * Checks critical FFS level.
         * @param aBytesToWrite     how many bytes will be written to FFS. 
         *                          Default is unknown (=0).
         * @return ETrue if FFS free space will not go below CL.
         */
        IMPORT_C TBool FFSClCheckL(TInt aBytesToWrite=0);

        /**
         * Checks critical FFS level.
         *
         * @param aCommandHandler   command handler wrapper. If FFS has more 
         *                          free space than CL the command is executed.
         * @param aBytesToWrite     how many bytes will be written to FFS. 
         *                          Default is unknown (=0).
         */
        IMPORT_C void FFSClCheckL
            (const MPbkCommandHandler& aCommandHandler, 
            TInt aBytesToWrite=0);

        /**
         * Checks critical MMC level.
         * @param aBytesToWrite     how many bytes will be written to FFS. 
         *                          Default is unknown (=0).
         * @return ETrue if FFS free space will not go below CL.
         */
        IMPORT_C TBool MMCClCheckL(TInt aBytesToWrite=0);

        /**
         * Checks critical MMC level
         *
         * @param aCommandHandler   command handler wrapper. If FFS has more 
         *                          free space than CL the command is executed.
         * @param aBytesToWrite     how many bytes will be written to FFS. 
         *                          Default is unknown (=0).         
         */
        IMPORT_C void MMCClCheckL
            (const MPbkCommandHandler& aCommandHandler, 
            TInt aBytesToWrite=0);

        #ifdef _DEBUG
        /// (Debug only) Makes next check fail
        inline void SetDebugFailNext() { iDebugFailNext=ETrue; }
        #endif // _DEBUG

    private:  // Implementation
        CPbkFFSCheck(CCoeEnv* aCoeEnv);
        void ConstructL();
        TBool DebugFailNext();
        
    private:    // Data
        /// Ref: CONE environment
        CCoeEnv* iConeEnv;
        #ifdef _DEBUG
        /// Own: if ETrue fails next check (debug only) 
        TBool iDebugFailNext;
        #endif // _DEBUG
    };

#endif // __CPbkFFSCheck_H__
            
// End of File
