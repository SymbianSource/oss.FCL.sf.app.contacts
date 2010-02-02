/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Provides methods for the test dll.
*
*/








#ifndef __CContactEngineEventQueue_H__
#define __CContactEngineEventQueue_H__

// INCLUDES
#include <MPbkContactDbObserver.h>
#include "CFixedQueue.h"


// CLASS DECLARATION
class CPbkContactChangeNotifier;
class CPbkContactEngine;

/**
 * Contact Engine event queue class.
 */ 
class CContactEngineEventQueue : 
        public CTimer, public MPbkContactDbObserver
    {
    public:
        /**
         * Creates a new event queue.
         * @param aView contact view to listen for events.
         * @param aMaxQueueSize maximum size of event queue.
         * @return a new instance of this class.
         */
        static CContactEngineEventQueue* NewL
            (CPbkContactEngine* aEngine=NULL, TInt aMaxQueueSize=128);
        
        /**
         * Destructor.
         */
        ~CContactEngineEventQueue();

        /**
         * Waits for an event from the database.
         * @param aTimeOut  max time to wait for an event.
         * @param aEvent    the received event, undefined if this function returns false.
         * @return true if an event was received, false if timeout expired first.
         */
        TBool ListenForEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactDbObserverEvent& aEvent);

        /**
         * Consumes events in a loop.
         * @param aTimeOut  max time to wait for an event.
         * @param aEvent    the received event, undefined if this function returns false.
         * @param aMaxCount max count of events to consume.
         * @return true if there did not come any new events within the max count
         *          else false.
         */
        TBool ConsumeEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactDbObserverEvent& aEvent,
            TInt aMaxCount);
        
       /**
         * Removes all previously arrived events from the queue.
         */
        inline void Flush()
            {
            iQueue.Reset();
            }

    protected: // From MPbkContactDbObserver.
	    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	    void DatabaseEventHandledL(TContactDbObserverEvent aEvent);

    private:  // from CTimer
        void RunL();

    private:  // Implementation
        CContactEngineEventQueue();
        void ConstructL(CPbkContactEngine* aEngine, TInt aMaxQueueSize);

    private:  // Data
        /// Own: the notifier for attaching to ContactEngine
        CPbkContactChangeNotifier* iNotifier;
        /// Own: event queue array
        CFixedQueue<TContactDbObserverEvent> iQueue;
    };

#endif // __CContactEngineEventQueue_H__

// End of File
