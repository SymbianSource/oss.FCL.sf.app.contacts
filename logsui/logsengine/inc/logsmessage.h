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
#ifndef LOGSMESSAGE_H
#define LOGSMESSAGE_H


#include <QObject>
#include <logsexport.h>

#include "logsevent.h"

/**
 * LogsMessage can be used to send message.
 */
class LogsMessage : public QObject 
{
    Q_OBJECT
    
public: 
 		
    explicit LogsMessage(LogsEvent& aEvent);
    explicit LogsMessage(unsigned int contactId, const QString& number);
    LOGSENGINE_EXPORT ~LogsMessage();
    
    bool isMessagingAllowed();
    
public slots:

    /**
     * Send message
     * @return true if sent succesfully
     */
    LOGSENGINE_EXPORT bool sendMessage();
        
private: //data 
    
    bool mIsAllowed;
    QString mNumber;
    unsigned int mContactId;
    
private:
    friend class UT_LogsMessage;
    
};
                  
#endif // LOGSMESSAGE_H
