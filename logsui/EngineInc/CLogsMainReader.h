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
*     Main list reader
*
*/


#ifndef __Logs_Engine_CLogsMainReader_H__
#define __Logs_Engine_CLogsMainReader_H__

//  INCLUDES
#include <logclientchangeobserver.h>
#include "CLogsBaseReader.h"

// FORWARD DECLARATION
class CLogViewEvent;
class CLogFilter;
class CLogFilterList;

// CLASS DECLARATION
class MLogsEventArray;

/**
 *  Main list reader
 */
class CLogsMainReader : public CLogsBaseReader,
                        public MLogClientChangeObserver
    {

    public:

        /**
         *  Two phase constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aEventArray  array of events
         *  @param aStrings     log client constants
         *  @param aModel       model id
         *  @param aObserver    observer
         */
        static CLogsMainReader* NewL( RFs& aFsSession, 
                        MLogsEventArray& aEventArray, 
                        TLogsEventStrings& aStrings, TLogsModel aModel,
                        MLogsObserver* aObserver,
                        CLogsEngine* aLogsEngineRef );
        
       /**
        *   Destructor.
        */
        virtual ~CLogsMainReader();

    private:

       /**
        *  Default constructor
        */
        CLogsMainReader();
       
        /**
         *  Constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aEventArray  array of events
         *  @param aStrings     log client constants
         *  @param aModel       model id
         *  @param aObserver    observer
         */
        CLogsMainReader( RFs& aFsSession, 
                        MLogsEventArray& aEventArray, 
                        TLogsEventStrings& aStrings, TLogsModel aModel,
                        MLogsObserver* aObserver,
                        CLogsEngine* aLogsEngineRef );
                                    
        /**
         *  Second phase constructor
         */
        void ConstructL();

        
        /**
         *  Creates and returns a filter.
         *  Leaves filter object on cleanupstack.
         *
         *  @param  aFilterType     Filter type id.
         *
         *  @return  Pointer to created and configured filter.
         */
        CLogFilter* SetOneFilterLC( TUid aFilterType );

        /**
         *  Creates and returns a filter.
         *  Leaves filter object on cleanupstack.
         *
         *  @param  aDirection     Filter direction
         *
         *  @return  Pointer to created and configured filter.
         */
        CLogFilter* SetOneFilterLC( TLogString& aDirection );

        /**
         *  Creates and returns a filter.
         *  Leaves filter object to cleanupstack.
         *
         *  @param  aNumber     number filter
         *  @param  aName       remoty party filter
         *
         *  @return  Pointer to created and configured filter.
         */
        CLogFilter* SetOneFilterLC( const TDesC* aNumber, const TDesC* aName );

        /**
         *  Set filter.
         *
         *  @param aFilter          Filter type.
         *  @param aNumber          Telephone number as filter criterion,
         *                              NULL is a default
         *  @param aName            Person's name as filter criterion,
         *                              NULL is a default
         */
        void SetFilterL( const TLogsFilter aFilter,
            const TDesC* aNumber = NULL, const TDesC* aName = NULL );

    private:  // from CLogsBaseReader 

        TInt ViewCountL() const;
        CLogEvent& Event() const;
        TBool DoNextL();
        void ConstructEventL( MLogsEvent& aDest, const CLogEvent& aSource );
    
    protected: // From CActive

        void DoCancel();
        

    public: // from MLogsReader

        void Stop();

        void StartL();

        void ConfigureL( const MLogsReaderConfig* aConfig ); 

    private: // from MLogClientChangeObserver
		/** (copied from LOGCLIENTCHANGEOBSERVER.H: 
        Handle a global change within the logs database
		
		This method is called when a generic event occurs within the log database. 
		Such events are	not specific to a view (see MLogViewChangeObserver for view 
		specific events).
		
		@param aChangeType The uid corresponding to the type of event which 
		occurred.
		@param aChangeParam1 A context-specific change paramater which can be 
		interpreted	based on the change type.
		@param aChangeParam2 A context-specific change paramater which can be 
		interpreted based on the change type.
		@param aChangeParam3 A context-specific change paramater which can be 
		interpreted	based on the change type.
		
		A list of change types and their associated parameters follows:
		
		KLogClientChangeEventLogCleared - no parameters. This event occurs when 
		the log database is cleared.	 */
        void HandleLogClientChangeEventL(   TUid aChangeType, 
                                            TInt aChangeParam1, 
                                            TInt aChangeParam2, 
                                            TInt aChangeParam3);
            
    private: // data
        
        /// Own: View to filtered events.
        CLogViewEvent* iLogViewEvent;        
               
        /// Own: Filterlist
        CLogFilterList* iFilterList;
    };

#endif      // __Logs_Engine_CLogsMainReader_H__


// End of File
      

        
       
