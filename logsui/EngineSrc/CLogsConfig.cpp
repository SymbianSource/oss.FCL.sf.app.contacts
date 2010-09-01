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
*     Log configuration setter / getter
*
*/


// INCLUDE FILES
#include "CLogsConfig.h"
#include "MLogsObserver.h"
#include "LogsUID.h"

#include "LogsDebug.h"
#include "LogsTraces.h"

// CONSTANTS
const TInt KBytesToRequest ( 64 * 1024 );

// ----------------------------------------------------------------------------
// CLogsConfig::NewL
// ----------------------------------------------------------------------------
//
CLogsConfig* CLogsConfig::NewL( 
    RFs& aFsSession, 
    MLogsObserver* aObserver )                
    {
    TRACE_ENTRY_POINT;
    CLogsConfig* self = new (ELeave) CLogsConfig( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::CLogsConfig
// ----------------------------------------------------------------------------
//
CLogsConfig::CLogsConfig( 
    RFs& aFsSession, 
    MLogsObserver* aObserver ) : 
        CActive( EPriorityStandard ),
        iFsSession( aFsSession ),
        iObserver( aObserver ),
        iState( EStateUndefined )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::~CLogsConfig
// ----------------------------------------------------------------------------
//
CLogsConfig::~CLogsConfig()
    {
    TRACE_ENTRY_POINT;
    Cancel(); 
    delete iLogClient;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsConfig::DoCancel()
    {
    TRACE_ENTRY_POINT;
    iFsSession.ReleaseReserveAccess( EDriveC );
    iLogClient->Cancel();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsConfig::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iLogClient = CLogClient::NewL( iFsSession );
    CActiveScheduler::Add( this ); 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::RunL
// ----------------------------------------------------------------------------
//
void CLogsConfig::RunL()
    {  
    TRACE_ENTRY_POINT;
    iFsSession.ReleaseReserveAccess( EDriveC );
    
    if( iObserver )
        {
        iState = EStateFinished;
        iObserver->StateChangedL( this );     
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::RunError
// ----------------------------------------------------------------------------
//
TInt CLogsConfig::RunError(TInt aError)
	{
	TRACE_ENTRY_POINT;
	if( aError == KErrAccessDenied )
		{
		TRACE_EXIT_POINT;
		return KErrNone;
		}
	else
		{
		TRACE_EXIT_POINT;
		return aError;
		}
	}

// ----------------------------------------------------------------------------
// CLogsConfig::SetConfig
// ----------------------------------------------------------------------------
//
void CLogsConfig::SetConfig( const TLogConfig& aConfig )
    {
    TRACE_ENTRY_POINT;
    if (!IsActive())
    	{
    	iFsSession.ReserveDriveSpace( EDriveC, KBytesToRequest );
    	iFsSession.GetReserveAccess( EDriveC );

    	iLogClient->ChangeConfig( aConfig, iStatus );
    	SetActive();
    	}
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::GetConfig
// ----------------------------------------------------------------------------
//
void CLogsConfig::GetConfig( TLogConfig& aConfig )
    {
    TRACE_ENTRY_POINT;
    if (!IsActive())
    	{
    	iState = EStateInitializing;    
    	iLogClient->GetConfig( aConfig, iStatus );
    	SetActive();
    	}
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsConfig::SetObserver( MLogsObserver* aObserver )
    {
    TRACE_ENTRY_POINT;
    iObserver = aObserver;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsConfig::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsConfig::State() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iState;
    }
