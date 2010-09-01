/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An API for long sim store operations.
*
*/



#ifndef MVPBKSIMSTOREOPERATION_H
#define MVPBKSIMSTOREOPERATION_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  An API for long sim store operations
*
*/
class MVPbkSimStoreOperation
    {
    public:  // destructor
        
        /**
        * Destructor.
        * Cancels the operation
        */
        virtual ~MVPbkSimStoreOperation() {}
    };

#endif      // MVPBKSIMSTOREOPERATION_H
            
// End of File
