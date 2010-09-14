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
* Description:  Phonebook2  helper class
*              
*/

//  INCLUDES
#include <Pbk2ProcessDecoratorFactory.h>
#include <pbk2uicontrols.rsg>
#include <avkon.hrh>
#include "CPbk2DelayedWaitNote.h"

#include <coemain.h> // For EActivePriorityClockTimer

const TInt KDelay = 500000; // 0.5 seconds

CPbk2DelayedWaitNote* CPbk2DelayedWaitNote::NewL
        ( MPbk2ProcessDecoratorObserver& aObserver, TInt aResourceId )
    {
    CPbk2DelayedWaitNote* self = 
        new ( ELeave ) CPbk2DelayedWaitNote( aObserver, aResourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CPbk2DelayedWaitNote::~CPbk2DelayedWaitNote()
    {
    Cancel();
    delete iDecorator;
    iDecorator = NULL;
    }

CPbk2DelayedWaitNote::CPbk2DelayedWaitNote
        ( MPbk2ProcessDecoratorObserver& aObserver, TInt aResourceId )
        : CTimer( EActivePriorityClockTimer ), iObserver( aObserver ), 
          iResourceId( aResourceId )
    {
    
    }

void CPbk2DelayedWaitNote::ConstructL()
    {
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }

void CPbk2DelayedWaitNote::Stop()
    {
    Cancel();
    DismissWaitNote();
    }

void CPbk2DelayedWaitNote::Start()
    {
    if ( !IsActive() && !iStarted )
        {
        After( TTimeIntervalMicroSeconds32( KDelay ) );
        }
    }

void CPbk2DelayedWaitNote::RunL()
    {   
    if( !iStarted && !iDecorator )
        {   
        iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
                  ( iResourceId, EFalse );
        iDecorator->SetObserver( *this );
        iDecorator->ProcessStartedL( 0 ); // wait note doesn't care about amount
        iStarted = ETrue;
        }
    }

void CPbk2DelayedWaitNote::ProcessDismissed( TInt aCancelCode )
    {
    iStarted = EFalse;
    iObserver.ProcessDismissed( aCancelCode );
    }

TInt CPbk2DelayedWaitNote::RunError( TInt /*aError*/ )
    {
    iStarted = EFalse;
    DismissWaitNote();
    return KErrNone;
    }

void CPbk2DelayedWaitNote::DismissWaitNote()
    {
    if ( iDecorator )
        {
        TRAPD( err, iDecorator->ProcessStopped() );
        if ( err != KErrNone )
            {
            delete iDecorator;
            iDecorator = NULL;
            }
        }
    else 
        {
        iObserver.ProcessDismissed( EAknSoftkeyDone );
        }   
    }
// End of File
