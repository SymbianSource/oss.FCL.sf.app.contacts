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
* Description:  ChangePbk2CenRepKeyValuesTool test module.
*
*/



#ifndef CHANGEPBK2CENREPKEYVALUESTOOL_H
#define CHANGEPBK2CENREPKEYVALUESTOOL_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

// MACROS
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// FORWARD DECLARATIONS
class TCaseInfoInternal;
class CRepository;

// CLASS DECLARATION

/**
*  This a ChangePbk2CenRepKeyValuesTool class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CChangePbk2CenRepKeyValuesTool) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CChangePbk2CenRepKeyValuesTool* NewL();

        /**
        * Destructor.
        */
        virtual ~CChangePbk2CenRepKeyValuesTool();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
        *       ChangePbk2CenRepKeyValuesTool. It is called once for every instance of 
        *       TestModuleChangePbk2CenRepKeyValuesTool after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of ChangePbk2CenRepKeyValuesTool.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from ChangePbk2CenRepKeyValuesTool. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );
        
        /**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CChangePbk2CenRepKeyValuesTool();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfoInternal& Case ( const TInt aCaseNumber ) const;

        /**
        * [test case declarations] - do not remove
        */
        //ADD NEW METHOD DEC HERE"
        
        void Setup();
        void Teardown();
        void TestPassed();

        void SetTCPromotionAlwaysOFF();

        void SetTCPromotionAlwaysON();

        void SetTCPromotionTillFirstTimeUse();
        
        void SetPbk2AlwaysOFF();
        void SetPbk2AlwaysON();        
        
        TInt GetLocalVariationFlagsL();
        TBool IsLocalVariationFeatureEnabledL( TInt aFeatureFlag );
        void SetLocalVariationFeatureL( TInt aFeatureFlag, TBool aVal );                

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // test result, not owned
        TTestResult* iResult;

        // Pointer to logger
        CStifLogger * iLog; 

        // Normal logger
        CStifLogger* iStdLog;

        // Test case logger
        CStifLogger* iTCLog;

        // Flag saying if test case title should be added to log file name
        TBool iAddTestCaseTitleToLogName;

        // Flag saying if version of test module was already sent
        TBool iVersionLogged;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
        // Read the variation flag
        CRepository* iRepository;        

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CHANGEPBK2CENREPKEYVALUESTOOL_H

// End of File
