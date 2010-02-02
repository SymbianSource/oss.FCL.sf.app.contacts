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
#include "MTPbkContactItemField.h"
#include <TPbkContactItemField.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>

// the string which will be set to textfields
_LIT(KFillText, "TESTDATA");
// first name string which will be set to textfields
_LIT(KFirstName, "John");
// email address which will be set to email address fields
_LIT(KEmailAddress, "john@company.com");
// the name of the contact database used in these tests
_LIT(KTestDBName,"c:MT_CPbkContactItem.cdb");
// date string
_LIT(KDate, "20060101:");
// formatted date string
_LIT(KFormattedDate, "02/02/2006");
// string containing spaces
_LIT(KSpacesString, "    ");
// phonenumber for testing purposes
const TInt KPhoneNumber = 5005001;
// lenght of phone number
const TInt KPhoneNumberLenght = 10;
// text's max lenght
const TInt KTextMaxLen = 100;
// size of TPbkContactItemField class
const TInt KClassSize = 12;
// speed dial for number 1
const TInt KSpeedDialOne = 1;

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
// CMTPbkContactItemField::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkContactItemField::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkContactItemField::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkContactItemField::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    	TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Test_size_of_TPbkContactItemField_class", CMTPbkContactItemField::RunTestSize ),
        ENTRY( "Create_and_delete_emptyvTPbkContactItemField", CMTPbkContactItemField::RunCreateEmptyTest ),
        ENTRY( "Create_and_delete_TPbkContactItemField_with_values", CMTPbkContactItemField::RunCreateWithValuesTest ),        
        ENTRY( "Set_the_contents_of_TPbkContactItemField_object", CMTPbkContactItemField::RunSetTest ),        
        ENTRY( "Get_icon_id", CMTPbkContactItemField::RunIconIdTest ),
        ENTRY( "Get_field_info", CMTPbkContactItemField::RunFieldInfoTest ),
        ENTRY( "Get_field's_text_content", CMTPbkContactItemField::RunTextTestL ),        
        ENTRY( "Get_field's_time_content", CMTPbkContactItemField::RunTimeTest ),        
        ENTRY( "Get_field's_date_content", CMTPbkContactItemField::RunDateTest ),
        ENTRY( "Get_field's_text_content._Field_is_text_type", CMTPbkContactItemField::RunGetTextLTestL ),
        ENTRY( "Get_field's_text_content._Field_is_date_type", CMTPbkContactItemField::RunGetTextLDateTestL ),        
        ENTRY( "Get_field's_text_content._Field_content_cannot_be_represented_as_text", CMTPbkContactItemField::RunGetTextLNotSupportedTestL ),        
        ENTRY( "Get_storage_type", CMTPbkContactItemField::RunStorageTypeTest ),
        ENTRY( "Get_embedded_contact_item_field", CMTPbkContactItemField::RunItemFieldTest ),
        ENTRY( "Get_text_storage", CMTPbkContactItemField::RunTextStorageTestL ),        
        ENTRY( "Set_and_get_label_text_of_this_field", CMTPbkContactItemField::RunLabelTestL ),        
        ENTRY( "Set_and_get_hidden_status", CMTPbkContactItemField::RunHiddenTest ),
        ENTRY( "Get_time_storage", CMTPbkContactItemField::RunDateTimeStorageTest ),
        ENTRY( "Test_IsSame_function", CMTPbkContactItemField::RunIsSameTestL ),        
        ENTRY( "Get_CContactItemField", CMTPbkContactItemField::RunContactItemFieldTest ),        
        ENTRY( "Test_Compare_function", CMTPbkContactItemField::RunCompareTestL ),
        ENTRY( "Get_an_uniqueid", CMTPbkContactItemField::RunUniqueFieldIdentityTest ),
        ENTRY( "Test_IsEmpty_function", CMTPbkContactItemField::RunIsEmptyTestL ),        
        ENTRY( "Test_IsEmptyOrAllSpaces_function", CMTPbkContactItemField::RunIsEmptyOrAllSpacesTestL ),        
        ENTRY( "Test_IsEmptyOrAllSpaces_function_Not_text_storage", CMTPbkContactItemField::RunIsEmptyOrAllSpacesNoTextTestL ),
        ENTRY( "Is_field_a_default_phone_number_field", CMTPbkContactItemField::RunDefaultPhoneNumberFieldTestL ),
        ENTRY( "Is_field_a_default_video_number_field", CMTPbkContactItemField::RunDefaultVideoNumberFieldTestL ),        
        ENTRY( "Is_field_a_default_sms_field", CMTPbkContactItemField::RunDefaultSmsFieldTestL ),        
        ENTRY( "Is_field_a_default_email_over_sms_field", CMTPbkContactItemField::RunDefaultEmailOverSmsFieldTestL ),
        ENTRY( "Is_field_a_default_email_field", CMTPbkContactItemField::RunDefaultEmailFieldTestL ),
        ENTRY( "Isfield_a_default_mms_field", CMTPbkContactItemField::RunDefaultMmsFieldTestL ),        
        ENTRY( "Isfield_a_default_Poc_field", CMTPbkContactItemField::RunDefaultPocFieldTestL ),        
        ENTRY( "Is_field_a_default_Voip_field", CMTPbkContactItemField::RunDefaultVoipFieldTestL ),
        ENTRY( "Get_speed_dial_indexes", CMTPbkContactItemField::RunSpeedDialIndexesLTestL ),
        ENTRY( "Get_field_type", CMTPbkContactItemField::RunPbkFieldTypeTest ),        
        ENTRY( "Get_field_id", CMTPbkContactItemField::RunPbkFieldIdTest ),        
        ENTRY( "Get_field_text", CMTPbkContactItemField::RunPbkFieldTextTestL ),
        ENTRY( "Get_field_time", CMTPbkContactItemField::RunPbkFieldTimeTest ),
        ENTRY( "Test_text_fields_equality", CMTPbkContactItemField::RunEqualsTextFieldsTestL ),        
        ENTRY( "Test_date_fields_equality", CMTPbkContactItemField::RunEqualsDateFieldsTestL ),        
        ENTRY( "Test_store_fields_equality", CMTPbkContactItemField::RunEqualsStoreFieldsTestL ),                
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::ContactItemField()
// Returns const reference to TPbkContactItemField
// @return Const reference to TPbkContactItemField
// ----------------------------------------------------------------------------
const TPbkContactItemField& CMTPbkContactItemField::ContactItemField()
    {
    return *iContactItemField;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::DummyMethod()
// Does nothing
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::DummyMethod(  )
    {  
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupEmptyL()
// Creates empty TPbkContactItemField
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupEmptyL(  )
    {
    iContactItemField = new (ELeave) TPbkContactItemField(); 
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupNameFieldL()
// Creates First name field
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupNameFieldL(  )
    {
    iCntField = CContactItemField::NewL( KStorageTypeText );
    iCntField->SetLabelL( KFillText );
    
    iFieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
                                  
    iContactItemField = new (ELeave) TPbkContactItemField( iCntField, iFieldInfo );  
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupDateTimeFieldL()
// Creates date/time field
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupDateTimeFieldL(  )
    {
    iCntField = CContactItemField::NewL( KStorageTypeDateTime );
    iCntField->SetLabelL( KFillText );
    
    iFieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdDate );
                                  
    iContactItemField = new (ELeave) TPbkContactItemField( iCntField, iFieldInfo );  
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupStoreFieldL()
// Creates store field
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupStoreFieldL(  )
    {
    iFieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
            
    iCntField = CContactItemField::NewL( KStorageTypeStore );
        
    iContactItemField = new (ELeave) TPbkContactItemField( iCntField, iFieldInfo );
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupNumberFieldL()
// Creates phone number field
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupNumberFieldL(  )
    {
    iFieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdPhoneNumberMobile );
            
    iCntField = CContactItemField::NewL( KStorageTypeText );
    TBuf<KPhoneNumberLenght> phoneNumber;
    phoneNumber.Num(KPhoneNumber);
    iCntField->TextStorage()->SetTextL( phoneNumber );
    
    iContactItemField = new (ELeave) TPbkContactItemField( iCntField, iFieldInfo );
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::SetupEmailFieldL()
// Creates email address field
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::SetupEmailFieldL(  )
    {
    iFieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdEmailAddress );
            
    iCntField = CContactItemField::NewL( KStorageTypeText ); 
    iCntField->TextStorage()->SetTextL( KEmailAddress );
    
    iContactItemField = new (ELeave) TPbkContactItemField( iCntField, iFieldInfo );
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::Teardown(  )
// Teardown
// ----------------------------------------------------------------------------
void CMTPbkContactItemField::Teardown(  )
    {
    delete iContactItemField;
    iContactItemField = NULL;
    
    delete iCntField;
    iCntField = NULL;
    }
    
// ----------------------------------------------------------------------------
// Test size of TPbkContactItemField class
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunTestSize()
    {
    
    ASSERT( 
            sizeof( TPbkContactItemField ) >= KClassSize  );
    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunCreateTest(  )
// Test default construction
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunCreateEmptyTest(  )
    {
    TPbkContactItemField* field = new (ELeave) TPbkContactItemField(); 
    
    ASSERT( field );
    
    delete field;
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunCreateWithValuesTest(  )
// Test construction with values
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunCreateWithValuesTest(  )
    {
    SetupNameFieldL();
    ASSERT( iContactItemField );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunSetTest(  )
// Run 'Set' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunSetTest(  )
    {
    SetupNameFieldL();
    iContactItemField->Set( iCntField, iFieldInfo );  
         
    ASSERT( 
        iContactItemField->Label() == KFillText );
    
    ASSERT( iFieldInfo->FieldId() == iContactItemField->FieldInfo().FieldId());
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunIconIdTest(  )
// Run 'IconId' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunIconIdTest(  )
    {
    SetupNameFieldL();
    ASSERT( iContactItemField->IconId() == iFieldInfo->IconId() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunFieldInfoTest(  )
// Run 'FieldInfo' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunFieldInfoTest(  )
    {
    SetupNameFieldL();
    const CPbkFieldInfo& info = iContactItemField->FieldInfo(); 
    
    ASSERT( info.FieldId() == iFieldInfo->FieldId() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunTextTest(  )
// Run 'Text' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunTextTestL(  )
    {
    SetupNameFieldL();
    TPtrC text1 = iContactItemField->Text(); 
    //text is not set. should not be found   
    ASSERT( !text1.Ptr() ); 
    
    iContactItemField->TextStorage()->SetTextL( KFirstName );
    TPtrC text2 = iContactItemField->Text(); 
    //text is set. should be found
    ASSERT( text2.Compare( KFillText ) );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunTimeTest(  )
// Run 'Time' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunTimeTest(  )
    {
    SetupDateTimeFieldL();
    TTime time(KDate);    
 
    iContactItemField->DateTimeStorage()->SetTime( time );
  
    ASSERT( iContactItemField->Time() == time );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDateTest(  )
// Run 'Date' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDateTest(  )
    {
    SetupDateTimeFieldL();
    TTime time(KDate);    

    iContactItemField->DateTimeStorage()->SetTime( time.DateTime() );
  
    ASSERT( iContactItemField->Time() == time );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunRunGetTextLTest(  )
// Run 'GetTextL' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunGetTextLTestL(  )
    {
    SetupNameFieldL();
    // no text
    TBuf<KTextMaxLen> text1;
    iContactItemField->GetTextL( text1 );
    
    // text should be empty
    ASSERT( text1 == KNullDesC );
    
    // text is set
    iContactItemField->TextStorage()->SetTextL( KFirstName );
    
    TBuf<KTextMaxLen> text2;
    iContactItemField->GetTextL( text2 );
    
    ASSERT( text2 == KFirstName );
    
    // let's make too short buffer
    TBuf<3> text3;
    TRAPD( err, iContactItemField->GetTextL( text3 ) );
    
    if ( err == KErrOverflow )
        {
        // text is clipped becouse it did not fit to buffer
        ASSERT( text3 != KFirstName );
        }
    else
        {
        ASSERT(0);
        }
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunRunGetTextLDateTest(  )
// Run 'GetTextL' testcase. Field type is date
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunGetTextLDateTestL(  )
    {
    SetupDateTimeFieldL();
    TBuf<KTextMaxLen> text1;
    iContactItemField->GetTextL( text1 );
    
    //text should be empty
    ASSERT( text1 == KNullDesC );

    // Set datetime data to the field
    TTime date( KDate );
    
    iContactItemField->DateTimeStorage()->SetTime( date.DateTime() );
    
    TBuf<KTextMaxLen> text2;
    iContactItemField->GetTextL( text2 );
    // The format is not dokumented in the header so can not test
    // the format. Test that we got something.
    ASSERT( text2.Length() > 0 );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunGetTextLNotSupportedTest(  )
// Run 'GetTextL' testcase. Field content cannot be 
// represented as text
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunGetTextLNotSupportedTestL(  )
    {       
    SetupStoreFieldL();
    TBuf<KTextMaxLen> text;
    TRAPD( err, iContactItemField->GetTextL( text ) );
    
    if ( err == KErrNotSupported )
        {
        // test passed
        }
    else
        {
        ASSERT( 0 );
        }
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunStorageTypeTest(  )
// Run 'StorageType' testcase 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunStorageTypeTest(  )
    {
    SetupNameFieldL();
    ASSERT( iContactItemField->StorageType() == KStorageTypeText );
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunItemFieldTest(  )
// Run 'ItemField' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunItemFieldTest(  )
    {
    SetupNameFieldL();
    CContactItemField& field = iContactItemField->ItemField();
   
    ASSERT( field.Id() == iCntField->Id() );
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunTextStorageTest(  )
// Run 'TextStorage' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunTextStorageTestL(  )
    {
    SetupNameFieldL();
    iContactItemField->TextStorage()->SetTextL( KFirstName );
    
    CContactTextField* textField = iContactItemField->TextStorage();
    ASSERT( textField );
    
    ASSERT( textField->Text() == KFirstName );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunLabelTestL(  )
// Run 'Label' testcase. Set and get label
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunLabelTestL(  )
    {
    SetupNameFieldL();
    iContactItemField->SetLabelL( KFirstName );
    
    ASSERT( iContactItemField->Label() == KFirstName );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunHiddenTest(  )
// Run 'Hidden' testcase. Set and get hidden status
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunHiddenTest(  )
    {
    SetupNameFieldL();
    iContactItemField->SetHidden( ETrue );
    ASSERT( iContactItemField->IsHidden() );
    
    iContactItemField->SetHidden( EFalse );
    ASSERT( !iContactItemField->IsHidden() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDateTimeStorageTest(  )
// Run 'DateTimeStorage' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDateTimeStorageTest(  )
    {
    SetupDateTimeFieldL();
    CContactDateField* date = iContactItemField->DateTimeStorage();
    ASSERT( date );
    
    // just get TTime and test it works
    TTime time = date->Time();
    ASSERT( time.DaysInMonth() > 0 );
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunIsSameTestL(  )
// Run 'IsSame' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunIsSameTestL(  )
    {
    SetupNumberFieldL();
    CPbkFieldInfo* fieldInfo = iContactEngine->FieldsInfo().
                                    Find( EPbkFieldIdPhoneNumberMobile );
    TPbkContactItemField* itemField = new (ELeave) TPbkContactItemField();
    
    // is not same
    ASSERT( !iContactItemField->IsSame( *itemField ) );
    
    //set same CContactItemField. should pass
    itemField->Set( &iContactItemField->ContactItemField(), fieldInfo );
    
    ASSERT( iContactItemField->IsSame( *itemField ) );
    
    delete itemField;
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunContactItemFieldTest(  )
// Run 'ContactItemField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunContactItemFieldTest(  )
    {
    SetupNameFieldL();
    CContactItemField& cntField1 = iContactItemField->ContactItemField();
    // just test that label is correct
    ASSERT( cntField1.Label() == KFillText );
    
    const CContactItemField& cntField2 = ContactItemField().ContactItemField();
    // just test that label is correct
    ASSERT( cntField2.Label() == KFillText );
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunCompareTestL(  )
// Run 'Compare' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunCompareTestL(  )
    {
SetupNumberFieldL();
    // create first name field
    CPbkFieldInfo* fieldInfo1 = iContactEngine->FieldsInfo().
                                    Find( EPbkFieldIdFirstName );
    
    CContactItemField* cntField1 = CContactItemField::NewL( KStorageTypeText );
    CleanupStack::PushL( cntField1 );
    cntField1->TextStorage()->SetTextL( KFirstName );
        
    TPbkContactItemField itemField1( cntField1, fieldInfo1 );
            
    // first name field vs. phone number field. compare does not match
    ASSERT( iContactItemField->Compare( itemField1 ) != 0 );
    
    
    // create phone number field
    CPbkFieldInfo* fieldInfo2 = iContactEngine->FieldsInfo().
                                    Find( EPbkFieldIdPhoneNumberMobile );
    
    CContactItemField* cntField2 = CContactItemField::NewL( KStorageTypeText );
    CleanupStack::PushL( cntField2 );
    TBuf<KPhoneNumberLenght> phoneNumber;
    phoneNumber.Num(KPhoneNumber);
    cntField2->TextStorage()->SetTextL( phoneNumber );
    
    
    TPbkContactItemField itemField2( cntField2, fieldInfo2 );
    
    // both fields are phone number fields. compare matches
    ASSERT( iContactItemField->Compare( itemField2 ) == 0 );
    
    CleanupStack::PopAndDestroy( cntField2 );
    CleanupStack::PopAndDestroy( cntField1 );
    Teardown();
    return KErrNone;
     }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunUniqueFieldIdentityTest(  )
// Run 'UniqueFieldIdentity' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunUniqueFieldIdentityTest(  )
    {
    SetupNameFieldL();
    ASSERT( iContactItemField->UniqueFieldIdentity() != 0 );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunIsEmptyTestL(  )
// Run 'IsEmpty' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunIsEmptyTestL(  )
    {
    SetupNameFieldL();
    // nothing is set to strage. should be empty
    ASSERT( iContactItemField->IsEmpty() );
    
    // put some text to text storage
    iCntField->TextStorage()->SetTextL( KFillText );
    // storage should be full
    ASSERT( !iContactItemField->IsEmpty() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunIsEmptyOrAllSpacesTestL(  )
// Run 'IsEmptyOrAllSpaces' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunIsEmptyOrAllSpacesTestL(  )
    {
    SetupNameFieldL();
    // nothing is set to storage. should be empty
    ASSERT( iContactItemField->IsEmptyOrAllSpaces() );
    
    // put spaces to text storage
    iCntField->TextStorage()->SetTextL( KSpacesString );
    // text consists of only white space characters
    ASSERT( iContactItemField->IsEmptyOrAllSpaces() );
    
    // put some text to text storage
    iCntField->TextStorage()->SetTextL( KFillText );
    // storage should be full
    ASSERT( !iContactItemField->IsEmptyOrAllSpaces() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunIsEmptyOrAllSpacesNoTextTestL(  )
// Run 'IsEmptyOrAllSpaces' testcase. Not text storage 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunIsEmptyOrAllSpacesNoTextTestL(  )
    {
    SetupDateTimeFieldL();
    // nothing is set to storage. should be empty
    ASSERT( iContactItemField->IsEmptyOrAllSpaces() );
    
    TTime time(KDate); 
    iContactItemField->DateTimeStorage()->SetTime( time.DateTime() );
    // Date set. Storage should be full
    ASSERT( !iContactItemField->IsEmptyOrAllSpaces() );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultPhoneNumberFieldTestL(  )
// Run 'DefaultPhoneNumberField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultPhoneNumberFieldTestL(  )
    {  
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultPhoneNumberField() );
    
    //set default field
    item->SetDefaultPhoneNumberFieldL( &field );
    ASSERT( field.DefaultPhoneNumberField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultVideoNumberFieldTestL(  )
// Run 'DefaultVideoNumberField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultVideoNumberFieldTestL(  )
    {   
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultVideoNumberField() );
    
    //set default field
    item->SetDefaultVideoNumberFieldL( &field );
    ASSERT( field.DefaultVideoNumberField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultSmsFieldTestL(  )
// Run 'DefaultSmsField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultSmsFieldTestL(  )
    {   
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultSmsField() );
    
    //set default field
    item->SetDefaultSmsFieldL( &field );
    ASSERT( field.DefaultSmsField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultEmailOverSmsFieldTestL(  )
// Run 'DefaultEmailOverSmsField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultEmailOverSmsFieldTestL(  )
    {  
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultEmailOverSmsField() );
    
    //set default field
    item->SetDefaultEmailOverSmsFieldL( &field );
    ASSERT( field.DefaultEmailOverSmsField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultEmailFieldTestL(  )
// Run 'DefaultEmailField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultEmailFieldTestL(  )
    {  
    SetupEmailFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultEmailField() );
    
    //set default field
    item->SetDefaultEmailFieldL( &field );
    ASSERT( field.DefaultEmailField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultMmsFieldTestL(  )
// Run 'DefaultMmsField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultMmsFieldTestL(  )
    {
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultMmsField() );
    
    //set default field
    item->SetDefaultMmsFieldL( &field );
    ASSERT( field.DefaultMmsField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultPocFieldTestL(  )
// Run 'DefaultPocField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultPocFieldTestL(  )
    {
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultPocField() );
    
    //set default field
    item->SetDefaultPocFieldL( &field );
    ASSERT( field.DefaultPocField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunDefaultVoipFieldTestL(  )
// Run 'DefaultVoipField' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunDefaultVoipFieldTestL(  )
    { 
    SetupNumberFieldL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
    TPbkContactItemField& field = item->AddFieldL( *iFieldInfo );
    
    ASSERT( !field.DefaultVoipField() );
    
    //set default field
    item->SetDefaultVoipFieldL( &field );
    ASSERT( field.DefaultVoipField() );
    
    CleanupStack::PopAndDestroy( item );
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunSpeedDialIndexesLTestL(  )
// Run 'SpeedDialIndexesL' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunSpeedDialIndexesLTestL(  )
    {
    SetupNumberFieldL();
    // no speed dial is set
    CArrayFix<TInt>* speeddials =  iContactItemField->SpeedDialIndexesL(); 
    ASSERT( !speeddials );
    
    //create contact and set speed dial
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL( item );
 
    for ( TInt i = 0; i < item->CardFields().Count(); ++i )
        {
        TPbkContactItemField& field = item->CardFields()[i];
        
        if (field.FieldInfo().IsPhoneNumberField())
            {
            // Set a number to first phone number field;
            TInt number = KPhoneNumber;
            TBuf<KPhoneNumberLenght> phoneNumber;
            phoneNumber.Num( number );
            field.TextStorage()->SetTextL( phoneNumber );
            
            break;
            }
        }
        
    TContactItemId cid = iContactEngine->AddNewContactL( *item );
    CPbkContactItem* item2 = iContactEngine->OpenContactL( cid );
    CleanupStack::PushL( item2 );
    
    // set speed dial
    for ( TInt i = 0; i < item2->CardFields().Count(); ++i )
        {
        TPbkContactItemField& field = item2->CardFields()[i];

        if ( field.FieldInfo().IsPhoneNumberField() )
            {    
            iContactEngine->SetFieldAsSpeedDialL( *item2, i, KSpeedDialOne );
            iContactItemField = &field;
            break;
            }
        }
   
    // one speed dial is set
    speeddials =  iContactItemField->SpeedDialIndexesL(); 
    ASSERT( speeddials->At(0) == KSpeedDialOne );
      
    iContactEngine->DeleteContactL( cid );
    
    CleanupStack::Pop( item2 );
    CleanupStack::PopAndDestroy( item );
    delete speeddials;
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunPbkFieldTypeTest(  )
// Run 'PbkFieldType' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunPbkFieldTypeTest(  )
    {
	SetupNumberFieldL();
    ASSERT( KStorageTypeText == iContactItemField->PbkFieldType());
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunPbkFieldIdTest(  )
// Run 'PbkFieldId' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunPbkFieldIdTest(  )
    {
    SetupNumberFieldL();
    ASSERT( iContactItemField->FieldInfo().FieldId() == 
            iContactItemField->PbkFieldId());
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunPbkFieldTextTestL(  )
// Run 'PbkFieldText' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunPbkFieldTextTestL(  )
    {
    SetupNumberFieldL();
    iContactItemField->TextStorage()->SetTextL( KFirstName );
    TPtrC t = iContactItemField->PbkFieldText();
    ASSERT( iContactItemField->Text() == iContactItemField->PbkFieldText());
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunPbkFieldTimeTest(  )
// Run 'PbkFieldTime' testcase. 
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunPbkFieldTimeTest(  )
    {   
    SetupDateTimeFieldL();
    TTime time(KDate);    
    iContactItemField->DateTimeStorage()->SetTime( time );
    
    ASSERT( iContactItemField->Time() == iContactItemField->PbkFieldTime());
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunEqualsTextFieldsTestL(  )
// Run '==' testcase. Text field.
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunEqualsTextFieldsTestL(  )
    {
    SetupNameFieldL();
    //create phone number field
    CContactItemField* cntField = CContactItemField::NewL( KStorageTypeText );
    TBuf<KPhoneNumberLenght> phoneNumber;
    phoneNumber.Num(KPhoneNumber);
    cntField->TextStorage()->SetTextL( phoneNumber );
    CPbkFieldInfo* fieldInfo = 
            iContactEngine->FieldsInfo().Find( EPbkFieldIdPhoneNumberMobile );
    TPbkContactItemField* contactItemField = 
            new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    // number field and name are not equal
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create last name field
    cntField = CContactItemField::NewL( KStorageTypeText );
    cntField->SetLabelL( KFillText );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdLastName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create first name field with different label
    cntField = CContactItemField::NewL( KStorageTypeText );
    cntField->SetLabelL( KFirstName );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create first name field. set as hidden
    cntField = CContactItemField::NewL( KStorageTypeText );
    cntField->SetLabelL( KFillText );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    contactItemField->SetHidden( !contactItemField->IsHidden() );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create first name field. different content type
    cntField = CContactItemField::
            NewL( KStorageTypeText, KUidContactFieldGivenNamePronunciation );
    cntField->SetLabelL( KFillText );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create first name field. different text
    cntField = CContactItemField::NewL( KStorageTypeText );
    cntField->SetLabelL( KFillText );
    TBuf<4> name(KFirstName);
    cntField->TextStorage()->SetTextL( name );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //create first name field
    cntField = CContactItemField::NewL( KStorageTypeText );
    cntField->SetLabelL( KFillText );
    fieldInfo = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( *iContactItemField == *contactItemField );
    
    delete cntField; 
    delete contactItemField;
    Teardown();
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunEqualsDateFieldsTestL(  )
// Run '==' testcase. Date field.
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunEqualsDateFieldsTestL(  )
    {
    SetupDateTimeFieldL();
    // create field with different date 
    CContactItemField* cntField  = CContactItemField::NewL( KStorageTypeDateTime );
    cntField->SetLabelL( KFillText );
    TTime time( KDate ); 
    cntField->DateTimeStorage()->SetTime( time );
    CPbkFieldInfo* fieldInfo  = iContactEngine->FieldsInfo().Find( EPbkFieldIdDate );
    TPbkContactItemField* contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT( !( *iContactItemField == *contactItemField ) );
    
    //use same date (no date set)
    cntField  = CContactItemField::NewL( KStorageTypeDateTime );
    cntField->SetLabelL( KFillText );
    fieldInfo  = iContactEngine->FieldsInfo().Find( EPbkFieldIdDate );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
    
    ASSERT(  *iContactItemField == *contactItemField );
    
    delete cntField; 
    delete contactItemField;
    Teardown();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMTPbkContactItemField::RunEqualsStoreFieldsTestL(  )
// Run '==' testcase. Store field.
// ----------------------------------------------------------------------------
TInt CMTPbkContactItemField::RunEqualsStoreFieldsTestL(  )
    {
    
    //create two store field with different content
    CContactItemField* cntField  = CContactItemField::NewL( KStorageTypeStore );
    HBufC8* buf = HBufC8::NewL( 8 );
    _LIT8( KText, "testdata" );
    *buf = KText; 
    cntField->StoreStorage()->SetThingL( *buf );
    CPbkFieldInfo* fieldInfo  = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    TPbkContactItemField* contactItemField = 
            new (ELeave) TPbkContactItemField( cntField, fieldInfo );
            
    CContactItemField* cntField2  = CContactItemField::NewL( KStorageTypeStore );
    HBufC8* buf2 = HBufC8::NewL( 8 );
    _LIT8( KText2, "testtext" );
    *buf2 = KText2; 
    cntField2->StoreStorage()->SetThingL( *buf2 );
    CPbkFieldInfo* fieldInfo2  = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    TPbkContactItemField* contactItemField2 = 
            new (ELeave) TPbkContactItemField( cntField2, fieldInfo2 );
            
    ASSERT( !( *contactItemField == *contactItemField2 ) );
    
    //create two store field with same content
    cntField  = CContactItemField::NewL( KStorageTypeStore );
    *buf = KText; 
    cntField->StoreStorage()->SetThingL( *buf );
    fieldInfo  = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField = new (ELeave) TPbkContactItemField( cntField, fieldInfo );
            
    cntField2  = CContactItemField::NewL( KStorageTypeStore );
    *buf2 = KText; 
    cntField2->StoreStorage()->SetThingL( *buf2 );
    fieldInfo2  = iContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName );
    contactItemField2 = new (ELeave) TPbkContactItemField( cntField2, fieldInfo2 );
            
    ASSERT( *contactItemField == *contactItemField2 );
        
    delete cntField; 
    delete contactItemField;
    delete cntField2; 
    delete contactItemField2;
    Teardown();
    return KErrNone;
    }

//  [End of File] - Do not remove
