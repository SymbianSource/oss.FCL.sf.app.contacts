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









#ifndef MTPBKCONTACTITER_H
#define MTPBKCONTACTITER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

//  INTERNAL INCLUDES
#include <cntdef.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkContactIterLogPath, "\\logs\\testframework\\MTPbkContactIter\\" ); 
// Log file
_LIT( KMTPbkContactIterLogFile, "MTPbkContactIter.txt" ); 
_LIT( KMTPbkContactIterLogFileWithTitle, "MTPbkContactIter_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkContactIter;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkContactIter test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkContactIter) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkContactIter* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkContactIter();

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
        CMTPbkContactIter( CTestModuleIf& aTestModuleIf );

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

		void SetupL(); 
        void SetupWithContactL();
        void SetupWith2ContactL();

		void Teardown(); 

		TInt TestCreation(); 
        TInt TestCreateAndReset();
        TInt TestEmptyDbL();
        TInt TestEmptyDbMinimalReadL();
        TInt TestIterOnDbWithOneContactL();
        TInt TestIterOnDbWithOneContact2L();
        TInt TestIterOnDbWithOneContact3L();
        TInt TestIterOnDbWithOneContact4L();
        TInt TestIterOnDbWithOneContact5L();
        TInt TestIterOnDbWithOneContact6L();
        TInt TestIterOnDbWithOneContactMinimalReadL();
        TInt TestIterOnDbWithOneContactMinimalRead2L();
        TInt TestIterOnDbWithOneContactMinimalRead3L();
        TInt TestIterOnDbWithOneContactMinimalRead4L();
        TInt TestIterOnDbWithOneContactMinimalRead5L();
        TInt TestIterOnDbWithOneContactMinimalRead6L();
        TInt TestIterOnDbWith2ContactsL();
        TInt TestIterOnDbWith2ContactsMinimalReadL();

        
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
        
        // Own: contact engine
        CPbkContactEngine* iEngine;
        // Own: added contact item id
        TContactItemId iAddedContactId;
        TContactItemId iAddedContactId2;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKCONTACTITER_H

// End of File
