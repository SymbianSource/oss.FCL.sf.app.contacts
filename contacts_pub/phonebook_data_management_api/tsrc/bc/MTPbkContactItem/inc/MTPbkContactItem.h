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









#ifndef MTPBKCONTACTITEM_H
#define MTPBKCONTACTITEM_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <PbkFields.hrh>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class MDefaultTestApi;
class TPbkContactItemField;

#include <e32def.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkContactItemLogPath, "\\logs\\testframework\\MTPbkContactItem\\" ); 
// Log file
_LIT( KMTPbkContactItemLogFile, "MTPbkContactItem.txt" ); 
_LIT( KMTPbkContactItemLogFileWithTitle, "MTPbkContactItem_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkContactItem;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkContactItem test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkContactItem) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkContactItem* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkContactItem();

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
        CMTPbkContactItem( CTestModuleIf& aTestModuleIf );

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

        void FillContactFieldsL(
            CPbkContactItem& aItem, 
            const TInt aBasePhoneNumber,
            const TBool aConstantNumber = EFalse);
        
        TBool AddFieldWithValueL(
	        CPbkContactItem& aDestItem,
	        TPbkFieldId aFieldId,
	        const TDesC &aValue,
            const TPbkFieldLocation& aType = EPbkFieldLocationNone);
            
        TPbkContactItemField* FindFieldForModificationL(
	        CPbkContactItem& aItem,
	        const TPbkFieldId aFieldId,
            const TPbkFieldLocation& aType = EPbkFieldLocationNone);    
                   
        TInt FindFieldMaximumLength(TPbkFieldId aFieldId);
        TBool DoDefaultTestL();
        
        TBool Invariant(CPbkContactEngine& aEngine, const CPbkContactItem& aItem);
        TBool NoDefaultsSet(const CPbkContactItem& aItem);
        TBool DoAllDefaultTestL();
        TBool DoRemoveAllFromEndTest();
        TBool DoRemoveAllFromStartTest();
        TBool DoAddFieldTestL();
        TBool DoUpdateFieldSetTestL();


        /**
        * Test methods are listed below. 
        */

        void Setup();
        void SetNewLTestL();  
        void SetEmptyContactL();  
        void SetFilledContactL(); 
        void SetMultiWorkPhoneContactL(); 
        void SetMultiPhoneContactL();
        void SetDefaultPhoneL(); 
        void SetDefaultMmsL();
        void SetDefaultSmsL();
        void SetVoiceTagL();
        void SetDefaultEmailL();
        void SetDefaultVideoNumberL();
        void SetDefaultPocL();
        void SetDefaultVoipL();
        void SetDefaultEmailOverSmsL();
        
        TInt TestSize();
        TInt RunDefaultTestL();
        TInt RunDefaultTest1L();
        TInt RunDefaultTest2L();
        TInt RunDefaultTest3L();
        TInt RunDefaultTest4L();
        TInt RunDefaultTest5L();
        TInt RunDefaultTest6L();
        TInt RunDefaultTest7L();
        TInt RunDefaultTest8L();
        TInt RunNewLTest();
        TInt RunCreateEmptyContact();
        TInt RunAllDefaultL();
        TInt RunRemoveAllFromEnd();
        TInt RunRemoveAllFromStart();
        TInt RunAddFieldsL();
        TInt RunUpdateFieldSetL();
        TInt RunGetContactTitleL();
        TInt RunGetContactTitleOrNullL();
        TInt RunGetContactIconIndexL();
        TInt RunFindFieldPlainL();
        TInt RunFindFieldIndexedL();
        TInt RunFindFieldWithInfoL();
        TInt RunFindFieldIndexedInfoL();
        TInt RunFindFieldIndex();
        TInt RunFindContactItemFieldIndex();
        TInt RunFindNextFieldWithPhoneNumberExact();
        TInt RunFindNextFieldWithPhoneNumberDigits();
        TInt RunFindNextFieldWithPhoneNumberNot();
        TInt RunFindNextFieldWithTextBasic();
        TInt RunFindNextFieldWithTextNot();
        TInt RunFindSameFieldL();
        TInt RunCanAcceptDataOfTypeL();
        TInt RunAddOrReturnUnUsedFieldL();
        TInt RunGetTypeL();
        TInt RunGetIdL();
        TInt RunGetContactItemL();
        TInt RunGroupsJoinedL();
        TInt RunAddAllFieldsL();
        TInt RunPbkFieldAtL();
        TInt RunPbkFieldCountL();
        TInt RunEqualsTestL();
        
        void Teardown();
        void TeardownContactItem();
        void TeardownDefaultTest();
        
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
        
        //Contact engine 
        CPbkContactEngine*  iContactEngine;
        //Contact item
        CPbkContactItem*    iContactItem;
        //Default test API
        MDefaultTestApi*    iTestApi;
        //Home number buffer
        TBuf<12>            iPhoneNumberHome;
        //Home number index
        TInt                iPhoneNumberHomeIndex;
        //Count of filled fields
        TInt                iFilledFields;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKCONTACTITEM_H

// End of File
