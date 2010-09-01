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
*     Defines abstract interface getting log event.
*
*/


#ifndef __MLogsGetEvent_H_
#define __MLogsGetEvent_H_

//  INCLUDES
#include <e32base.h>
#include <logwrap.h>
#include "LogsEng.hrh"
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MLogsObserver;
class MLogsEventGetter;

/**
 *	Defines abstract interface getting log event
 */
class MLogsGetEvent
    {
    public:
    
        virtual ~MLogsGetEvent() {};

    public:

       /**
        *   Starts async get operation
        *
        *   @param aId log event id
        */
        virtual void Get( TLogId aId ) = 0;
    
       /**
        *   Event get method.
        *
        *   @return MLogsEventGetter object
        */
        virtual const MLogsEventGetter* Event() = 0;

       /**
        *   Log event get method.
        *
        *   @return CLogEvent object
        */
        virtual const CLogEvent* LogEvent() const = 0;

       /**
        *   Set UI observer for notifications
        * 
        *   @param aObserver pointer to observer.
        */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0;                    
    };

            
#endif

// End of File  __MLogsGetEvent_H_
