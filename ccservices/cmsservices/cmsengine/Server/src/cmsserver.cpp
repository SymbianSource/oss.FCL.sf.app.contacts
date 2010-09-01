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


// INCLUDE FILES
#include "cmsdebug.h"
#include "cmsserver.h"
#include "cmsserversession.h"
#include "cmsphonebookproxy.h"
#include <crcseprofileregistry.h>

// ----------------------------------------------------------
// CCmsServer::New
// 
// ----------------------------------------------------------
//
CCmsServer* CCmsServer::NewLC()
    {
    PRINT( _L( "Start CCmsServer::NewLC()" ) );
    CCmsServer* self = new CCmsServer( EPriorityHigh );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    PRINT( _L( "End CCmsServer::NewLC()" ) );
    return self;
    }

// ----------------------------------------------------------
// CCmsServer::New
// 
// ----------------------------------------------------------
//
void CCmsServer::ConstructL()
    {
    PRINT( _L( "Start CCmsServer::ConstructL()" ) );
    StartL( KCmsServerLib );
    User::LeaveIfError( iFS.Connect() );
    iPhonebookProxy = CCmsPhonebookProxy::NewL( *this, iFS );
    PRINT( _L( "End CCmsServer::ConstructL()" ) );
    }
    
// ----------------------------------------------------------
// CCmsServer::NewSessionL
// 
// ----------------------------------------------------------
//
CSession2* CCmsServer::NewSessionL( const TVersion& aVersion,
                                    const RMessage2& /*aMessage*/ ) const
    {
    
    // Check version number of API against our known version number.
    TVersion thisVersion( KCmsSrvMajorVersionNumber,
                          KCmsSrvMinorVersionNumber,
                          KCmsSrvBuildVersionNumber );
    if( !User::QueryVersionSupported( thisVersion, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
        
    return CCmsServerSession::NewL(( CCmsServer* ) this );
    }

// ----------------------------------------------------------
// CCmsServer::CCmsServer
// 
// ----------------------------------------------------------
//
CCmsServer::~CCmsServer()
    {
    PRINT( _L( "Start CCmsServer::~CCmsServer()" ) );
    Cancel();
    delete iPhonebookProxy;
    delete iRCSEProfileRegistry;
    iFS.Close();
    PRINT( _L( "End CCmsServer::~CCmsServer()" ) );
    }

// ----------------------------------------------------------
// CCmsServer::CCmsServer
// 
// ----------------------------------------------------------
//
CCmsServer::CCmsServer( TInt aPriority ) : CServer2( aPriority )
    {
    }

// ----------------------------------------------------------
// CCmsServer::PanicServer
// 
// ----------------------------------------------------------
//
void CCmsServer::PanicServer( TCmsServerPanic aPanic )
    {
    PRINT1( _L( "CCmsServer::PanicServer() - Panic: %d" ), aPanic );
	
    _LIT( KCmsServerPanic, "CCmsServer");
    User::Panic( KCmsServerPanic, aPanic );
    }

// ----------------------------------------------------
// CCmsServer::FileSession
// 
// ----------------------------------------------------
//
RFs& CCmsServer::FileSession()
    {
    return iFS;
    }

// ----------------------------------------------------
// CCmsServer::RCSEProfileRegistryL
// 
// ----------------------------------------------------
//
CRCSEProfileRegistry& CCmsServer::RCSEProfileRegistryL()
    {
    if( !iRCSEProfileRegistry )
        {
        iRCSEProfileRegistry = CRCSEProfileRegistry::NewL();
        }
    return *iRCSEProfileRegistry;
    }

// ----------------------------------------------------
// CCmsServer::ContactProxy
// 
// ----------------------------------------------------
//
CCmsPhonebookProxy& CCmsServer::PhonebookProxyHandle()
    {
    return *iPhonebookProxy;
    }

// ----------------------------------------------------
// CCmsServer::StoreOpenComplete
// 
// ----------------------------------------------------
//
void CCmsServer::StoreOpenComplete()
    {
    iSessionIter.SetToFirst();
    CSession2* session = iSessionIter;
    while( session != NULL )
        {
    	static_cast<CCmsServerSession*>(session)->StoreOpenComplete();
    	iSessionIter++;
        session = iSessionIter;
        }
    }

// ----------------------------------------------------
// CCmsServer::CmsSingleContactOperationComplete
// 
// ----------------------------------------------------
//
void CCmsServer::CmsSingleContactOperationComplete( TInt aError )
    {
    iSessionIter.SetToFirst();
    CSession2* session = iSessionIter;
    while( session != NULL )
        {
    	static_cast<CCmsServerSession*>(session)->
    	    CmsSingleContactOperationComplete( aError );
    	iSessionIter++;
        session = iSessionIter;
        }	
    }

// ----------------------------------------------------
// CCmsServer::StartThreadL
// 
// ----------------------------------------------------
//
TInt CCmsServer::StartThreadL()
    {
    PRINT( _L( "Start CCmsServer::StartThreadL()" ) );
	User::LeaveIfError( User::RenameThread( KCmsServerName ) );
	CCmsScheduler* scheduler = new ( ELeave ) CCmsScheduler;
	CleanupStack::PushL( scheduler );
    if( !CActiveScheduler::Current() )
        {
        PRINT( _L("CCmsServer::StartThreadL(): Scheduler not installed => install" ) );
        CActiveScheduler::Install( scheduler );
        }
    
    CCmsServer* server = CCmsServer::NewLC();
    
    //Open all contact stores so that they are ready when
    //the first session is requested
    server->PhonebookProxyHandle().InitStoresL();
    RProcess::Rendezvous( KErrNone );
    CActiveScheduler::Start();

	CleanupStack::PopAndDestroy( 2 );  //server, scheduler
	PRINT( _L( "End CCmsServer::StartThreadL()" ) );
	return KErrNone;
    }

// ================= SCHEDULER =======================

// ----------------------------------------------------
// CCmsScheduler::Error
// 
// ----------------------------------------------------
//
void CCmsScheduler::Error( TInt aError ) const
    {
    //To suppress build warnings
    TInt err = aError;
    PRINT1( _L( "*** CCmsScheduler::Error() - Error: %d ***" ), err );
    }

