/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       
*       Provides methods for the test dll. 
*		Testing binary compatibility of	CPbkViewState.h
*
*/








#ifndef __MT_CPBKVIEWSTATE_H__
#define __MT_CPBKVIEWSTATE_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <RPbkViewResourceFile.h>

#include <PbkFields.hrh> 
#include <cntdef.h>

#include <s32mem.h>

//  INTERNAL INCLUDES



//  FORWARD DECLARATIONS
class CPbkViewState;
class CPbkContactEngine;
class CPbkContactItem;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * Test cases for testing binary compatibility of CPbkViewState.h
 *
 */
NONSHARABLE_CLASS( MT_CPbkViewState )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_CPbkViewState* NewL();
        static MT_CPbkViewState* NewLC();
        /**
         * Destructor
         */
        ~MT_CPbkViewState();

    private:    // Constructors and destructors

        MT_CPbkViewState();
        void ConstructL();
                                                   

    private:    // Setup and Teardown

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

    private:    // Helper

         /**
         * Creating file to use with NewL(RWriteStream) 
         * and NewL(TDesC) tests
         */ 
         void CreateFileForTestingL();


    private:    // Test cases

         /**
         * Testing NewL
         */ 
         void MT_CPbkViewState_NewLL();
        
         /**
         * Testing NewLC
         */        
         void MT_CPbkViewState_NewLCL();
        
         /**
         * Testing NewL(RReadStream& aStream) NOTE: Externalize test must
         * be executed before this, because it will create file used with this test
         */        
         void MT_CPbkViewState_NewL_StreamL();
        
         /**
         * Testing NewLC(RReadStream& aStream) NOTE: Externalize test must
         * be executed before this, because it will create file used with this test
         */        
         void MT_CPbkViewState_NewLC_StreamL();
        
         /**
         * Testing NewL(const TDesC8& aBuf) NOTE: Externalize test must
         * be executed before this, because it will create file used with this test
         */        
         void MT_CPbkViewState_NewL_TDesCL();
        
         /**
         * Testing NewLC(const TDesC8& aBuf) NOTE: Externalize test must
         * be executed before this, because it will create file used with this test
         */        
         void MT_CPbkViewState_NewLC_TDesCL();
        
         /**
         * Testing Uid
         */        
         void MT_CPbkViewState_UidL();
        
         /**
         * Testing TDataType enum
         */
         void MT_CPbkViewState_TDataTypeEnumL();

         /**
         * Testing TFlags enum
         */         
         void MT_CPbkViewState_TFlagsEnumL();        
        
          /**
         * Testing SetFocusedContactId and FocusedContactId
         */
         void MT_CPbkViewState_SetGetFocusedContactIdL();
        
         /**
         * Testing SetTopContactId and TopContactId
         */        
         void MT_CPbkViewState_SetGetTopContactIdL();
        
         /**
         * Testing SetMarkedContactIds and MarkedContactIds
         */        
         void MT_CPbkViewState_SetGetMarkedContactIdsL();
        
         /**
         * Testing SetFocusedFieldIndex and FocusedFieldIndex
         */        
         void MT_CPbkViewState_SetGetFocusedFieldIndexL();
        
         /**
         * Testing SetTopFieldIndex and TopFieldIndex
         */        
         void MT_CPbkViewState_SetGetTopFieldIndexL();
                
         /**
         * Testing SetFieldDataArray and FieldDataArray
         */
         void MT_CPbkViewState_SetGetFieldDataArrayL();
        
         /**
         * Testing Reset
         */        
         void MT_CPbkViewState_ResetL();
        
         /**
         * Testing SetParentContactId and ParentContactId
         */        
         void MT_CPbkViewState_SetGetParentContactIdL();
        
         /**
         * Testing SetFlagsL and FlagsL
         */        
         void MT_CPbkViewState_SetGetFlagsL();
        
         /**
         * Testing PackL and UnpackL
         */        
         void MT_CPbkViewState_PackLAndUnpackLL();
        
         /**
         * Testing PackLC
         */        
         void MT_CPbkViewState_PackLCL();
        
         /**
         * Testing ExternalizeL, NewL(RReadStream) and InternalizeL
         * 
         * File created here will be used with NewL(RWriteStream) 
         * and NewL(TDesC) tests, so be sure it will stay after, 
         * if making modifications or those  tests will fail
         */        
         void MT_CPbkViewState_ExternalizeLL();
        
         /**
         * Testing InternalizeL
         * 
         * Internalize function also tested through Externalize and 
         * NewL / NewLC(RReadStream& aStream).
         * 
         * In this case viewState is created from "scratch", 
         * see end of CPbkViewState.h header documentation 
         */        
         void MT_CPbkViewState_InternalizeLL();
        
         /**
         * Testing == Operator
         */        
         void MT_CPbkViewState_operatorL();
        

    private:    // Data

        RFs                         iFs;
        RPbkViewResourceFile        iPbkViewResourceFile;
        TInt                        iDefaultItemsCount;
        TContactItemId              iDefaultContactItemId;
        CPbkViewState*              iViewState;
        
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __MT_CPBKVIEWSTATE_H__

// End of file
