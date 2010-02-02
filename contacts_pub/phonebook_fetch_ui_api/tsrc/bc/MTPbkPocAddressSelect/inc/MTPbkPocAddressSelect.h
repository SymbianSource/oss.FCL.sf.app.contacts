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









#ifndef MTPBKPOCADDRESSSELECT_H
#define MTPBKPOCADDRESSSELECT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <RPbkViewResourceFile.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CPbkPocAddressSelect;
class CPbkContactEngine;
class CPbkContactItem;
class TPbkContactItemField;
class T_AddressAndNumberSelect;
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
_LIT( KMTPbkPocAddressSelectLogPath, "\\logs\\testframework\\MTPbkPocAddressSelect\\" ); 
// Log file
_LIT( KMTPbkPocAddressSelectLogFile, "MTPbkPocAddressSelect.txt" ); 
_LIT( KMTPbkPocAddressSelectLogFileWithTitle, "MTPbkPocAddressSelect_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkPocAddressSelect;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkPocAddressSelect test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkPocAddressSelect) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkPocAddressSelect* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkPocAddressSelect();

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
        CMTPbkPocAddressSelect( CTestModuleIf& aTestModuleIf );

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
		* Focused memory entry does not contain an email address 
		* but it contains a name or a company name.
		*/
		TInt RunExecuteWithContactNameL();
		TInt RunExecuteWithCompanyNameL();

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
		*	Focused memory entry contains only one email address.
		*/
		TInt RunExecuteWithOneEmailL();
		TInt RunExecuteWithOneEmailFocusNullL();
		TInt RunExecuteWithOneEmailDirectlyTrueL();

		/**
		*	Email address has a focus. Focused email address should be
		*	returned directly without showing any email address list.
		*/
		TInt RunExecuteWithMultiEmailL();
		
		/**
		*	The focused memory entry contains more than one email address. 
		*	The email address list of the focused memory entry
		*	is displayed as a list query.
		*/
		TInt RunExecuteWithMultiEmailFocusNullL();

		/**
		*	Default e-mail is set and focus is set to one of the email addresses.
		*	This returns directly the focused email address.
		*/
		TInt RunExecuteWithMultiEmailAndDefaultEmailL();

		/**
		*	Default e-mail address set and focus on one of the email addresses.
		*	The focused email address is directly returned and used as recipient.
		*	Comments: Test  gives default and focus value for every e-mail.
		*/
		TInt RunExecuteMultiEmailWithDefaultEmailAndFocusL();

		/**
		*	Default e-mail address set and focus on one of the email addresses.
		*	The focused email address is directly returned and used as recipient.
		*/
		TInt RunExecuteMultiEmailWithFocusL();

		/**
		*	Default e-mail is set. Returns the default email address focused.
		*/
		TInt RunExecuteMultiEmailWithNoFocusL();

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
		*	Creates and initialises one CPbkContactItem object
		*	with two email addresses.
		*/
		void SetupContactItemWithTwoEmailFieldsL();

		/**
		*	Deletes CPbkContactItem
		*/
        void Teardown();
        
		/**
		*	Handles key events needed in test run
		*/
		void SendKeyEventsL( TKeyCode& aTestedSoftKeyCode );

		/**
		*	Handles key events needed in test run
		*/
		void SendKeyEventsL( );

		/**
		*	Calls CPbkEmailAddressSelect -object's ExecuteLD -method
		*/
		TBool RunExecuteL(
				const CPbkContactItem& aItem, 
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly);
        
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
        
        CPbkPocAddressSelect*	    iPocAddressSelect;
		RPbkViewResourceFile 		iPbkViewResourceFile;
        CPbkContactEngine*			iContactEngine;
        CPbkContactItem*			iContactItem;
        // Filling contact/company information
        T_AddressAndNumberSelect*	iAddressAndNumberSelect;
		// Simulates key events
		CSimulateKeyEvents*			iSimulateKeyEvents;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKPOCADDRESSSELECT_H

// End of File
