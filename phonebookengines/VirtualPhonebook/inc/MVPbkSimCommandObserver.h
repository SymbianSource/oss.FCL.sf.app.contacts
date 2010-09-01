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
* Description:  A command observer API
*
*/



#ifndef MVPBKSIMCOMMANDOBSERVER_H
#define MVPBKSIMCOMMANDOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCommand;

// CLASS DECLARATION

/**
*  A command observer API
*
*/
class MVPbkSimCommandObserver
    {
    public: // New functions

        /**
        * Called after command executed successfully
        * @param aCommand the command that completed
        */
        virtual void CommandDone( MVPbkSimCommand& aCommand ) = 0;

        /**
        * Called if command execution failed
        * @param aCommand the command that completed
        * @param the error code from the failed command
        */
        virtual void CommandError( MVPbkSimCommand& aCommand, TInt aError ) = 0;

    protected:  // Destructor
        
        /**
        * Destructor.
        */
        virtual ~MVPbkSimCommandObserver() {}
    };

#endif      // MVPBKSIMCOMMANDOBSERVER_H
            
// End of File
