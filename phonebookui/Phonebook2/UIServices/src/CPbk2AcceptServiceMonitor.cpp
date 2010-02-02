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
* Description:  Phonebook 2 UI Services accept service monitor.
*
*/


#include "CPbk2AcceptServiceMonitor.h"

// Phonebook 2
#include "MPbk2ConnectionObserver.h"
#include "RPbk2UIService.h"

// Unnamed namespace for local definitions
namespace {

// This is just an assumption, but in the unlikely case that this is not
// enough the server side will complete the message with KErrOverflow
const TInt KMaxLinkLength = 512;

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::CPbk2AcceptServiceMonitor
// --------------------------------------------------------------------------
//
CPbk2AcceptServiceMonitor::CPbk2AcceptServiceMonitor(
        RPbk2UIService& aPbk2AppService,
        MPbk2ConnectionObserver& aObserver ) :
            CActive( EPriorityStandard ),
            iPbk2AppService( aPbk2AppService ),
            iObserver(aObserver)
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::~CPbk2AcceptServiceMonitor
// --------------------------------------------------------------------------
//
CPbk2AcceptServiceMonitor::~CPbk2AcceptServiceMonitor()
    {
    Cancel();
    delete iLinkBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2AcceptServiceMonitor* CPbk2AcceptServiceMonitor::NewL
        ( RPbk2UIService& aPbk2AppService,
          MPbk2ConnectionObserver& aObserver )
    {
    CPbk2AcceptServiceMonitor* self =
        new ( ELeave ) CPbk2AcceptServiceMonitor
            ( aPbk2AppService, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AcceptServiceMonitor::ConstructL()
    {
    iLinkBuffer = HBufC8::NewL( KMaxLinkLength );
    iPbk2AppService.AcceptRequestL
        ( iStatus, iNumContacts, *iLinkBuffer );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2AcceptServiceMonitor::RunL()
    {
    switch ( iStatus.Int() )
        {
        case KErrNone:
            {
            // Ask is the output acceptable
            TBool accept =
                iObserver.AcceptSelectionL( iNumContacts, *iLinkBuffer );

            // Always request new notification
            iPbk2AppService.AcceptRequestL
                ( iStatus, iNumContacts, *iLinkBuffer );
            SetActive();

            // Always complete the notification protocol
            iPbk2AppService.AcceptServiceL
                ( accept, iLinkBuffer );
            break;
            }
        case KErrServerTerminated:
            {
            break;
            }

        default:
            {
            User::Leave( iStatus.Int() );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2AcceptServiceMonitor::RunError( TInt aError )
    {
    TRAPD( err, iObserver.OperationErrorL( aError ) );
    return err;
    }

// --------------------------------------------------------------------------
// CPbk2AcceptServiceMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AcceptServiceMonitor::DoCancel()
    {
    iPbk2AppService.CancelAcceptRequest();
    }

// End of File
