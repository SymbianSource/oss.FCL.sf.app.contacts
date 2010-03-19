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

#ifndef LOGSREADER_H
#define LOGSREADER_H

//  INCLUDES
#include <QList>
#include <QHash>
#include <e32base.h>
#include <logclientchangeobserver.h>
#include <logviewchangeobserver.h>
#include "logsreaderstatecontext.h"
#include "logsreaderstates.h"

// FORWARD DECLARATION
class CLogViewRecent;
class RFs;
class CLogClient;
class CLogViewEvent;
class LogsReaderObserver;
class LogsEvent;
class LogsEventStrings;
class LogsReaderStateFiltering;

// CLASS DECLARATION



/**
 * LogsReader is used to read events from database
 */
class LogsReader : public CActive,
                   public MLogClientChangeObserver,
                   public MLogViewChangeObserver,
                   public LogsReaderStateContext
    {

        friend class UT_LogsReader;
        friend class UT_LogsDbConnector;

    public:
        
        /**
         *  Constructor
         *  @param fsSession, connected fileserver session
         *  @param logClient
         *  @param strings, commonly used strings
         *  @param events, filled with events once reading progresses
         *  @param observer, information about read progress is notified
         *      via the interface
         *  @param readAllEvents, true if all events in db should be read,
         *      otherwise only recent events are read
         */
        LogsReader( RFs& fsSession,
                    CLogClient& logClient,
                    LogsEventStrings& strings,
                    QList<LogsEvent*>& events,
                    LogsReaderObserver& observer,
                    bool readAllEvents = false );
  
       /**
        *   Destructor.
        */
        virtual ~LogsReader();
        
        

    public:

        /**
         * Start reading
         * @return 0 if reading started succesfully
         */
        int start();
        
        /**
         * Stop reading
         */
        void stop();
        
        /**
         * Synchronously update details (rematches from phonebook) for 
         * already read events
         * @param clearCached if true, cached matches will be cleaned before
         * rematching
         */
        void updateDetails(bool clearCached);
                
        /**
         * Starts modifying the event and all its duplicates as read
         * @param eventId, id of the event to be removed
         * @return 0 if removing started succesfully
         */
        int markEventSeen(int eventId);

    protected: // From CActive

        void RunL();
        void DoCancel();
        TInt RunError(TInt error);

    private: // From MLogClientChangeObserver

        void HandleLogClientChangeEventL( TUid aChangeType, 
                                          TInt aChangeParam1, 
                                          TInt aChangeParam2, 
                                          TInt aChangeParam3 );
    private: // From MLogViewChangeObserver
        
        void HandleLogViewChangeEventAddedL( 
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);

        void HandleLogViewChangeEventChangedL(   
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);

        void HandleLogViewChangeEventDeletedL( 
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);
    
    private: // From LogsReaderStateContext
        
        inline void setCurrentState(const LogsReaderStateBase& state);
        inline CLogView& logView();
        inline CLogViewDuplicate& duplicatesView();
        inline QList<LogsEvent*>& events();
        inline int& index();
        inline LogsEventStrings& strings();
        inline TRequestStatus& reqStatus();
        inline LogsReaderObserver& observer();
        inline QHash<QString, ContactCacheEntry>& contactCache();
        inline int modifyingEventId();
        inline CLogClient& logClient();
        inline bool isRecentView();
        
    private:

        void startL();
        void markEventSeenL(int eventId);
        
        /**
         * Cancel all outstanding requests if possible. In case of deleting is
         * in progress, it will leave with error
         */
        void cancelCurrentRequestL();
        LogsReaderStateBase& currentState();
        void initializeReadStates();
        void initializeDeleteStates();
        void handleViewChange( int totalChangeCount = 1 );
        void handleError(int error);
        void createLogViewL();
        LogsReaderStateFiltering* createFilteringState();
        
    private: // data
        
        CLogViewRecent* mLogViewRecent;
        CLogViewEvent* mLogViewEvent;
        CLogViewDuplicate* mDuplicatesView;
        
        RFs& mFsSession;
        CLogClient& mLogClient;
        LogsEventStrings& mStrings;
        QList<LogsEvent*>& mEvents;
        LogsReaderObserver& mObserver;
        bool mReadAllEvents;
        
        int mIndex;
        QList<LogsReaderStateBase*> mReadStates;
        QList<LogsReaderStateBase*> mModifyingStates;
        int mCurrentStateIndex;
        QList<LogsReaderStateBase*>* mCurrentStateMachine;
        
        QHash<QString, ContactCacheEntry> mContactCache;

        int mModifyingEventId;
    };

#endif      // LOGSREADER_H


// End of File
      

        
       
