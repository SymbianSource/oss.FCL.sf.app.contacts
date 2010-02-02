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
#include "MTPbkPhoneNumberSelect.h"
#include <CPbkPhoneNumberSelect.h>
#include <CPbkPhoneNumberSelect.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>
#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"
#include <pbkfields.hrh>
#include <e32def.h>
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
// CMTPbkPhoneNumberSelect::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkPhoneNumberSelect::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkPhoneNumberSelect::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkPhoneNumberSelect::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkPhoneNumberSelect::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item", CMTPbkPhoneNumberSelect::RunExecuteEmptyContactL ),
        ENTRY( "Test_with_empty_contact_itemFocusNULL", CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNullL ),
        ENTRY( "Test_with_empty_contact_itemDirectlyTRUE", CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrueL ),
        ENTRY( "Test_with_contact_item_with_name_fields", CMTPbkPhoneNumberSelect::RunExecuteWithContactNameL ),
        ENTRY( "Test_with_contact_item_with_company_name", CMTPbkPhoneNumberSelect::RunExecuteWithCompanyNameL ),
        ENTRY( "Test_with_contact_item_with_name_fieldsFocusNULL", CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNull1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldFocusNULL", CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNull2L ),
        ENTRY( "Test_with_contact_item_with_name_fieldsDirectlyTRUE", CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrue1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldDirectlyTRUE", CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrue2L ),
        ENTRY( "Test_one_phone_number", CMTPbkPhoneNumberSelect::RunExecuteWithOnePhoneNumberL ),
        ENTRY( "Test_one_phone_numberFocusNULL", CMTPbkPhoneNumberSelect::RunExecuteWithOneNumberFocusNullL ),
        ENTRY( "Test_one_phone_numberDirectlyTRUE", CMTPbkPhoneNumberSelect::RunExecuteWithOneNumberDirectlyTrueL ),
        ENTRY( "Test_two_phone_numbers_and_key_event_simulation", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberL ),
        ENTRY( "Test_focus_on_phone_number_DirectlyFALSE", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndFocusL ),
        ENTRY( "Test_two_phone_numbers_one_default_email", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndOneDefaultL ),
        ENTRY( "Test_two_phone_numbers_one_default_and_one_focused_phone_number", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndOneFocusAndDefaultL ),
        ENTRY( "Test_One_default_company_focus_ret_default_number_focused", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberCompanyFocusL ),
        ENTRY( "Test_One_default_NULL_focus_ret_default_number_focused", CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberNullFocusL ),
        ENTRY( "Test__Soft_key_events", CMTPbkPhoneNumberSelect::RunExecuteTestSoftKeys1L ),
        ENTRY( "Test__Soft_key_events", CMTPbkPhoneNumberSelect::RunExecuteTestSoftKeys2L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkPhoneNumberSelect::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
	
	if (iPhoneNumberSelect)
		{
		delete iPhoneNumberSelect;
		iPhoneNumberSelect = NULL;
		}
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunTest_ClassSizeL()
    {
    SetupEmptyContactItemL();
    static const TInt sizeof_class_CPbkPhoneNumberSelect_30 = 32;
    static const TInt sizeof_class_CPbkPhoneNumberSelect_TParams = 32;

    RDebug::Printf("Class: sizeof(class CPbkPhoneNumberSelect)=%d\n", 
                sizeof(class CPbkPhoneNumberSelect));
    RDebug::Printf("Class: sizeof(class CPbkPhoneNumberSelect::TParams)=%d\n", 
                sizeof(class CPbkPhoneNumberSelect::TParams));

    ASSERT( sizeof(class CPbkPhoneNumberSelect) >= 
                sizeof_class_CPbkPhoneNumberSelect_30 );
    ASSERT( sizeof(class CPbkPhoneNumberSelect::TParams) >= 
                sizeof_class_CPbkPhoneNumberSelect_TParams );
    Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteEmptyContact()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteEmptyContactL()
	{
	SetupEmptyContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNullL()
	{
	SetupEmptyContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNull1L()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}
	
/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseFocusNull2L()
	{
	SetupContactItemWithCompanyNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	TPbkContactItemField* 	focusedField = NULL;

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrueL()
	{
	SetupEmptyContactItemL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;
	
	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrue1L()
	{
	SetupContactItemWithNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;
	
	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}
	
/*******************************************************************
*
*	RunExecuteTestCaseDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteTestCaseDirectlyTrue2L()
	{
	SetupContactItemWithCompanyNameL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = ETrue,
							ret = ETrue;
	
	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithContactName()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithContactNameL()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	// Focus to first name field
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;
	
	//focus on contacts first name
	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithCompanyName()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithCompanyNameL()
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

	// Return value must be EFalse if canceled or no email address found.
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOnePhoneNumberL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithOnePhoneNumberL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt 					search = 0,
                            // Setup initizes general phone number.
                            type = EPbkFieldIdPhoneNumberGeneral,
	                        index = 0;
	TPbkContactItemField* 	focusedField = NULL;
	
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search);
	ASSERT( focusedField );

    SendKeyEventsL();
    
	ret = RunExecuteL( *iContactItem,focusedField,callDefaultDirectly );	
	ASSERT( ret );

	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( type, index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneNumberFocusNullL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithOneNumberFocusNullL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
                            // Setup initizes general phone number.
    TInt                    type = EPbkFieldIdPhoneNumberGeneral,
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
*	RunExecuteWithOneNumberDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithOneNumberDirectlyTrueL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
    TInt                    type = EPbkFieldIdPhoneNumberGeneral,
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
*	RunExecuteWithMultiNumberL()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to false.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Later focus is set to EPbkFieldIdPhoneNumberHome field.
	TPbkContactItemField* 	focusedField = NULL; 
	TInt 					acceptKeyCounter = 0,
							phoneNumbers = 0,
                            type = EPbkFieldIdPhoneNumberGeneral;
	TKeyCode                selectionKeyCode = EKeyNull;
	
	// A loop for simulation of select key actions.
	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		for (phoneNumbers=0; 
		    phoneNumbers < iAddressAndNumberSelect->PhoneNumbers(); 
		    phoneNumbers++)
			{
			iSimulateKeyEvents->ClearAllKeyEventsL();

			// User down arrow key actions are added here.
			for( TInt downKeyCounter = 0 ;
				 downKeyCounter < phoneNumbers ; 
				 downKeyCounter++ )
				{
				iSimulateKeyEvents->AddKeyEventCodeL( downCode );	
				}

			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			// User selection action is added here.
			iSimulateKeyEvents->AddKeyEventCodeL( selectionKeyCode );
			iSimulateKeyEvents->CallBackStartL();

        	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
        	ASSERT( ret );
			
			// Check return value
        	ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( type, phoneNumbers, *iContactItem );
        	ASSERT( ret );

			} // Email addresses loop ends
		 } // Select keys loop ends
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndFocus()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndFocusL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					focusSearch= 0,
							focusIndex = 0,
                            type = EPbkFieldIdPhoneNumberGeneral;

	for ( focusIndex = 0; 
			focusIndex < iAddressAndNumberSelect->PhoneNumbers(); focusIndex++)
		{
		focusedField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, 
		                                        focusSearch );		
		ASSERT( focusedField );

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
       	ASSERT( ret );
		
		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
		                        type, focusIndex, *iContactItem );
       	ASSERT( ret );

		focusSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndOneDefault()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndOneDefaultL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					type = EPbkFieldIdPhoneNumberGeneral,
                            focusTests = 0,
                            defaultSearch = 0,
                            defaultIndex = 0;

    for ( focusTests=0; focusTests< KFocusTests; focusTests++ )
        {
        switch ( focusTests )
            {
            case 0:
                {
                focusedField = NULL;
                break;
                }
            case 1:
                {
    			focusedField = iContactItem->FindField( EPbkFieldIdEmailAddress );
        		ASSERT( focusedField );
                break;
                }
            default:
                {
               	ASSERT( (focusTests != 0 && focusTests != 1));
                }
            }

		defaultSearch=0;
    	for ( defaultIndex = 0; 
    			defaultIndex < iAddressAndNumberSelect->PhoneNumbers(); defaultIndex++)
    		{
    		defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, 
    		                                        defaultSearch );		
    		ASSERT( defaultField );

			// Set default phone number
			iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
			ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
           	ASSERT( ret );
    		
    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
    		                        type, defaultIndex, *iContactItem );
           	ASSERT( ret );

			defaultSearch++;
    		} // focusIndex loop
        
        }
       Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndOneFocusAndDefault()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberAndOneFocusAndDefaultL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					focusSearch= 0,
                            type = EPbkFieldIdPhoneNumberGeneral,
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

			// Set default phone number
			iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
			ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
           	ASSERT( ret );
    		
    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
    		                        type, focusIndex, *iContactItem );
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
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberCompanyFocusL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					focusSearch= 0,
                            type = EPbkFieldIdPhoneNumberGeneral,
                            defaultSearch = 0,
                            phoneNumber = 0,
                            search = 0,
                            acceptKeyCounter = 0;
	TKeyCode                selectionKeyCode = EKeyNull;

	// Give focus to every phone number field
	for ( acceptKeyCounter=0; acceptKeyCounter< KSelectKeys; acceptKeyCounter++)
	    {
        search = 0;
		focusedField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, focusSearch );
   		ASSERT( focusedField );

		defaultSearch = 0;
    	for ( phoneNumber = 0; 
    			phoneNumber < iAddressAndNumberSelect->PhoneNumbers(); phoneNumber++)
    		{
    		defaultField= iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, 
    		                                        search );		
    		ASSERT( defaultField );

			// Set default phone number
			iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
			ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );	
	
    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
           	ASSERT( ret );
    		
    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
    		                        type, phoneNumber, *iContactItem );
           	ASSERT( ret );

			defaultSearch++; // Take next phone number field.
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
TInt CMTPbkPhoneNumberSelect::RunExecuteWithMultiNumberNullFocusL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TInt 					type = EPbkFieldIdPhoneNumberGeneral,
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
			iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
			ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );	
	
    		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
           	ASSERT( ret );
    		
    		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
    		                        type, phoneNumber, *iContactItem );
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
TInt CMTPbkPhoneNumberSelect::RunExecuteTestSoftKeys1L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();						  
	TInt 					focusIndex = 0;
	
	// Give focus to every e-mail address field
	for ( focusIndex=0; 
		  focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++ )
		{			
    	TInt defaultSearch = 0;
		TPbkContactItemField* defaultField = iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, 
		                                            defaultSearch );		
   		ASSERT( defaultField );

		iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
		ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

    	TKeyCode testedSoftKeyCode = rightSoftKeyCode;
		SendKeyEventsL( testedSoftKeyCode );	

    	// Set call default directly to false.
    	TBool 					callDefaultDirectly = EFalse,
    							ret = EFalse;
    	TPbkContactItemField* 	noFocusedField = NULL;

		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			
		ASSERT( ret == EFalse);

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
TInt CMTPbkPhoneNumberSelect::RunExecuteTestSoftKeys2L()
	{
	SetupContactItemWithTwoPhoneNumberFieldsL();					  
	TInt 					focusIndex = 0;
	
	// Give focus to every e-mail address field
	for ( focusIndex=0; 
		  focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++ )
		{			
    	TInt defaultSearch = 0;
		TPbkContactItemField* defaultField = iContactItem->FindField( EPbkFieldIdPhoneNumberGeneral, 
			                                            defaultSearch );		
   		ASSERT( defaultField );

		iContactItem->SetDefaultPhoneNumberFieldL( defaultField );
		ASSERT( iContactItem->DefaultPhoneNumberField() == defaultField);

    	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		SendKeyEventsL( testedSoftKeyCode );	

    	// Set call default directly to false.
    	TBool 					callDefaultDirectly = EFalse,
    							ret = EFalse;
    	TPbkContactItemField* 	noFocusedField = NULL;
		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			
		ASSERT( ret );

		TInt type = EPbkFieldIdPhoneNumberGeneral;
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
*******************************************************************/
TBool CMTPbkPhoneNumberSelect::RunExecuteL(
				const CPbkContactItem& aItem, 
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly)
	{
	TBool	ret = EFalse;

	iPhoneNumberSelect = new(ELeave) CPbkPhoneNumberSelect;

	CPbkPhoneNumberSelect::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = iPhoneNumberSelect->ExecuteLD(params);
	//ExecuteLD deletes iPhoneNumberSelect but it has to be zeroed also
	iPhoneNumberSelect = NULL;

	return ret;
	}

/*********************** SETUP **************************************/

void CMTPbkPhoneNumberSelect::SetupEmptyContactItemL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

void CMTPbkPhoneNumberSelect::SetupContactItemWithCompanyNameL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactCompanyNameFieldL( *iContactItem, iContactEngine );
    }

void CMTPbkPhoneNumberSelect::SetupContactItemWithNameL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

void CMTPbkPhoneNumberSelect::SetupDefaultContactItemL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    }

void CMTPbkPhoneNumberSelect::SetupContactItemWithTwoPhoneNumberFieldsL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, EFalse,
			EFalse, EFalse, ETrue);
	}

/*******************************************************************
*
*	SendKeyEventsL()
*
*******************************************************************/
void CMTPbkPhoneNumberSelect::SendKeyEventsL(  )
	{

	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( testedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}
void CMTPbkPhoneNumberSelect::SendKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}


//  [End of File] - Do not remove
