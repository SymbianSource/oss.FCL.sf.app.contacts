/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wraps queue handling of TVPbkSimContactEventData objects.
*
*/


#ifndef CVPBKSIMSERVEREVENTQUEUE_H
#define CVPBKSIMSERVEREVENTQUEUE_H

// INCLUDES
#include <e32base.h>
#include <VPbkSimServerCommon.h>

// CONSTANTS
const TInt KNextEvent(0);

// FORWARD DECLARATIONS


// CLASS DECLARATIONS

/**
 * Wraps queue handling of TVPbkSimContactEventData objects.
 */
NONSHARABLE_CLASS(CVPbkSimServerEventQueue) : public CBase
    {
    public: // C'tor & d'tor
        static CVPbkSimServerEventQueue* NewL(TInt aGranularity, TInt aMaxEvents);
        
        ~CVPbkSimServerEventQueue();
    
    public: // interface
    
        /**
         * @return first event from queue
         * @see RArray::operator[]
         */
        TVPbkSimContactEventData& FirstEvent();
    
        /**
         * @return general error code
         * @see RArray::Append
         */
        TInt Push(TVPbkSimContactEventData aEvent);

        /**
         * Removes first item from event queue.
         * @see RArray::Remove
         */
        void Pop();
        
        /**
         * @return ETrue if queue count >= iMaxEventCount
         */
        TBool IsFull();        
        
        /**
         * @return ETrue if queue count == 0
         */
        TInt IsEmpty();
    
    private: // implementation
        CVPbkSimServerEventQueue(TInt aGranularity, TInt aMaxEvents);
    
    private: // data
        /// Own: queue of events
        RArray<TVPbkSimContactEventData> iEventQueue;
        /// Own: max amount of events in queue
        TInt iMaxEventCount;
    };
    
#endif // CVPBKSIMSERVEREVENTQUEUE_H
// End of file
