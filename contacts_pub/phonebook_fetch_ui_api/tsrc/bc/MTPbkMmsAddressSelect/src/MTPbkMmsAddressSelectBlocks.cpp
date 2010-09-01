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
#include <e32def.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MTPbkMmsAddressSelect.h"
#include <CPbkMmsAddressSelect.h>
#include <CPbkEmailAddressSelect.h>
#include <CPbkContactEngine.h>
#include <eikenv.h>
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <pbkfields.hrh>
//	INTERNAL INCLUDES
#include "T_AddressAndNumberSelect.h"
#include "CSimulateKeyEvents.h"


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
// CMTPbkMmsAddressSelect::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkMmsAddressSelect::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkMmsAddressSelect::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkMmsAddressSelect::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class_", CMTPbkMmsAddressSelect::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item_", CMTPbkMmsAddressSelect::RunExecuteEmptyContactL ),
        ENTRY( "Test_with_empty_contact_itemFocusNULL_", CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNullL ),
        ENTRY( "Test_with_empty_contact_itemDirectlyTRUE_", CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrueL ),
        ENTRY( "Test_with_contact_item_with_name_fields_", CMTPbkMmsAddressSelect::RunExecuteWithContactNameL ),
        ENTRY( "Test_with_contact_item_with_company_name_", CMTPbkMmsAddressSelect::RunExecuteWithCompanyNameL ),
        ENTRY( "Test_with_contact_item_with_name_fieldsFocusNULL_", CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNull1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldFocusNULL_", CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNull2L ),
        ENTRY( "Test_with_contact_item_with_name_fieldsDirectlyTRUE_", CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrue1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldDirectlyTRUE_", CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrue2L ),
        ENTRY( "Test_one_phone_number_no_emails_", CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberL ),
        ENTRY( "Test_one_phone_number_no_emails_FocusNULL_", CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberFocusNullL ),
        ENTRY( "Test_one_phone_number_no_emails_DirectlyTRUE", CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberDirectlyTrueL ),
        ENTRY( "Test_one_email_no_phone_numbers_", CMTPbkMmsAddressSelect::RunExecuteWithOneEmailL ),
        ENTRY( "Test_one_email_no_phone_numbers_FocusNULL_", CMTPbkMmsAddressSelect::RunExecuteWithOneEmailFocusNullL ),
        ENTRY( "Test_one_email_no_phone_numbers_DirectlyTRUE_", CMTPbkMmsAddressSelect::RunExecuteWithOneEmailDirectlyTrueL ),
        ENTRY( "Test_two_MMS_addresses_and_key_event_simulation", CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndKeySimL ),
        ENTRY( "Test_focus_on_MMS_field_DirectlyFALSE_", CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndKeySimFocusNullL ),
        ENTRY( "Test_Four_MMS_fields_one_default_MMS_null_or_name_focus", CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSL ),
        ENTRY( "Test_Four_MMS_fields_one_default_MMS_and_one_focused_MMS_", CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSAndMMSfocusL ),
        ENTRY( "Test_One_default_MMS_company_focus_ret_default_MMS_number_focused_", CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSAndCompanyfocusL ),
        ENTRY( "Test_One_default_MMS_NULL_focus_ret_default_number_focused_", CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndDefNoFocusL ),
        ENTRY( "Test_Soft_key_events_", CMTPbkMmsAddressSelect::RunExecuteTestSoftKeys1L ),
        ENTRY( "Test_Soft_key_events_", CMTPbkMmsAddressSelect::RunExecuteTestSoftKeys2L ),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkMmsAddressSelect::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunTest_ClassSizeL()
    {
    SetupContactItemWithNameL();
    static const TInt sizeof_class_CPbkMmsAddressSelect_30 = 32;
    static const TInt sizeof_class_CPbkMmsAddressSelect_TParams = 32;

    RDebug::Printf("Class: sizeof(class CPbkMmsAddressSelect)=%d\n", 
                sizeof(class CPbkMmsAddressSelect));
    RDebug::Printf("Class: sizeof(class CPbkMmsAddressSelect::TParams)=%d\n", 
            sizeof(class CPbkMmsAddressSelect::TParams));

    ASSERT( sizeof(class CPbkMmsAddressSelect) >= 
                sizeof_class_CPbkMmsAddressSelect_30 );
    ASSERT( sizeof(class CPbkMmsAddressSelect::TParams) >= 
                sizeof_class_CPbkMmsAddressSelect_TParams );
	Teardown();
	return KErrNone;             
    }

/*******************************************************************
*
*	RunExecuteEmptyContactL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteEmptyContactL()
	{
	SetupEmptyContactItemL();
	TInt 	                search=0;
	TBool 	                ret = EFalse,
			                callDefaultDirectly = EFalse;
	TPbkContactItemField* 	focusedField = NULL;

	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search );
	ASSERT( focusedField );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNullL()
	{
	SetupEmptyContactItemL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNull1L()
	{
	SetupContactItemWithNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseFocusNull2L()
	{
	SetupContactItemWithCompanyNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrueL()
	{
	// Set no focus
	SetupEmptyContactItemL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrue1L()
	{
	// Set no focus
	SetupContactItemWithNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestCaseDirectlyTrue2L()
	{
	// Set no focus
	SetupContactItemWithCompanyNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}
	
/*******************************************************************
*
*	RunExecuteWithContactNameL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithContactNameL()
	{
	SetupContactItemWithNameL();
	TInt 	                search=0;
	TBool 	                callDefaultDirectly = EFalse,
			                ret = EFalse;
	TPbkContactItemField*   focusedField = NULL;

	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search );
	ASSERT( focusedField  );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithCompanyNameL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithCompanyNameL()
	{
	SetupContactItemWithCompanyNameL();
	TInt 	                search=0;
	TBool 	                callDefaultDirectly = EFalse,
			                ret = EFalse;
	TPbkContactItemField*   focusedField = NULL;
	
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search );
	ASSERT( focusedField );

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	

	// Return value must be EFalse if canceled or no mail address found.
	ASSERT( ret == EFalse );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithPhoneNumberL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberL()
	{
	SetupDefaultContactItemWithoutEmailL();
	TInt 	                search = 0,
			                index = 0,
			                type = EPbkFieldIdPhoneNumberGeneral;
	TBool 	                callDefaultDirectly = EFalse,
			                ret = EFalse;
	TPbkContactItemField*   focusedField = NULL;
	
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search);
	ASSERT( focusedField  );

    SendKeyEventsL();

	ret = RunExecuteL( *iContactItem,focusedField,callDefaultDirectly );	
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( type, index, *iContactItem );
	ASSERT( ret  );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithPhoneNumberFocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberFocusNullL()
	{
	SetupDefaultContactItemWithoutEmailL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt					type = EPbkFieldIdPhoneNumberGeneral,
							index = 0;
	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret );

	ret = iAddressAndNumberSelect ->IsCorrectPhoneNumberL( type, index, *iContactItem );
	ASSERT( ret  );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithPhoneNumberDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithPhoneNumberDirectlyTrueL()
	{
	SetupDefaultContactItemWithoutEmailL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	TInt					type = EPbkFieldIdPhoneNumberGeneral,
							index = 0;
	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( type, index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmailL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithOneEmailL()
	{
	SetupDefaultContactItemWithoutPhoneNumberL();
	TInt 	                search=0,
			                index = 0;
	TBool 	                callDefaultDirectly = EFalse,
			                ret = EFalse;
	TPbkContactItemField*   focusedField;
	
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search );
	ASSERT( focusedField );

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret);
	
	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmailFocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithOneEmailFocusNullL()
	{
	SetupDefaultContactItemWithoutPhoneNumberL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt 					index = 0;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret);

	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmailDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithOneEmailDirectlyTrueL()
	{
	SetupDefaultContactItemWithoutPhoneNumberL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	TInt 					index = 0;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret);

	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteMultiMMSAndKeySimL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndKeySimL()
	{
	SetupContactItemWithFourMMSFieldsL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TKeyCode 				selectionKeyCode = EKeyNull;
	TInt 					acceptKeyCounter = 0,
							downKeyCounter = 0,
							findMmsAddress = 0;

	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		for ( findMmsAddress = 0; 
			findMmsAddress < iAddressAndNumberSelect->MmsAddresses(); 
			findMmsAddress++)
			{
			iSimulateKeyEvents->ClearAllKeyEventsL();

			for( downKeyCounter = 0; downKeyCounter < findMmsAddress; downKeyCounter++ )
				{
				iSimulateKeyEvents->AddKeyEventCodeL( downCode );	
				}
			
			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
    		ASSERT( selectionKeyCode != EKeyNull );

			iSimulateKeyEvents->AddKeyEventCodeL( selectionKeyCode );
			iSimulateKeyEvents->CallBackStartL();


			ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
        	ASSERT( ret);
			
			ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( findMmsAddress, *iContactItem);
        	ASSERT( ret );
			} // Email addresses loop ends
		}// Select keys loop ends
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteMultiMMSAndKeySimFocusNullL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndKeySimFocusNullL()
	{	
	SetupContactItemWithFourMMSFieldsL();
	// Set call default directly to false.
	TBool 					callDefaultDirectly = EFalse,
							ret;
	// Later focus is set to EPbkFieldIdPhoneNumberGeneral field.
	TPbkContactItemField* 	focusedField = NULL; 
	TInt 					focusSearch= 0,
							focusIndex = 0;
	
	// Give focus to every mms address field
	for (focusIndex=0; focusIndex < iAddressAndNumberSelect->MmsAddresses(); focusIndex++)
		{
		focusedField = NULL;

		// At first focus is given to phone number field.
		if ( focusIndex < iAddressAndNumberSelect->PhoneNumbers() )
			{
			focusedField= iContactItem->FindField( 
										EPbkFieldIdPhoneNumberGeneral, 
										focusSearch );		
        	ASSERT( focusedField);
			}
		// Then focus is given to email field.
		else if ( focusIndex < (iAddressAndNumberSelect->PhoneNumbers() + 
		                	iAddressAndNumberSelect->EmailAddresses()))
			{
   			if( focusIndex == iAddressAndNumberSelect->PhoneNumbers() ) 
   				{
   				// FocusSearch variable is nulled when first time searching 
   				// email address field.
   				focusSearch=0; 
   				}
   			focusedField= iContactItem->FindField( 
   										EPbkFieldIdEmailAddress, 
   										focusSearch );		
           	ASSERT( focusedField );
			}
		else
			{ 
			//focusedField is compared here only for getting API to stop
           	ASSERT( focusedField );
			}

       	ASSERT( focusedField );

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
       	ASSERT( ret);
	
		ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( focusIndex, *iContactItem );
       	ASSERT( ret );

		focusSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiMMSL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSL()
	{	
	SetupContactItemWithFourMMSFieldsL();
	// If callDefaultDirectly = EFalse is in test case 
	// then default MMS address is only highlighted
	// not directly selected.
	TBool 					callDefaultDirectly = ETrue,
							ret;
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt					focusTests,
							focusedFieldId = -1,
							defaultSearch = 0,
							defaultIndex;

	for ( focusTests=0; focusTests < KFocusTests; focusTests++)
		{
		focusedField = NULL;
		// Test dialog with focused first name and with no focus.
		focusedFieldId = -1;
		
		if ( focusTests == 1 )
			{
			focusedField = iContactItem->FindField( EPbkFieldIdFirstName );		
           	ASSERT( focusedField  );
			break;
			}
        else
			{
           	ASSERT( !focusTests  );
		    }

		defaultSearch = 0;
		// Set default to every phone number field
		for ( defaultIndex=0; 
			  defaultIndex < iAddressAndNumberSelect->MmsAddresses(); 
			  defaultIndex++ )
			{
			defaultField = NULL;

			// Deside focused field type.
			focusedFieldId = iAddressAndNumberSelect->MmsAddressFieldId(defaultIndex);
           	ASSERT( ( focusedFieldId > -1 )  );
			if (defaultIndex == iAddressAndNumberSelect->PhoneNumbers() )
				{
				defaultSearch=0;
				}
			
			defaultField= iContactItem->FindField( focusedFieldId, defaultSearch );
           	ASSERT( defaultField );

			// Set default MMS
			iContactItem->SetDefaultMmsFieldL( defaultField );
			ASSERT( iContactItem->DefaultMmsField() == defaultField );

			ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
           	ASSERT( ret);

			ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( defaultIndex, *iContactItem );
           	ASSERT( ret );

			defaultSearch++;
			} // defaultIndex loop ends
		}// focusTests loop ends 	
	Teardown();
	return KErrNone;  
	}

/*******************************************************************
*
*	RunExecuteWithMultiMMSAndMMSfocusL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSAndMMSfocusL()
	{	
	SetupContactItemWithFourMMSFieldsL();
	TInt 					focusSearch = 0,
							defaultSearch = 0,
							focusedFieldId = 0,
							focusedDefaultFieldId = 0,
							focusIndex,
							defaultIndex;
	TBool 					callDefaultDirectly = EFalse,
							ret;

	// Test dialog with focused email and phone number fields.
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	
	focusSearch=0;
	// Give focus to every phone number field
	for ( focusIndex=0; 
		 focusIndex < iAddressAndNumberSelect->MmsAddresses(); focusIndex++)
		{
		// Decide focused field type.
		focusedFieldId = iAddressAndNumberSelect->MmsAddressFieldId(focusIndex);
       	ASSERT( ( focusedFieldId > -1 ));

		if (focusIndex == iAddressAndNumberSelect->PhoneNumbers() )
			{
			focusSearch=0;
			}
		focusedField= iContactItem->FindField( focusedFieldId, focusSearch );		
       	ASSERT( focusedField);
			
		defaultSearch = 0;
		// Give default to every phone number field.
		for ( defaultIndex = 0; 
			 defaultIndex< iAddressAndNumberSelect->MmsAddresses();
			 defaultIndex++)
			{
 			
			focusedDefaultFieldId = iAddressAndNumberSelect->MmsAddressFieldId(defaultIndex);
           	ASSERT( ( focusedDefaultFieldId > -1 ));

			if (defaultIndex == iAddressAndNumberSelect->PhoneNumbers() )
				{
				defaultSearch=0;
				}

			defaultField = iContactItem->FindField( focusedDefaultFieldId,defaultSearch );		
           	ASSERT( defaultField);

			iContactItem->SetDefaultMmsFieldL( defaultField );			
			ASSERT( iContactItem->DefaultMmsField() == defaultField );

			ret = RunExecuteL( *iContactItem, focusedField,	callDefaultDirectly );	
           	ASSERT( ret);

			// Focused MMS address field should be returned
			ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( focusIndex, *iContactItem );
           	ASSERT( ret );

			defaultSearch++; // Take next phone number field.
			} // default index loop

			focusSearch++; // update focus for the next possible search round
		} //focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiMMSAndCompanyfocusL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteWithMultiMMSAndCompanyfocusL()
	{
	SetupContactItemWithFourMMSFieldsL();
	TInt 					defaultSearch=0,
							focusedDefaultFieldId = -1,
							acceptKeyCounter = 0,
							defaultIndex = 0;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TKeyCode 				selectionKeyCode = EKeyNull;

	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName);		
   	ASSERT( focusedField);

	// Go through different kind of accept key events.
	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		defaultSearch=0;
		focusedDefaultFieldId = -1;

		for ( defaultIndex=0; 
			defaultIndex < iAddressAndNumberSelect->MmsAddresses(); 
			defaultIndex++)
			{
			focusedDefaultFieldId = iAddressAndNumberSelect->MmsAddressFieldId(defaultIndex);
           	ASSERT( ( focusedDefaultFieldId > -1 ));

			if (defaultIndex == iAddressAndNumberSelect->PhoneNumbers() )
				{
				// Null search variable when focused field is first time email type.
				defaultSearch=0; 
				}
			defaultField = iContactItem->FindField( focusedDefaultFieldId, defaultSearch);		
           	ASSERT( defaultField );

			iContactItem->SetDefaultMmsFieldL( defaultField );			
			ASSERT( iContactItem->DefaultMmsField() == defaultField );
			
			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );
	
			ret = RunExecuteL( *iContactItem, focusedField,	callDefaultDirectly );	
           	ASSERT( ret);
		
			// Check return values
			ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( defaultIndex, *iContactItem );
           	ASSERT( ret );

			defaultSearch++; // Take next phone number field.
			}// for numbers
		}// for SelectionKeys
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteMultiMMSAndDefNoFocusL()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteMultiMMSAndDefNoFocusL()
	{
	SetupContactItemWithFourMMSFieldsL();
	// Focused field has value NULL otherwise this test case is
	// like previous test case 8.
	// Test cases 8 and 9 could be combined.

	// Set call default directly to false.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	noFocusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					defaultSearch = 0,
							focusedDefaultFieldId = 0,
							acceptKeyCounter = 0,
							defaultIndex = 0;
	TKeyCode 				selectionKeyCode = EKeyNull;

	for( acceptKeyCounter = 1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		defaultSearch=0;
		// Give focus to every phone number field
		for ( defaultIndex=0; 
			  defaultIndex < iAddressAndNumberSelect->MmsAddresses(); 
			  defaultIndex++ )
			{
			focusedDefaultFieldId = iAddressAndNumberSelect->MmsAddressFieldId(defaultIndex);
           	ASSERT( ( focusedDefaultFieldId > -1 ) );

			if (defaultIndex == iAddressAndNumberSelect->PhoneNumbers() )
				{
				// Null search variable when focused field is first time email type.
				defaultSearch=0; 
				}
			
			defaultField= iContactItem->FindField( focusedDefaultFieldId , defaultSearch );
           	ASSERT( defaultField );

			iContactItem->SetDefaultMmsFieldL( defaultField );
			ASSERT( iContactItem->DefaultMmsField() == defaultField  ); 
						
			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );
			
			ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );
           	ASSERT( ret);
		
			// Check return values
			ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( defaultIndex, *iContactItem );
           	ASSERT( ret );
		
			defaultSearch++;
			} // defaultIndex loop ends
		} // acceptKeyCounter loop ends
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestSoftKeys1L()
*
*******************************************************************/
TInt CMTPbkMmsAddressSelect::RunExecuteTestSoftKeys1L()
	{
	SetupContactItemWithFourMMSFieldsL();
	TInt 					focusIndex = 0;

	// Focus is set to first and to second MMS field.
	for ( focusIndex=0; focusIndex < KFocusTests; focusIndex++ )
		{
		TInt focusedDefaultFieldId = iAddressAndNumberSelect->MmsAddressFieldId(focusIndex);
       	ASSERT( ( focusedDefaultFieldId > -1 ) );

    	TInt defaultSearch = 0;
    	
		if (focusIndex == iAddressAndNumberSelect->PhoneNumbers() )
			{
			// Null search variable when focused field is first time email type.
			defaultSearch=0; 
			}
			
		TPbkContactItemField* defaultField= iContactItem->FindField( 
		            focusedDefaultFieldId, defaultSearch );
       	ASSERT( defaultField );

		iContactItem->SetDefaultMmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultMmsField() == defaultField );
		
    	TKeyCode    testedSoftKeyCode = rightSoftKeyCode;
		SendKeyEventsL( testedSoftKeyCode );

    	// Set call default directly to false.
    	TBool 					callDefaultDirectly = EFalse,
    							ret = EFalse;
    	TPbkContactItemField* 	noFocusedField = NULL;

		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			
       	ASSERT( ret == EFalse );
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
TInt CMTPbkMmsAddressSelect::RunExecuteTestSoftKeys2L()
	{
	SetupContactItemWithFourMMSFieldsL();				  
	TInt 					focusIndex = EFalse;

	// Focus is set to first and to second MMS field.
	for ( focusIndex=0; focusIndex < KFocusTests; focusIndex++ )
		{
		TInt focusedDefaultFieldId = iAddressAndNumberSelect->MmsAddressFieldId(focusIndex);
       	ASSERT( ( focusedDefaultFieldId > -1 ) );

    	TInt defaultSearch = 0;
    	
		if (focusIndex == iAddressAndNumberSelect->PhoneNumbers() )
			{
			// Null search variable when focused field is first time email type.
			defaultSearch=0; 
			}
			
		TPbkContactItemField* defaultField= iContactItem->FindField( focusedDefaultFieldId, defaultSearch );
       	ASSERT( defaultField );

		iContactItem->SetDefaultMmsFieldL( defaultField );
		ASSERT( iContactItem->DefaultMmsField() == defaultField );
		
    	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		SendKeyEventsL( testedSoftKeyCode );

    	// Set call default directly to false.
    	TBool 					callDefaultDirectly = EFalse,
    							ret = EFalse;
    	TPbkContactItemField* 	noFocusedField = NULL;

		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			
       	ASSERT( ret);
		ret = iAddressAndNumberSelect->IsCorrectMmsAddressL( focusIndex, *iContactItem );
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
*******************************************************************/
TBool CMTPbkMmsAddressSelect::RunExecuteL(
				const CPbkContactItem& aItem, 
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly)
	{
    TBool   ret = EFalse;

	iMmsAddressSelect = new(ELeave) CPbkMmsAddressSelect;

	CPbkMmsAddressSelect::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = iMmsAddressSelect->ExecuteLD(params);
	//ExecuteLD deletes iEmailAddressSelect but it has to be zeroed also
	iMmsAddressSelect = NULL;

	return ret;
	}

/*********************** SETUP *************************************
*
* Creates and initialises one empty CPbkContactItem object.
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupEmptyContactItemL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

/*******************************************************************
*
* Creates and initialises one almost empty CPbkContactItem object.
* A company name is added.
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupContactItemWithCompanyNameL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactCompanyNameFieldL( *iContactItem, iContactEngine );
    }

/*******************************************************************
*
* Creates and initialises one almost empty CPbkContactItem object.
* A contact name is added.
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupContactItemWithNameL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

/*******************************************************************
*
* Creates and initialises one CPbkContactItem object with 
* all basic fields but not the email address. 
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupDefaultContactItemWithoutEmailL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
	
	TPbkContactItemField* field = iContactItem->FindField(EPbkFieldIdEmailAddress);
    iContactItem->RemoveField(iContactItem->FindFieldIndex(*field));
    }

/*******************************************************************
*
* Creates and initialises one CPbkContactItem object.
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupDefaultContactItemWithoutPhoneNumberL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
	// Be careful that correct phone number type is removed.
	iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
	    
	TPbkContactItemField* field = iContactItem->FindField(EPbkFieldIdPhoneNumberGeneral);
    iContactItem->RemoveField(iContactItem->FindFieldIndex(*field));
    }

/*******************************************************************
*
* Creates and initialises one CPbkContactItem object
* with four SMS fields (two email addresses+ two phone numbers).
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SetupContactItemWithFourMMSFieldsL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
				*iContactItem,  
				iContactEngine,
				EFalse, 
				ETrue, // Multiple Mms number fields
				EFalse,
				EFalse );					
	}

/*******************************************************************
*
*	SendKeyEventsL() for running nightly build
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SendKeyEventsL(  )
	{

	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( testedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

/*******************************************************************
*
*	SendKeyEventsL() for running nightly build
*
*******************************************************************/
void CMTPbkMmsAddressSelect::SendKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

//  [End of File] - Do not remove
