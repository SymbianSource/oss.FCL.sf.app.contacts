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









#ifndef MTPBKICONINFO_H
#define MTPBKICONINFO_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <rpbkviewresourcefile.h>

//  FORWARD DECLARATIONS
class CPbkIconInfoContainer;
class PbkIconUtils;
class TPbkIconInfo;
class CCoeEnv;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkIconInfoLogPath, "\\logs\\testframework\\MTPbkIconInfo\\" ); 
// Log file
_LIT( KMTPbkIconInfoLogFile, "MTPbkIconInfo.txt" ); 
_LIT( KMTPbkIconInfoLogFileWithTitle, "MTPbkIconInfo_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkIconInfo;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkIconInfo test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkIconInfo) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkIconInfo* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkIconInfo();

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
        CMTPbkIconInfo( CTestModuleIf& aTestModuleIf );

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
          * Open resource file
          *
          * @return Resource file class
          */
         RPbkViewResourceFile* OpenResourceFileL();
         
         /**
          * Does nothing
          */
         void DummyMethod();
         
         
         //////////////////////////////////////////////////////////////////////
         // CPbkIconInfoContainer test methods                               //
         //////////////////////////////////////////////////////////////////////
                 
         /**
          * Test "CPbkIconInfoContainer::NewL" with CCoeEnv
          */
         TInt RunConstructContainerWithCoeEnvTestL();
         
         /**
          * Test "CPbkIconInfoContainer::NewL" without CCoeEnv
          */
         TInt RunConstructContainerWithoutCoeEnvTestL();
         
         /**
          * Run 'CPbkIconInfoContainer::Find' testcase
          */
         TInt RunFindTestL();
         
         /**
          * Run 'CPbkIconInfoContainer::LoadBitmapL' testcase
          */
         TInt RunLoadBitmapLTestL();
         
        
         //////////////////////////////////////////////////////////////////////
         // TPbkIconInfo test methods                                        //
         //////////////////////////////////////////////////////////////////////
        
         /**
          * Create TPbkIconInfo 
          */
         void SetupIconInfoL();
    
         /**
          * Teardown TPbkIconInfo 
          */
         void TeardownIconInfo();
         
         /**
          * Run 'TPbkIconInfo::PbkIconInfoId' testcase
          */
         TInt RunPbkIconInfoIdTest();
         
         /**
          * Run 'TPbkIconInfo::MbmFileName' testcase
          */
         TInt RunMbmFileNameTest();
         
         /** 
          * Run 'TPbkIconInfo::IconId' testcase
          */
         TInt RunIconIdTest();
         
         /** 
          * Run 'TPbkIconInfo::MaskId' testcase
          */
         TInt RunMaskIdTest();
         
         /** 
          * Run 'TPbkIconInfo::SkinId' testcase
          */
         TInt RunSkinIdTest();
         
         
         //////////////////////////////////////////////////////////////////////
         // PbkIconUtils test methods                                        //
         //////////////////////////////////////////////////////////////////////
         
         /**
          * Run 'PbkIconUtils::CreateIconL' testcase
          */
         TInt RunCreateIconLTestL();
         
         /**
          * Run 'PbkIconUtils::CreateIconLC' testcase
          */
         TInt RunCreateIconLCTestL();
         
        
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
        
        CPbkIconInfoContainer*  iContainer; // Icon info container
        
        const TPbkIconInfo* iIconInfo;      // Icon info    

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKICONINFO_H

// End of File
