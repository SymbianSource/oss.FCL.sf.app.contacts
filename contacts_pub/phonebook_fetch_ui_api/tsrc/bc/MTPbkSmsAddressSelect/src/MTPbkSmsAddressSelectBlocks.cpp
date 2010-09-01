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
#include "MTPbkSmsAddressSelect.h"
#include <CPbkSmsAddressSelect.h>

#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>
#include <e32def.h>
#include <pbkfields.hrh>

#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"

const	TKeyCode    KDownCode(EKeyDownArrow);


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
// CMTPbkSmsAddressSelect::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkSmsAddressSelect::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkSmsAddressSelect::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkSmsAddressSelect::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class_", CMTPbkSmsAddressSelect::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item", CMTPbkSmsAddressSelect::RunExecuteEmptyContactL ),
        ENTRY( "Test_with_empty_contact_itemFocusNULL", CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNullL ),
        ENTRY( "Test_with_empty_contact_itemDirectlyTRUE", CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrueL ),
        ENTRY( "Test_with_contact_item_with_name_fields", CMTPbkSmsAddressSelect::RunExecuteWithContactNameL ),
        ENTRY( "Test_with_contact_item_with_company_name", CMTPbkSmsAddressSelect::RunExecuteWithCompanyNameL ),
        ENTRY( "Test_with_contact_item_with_name_fieldsFocusNULL", CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNull1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldFocusNULL", CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNull2L ),
        ENTRY( "Test_with_contact_item_with_name_fieldsDirectlyTRUE", CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrue1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldDirectlyTRUE", CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrue2L ),
        ENTRY( "Test_with_one_email_address", CMTPbkSmsAddressSelect::RunExecuteWithOnePhoneNumberL ),
        ENTRY( "Test_one_email_addressFocusNULL", CMTPbkSmsAddressSelect::RunExecuteWithOneNumberFocusNullL ),
        ENTRY( "Test_one_email_addressDirectlyTRUE", CMTPbkSmsAddressSelect::RunExecuteWithOneNumberDirectlyTrueL ),
        ENTRY( "Test_two_email_addresses_and_key_event_simulation", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberL ),
        ENTRY( "Test_two_email_addresses_one_default_email", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndFocusL ),
        ENTRY( "Test_two_phone_numbers_one_default_SMS", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneDefault1L ),
        ENTRY( "Test_two_phone_numbers_one_default_SMS", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneDefault2L ),
        ENTRY( "Test_two_phone_numbers_one_default_SMS_and_one_focused_phone_number", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneFocusAndDefaultL ),
        ENTRY( "Test_One_default_SMS_company_focus_ret_default_SMS_number_focused", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberCompanyFocusL ),
        ENTRY( "Test_One_default_SMS_NULL_focus_ret_default_number_focused", CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberNullFocusL ),
        ENTRY( "Test_Soft_key_events", CMTPbkSmsAddressSelect::RunExecuteTestSoftKeys1L ),
        ENTRY( "Test_Soft_key_events", CMTPbkSmsAddressSelect::RunExecuteTestSoftKeys2L ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkSmsAddressSelect::Teardown(  )
    {
    delete iContactItem;
	iContactItem = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunTest_ClassSizeL()
    {
    SetupEmptyContactItemL();
    static const TInt sizeof_class_CPbkSmsAddressSelect_30 = 28;
    static const TInt sizeof_class_CPbkSmsAddressSelect_TParams = 28;

    RDebug::Printf("Class: sizeof(class CPbkSmsAddressSelect)=%d\n", 
                sizeof(class CPbkSmsAddressSelect));
    RDebug::Printf("Class: sizeof(class CPbkSmsAddressSelect::TParams)=%d\n", 
                sizeof(class CPbkSmsAddressSelect::TParams));

    ASSERT( sizeof(class CPbkSmsAddressSelect) >= 
                sizeof_class_CPbkSmsAddressSelect_30 );
    ASSERT( sizeof(class CPbkSmsAddressSelect::TParams) >= 
                sizeof_class_CPbkSmsAddressSelect_TParams );
	Teardown();
	return KErrNone;                
    }

/*******************************************************************
*
*	RunExecuteEmptyContactL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteEmptyContactL()
	{
	SetupEmptyContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Focus to first name field
	TInt 					search=0;
	TPbkContactItemField* 	focusedField = NULL;

	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );

	// Return value must be EFalse if canceled
    // or no phone number found.
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNullL()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}
	
/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNull1L()
	{
	SetupEmptyContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseFocusNull2L()
	{
	SetupContactItemWithCompanyNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrueL()
	{
	SetupEmptyContactItemL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrue1L()
	{
	SetupContactItemWithNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestCaseDirectlyTrue2L()
	{
	SetupContactItemWithCompanyNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteWithContactName()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithContactNameL()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse;
	TBool					ret = ETrue;
	// Focus to first name field
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	//focus on contacts first name
	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteWithCompanyName()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithCompanyNameL()
	{
	SetupContactItemWithCompanyNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Focus to company name
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	//focus on company name
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search);
	ASSERT( focusedField );

	// Return value must be EFalse if canceled or no phone number found.
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
	ASSERT( !ret);
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteWithOnePhoneNumberL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithOnePhoneNumberL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt 					search = 0,
			                index = 0,
	                        phoneNumberType = EPbkFieldIdPhoneNumberGeneral;
	TPbkContactItemField* 	focusedField = NULL;

	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search);
	ASSERT( focusedField );

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
	                    phoneNumberType, index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneNumberFocusNullL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithOneNumberFocusNullL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					index = 0,
	                        phoneNumberType = EPbkFieldIdPhoneNumberGeneral;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
	                    phoneNumberType, index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;	
	}

/*******************************************************************
*
*	RunExecuteWithOneNumberDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithOneNumberDirectlyTrueL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					index = 0,
	                        phoneNumberType = EPbkFieldIdPhoneNumberGeneral;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
	            phoneNumberType, index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TKeyCode 				selectionKeyCode = EKeyNull;
	TInt 					acceptKeyCounter = 0,
							downKeyCounter = 0,
							findPhoneNumbers = 0,
	                        phoneNumberType = EPbkFieldIdPhoneNumberGeneral;

	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		for ( findPhoneNumbers = 0;
			findPhoneNumbers < iAddressAndNumberSelect->PhoneNumbers();
			findPhoneNumbers++)
			{
			for( downKeyCounter = 0; downKeyCounter < findPhoneNumbers; downKeyCounter++ )
				{
				iSimulateKeyEvents->AddKeyEventCodeL( KDownCode );
				}

			iSimulateKeyEvents->SelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull);

			SendKeyEventsL( selectionKeyCode );

			ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
        	ASSERT( ret );

			ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
			        phoneNumberType, findPhoneNumbers, *iContactItem );
        	ASSERT( ret );

			} // Phone number loop ends
		}// Select keys loop ends
	Teardown();
	return KErrNone;		
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndFocusL()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndFocusL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					focusSearch= 0,
							focusIndex = 0,
	                        phoneNumberType = EPbkFieldIdPhoneNumberGeneral;

	for ( focusIndex = 0;
			focusIndex < iAddressAndNumberSelect->PhoneNumbers(); focusIndex++)
		{
		focusedField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, focusSearch );
		ASSERT( focusedField  );

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
       	ASSERT( ret );

		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
		                phoneNumberType, focusIndex, *iContactItem );
    	ASSERT( ret );

		focusSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;		
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndOneDefault1L()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneDefault1L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	TInt 					defaultSearch = 0,
                            defaultIndex = 0;
    TBool                   ret = EFalse;

	defaultSearch=0;
   	for ( defaultIndex = 0; defaultIndex < iAddressAndNumberSelect->PhoneNumbers(); 
   	        defaultIndex++)
   		{
   		TPbkContactItemField* defaultField= iContactItem->FindField( 
   		                    EPbkFieldIdPhoneNumberGeneral,defaultSearch );
   		ASSERT( defaultField );

		// Set default sms
		iContactItem->SetDefaultSmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultSmsField() == defaultField);

    	// Set call default directly to true.
    	TBool 	callDefaultDirectly = ETrue;
    	// Set No focused field
    	TPbkContactItemField* 	focusedField = NULL;

   		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
        ASSERT( ret );

    	TInt 	phoneType = EPbkFieldIdPhoneNumberGeneral,

   		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
   		                        phoneType, defaultIndex, *iContactItem );
       	ASSERT( ret);

		defaultSearch++;
   		} // defaultIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndOneDefault2L()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneDefault2L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					defaultSearch = 0,
                            defaultIndex = 0;
    TBool                   ret = EFalse;

	focusedField = iContactItem->FindField( EPbkFieldIdEmailAddress );
	ASSERT( focusedField );

	defaultSearch=0;
   	for ( defaultIndex = 0; defaultIndex < iAddressAndNumberSelect->PhoneNumbers(); 
   	        defaultIndex++)
   		{
   		TPbkContactItemField*  defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral,
    		                                        defaultSearch );
   		ASSERT( defaultField );

		// Set default sms
		iContactItem->SetDefaultSmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultSmsField() == defaultField);

    	// Set call default directly to true.
    	TBool callDefaultDirectly = ETrue;
    	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );

       	ASSERT( ret );

        TInt 	phoneType = EPbkFieldIdPhoneNumberGeneral,
   		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
    		                        phoneType, defaultIndex, *iContactItem );
       	ASSERT( ret);

		defaultSearch++;
   		} // defaultSearch loop
	Teardown();
	return KErrNone;   		
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndOneFocusAndDefault()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberAndOneFocusAndDefaultL()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					focusSearch= 0,
                            phoneType = EPbkFieldIdPhoneNumberGeneral,
                            defaultSearch = 0,
                            defaultIndex = 0,
                            focusIndex = 0;

	// Give focus to every phone number field
	for ( focusIndex=0; focusIndex< iAddressAndNumberSelect->PhoneNumbers();
	        focusIndex++)
	    {
		focusedField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, focusSearch );
   		ASSERT( focusedField );

		defaultSearch = 0;
    	for ( defaultIndex = 0;
    			defaultIndex < iAddressAndNumberSelect->PhoneNumbers(); defaultIndex++)
    		{
    		defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral,
    		                                        defaultSearch );
    		ASSERT( defaultField );

			// Set default sms
			iContactItem->SetDefaultSmsFieldL( defaultField );
			ASSERT( iContactItem->DefaultSmsField() == defaultField);
							

    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
           	ASSERT( ret );

    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
    		                        phoneType, focusIndex, *iContactItem );
           	ASSERT( ret );

			defaultSearch++; // Take next phone number field.
            }// defaultIndex loop
   		} // focusIndex loop
   		focusSearch++;
	Teardown();
	return KErrNone;   		
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberCompanyFocus()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberCompanyFocusL()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					focusSearch= 0,
                            phoneType = EPbkFieldIdPhoneNumberGeneral,
                            phoneNumber = 0,
                            search = 0,
                            acceptKeyCounter = 0;
	TKeyCode                selectionKeyCode = EKeyNull;

	focusedField= iContactItem->FindField( EPbkFieldIdCompanyName, focusSearch );
	ASSERT( focusedField );

	// Give focus to every phone number field
	for ( acceptKeyCounter=0; acceptKeyCounter< KSelectKeys; acceptKeyCounter++)
	    {
        search = 0;
    	for ( phoneNumber = 0;
    			phoneNumber < iAddressAndNumberSelect->PhoneNumbers(); phoneNumber++)
    		{
    		defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral,
    		                                        search );
    		ASSERT( defaultField );

			// Set default phone number
			iContactItem->SetDefaultSmsFieldL( defaultField );
			ASSERT( iContactItem->DefaultSmsField() == defaultField);
								
			iSimulateKeyEvents->SelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull);

			SendKeyEventsL( selectionKeyCode );

    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
           	ASSERT( ret );

    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
    		                        phoneType, phoneNumber, *iContactItem );
           	ASSERT( ret );

            search++;
            }// defaultIndex loop
   		} // acceptKeyCounter loop
   		focusSearch++;
   	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberNullFocus()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteWithMultiNumberNullFocusL()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					phoneType = EPbkFieldIdPhoneNumberGeneral,
                            defaultSearch = 0,
                            phoneNumber = 0,
                            acceptKeyCounter = 0;
	TKeyCode                selectionKeyCode = EKeyNull;

	// Give focus to every phone number field
	for ( acceptKeyCounter=0; acceptKeyCounter< KSelectKeys; acceptKeyCounter++)
	    {
        defaultSearch = 0;
    	for ( phoneNumber = 0;
    			phoneNumber < iAddressAndNumberSelect->PhoneNumbers(); phoneNumber++)
    		{
    		defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral,
    		                                        defaultSearch );
    		ASSERT( defaultField );

			// Set default phone number
			iContactItem->SetDefaultSmsFieldL( defaultField );
			ASSERT( iContactItem->DefaultSmsField() == defaultField);
								
			iSimulateKeyEvents->SelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull);

			SendKeyEventsL( selectionKeyCode );

    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
           	ASSERT( ret );

    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
    		                        phoneType, phoneNumber, *iContactItem );
           	ASSERT( ret );

			defaultSearch++; // Take next phone number field.
            }// defaultIndex loop
   		} // acceptKeyCounter loop
   		Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestSoftKeys1L()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestSoftKeys1L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	TKeyCode 				testedSoftKeyCode = rightSoftKeyCode;
	TInt 					defaultSearch = 0,
							focusIndex = 0;
	    					
	TPbkContactItemField* 	defaultField = NULL;

	// Give focus to every e-mail address field
	for ( focusIndex=0;
		  focusIndex < iAddressAndNumberSelect->PhoneNumbers(); focusIndex++ )
		{
		defaultField = iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral,
		                                            defaultSearch );
   		ASSERT( defaultField );

		iContactItem->SetDefaultSmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultSmsField() == defaultField);
							
		SendKeyEventsL( testedSoftKeyCode );

    	// Set call default directly to false.
    	TBool 	callDefaultDirectly = EFalse,
    			ret = EFalse;
    	TPbkContactItemField* 	focusedField = NULL;
    	
		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );

		ASSERT( !ret);

		defaultSearch++;
		} // focusIndex loop
		Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestSoftKeys2L()
*
*******************************************************************/
TInt CMTPbkSmsAddressSelect::RunExecuteTestSoftKeys2L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();
	TKeyCode 				testedSoftKeyCode = leftSoftKeyCode;
	TInt 					defaultSearch = 0,
							focusIndex = 0;
	TBool	                ret = EFalse;

	// Give focus to every e-mail address field
	for ( focusIndex=0;
		  focusIndex < iAddressAndNumberSelect->PhoneNumbers(); focusIndex++ )
		{
		TPbkContactItemField* defaultField = iContactItem->FindField( 
		                    EPbkFieldIdPhoneNumberGeneral,defaultSearch );
   		ASSERT( defaultField );

		iContactItem->SetDefaultSmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultSmsField() == defaultField);
							

		SendKeyEventsL( testedSoftKeyCode );

    	// Set call default directly to false.
    	TBool 	                callDefaultDirectly = EFalse;
    	TPbkContactItemField* 	focusedField = NULL;

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );

		ASSERT( ret );

		TInt                    type = EPbkFieldIdPhoneNumberGeneral;
		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL(
				                                type, focusIndex, *iContactItem );
		ASSERT( ret );

		defaultSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteL ()
*
*	Runs CPbkSmsAddressSelect's ExecuteLD method
*
*******************************************************************/
TBool CMTPbkSmsAddressSelect::RunExecuteL(
				const CPbkContactItem& aItem,
                const TPbkContactItemField* aFocusedField,
                TBool& aCallDefaultDirectly)
	{
	TBool	ret = EFalse;

	iSmsAddressSelect = new(ELeave) CPbkSmsAddressSelect;

	CPbkSmsAddressSelect::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = iSmsAddressSelect->ExecuteLD(params);
	//ExecuteLD deletes iSmsAddressSelect but it has to be zeroed also
	iSmsAddressSelect = NULL;
	return ret;

	}

/*********************** SETUP *************************************
*
* Creates and initialises one empty CPbkContactItem object.
*
*******************************************************************/
void CMTPbkSmsAddressSelect::SetupEmptyContactItemL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

void CMTPbkSmsAddressSelect::SetupContactItemWithCompanyNameL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactCompanyNameFieldL( *iContactItem, iContactEngine );
    }

void CMTPbkSmsAddressSelect::SetupContactItemWithNameL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

void CMTPbkSmsAddressSelect::SetupDefaultContactItemL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    }

void CMTPbkSmsAddressSelect::SetupContactItemWithTwoEmailFieldsL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();

    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, EFalse,
			EFalse, ETrue, EFalse);
	}

void CMTPbkSmsAddressSelect::SetupContactItemWithTwoPhoneNumberFieldsL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();

    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, EFalse,
			EFalse, EFalse, ETrue);
	}

/******************************************************************
*
* Handle key events and codes
*
*******************************************************************/
void CMTPbkSmsAddressSelect::SendKeyEventsL( )
	{

	TKeyCode testedSoftKeyCode = leftSoftKeyCode;

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( testedSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();
	}

void CMTPbkSmsAddressSelect::SendKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();
	}

//  [End of File] - Do not remove
