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
* Description:  An API for commands
*
*/



#ifndef MVPBKSIMCOMMAND_H
#define MVPBKSIMCOMMAND_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCommandObserver;

// CLASS DECLARATION

/**
*  An API for commands
*
*/
class MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MVPbkSimCommand() {}

    public: // New functions
        
        /**
        * Executes the command
        */
        virtual void Execute() = 0;

        /**
        * Adds an observer to the command.
        * @param aObserver a new observer to the command
        */
        virtual void AddObserverL( MVPbkSimCommandObserver& aObserver ) = 0;

        /**
        * Cancels the command
        */
        virtual void CancelCmd() = 0;
    };
#endif      // MVPBKSIMCOMMAND_H
            
// End of File
