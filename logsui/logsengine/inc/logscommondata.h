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

#ifndef LOGSCOMMONDATA_H
#define LOGSCOMMONDATA_H

#include <qmobilityglobal.h>
#include "logsevent.h"

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/**
 * Commonly shared data.
 */
class LogsCommonData 
{ 
    private: 
        explicit LogsCommonData();
        virtual ~LogsCommonData();
        
    public: 
        static LogsCommonData& getInstance();
        static void freeCommonData();
        
        QContactManager& contactManager();
        
        /**
         * Configure maximum number of events. It is possible to
         * configure that reading is stopped once defined amount
         * of events of certain direction has been read.
         */
        void configureReadSize(
                int maxSize, 
                LogsEvent::LogsDirection dir = LogsEvent::DirUndefined );
        /**
         * Get max read size
         * @return max number of events, -1 if max num is not configured 
         */
        int maxReadSize() const;
        LogsEvent::LogsDirection maxReadSizeDirection() const;
        
    private:
        
        QContactManager* mContactManager;
        int mMaxReadSize;
        LogsEvent::LogsDirection mMaxReadSizeDir;
    
    private:
        friend class UT_LogsCommonData;
        
};


#endif // LOGSCOMMONDATA_H
