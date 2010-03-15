/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Soruce file for client test suite 
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "PsClientTestSuite.h"
#include <SettingServerClient.h>
#include "psclienttestsuitedefs.h"
#include "pstestcontactshandler.h"
#include <CPsQueryItem.h>
#include <CPsQuery.h>
#include <CPsRequestHandler.h>
#include <CPsSettings.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsClientTestSuite::CPsClientTestSuite(CTestModuleIf& aTestModuleIf):CScriptBase( aTestModuleIf )
{
}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsClientTestSuite::ConstructL()
{
    // Local timer for block fetch delay. This is created as first step to 
    // ensure that the handle is valid in destructor, even if some other
    // step of construction fails and leaves.
    iTimer.CreateLocal();

    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
        
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KPsTestSuiteLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KPsTestSuiteLogFile);
        }

    iLog = CStifLogger::NewL( KPsTestSuiteLogPath, 
                              logFileName,
                              CStifLogger::ETxt,
                              CStifLogger::EFile,
                              EFalse );
    
    // Create private data members
    iPsQuery = CPsQuery::NewL();
    iSettings =  CPsSettings::NewL();
    iPsClientHandler = CPSRequestHandler::NewL();
    iPsClientHandler->AddObserverL(this); 
	iCurrentOperation = EDefaultCase;
}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsClientTestSuite* CPsClientTestSuite::NewL( CTestModuleIf& aTestModuleIf )
{
    CPsClientTestSuite* self = new (ELeave) CPsClientTestSuite( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// Destructor
CPsClientTestSuite::~CPsClientTestSuite()
{ 
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog;
    
	if(iPsQuery)
	{
		delete iPsQuery;
		iPsQuery = NULL;	
	}

	if(iPsClientHandler)
	{
		delete iPsClientHandler;
		iPsClientHandler = NULL;	
	}

	if(iInputParsedData)
	{
		delete iInputParsedData;
		iInputParsedData = NULL;
	
	}
	
	if(iSettings)
	{
		delete iSettings;
		iSettings =NULL;
	} 
	iMarkedContacts.ResetAndDestroy();
	iMarkedContacts.Close();
	iTimer.Cancel();
    iTimer.Close();
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
{
    return ( CScriptBase* ) CPsClientTestSuite::NewL( aTestModuleIf );
}


//  End of File
