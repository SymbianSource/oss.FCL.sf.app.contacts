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

#ifndef LOGSREADERSTATECONTEXT_H
#define LOGSREADERSTATECONTEXT_H

//  INCLUDES
#include <QList>
#include <QHash>

// FORWARD DECLARATION
class LogsReaderStateBase;
class CLogView;
class CLogViewDuplicate;
class CLogViewEvent;
class LogsEvent;
class LogsEventStrings;
class TRequestStatus;
class LogsReaderObserver;
class CLogClient;

// CLASS DECLARATION

class ContactCacheEntry 
{
public:
    inline ContactCacheEntry() : mRemoteParty(QString()), mContactLocalId(0) {}
    inline ContactCacheEntry( const ContactCacheEntry& entry );
    inline ContactCacheEntry(QString remoteParty, unsigned int contactId);
    
    QString mRemoteParty;
    unsigned int mContactLocalId;
};
        

/**
 * Context for reader state machine
 */
class LogsReaderStateContext {
    public:
        
        /**
         * Change state
         * @param state, new state
         */
        virtual void setCurrentState(const LogsReaderStateBase& state) = 0;
        
        /**
         * Get log view
         * @return current log view
         */
        virtual CLogView& logView() = 0;
        
        /**
         * Get duplicates view
         * @return current duplicates view
         */
        virtual CLogViewDuplicate& duplicatesView() = 0;
        
        /**
         * Get event container
         * @return events
         */
        virtual QList<LogsEvent*>& events() = 0;
        
        /**
         * Get view index
         * @return ref to view index
         */
        virtual int& index() = 0;
        
        /**
         * Get commonly used strings
         * @return strings
         */
        virtual LogsEventStrings& strings() = 0;
        
        /**
         * Request status for async operations
         * inside states
         * @return reqstatus
         */
        virtual TRequestStatus& reqStatus() = 0;
        
        /**
         * Get observer
         * @return observer
         */
        virtual LogsReaderObserver& observer() = 0;
        
        /**
         * Get contact cache
         * @return cache (key:telnum, value:contactname)
         */
        virtual QHash<QString, ContactCacheEntry>& contactCache() = 0;
        
        /**
         * Get ID of the event to be modified
         * @return ID of the event
         */
        virtual int modifyingEventId() = 0;
        
        /**
         * Get log client
         * @return log client
         */
        virtual CLogClient& logClient() = 0;
        
        /**
         * Check if current view is recent view.
         * @return true if recent view
         */
        virtual bool isRecentView() = 0;
};



inline ContactCacheEntry::ContactCacheEntry( const ContactCacheEntry& entry ) 
{
    mRemoteParty = entry.mRemoteParty;
    mContactLocalId = entry.mContactLocalId;
}

inline ContactCacheEntry::ContactCacheEntry(QString remoteParty, unsigned int contactId)
{
    mRemoteParty = remoteParty;
    mContactLocalId = contactId;    
}

#endif      // LOGSREADERSTATECONTEXT_H


// End of File
      

        
       
