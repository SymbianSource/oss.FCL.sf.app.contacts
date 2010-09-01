/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*/



// INCLUDE FILES
#include <fbs.h>
#include <s32mem.h>
#include <s32strm.h>
#include <StifParser.h>
#include <StifItemParser.h>
#include <Stiftestinterface.h>
#include <AknIconSrvClient.h>


#include "contactpresenceapitester.h"
#include "contactpresenceapihandler.h"
#include "contactpresencephonebook.h"
#include "contactpresencepbhandler.h"

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::CContactPresenceApiTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactPresenceApiTester::CContactPresenceApiTester( CTestModuleIf& aTestModuleIf ) : CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactPresenceApiTester::ConstructL()
    {
    iLog = CStifLogger::NewL( KCmsTesterLogPath, KCmsTesterLogFile,
                              CStifLogger::ETxt, CStifLogger::EFile, EFalse );
    iLinkArray = new ( ELeave ) CPtrC8Array( 10 );
    
    iLog->Log( _L(" ConstructL creates iContactPresenceHandler") );          
    iContactPresenceHandler = CContactPresenceApiHandler::NewL( *this );
        
    iPhonebook = CContactPresencePhonebook::NewL();          
    iPhonebookHandler = CContactPresencePbHandler::NewL( *this, *iPhonebook );  
    
    iSettings = CSPSettings::NewL();    

    //Needed for FBSBitmaps
    User::LeaveIfError( iFs.Connect() );
    iFbsSession.Connect(iFs);
    
    //Needed for AknIconUtils
    iLog->Log( _L(" ConstructL connects RAknIconSrvClient") );    
    User::LeaveIfError( RAknIconSrvClient::Connect() ); 
    
    iMyTimer = new (ELeave) CMyTimer(*this);    
    
    // This is needed because of iFbsSession.Disconnect( cannot be called for some reason.
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles, this );           
    
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactPresenceApiTester* CContactPresenceApiTester::NewL( CTestModuleIf& aTestModuleIf )
    {
    CContactPresenceApiTester* self = new ( ELeave ) CContactPresenceApiTester( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CContactPresenceApiTester::~CContactPresenceApiTester()
    {
    delete iMyTimer;
    
    Delete();
    
    delete iSettings;
    
    RAknIconSrvClient::Disconnect();         
    // This crashes : 
    iFbsSession.Disconnect();
    iFs.Close();
            
    delete iLog;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::RequestComplete
// 
// -----------------------------------------------------------------------------
//
void CContactPresenceApiTester::RequestComplete( TInt aStatus )
    {
    TInt status = iExpectedResult == aStatus ? KErrNone : KErrGeneral;
    Signal( status );
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::TesterAtoi
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::TesterAtoi( const TDesC& aDescInteger )
    {
    TLex lex( aDescInteger );
    TInt result = KErrGeneral;
    lex.Val( result );
    return result;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::StoreOneContactLinkL
// 
// -----------------------------------------------------------------------------
//
void CContactPresenceApiTester::StoreOneContactLinkL( HBufC8* aContactLink, CVPbkContactManager* aManager )
    {
    iManager = aManager;
    iLinkArray->AppendL( *aContactLink );
    }


/************************************************************
 * 
 * CContactPresenceApiTester::DoDoWaitSec
 * 
 ************************************************************/

 void CContactPresenceApiTester::DoDoWaitSec( TInt aSec )
     {
 	 iMyTimer->Start( aSec );
     }
     
 // -----------------------------------------------------------------------------
 //
void CContactPresenceApiTester::LogThis( const TDesC& aText )
     {
     iLog->Log(aText );
     }     
 
 // ---------------------------------------------------------------------------
 // Expiry timer
 // ---------------------------------------------------------------------------
 //

 // ======== MEMBER FUNCTIONS ========

 CMyTimer::CMyTimer(
     CContactPresenceApiTester& aTester )
     : CActive( CActive::EPriorityStandard),
       iTester(aTester)

     {
     // Add this to the scheduler
     (void) iTimer.CreateLocal();
     CActiveScheduler::Add(this);
     }

 CMyTimer::~CMyTimer()
     {
     Cancel();
     iTimer.Close();
     }

 // -----------------------------------------------------------------------------
 // CMyTimer::Start
 // -----------------------------------------------------------------------------
 void CMyTimer::Start( TInt aWaitSeconds )
     {
     const TInt KUseAfterLimit = 240;      
     // Cancel is needed because of the timer may be reset.
     Cancel();

     if ( aWaitSeconds <= 0 )
         {
         return;
         }

     iSeconds = aWaitSeconds;

     // The At function caused a CUserbase-Panic 46 in very small
     // time values. 1-4 seconds. Now if the KeepAlive time
     // is smaller than UseAfterLimit, then we use the After function
     // If it is larger then use the At function
     // The reason not to use the After function for every situation is
     // that the TInt overflows after 35 minutes. 1000000*60*36 > 2^31
     if( iSeconds <= KUseAfterLimit )
         {
         iTimer.After( iStatus, iSeconds * 1000000 );
         }
     else
         {
         TTime myKeepAlive;
         myKeepAlive.HomeTime();
         myKeepAlive += TTimeIntervalSeconds( iSeconds );
         iTimer.At( iStatus, myKeepAlive );
         }
     iStatus = KRequestPending;
     SetActive();
     }      

 // -----------------------------------------------------------------------------
 // CMyTimer::DoCancel
 // -----------------------------------------------------------------------------
 void CMyTimer::DoCancel( )
     {
     iTimer.Cancel();
     }

 // -----------------------------------------------------------------------------
 // CMyTimer::RunL
 // -----------------------------------------------------------------------------
 void CMyTimer::RunL(  )
     {
     iTester.RequestComplete( KErrNone );
     }

 // -----------------------------------------------------------------------------
 // CMyTimer::RunError
 // -----------------------------------------------------------------------------
 TInt CMyTimer::RunError(TInt /*aError*/)
     {
     return KErrNone;
     }
   

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {    
    return ( CScriptBase* ) CContactPresenceApiTester::NewL( aTestModuleIf );    
    }

//  End of File
