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
* Description: 
*       Phonebook 2 command factory abstract API.
*
*/


#ifndef MPBK2COMMANDFACTORY_H
#define MPBK2COMMANDFACTORY_H

#include <Pbk2Commands.hrh>

//  FORWARD DECLARATIONS
class MPbk2Command;
class MPbk2ContactUiControl;

//  CLASS DECLARATION 

/**
 * Phonebook command factory API.
 */
class MPbk2CommandFactory
    {
    public:  // Interface
	    /**
		 * Virtual destructor.
		 */
		virtual ~MPbk2CommandFactory() { }

		/**
         * Creates a command given command ID.
         *
         * @param aCommandId ID of the command that the command object is 
         *                   created for.
         * @param aParams Phonebook 2 command object parameters
         * @return the created command object
         */
        virtual MPbk2Command* CreateCommandForIdL(
                TPbk2CommandId aCommandId,
                MPbk2ContactUiControl& aParams) const =0;
    };

#endif // MPBK2COMMANDFACTORY_H

// End of File
