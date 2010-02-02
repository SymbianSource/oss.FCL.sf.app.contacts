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









#ifndef MTPBKMMSADDRESSSELECT_H
#define MTPBKMMSADDRESSSELECT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>
#include <RPbkViewResourceFile.h>

//  FORWARD DECLARATIONS
class CPbkMmsAddressSelect;
class CPbkContactEngine;
class CSimulateKeyEvents;
class T_AddressAndNumberSelect;
class CCoeEnv;
class CPbkContactItem;
class TPbkContactItemField;

#include <e32def.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkMmsAddressSelectLogPath, "\\logs\\testframework\\MTPbkMmsAddressSelect\\" ); 
// Log file
_LIT( KMTPbkMmsAddressSelectLogFile, "MTPbkMmsAddressSelect.txt" ); 
_LIT( KMTPbkMmsAddressSelectLogFileWithTitle, "MTPbkMmsAddressSelect_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkMmsAddressSelect;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkMmsAddressSelect test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkMmsAddressSelect) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkMmsAddressSelect* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkMmsAddressSelect();

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
        CMTPbkMmsAddressSelect( CTestModuleIf& aTestModuleIf );

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
		void Teardown();
		/**
		*	test class size
		*/
        TInt RunTest_ClassSizeL();

        /**
        *   Focused memory entry does not contain a phone number
        *   nor a name or a company name. 
        *   Information note should show text: "No phone number".
        */
		TInt RunExecuteEmptyContactL();
		TInt RunExecuteTestCaseFocusNullL();
		TInt RunExecuteTestCaseFocusNull1L();
		TInt RunExecuteTestCaseFocusNull2L();
		TInt RunExecuteTestCaseDirectlyTrueL();
		TInt RunExecuteTestCaseDirectlyTrue1L();
		TInt RunExecuteTestCaseDirectlyTrue2L();

        /**
        *   Focused memory entry does not contain a phone number 
        *   or an mail address but it contains a name or a company name.
        */
		TInt RunExecuteWithContactNameL();
		TInt RunExecuteWithCompanyNameL();

        /**
        *   Focused memory entry contains only one phone number or 
        *   one mail address. Then that address is directly used as 
        *   recipient without presenting any selection list.
        */
		TInt RunExecuteWithPhoneNumberL();
		TInt RunExecuteWithPhoneNumberFocusNullL();
		TInt RunExecuteWithPhoneNumberDirectlyTrueL();
		TInt RunExecuteWithOneEmailL();
		TInt RunExecuteWithOneEmailFocusNullL();
		TInt RunExecuteWithOneEmailDirectlyTrueL();

        /**
        *   The focused memory entry contains more than one phone 
        *   number and/or email addresses. 
        *   The selection list of the focused memory entry
        *   is displayed as a list query.
        */
		TInt RunExecuteMultiMMSAndKeySimL();
		
        /**
        *   Focus is on a phone number or email address. Focused 
        *   phone number or email address is used as recipien 
        *   directly without showing any seleciton list.
        */
		TInt RunExecuteMultiMMSAndKeySimFocusNullL();
		
        /**
        *   Default MMS is set. Focus is on first name field or no 
        *   focus at all. This returns directly the default MMS.
        */
		TInt RunExecuteWithMultiMMSL();
		
        /**
        *   Default MMS number set and focus on MMS field.
        *   Focused MMS address is directly returned.
        */
		TInt RunExecuteWithMultiMMSAndMMSfocusL();
		
        /**
        *   Default MMS number set and focus not on MMS field.
        *   The selection list is presented with the default MMS focused.
        *   In this test case key events are simulated.
        */
		TInt RunExecuteWithMultiMMSAndCompanyfocusL();
		
        /**
        *   Default MMS number is set. Returns focused default MMS field.
        */
		TInt RunExecuteMultiMMSAndDefNoFocusL();
		
        /**
        *   Tests for user actions in MMS number list.
        */
		TInt RunExecuteTestSoftKeys1L();
		
        /**
        *   Tests for user actions in MMS number list.
        */
		TInt RunExecuteTestSoftKeys2L();
		
		TBool RunExecuteL(const CPbkContactItem& aItem, 
                		const TPbkContactItemField* aFocusedField,
                		const TBool& aCallDefaultDirectly);
        
        /**
        *   Create contact item without information
        */
		void SetupEmptyContactItemL();

        /**
        *   Create contact item with company name
        */
		void SetupContactItemWithCompanyNameL();

        /**
        *   Create contact item with contact name
        */
		void SetupContactItemWithNameL();

        /**
        *   Create contact item with one phone number, no emails
        */
		void SetupDefaultContactItemWithoutEmailL();

        /**
        *   Create contact item with one email and no phone number
        */
		void SetupDefaultContactItemWithoutPhoneNumberL();

        /**
        *   Default setup function initializes 
        *   two sms fields = one email + and one phone number field.
        */
		void SetupContactItemWithFourMMSFieldsL();
		
		void SendKeyEventsL();
		void SendKeyEventsL( TKeyCode& aTestedSoftKeyCode );
        
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
        
		CCoeEnv& 					iConeEnv;
		RPbkViewResourceFile 		iPbkViewResourceFile;
        CPbkContactEngine*			iContactEngine;
		CPbkMmsAddressSelect*		iMmsAddressSelect;
		T_AddressAndNumberSelect* 	iAddressAndNumberSelect;
		CSimulateKeyEvents* 		iSimulateKeyEvents;
        CPbkContactItem*			iContactItem;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKMMSADDRESSSELECT_H

// End of File
