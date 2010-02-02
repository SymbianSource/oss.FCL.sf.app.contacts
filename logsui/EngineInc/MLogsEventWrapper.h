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
*     Defines abstract interface for Logs event wrapper.
*
*/


#ifndef __MLogsEventWrapper_H_
#define __MLogsEventWrapper_H_

// INCLUDES
#include "MLogsEventGetter.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MLogsEvent;

/**
 *	Defines abstract interface for Logs event wrapper
 */
class MLogsEventWrapper : public MLogsEventGetter
    {
    public:
    
        virtual ~MLogsEventWrapper() {};

    public:
            
       /**
        *   wrapper
        *
        *   @return aEvent interface pointer to Logs event
        */
        virtual const MLogsEventGetter* Wrap( MLogsEvent* aEvent ) = 0;
    };

            
#endif

// End of File  __MLogsEventWrapper_H_
