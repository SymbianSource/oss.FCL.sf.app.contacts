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









#ifndef MTPBKCONTACTITEMFIELD_H
#define MTPBKCONTACTITEMFIELD_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

//  FORWARD DECLARATIONS
class TPbkContactItemField;
class CPbkContactEngine;
class CContactItemField;
class CPbkFieldInfo;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkContactItemFieldLogPath, "\\logs\\testframework\\MTPbkContactItemField\\" ); 
// Log file
_LIT( KMTPbkContactItemFieldLogFile, "MTPbkContactItemField.txt" ); 
_LIT( KMTPbkContactItemFieldLogFileWithTitle, "MTPbkContactItemField_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkContactItemField;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkContactItemField test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkContactItemField) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkContactItemField* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkContactItemField();

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
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

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
        CMTPbkContactItemField( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
         * Returns const reference to contact item field
         *
         * @return Const reference to TPbkContactItemField
         */ 
        const TPbkContactItemField& ContactItemField();
        
        /**
         * Does nothing
         */
        void DummyMethod(); 
        
        /**
         * Creates empty TPbkContactItemField
         */
        void SetupEmptyL();
        
        /**
         * Creates First name field
         */
        void SetupNameFieldL();
        
        /**
         * Creates date/time field
         */
        void SetupDateTimeFieldL();
        
        /**
         * Creates store field
         */
        void SetupStoreFieldL();
        
        /**
         * Creates phone number field
         */
        void SetupNumberFieldL();
        
        /**
         * Creates Creates email address field field
         */
        void SetupEmailFieldL();
        
        /**
         * Teardown
         */
        void Teardown();
        
        /**
         * Test size of TPbkContactItemField class
         */
        TInt RunTestSize();
        
        /**
         * Test default construction
         */
        TInt RunCreateEmptyTest();
        
        /**
         * Test construction with values
         */
        TInt RunCreateWithValuesTest();
        
        /**
         * Run 'Set' testcase
         */
        TInt RunSetTest();
        
        /**
         * Run 'IconId' testcase
         */
        TInt RunIconIdTest() ;
        
        /**
         * Run 'FieldInfo' testcase
         */
        TInt RunFieldInfoTest();
        
        /**
         * Run 'Text' testcase
         */
        TInt RunTextTestL();
        
        /**
         * Run 'Time' testcase
         */
        TInt RunTimeTest();
        
        /**
         * Run 'Date' testcase
         */
        TInt RunDateTest();
        
        /**
         * Run 'GetTextL' testcase
         */
        TInt RunGetTextLTestL();
        
        /**
         * Run 'GetTextL' testcase. Field type is date
         */
        TInt RunGetTextLDateTestL();
        
        /**
         * Run 'GetTextL' testcase. Field content cannot be represented as text
         */
        TInt RunGetTextLNotSupportedTestL();
        
        /**
         * Run 'StorageType' testcase. 
         */
        TInt RunStorageTypeTest();
        
        /**
         * Run 'ItemField' testcase
         */
        TInt RunItemFieldTest();
        
        /**
         * Run 'TextStorage' testcase
         */
        TInt RunTextStorageTestL();
        
        /**
         * Run 'Label' testcase. Set and get label
         */
        TInt RunLabelTestL();
         
        /**
         * Run 'Hidden' testcase. Set and get hidden status
         */
        TInt RunHiddenTest();
        
        /**
         * Run 'DateTimeStorage' testcase. 
         */
        TInt RunDateTimeStorageTest();
        
        /**
         * Run 'IsSame' testcase. 
         */
        TInt RunIsSameTestL();
        
        /**
         * Run 'ContactItemField' testcase. 
         */
        TInt RunContactItemFieldTest();
        
        /**
         * Run 'Compare' testcase. 
         */
        TInt RunCompareTestL();
        
        /**
         * Run 'UniqueFieldIdentity' testcase. 
         */
        TInt RunUniqueFieldIdentityTest();
        
        /**
         * Run 'IsEmpty' testcase. 
         */
        TInt RunIsEmptyTestL();
        
        /**
         * Run 'IsEmptyOrAllSpaces' testcase. 
         */
        TInt RunIsEmptyOrAllSpacesTestL();
        
        /**
         * Run 'IsEmptyOrAllSpaces' testcase. Not text storage 
         */
        TInt RunIsEmptyOrAllSpacesNoTextTestL();
        
        /**
         * Run 'DefaultPhoneNumberField' testcase. 
         */
        TInt RunDefaultPhoneNumberFieldTestL();
        
        /**
         * Run 'DefaultVideoNumberField' testcase. 
         */
        TInt RunDefaultVideoNumberFieldTestL();
        
        /**
         * Run 'DefaultSmsField' testcase. 
         */
        TInt RunDefaultSmsFieldTestL();
         
        /**
         * Run 'DefaultEmailOverSmsField' testcase. 
         */
        TInt RunDefaultEmailOverSmsFieldTestL();
        
        /**
         * Run 'DefaultEmailField' testcase. 
         */
        TInt RunDefaultEmailFieldTestL();
        
        /**
         * Run 'DefaultMmsField' testcase. 
         */
        TInt RunDefaultMmsFieldTestL();
        
        /**
         * Run 'DefaultPocField' testcase. 
         */
        TInt RunDefaultPocFieldTestL();
        
        /**
         * Run 'DefaultVoipField' testcase. 
         */
        TInt RunDefaultVoipFieldTestL();
        
        /**
         * Run 'SpeedDialIndexesL' testcase. 
         */
        TInt RunSpeedDialIndexesLTestL();
        
        /**
         * Run 'PbkFieldType' testcase. 
         */
        TInt RunPbkFieldTypeTest();
        
        /**
         * Run 'PbkFieldId' testcase. 
         */
        TInt RunPbkFieldIdTest();
        
        /**
         * Run 'PbkFieldText' testcase. 
         */
        TInt RunPbkFieldTextTestL();
        
        /**
         * Run 'PbkFieldTime' testcase. 
         */
        TInt RunPbkFieldTimeTest();
            
        /**
         * Run '==' testcase. Text field.
         */
        TInt RunEqualsTextFieldsTestL();
        
        /**
         * Run '==' testcase. Date field.
         */
        TInt RunEqualsDateFieldsTestL();
        
        /**
         * Run '==' testcase. Store field.
         */
        TInt RunEqualsStoreFieldsTestL();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        TPbkContactItemField*   iContactItemField;  //Contact item field
        CPbkContactEngine*      iContactEngine;     //Contact engine
        CContactItemField*      iCntField;          //CContactItemField
        CPbkFieldInfo*          iFieldInfo;         //Field info

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKCONTACTITEMFIELD_H

// End of File
