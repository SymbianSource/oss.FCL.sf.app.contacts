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
* Description:  MT_VPbkEng_SelectorFactory test module.
*
*/



#ifndef MT_VPBKENG_SELECTORFACTORY_H
#define MT_VPBKENG_SELECTORFACTORY_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

#include "CVPbkFieldTypeList.h"

#include <spdefinitions.h>
#include <BADESCA.H>


// MACROS
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// FORWARD DECLARATIONS
class TCaseInfoInternal;
class CVPbkContactTestUtils;
class MVPbkFieldTypeList;
class CVPbkFieldTypeSelector;
class CAssertUtils;
class CSPSettings;
class MVPbkStoreContact;
class MVPbkStoreContactField;


// CLASS DECLARATION

/**
*  This a MT_VPbkEng_SelectorFactory class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMT_VPbkEng_SelectorFactory) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CMT_VPbkEng_SelectorFactory* NewL();

        /**
        * Destructor.
        */
        virtual ~CMT_VPbkEng_SelectorFactory();

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
        *       MT_VPbkEng_SelectorFactory. It is called once for every instance of 
        *       TestModuleMT_VPbkEng_SelectorFactory after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of MT_VPbkEng_SelectorFactory.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from MT_VPbkEng_SelectorFactory. 
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
        CMT_VPbkEng_SelectorFactory();

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
        
        void BaseSetupL();
        void BaseTeardown();
        
        void Setup();
        void SetupVoipL();
        void SetupVoipCallOutL();
        void SetupGtalkSupportsVoipL();
        void Teardown();
        
        void TestPassed();
        void TestFieldSelectorFactoryValid();
        void TestFieldSelectorFactoryInvalid();
        void TestCommSelectorFactoryValid();
        void TestCommSelectorFactoryInvalid();
        void TestCommSelectorFactoryNoList();        
        void TestCommSelectorFactoryUniEditor();
        void TestUniEditorSelectionL();
        void TestVoipNotAvailableSelectionL();
        void TestVoiceCallSelectionL();
        void TestVoipNotSupportedSelectionL();
        void TestVoipCallSelectionL();
        void TestVoipCallOutSelectionL();
        void TestXspSupportsVoipSelectionL();
        void TestVideoSelectorL();
        void TestURLSelectorL();
        void TestEmailSelectorL();
        void TestIMSelectorL();
        void TestEmptyContactFieldSelectionL();
        void TestGtalkContactSelectionL();
        void TestTwoServicesContactSelectionL();
        void TestVersion1SelectorL();
        void TestMultipleSelectionL();
        void TestNonVersitTypeSelectionL();
        
        CVPbkFieldTypeList* CreateFieldTypeListL();
        void AssertTypesL(
                CVPbkFieldTypeSelector& aSelector,
                const TArray<TInt> aArray,
                const MVPbkFieldTypeList& aFieldTypes);
        
        void AssertFieldsL(
                CVPbkFieldTypeSelector& aSelector,
                const MVPbkStoreContact& aContact,
                TArray<TInt> aAsserFields,
                CDesCArray* aServiceNames = NULL);
        
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
        
        /// Test utils
        CVPbkContactTestUtils* iTestUtils;
        const MVPbkFieldTypeList* iFieldTypes;
        MVPbkStoreContact* iContact; 
        CAssertUtils* iAssertUtils;
        
        CSPSettings* iSpSettings;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MT_VPBKENG_SELECTORFACTORY_H

// End of File
