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
*     Class for setting a missed calls event and its duplicates read. This is needed only for 
*     clearing the new missed icon and duplicate counters for one missed calls event if a call or 
*     message is initiated. 
* 
*
*/


#ifndef __CLOGSCLEARNEWMISSED_H_
#define __CLOGSCLEARNEWMISSED_H_

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <logwrap.h>
#include <logviewchangeobserver.h>


#include "MLogsEvent.h"
#include "MLogsClearLog.h"
#include "MLogsStateHolder.h"
#include "MLogsClearNewMissed.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogClient;
class MLogsObserver;
class CLogViewRecent;
class CLogViewDuplicate;
class MLogsEventGetter;

// CLASS DECLARATION

/**
 *	Clearing class.
 */
class CLogsClearNewMissed : public CActive,
                         public MLogsClearNewMissed
                     // public MLogsStateHolder
    {
    public:
        /**
         * Symbian OS constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer. Default NULL
         * @return New instance of this class
         */
        static CLogsClearNewMissed* NewL
                            (   RFs& aFsSession );

        /**
         *  Destructor
         */
        ~CLogsClearNewMissed();

    private:
    
        /**
         * Default constructor. 
         */
        CLogsClearNewMissed();

        /**
         * Second phase constructor. 
         */
        void ConstructL();

        /**
         * C++ constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer
         */
        CLogsClearNewMissed( RFs& aFsSession );

    protected: // from CActive
        void DoCancel();
        void RunL();
		TInt RunError(TInt aError);

    public: 
        /**
         * Sets one Log missed call event read in order to clear the new missed call icon.
         * 
         * As a first thing calls GetEvent to fetch the event details from the log as we cannot relay 
         * to Logs event array which could be reseted just before a call to SetEventRead.
         * 
         * If the event doesn't have any duplicates, then simply calls CLogClient::ChangeEvent. 
         * If there are duplicates, we need to first set the recent lists view
         * and then the get the duplicates view of the current event.
         * 
         * @param aLogid    the id of the Log event
         */
        void SetNewMissedCallRead(TLogId aLogId, TInt aDuplicates);    
    
    private:
        
       /**
        * Tries to set a filtered recent view.
        * Goal is to get a view which only contains the event being processed
        * as this is only needed to get the duplicates of the event.
        * 
        * return true/false 
        */
        TBool TrySetRecentViewL();
        
       /**
        * If the recent view was succesfully set, proceed to 
        * get the duplicates view of the current event.
        * 
        * return true/false 
        */
        TBool TrySetDuplicatesViewL();
        
       /**
        * Set the read flags of the recent and duplicates view.
        * Clears all duplicates too. 
        */
        void SetReadFlagsL();
        
       /**
        * Helper function to set the recent lists filter. Goal is to 
        * get a view which only contains the event being processed. 
        */
        void SetRecentFilterL();
        
       /**
        * As a first thing we'll fetch the event details from the log.          
        */
        void GetEventL(TLogId aLogId);
        
 // internal states of object
        enum TClearNewMissedPhase
            {
            EUnInitilized = 0,
            EGetEvent,
            EChangeEvent,
        	ESetRecentView,
        	ESetDuplicatesView,
            EFinished
            };
    

    private: // data
        /// Ref: File server session
        RFs&                iFsSession;

        /// Own: Log client
        CLogClient*         iLogClient;
        CLogEvent*          iLogEvent;

        TClearNewMissedPhase        iPhase;
        
        TInt 				iDuplicates;
        
        /// Own: View to filtered events.
        CLogViewRecent*         iLogViewRecent;
        /// Own: duplicate filter, excludes 
        CLogFilter*             iRecentFilter;
        
        CLogViewDuplicate* 		iLogViewDuplicates;
        /// Own: duplicate filter, excludes 
        CLogFilter*             iDuplicateFilter;

    };

#endif

// End of File  __CLOGSCLEARNEWMISSED_H_
