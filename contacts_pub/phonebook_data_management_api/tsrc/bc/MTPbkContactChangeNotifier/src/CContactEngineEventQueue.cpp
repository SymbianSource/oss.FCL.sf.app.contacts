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








// INCLUDES
#include "CContactEngineEventQueue.h"

#include <E32SVR.H>
#include <CPbkContactChangeNotifier.h>



const TInt KSecondsToMicrosecondsMultiplier = 1000000;

// ================= MEMBER FUNCTIONS =======================

inline CContactEngineEventQueue::CContactEngineEventQueue()
    : CTimer(CActive::EPriorityStandard)
    {
    }

CContactEngineEventQueue* CContactEngineEventQueue::NewL
        (CPbkContactEngine* aEngine, TInt aMaxQueueSize)
    {
    CContactEngineEventQueue* self = new(ELeave) CContactEngineEventQueue;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine,aMaxQueueSize);
    CleanupStack::Pop(self);
    return self;
    }

CContactEngineEventQueue::~CContactEngineEventQueue()
    {
    CTimer::Cancel();
    delete iNotifier;
    }

TBool CContactEngineEventQueue::ListenForEvent(TTimeIntervalSeconds aTimeOut,
                                               TContactDbObserverEvent& aEvent)
    {
    CTimer::Cancel();
    if (iQueue.IsEmpty())
        {
        CTimer::After(aTimeOut.Int() * KSecondsToMicrosecondsMultiplier);
        // Keep execution here until the timer expires or it is canceled
        do  {
            CActiveScheduler::Start();
            }
        while (CTimer::IsActive());
        }

    if (!iQueue.IsEmpty())
        {
        aEvent = iQueue.Head();
        iQueue.PopHead();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CContactEngineEventQueue::ConsumeEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactDbObserverEvent& aEvent,
            TInt aMaxCount)
    {    
    TBool found;
    TInt count = 0;
    do
        {
        found  = ListenForEvent(aTimeOut,aEvent);
        count++;
        
        // don't wait forever, break after
        // aMaxCount number of events found
        if (count > aMaxCount)
            {
            break;        
            }
        }
    while (found);
    
    // if still found return false, else true
    return !found;
    }


void CContactEngineEventQueue::RunL()
    {
    // Timer expired
    CActiveScheduler::Stop();
    }


void CContactEngineEventQueue::HandleDatabaseEventL
        (TContactDbObserverEvent aEvent)
    {
    const TBool timerActive = CTimer::IsActive();
    CTimer::Cancel();
    ASSERT(iQueue.Push(aEvent));
    if (timerActive)
        {
        CActiveScheduler::Stop();
        }
    }

void CContactEngineEventQueue::DatabaseEventHandledL
        (TContactDbObserverEvent aEvent)
    {
    const TBool timerActive = CTimer::IsActive();
    CTimer::Cancel();
    ASSERT(iQueue.Push(aEvent));
    if (timerActive)
        {
        CActiveScheduler::Stop();
        }
    }

void CContactEngineEventQueue::ConstructL
        (CPbkContactEngine* aEngine, TInt aMaxQueueSize)
    {
    CActiveScheduler::Add(this);
    CTimer::ConstructL();
    iQueue.ConstructL(aMaxQueueSize);
    
    if (aEngine)
        {
        iNotifier = CPbkContactChangeNotifier::NewL(*aEngine, this);
        }
    }

// End of File
