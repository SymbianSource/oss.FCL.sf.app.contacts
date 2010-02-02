/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCA client's connecting class.
*
*/

#include <e32std.h>
#include <s32mem.h>
#include <aknappui.h>

#include "ccaclientheaders.h"

/// Unnamed namespace for local definitions
namespace {

const TInt KCCAStreamBufferSize = 256;
const TInt KCCAConnectionMaxRetriesForLaunchKErrInUse = 5;
const TInt KCCAConnectionRelaunchDelay = 0.2 * 1000000;// 0.2 sec

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_DoLaunchL = 1,
    EPanicPreCond_DoLaunchL2,
    EPanicPreCond_DoLaunchL3
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CCCAConnection");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace



// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCCAConnection::NewL
// ----------------------------------------------------------
//
CCCAConnection* CCCAConnection::NewL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::NewL"));

    CCCAConnection* self = new (ELeave) CCCAConnection();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CCCAConnection::CCCAConnection
// ----------------------------------------------------------
//
CCCAConnection::CCCAConnection() :
    CActive( EPriorityStandard ),
    iAppStatus( EAppClosed ),
    iMsgStreamPtr( NULL, 0 )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::CCCAConnection"));
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------
// CCCAConnection::ConstructL
// ----------------------------------------------------------
//
void CCCAConnection::ConstructL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::ConstructL"));
    iSchedulerWait = new ( ELeave ) CActiveSchedulerWait();
    }

// ----------------------------------------------------------
// CCCAConnection::~CCCAConnection
// ----------------------------------------------------------
//
CCCAConnection::~CCCAConnection()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::~CCCAConnection"));

    Cancel();
    delete iSchedulerWait;
    delete iMsgStream;
    DisconnectServerApplication();
    CloseParameter();
    }

// ----------------------------------------------------------
// CCCAConnection::Close
// ----------------------------------------------------------
//
void CCCAConnection::Close()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::Close"));
    delete this;
    }

// --------------------------------------------------------------------------
// CCCAConnection::RunL
// --------------------------------------------------------------------------
//
void CCCAConnection::RunL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::RunL: iStatus: %d"), iStatus.Int());
    if ( iSchedulerWait->IsStarted() )
        {
        iSchedulerWait->AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// CCCAConnection::DoCancel
// --------------------------------------------------------------------------
//
void CCCAConnection::DoCancel()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::DoCancel"));
    if ( iSchedulerWait->IsStarted() )
        {
        iSchedulerWait->AsyncStop();
        }
    }

// ----------------------------------------------------------
// CCCAConnection::LaunchApplicationL
// ----------------------------------------------------------
//
void CCCAConnection::LaunchApplicationL( MCCAParameter& aParameter )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::LaunchApplicationL"));
    
    //Only one active connection per time
    __ASSERT_ALWAYS( iAppStatus == EAppClosed, User::Leave( KErrAlreadyExists ));
    
    iAppStatus = EAppLaunching;

    //PERFORMANCE LOGGING: 1. Starting AppServer (StartPoint)
    WriteToPerfLog();
    WriteToPerfLaunchLog(_L(" CCA launched"));

    // Need to trap, because consumer freezes if ConnectL function leaves.
    // Reason for the freeze is that connection stays open.
    TRAPD( error, ConnectServerApplicationL() );
    if ( KErrNone != error )
        {
        CCA_DP(KCCAClientLoggerFile, CCA_L( "::LaunchApplicationL: Error connect: %d"), error);
        DisconnectServerApplication();
        User::Leave( error );
        }

    DoLaunchL( aParameter );
    SetActive();
    iSchedulerWait->Start();

    error = iStatus.Int();
    if ( KErrNone != error )
        {
        CCA_DP(KCCAClientLoggerFile, CCA_L( "::LaunchApplicationL: Error launch: %d"), error);
        DisconnectServerApplication();
        User::Leave( error );
        }

    iAppStatus = EAppRunning;

    CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::LaunchApplicationL"));
    }

// ----------------------------------------------------------
// CCCAConnection::LaunchAppL
// ----------------------------------------------------------
//
void CCCAConnection::LaunchAppL(
    MCCAParameter& aParameter,
    MCCAObserver* aObserver )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::LaunchAppL"));
    
    if (iAppStatus == EAppClosed)
        {
        LaunchApplicationL( aParameter );

        CloseParameter();
        iParameter = static_cast<CCCAParameter*>( &aParameter );
        iObserver = aObserver;
        }
    
    CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::LaunchAppL"));
    }

// --------------------------------------------------------------------------
// CCCAConnection::CloseAppL
// --------------------------------------------------------------------------
//
void CCCAConnection::CloseAppL()
    {
    if ( !iClosePending ) //To protect from duplicate close attempts
        {
        iAppService.DoCloseApplication(); 
        iClosePending = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CCCAConnection::HandleServerAppExit
// --------------------------------------------------------------------------
//
void CCCAConnection::HandleServerAppExit( TInt aReason )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::HandleServerAppExit"));
    CCA_DP(KCCAClientLoggerFile, CCA_L( "::HandleServerAppExit: aReason: %d"), aReason);

    DisconnectServerApplication();

	TInt flags(0);
	if(iParameter)
	    {
	    flags = iParameter->ConnectionFlag();
	    }
	
    if ( iObserver )
        {
        CCA_DP(KCCAClientLoggerFile, CCA_L( "::HandleServerAppExit - informing observer about exit"));
        CloseParameter();
        TRAP_IGNORE( iObserver->CCASimpleNotifyL( MCCAObserver::EExitEvent, aReason ));
        iObserver = NULL;
        }

    //Kill us if not soft exit requested
    if(!(flags & MCCAParameter::ESoftExit))    
        {
		//Kill this process, i.e. the application running ccaclient
	    // Following call ends application if aReason == EAknCmdExit (and does nothing else)
        MAknServerAppExitObserver::HandleServerAppExit( aReason );
        }

   iClosePending = EFalse; 
   CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::HandleServerAppExit"));
    }

// --------------------------------------------------------------------------
// CCCAConnection::DisconnectServerApplication
// --------------------------------------------------------------------------
//
void CCCAConnection::DisconnectServerApplication()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::DisconnectServerApplication"));

    delete iMonitor;
    iMonitor = NULL;
    iAppService.Close();
    iAppStatus = EAppClosed;

    CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::DisconnectServerApplication"));
    }

// --------------------------------------------------------------------------
// CCCAConnection::ConnectServerApplicationL
// --------------------------------------------------------------------------
//
void CCCAConnection::ConnectServerApplicationL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::ConnectServerApplicationL"));

    /*
     * In cases when the CCApplication is running on background and connection
     * is closed and reopened instantly, the opening of connection returns
     * sometimes KErrInUse. One solution could be to arrange Rendezvous-signals
     * between processes, but to keep implementation somewhat simple, launch is
     * now just tried again couple of times with small delay between.
     */
    TInt error = KErrInUse;
    for ( TInt i = 0; i < KCCAConnectionMaxRetriesForLaunchKErrInUse && 
            (KErrNotFound == error || KErrInUse == error); i++ )
        {
        TRAP( error, iAppService.DoConnectL());
        CCA_DP(KCCAClientLoggerFile, CCA_L( "::ConnectServerApplicationL: error: %d i: %d"), error, i );
        if ( KErrInUse == error &&
             i < KCCAConnectionMaxRetriesForLaunchKErrInUse - 1 )
            {
            User::After( KCCAConnectionRelaunchDelay );
            }
        }
    User::LeaveIfError( error );

    delete iMonitor;
    iMonitor = NULL;
    iMonitor = CApaServerAppExitMonitor::NewL(
        iAppService, *this, CActive::EPriorityStandard );

    CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::ConnectServerApplicationL"));
    }

// --------------------------------------------------------------------------
// CCCAConnection::DoLaunchL
// --------------------------------------------------------------------------
//
void CCCAConnection::DoLaunchL( MCCAParameter& aParameter )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "->CCCAConnection::DoLaunchL"));
    __ASSERT_DEBUG( &aParameter, Panic(EPanicPreCond_DoLaunchL) );

    CCCAParameter& param = STATIC_CAST(CCCAParameter&, aParameter);
    CBufSeg* buf1 = CBufSeg::NewL( KCCAStreamBufferSize );
    CleanupStack::PushL( buf1 );

    RBufWriteStream writeStream( *buf1 );
    CleanupClosePushL( writeStream );
    param.ExternalizeL( writeStream );
    writeStream.CommitL();

    delete iMsgStream;
    iMsgStream = NULL;
    iMsgStreamPtr.Set( NULL, 0, 0 );
    
    const TInt neededSize = buf1->Size();
    iMsgStream = HBufC8::NewL( neededSize );
    iMsgStreamPtr.Set( iMsgStream->Des() );    
    buf1->Read( 0, iMsgStreamPtr, neededSize );
    CleanupStack::PopAndDestroy(2); //buf1, writeStream

    __ASSERT_DEBUG( 0 < iMsgStreamPtr.Size(), Panic(EPanicPreCond_DoLaunchL3) );

    iStatus = KRequestPending;
    iAppService.DoLaunch( iStatus, iMsgStreamPtr );

    CCA_DP(KCCAClientLoggerFile, CCA_L( "<-CCCAConnection::DoLaunchL"));
    }

// ----------------------------------------------------------
// CCCAConnection::CloseParameter
// ----------------------------------------------------------
//
void CCCAConnection::CloseParameter( )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAConnection::CloseParameter"));
    if ( iParameter )
        {
        iParameter->Close();
        iParameter = NULL;
        }
    }

// End of file
