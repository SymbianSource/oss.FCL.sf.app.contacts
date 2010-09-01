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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "t_ccacmscontactfetcherwrapper.h"
#include <SettingServerClient.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::T_CCCACmsContactFetcherWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
T_CCCACmsContactFetcherWrapper::T_CCCACmsContactFetcherWrapper( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::ConstructL()
    {
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
        logFileName.Format(KT_CCCACmsContactFetcherWrapperLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KT_CCCACmsContactFetcherWrapperLogFile);
        }

    iLog = CStifLogger::NewL( KT_CCCACmsContactFetcherWrapperLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
T_CCCACmsContactFetcherWrapper* T_CCCACmsContactFetcherWrapper::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    T_CCCACmsContactFetcherWrapper* self = new (ELeave) T_CCCACmsContactFetcherWrapper( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
T_CCCACmsContactFetcherWrapper::~T_CCCACmsContactFetcherWrapper()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }


//-----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("ut_cmscontactfetcher.dll");

	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName);
	}
	

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) T_CCCACmsContactFetcherWrapper::NewL( aTestModuleIf );

    }


//  End of File
