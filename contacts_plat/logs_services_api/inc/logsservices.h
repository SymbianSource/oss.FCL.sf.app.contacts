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
#ifndef LOGSSERVICES_H
#define LOGSSERVICES_H

#include <logsexport.h> 

/**
* Logs application services
*/
class LogsServices
{
    public:
    
    enum LogsView{
        ViewAll,
        ViewReceived,
        ViewCalled,
        ViewMissed
    };
    /**
     * Requests to start Logs application. If Logs is already running,
     * activates the requested view.
     *
     * @param activatedView   The view to be activated in Logs.
     * @param showDialpad     Specifies whether dialpad should be shown on top
     *                        of the activated view
     * @return 0 if started/activated succesfully
     */
    LOGSSERVICES_EXPORT static int start(LogsView activatedView, bool showDialpad);

};

#endif  //LOGSSERVICES_H
