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









#ifndef MTPBKIDLEFINDER_H
#define MTPBKIDLEFINDER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
class CPbkContactEngine;
class CPbkContactItem;
class CPbkIdleFinder;
class CContactIdArray;
class CContactEngineEventQueue;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkIdleFinderLogPath, "\\logs\\testframework\\MTPbkIdleFinder\\" ); 
// Log file
_LIT( KMTPbkIdleFinderLogFile, "MTPbkIdleFinder.txt" ); 
_LIT( KMTPbkIdleFinderLogFileWithTitle, "MTPbkIdleFinder_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkIdleFinder;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkIdleFinder test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkIdleFinder) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkIdleFinder* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkIdleFinder();

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
        CMTPbkIdleFinder( CTestModuleIf& aTestModuleIf );

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

         void SetTextFieldValueL(CPbkContactItem& aItem, 
                                 const TDesC& aTextValue);

         void AddTestContactItemsL(TInt count);

         void SetupL();
         void SetupEmptyL();
        
         void Teardown();
        
         TInt MT_CPbkIdleFinder_FindL();
         TInt MT_CPbkIdleFinder_Find1L();

         void MT_CPbkIdleFinder_IsCompleteL();
        
        
         void MT_CPbkIdleFinder_TakeContactIdsL();
        
        
         void MT_CPbkIdleFinder_ErrorL();
        
        
         void MT_CPbkIdleFinder_IdleFinderL();
        
        
         void MT_CPbkIdleFinder_FieldDefL();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

 public:
        class CFindObserver;
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        CPbkContactEngine* iContactEngine;     // owns
        CPbkIdleFinder*    iIdleFinder;        // owns
        CPbkContactItem*   iContactItem;       // owns
        CContactIdArray*   iFoundContacts;
        TInt               iTestContactCount;
        TBool              iRunning;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKIDLEFINDER_H

// End of File
