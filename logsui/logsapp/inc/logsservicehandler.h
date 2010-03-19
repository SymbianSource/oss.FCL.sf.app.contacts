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
#ifndef LOGSSERVICEHANDLER_H
#define LOGSSERVICEHANDLER_H

#include <xqserviceprovider.h>
#include <logsservices.h>

class LogsServiceHandler : public XQServiceProvider
    {
        Q_OBJECT
    public:
        LogsServiceHandler();
        ~LogsServiceHandler();
    
    public slots:
        int start(int activatedView, bool showDialpad);
        
    signals:
    
        void activateView(LogsServices::LogsView activatedView, bool showDialpad);
    
    public:
        
        LogsServices::LogsView currentlyActivatedView();
        bool isStartedUsingService() const;
        
    private:
        int mActivatedView;
        bool mIsAppStartedUsingService;
        
    private:
        friend class UT_LogsServiceHandler;
        friend class UT_LogsViewManager;
    };

#endif //LOGSSERVICEHANDLER_H
