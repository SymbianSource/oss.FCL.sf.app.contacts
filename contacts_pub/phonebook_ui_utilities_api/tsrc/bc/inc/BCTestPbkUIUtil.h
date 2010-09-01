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









#ifndef BCTESTPBKUIUTIL_H
#define BCTESTPBKUIUTIL_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <RPbkViewResourceFile.h>
#include <PbkFields.hrh> 
#include <RPbkViewResourceFile.h>
#include <cntdef.h>
#include <s32mem.h>
#include <e32def.h>



//  FORWARD DECLARATIONS
class CPbkViewState;
class CPbkContactEngine;
class CPbkContactItem;
class CPbkIconInfoContainer;
class PbkIconUtils;
class TPbkIconInfo;
class CCoeEnv;



// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KBCTestPbkUIUtilLogPath, "\\logs\\testframework\\BCTestPbkUIUtil\\" ); 
// Log file
_LIT( KBCTestPbkUIUtilLogFile, "BCTestPbkUIUtil.txt" ); 
_LIT( KBCTestPbkUIUtilLogFileWithTitle, "BCTestPbkUIUtil_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBCTestPbkUIUtil;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBCTestPbkUIUtil test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CBCTestPbkUIUtil) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTestPbkUIUtil* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBCTestPbkUIUtil();

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
		CBCTestPbkUIUtil( CTestModuleIf& aTestModuleIf );

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
		* Setup for testing
		*/ 
		void SetupL();

		/**
		* Empty Setup
		*/ 
		void EmptySetup();

		/**
		* Setup for testing, creates iFileWriteStream and iFileReadStream
		*/ 
		void FileSetupL();

		/**
		* Deleting objects after testing
		*/ 
		void Teardown();

		/**
		* Empty teardown
		*/ 
		void EmptyTeardown();

		/**
		* Deleting objects after testing, also deleting iFileWriteStream and iFileReadStream
		*/ 
		void FileTeardown();

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
         
		 void RPbkViewResourceFile_SetupL();
         void RPbkViewResourceFile_Teardown();

         
	private:    // Helper

         /**
         * Creating file to use with NewL(RWriteStream) 
         * and NewL(TDesC) tests
         */ 
         void CreateFileForTestingL();

    private:    // Test cases
   
   		/**
		* Test methods are listed below. 
		*/
		/**
		* Testing NewL
		*/ 
		virtual TInt MT_CPbkViewState_NewLL();

		/**
		* Testing NewLC
		*/        
		TInt MT_CPbkViewState_NewLCL();

		/**
		* Testing NewL(RReadStream& aStream) NOTE: Externalize test must
		* be executed before this, because it will create file used with this test
		*/        
		TInt MT_CPbkViewState_NewL_StreamL();

		/**
		* Testing NewLC(RReadStream& aStream) NOTE: Externalize test must
		* be executed before this, because it will create file used with this test
		*/        
		TInt MT_CPbkViewState_NewLC_StreamL();

		/**
		* Testing NewL(const TDesC8& aBuf) NOTE: Externalize test must
		* be executed before this, because it will create file used with this test
		*/        
		TInt MT_CPbkViewState_NewL_TDesCL();

		/**
		* Testing NewLC(const TDesC8& aBuf) NOTE: Externalize test must
		* be executed before this, because it will create file used with this test
		*/        
		TInt MT_CPbkViewState_NewLC_TDesCL();

		/**
		* Testing Uid
		*/        
		TInt MT_CPbkViewState_UidL();

		/**
		* Testing TDataType enum
		*/
		TInt MT_CPbkViewState_TDataTypeEnumL();

		/**
		* Testing TFlags enum
		*/         
		TInt MT_CPbkViewState_TFlagsEnumL();        

		/**
		* Testing SetFocusedContactId and FocusedContactId
		*/
		TInt MT_CPbkViewState_SetGetFocusedContactIdL();

		/**
		* Testing SetTopContactId and TopContactId
		*/        
		TInt MT_CPbkViewState_SetGetTopContactIdL();

		/**
		* Testing SetMarkedContactIds and MarkedContactIds
		*/        
		TInt MT_CPbkViewState_SetGetMarkedContactIdsL();

		/**
		* Testing SetFocusedFieldIndex and FocusedFieldIndex
		*/        
		TInt MT_CPbkViewState_SetGetFocusedFieldIndexL();

		/**
		* Testing SetTopFieldIndex and TopFieldIndex
		*/        
		TInt MT_CPbkViewState_SetGetTopFieldIndexL();

		/**
		* Testing SetFieldDataArray and FieldDataArray
		*/
		TInt MT_CPbkViewState_SetGetFieldDataArrayL();

		/**
		* Testing Reset
		*/        
		TInt MT_CPbkViewState_ResetL();

		/**
		* Testing SetParentContactId and ParentContactId
		*/        
		TInt MT_CPbkViewState_SetGetParentContactIdL();

		/**
		* Testing SetFlagsL and FlagsL
		*/        
		TInt MT_CPbkViewState_SetGetFlagsL();

		/**
		* Testing PackL and UnpackL
		*/        
		TInt MT_CPbkViewState_PackLAndUnpackLL();

		/**
		* Testing PackLC
		*/        
		TInt MT_CPbkViewState_PackLCL();

		/**
		* Testing ExternalizeL, NewL(RReadStream) and InternalizeL
		* 
		* File created here will be used with NewL(RWriteStream) 
		* and NewL(TDesC) tests, so be sure it will stay after, 
		* if making modifications or those  tests will fail
		*/        
		TInt MT_CPbkViewState_ExternalizeLL();

		/**
		* Testing InternalizeL
		* 
		* Internalize function also tested through Externalize and 
		* NewL / NewLC(RReadStream& aStream).
		* 
		* In this case viewState is created from "scratch", 
		* see end of CPbkViewState.h header documentation 
		*/        
		TInt MT_CPbkViewState_InternalizeLL();

		/**
		* Testing == Operator
		*/        
		TInt MT_CPbkViewState_operatorL();

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
         TInt RunPbkIconInfoIdTestL();
         
         /**
          * Run 'TPbkIconInfo::MbmFileName' testcase
          */
         TInt RunMbmFileNameTestL();
         
         /** 
          * Run 'TPbkIconInfo::IconId' testcase
          */
         TInt RunIconIdTestL();
         
         /** 
          * Run 'TPbkIconInfo::MaskId' testcase
          */
         TInt RunMaskIdTestL();
         
         /** 
          * Run 'TPbkIconInfo::SkinId' testcase
          */
         TInt RunSkinIdTestL();
         
         
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
          
         
         //////////////////////////////////////////////////////////////////////
         // RPbkViewResourceFile test methods                                        //
         //////////////////////////////////////////////////////////////////////
         
		 TInt RunLoadResourceTestL();

         TInt RunTest_ClassSizeL();
         
		 void SendTestClassVersion();



    private:    // Data
        
        RFs                         iFs;
        RPbkViewResourceFile        iPbkViewResourceFile;
        TInt                        iDefaultItemsCount;
        TContactItemId              iDefaultContactItemId;
        CPbkViewState*              iViewState;

		CPbkIconInfoContainer*  iContainer; // Icon info container
        const TPbkIconInfo* iIconInfo;      // Icon info    
        
        RPbkViewResourceFile *iViewResourceFile;
    };

#endif      // BCTESTPBKUIUTIL_H

// End of File
