/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer.
*
*/


// INCLUDE FILES
#include    "CLogsTimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLogsTimer::CLogsTimer
// ---------------------------------------------------------
//
CLogsTimer::CLogsTimer( TInt aPriority ) 
    : CTimer( aPriority)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CLogsTimer::ConstructL
// ---------------------------------------------------------
//
void CLogsTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------
// CLogsTimer::NewL
// ---------------------------------------------------------
//
CLogsTimer* CLogsTimer::NewL( TInt aPriority )
    {
    CLogsTimer* self = new (ELeave) CLogsTimer( aPriority );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ---------------------------------------------------------
// CLogsTimer::~CLogsTimer
// ---------------------------------------------------------
//
CLogsTimer::~CLogsTimer()
    {
    CTimer::Cancel();    
    }

// ---------------------------------------------------------
// CLogsTimer::RunL()
// ---------------------------------------------------------
//
void CLogsTimer::RunL()
    {
    if ( iStatus != KErrNone )
        {
        //error code is ignored, as CPeriodic. 
        return;
        }
   
    if( iTimerObserver == NULL )
        {
        iCallBack.CallBack();
        }
    else
        {
        iTimerObserver->HandleLogsTimerL( iTimerObserverParam );
        }
    }

// ---------------------------------------------------------
// CLogsTimer::After()
// ---------------------------------------------------------
//
void CLogsTimer::After( TTimeIntervalMicroSeconds32 anInterval, 
                        TCallBack aCallBack )
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iTimerObserver = NULL;
    iCallBack = aCallBack;
    CTimer::After( anInterval );
    }

// ---------------------------------------------------------
// CLogsTimer::After()
// ---------------------------------------------------------
//
void CLogsTimer::After( TTimeIntervalMicroSeconds32 anInterval, 
                        MLogsTimer* aObserver,
                        TAny* aPtr )
    {
    if ( IsActive() )
        {
        Cancel();
        }
        
    iTimerObserver = aObserver;
    iTimerObserverParam = aPtr;
    CTimer::After( anInterval );
    }

//  End of File  

