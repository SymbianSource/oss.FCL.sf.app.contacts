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









#ifndef MTPBKEMAILADDRESSSELECT_H
#define MTPBKEMAILADDRESSSELECT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>
#include <RPbkViewResourceFile.h>
#include <e32def.h>


//  FORWARD DECLARATIONS
class CPbkEmailAddressSelect;
class CPbkContactEngine;
class CPbkContactItem;
class CPbkAddressSelect;
class T_AddressAndNumberSelect;
class CCoeEnv;
class TPbkContactItemField;
class CSimulateKeyEvents;


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkEmailAddressSelectLogPath, "\\logs\\testframework\\MTPbkEmailAddressSelect\\" ); 
// Log file
_LIT( KMTPbkEmailAddressSelectLogFile, "MTPbkEmailAddressSelect.txt" ); 
_LIT( KMTPbkEmailAddressSelectLogFileWithTitle, "MTPbkEmailAddressSelect_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkEmailAddressSelect;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkEmailAddressSelect test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkEmailAddressSelect) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkEmailAddressSelect* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkEmailAddressSelect();

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
        CMTPbkEmailAddressSelect( CTestModuleIf& aTestModuleIf );

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
		TInt RunExecuteTestCaseDirectlyTrueL();
		TInt RunExecuteTestCaseDirectlyTrue1L();
		TInt RunExecuteTestCaseDirectlyTrue2L();

		/**
		*	Focused memory entry contains only one email address.
		*/
		TInt RunExecuteWithOneEmailL();
		TInt RunExecuteWithOneEmail1L();
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
		*	Calls CPbkEmailAddressSelect -object's ExecuteLD -method
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
        
		// Email address select
        CPbkEmailAddressSelect*		iEmailAddressSelect;
        // Phonebook engine instance
        CPbkContactEngine*			iContactEngine;
        // aItem passed to ExecuteLD
        CPbkContactItem*			iContactItem;
        // Filling contact/company information
        T_AddressAndNumberSelect*		iAddressAndNumberSelect;
        // Control environment
		CCoeEnv& 					iConeEnv;
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

#endif      // MTPBKEMAILADDRESSSELECT_H

// End of File
