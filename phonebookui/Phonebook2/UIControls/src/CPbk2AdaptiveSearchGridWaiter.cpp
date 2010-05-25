/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 Adaptive Search Grid
*               helper class
*/

//  INCLUDES
#include <Pbk2ProcessDecoratorFactory.h>
#include <Pbk2UIControls.rsg>
#include <AknWaitDialog.h>
#include "CPbk2AdaptiveSearchGridWaiter.h"

const TInt KDelay = 500000; // 0.5 seconds

CPbk2AdaptiveSearchGridWaiter* CPbk2AdaptiveSearchGridWaiter::NewL
        ( MPbk2SearchGridWaiterObserver& aObserver )
    {
    CPbk2AdaptiveSearchGridWaiter* self = 
        new ( ELeave ) CPbk2AdaptiveSearchGridWaiter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CPbk2AdaptiveSearchGridWaiter::~CPbk2AdaptiveSearchGridWaiter()
    {
    Cancel();
    delete iWaitNote;
    iWaitNote = NULL;
    }

CPbk2AdaptiveSearchGridWaiter::CPbk2AdaptiveSearchGridWaiter
        ( MPbk2SearchGridWaiterObserver& aObserver )
        : CTimer( CActive::EPriorityStandard ), iObserver( aObserver )
    {
    
    }

void CPbk2AdaptiveSearchGridWaiter::ConstructL()
    {
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }

void CPbk2AdaptiveSearchGridWaiter::Stop()
    {
    Cancel();
    DismissWaitNote();
    }

void CPbk2AdaptiveSearchGridWaiter::Start()
    {
    if ( !IsActive() )
        {
        Stop();
        After( TTimeIntervalMicroSeconds32( KDelay ) );
        }
    }

void CPbk2AdaptiveSearchGridWaiter::RunL()
    {
    if( !iStarted && iWaitNote == NULL )
        { 
        iObserver.GridDelayCompleteL();
        
        iWaitNote = new(ELeave) 
            CAknWaitDialog(reinterpret_cast<CEikDialog**>( &iWaitNote ), ETrue );
        iWaitNote->SetCallback( this );
        iWaitNote->ExecuteLD( R_QTN_GEN_NOTE_CHECKING_PROGRESS );
        iStarted = ETrue;
        }
    }

TInt CPbk2AdaptiveSearchGridWaiter::RunError( TInt /*aError*/ )
    {
    iStarted = EFalse;
    DismissWaitNote();
    return KErrNone;
    }

void CPbk2AdaptiveSearchGridWaiter::DialogDismissedL( TInt /*aButtonId*/ )
    {
    iStarted = EFalse;
    iObserver.WaitNoteDismissed();
    }

void CPbk2AdaptiveSearchGridWaiter::DismissWaitNote()
    {
    if ( iWaitNote )
        {
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if ( err != KErrNone )
            {
            delete iWaitNote;
            iWaitNote = NULL;
            }
        }
    }
// End of File
