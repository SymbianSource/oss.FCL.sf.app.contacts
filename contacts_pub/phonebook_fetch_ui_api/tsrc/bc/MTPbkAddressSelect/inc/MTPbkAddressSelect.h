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









#ifndef MTPBKADDRESSSELECT_H
#define MTPBKADDRESSSELECT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include "CPbkAddressSelect.h"
#include <f32file.h>
#include <RPbkViewResourceFile.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkAddressSelectLogPath, "\\logs\\testframework\\MTPbkAddressSelect\\" ); 
// Log file
_LIT( KMTPbkAddressSelectLogFile, "MTPbkAddressSelect.txt" ); 
_LIT( KMTPbkAddressSelectLogFileWithTitle, "MTPbkAddressSelect_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkAddressSelect;
class CAddressSelect;
class CPbkContactEngine;
class CPbkContactItem;
class T_AddressAndNumberSelect;
class CSimulateKeyEvents;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkAddressSelect test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkAddressSelect) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkAddressSelect* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkAddressSelect();

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
        void SetContactReturnValue ( const TBool& aIsSame );
        void SetSelectedAndFocusedField( const TBool& aSelectedField );

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
        CMTPbkAddressSelect( CTestModuleIf& aTestModuleIf );

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
		* Focused memory entry does not contain an email address 
		* but it contains a name or a company name.
		*/
		TInt RunExecuteWithContactNameL();

		/**
		*	Focused memory entry contains only one email address.
		*/
		TInt RunExecuteWithOneEmailL();

		/**
		*	The focused memory entry contains more than one email address. 
		*	The email address list of the focused memory entry
		*	is displayed as a list query.
		*/
		TInt RunExecuteWithMultiEmailFocusNullL();

		/**
		*	Calls CPbkAddressSelect -object's ExecuteLD -method
		*/
		TBool RunExecuteL(const CPbkContactItem& aItem, 
                		const TPbkContactItemField* aFocusedField,
                		const TBool& aCallDefaultDirectly);

        void SendKeyEventsL(
				CSimulateKeyEvents& aSimulator,
				TKeyCode& aTestedSoftKeyCode );
        
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
        
		// Resource-file
		RPbkViewResourceFile	    iPbkViewResourceFile;
        // Phonebook engine instance
        CPbkContactEngine*			iContactEngine;
        // aItem passed to ExecuteLD
        CPbkContactItem*			iContactItem;
        // Filling contact/company information
        T_AddressAndNumberSelect*	iAddressAndNumberSelect;
        TBool                       iIsSame;
        TBool                       iSelectedField;
        CSimulateKeyEvents* 		iKeyEventSimulator;
		/// Own: resource loader
        RConeResourceLoader         iLoader;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKADDRESSSELECT_H

// End of File
