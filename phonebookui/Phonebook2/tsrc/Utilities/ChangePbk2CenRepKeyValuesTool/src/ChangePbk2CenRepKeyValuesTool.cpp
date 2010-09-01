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
* Description:  ChangePbk2CenRepKeyValuesTool class member functions
*
*/



// [INCLUDE FILES] - do not remove
#include <Stiftestinterface.h>
#include "ChangePbk2CenRepKeyValuesTool.h"
#include <SettingServerClient.h>
#include <Phonebook2PrivateCRKeys.h>
#include <centralrepository.h>

// Logging path
_LIT( KChangePbk2CenRepKeyValuesToolLogPath, "\\logs\\testframework\\ChangePbk2CenRepKeyValuesTool\\" ); 
// Log file
_LIT( KChangePbk2CenRepKeyValuesToolLogFile, "ChangePbk2CenRepKeyValuesTool.txt" ); 
_LIT( KChangePbk2CenRepKeyValuesToolLogFileWithTitle, "ChangePbk2CenRepKeyValuesTool_[%S].txt" );

#define GETPTR &
#define ENTRY(str1,str2,func1,func2,func3) {(const TText*)(L##str1 L" [" L##str2 L"]"),GETPTR func1,GETPTR func2, GETPTR func3, 0,0,0}
#define OOM_ENTRY(str1,str2,func1,func2,func3,a,b,c) {(const TText*)(L##str1 L" [" L##str2 L"]"), GETPTR func1,GETPTR func2, GETPTR func3,a,b,c}

typedef void (CChangePbk2CenRepKeyValuesTool::* TestFunction)();

class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iSetup;
        TestFunction    iMethod;
        TestFunction    iTeardown;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
        TPtrC CaseName() const {return TPtrC(iCaseName);};        
    };

// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::CChangePbk2CenRepKeyValuesTool
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CChangePbk2CenRepKeyValuesTool::CChangePbk2CenRepKeyValuesTool()
    {

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::ConstructL
// Symbian 2nd phase constructor can leave.
//
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::ConstructL()
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
    iAddTestCaseTitleToLogName = loggerSettings.iAddTestCaseTitle;

    iStdLog = CStifLogger::NewL( KChangePbk2CenRepKeyValuesToolLogPath, 
                          KChangePbk2CenRepKeyValuesToolLogFile);
    iLog = iStdLog;

    // Sample how to use logging
    _LIT( KLogStart, "ChangePbk2CenRepKeyValuesTool logging starts!" );
    iLog->Log( KLogStart );

    iVersionLogged = EFalse;
    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CChangePbk2CenRepKeyValuesTool* CChangePbk2CenRepKeyValuesTool::NewL()
    {
    CChangePbk2CenRepKeyValuesTool* self = new (ELeave) CChangePbk2CenRepKeyValuesTool;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CChangePbk2CenRepKeyValuesTool::~CChangePbk2CenRepKeyValuesTool()
    {
    iLog = NULL;
    delete iStdLog;
    iStdLog = NULL;
    delete iTCLog;
    iTCLog = NULL;
    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CChangePbk2CenRepKeyValuesTool::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CChangePbk2CenRepKeyValuesTool::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {

        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();

        // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).CaseName() );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );

        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );

        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CChangePbk2CenRepKeyValuesTool::RunTestCaseL( 
    const TInt aCaseNumber,   
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {
    iResult = &aResult;
    if(!iVersionLogged)
    	{
    	SendTestModuleVersion();
    	iVersionLogged = ETrue;
    	}
    
    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    const TCaseInfoInternal& tmp = Case ( aCaseNumber );

    _LIT( KLogStartTC, "Starting testcase [%S]" );
    TPtrC caseName = tmp.CaseName();
    iLog->Log( KLogStartTC, &caseName);

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        //Open new log file with test case title in file name
        if(iAddTestCaseTitleToLogName)
            {
            //Delete test case logger if exists
            if(iTCLog)
                {
                delete iTCLog;
                iTCLog = NULL;
                }

            TFileName logFileName;
            TName title;
            TestModuleIf().GetTestCaseTitleL(title);
        
            logFileName.Format(KChangePbk2CenRepKeyValuesToolLogFileWithTitle, &title);

            iTCLog = CStifLogger::NewL(KChangePbk2CenRepKeyValuesToolLogPath, 
                                       logFileName);
            iLog = iTCLog;                                       
            }


        TInt err = KErrNone;
        TRAP(err, ( this->*(tmp.iSetup) )());
        if(err!=KErrNone)
        	{
        	_LIT( KDescription, "Setup failed" );
        	aResult.SetResult( err, KDescription );
        	execStatus = KErrNone;
        	}
        else
        	{
	        aResult.iResult = KErrNone;
	        TRAP(err, ( this->*(tmp.iMethod) )());
	        if(err==KErrNone)
	        	{
	        	_LIT( KDescription, "Test passed" );
	        	aResult.SetResult( err, KDescription );
	        	}
	        else if ( aResult.iResult == KErrNone )
	            {
	        	_LIT( KDescription, "Test failed" );
	        	aResult.SetResult( err, KDescription );
	            }
	        TRAP(err, ( this->*(tmp.iTeardown) )());
	        if(err!=KErrNone)
	        	{
	        	_LIT( KDescription, "Teardown failed" );
	        	aResult.SetResult( err, KDescription );
	        	execStatus = KErrNone;
	        	}
        	}
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
// -----------------------------------------------------------------------------
//
TBool CChangePbk2CenRepKeyValuesTool::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt aCaseNumber, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& aFirstMemFailure, 
                                TInt& aLastMemFailure ) 
    {
    _LIT( KLogOOMTestQueryL, "CChangePbk2CenRepKeyValuesTool::OOMTestQueryL" );
    iLog->Log( KLogOOMTestQueryL );     

    aFirstMemFailure = Case( aCaseNumber ).iFirstMemoryAllocation;
    aLastMemFailure = Case( aCaseNumber ).iLastMemoryAllocation;

    return Case( aCaseNumber ).iIsOOMTest;

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: User may add implementation for OOM test environment initialization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: User may add implementation for OOM test warning handling. Usually no
// implementation is required.
// -----------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: User may add implementation for OOM test environment finalization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

//-----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::SendTestModuleVersion
// Method used to send version of test module
//-----------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::SendTestModuleVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
	moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("ChangePbk2CenRepKeyValuesTool.dll");

	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CChangePbk2CenRepKeyValuesTool::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CChangePbk2CenRepKeyValuesTool::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfoInternal& CChangePbk2CenRepKeyValuesTool::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    ChangePbk2CenRepKeyValuesTool.cpp file and to ChangePbk2CenRepKeyValuesTool.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CChangePbk2CenRepKeyValuesTool::PrintTest. Otherwise the compiler
        // gives errors.
#include "ChangePbk2CenRepKeyValuesTooltesttable.cpp"                                
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CChangePbk2CenRepKeyValuesTool::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CChangePbk2CenRepKeyValuesTool::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        static const TCaseInfoInternal nullCase = {NULL, NULL, NULL,NULL,EFalse,0,0};
        return nullCase;
        } 

    return KCases[ aCaseNumber ];
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::LocalVariationFeatureEnabled
// --------------------------------------------------------------------------
//
TInt CChangePbk2CenRepKeyValuesTool::GetLocalVariationFlagsL()
    {
    TInt lvFlags( KErrNotFound );
        
    // Read local variation flags
    User::LeaveIfError(iRepository->Get( KPhonebookLocalVariationFlags, lvFlags ));
    
    if(lvFlags == KErrNotFound)
        User::Leave(KErrNotFound);
    
    return lvFlags;
    }
    
// --------------------------------------------------------------------------
// CPbk2AppUi::LocalVariationFeatureEnabled
// --------------------------------------------------------------------------
//
TBool CChangePbk2CenRepKeyValuesTool::IsLocalVariationFeatureEnabledL( TInt aFeatureFlag )
    {
    TInt lvFlags = GetLocalVariationFlagsL();     
    return (lvFlags & aFeatureFlag);
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::LocalVariationFeatureEnabled
// --------------------------------------------------------------------------
//
void CChangePbk2CenRepKeyValuesTool::SetLocalVariationFeatureL( TInt aFeatureFlag, TBool aVal )
    {
    TInt lvFlags = GetLocalVariationFlagsL();     
    if(aVal)
        {
        // Set the bit ON
        lvFlags |= aFeatureFlag;        
        }
    else
        {
        // one's complement needed to set the flag OFF
        // Set the bit OFF
        lvFlags &= (~aFeatureFlag);        
        }

    TInt res = iRepository->Set(KPhonebookLocalVariationFlags, lvFlags);    
    User::LeaveIfError(res);    
               
    }

//  End of File
