/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDES
#include "CContactEngineEventQueue.h"
#include <E32SVR.H>
#include <CPbkContactChangeNotifier.h>


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

TBool CContactEngineEventQueue::ListenForEvent(
        TTimeIntervalSeconds aTimeOut, TContactDbObserverEvent& aEvent)
    {
    RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ListenForEvent, start"));

    CTimer::Cancel();
    if (iQueue.IsEmpty())
        {
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ListenForEvent, iQueue.IsEmpty()"));
        
        CTimer::After(aTimeOut.Int() * 1000000);
        // Keep execution here until the timer expires or it is canceled
        do  {
            CActiveScheduler::Start();
            }
        while (CTimer::IsActive());
        }
    if (!iQueue.IsEmpty())
        {
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ListenForEvent, !iQueue.IsEmpty()"));

        aEvent = iQueue.Head();
        iQueue.PopHead();
        return ETrue;
        }
    else
        {
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ListenForEvent, ELSE case"));

        return EFalse;
        }
    }

TBool CContactEngineEventQueue::ConsumeEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactDbObserverEvent& aEvent,
            TInt aMaxCount)
    {    
    RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ConsumeEvent, start"));

    TBool found;
    TInt count = 0;
    do
        {
        found  = ListenForEvent(aTimeOut,aEvent);
        count++;
        
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ConsumeEvent, listening, count = %d"), count);

        // don't wait forever, break after
        // aMaxCount number of events found
        if (count > aMaxCount)
            {
            break;        
            }
        }
    while (found);
    
    if (found)
        {
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ConsumeEvent, off-loop, more exists"));
        }
    else
        {
        RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::ConsumeEvent, off-loop, NO exists"));        
        }
    
    // if still found return false, else true
    return !found;
    }

void CContactEngineEventQueue::RunL()
    {
    // Timer expired
    RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::RunL() timer expired."));
    CActiveScheduler::Stop();
    }

void CContactEngineEventQueue::HandleDatabaseEventL
        (TContactDbObserverEvent aEvent)
    {
    RDebug::Print(_L("**HWDBG**: CContactEngineEventQueue::HandleDatabaseEventL()"));

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
