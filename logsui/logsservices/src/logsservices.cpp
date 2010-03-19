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

#include <xqservicerequest.h>
#include <logsservices.h>
#include "logslogger.h"

// --------------------------------------------------------------------------
// LogsServices::start
// --------------------------------------------------------------------------
//
int LogsServices::start(LogsView activatedView, bool showDialpad)
{
    LOGS_QDEBUG_2( "LogsServices::start ->", activatedView )
    // Need to do request in async manner, otherwise new logs ui process
    // will be started due bug(?) in highway.
    XQServiceRequest snd("com.nokia.services.logsservices.starter",
            "start(int,bool)", false);
    snd << (int)activatedView;
    snd << showDialpad;
    int retValue;
    bool res=snd.send(retValue);
    LOGS_QDEBUG_3( "LogsServices::start <-", activatedView, retValue )
    return res ? retValue : -1;
}
