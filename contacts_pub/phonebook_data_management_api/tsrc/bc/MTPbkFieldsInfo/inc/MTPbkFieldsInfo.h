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









#ifndef MTPBKFIELDSINFO_H
#define MTPBKFIELDSINFO_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

//  FORWARD DECLARATIONS
class CPbkFieldInfo;
class CPbkFieldsInfo;
class CPbkContactEngine;


#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkFieldsInfoLogPath, "\\logs\\testframework\\MTPbkFieldsInfo\\" ); 
// Log file
_LIT( KMTPbkFieldsInfoLogFile, "MTPbkFieldsInfo.txt" ); 
_LIT( KMTPbkFieldsInfoLogFileWithTitle, "MTPbkFieldsInfo_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkFieldsInfo;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkFieldsInfo test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkFieldsInfo) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkFieldsInfo* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkFieldsInfo();

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

        const CPbkFieldsInfo& FieldsInfo() const;

        /**
         * Type for a test function which tests some property of a single 
         * CPbkFieldInfo object.
         */
        typedef void (*TFieldInfoTest)(const CPbkFieldsInfo&, const CPbkFieldInfo&);

        /**
         * Executes aTest for each field info object in FieldsInfo().
         */
        void ForEachL(TFieldInfoTest aTest);

    private:

        /**
        * C++ default constructor.
        */
        CMTPbkFieldsInfo( CTestModuleIf& aTestModuleIf );

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
        
         void Teardown();
        
         TInt RunTestIsSameL();
         TInt RunTestIsEqualToL();
         TInt RunTestIdsUniqueL();
         TInt RunTestMatchByFieldIdL();
         TInt RunTestMatchesDbExportTypeL();
         TInt RunTestDoesNotMatchWrongStorageTypeL();
         TInt RunTestFieldOrderingL();        
         void RunTestClassSize_CPbkFieldsInfoL();
         void RunPrintFieldData_CPbkFieldsInfoL();
         TInt RunTestFieldData_CPbkFieldsInfoL();
         TInt RunTestContactItemViewDefL();
         TInt RunTestContactItemFieldDefL();
        
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
        
        CPbkContactEngine* iEngine;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKFIELDSINFO_H

// End of File
