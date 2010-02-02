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
*     Implements interface for log event getting
*
*/


#ifndef __Logs_Engine_CLogsGetEvent_H__
#define __Logs_Engine_CLogsGetEvent_H__

//  INCLUDES
#include <e32base.h>
#include <logwrap.h>
#include "MLogsGetEvent.h"
#include "MLogsStateHolder.h"
#include "TLogsEventStrings.h"

// FORWARD DECLARATION
class CLogClient;
class CLogViewEvent;
class CLogFilter;
class CLogFilterList;
class MLogsEventGetter;
class MLogsEvent;
class CLogEvent;
class MLogsObserver;
class MLogsEvent;
class MLogsEventWrapper;
class CLogsGetEventNotify;

// CLASS DECLARATION

/**
 *  Implements interface for Logs reader. Common functionality.
 */
class CLogsGetEvent : public CActive, 
                        public MLogsGetEvent,
                        public MLogsStateHolder
    {
    public:
        
        /**
         *  EPOC constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aStrings     log client constants
         *  @param aObserver    observer
         */
        static CLogsGetEvent* NewL( RFs& aFsSession, 
            TLogsEventStrings& aStrings,
            MLogsObserver* aObserver = NULL );

       /**
        *   Destructor.
        */
        virtual ~CLogsGetEvent();

    private:

       /**
        *  Default constructor
        */
        CLogsGetEvent();
       
        /**
         *  Constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aStrings     log client constants
         *  @param aObserver    observer
         */
        CLogsGetEvent( RFs& aFsSession, 
                        TLogsEventStrings& aStrings, MLogsObserver* aObserver );

       /**
        *  2nd phase construction
        */
        void ConstructL();
                                            
        /**
         *  Event construction
         *
         *  @param aDest    ref. to MLogsEvent implemetation object
         *  @param aSource  pointer to CLogEvent object
         */
        void ConstructEventL( MLogsEvent& aDest, const CLogEvent& aSource );
                

    protected: // From CActive

        void DoCancel();
        void RunL();
		TInt RunError(TInt aError);

    public: // from MLogsGetEvent

        void Get( TLogId aId );
        const MLogsEventGetter* Event();
        const CLogEvent* LogEvent() const;
        void SetObserver( MLogsObserver* aObserver );  

    public: // from MLogsStateHolder

        TLogsState State() const;
      
                   
    private:    // data
        
        /// ref: file session
        RFs&            iFsSession;  
                
        /// own: string constants
        TLogsEventStrings&   iStrings;

        /// own: log client
        CLogClient*     iLogClient;

        /// ref: observer
        MLogsObserver*  iObserver;
                         
        /// own: state for observer
        TLogsState      iState;  
        
        /// Own: Event
        CLogEvent* iLogEvent;

        /// Own: Logs Event
        MLogsEvent* iEvent;

        /// Own: wrapper for CLogsGetEvent
        MLogsEventWrapper* iWrapper;

        /// Own:
        CLogsGetEventNotify* iNotify;

        /// Own: const emergency nbr supported
        TBool iStaticEmerg;
    };

#endif      // __Logs_Engine_CLogsGetEvent_H__


// End of File



        

        
       
