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
#ifndef LOGSREMOVE_H
#define LOGSREMOVE_H

#include <QObject>
#include <e32base.h>
#include "logsmodel.h"

// FORWARDS DECLARATIONS
class CLogClient;
class RFs;
class CLogViewRecent;

// CLASS DECLARATION
class LogsRemoveObserver;
/**
 *	Clearing class.
 */
class LogsRemove : public CActive
    {
    public:
        friend class UT_LogsRemove;

        /**
         *  Destructor
         */
        ~LogsRemove();
        
        LogsRemove( LogsRemoveObserver& observer, bool readingAllEvents = false );
	

        /**
          * Initialize LogsRemove
          * @return 0 if initialized succesfully
          */
        int init();

        bool clearList(LogsModel::ClearType cleartype);
        
        int clearEvents(const QList<int>& eventIds, bool& async);
        
    protected: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:
        
        void initL();
        void clearEventsL(const QList<int>& eventIds, bool& async);
        bool DeleteNextEvent();
        
    private: // data
        LogsRemoveObserver& mObserver;
        bool mReadingAllEvents;
        
        QList<int> mRemovedEvents;
        CLogClient* mLogClient;
        CLogViewRecent* mRecentView;
        RFs* mFsSession;
    };

                  
#endif // LOGSREMOVE_H
