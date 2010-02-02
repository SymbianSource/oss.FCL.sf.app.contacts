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
*     Defines abstract interface for getting Logs state
*
*/


#ifndef __MLogsStateHolder_H_
#define __MLogsStateHolder_H_

// INCLUDES
#include "LogsEng.hrh"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *	Defines abstract interface for getting Logs state
 */
class MLogsStateHolder
    {
    public:        
        
        /**
         *   State of this object to tell to the observer
         *
         *   @return state
         */
        virtual TLogsState State() const = 0;
    };

            
#endif

// End of File  __MLogsStateHolder_H_
