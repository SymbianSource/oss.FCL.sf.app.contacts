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
*     Defines abstract interface for Logs event
*
*/


#ifndef __MLogsEvent_H_
#define __MLogsEvent_H_

//  INCLUDES
#include <e32std.h>
#include <logwrap.h>
#include <cntdef.h>
#include "LogsEng.hrh"

#include "MLogsEventData.h"
#include "TLogsEventStrings.h"


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Defines abstract interface for Logs event
 */
class MLogsEvent
    {
    public:

        virtual ~MLogsEvent() {};

    public:

       /**
        *   Time setter
        *
        *   @param  aTime   time
        */
        virtual void SetTime( const TTime aTime ) = 0;

       /**
        *   Log Id setter
        *
        *   @param  aLogId   log event id
        */
        virtual void SetLogId( const TLogId aLogId ) = 0;

       /**
        *   Number setter
        *
        *   @param  aNumber   Phone number
        */
        virtual void SetNumber( HBufC* aNumber ) = 0;


       /**
        *   Remove party setter
        *
        *   @param  aRemoteParty   Remove party name
        */
        virtual void SetRemoteParty( HBufC* aRemoteParty ) = 0;

       /**
        *   Direction setter
        *
        *   @param  aDirection   direction
        */
        virtual void SetDirection( const TLogsDirection aDirection ) = 0;

       /**
        *   Uid setter
        *
        *   @param  aUid   Uid
        */
        virtual void SetEventUid( const TUid aUid ) = 0;

       /**
        *   Event type setter
        *
        *   @param  aEventType   event type
        */
        virtual void SetEventType( const TLogsEventType aEventType ) = 0;

       /**
        *   Duplicates setter
        *
        *   @param  aDuplicates   number of duplicates
        */
        virtual void SetDuplicates( const TInt8 aDuplicates ) = 0; 

	   /**
        * Is read/unread setter
        *
        * @param true/false
        */
        virtual void SetIsRead(TBool aRead) = 0;
        
       /**
        * Is read/unread getter
        *
        * @return true/false
        */
		virtual TBool IsRead() const = 0;
        
       /**
        *   Time getter
        *
        *   @return  time
        */
        virtual TTime Time() const = 0;

       /**
        *   Log Id getter
        *
        *   @return  log event id
        */
        virtual TLogId LogId() const = 0;

       /**
        *   Number getter
        *
        */
        virtual  HBufC* Number() = 0;

       /**
        *   Remote party getter
        *
        */
        virtual  HBufC* RemoteParty() = 0;

       /**
        *   Direction getter
        *
        *   @return  aDirection   direction
        */
        virtual  TLogsDirection Direction() const = 0;

       /**
        *   Uid getter
        *
        *   @param   Uid
        */
        virtual  TUid EventUid() const = 0;

       /**
        *   Event type getter
        *
        *   @param  event type
        */
        virtual  TLogsEventType EventType() const = 0;

       /**
        *   Duplicates getter
        *
        *   @param  number of duplicates
        */
        virtual  TInt8 Duplicates() const = 0;

       /**
        *   Additional LogsData object.
        *
        *   @return  MLogsEventData object.
        */
        virtual  MLogsEventData* LogsEventData() const = 0;

        /**
        *   Set Additional LogsData object.
        *
        *   @param pointer to eventData object
        */
        virtual void SetLogsEventData( MLogsEventData* aLogsEventData ) = 0;

       /**
        *   ALS flag (Alternate line service).
        *
        *   @return  ALS status flag. EFalse if not ALS event, ETrue if ALS event.
        */
        virtual  TBool ALS() const = 0;

        /**
        *   Set ALS flag.
        *
        *   @parama aAls als status
        */
        virtual void SetALS( TBool aALS ) = 0;

        /**
        *   CNAP flag (Calling name presentation service).
        *
        *   @return  CNAP status flag. EFalse if not CNAP event, ETrue if CNAP event.
        */
        virtual  TBool CNAP() const = 0;

        /**
        * Reset the event
        */
//        virtual void Reset() = 0;


        /**
        * NEW IMPLEMENTATION
        * Construct the event
        */
        virtual void InitializeEventL( const CLogEvent& /* aSource */, 
                                       TLogsEventStrings /*aStrings*/,
                                       TLogsModel /* aModel */ )  = 0; 


        
        
//For ring duation feature     
        /**
        *   Return Ring Duration
        *
        *   @return  Ring Duration
        */
        virtual TLogDuration RingDuration() const = 0;        

        /**
        *   Set Ring Duration
        *
        *   @parama  aDuration Ring duration
        */
        virtual void SetRingDuration( TLogDuration aDuration ) = 0; 
        
        /**
          * Phone number field type setter
          *
          * @param number field type
          */
         virtual void SetNumberFieldType( const TInt aNumberFieldType ) = 0;
         
         /**
           * Return Phone number field type
           *
           * @return Phone number field type
           */       
         virtual TInt NumberFieldType() const = 0;
    };

            
#endif

// End of File  __MLogsEvent_H_
