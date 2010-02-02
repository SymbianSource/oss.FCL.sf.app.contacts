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
* Description:  Phonebook2 find delay
*
*/


// INCLUDE FILES
#include "CPbk2FindDelay.h"
#include "MPbk2FindDelayObserver.h"


// ================= MEMBER FUNCTIONS =======================
// --------------------------------------------------------------------------
// CPbk2FindDelay::CPbk2FindDelay
// --------------------------------------------------------------------------
//
inline CPbk2FindDelay::CPbk2FindDelay(
        MPbk2FindDelayObserver& aObserver ) :
    CTimer( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FindDelay::~CPbk2FindDelay
// --------------------------------------------------------------------------
//
CPbk2FindDelay::~CPbk2FindDelay()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2FindDelay::NewL
// --------------------------------------------------------------------------
//
CPbk2FindDelay* CPbk2FindDelay::NewL(
        MPbk2FindDelayObserver& aObserver )
    {
    CPbk2FindDelay* self = new (ELeave) CPbk2FindDelay( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FindDelay::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FindDelay::ConstructL()
    {
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2FindDelay::RunL
// --------------------------------------------------------------------------
//                
void CPbk2FindDelay::RunL()
    {
    iObserver.FindDelayComplete();
    }
    
//  End of File
