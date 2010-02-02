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









#ifndef BCTESTSPEEDDIAL_H
#define BCTESTSPEEDDIAL_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <spdiacontrol.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KBCTestSpeedDialLogPath, "\\logs\\testframework\\BCTestSpeedDial\\" ); 
// Log file
_LIT( KBCTestSpeedDialLogFile, "BCTestSpeedDial.txt" ); 
_LIT( KBCTestSpeedDialLogFileWithTitle, "BCTestSpeedDial_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBCTestSpeedDial;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBCTestSpeedDial test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CBCTestSpeedDial) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTestSpeedDial* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBCTestSpeedDial();

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
        CBCTestSpeedDial( CTestModuleIf& aTestModuleIf );

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
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleForTestSpdialL( CStifItemParser& aItem );
        //test cases
        virtual TInt TestNewLL( CStifItemParser& aItem );
        virtual TInt TestNewL2L( CStifItemParser& aItem );
        virtual TInt TestAssignDialNumberLL( CStifItemParser& aItem );
        virtual TInt TestExecuteLDL( CStifItemParser& aItem );
        virtual TInt TestDialNumberLL( CStifItemParser& aItem );
        virtual TInt TestVoiceMailLL( CStifItemParser& aItem );
        virtual TInt TestPbkEngineL( CStifItemParser& aItem );
        virtual TInt TestIndexL( CStifItemParser& aItem );
        virtual TInt TestNumberL( CStifItemParser& aItem );
        virtual TInt TestContactIdL( CStifItemParser& aItem );
        virtual TInt TestPhoneDialNumberL( CStifItemParser& aItem );
        virtual TInt TestPhoneNumberL( CStifItemParser& aItem );
        virtual TInt TestThumbIndexL( CStifItemParser& aItem );
        virtual TInt TestIconIndexL( CStifItemParser& aItem );
        virtual TInt TestNumberTypeL( CStifItemParser& aItem );
        virtual TInt TestIconArrayL( CStifItemParser& aItem );
        virtual TInt TestCheckingIfPopUpNeededL( CStifItemParser& aItem );
        virtual TInt TestRemoveDialIndexLL( CStifItemParser& aItem );
        virtual TInt TestVMBoxPositionL( CStifItemParser& aItem );
        virtual TInt TestSpdIconIndexL( CStifItemParser& aItem );
        virtual TInt TestDeleteIconArrayL( CStifItemParser& aItem );
        virtual TInt TestReloadIconArrayL( CStifItemParser& aItem );
        virtual TInt TestVoiceMailTypeL( CStifItemParser& aItem );
        virtual TInt TestGetSpdCtrlLastErrorL( CStifItemParser& aItem );

        virtual TInt DeleteSelf( CStifItemParser& aItem );
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
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
    	CSpdiaControl* iControl;
    	RFs iSession;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // BCTESTSPEEDDIAL_H

// End of File
