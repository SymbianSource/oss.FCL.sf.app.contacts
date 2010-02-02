/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// INCLUDE FILES
#include <Stiftestinterface.h>
#include "BCTestPbkUIUtil.h"
#include <SettingServerClient.h>
#include <coemain.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBCTestPbkUIUtil::CBCTestPbkUIUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBCTestPbkUIUtil::CBCTestPbkUIUtil( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iPbkViewResourceFile( *CCoeEnv::Static() )
    {
    }

// -----------------------------------------------------------------------------
// CBCTestPbkUIUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBCTestPbkUIUtil::ConstructL()
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
        logFileName.Format(KBCTestPbkUIUtilLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KBCTestPbkUIUtilLogFile);
        }

    iLog = CStifLogger::NewL( KBCTestPbkUIUtilLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    User::LeaveIfError( iFs.Connect() );
    iPbkViewResourceFile.OpenL();

    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CBCTestPbkUIUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBCTestPbkUIUtil* CBCTestPbkUIUtil::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CBCTestPbkUIUtil* self = new (ELeave) CBCTestPbkUIUtil( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CBCTestPbkUIUtil::~CBCTestPbkUIUtil()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
    iFs.Close();
    iPbkViewResourceFile.Close();   
    }

//-----------------------------------------------------------------------------
// CBCTestPbkUIUtil::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//

void CBCTestPbkUIUtil::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("BCTestPbkUIUtil.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
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

    return ( CScriptBase* ) CBCTestPbkUIUtil::NewL( aTestModuleIf );

    }


//  End of File
