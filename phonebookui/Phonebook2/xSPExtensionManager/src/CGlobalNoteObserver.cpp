/*
* Copyright (c) 2002 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Generic global note events observer.
*
*/


// INCLUDE FILES
#include "CGlobalNoteObserver.h"
#include "MGlobalNoteObserver.h"

#include <basched.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::CGlobalNoteObserver
// C++ constructor
// -----------------------------------------------------------------------------
//
CGlobalNoteObserver::CGlobalNoteObserver( MGlobalNoteObserver* aObserver )
    : CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlobalNoteObserver* CGlobalNoteObserver::NewL( MGlobalNoteObserver* aObserver )
    {
    CGlobalNoteObserver* self = new(ELeave) CGlobalNoteObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlobalNoteObserver::ConstructL()
    {
    if( !iObserver )
        {
        User::Leave( KErrArgument );
        }
    CActiveScheduler::Add( this );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::~CGlobalNoteObserver
// Destructor
// -----------------------------------------------------------------------------
//
CGlobalNoteObserver::~CGlobalNoteObserver()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::DoCancel
// -----------------------------------------------------------------------------
//
void CGlobalNoteObserver::DoCancel()
    {
    // Currently there is no need to be able to cancel the note; we can just
    // wait that the user dismisses it.
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::RunL
// -----------------------------------------------------------------------------
//
void CGlobalNoteObserver::RunL()
    {
    iObserver->GlobalNoteClosed( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CGlobalNoteObserver::RunError
// -----------------------------------------------------------------------------
//
TInt CGlobalNoteObserver::RunError( TInt aError )
    {
    // Pass on leave code from application exit
    if( aError != KLeaveExit )
        {
        return KLeaveExit;
        }

    iObserver->GlobalNoteError( aError );

    // Error handled, return KErrNone
    return KErrNone;
    }

//  End of File
