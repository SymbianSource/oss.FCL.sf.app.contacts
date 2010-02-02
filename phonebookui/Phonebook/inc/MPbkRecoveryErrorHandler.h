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
*       Map of UIDs.
*
*/


#ifndef __MPbkRecoveryErrorHandler_H__
#define __MPbkRecoveryErrorHandler_H__

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
 * Contact database recovery error handler.
 */
class MPbkRecoveryErrorHandler
    {
    public:  // New functions
        /**
         * Handles a DB recovery error. 
         *
         * @param aError    recovery error code.
         * @return ETrue if recovery should be tried again, EFalse otherwise.
         * @precond aError != KErrNone (caller guarantees, implementation may 
         *          assume).
         */
        virtual TBool HandleDbRecoveryError(TInt aError) =0;
    };

#endif // __MPbkRecoveryErrorHandler_H__

// End of File
