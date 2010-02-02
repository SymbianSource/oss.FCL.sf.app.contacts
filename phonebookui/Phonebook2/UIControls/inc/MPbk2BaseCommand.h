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
* Description:  Phonebook 2 generic command.
* 				 Basic command interface not coupled to UiControl,
* 				 used only in ContactEditor ATM
*
*/


#ifndef MPBK2BASECOMMAND_H
#define MPBK2BASECOMMAND_H

// CLASS DECLARATION

/**
 * Phonebook 2 basic command.
 */
class MPbk2BaseCommand
    {
    public: // Interface

        /**
         * Cancel and destroy command.
         */
        virtual ~MPbk2BaseCommand() {}
    };

#endif // MPBK2BASECOMMAND_H

// End of File
