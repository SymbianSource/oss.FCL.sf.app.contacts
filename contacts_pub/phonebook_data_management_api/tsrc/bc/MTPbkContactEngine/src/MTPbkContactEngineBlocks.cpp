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









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MTPbkContactEngine.h"
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <CPbkIdleFinder.h>
#include "CContactEngineEventQueue.h"

#include <pbkfields.hrh>
#include <CNTDB.H>

_LIT(KPanicText, "CMTPbkContactEngine");
_LIT(KTestDBName,"c:MT_PbkContactEngine.cdb");
_LIT(KFindText_1, "Find this");


// phone number matching - how many digits are used
const TInt phoneNumberMatching = 8;
// size of CPbkContactEngine class
const TInt KClassSize = 52;

// panic codes
enum TPanicCodes
    {
    KPanicUnspportedFieldType = 1
    };

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMTPbkContactEngine::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkContactEngine::Delete() 
    {

    }


//---------------------------------------------------------------
//----------------- Helper classes ---------------------------
//---------------------------------------------------------------


/**
 * Helper class for idle find observer check.
 */
class CMTPbkContactEngine::CFindObserver :
        public CBase, public MIdleFindObserver
    {
    public: // constructor & destructor
        CFindObserver();
        ~CFindObserver();

    public: // interface
	    void IdleFindCallback();
        CContactIdArray* FindSyncL(
            CPbkContactEngine& aEngine,
            const TDesC& aText,
            const CPbkFieldIdArray* aFieldTypes=NULL);

    private: // members
        CPbkIdleFinder* iIdleFinder; // owns
        CContactIdArray* iFoundContacts;
        TBool iRunning;
    };
/**
 * Default constructor
 */
CMTPbkContactEngine::CFindObserver::CFindObserver()
    {
    }

/**
 * Destructor
 */
CMTPbkContactEngine::CFindObserver::~CFindObserver()
    {
    delete iIdleFinder;
    }


/**
 * Callback for idle find
 */
void CMTPbkContactEngine::CFindObserver::IdleFindCallback()
    {
    ASSERT(iIdleFinder);

    if (iIdleFinder->IsComplete())
        {
        if (iIdleFinder->Error() != KErrNone)
            {
            User::Panic(KPanicText, 0);
            }
        else
            {
            iFoundContacts = iIdleFinder->TakeContactIds();
            if (iRunning)
                {
                CActiveScheduler::Stop();
                }
            }
        delete iIdleFinder;
        iIdleFinder = NULL;
        }
    }

/**
 * Find synchronoysly
 */
CContactIdArray* CMTPbkContactEngine::CFindObserver::FindSyncL(
        CPbkContactEngine& aEngine,
        const TDesC& aText,
        const CPbkFieldIdArray* aFieldTypes)
    {
    if (iIdleFinder)
        {
        delete iIdleFinder;
        iIdleFinder = NULL;
        }

    iIdleFinder =  aEngine.FindAsyncL(aText, aFieldTypes, this);
    iRunning = ETrue;
    CActiveScheduler::Start();

    return iFoundContacts;
    }



// -----------------------------------------------------------------------------
// CMTPbkContactEngine::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkContactEngine::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Create_default_engine", CMTPbkContactEngine::RunCreateDefaultEngineL ),
        ENTRY( "Test_size_of_CPbkContactEngine_class", CMTPbkContactEngine::RunTestSize ),
        ENTRY( "Create_and_delete_CPbkContactEngine", CMTPbkContactEngine::RunCreateEngine ),
        ENTRY( "Null_before_create", CMTPbkContactEngine::RunNullBeforeCreate ),
        ENTRY( "Not_Null_after_create", CMTPbkContactEngine::RunNotNullAfterCreate ),
        ENTRY( "Null_After_Destroy", CMTPbkContactEngine::RunNullAfterDestroy ),
        ENTRY( "Replace_database", CMTPbkContactEngine::RunReplaceDBL ),
        ENTRY( "Create_and_delete_CPbkContactEngine", CMTPbkContactEngine::RunCreateEngine1 ),
        ENTRY( "Get_Database", CMTPbkContactEngine::RunGetDatabase ),
        ENTRY( "Get_FielsInfo", CMTPbkContactEngine::RunGetFieldsInfo ),
        ENTRY( "Get_file_server_session", CMTPbkContactEngine::RunGetFsSession ),
        ENTRY( "Create_iterator", CMTPbkContactEngine::RunCreateContactIteratorL ),
        ENTRY( "Create_contact_change_notifier", CMTPbkContactEngine::RunCreateContactChangeNotifierL ),
        ENTRY( "Get_unnamed_title", CMTPbkContactEngine::RunGetUnnamedTitle ),
        ENTRY( "Read_minimal_contact", CMTPbkContactEngine::RunReadMinimalContactL ),
        ENTRY( "Commit_contact", CMTPbkContactEngine::RunCommitContactL ),
        ENTRY( "Close_contact", CMTPbkContactEngine::RunCloseContactL ),
        ENTRY( "Delete_contact", CMTPbkContactEngine::RunDeleteContactL ),
        ENTRY( "Delete_nonexisting_contact", CMTPbkContactEngine::RunDeleteNonexistingContactL ),
        ENTRY( "Add/read_group", CMTPbkContactEngine::RunAddReadGroupL ),
        ENTRY( "Add/open_group", CMTPbkContactEngine::RunAddReadGroupL1 ),
        ENTRY( "Add/delete_group", CMTPbkContactEngine::RunAddDeleteGroupL ),
        ENTRY( "Add/remove_contact_to_group", CMTPbkContactEngine::RunAddRemoveContactToGroupL ),
        ENTRY( "Commit_contact_group", CMTPbkContactEngine::RunCommitContactGroupL ),
        ENTRY( "Add/open_group1", CMTPbkContactEngine::RunOpenContactGroupL ),
        ENTRY( "Delete_contacts", CMTPbkContactEngine::RunDeleteContactsPlainL ),
        ENTRY( "Delete_contacts_on_background", CMTPbkContactEngine::RunDeleteContactsOnBackgroundL ),
        ENTRY( "Delete_contacts_with_empty_array_plain", CMTPbkContactEngine::RunDeleteContactsWithEmptyArrayPlainL ),
        ENTRY( "Delete_contacts_with_empty_array_BG", CMTPbkContactEngine::RunDeleteContactsWithEmptyArrayBGL ),
        ENTRY( "Delete_contacts_with_partly_array_plain", CMTPbkContactEngine::RunDeleteContactsPartlyArrayPlainL ),
        ENTRY( "Delete_contacts_with_partly_array_BG", CMTPbkContactEngine::RunDeleteContactsPartlyArrayBGL ),
        ENTRY( "Duplicate_contact", CMTPbkContactEngine::RunDuplicateContactL ),
        ENTRY( "Read_customized_contact", CMTPbkContactEngine::RunReadCustomizedContactL ),
        ENTRY( "Read_contact_Defined_fields", CMTPbkContactEngine::RunReadContactL ),
        ENTRY( "Read_contact_All_fields", CMTPbkContactEngine::RunReadContactAllFieldsL ),
        ENTRY( "Get_contact_title", CMTPbkContactEngine::RunGetContactTitleL ),
        ENTRY( "Get_contact_title_or_null", CMTPbkContactEngine::RunGetContactTitleOrNullL ),
        ENTRY( "Is_title_field", CMTPbkContactEngine::RunIsTitleFieldL ),
        ENTRY( "Match_phone_number", CMTPbkContactEngine::RunMatchPhoneNumberL ),
        ENTRY( "Test_RunFindLC", CMTPbkContactEngine::RunFindLCL ),
        ENTRY( "Test_FindAsyncL", CMTPbkContactEngine::RunFindAsyncL ),
        ENTRY( "Create_Empty_Contact", CMTPbkContactEngine::RunCreateEmptyContact ),
        ENTRY( "Add/Read_empty_card", CMTPbkContactEngine::RunAddReadEmptyCardL ),
        ENTRY( "Add/Open_empty_card", CMTPbkContactEngine::RunAddOpenEmptyCardL ),
        ENTRY( "Add/Read_filled_card", CMTPbkContactEngine::RunAddReadCardL ),
        ENTRY( "Add/Open_filled_card", CMTPbkContactEngine::RunAddOpenCardL ),
        ENTRY( "Add/Open_filled_card1", CMTPbkContactEngine::RunOpenContactL ),
        ENTRY( "Assign_speed_dial", CMTPbkContactEngine::RunAssignSpeedDialL ),
        ENTRY( "Test_is_speed_dial_assigned", CMTPbkContactEngine::RunIsSpeedDialAssignedL ),
        ENTRY( "Logs_FindLC_case", CMTPbkContactEngine::RunLogsFindLCL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/**
 * Copies file.
 *
 * @param aSourceFileName Source file
 * @param aTargtFileName Destination file
 */
void CMTPbkContactEngine::CopyFileL(
        const TDesC& aSourceFileName,
        const TDesC& aTargtFileName)
    {

    RFile inFile;
	if(inFile.Open(iFs, aSourceFileName, EFileRead) != KErrNone)
		{
        User::Leave(KErrGeneral);
		}

    RFile outFile;
	if(outFile.Replace(iFs, aTargtFileName, EFileWrite) != KErrNone)
        {
        User::Leave(KErrGeneral);
        }

    TBuf8<40> buf;
    do
        {
	    inFile.Read(buf);
        outFile.Write(buf);
        } while (buf.Length() > 0);

	inFile.Close();
    outFile.Close();
    }

/**
 * Fills contact item fields with test data.
 */
void CMTPbkContactEngine::FillContactFieldsL(
        CPbkContactItem& aItem,
        const TInt aBasePhoneNumber,
        const TBool aConstantNumber)
    {
    _LIT(KFillText, "TESTDATA");

    for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            // Set a different number for each phonenumber;
            // just simply set the loop counter
            TInt number = aBasePhoneNumber;
            if (!aConstantNumber)
                {
                number = number+ i;
                }

            TBuf<10> phoneNumber;
            phoneNumber.Num(number);
            field.TextStorage()->SetTextL(phoneNumber);
            }
        else
            {
            switch (field.StorageType())
                {
                case KStorageTypeText:
                    {
                    field.TextStorage()->SetTextL(KFillText);
                    break;
                    }
                case KStorageTypeDateTime:
                    {
                    TTime now;
                    now.UniversalTime();
                    field.DateTimeStorage()->SetTime(now);
                    break;
                    }
                default:
                    {
                    User::Panic(KPanicText, KPanicUnspportedFieldType);
                    break;
                    }
                }
            }
        }
    }

/*
 * Returns ETrue if aField types match aFieldInfo.
 */
TBool CMTPbkContactEngine::FieldTypesMatch
        (const CContactItemField& aField,
        const CPbkFieldInfo& aFieldInfo,
        const TBool aIgnoreContentType)
    {
    TBool result =
        (aField.StorageType() == aFieldInfo.FieldStorageType() &&
        aField.IsHidden() == aFieldInfo.IsHidden() &&
        aField.IsReadOnly() == aFieldInfo.IsReadOnly() &&
        aField.IsDisabled() == aFieldInfo.IsDisabled());

    if (!aIgnoreContentType)
        {
        result = result && (aField.ContentType() == aFieldInfo.ContentType());
        }

    return result;
    }

/**
 * Returns ETrue if aField1 and aField2 types match.
 */
TBool CMTPbkContactEngine::FieldTypesMatch
        (const CContactItemField& aField1,
        const CContactItemField& aField2)
    {
    TBool result =
        (aField1.StorageType() == aField2.StorageType() &&
        aField1.IsHidden() == aField2.IsHidden() &&
        aField1.IsReadOnly() == aField2.IsReadOnly() &&
        aField1.IsDisabled() == aField2.IsDisabled() &&
        aField1.ContentType() == aField2.ContentType());

    return result;
    }

/**
 * Check that contact items contain same data.
 */
TBool CMTPbkContactEngine::ContainsSameData
        (const CPbkContactItem& aContact1,
        const CPbkContactItem& aContact2)
    {
    if (aContact1.CardFields().Count() != aContact2.CardFields().Count())
        {
        //("Different field counts %d %d\n"),
        //    aContact1.CardFields().Count(),
        //    aContact2.CardFields().Count());
        return EFalse;
        }

    for (TInt i = 0; i < aContact1.CardFields().Count(); ++i)
        {
        const CContactItemField& field1 = aContact1.CardFields()[i].ItemField();
        const CContactItemField& field2 = aContact2.CardFields()[i].ItemField();
        if (!FieldTypesMatch(field1, field2))
            {
            //("Field type dont match <%d> <%d>\n"),
            //    &field1.Label(), &field2.Label());
            return EFalse;
            }

        if (field1.Label().CompareF(field2.Label()) != 0)
            {
            //("Field labels dont match <%S> <%S>\n"),
            //    &field1.Label(), &field2.Label());
            return EFalse;
            }

        if (!ContainsSameData(field1,field2))
            {
            //("Field data dont match <%S> <%S>\n"),
            //    &field1.Label(), &field2.Label());
            return EFalse;
            }
        }

    return ETrue;
    }

/**
 * Check that contact items fields contain same data.
 */
TBool CMTPbkContactEngine::ContainsSameData
        (const CContactItemField& aField1,
        const CContactItemField& aField2)
    {
    if (aField1.StorageType() != aField2.StorageType())
        return EFalse;

    switch (aField1.StorageType())
        {
        case KStorageTypeText:
            {
            if (aField1.TextStorage()->Text() == aField2.TextStorage()->Text())
                return ETrue;
            else
                {
                //("Different field data %S/%S, %S/%S\n"),
                //    &aField1.Label(),
                //    &aField1.TextStorage()->Text(),
                //    &aField2.Label(),
                //    &aField2.TextStorage()->Text());
                return EFalse;
                }
            }
        case KStorageTypeDateTime:
            {
            if (aField1.DateTimeStorage()->Time() == aField2.DateTimeStorage()->Time())
                return ETrue;
            else
                {
                //"Different dates %S/%S\n"),
                //    &aField1.Label(),
                //    &aField2.Label());
                return EFalse;
                }
            }
        default:
            {
            User::Panic(KPanicText, KPanicUnspportedFieldType);
            return EFalse;
            }
        }
    }

/**
 * Add the given item to db and contact array.
 * If CContactIdArray is null then adding to it
 * is ignored.
 * If success returns the TContactItemId of the
 * Added item, else KErrNotFound.
 */
TInt CMTPbkContactEngine::AddAndCheckItemToDBAndArrayL(
        CPbkContactItem& aItem,
        CContactIdArray* aContacts)
    {
    TContactItemId cid;
    cid = iContactEngine->AddNewContactL(aItem);

    CPbkContactItem* item;
    item = iContactEngine->ReadContactLC(cid);

    TInt retval;
    if (item)
        {
        if (aContacts)
            {
            aContacts->AddL(cid);
            }
        retval = cid;
        }
    else
        {
        retval = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(item);

    return retval;
    }

/**
 * Delete given item from DB and check that it does not
 * exist there any more.
 */
TBool CMTPbkContactEngine::DeleteAndCheckItemFromDBL(TContactItemId cid)
    {
    iContactEngine->DeleteContactL(cid);

    CPbkContactItem* item = NULL;
    TBool result = EFalse;
    TRAPD(err, item = iContactEngine->ReadContactLC(cid);CleanupStack::Pop(item));

    switch (err)
        {
        case KErrNotFound:
            {
            result = ETrue;
            break;
            }
        case KErrNoMemory:
            {
            User::Leave(err);
            break;
            }
        default:
            {
            result = EFalse;
            break;
            }
        }

    return result;
    }

/**
 * Returns ETrue if iContactId arrays contain the same id.
 */
TBool CMTPbkContactEngine::ContactIdArraysMatch(
            const CContactIdArray& aArray1,
            const CContactIdArray& aArray2)
    {
    if (aArray1.Count() != aArray2.Count())
        {
        return EFalse;
        }

    for (TInt i = 0; i < aArray2.Count();++i)
        {
        if (aArray1.Find(aArray2[i]) == KErrNotFound)
            {
            return EFalse;
            }
        }
    // everything ok!
    return ETrue;
    }

/**
 * Returns ETrue if iContactItem has all the template fields from FieldsInfo
 * and field types and labels are correct.
 */
TBool CMTPbkContactEngine::ContactMatchesTemplate
        (CPbkContactItem* aItem, const TBool aIgnoreContentType)
    {
    const CPbkFieldsInfo& fieldsInfo = iContactEngine->FieldsInfo();
    TBool result = ETrue;

    TInt templateCount = 0;
    for (TInt i = 0; i < fieldsInfo.Count(); ++i)
        {
        CPbkFieldInfo* fieldInfo = fieldsInfo[i];
        if (fieldInfo->TemplateField())
            {
            TPbkContactItemField* field = aItem->FindField(*fieldInfo);
            // Check that all template fields are present
            if (field)
                {
                // Check field type
                if (!FieldTypesMatch(field->ItemField(), *fieldInfo, aIgnoreContentType))
                    {
                    //("field type dont match. Field <%S>\n"), &field->Label()
                    result = EFalse;
                    }
                // Check field label
                if (field->Label().CompareF(fieldInfo->FieldName()) != 0)
                    {
                    //("field label dont match. Field <%S>\n"), &field->Label()
                    result = EFalse;
                    }
                }
            else
                {
                //("Template field %S not found\n"), &fieldInfo->FieldName();
                result = EFalse;
                }
            ++templateCount;
            }
        }

    // Only template fields should be present
    if (aItem->CardFields().Count() != templateCount)
        {
        //("template field count is incorrect\n");
        result = EFalse;
        }

    return result;
    }

/**
 * Perform AddReadEmptyCard test.
 */
TBool CMTPbkContactEngine::DoAddReadEmptyCardTestL()
    {
    TBool result = ContactMatchesTemplate(iContactItem,EFalse);
    if (result)
        {
        TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
        CPbkContactItem* item2 = iContactEngine->ReadContactLC(cid);
        result = !ContainsSameData(*iContactItem, *item2);
        CleanupStack::PopAndDestroy(item2);
        iContactEngine->DeleteContactL(cid);
        }

    return result;
    }

/**
 * Perform AddOpenEmptyCard test.
 */
TBool CMTPbkContactEngine::DoAddOpenEmptyCardTestL()
    {
    TBool result = ContactMatchesTemplate(iContactItem,EFalse);
    if (result)
        {
        TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
        CPbkContactItem* item2 = iContactEngine->OpenContactLCX(cid);
        result = !ContainsSameData(*iContactItem, *item2);
        CleanupStack::PopAndDestroy(2);  // item2, lock
        iContactEngine->DeleteContactL(cid);
        }

    return result;
    }

/**
 * Perform AddReadCard test.
 */
TBool CMTPbkContactEngine::DoAddReadCardTestL()
    {
    TBool result = ContactMatchesTemplate(iContactItem,EFalse);
    if (result)
        {
        TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
        CPbkContactItem* item2 = iContactEngine->ReadContactLC(cid);
        result = ContainsSameData(*iContactItem, *item2);
        CleanupStack::PopAndDestroy(item2);
        iContactEngine->DeleteContactL(cid);
        }

    return result;
    }

/**
 * Perform AddOpenCard test.
 */
TBool CMTPbkContactEngine::DoAddOpenCardTestL()
    {
    TBool result = ContactMatchesTemplate(iContactItem,EFalse);
    if (result)
        {
        TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
        CPbkContactItem* item2 = iContactEngine->OpenContactLCX(cid);
        result = ContainsSameData(*iContactItem, *item2);
        CleanupStack::PopAndDestroy(2);  // item2, lock
        iContactEngine->DeleteContactL(cid);
        }

    return result;
    }

/**
 * Run 'Open contact' test case
 */
TInt CMTPbkContactEngine::RunOpenContactL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
    CPbkContactItem* item = iContactEngine->OpenContactL(cid);

    ASSERT(ContainsSameData(*iContactItem, *item));

    iContactEngine->CloseContactL(cid);
    delete item;
    TeardownEngineAndItem();
    return KErrNone;
    }

//---------------------------------------------------------------
//----------------- TEST CASE METHODS ---------------------------
//---------------------------------------------------------------

/**
 * Does nothing. Used as a dummy setup and teardown method
 */
 void CMTPbkContactEngine::DummyMethod()
    {
    }

/**
 * Set the contact engine.
 */
void CMTPbkContactEngine::SetContactEngineL()
    {
    iContactEngine = CPbkContactEngine::NewL(KTestDBName,EFalse);
    }

/**
 * Setup Logs 'FindLC' test case
 */
void CMTPbkContactEngine::SetupLogsFindLCL()
    {
    iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);
    }

/**
 * Set the CreateEmptyContact test case.
 */
void CMTPbkContactEngine::SetCreateEmptyContactL()
    {
    SetContactEngineL();
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

/**
 * Set the NullAfterDestroy test case.
 */
TInt CMTPbkContactEngine::SetNullAfterDestroyL()
    {
    SetContactEngineL();
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Set a filled contact.
 */
void CMTPbkContactEngine::SetFilledContactL()
    {
    SetCreateEmptyContactL();
    FillContactFieldsL(*iContactItem,0);
    }

/**
 * Setup add contact to group
 */
void CMTPbkContactEngine::SetupAddContactToGroupL()
    {
    SetFilledContactL();

    // Add contact group
    _LIT(KGroupName,"Another3TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    // get the id of the group
    iContactGroupId = cgroup->Id();
    CleanupStack::PopAndDestroy(cgroup);

    iContactItemId = iContactEngine->AddNewContactL(*iContactItem);
    }

/**
 * Setup delete contacts
 */
void CMTPbkContactEngine::SetupDeleteContactsL()
    {
    SetFilledContactL();

    iContactIdArray = CContactIdArray::NewL();

    // add some items to db
    const TInt count = 5;

    for (TInt i = 0; i < count; ++i)
        {
        if (!(AddAndCheckItemToDBAndArrayL(*iContactItem, iContactIdArray) > 0))
            User::Leave(KErrGeneral);
        }
    }

/**
 * Setup 'FindLC' test case
 */
void CMTPbkContactEngine::SetupFindLCL()
    {
    SetFilledContactL();

    // store some items with the text value to find
    TContactItemId cid;
    iContacts = CContactIdArray::NewL();
    CPbkContactItem* okContact = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(okContact);
    SetTextFieldValueL(*okContact, KFindText_1);

    const TInt okCount = 3;
    TInt i;
    for (i = 0; i < okCount; ++i)
        {
        cid = iContactEngine->AddNewContactL(*okContact);
        iContacts->AddL(cid);
        }
    CleanupStack::PopAndDestroy(okContact);

    // add some filled-but-not-ok contacts
    const TInt filledCount = 3;
    for (i = 0; i < filledCount; ++i )
        {
        cid = iContactEngine->AddNewContactL(*iContactItem);
        }

    // add some empty contacts
    CPbkContactItem* emptyContact = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyContact);
    const TInt emptyCount = 4;
    for (i = 0; i < emptyCount; ++i )
        {
        cid = iContactEngine->AddNewContactL(*emptyContact);
        }
    CleanupStack::PopAndDestroy(emptyContact);
    }

/**
 * Fills contact item text fields with given value.
 * However does not fill Phonenumberfields!
 */
void CMTPbkContactEngine::SetTextFieldValueL(
        CPbkContactItem& aItem,
        const TDesC& aTextValue)
    {
    for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];
        if ((!field.FieldInfo().IsPhoneNumberField()) &&
                (field.StorageType() == KStorageTypeText))
            {
            field.TextStorage()->SetTextL(aTextValue);
            }
        }
    }

/**
 * Test size of CPbkContactItem class
 */
TInt CMTPbkContactEngine::RunTestSize()
    {
    ASSERT( sizeof(CPbkContactEngine) >= KClassSize);
    return KErrNone;
    }

/**
 * Run create engine test case.
 */
TInt CMTPbkContactEngine::RunCreateEngine()
    {
	SetContactEngineL();
    ASSERT(iContactEngine);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run create engine test case.
 */
TInt CMTPbkContactEngine::RunCreateEngine1()
    {
	SetContactEngineL();
    ASSERT(iContactEngine);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run create default engine test case.
 */
TInt CMTPbkContactEngine::RunCreateDefaultEngineL()
    {
    CPbkContactEngine* contactEngine = CPbkContactEngine::NewL();
    ASSERT(contactEngine);
    delete contactEngine;
    return KErrNone;    
    }

/**
 * Run replace database test case.
 */
TInt CMTPbkContactEngine::RunReplaceDBL()
    {
    // DB cannot be replaced since it is in use,
    // however we do not even test asserting it returning KErrInUse,
    // since in some emulator test environment the test might pass
    SetContactEngineL();
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run Null before create test case.
 */
TInt CMTPbkContactEngine::RunNullBeforeCreate()
    {
    ASSERT( !CPbkContactEngine::Static() );
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run Not Null after create test case.
 */
TInt CMTPbkContactEngine::RunNotNullAfterCreate()
    {
    SetContactEngineL();
    ASSERT(CPbkContactEngine::Static() == iContactEngine);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run NullAfterDestroy test case.
 */
TInt CMTPbkContactEngine::RunNullAfterDestroy()
    {
    SetNullAfterDestroyL();
    ASSERT( !CPbkContactEngine::Static() );
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Get database' test case
 */
TInt CMTPbkContactEngine::RunGetDatabase()
    {
    SetContactEngineL();
    CContactDatabase& dbase = iContactEngine->Database();

    // just call a method of the CContactDatabase
    ASSERT(dbase.MachineId() != 0);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Get fieldsinfo' test case
 */
TInt CMTPbkContactEngine::RunGetFieldsInfo()
    {
    SetContactEngineL();
    const CPbkFieldsInfo& finfo = iContactEngine->FieldsInfo();

    ASSERT( finfo.Count() != 0);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Get FsSession' test case
 */
TInt CMTPbkContactEngine::RunGetFsSession()
    {
    SetContactEngineL();
    const RFs& fs = iContactEngine->FsSession();
    //just test file server session does something
    TVersion version = fs.Version();

    ASSERT( version.iMajor >= 0 );
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Create contact iterator' test case
 */
TInt CMTPbkContactEngine::RunCreateContactIteratorL()
    {
    SetContactEngineL();
    CPbkContactIter* iterator = iContactEngine->CreateContactIteratorLC();
    ASSERT(iterator);
    CleanupStack::PopAndDestroy(iterator);

    CPbkContactIter* iterator2 = iContactEngine->CreateContactIteratorLC(ETrue);
    ASSERT(iterator2);
    CleanupStack::PopAndDestroy(iterator2);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Create contact change notifier' test case
 */
TInt CMTPbkContactEngine::RunCreateContactChangeNotifierL()
    {
    SetContactEngineL();
    CContactEngineEventQueue* observer = CContactEngineEventQueue::NewL(iContactEngine);
    CleanupStack::PushL(observer);

    CPbkContactChangeNotifier* notifier = iContactEngine->CreateContactChangeNotifierL(observer);
    CleanupStack::PushL(notifier);
    ASSERT(notifier);
    CleanupStack::PopAndDestroy(2); //observer, notifier
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Get unnamed title' test case
 */
TInt CMTPbkContactEngine::RunGetUnnamedTitle()
    {
    SetContactEngineL();
    const TDesC& title = iContactEngine->UnnamedTitle();
    ASSERT(title.Compare(KNullDesC) != 0);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Read minimal contact' test case
 */
TInt CMTPbkContactEngine::RunReadMinimalContactL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
    CPbkContactItem* item2 = iContactEngine->ReadMinimalContactLC(cid);

    ASSERT(item2);
    CleanupStack::PopAndDestroy(item2);
    iContactEngine->DeleteContactL(cid);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Commit contact' test case
 */
TInt CMTPbkContactEngine::RunCommitContactL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
    CPbkContactItem* item2 = iContactEngine->OpenContactLCX(cid);

    // The new value for phonenumber
    TBuf<7> phoneNumber;
    phoneNumber.Num(5664540);

    TInt i;
    for (i = 0; i < item2->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = iContactItem->CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            field.TextStorage()->SetTextL(phoneNumber);
            }
        }

    iContactEngine->CommitContactL(*item2);
    CleanupStack::PopAndDestroy(2); // lock, item2

    // Read the contact again to check the contents
    CPbkContactItem* item3 = iContactEngine->ReadContactLC(cid);
    for (i = 0; i < item3->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = iContactItem->CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            // check that the previously set data was saved
            ASSERT (field.TextStorage()->Text() == phoneNumber);
            }
        }

    CleanupStack::PopAndDestroy(item3); // item3
    iContactEngine->DeleteContactL(cid);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Close contact' test case
 */
TInt CMTPbkContactEngine::RunCloseContactL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
    CPbkContactItem* item2 = iContactEngine->OpenContactLCX(cid);

    ASSERT(ContainsSameData(*iContactItem,*item2));

    // The new value for phonenumber
    TBuf<7> phoneNumber;
    phoneNumber.Num(5664540);

    TInt i;
    for (i = 0; i < item2->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = item2->CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            field.TextStorage()->SetTextL(phoneNumber);
            }
        }

    iContactEngine->CloseContactL(cid);
    CleanupStack::PopAndDestroy(2); // item2, lock

    CPbkContactItem* item3 = iContactEngine->ReadContactLC(cid);

    // the contact must be unchanged compared to the default one
    ASSERT(ContainsSameData(*iContactItem , *item3));

    CleanupStack::PopAndDestroy(item3); // item3
    iContactEngine->DeleteContactL(cid);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Delete nonexisting contact' test case
 */
TInt CMTPbkContactEngine::RunDeleteNonexistingContactL()
    {
    SetContactEngineL();
    // try to delete nonexisting cid
    TBool result = ETrue;
    TContactItemId cid2 = 29;
    TRAPD(err, iContactEngine->DeleteContactL(cid2));
    switch (err)
        {
        case KErrNotFound:
            {
            result = ETrue;
            break;
            }
        case KErrNoMemory:
            {
            User::Leave(err);
            break;
            }
        default:
            {
            // Any other case
            result = EFalse;
            break;
            }
        }
    ASSERT(result);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Add and read group' test case
 */
TInt CMTPbkContactEngine::RunAddReadGroupL()
    {
    SetContactEngineL();
    // Add contact group
    _LIT(KGroupName,"AnotherTestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    ASSERT(cgroup);

    // get the id of the group
    TInt cid = cgroup->Id();
    CContactGroup* cgroup2 = iContactEngine->ReadContactGroupL(cid);
    CleanupStack::PushL(cgroup2);

    ASSERT(cgroup2);

    CleanupStack::PopAndDestroy(cgroup2);
    CleanupStack::PopAndDestroy(cgroup);

    iContactEngine->DeleteContactGroupL(cid);
    TeardownContactEngine();
    return KErrNone;    
    }
    
 /**
 * Run 'Add and read group' test case
 */
TInt CMTPbkContactEngine::RunAddReadGroupL1()
    {
    SetContactEngineL();
    // Add contact group
    _LIT(KGroupName,"AnotherTestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    ASSERT(cgroup);

    // get the id of the group
    TInt cid = cgroup->Id();
    CContactGroup* cgroup2 = iContactEngine->ReadContactGroupL(cid);
    CleanupStack::PushL(cgroup2);

    ASSERT(cgroup2);

    CleanupStack::PopAndDestroy(cgroup2);
    CleanupStack::PopAndDestroy(cgroup);

    iContactEngine->DeleteContactGroupL(cid);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Add and open group' test case
 */
TInt CMTPbkContactEngine::RunAddOpenGroupL()
    {
    // Add contact group
    _LIT(KGroupName,"Another2TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    ASSERT(cgroup);

    // get the id of the group
    TInt cid = cgroup->Id();
    CContactGroup* cgroup2 = iContactEngine->OpenContactGroupLCX(cid);

    ASSERT(cgroup2);

    CleanupStack::PopAndDestroy(cgroup2);
    CleanupStack::PopAndDestroy(); // lock
    CleanupStack::PopAndDestroy(cgroup);

    iContactEngine->DeleteContactGroupL(cid);
    return KErrNone;    
    }

/**
 * Test case for OpenContactGroupL
 */
TInt CMTPbkContactEngine::RunOpenContactGroupL()
    {
    SetContactEngineL();
    // Add contact group
    _LIT(KGroupName,"Another2TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    ASSERT(cgroup);

    // get the id of the group
    TInt cid = cgroup->Id();
    CContactGroup* cgroup2 = iContactEngine->OpenContactGroupL(cid);
    CleanupStack::PushL(cgroup2);

    ASSERT(cgroup2);

    iContactEngine->CommitContactGroupL(*cgroup2);

    iContactEngine->DeleteContactGroupL(cid);

    CleanupStack::PopAndDestroy(cgroup2);
    CleanupStack::PopAndDestroy(cgroup);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Add and delete group' test case
 */
TInt CMTPbkContactEngine::RunAddDeleteGroupL()
    {
    SetContactEngineL();
    // Add contact group
    _LIT(KGroupName,"Another3TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    // get the id of the group
    TInt cid = cgroup->Id();
    CleanupStack::PopAndDestroy(cgroup);

    iContactEngine->DeleteContactGroupL(cid);

    CContactGroup* cgroup2 = NULL;
    TRAPD(err, cgroup2 = iContactEngine->ReadContactGroupL(cid));
    switch (err)
        {
        case KErrNotFound:
            {
            //test passed
            break;
            }
        case KErrNoMemory:
            {
            //FAIL_TEST( "ReadContactGroupL returned error: KErrNoMemory" );
            ASSERT(0);
            }
        default:
            {
            // Any other case
            //FAIL_TEST( "ReadContactGroupL returned error" );
            ASSERT(0);
            }
        }
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Run 'Add and delete group' test case
 */
TInt CMTPbkContactEngine::RunAddRemoveContactToGroupL()
    {
	SetupAddContactToGroupL();
    iContactEngine->AddContactToGroupL(iContactItemId, iContactGroupId);

    CContactGroup* cgroup2 = iContactEngine->ReadContactGroupL(iContactGroupId);
    CleanupStack::PushL(cgroup2);

    // Check that item is in the group
    ASSERT(cgroup2->ContainsItem(iContactItemId));

    CleanupStack::PopAndDestroy(cgroup2);

    iContactEngine->RemoveContactFromGroupL(iContactItemId, iContactGroupId);

    CContactGroup* cgroup3 = iContactEngine->ReadContactGroupL(iContactGroupId);
    CleanupStack::PushL(cgroup3);

    // check that item is not there any more
    ASSERT(!cgroup3->ContainsItem(iContactItemId));

    CleanupStack::PopAndDestroy(cgroup3);
	TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Commit contact group' test case
 */
TInt CMTPbkContactEngine::RunCommitContactGroupL()
    {
    SetContactEngineL();
    // Add contact group
    _LIT(KGroupName,"TestCommitGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);

    // get the id of the group
    TInt groupId = cgroup->Id();

    CleanupStack::PopAndDestroy(cgroup);
    CContactGroup* cgroup2 = iContactEngine->OpenContactGroupLCX(groupId);

    ASSERT(cgroup2->GetGroupLabelL() == KGroupName);

    // change group by changing its label
    _LIT(KNewLabeL,"NewLabel");
    cgroup2->SetGroupLabelL(KNewLabeL);

    ASSERT(cgroup2->GetGroupLabelL() == KNewLabeL);

    iContactEngine->CommitContactGroupL(*cgroup2);
    CleanupStack::PopAndDestroy(2);  // cgroup2, lock

    CContactGroup* cgroup3 = iContactEngine->ReadContactGroupL(groupId);
    CleanupStack::PushL(cgroup3);

    // check that the dataupdate went to DB
    ASSERT(cgroup3->GetGroupLabelL() == KNewLabeL);

    CleanupStack::PopAndDestroy(cgroup3);
    TeardownContactEngine();
    return KErrNone;    
    }
    

/**
 * Run 'Delete contacts' test case.
 */
TInt CMTPbkContactEngine::RunDeleteContactsPlainL()
    {
    SetupDeleteContactsL();
    RunDeleteContactsL(EFalse);
    TeardownDeleteContacts();
    return KErrNone;    
    }

/**
 * Run 'Delete contact' test case
 */
TInt CMTPbkContactEngine::RunDeleteContactL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    CPbkContactItem* item = iContactEngine->ReadContactLC(cid);
    ASSERT(item);
    CleanupStack::PopAndDestroy(item);

    TRAPD(error,iContactEngine->DeleteContactL(cid));
    switch (error)
        {
        case KErrNone:
            {
            //test passed
            break;
            }
        case KErrNoMemory:
            {
		    //FAIL_TEST( "ReadContactGroupL returned error: KErrNoMemory" );
			ASSERT(0);
            }
        default:
            {
            // Any other case
            //FAIL_TEST( "ReadContactGroupL returned error" );
            ASSERT(0);
            }
        }

    CPbkContactItem* item2 = NULL;
    TRAPD(err, item2 = iContactEngine->ReadContactLC(cid);CleanupStack::Pop(item2));
    switch (err)
        {
        case KErrNotFound:
            {
            //TEST PASSED
            break;
            }
        case KErrNoMemory:
            {
            //FAIL_TEST( "ReadContactGroupL returned error: KErrNoMemory" );
            ASSERT(0);
            }
        default:
            {
            // Any other case
            //FAIL_TEST( "ReadContactGroupL returned error" );
            ASSERT(0);
            }
        }
	TeardownEngineAndItem();       
    return KErrNone;    
    }

/**
 * Generic run for DeleteContacts and DeleteContactsOnbackground
 */
TInt CMTPbkContactEngine::RunDeleteContactsL(TBool aOnBackGround)
    {
    TContactItemId cid;

    if ( !aOnBackGround )
        {
        iContactEngine->DeleteContactsL(*iContactIdArray);
        }
    else
        {
        iContactEngine->DeleteContactsOnBackgroundL(*iContactIdArray);
        }

    CPbkContactItem* item2 = NULL;
    for (TInt i = 0; i < iContactIdArray->Count();++i)
        {
        cid = (*iContactIdArray)[i];

        TRAPD(err,
                item2 = iContactEngine->ReadContactLC(cid);
                CleanupStack::PopAndDestroy(); // item2
            );
        switch (err)
            {
            case KErrNotFound:
                {
                //test passed
                break;
                }
            case KErrNoMemory:
                {
//                FAIL_TEST( "ReadContactLC returned error: KErrNoMemory" );
				ASSERT(0);
                }
            default:
                {
                // Any other case
                //FAIL_TEST( "ReadContactLC returned error" );
                ASSERT(0);
                break;
                }
            }
        }
    return KErrNone;    
    }

/**
 * Run generic 'Delete contacts with empty array' test case.
 */
TInt CMTPbkContactEngine::RunDeleteContactsWithEmptyArrayL(TBool aOnBackground)
    {
    CContactIdArray* contacts = CContactIdArray::NewLC();

    if (aOnBackground)
        {
        TRAPD( err, iContactEngine->DeleteContactsOnBackgroundL(*contacts) );
        ASSERT ( err==KErrNone );
        }
    else
        {
        TRAPD( err, iContactEngine->DeleteContactsL(*contacts) );
        ASSERT ( err == KErrNone );
        }
    CleanupStack::PopAndDestroy(contacts);
    return KErrNone;    
    }

/**
 * Run generic 'Delete contacts with empty array whihc has nonexisting items' test case.
 */
void CMTPbkContactEngine::RunDeletePartlyNonexistingContactsL(TBool aOnBackground)
    {
    CContactIdArray* contacts = CContactIdArray::NewLC();

    // add some permanent items to db
    ASSERT(AddAndCheckItemToDBAndArrayL(*iContactItem, contacts) > 0);
    ASSERT(AddAndCheckItemToDBAndArrayL(*iContactItem, contacts) > 0);
    ASSERT(AddAndCheckItemToDBAndArrayL(*iContactItem, contacts) > 0);
    ASSERT(AddAndCheckItemToDBAndArrayL(*iContactItem, contacts) > 0);

    // add some items and delete right away
    TContactItemId cid;
    cid = AddAndCheckItemToDBAndArrayL(*iContactItem, contacts);
    ASSERT(DeleteAndCheckItemFromDBL(cid));

    cid = AddAndCheckItemToDBAndArrayL(*iContactItem, contacts);
    ASSERT(DeleteAndCheckItemFromDBL(cid));

    cid = AddAndCheckItemToDBAndArrayL(*iContactItem, contacts);
    ASSERT(DeleteAndCheckItemFromDBL(cid));

    TBool result = ETrue;
    if(aOnBackground)
        {
        iContactEngine->DeleteContactsOnBackgroundL(*contacts);
        }
    else
        {
        // Well.. according to Symbian documentation
        // this should leave if array contains nonexisting items
        TRAPD(error, iContactEngine->DeleteContactsL(*contacts));
        switch (error)
            {
            case KErrNotFound:
                {
                result = ETrue;
                break;
                }
            case KErrNoMemory:
                {
                User::Leave(error);
                break;
                }
            default:
                {
                // Any other case
                result = EFalse;
                break;
                }
            }
        ASSERT(result);
        CleanupStack::PopAndDestroy(contacts);
        return;
        }

    CPbkContactItem* item2;
    for (TInt i = 0; i < contacts->Count();++i)
        {
        cid = (*contacts)[i];

        TRAPD(err,
            item2 = iContactEngine->ReadContactLC(cid);
            CleanupStack::PopAndDestroy(); // item2
            );
        switch (err)
            {
            case KErrNotFound:
                {
                result = ETrue;
                break;
                }
            case KErrNoMemory:
                {
                User::Leave(err);
                break;
                }
            default:
                {
                // Any other case
                result = EFalse;
                break;
                }
            }
        ASSERT(result);
        }
    CleanupStack::PopAndDestroy(contacts);
  
    }

/**
 * Run 'Delete contacts on background' test case.
 */
TInt CMTPbkContactEngine::RunDeleteContactsOnBackgroundL()
    {
    SetupDeleteContactsL();
    RunDeleteContactsL(ETrue);
    TeardownDeleteContacts();
    return KErrNone;    
    }

/**
 * Run 'Delete contacts with empty array'  test case for the
 * plain deletecontacts.
 */
TInt CMTPbkContactEngine::RunDeleteContactsWithEmptyArrayPlainL()
    {
    SetFilledContactL();
    RunDeleteContactsWithEmptyArrayL(EFalse);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Delete contacts with empty array'  test case for the
 * background deletecontacts.
 */
TInt CMTPbkContactEngine::RunDeleteContactsWithEmptyArrayBGL()
    {
    SetFilledContactL();
    RunDeleteContactsWithEmptyArrayL(ETrue);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Delete contacts with partly nonexisting items in array'  test case for the
 * plain deletecontacts.
 */
TInt CMTPbkContactEngine::RunDeleteContactsPartlyArrayPlainL()
    {
    SetFilledContactL();
    RunDeletePartlyNonexistingContactsL(EFalse);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Delete contacts with partly nonexisting items in array'  test case for the
 * background deletecontacts.
 */
TInt CMTPbkContactEngine::RunDeleteContactsPartlyArrayBGL()
    {
    SetFilledContactL();
    RunDeletePartlyNonexistingContactsL(ETrue);
    TeardownEngineAndItem();
    return KErrNone;    
    }
/**
 * Run 'Duplicate contact' test case.
 */
TInt CMTPbkContactEngine::RunDuplicateContactL()
    {
    SetFilledContactL();
    TContactItemId cid1;
    TContactItemId cid2;

    cid1 = iContactEngine->AddNewContactL(*iContactItem);
    cid2 = iContactEngine->DuplicateContactL(cid1);

    CPbkContactItem* item = iContactEngine->ReadContactLC(cid2);

    ASSERT(ContainsSameData(*item,*iContactItem));

    CleanupStack::PopAndDestroy(item);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Read customized contact' test case. Reads only defined fields for a
 * contact from the DB.
 */
TInt CMTPbkContactEngine::RunReadCustomizedContactL()
    {
    SetFilledContactL();
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields);
    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdLastName);
    fields->AppendL(EPbkFieldIdFirstName);
    fields->AppendL(EPbkFieldIdJobTitle);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);

    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    CPbkContactItem* item = iContactEngine->ReadContactLC(cid,fields);

    TInt count = item->CardFields().Count();
    // Assume all specified fields are found
    ASSERT(count >= fields->Count());

    TInt found = 0;
    for (TInt i = 0; i < count; ++i)
        {
        TPbkContactItemField& field = item->CardFields()[i];
        // these six fields should be found
        switch (field.FieldInfo().FieldId())
            {
            case EPbkFieldIdLastName:
                {
                found++;
                break;
                }
            case EPbkFieldIdFirstName:
                {
                found++;
                break;
                }
            case EPbkFieldIdJobTitle:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberGeneral:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberMobile:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberVideo:
                {
                found++;
                break;
                }
            default:
                {
                break;
                }
            }
        }

    ASSERT( found == 6 );

    CleanupStack::PopAndDestroy( item );
    CleanupStack::PopAndDestroy( fields );
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Read contact' test case. Reads all fields for a
 * contact from the DB using ContactL.
 */
TInt CMTPbkContactEngine::RunReadContactAllFieldsL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL( *iContactItem );

    CPbkContactItem* item = iContactEngine->ReadContactL( cid );

    ASSERT( item );

    delete item;
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Read contact' test case. Reads only defined fields for a
 * contact from the DB using ContactL.
 */
TInt CMTPbkContactEngine::RunReadContactL()
    {
    SetFilledContactL();
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields);
    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdLastName);
    fields->AppendL(EPbkFieldIdFirstName);
    fields->AppendL(EPbkFieldIdJobTitle);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);

    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    CPbkContactItem* item = iContactEngine->ReadContactL(cid,fields);
    CleanupStack::PushL(item);

    TInt count = item->CardFields().Count();
    // Assume all specified fields are found
    ASSERT(count >= fields->Count());

    TInt found = 0;
    for (TInt i = 0; i < count; ++i)
        {
        TPbkContactItemField& field = item->CardFields()[i];
        // these six fields should be found
        switch (field.FieldInfo().FieldId())
            {
            case EPbkFieldIdLastName:
                {
                found++;
                break;
                }
            case EPbkFieldIdFirstName:
                {
                found++;
                break;
                }
            case EPbkFieldIdJobTitle:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberGeneral:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberMobile:
                {
                found++;
                break;
                }
            case EPbkFieldIdPhoneNumberVideo:
                {
                found++;
                break;
                }
            default:
                {
                }
            }
        }

    ASSERT ( found == 6 );

    CleanupStack::PopAndDestroy(item);
    CleanupStack::PopAndDestroy(fields);
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Get contact title' test case.
 */
TInt CMTPbkContactEngine::RunGetContactTitleL()
    {
    SetFilledContactL();
    HBufC* title = iContactEngine->GetContactTitleL(*iContactItem);
    CleanupStack::PushL(title);

    ASSERT(title->Length() > 0);

    CleanupStack::PopAndDestroy(title);

    //test unnamed title
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();

    title = iContactEngine->GetContactTitleL(*item);
    CleanupStack::PushL(title);

    ASSERT( title->Length() > 0 );

    ASSERT( title->Compare( iContactEngine->UnnamedTitle() ) == 0 );

    CleanupStack::PopAndDestroy(title);
    delete item;
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Get contact title or null' test case.
 */
TInt CMTPbkContactEngine::RunGetContactTitleOrNullL()
    {
    SetFilledContactL();
    HBufC* title = iContactEngine->GetContactTitleOrNullL(*iContactItem);
    CleanupStack::PushL(title);

    ASSERT(title->Length() > 0);

    CleanupStack::PopAndDestroy(title);

    //test no title
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();

    title = iContactEngine->GetContactTitleOrNullL(*item);
    CleanupStack::PushL(title);

    ASSERT( !title );

    CleanupStack::PopAndDestroy(title);
    delete item;
	TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Is title field' test case
 */
TInt CMTPbkContactEngine::RunIsTitleFieldL()
    {
    SetFilledContactL();
    // these should be:
    ASSERT(iContactEngine->IsTitleField(EPbkFieldIdLastName));
    ASSERT(iContactEngine->IsTitleField(EPbkFieldIdFirstName));
    ASSERT(iContactEngine->IsTitleField(EPbkFieldIdCompanyName));

    // these should not be:
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPhoneNumberWork));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPhoneNumberStandard));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPhoneNumberHome));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPhoneNumberWork));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPhoneNumberMobile));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPagerNumber));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdEmailAddress));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPostalAddress));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdURL));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdJobTitle));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdCompanyAddress));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdDTMFString));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdDate));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdNote));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPicture));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdThumbnailImage));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdSpeedDialIndication));
    ASSERT(!iContactEngine->IsTitleField(EPbkFieldIdPersonalRingingToneIndication));
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'Match phone number' test case
 */
TInt CMTPbkContactEngine::RunMatchPhoneNumberL()
    {
    SetFilledContactL();
    // add contact with number1
    CContactIdArray* num1Contacts = CContactIdArray::NewLC();
    CPbkContactItem* contact1 = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(contact1);
    const TInt number1 = 5664549;
    FillContactFieldsL(*contact1, number1, ETrue);

    TInt i;
    const TInt num1Count = 3;
    TContactItemId cid;
    for (i = 0; i < num1Count; ++i )
        {
        cid = iContactEngine->AddNewContactL(*contact1);
        num1Contacts->AddL(cid);
        }
    CleanupStack::PopAndDestroy(contact1);

    // add contact with number2
    CContactIdArray* num2Contacts = CContactIdArray::NewLC();
    CPbkContactItem* contact2 = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(contact2);
    const TInt number2 = 727612;
    FillContactFieldsL(*contact2, number2, ETrue);

    const TInt num2Count = 4;
    for (i = 0; i < num2Count; ++i )
        {
        cid = iContactEngine->AddNewContactL(*contact2);
        num2Contacts->AddL(cid);
        }
    CleanupStack::PopAndDestroy(contact2);

    // add empty contacts
    CPbkContactItem* emptyContact = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyContact);

    const TInt emptyCount = 2;
    for (i = 0; i < emptyCount; ++i )
        {
        cid = iContactEngine->AddNewContactL(*emptyContact);
        }
    CleanupStack::PopAndDestroy(emptyContact);

    TBuf<10> phoneNumber;

    // check number1 items
    phoneNumber.Num(number1);
    CContactIdArray* foundContacts=
		iContactEngine->MatchPhoneNumberL(phoneNumber, phoneNumberMatching);
    CleanupStack::PushL(foundContacts);

    ASSERT(ContactIdArraysMatch(*foundContacts, *num1Contacts));

    CleanupStack::PopAndDestroy(foundContacts);

    // check number2 items
	phoneNumber.Num(number2);
    CContactIdArray* foundContacts2=
		iContactEngine->MatchPhoneNumberL(phoneNumber, phoneNumberMatching);
    CleanupStack::PushL(foundContacts2);

    ASSERT(ContactIdArraysMatch(*foundContacts2, *num2Contacts));

    CleanupStack::PopAndDestroy(foundContacts2);

    // Try to find with nonexisting number
    const TInt wrongNumber = 123456;
	phoneNumber.Num(wrongNumber);
    CContactIdArray* foundContacts3=
		iContactEngine->MatchPhoneNumberL(phoneNumber, phoneNumberMatching);
    CleanupStack::PushL(foundContacts3);

    ASSERT(foundContacts3->Count() == 0);

    CleanupStack::PopAndDestroy(foundContacts3);
	CleanupStack::PopAndDestroy(2); // num1Contacts, num2Contacts
	TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run 'FindLC' test case
 */
TInt CMTPbkContactEngine::RunFindLCL()
    {	
	SetupFindLCL();
    // first try to find given value from the phone fields
    // -> should fail.
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields);
    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdPhoneNumberStandard);
    fields->AppendL(EPbkFieldIdPhoneNumberHome);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);
    CContactIdArray* foundContacts = iContactEngine->FindLC(KFindText_1,fields);

    ASSERT(!(foundContacts->Count() == 0));

    CleanupStack::PopAndDestroy(2); // foundContacts, fields

    // then try to find valid defined fields
    CPbkFieldIdArray* fields2 = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields2);
    // Add desired fields to the field id array
    fields2->AppendL(EPbkFieldIdLastName);
    fields2->AppendL(EPbkFieldIdJobTitle);
    CContactIdArray* foundContacts2 = iContactEngine->FindLC(KFindText_1,fields2);

    ASSERT(ContactIdArraysMatch(*foundContacts2, *iContacts));
    CleanupStack::PopAndDestroy(2); // foundContacts2, fields

    // finally try to find without field subset
    CContactIdArray* foundContacts3 = iContactEngine->FindLC(KFindText_1);

    ASSERT(ContactIdArraysMatch(*foundContacts3, *iContacts));
    CleanupStack::PopAndDestroy(foundContacts3); //

    // Yet, try to find with empty string
    CPbkFieldIdArray* fields3 = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields3);
    // Add desired fields to the field id array
    fields3->AppendL(EPbkFieldIdLastName);
    fields3->AppendL(EPbkFieldIdJobTitle);
    CContactIdArray* foundContacts4 = iContactEngine->FindLC(KNullDesC, fields3);

    // Actually returns all contacts...
    // just check that no errors occur
    ASSERT(foundContacts4->Count() > 0);
    CleanupStack::PopAndDestroy(2); // foundContacts4, fields3
    TeardownFindLCTest();
    return KErrNone;    
    }

/**
 * Run FindAsyncL test case
 */
TInt CMTPbkContactEngine::RunFindAsyncL()
    {
    SetFilledContactL();
    _LIT(KFindText, "this is right data");

    // Add some valid items to the DB
    CContactIdArray* okContacts = CContactIdArray::NewLC();
    CPbkContactItem* okContact = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(okContact);
    SetTextFieldValueL(*okContact, KFindText);

    TContactItemId cid;
    const TInt okCount = 3;
    TInt i;
    for (i = 0; i < okCount; ++i)
        {
        cid = iContactEngine->AddNewContactL(*okContact);
        okContacts->AddL(cid);
        }
    CleanupStack::PopAndDestroy(okContact);

    // add some filled-but-not-ok contacts
    const TInt filledCount = 3;
    for (i = 0; i < filledCount; ++i )
        {
        cid = iContactEngine->AddNewContactL(*iContactItem);
        }

    // add some empty contacts
    CPbkContactItem* emptyContact = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyContact);
    const TInt emptyCount = 4;
    for (i = 0; i < emptyCount; ++i )
        {
        cid = iContactEngine->AddNewContactL(*emptyContact);
        }
    CleanupStack::PopAndDestroy(emptyContact);

    // create the observer which converts the
    // FindAsyncL to a sync operation
    CFindObserver* observer = new (ELeave) CFindObserver;
    CleanupStack::PushL(observer);

    // first try to find given value from the phone fields
    // -> should fail.
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields);
    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdPhoneNumberStandard);
    fields->AppendL(EPbkFieldIdPhoneNumberHome);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);
    CContactIdArray* foundContacts  = observer->FindSyncL(*iContactEngine,KFindText,fields);
    CleanupStack::PushL(foundContacts);

    ASSERT(!(foundContacts->Count() == 0));

    CleanupStack::PopAndDestroy(2); // foundContacts, fields

    // then try to find valid defined fields
    CPbkFieldIdArray* fields2 = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields2);
    // Add desired fields to the field id array
    fields2->AppendL(EPbkFieldIdLastName);
    fields2->AppendL(EPbkFieldIdJobTitle);
    CContactIdArray* foundContacts2  = observer->FindSyncL(*iContactEngine,KFindText,fields2);
    CleanupStack::PushL(foundContacts2);

    ASSERT(ContactIdArraysMatch(*foundContacts2, *okContacts));
    CleanupStack::PopAndDestroy(2); // foundContacts2, fields

    // finally try to find without field subset
    CContactIdArray* foundContacts3  = observer->FindSyncL(*iContactEngine,KFindText);
    CleanupStack::PushL(foundContacts3);

    ASSERT(ContactIdArraysMatch(*foundContacts3, *okContacts));
    CleanupStack::PopAndDestroy(foundContacts3);

    // Yet, try to find with empty string
    CPbkFieldIdArray* fields3 = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fields3);
    // Add desired fields to the field id array
    fields3->AppendL(EPbkFieldIdLastName);
    fields3->AppendL(EPbkFieldIdJobTitle);
    CContactIdArray* foundContacts4  = observer->FindSyncL(*iContactEngine, KNullDesC,fields3);
    CleanupStack::PushL(foundContacts4);

    // Actually returns all contacts...
    // just check that no errors occur
    ASSERT(foundContacts4->Count() > 0);
    CleanupStack::PopAndDestroy(2); // foundContacts4, fields3

    CleanupStack::PopAndDestroy(2); // observer, okContacts
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run NullAfterDestroy test case.
 */
TInt CMTPbkContactEngine::RunCreateEmptyContact()
    {
    SetCreateEmptyContactL();
    ASSERT(ContactMatchesTemplate(iContactItem,EFalse));
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run AddReadEmptyCard test case.
 */
TInt CMTPbkContactEngine::RunAddReadEmptyCardL()
    {
    SetCreateEmptyContactL();
    ASSERT(DoAddReadEmptyCardTestL());
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run AddOpenEmptyCard test case.
 */
TInt CMTPbkContactEngine::RunAddOpenEmptyCardL()
    {
    SetCreateEmptyContactL();
    ASSERT(DoAddOpenEmptyCardTestL());
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run AddReadCard test case.
 */
TInt CMTPbkContactEngine::RunAddReadCardL()
    {
    SetFilledContactL();
    ASSERT(DoAddReadCardTestL());
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run AddOpenCard test case.
 */
TInt CMTPbkContactEngine::RunAddOpenCardL()
    {
    SetFilledContactL();
    ASSERT(DoAddOpenCardTestL());
    TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run Assign speed dial test case.Tests also the 'SpeedDialsChanged' event.
 */
TInt CMTPbkContactEngine::RunAssignSpeedDialL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    CPbkContactItem* item2 = iContactEngine->OpenContactLCX(cid);
    const TInt speedDialLocation = 1;

    for (TInt i = 0; i < item2->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = item2->CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            // After set, this function commits the contact
            iContactEngine->SetFieldAsSpeedDialL(*item2, i, speedDialLocation);
            //("Speed dial assigned to field <%S>\n"), &field.Label()
            break;
            }
        }

    CPbkContactItem* item3 = iContactEngine->OpenContactLCX(cid);

    TBuf<256> phoneNumber;
    ASSERT( cid == iContactEngine->GetSpeedDialFieldL(speedDialLocation, phoneNumber));

    // compare items
    ASSERT(ContainsSameData(*item2, *item3));
    ASSERT(ContactMatchesTemplate(item3,ETrue));

    CleanupStack::PopAndDestroy(2);  // lock, item2
    CleanupStack::PopAndDestroy(2);  // lock, item3

    // Remove speed dial and check that it was removed
    iContactEngine->RemoveSpeedDialFieldL(cid, speedDialLocation);

    TBuf<256> tel;
    TContactItemId id = iContactEngine->GetSpeedDialFieldL(speedDialLocation, tel);
    ASSERT ( id == KNullContactId );

    iContactEngine->DeleteContactL(cid);
    TeardownEngineAndItem();
    return KErrNone;    
    }


/**
 * Run is speed dial assigned test case.
 */
TInt CMTPbkContactEngine::RunIsSpeedDialAssignedL()
    {
    SetFilledContactL();
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    CPbkContactItem* item = iContactEngine->OpenContactLCX(cid);
    TBool found = EFalse;

    for (TInt i = 0; i < item->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = item->CardFields()[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            found = ETrue;

            //speed dial should not be assigned
            ASSERT( !iContactEngine->IsSpeedDialAssigned( *item, i ) );

            iContactEngine->SetFieldAsSpeedDialL( *item, i, 1);

            //speed dial should be assigned
            ASSERT( iContactEngine->IsSpeedDialAssigned( *item, i ) );

            iContactEngine->RemoveSpeedDialFieldL( cid , 1);

            break;
            }
        }

    CleanupStack::PopAndDestroy(2); // lock, item

    ASSERT( found );
	TeardownEngineAndItem();
    return KErrNone;    
    }

/**
 * Run Logs 'FindLC' test case. This is a case that may have
 * caused problems in Logs - here seems to work...
 */
TInt CMTPbkContactEngine::RunLogsFindLCL()
    {
    SetupLogsFindLCL();
    CPbkFieldIdArray* searchFields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(searchFields);

    searchFields->AppendL( EPbkFieldIdPhoneNumberStandard );
    searchFields->AppendL( EPbkFieldIdPhoneNumberHome );
	searchFields->AppendL( EPbkFieldIdPhoneNumberWork );
	searchFields->AppendL( EPbkFieldIdPhoneNumberMobile );

    _LIT(KFindText, "0500 6762 37 432");

    CContactIdArray* foundContacts = iContactEngine->FindLC(KFindText,searchFields);

    CleanupStack::PopAndDestroy(foundContacts );
    CleanupStack::PopAndDestroy(searchFields);
    TeardownContactEngine();
    return KErrNone;    
    }

/**
 * Teardown the contact engine.
 */
void CMTPbkContactEngine::TeardownContactEngine()
    {
    delete iContactEngine;
    iContactEngine = NULL;

    iFs.Delete(KTestDBName);
    }

/**
 * Teardown engine and item.
 */
void CMTPbkContactEngine::TeardownEngineAndItem()
    {
    delete iContactItem;
    iContactItem = NULL;

    TeardownContactEngine();   
    }

/**
 * Teardown delete contacts
 */
void CMTPbkContactEngine::TeardownDeleteContacts()
    {
    delete iContactIdArray;
    iContactIdArray = NULL;
    TeardownContactEngine();   
    }

/**
 * Teardown 'FindLC' test case
 */
void CMTPbkContactEngine::TeardownFindLCTest()
    {
    delete iContacts;
    iContacts = NULL;
    TeardownEngineAndItem();
    }

//  [End of File] - Do not remove
