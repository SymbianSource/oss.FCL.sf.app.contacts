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
* Description:  Phonebook 2 UI Service server application connection.
*
*/


#include "CPbk2ServerAppConnection.h"

// Phonebook 2
#include "MPbk2ConnectionObserver.h"
#include "CPbk2ExitServiceMonitor.h"
#include "CPbk2AcceptServiceMonitor.h"
#include <Pbk2InternalUID.h>

// System includes
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::CPbk2ServerAppConnection
// --------------------------------------------------------------------------
//
CPbk2ServerAppConnection::CPbk2ServerAppConnection
        ( MPbk2ConnectionObserver& aObserver ) :
            CActive( EPriorityStandard ),
            iObserver( aObserver )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::CPbk2ServerAppConnection()") );
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::~CPbk2ServerAppConnection
// --------------------------------------------------------------------------
//
CPbk2ServerAppConnection::~CPbk2ServerAppConnection()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::~CPbk2ServerAppConnection()") );
    Cancel();
    delete iArrayResults;
    delete iFieldResults;
    DisconnectMonitors();
    DisconnectServerApplication();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerAppConnection* CPbk2ServerAppConnection::NewL
        ( MPbk2ConnectionObserver& aObserver )
    {
    CPbk2ServerAppConnection* self =
        new ( ELeave ) CPbk2ServerAppConnection( aObserver );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::LaunchAssignL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::LaunchAssignL
        ( HBufC8* aConfigurationPackage, HBufC8* aDataPackage,
          HBufC8* aAssignInstructions )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::LaunchAssignL()") );
    // Need to trap, because consumer freezes if ConnectL function leaves.
    // Reason for the freeze is that connection stays open.
    TRAPD( error, ConnectL() );
    if ( error == KErrNone )
        {
        iPbk2AppService.LaunchAssignL(
            iStatus,  aConfigurationPackage, aDataPackage,
            aAssignInstructions, iCanceled );
        SetActive();
        }
    else
        {
        DisconnectMonitors();
        DisconnectServerApplication();
        User::Leave( error );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::LaunchAttributeAssignL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::LaunchAttributeAssignL
    ( HBufC8* aConfigurationPackage,
      TPbk2AttributeAssignData& aAttributePackage,
      HBufC8* aAssignInstructions )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::LaunchAttributeAssignL()") );
    ConnectL();
    iPbk2AppService.LaunchAttributeAssignL
        ( iStatus,  aConfigurationPackage, aAttributePackage,
          aAssignInstructions, iCanceled );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::LaunchAttributeUnassignL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::LaunchAttributeUnassignL
    ( HBufC8* aConfigurationPackage,
      TPbk2AttributeAssignData aAttributePackage,
      HBufC8* aAssignInstructions )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::LaunchAttributeUnassignL()") );
    ConnectL();
    iPbk2AppService.LaunchAttributeUnassignL
        ( iStatus,  aConfigurationPackage, aAttributePackage,
          aAssignInstructions, iCanceled );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::CancelAssign
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::CancelAssign()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::CancelAssign()") );
    if ( iPbk2AppService.Handle() != KNullHandle )
        {
        iPbk2AppService.CancelAssign( iCanceled );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::LaunchFetchL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::LaunchFetchL
        ( HBufC8* aConfigurationPackage, HBufC8* aFetchInstructions )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::LaunchFetchL()") );
    //AIW Cancel command uses CancelFetch and leaves AO active,
    //call here Cancel() to avoid using active AO (E32User-CBase 42)
    Cancel();
    // Need to trap, because consumer freezes if ConnectL function leaves.
    // Reason for the freeze is that connection stays open.
    TRAPD( error, ConnectL() );

    if ( error == KErrNone )
        {
        iPbk2AppService.LaunchFetchL
            ( iStatus, aConfigurationPackage,
              aFetchInstructions, iCanceled );
        SetActive();
        }
    else
        {
        DisconnectMonitors();
        DisconnectServerApplication();
        User::Leave( error );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::CancelFetch
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::CancelFetch()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::CancelFetch()") );
    if ( iPbk2AppService.Handle() != KNullHandle )
        {
        iPbk2AppService.CancelFetch( iCanceled );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::RunL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::RunL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::RunL(%d)"), iStatus.Int() );
    switch( iStatus.Int() )
        {
        case KErrNone:
            {
            GetResultsL();
            iObserver.OperationCompleteL( *iArrayResults, iExtraResultData,
                *iFieldResults );
            DisconnectMonitors();
            DisconnectServerApplication();
            break;
            }
        case KErrCancel:            // FALLTHROUGH
        case KErrServerTerminated:
            {
            iObserver.OperationCanceledL();
            DisconnectMonitors();
            DisconnectServerApplication();
            break;
            }
        case KErrAbort:
            {
            // Abort is like completion with KErrCancel,
            // expect that we leave the server application alive
            // and wait for it to close itself down and send
            // HandleServerAppExit notification
            iObserver.OperationCanceledL();
            DisconnectMonitors();
            break;
            }
        default:
            {
            // Delegate to RunError
            User::Leave( iStatus.Int() );
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ServerAppConnection::RunError( TInt aError )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::RunError(%d)"), aError );
    TRAPD( err,  iObserver.OperationErrorL( aError ) );
    DisconnectMonitors();
    DisconnectServerApplication();
    return err;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::DoCancel()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::DoCancel()") );
    // Cancels outstanding notification monitors
    if ( iExitServiceMonitor )
        {
        iExitServiceMonitor->Cancel();
        }
    if ( iAcceptServiceMonitor )
        {
        iAcceptServiceMonitor->Cancel();
        }

    // Cancels outstanding operations
    CancelAssign();
    CancelFetch();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::HandleServerAppExit
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::HandleServerAppExit( TInt aReason )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2ServerAppConnection::HandleServerAppExit(%d)"), aReason );
    TBool okToExit = EFalse;

    // Query the observer whether it is ok to exit application
    // (the consumer process)
    TRAPD( err,
        okToExit = iObserver.OkToExitL( aReason, EPbk2ExitApplication ) );

    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }

    if (okToExit)
        {
        MAknServerAppExitObserver::HandleServerAppExit( aReason );
        }
    else
        {
        DisconnectServerApplication();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::ConnectL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::ConnectL()
    {
    if ( iPbk2AppService.Handle() == KNullHandle )
        {
        iPbk2AppService.ConnectChainedAppL
            ( TUid::Uid( KPbk2ServerAppUID3 ) );

        delete iExitAppServerMonitor;
        iExitAppServerMonitor = NULL;
        iExitAppServerMonitor = CApaServerAppExitMonitor::NewL
            ( iPbk2AppService, *this, CActive::EPriorityStandard );

        delete iExitServiceMonitor;
        iExitServiceMonitor = NULL;
        iExitServiceMonitor = CPbk2ExitServiceMonitor::NewL
            ( iPbk2AppService, iObserver );

        delete iAcceptServiceMonitor;
        iAcceptServiceMonitor = NULL;
        iAcceptServiceMonitor = CPbk2AcceptServiceMonitor::NewL
            ( iPbk2AppService, iObserver );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::GetResultsL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::GetResultsL()
    {
    // Important to delete old results
    delete iArrayResults;
    iArrayResults = NULL;
    delete iFieldResults;
    iFieldResults = NULL;

    iExtraResultData = KErrNotSupported;

    // First get the result size
    TInt arraySize = 0;
    TInt fieldSize = 0;
    iPbk2AppService.GetResultSizeL( arraySize, fieldSize );
    iArrayResults = HBufC8::NewL( arraySize );
    iFieldResults = HBufC::NewL( fieldSize );

    // Then get the results
    iPbk2AppService.GetResultsL( *iArrayResults, iExtraResultData,
        *iFieldResults );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::DisconnectMonitors
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::DisconnectMonitors()
    {
    delete iExitServiceMonitor;
    iExitServiceMonitor = NULL;
    delete iAcceptServiceMonitor;
    iAcceptServiceMonitor = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppConnection::DisconnectServerApplication
// --------------------------------------------------------------------------
//
void CPbk2ServerAppConnection::DisconnectServerApplication()
    {
    delete iExitAppServerMonitor;
    iExitAppServerMonitor = NULL;
    iPbk2AppService.Close();
    }

// End of File
