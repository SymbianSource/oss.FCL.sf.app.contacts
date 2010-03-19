/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef LOGSDBCONNECTOR_H
#define LOGSDBCONNECTOR_H

//  INCLUDES
#include <QObject>
#include "logsengdefs.h"
#include "logsreaderobserver.h"
#include "logsremoveobserver.h"
#include "logsmodel.h"
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogClient;
class LogsReader;
class LogsEvent;
class LogsRemove;
class RFs;
class CRepository;

// CLASS DECLARATIONS


/**
 * LogsDbConnector is used to initialize database access
 */
class LogsDbConnector :  
    public QObject, public LogsReaderObserver, public LogsRemoveObserver
    {
    Q_OBJECT
    
    public:

        /**
         * Constructor
         * @param events, connector fills the array with events
         *  read from database. Ownership of events is not transferred.
         * @param checkAllEvents, true if interested in all events of the database,
         *  false if interested only in recent events
         */
        LogsDbConnector( QList<LogsEvent*>& events, bool checkAllEvents = false );
        ~LogsDbConnector();
        
    signals:
    
        /**
         * Signaled once some new data has been read from database.
         * Added data is placed to event array already when this signal
         * is emitted.
         * @param addedIndexes, indexes for new events
         */
        void dataAdded(QList<int> addedIndexes);   
        
        /**
         * Signaled once some data has been updated.
         * Corresponding events have been already updated at event array
         * when this signal is emitted.
         * @param updatedIndexes, indexes for updated events
         */
        void dataUpdated(QList<int> updatedIndexes);
        
        /**
         * Signaled once some data has been removed.
         * Corresponding events have been already removed from event array
         * when this signal is emitted.
         * @param removedIndexes, indexes for removed events
         */
        void dataRemoved(QList<int> removedIndexes);
        
        /**
         * Signaled once asycn event clearing has completed.
         * @param err, 0 if marking completed succesfully
         */
        void clearingCompleted(int err);
        
        /**
         * Signaled once asycn event marking has completed.
         * @param err, 0 if marking completed succesfully
         */
        void markingCompleted(int err);

    public:
        
        /**
         * Initialize database access
         * @return 0 if initialized succesfully
         */
		int init();
		
		/**
         * Start reading data from database
         * @return 0 if reading started succesfully
         */
		int start();
		
        /**
		 * Synchronously updates details (rematches from phonebook) for 
         * already read events
         * @param clearCached if true, cached matches will be cleaned before
         * rematching
         */
        int updateDetails(bool clearCached);
		
        /**
         * Clear events of defined type. Clearing is async and completion is
         * indicated by clearingCompleted signal.
         * @return true if async clearing started 
         */
		bool clearList(LogsModel::ClearType cleartype);
		
        /**
         * Starts removing events and all their duplicates. Clearing
         * can be sync or async. In case of async, completion is
         * indicated by clearingCompleted signal.
         * @param eventIds, ids of the events to be removed
         * @return true if async clearing started
         */
        bool clearEvents(const QList<int>& eventIds);
        
        /**
         * Mark events as seen. Completion is indicated by
         * markingCompleted signal.
         * @param eventIds, ids of the events to be marked
         * @return true if marking started
         */
        bool markEventsSeen(const QList<int>& eventIds);
        
        /**
         * Clear missed calls counter.
         * @return 0 if clearing was success
         */
        int clearMissedCallsCounter();
		
    protected: // From LogsReaderObserver
        
        virtual void readCompleted(int readCount);
        virtual void errorOccurred(int err);
		virtual void temporaryErrorOccurred(int err);
		virtual void eventModifyingCompleted();
		
	protected: // From LogsRemoveObserver
		virtual void removeCompleted();
		virtual void logsRemoveErrorOccured(int err);
        
    private:
		void initL();
		void handleTemporaryError(int& error);
		void deleteRemoved(int newEventCount);
		int doMarkEventSeen();
		bool handleModifyingCompletion(int err=0);

    private: // data

        QList<LogsEvent*>& mModelEvents;
        bool mCheckAllEvents;
        CLogClient* mLogClient;
        LogsEventStrings mLogEventStrings;
        RFs* mFsSession;
        LogsReader* mReader;
        LogsRemove* mLogsRemove;
        CRepository* mRepository;
        
        QList<LogsEvent*> mEvents;
        QList<int> mRemovedEventIndexes;
        QList<int> mUpdatedEventIndexes;
        QList<int> mAddedEventIndexes;
        QList<int> mEventsSeen;

    private: // Testing related friend definitions
        
        friend class UT_LogsDbConnector;
        friend class UT_LogsRemove;
        friend class UT_LogsModel;
    
    };

            
#endif

// End of File  LOGSDBCONNECTOR_H
