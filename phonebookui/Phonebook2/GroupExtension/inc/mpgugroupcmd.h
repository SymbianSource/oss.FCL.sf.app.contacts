/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Group Extn command object interface.
*
*/


#ifndef MPGUGROUPCMD_H
#define MPGUGROUPCMD_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Group extention command object interface. 
 * 
 * Pbk2 Group Extention cmd's are inherited from this interface
 * along with MPbk2Command  
 */
class MPguGroupCmd
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPguGroupCmd()
                {};

        /**
         * Aborts the Command
         */
        virtual void Abort() = 0;
        
    };

#endif // MPGUGROUPCMD_H

// End of File
