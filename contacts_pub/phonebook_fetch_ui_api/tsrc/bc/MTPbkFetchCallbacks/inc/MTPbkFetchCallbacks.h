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









#ifndef MTPBKFETCHCALLBACKS_H
#define MTPBKFETCHCALLBACKS_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <RPbkViewResourceFile.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class T_AddressAndNumberSelect;
class CSimulateKeyEvents;
class CPbkSingleEntryFetchDlg;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkFetchCallbacksLogPath, "\\logs\\testframework\\MTPbkFetchCallbacks\\" ); 
// Log file
_LIT( KMTPbkFetchCallbacksLogFile, "MTPbkFetchCallbacks.txt" ); 
_LIT( KMTPbkFetchCallbacksLogFileWithTitle, "MTPbkFetchCallbacks_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkFetchCallbacks;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkFetchCallbacks test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkFetchCallbacks) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkFetchCallbacks* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkFetchCallbacks();

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
        CMTPbkFetchCallbacks( CTestModuleIf& aTestModuleIf );

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
		*	Creates and initialises one empty CPbkContactItem object.
		*	A contact and company name and email address is added.
		*/
		void SetupDefaultContactItemL();

		/**
		*	Deletes CPbkContactItem
		*/
        void Teardown();
        
		/**
		*	Calls CPbkSingleEntryFetchDlg -object's ExecuteLD -method
		*/
        TInt RunExecuteL();
        
		/**
		*	Handles key events needed in test run
		*/
		void SendKeyEventsL( );
        
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
        
		RPbkViewResourceFile		iPbkViewResourceFile;
        // Phonebook engine instance
        CPbkContactEngine*			iContactEngine;
        // For setting up contacts
        CPbkContactItem*			iContactItem;
        // Filling contact/company information
        T_AddressAndNumberSelect*   iAddressAndNumberSelect;
		// Simulates key events
		CSimulateKeyEvents*			iSimulateKeyEvents;
        CPbkSingleEntryFetchDlg*    iSingleEntryFetchDlg;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKFETCHCALLBACKS_H

// End of File
