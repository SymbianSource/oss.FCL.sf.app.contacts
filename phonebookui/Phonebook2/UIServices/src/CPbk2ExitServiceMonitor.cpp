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
* Description:  Phonebook 2 UI Services exit service monitor.
*
*/


#include "CPbk2ExitServiceMonitor.h"

// Phonebook 2
#include "MPbk2ConnectionObserver.h"
#include "RPbk2UIService.h"

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::CPbk2ExitServiceMonitor
// --------------------------------------------------------------------------
//
CPbk2ExitServiceMonitor::CPbk2ExitServiceMonitor
        ( RPbk2UIService& aPbk2AppService,
          MPbk2ConnectionObserver& aObserver ) :
            CActive( EPriorityStandard ),
            iPbk2AppService( aPbk2AppService ),
            iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::~CPbk2ExitServiceMonitor
// --------------------------------------------------------------------------
//
CPbk2ExitServiceMonitor::~CPbk2ExitServiceMonitor()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2ExitServiceMonitor* CPbk2ExitServiceMonitor::NewL
        ( RPbk2UIService& aPbk2AppService,
          MPbk2ConnectionObserver& aObserver )
    {
    CPbk2ExitServiceMonitor* self = new ( ELeave ) CPbk2ExitServiceMonitor
        ( aPbk2AppService, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ExitServiceMonitor::ConstructL()
    {
    iPbk2AppService.ExitRequestL( iStatus, iExitCommandId );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2ExitServiceMonitor::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        // Ask it is ok to exit the server application
        TBool okToExit = iObserver.OkToExitL
            ( iExitCommandId, EPbk2ExitServiceApplication );

        // Always complete the notification protocol
        iPbk2AppService.ExitServiceL( okToExit, iExitCommandId );

        // Always request new notification
        iPbk2AppService.ExitRequestL( iStatus, iExitCommandId );
        SetActive();
        }
    else
        {
        User::Leave( iStatus.Int() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ExitServiceMonitor::RunError( TInt aError )
    {
    return aError;
    }

// --------------------------------------------------------------------------
// CPbk2ExitServiceMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ExitServiceMonitor::DoCancel()
    {
    iPbk2AppService.CancelExitRequest();
    }

// End of File
