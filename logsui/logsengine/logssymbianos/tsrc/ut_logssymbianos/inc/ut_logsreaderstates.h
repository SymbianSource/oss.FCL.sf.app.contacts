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
#ifndef UT_LOGSREADERSTATES_H
#define UT_LOGSREADERSTATES_H

#include <QObject>
#include <QList>
#include <e32base.h>
#include "logsengdefs.h"
#include "logsreaderstatecontext.h"
#include "logsreaderobserver.h"

class CLogClient;
class LogsEvent;
class CLogViewRecent;
class CLogViewEvent;
class LogsReaderStateBase;

class UT_LogsReaderStates : public QObject, 
                            public LogsReaderStateContext, 
                            public LogsReaderObserver              
{
     Q_OBJECT
    
private slots:

/*
 * In addition, there are four private slots that are not treated as testfunctions. 
 * They will be executed by the testing framework and can be used to initialize and clean up 
 * either the entire test or the current test function.
 * 
 * initTestCase() will be called before the first testfunction is executed.
 * cleanupTestCase() will be called after the last testfunction was executed.
 * init() will be called before each testfunction is executed.
 * cleanup() will be called after every testfunction.
*/
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots: //test methods
     
    void testStateBase();
    void testStateFiltering();
    void testStateFilteringAll();
    void testStateReading();
    void testStateReading2();
    void testStateFillDetails();
    void testStateDone();
    void testStateSearchingEvent();
    void testStateFindingDuplicates();
    void testStateModifyingDone();

protected: // From LogsReaderStateContext
    
      void setCurrentState(const LogsReaderStateBase& state);
      CLogView& logView();
      CLogViewDuplicate& duplicatesView();
      QList<LogsEvent*>& events();
      int& index();
      LogsEventStrings& strings();
      TRequestStatus& reqStatus();
      LogsReaderObserver& observer();
      QHash<QString, ContactCacheEntry>& contactCache();
      int modifyingEventId();
      CLogClient& logClient();
      bool isRecentView();
      

protected: // From LogsReaderObserver
    
      void readCompleted(int readCount);
      void errorOccurred(int err);
      void temporaryErrorOccurred(int err);
      void eventModifyingCompleted();

          
private:
    
    void reset();

    
private:

      CLogClient* mLogClient;
      const LogsReaderStateBase* mCurrentState;
      CLogViewRecent* mLogView;
      CLogViewEvent* mLogViewEvent;
      CLogViewDuplicate* mDuplicatesView;
      QList<LogsEvent*> mEvents;
      int mIndex;
      LogsEventStrings mStrings;
      TRequestStatus mReqStatus;
      bool mIsRecentView;
      
      int mError;
      int mTemporaryError;
      bool mReadCompleted;
      bool mModifyCompleted;
      int mReadCount;
      
      QHash<QString, ContactCacheEntry> mContactCache;
      
      int mModifyingEventId;
};


#endif //UT_LOGSREADERSTATES_H
