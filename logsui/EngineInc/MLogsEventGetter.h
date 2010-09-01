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
*     Defines abstract interface for Logs event getter.
*
*/


#ifndef __MLogsEventGetter_H_
#define __MLogsEventGetter_H_

// INCLUDES
#include <e32std.h>
#include <logwrap.h>
#include <cntdef.h>
#include "LogsEng.hrh"


#include "MLogsEventData.h"
#include "MLogsEvent.h"


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Defines abstract interface for Logs event getter
 */
class MLogsEventGetter
    {
    public:
    
        virtual ~MLogsEventGetter() {};

    public:
    
       /**
        *   Event getter
        *
        *   @return interface to MLogsEvent
        */
        virtual MLogsEvent* Event() const = 0;
            
       /**
        *   Duplicates getter
        *
        *   @return number of duplicates
        */
        virtual TInt8 Duplicates() const = 0;

       /**
        *   Event type getter
        *
        *   @return event type
        */
        virtual TLogsEventType EventType() const = 0;

       /**
        *   Uid getter
        *
        *   @return uid
        */
        virtual TUid EventUid() const = 0;

       /**
        *   Direction getter
        *
        *   @return direction
        */
        virtual TLogsDirection Direction() const = 0;

       /**
        *   Time getter
        *
        *   @return time
        */
        virtual TTime Time() const = 0;

       /**
        *   Log Id getter
        *
        *   @return log id
        */
        virtual TLogId LogId() const = 0;

       /**
        *   Number getter
        *
        *   @return number
        */
        virtual HBufC* Number() const = 0;

       /**
        *   Remote party getter
        *
        *   @return remote party
        */
        virtual HBufC* RemoteParty() const = 0;

       /**
        *   Additional LogsData object.
        *
        *   @return  MLogsEventData object.
        */
        virtual MLogsEventData* LogsEventData() const = 0;

       /**
        *   ALS flag (Alternate line service).
        *
        *   @return  ALS status flag. EFalse if not ALS event, ETrue if ALS event.
        */
        virtual TBool ALS() const = 0;

        /**
        *   CNAP flag (Calling name presentation service).
        *
        *   @return  CNAP status flag. EFalse if not CNAP event, ETrue if CNAP event.
        */
        virtual TBool CNAP() const = 0;

//For ring duation feature
        /**
        *   Return Ring Duration
        *
        *   @return  Ring Duration
        */
        virtual TLogDuration RingDuration() const = 0;
        
        /**
        * Return Phone number field type
        *
        * @param Phone number field type
        */       
        virtual TInt NumberFieldType() const = 0;
 


    };

            
#endif

// End of File  __MLogsEventGetter_H_
