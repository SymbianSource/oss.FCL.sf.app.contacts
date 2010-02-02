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



#include "ClientSrvTester.h"



// -----------------------------------------------------------------------------
// CClientSrvTester::CClientSrvTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CClientSrvTester::CClientSrvTester( CTestModuleIf& aTestModuleIf ) : CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CClientSrvTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClientSrvTester::ConstructL()
    {
    /*
    ETestLeaksMem       = 0x00000001,
    ETestLeaksRequests  = 0x00000002,
    ETestLeaksHandles   = 0x00000004,
    EOOMDisableLeakChecks = 0x00000008,
    */
    iLog = CStifLogger::NewL( KCmsTesterLogPath, KCmsTesterLogFile,
                              CStifLogger::ETxt, CStifLogger::EFile, EFalse );
    
    iLog->Log( _L(" ConstructL creates Handler") );          
    

    User::LeaveIfError( iFs.Connect() );
    
    
    // This is needed because of iFbsSession.Disconnect( cannot be called for some reason.
    // TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles, this );           
    
    }

// -----------------------------------------------------------------------------
// CClientSrvTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClientSrvTester* CClientSrvTester::NewL( CTestModuleIf& aTestModuleIf )
    {
    CClientSrvTester* self = new ( ELeave ) CClientSrvTester( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CClientSrvTester::~CClientSrvTester()
    {                 
    DeleteMe();
         
    iFs.Close();
        
    delete iLog;
    }

// -----------------------------------------------------------------------------
// CClientSrvTester::RequestComplete
// 
// -----------------------------------------------------------------------------
//
void CClientSrvTester::RequestComplete( TInt aStatus )
    {
    TInt status = iExpectedResult == aStatus ? KErrNone : KErrGeneral;
    Signal( status );
    }

// -----------------------------------------------------------------------------
// CClientSrvTester::TesterAtoi
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::TesterAtoi( const TDesC& aDescInteger )
    {
    TLex lex( aDescInteger );
    TInt result = KErrGeneral;
    lex.Val( result );
    return result;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    return ( CScriptBase* ) CClientSrvTester::NewL( aTestModuleIf );    
    }

//  End of File
