/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UT_AddFavoritiesVisibility class member functions
*
*/


#include "CCallbackTimer.h"


CCallbackTimer::~CCallbackTimer()
    {
    if(iWait.IsStarted())
        {
        iWait.AsyncStop();       
        }
    Cancel();    
    }


CCallbackTimer::CCallbackTimer()
: CTimer(CActive::EPriorityStandard)
    {
    
    }

CCallbackTimer* CCallbackTimer::NewL()
    {
    CCallbackTimer* self= new(ELeave) CCallbackTimer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }
    
void CCallbackTimer::ConstructL()    
    {
    CActiveScheduler::Add(this);
    CTimer::ConstructL();    
    }

void CCallbackTimer::StartWaitingTillCallbackOrTimeOutL()
    {
    After(TTimeIntervalMicroSeconds32(1000000));
    iWait.Start(); // can fallback to TestCombinationL       
    }

void CCallbackTimer::CallbackReceived()
    {
    Cancel();
    iWait.AsyncStop();    
    }
    
void CCallbackTimer::RunL()
    {
    iWait.AsyncStop(); // can fallback to TestCombinationL
    }
    


