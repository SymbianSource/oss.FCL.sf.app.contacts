/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Class for log clearing
*
*/


// INCLUDE FILES
#include <logcli.h>
#include <logview.h>
#include <sysutil.h>

#include "CLogsClearLog.h"
#include "MLogsObserver.h"
#include "LogsUID.h"

#include "LogsDebug.h"
#include "LogsTraces.h"

// CONSTANTS
_LIT( KMaxTime, "99991130:235959.999999");
const TInt KBytesToRequest ( 64 * 1024 );


// ----------------------------------------------------------------------------
// CLogsClearLog::NewL
// ----------------------------------------------------------------------------
//
CLogsClearLog* CLogsClearLog::NewL( 
    RFs& aFsSession,
    MLogsObserver* aObserver )
    {
    TRACE_ENTRY_POINT;
    
    CLogsClearLog* self = new (ELeave) CLogsClearLog( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::CLogsClearLog
// ----------------------------------------------------------------------------
//
CLogsClearLog::CLogsClearLog( 
    RFs& aFsSession, MLogsObserver* aObserver ) :
        CActive( EPriorityStandard ),
        iFsSession( aFsSession ),
        iObserver( aObserver ),
        iState( EStateUndefined )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsClearLog::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iLogClient = CLogClient::NewL( iFsSession );
    CActiveScheduler::Add( this );
    TRACE_EXIT_POINT; 
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::~CLogsClearLog
// ----------------------------------------------------------------------------
//
CLogsClearLog::~CLogsClearLog()
    {
    TRACE_ENTRY_POINT;
    Cancel();     
    delete iViewRecent;
    delete iLogClient;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsClearLog::SetObserver( MLogsObserver* aObserver )
    {
    TRACE_ENTRY_POINT;
    iObserver = aObserver;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsClearLog::DoCancel()
    { 
    TRACE_ENTRY_POINT;
    
    if( iViewRecent )
        {
        iViewRecent->Cancel();
        }
        
    if( iLogClient )
        {
        iLogClient->Cancel();
        }
        
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CLogsClearLog::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsClearLog::State() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iState;
    }


// ----------------------------------------------------------------------------
// CLogsClearLog::ClearModelL
// ----------------------------------------------------------------------------
//
void CLogsClearLog::ClearModelL( const TLogsModel aModel )
    {
    TRACE_ENTRY_POINT;
    TTime time( KMaxTime );

    iPhase = EClearLog;

    if( !IsActive() )
        { 
        // If disk space below critical, we will leave here and a global error note is
        // to be shown
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, KBytesToRequest ) )
            {       
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                ( "CLogsClearLog::ClearModelL - FFSSpaceBelowCriticalLevelL" )); 
            // don't show any own notes here 
            User::Leave( KErrDiskFull );
            }   

        
        switch( aModel )
            {
            case ELogsMainModel:
                iLogClient->ClearLog( time, iStatus );
                break;

            case ELogsAllRecent: // fall through
            case ELogsReceivedModel:
            case ELogsDialledModel:
            case ELogsMissedModel:
            default:                
                iLogClient->ClearLog( aModel, iStatus );
                break;
            }
            
        SetActive();
        }
        
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::DeleteEventL
// ----------------------------------------------------------------------------
//
void CLogsClearLog::DeleteEventL( TLogId aId )
    {
    TRACE_ENTRY_POINT;
    iPhase = EClearEvent;

    if( iViewRecent )
        {
        delete iViewRecent;
        iViewRecent = NULL;
        }
        
    iViewRecent = CLogViewRecent::NewL( *iLogClient );
    iViewRecent->RemoveL( aId );
    iState = EStateClearLogFinished;
    
    if( iObserver )
        {
        iObserver->StateChangedL( this );
        }
        
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::RunL
// ----------------------------------------------------------------------------
//
void CLogsClearLog::RunL()
    {
    TRACE_ENTRY_POINT;  
        
    iState = EStateClearLogFinished;
    
    if( iObserver )
        {
        iObserver->StateChangedL( this );
        }
        
    TRACE_EXIT_POINT;     
    }

// ----------------------------------------------------------------------------
// CLogsClearLog::RunError
// ----------------------------------------------------------------------------
//
TInt CLogsClearLog::RunError(TInt aError)
	{
	TRACE_ENTRY_POINT;
	TInt ret;
	
	if( aError == KErrAccessDenied )
		{
		ret = KErrNone;
		}
	else
		{
		ret = aError;
		}
		
	TRACE_EXIT_POINT; 
	return ret;	
	}
