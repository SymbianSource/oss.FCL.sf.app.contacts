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
* Description: 
*       
*
*/

#include <flogger.h>
#include "cmssession.h"
#include "cmscommondefines.h"

//CLASS DECLARATION
EXPORT_C RCmsSession::RCmsSession() : iConnected( EFalse )
    {
    }
        
// ----------------------------------------------------
// RCmsClient::Version
// 
// ----------------------------------------------------
//
EXPORT_C TVersion RCmsSession::Version() const
	{
	return TVersion( KCmsSrvMajorVersionNumber,
					 KCmsSrvMinorVersionNumber,
					 KCmsSrvBuildVersionNumber );
	}

// ----------------------------------------------------
// RCmsClient::~RCmsClient
// 
// ----------------------------------------------------
//
EXPORT_C RCmsSession::~RCmsSession()
    {
    }

// ----------------------------------------------------
// RCmsClient::Connect
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsSession::Connect()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsSession::Connect()" ) );
    #endif
    TInt ret = KErrNone;
    if( !iConnected )
        {
        TInt threadStart = StartServer();
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  StartServer() returned %d" ), threadStart );
        #endif
        if( threadStart == KErrNone || threadStart == KErrAlreadyExists )
            {
            ret = CreateSession( KCmsServerLib, Version(), KTTDefaultMessageSlots );
            #ifdef _DEBUG
                RCmsSession::WriteToLog( _L8( "  CreateSession() returned %d" ), ret );
            #endif
            iConnected = ( ret == KErrNone );
            }
        else ret = threadStart;    
        }
    return ret; 
    }

// ----------------------------------------------------
// RCmsSession::StartServer
//
// ----------------------------------------------------
//
TInt RCmsSession::StartServer()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsSession::StartServer()" ) );
    #endif
    TInt ret = KErrNone;
    TRequestStatus status = KRequestPending; 
    if ( !IsRunning() )
        {
        RProcess server;
        ret = server.Create( KCmsServerExe, KNullDesC );
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  RThread::Create() returned: %d" ), ret );
        #endif      
        if( ret != KErrNone )
            return ret;     
        server.Rendezvous( status );    	
        status != KRequestPending ? server.Kill( 0 ) : server.Resume();
        //Wait for start or death 
        User::WaitForRequest( status );	
        ret = server.ExitType() == EExitPanic ? KErrGeneral : status.Int();
        server.Close();	
        }
    return ret;
    }

// ----------------------------------------------------
// RCmsSession::IsRunning
//
// ----------------------------------------------------
//
TBool RCmsSession::IsRunning()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsSession::IsRunning()" ) );
    #endif
    TFindServer findServer( KCmsServerName );
    TFullName name;
    return ( findServer.Next( name ) == KErrNone );
    } 

#ifdef _DEBUG
// ----------------------------------------------------
// RCmsSession::::WriteToLog
// 
// ----------------------------------------------------
//
void RCmsSession::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KClientLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KCmsClientLogDir, KCmsClientLogFile, EFileLoggingModeAppend, buf );
    RDebug::RawPrint( buf );
    }
#endif  //_DEBUG
