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


// INCLUDES
#include "CVPbkSimServerEventQueue.h"

namespace 
    {
    const TInt KFirstItemIndex(0);
    
    #ifdef _DEBUG
        enum TPanicCode
            {
            ELogicPanic_Push,
            ELogicPanic_FirstEvent,
            ELogicPanic_Pop
            };
        void Panic(TPanicCode aPanic)
            {
            _LIT(KPanicCat, "CVPbkSimServerEventQueue");
            User::Panic(KPanicCat, aPanic);
            }
    #endif // _DEBUG
    } // unnamed namespace
    
CVPbkSimServerEventQueue::CVPbkSimServerEventQueue(TInt aGranularity, TInt aMaxEvents)
:   iEventQueue(aGranularity), iMaxEventCount(aMaxEvents)
    {    
    }
    
CVPbkSimServerEventQueue* CVPbkSimServerEventQueue::NewL(TInt aGranularity, 
                                                         TInt aMaxEvents)
    {
    CVPbkSimServerEventQueue* self = new (ELeave)CVPbkSimServerEventQueue(aGranularity,
                                                                          aMaxEvents);
    return self;
    }

CVPbkSimServerEventQueue::~CVPbkSimServerEventQueue()
    {
    iEventQueue.Close();
    }

TVPbkSimContactEventData& CVPbkSimServerEventQueue::FirstEvent()
    {
    __ASSERT_DEBUG(!IsEmpty(), Panic(ELogicPanic_FirstEvent));
    return iEventQueue[KFirstItemIndex];        
    }

TInt CVPbkSimServerEventQueue::Push(TVPbkSimContactEventData aEvent)
    {
    __ASSERT_DEBUG(!IsFull(), Panic(ELogicPanic_Push));
    return iEventQueue.Append(aEvent);
    }

void CVPbkSimServerEventQueue::Pop()
    {
    __ASSERT_DEBUG(!IsEmpty(), Panic(ELogicPanic_Pop));
    iEventQueue.Remove(KFirstItemIndex);
    }

TBool CVPbkSimServerEventQueue::IsFull()
    {
    return (iEventQueue.Count() >= iMaxEventCount);
    }

TInt CVPbkSimServerEventQueue::IsEmpty()
    {
    return (iEventQueue.Count() == 0);
    }

// End of file
