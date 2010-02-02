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









#ifndef MTPBKCONTACTENGINE_H
#define MTPBKCONTACTENGINE_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>
#include <CNTDEF.H>
#include <cntdbobs.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class CContactItemField;
class CPbkFieldInfo;
class CPbkIdleFinder;

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KMTPbkContactEngineLogPath, "\\logs\\testframework\\MTPbkContactEngine\\" ); 
// Log file
_LIT( KMTPbkContactEngineLogFile, "MTPbkContactEngine.txt" ); 
_LIT( KMTPbkContactEngineLogFileWithTitle, "MTPbkContactEngine_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMTPbkContactEngine;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMTPbkContactEngine test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMTPbkContactEngine) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMTPbkContactEngine* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMTPbkContactEngine();

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
        CMTPbkContactEngine( CTestModuleIf& aTestModuleIf );

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
        * Copies file.
        *
        * @param aSourceFileName Source file
        * @param aTargtFileName Destination file
        */
        void CopyFileL(
            const TDesC& aSourceFileName ,
            const TDesC& aTargtFileName);
            
        /**
        * Fills contact item fields with test data.
        *
        * @param aItem Contact item
        * @param aBasePhoneNumber Base phone number
        * @param aConstantNumber Number is not increased if ETrue. 
        *                        EFalse is default
        */
        void FillContactFieldsL(
            CPbkContactItem& aItem, 
            const TInt aBasePhoneNumber,
            const TBool aConstantNumber = EFalse);
           
        /**
        * Test are field types same.
        *
        * @param aField1 Other field to test
        * @param aField2 Other field to test
        */
        TBool FieldTypesMatch(
            const CContactItemField& aField1, 
            const CContactItemField& aField2);
           
        /**
        * Test if aField types match aFieldInfo.
        *
        * @param aField Field to test
        * @param aFieldInfo Field info to test
        * @param aIgnoreContentType Should content type be tested
        */ 
        TBool FieldTypesMatch(
            const CContactItemField& aField, 
            const CPbkFieldInfo& aFieldInfo,
            const TBool aIgnoreContentType);
           
        /**
        * Check that contact items contain same data.
        *
        * @param aContact1 Contact item to test
        * @param aContact2 Contact item to test
        */ 
        TBool ContainsSameData(
            const CPbkContactItem& aContact1,
            const CPbkContactItem& aContact2);
        
        /**
        * Check that contact item fields contain same data.
        *
        * @param aField1 Contact item field to test
        * @param aFieldc Contact item field to test
        */     
        TBool ContainsSameData(
            const CContactItemField& aField1,
            const CContactItemField& aField2);
        
        /**
        * Add the given item to db and contact array.
        * If CContactIdArray is null then adding to it 
        * is ignored.
        * If success returns the TContactItemId of the
        * Added item, else KErrNotFound.
        */    
        TInt AddAndCheckItemToDBAndArrayL(
                CPbkContactItem& aItem,
                CContactIdArray* aContacts = NULL);
        
        /**
        * Delete given item from DB and check that it does not
        * exist there any more.
        *
        * @param cid Contact item's id
        */   
        TBool DeleteAndCheckItemFromDBL(TContactItemId cid);
        
        /**
        * Returns ETrue if iContactId arrays contain the same id.
        *
        * @param aArray1 Array to test
        * @param aArray2 Array to test
        */
        TBool ContactIdArraysMatch(
            const CContactIdArray& aArray1,
            const CContactIdArray& aArray2);
            
        /**
        * Returns ETrue if iContactItem has all the template fields from FieldsInfo
        * and field types and labels are correct.
        *
        * @param aItem Contact item
        * @param aIgnoreContentType ETrue for ingoring content type
        */
        TBool ContactMatchesTemplate
        (CPbkContactItem* aItem, const TBool aIgnoreContentType);
        
        /**
        * Perform AddReadEmptyCard test.
        */ 
        TBool DoAddReadEmptyCardTestL();
        
        /**
        * Perform AddOpenEmptyCard test.
        */ 
        TBool DoAddOpenEmptyCardTestL();
        
        /**
        * Perform AddReadCard test.
        */ 
        TBool DoAddReadCardTestL();
        
        /**
        * Perform AddOpenCard test.
        */ 
        TBool DoAddOpenCardTestL();
        
        /**
        * Perform AssignCheckSpeedDial test.    
        */ 
        TBool DoAssignCheckSpeedDialTestL();
        
        /**
        * Test setting speed dial definition for a single phonefield.
        *
        * @param aItem Contact item
        * @param aCid Contact id
        * @param aFieldId Field id
        * @param aSpeedIndex Index of speed dial
        */ 
        TBool ValidateSettingSpeedDialL
            (CPbkContactItem& aItem, 
            const TInt aCid,
            const TInt aFieldId,
            TInt& aSpeedIndex);
 
    private:  // Test case functions
        
        /**
        * Does nothing. Used as a dummy setup and teardown method
        */
        void DummyMethod();
        
        /**
        * Set the contact engine.
        */ 
        void SetContactEngineL();
        
        /**
        * Setup Logs 'FindLC' test case
        */ 
        void SetupLogsFindLCL();
        
        /**
        * Set the NullAfterDestroy test case.
        */ 
        TInt SetNullAfterDestroyL();
        
        /**
        * Set a filled contact.
        */ 
        void SetFilledContactL();
        
        /**
        * Set the CreateEmptyContact test case.
        */ 
        void SetCreateEmptyContactL();
        
        /**
        * Setup add contact to group
        */ 
        void SetupAddContactToGroupL();
        
        /**
        * Setup delete contacts
        */ 
        void SetupDeleteContactsL();
        
        /**
        * Setup 'FindLC' test case
        */ 
        void SetupFindLCL();
        
        /**
        * Fills contact item text fields with given value.
        * However does not fill Phonenumberfields!
        *
        * @param aItem Contact item to fill
        * @param aTextValue Text value
        */
        void SetTextFieldValueL(
            CPbkContactItem& aItem, 
            const TDesC& aTextValue);
 
        /**
        * Test size of CPbkContactItem class
        */
        TInt RunTestSize();
       
        /**
        * Run create default engine test case.
        */ 
        TInt RunCreateDefaultEngineL(); 
        
        /**
        * Run replace database test case.
        */ 
        TInt RunReplaceDBL();
        
        /**
        * Run create engine test case.
        */ 
        TInt RunCreateEngine();
        TInt RunCreateEngine1();
        
        /**
        * Run Null before create test case.
        */ 
        TInt RunNullBeforeCreate();
        
        /**
        * Run Not Null after create test case.
        */ 
        TInt RunNotNullAfterCreate();
                
        /**
        * Run NullAfterDestroy test case.
        */ 
        TInt RunNullAfterDestroy();
        
        /**
        * Run 'Get database' test case
        */ 
        TInt RunGetDatabase();
        
        /**
        * Run 'Get fieldsinfo' test case
        */ 
        TInt RunGetFieldsInfo();
        
        /**
        * Run 'Get FsSession' test case
        */     
        TInt RunGetFsSession();
            
        /**
        * Run 'Create contact iterator' test case
        */ 
        TInt RunCreateContactIteratorL();
        
        /**
        * Run 'Commit contact' test case
        */ 
        TInt RunCommitContactL();
        
        /**
        * Run 'Delete contact' test case
        */ 
        TInt RunDeleteContactL();
        
        /**
        * Run 'Delete nonexisting contact' test case
        */ 
        TInt RunDeleteNonexistingContactL();
        
        /**
        * Run 'Create contact change notifier' test case
        */ 
        TInt RunCreateContactChangeNotifierL();
        
        /**
        * Run 'Get unnamed title' test case
        */ 
        TInt RunGetUnnamedTitle();
        
        /**
        * Run 'Read minimal contact' test case
        */ 
        TInt RunReadMinimalContactL();
        
        /**
        * Run 'Close contact' test case
        */ 
        TInt RunCloseContactL();
        
        /**
        * Run 'Add and read group' test case
        */ 
        TInt RunAddReadGroupL();
        TInt RunAddReadGroupL1();
        
        /**
        * Run 'Add and open group' test case
        */ 
        TInt RunAddOpenGroupL();
        
        /**
        * Test case for OpenContactGroupL
        */ 
        TInt RunOpenContactGroupL();
        
        /**
        * Run 'Add and delete group' test case
        */ 
        TInt RunAddDeleteGroupL();
        
        /**
        * Run 'Add and delete group' test case
        */ 
        TInt RunAddRemoveContactToGroupL();
                
        /**
        * Run 'Commit contact group' test case
        */ 
        TInt RunCommitContactGroupL();

        /**
        * Run 'Delete contacts' test case.
        */ 
        TInt RunDeleteContactsPlainL();
        
        /**
        * Generic run for DeleteContacts and DeleteContactsOnbackground
        *
        * @param aOnBackGround If True uses CPbkContactEngine::DeleteContactsOnBackgroundL
        *                       instead of CPbkContactEngine::DeleteContactsL
        */ 
        TInt RunDeleteContactsL(TBool aOnBackGround);
        
        /**
        * Run 'Delete contacts on background' test case.
        */ 
        TInt RunDeleteContactsOnBackgroundL();
        
        /**
        * Run 'Delete contacts with empty array'  test case for the
        * plain deletecontacts.
        */ 
        TInt RunDeleteContactsWithEmptyArrayPlainL();
        
        /**
        * Run generic 'Delete contacts with empty array' test case.
        *
        * @param aOnBackGround If True uses CPbkContactEngine::DeleteContactsOnBackgroundL
        *                       instead of CPbkContactEngine::DeleteContactsL
        */ 
        TInt RunDeleteContactsWithEmptyArrayL(TBool aOnBackground);
        
        /**
        * Run 'Delete contacts with empty array'  test case for the
        * background deletecontacts.
        */ 
        TInt RunDeleteContactsWithEmptyArrayBGL();
        
        /**
        * Run 'Delete contacts with partly nonexisting items in array'  test case for the
        * plain deletecontacts.
        */ 
        TInt RunDeleteContactsPartlyArrayPlainL();
        
        /**
        * Run generic 'Delete contacts with empty array whihc has nonexisting items' test case.
        *
        * @param aOnBackGround If True uses CPbkContactEngine::DeleteContactsOnBackgroundL
        *                       instead of CPbkContactEngine::DeleteContactsL
        */ 
        void RunDeletePartlyNonexistingContactsL(TBool aOnBackground);
        
        /**
        * Run 'Delete contacts with partly nonexisting items in array'  test case for the
        * background deletecontacts.
        */ 
        TInt RunDeleteContactsPartlyArrayBGL();
        
        /**
        * Run 'Duplicate contact' test case.
        */ 
        TInt RunDuplicateContactL();
        
        /**
        * Run 'Read customized contact' test case. Reads only defined fields for a
        * contact from the DB.
        */ 
        TInt RunReadCustomizedContactL();
        
        /**
        * Run 'Read contact' test case. Reads only defined fields for a
        * contact from the DB using ContactL.
        */ 
        TInt RunReadContactL();
        
        /**
        * Run 'Read contact' test case. Reads all fields for a
        * contact from the DB using ContactL.
        */
        TInt RunReadContactAllFieldsL();
            
        /**
        * Run 'Get contact title' test case.
        */ 
        TInt RunGetContactTitleL();
        
        /**
        * Run 'Get contact title or null' test case.
        */ 
        TInt RunGetContactTitleOrNullL();
        
        /**
        * Run 'Is title field' test case
        */ 
        TInt RunIsTitleFieldL();
        
        /**
        * Run 'Match phone number' test case
        */
        TInt RunMatchPhoneNumberL();
        
        /**
        * Run 'FindLC' test case
        */ 
        TInt RunFindLCL();
        
        /**
        * Run FindAsyncL test case
        */ 
        TInt RunFindAsyncL();
        
        /**
        * Run NullAfterDestroy test case.
        */ 
        TInt RunCreateEmptyContact();
        
        /**
        * Run AddReadEmptyCard test case.
        */ 
        TInt RunAddReadEmptyCardL();
        
        /**
        * Run AddOpenEmptyCard test case.
        */ 
        TInt RunAddOpenEmptyCardL();
        
        /**
        * Run AddReadCard test case.
        */ 
        TInt RunAddReadCardL();
        
        /**
        * Run AddOpenCard test case.
        */ 
        TInt RunAddOpenCardL();
        
        /**
        * Run 'Open contact' test case
        */ 
        TInt RunOpenContactL();
        
        /**
        * Run Assign speed dial test case.Tests also the 'SpeedDialsChanged' event.
        */ 
        TInt RunAssignSpeedDialL();
        
        /**
        * Run Assign and check speed dial test case.
        */ 
        TInt RunAssignCheckSpeedDialL();
        
        /**
        * Run is speed dial assigned test case.
        */ 
        TInt RunIsSpeedDialAssignedL();
        
        /**
        * Run 'Duplicate contact events' test case.
        */ 
        TInt RunDuplicateContactEventsL();
        
        /**
        * Run Logs 'FindLC' test case. This is a case that may have
        * caused problems in Logs - here seems to work...
        */ 
        TInt RunLogsFindLCL();
        
        /**
        * Teardown engine and item.
        */ 
        void TeardownEngineAndItem();
        
        /**
        * Teardown the contact engine.
        */ 
        void TeardownContactEngine();
        
        /**
        * Teardown delete contacts
        */ 
        void TeardownDeleteContacts();
       
        /**
        * Teardown 'FindLC' test case
        */ 
        void TeardownFindLCTest();
                
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
		class CFindObserver;
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        CPbkContactEngine*          iContactEngine; //Contact engine
        CPbkContactItem*            iContactItem;   //Contact item
        RFs                         iFs;            //Handle to file server session
        TInt                        iContactGroupId;//Contact group id
        TContactItemId              iContactItemId; //Contact item id
        CContactIdArray*            iContactIdArray;//Array of contact ids
        CContactIdArray*            iContacts;      //Array of contacts
        CPbkIdleFinder*             iIdleFinder;    //Phonebook idle finder

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MTPBKCONTACTENGINE_H

// End of File
