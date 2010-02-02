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









#ifndef MTPBKPHONENUMBERSELECT_H
#define MTPBKPHONENUMBERSELECT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <RPbkViewResourceFile.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CPbkPhoneNumberSelect;
class CPbkContactEngine;
class CPbkContactItem;
class CPbkAddressSelect;
class T_AddressAndNumberSelect;
class TPbkContactItemField;
class CSimulateKeyEvents;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkPhoneNumberSelectLogPath, "\\logs\\testframework\\MTPbkPhoneNumberSelect\\" ); 
// Log file
_LIT( KMTPbkPhoneNumberSelectLogFile, "MTPbkPhoneNumberSelect.txt" ); 
_LIT( KMTPbkPhoneNumberSelectLogFileWithTitle, "MTPbkPhoneNumberSelect_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkPhoneNumberSelect;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkPhoneNumberSelect test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkPhoneNumberSelect) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkPhoneNumberSelect* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkPhoneNumberSelect();

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
        CMTPbkPhoneNumberSelect( CTestModuleIf& aTestModuleIf );

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
		*	test class size
		*/
        TInt RunTest_ClassSizeL();

		/**
		* Focused memory entry does not contain an email address 
		* nor a name or a company name.
		* (User starts sending email in the names list (?).)
		*/
		TInt RunExecuteEmptyContactL();

		/** 
		*	Used in test 1 and 2
		*	Doesn't contain any email address but contains a name or 
		*	company name
		*/
		TInt RunExecuteTestCaseFocusNullL();
		TInt RunExecuteTestCaseFocusNull1L();
		TInt RunExecuteTestCaseFocusNull2L();
		TInt RunExecuteTestCaseDirectlyTrueL();
		TInt RunExecuteTestCaseDirectlyTrue1L();
		TInt RunExecuteTestCaseDirectlyTrue2L();

		/**
		* Focused memory entry does not contain an email address 
		* but it contains a name or a company name.
		*/
		TInt RunExecuteWithContactNameL();
		TInt RunExecuteWithCompanyNameL();

		/**
	    *   Focused memory entry contains only one phone number. 
	    *   Then that number is directly used as recipient without 
	    *   presenting any phone number list.
		*/
        TInt RunExecuteWithOnePhoneNumberL();
        TInt RunExecuteWithOneNumberFocusNullL();
        TInt RunExecuteWithOneNumberDirectlyTrueL();

		/**
	    *   The focused memory entry contains more than one phone number. 
	    *   The phone number list of the focused memory entry
	    *   is displayed as a list query.
		*/
		TInt RunExecuteWithMultiNumberL();
		
		/**
    	*   Phone number has a focus. Focused phone number should be
    	*   returned directly without showing any phone number list. 
		*/
        TInt RunExecuteWithMultiNumberAndFocusL();

		/**
    	*   Default phone number is set and focus is set to one of the phone number.
    	*   This returns directly the focused phone number.
		*/
        TInt RunExecuteWithMultiNumberAndOneDefaultL();
        
		/**
    	*   Default phone number set and focus on one of the email addresses.
    	*   The focused email address is directly returned and used as recipient.
    	*   Comments: Test case gives default and focus value for every e-mail.
		*/
		TInt RunExecuteWithMultiNumberAndOneFocusAndDefaultL();

		/**
	    *   Default phone number set and focus not on one of the phone numbers.
	    *   The phone number list is presented with the default number focused.
	    *   In this test case key events are simulated.
		*/
		TInt RunExecuteWithMultiNumberCompanyFocusL();
		
		/**
	    *   Default phone number is set. Returns the default phone number focused.
		*/
        TInt RunExecuteWithMultiNumberNullFocusL();
        
		/**
		*	The focused memory entry contains more than one email address. 
		*	The email address list of the focused memory entry
		*	is displayed as a list query.
		*	Test user actions in email address list.
		*/
		TInt RunExecuteTestSoftKeys1L();

		/**
		*	The focused memory entry contains more than one email address. 
		*	The email address list of the focused memory entry
		*	is displayed as a list query.
		*	Test user actions in email address list.
		*/
		TInt RunExecuteTestSoftKeys2L();

		/**
		*	Deletes CPbkContactItem
		*/
        void Teardown();
        
		/**
		*	Calls CPbkPhoneNumberSelect -object's ExecuteLD -method
		*/
		TBool RunExecuteL(const CPbkContactItem& aItem, 
                		const TPbkContactItemField* aFocusedField,
                		const TBool& aCallDefaultDirectly);
		/**
		*	Creates and initialises one empty CPbkContactItem object.
		*/
		void SetupEmptyContactItemL();
		
		/**
		*	Creates and initialises one almost empty CPbkContactItem object.
		*	A company name is added.
		*/
		void SetupContactItemWithCompanyNameL();
		
		/**
		*	Creates and initialises one almost empty CPbkContactItem object.
		*	A contact name is added.
		*/
		void SetupContactItemWithNameL();
		
		/**
		*	Creates and initialises one empty CPbkContactItem object.
		*	A contact and company name and email address is added.
		*/
		void SetupDefaultContactItemL();

        /**
        * Creates and initialises one CPbkContactItem object
        * with two home phone numbers.
        */
		void SetupContactItemWithTwoPhoneNumberFieldsL();

		/**
		*	Handles key events needed in test run
		*/
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
        
        // Phone number select
        CPbkPhoneNumberSelect*      iPhoneNumberSelect;
        // Phonebook engine instance
        CPbkContactEngine*			iContactEngine;
        // aItem passed to ExecuteLD
        CPbkContactItem*			iContactItem;
        // Filling contact/company information
        T_AddressAndNumberSelect*	iAddressAndNumberSelect;
		// Resource-file
		RPbkViewResourceFile		iPbkViewResourceFile;
		// Simulates key events
		CSimulateKeyEvents*			iSimulateKeyEvents;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKPHONENUMBERSELECT_H

// End of File
