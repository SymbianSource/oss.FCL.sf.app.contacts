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
#include "MTPbkPocAddressSelect.h"
#include <CPbkPocAddressSelect.h>
#include <eikenv.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include "T_AddressAndNumberSelect.h"
#include "CSimulateKeyEvents.h"
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
// CMTPbkPocAddressSelect::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkPocAddressSelect::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkPocAddressSelect::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkPocAddressSelect::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class_", CMTPbkPocAddressSelect::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item", CMTPbkPocAddressSelect::RunExecuteEmptyContactL ),
        ENTRY( "Test_with_empty_contact_itemFocusNULL_", CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNullL ),
        ENTRY( "Test_with_empty_contact_itemDirectlyTRUE_", CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrueL ),
        ENTRY( "Test_with_contact_item_with_name_fields", CMTPbkPocAddressSelect::RunExecuteWithContactNameL ),
        ENTRY( "Test_with_contact_item_with_company_name_", CMTPbkPocAddressSelect::RunExecuteWithCompanyNameL ),
        ENTRY( "Test_with_contact_item_with_name_fieldsFocusNULL_", CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNull1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldFocusNULL_", CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNull2L ),
        ENTRY( "Test_with_contact_item_with_name_fieldsirectlyTRUE", CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrue1L ),
        ENTRY( "Test_with_contact_item_with_company_name_fieldDirectlyTRUE_", CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrue2L ),
        ENTRY( "Test_with_one_email_address", CMTPbkPocAddressSelect::RunExecuteWithOneEmailL ),
        ENTRY( "Test_one_email_addressFocusNULL_", CMTPbkPocAddressSelect::RunExecuteWithOneEmailFocusNullL ),
        ENTRY( "Test_one_email_addressDirectlyTRUE_", CMTPbkPocAddressSelect::RunExecuteWithOneEmailDirectlyTrueL ),
        ENTRY( "Test_two_email_addresses_and_key_event_simulation_", CMTPbkPocAddressSelect::RunExecuteWithMultiEmailL ),
        ENTRY( "Test_focus_on_phone_number_FocusNULL_", CMTPbkPocAddressSelect::RunExecuteWithMultiEmailFocusNullL ),
        ENTRY( "Test_two_email_addresses_one_default_email", CMTPbkPocAddressSelect::RunExecuteWithMultiEmailAndDefaultEmailL ),
        ENTRY( "Test_two_email_addresses_one_default_and_one_focused_email_address_", CMTPbkPocAddressSelect::RunExecuteMultiEmailWithDefaultEmailAndFocusL ),
        ENTRY( "Test_One_default_no_email_focus_ret_default_address_focused_", CMTPbkPocAddressSelect::RunExecuteMultiEmailWithFocusL ),        
        ENTRY( "Test_One_default_no_focus_ret_default_address_focused_", CMTPbkPocAddressSelect::RunExecuteMultiEmailWithNoFocusL ),
        ENTRY( "Test_Different_key_events_", CMTPbkPocAddressSelect::RunExecuteTestSoftKeys1L ),
        ENTRY( "Test_Different_key_events_", CMTPbkPocAddressSelect::RunExecuteTestSoftKeys2L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkPocAddressSelect::Teardown(  )
    {
    delete iContactItem;
	iContactItem = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunTest_ClassSizeL()
    {
    SetupEmptyContactItemL();
    static const TInt sizeof_class_CPbkPocAddressSelect_30 = 32;
    static const TInt sizeof_class_CPbkPocAddressSelect_TParams = 32;

    RDebug::Printf("Class: sizeof(class CPbkPocAddressSelect)=%d\n", 
                sizeof(class CPbkPocAddressSelect));
    RDebug::Printf("Class: sizeof(class CPbkPocAddressSelect::TParams)=%d\n", 
                sizeof(class CPbkPocAddressSelect::TParams));

    ASSERT( sizeof(class CPbkPocAddressSelect) >= 
                sizeof_class_CPbkPocAddressSelect_30 );
    ASSERT( sizeof(class CPbkPocAddressSelect::TParams) >= 
                sizeof_class_CPbkPocAddressSelect_TParams );
    Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteEmptyContactL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteEmptyContactL()
	{
	SetupEmptyContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret;
	// Focus to first name field
	TInt 					search=0;
	TPbkContactItemField* 	focusedField;

	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );	

	// Return value must be EFalse if canceled 
    // or no email address found.
	ASSERT( ret == EFalse);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestCase1FocusNullL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNullL()
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
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNull1L()
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
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseFocusNull2L()
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
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrueL()
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
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrue1L()
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
TInt CMTPbkPocAddressSelect::RunExecuteTestCaseDirectlyTrue2L()
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
TInt CMTPbkPocAddressSelect::RunExecuteWithContactNameL()
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
TInt CMTPbkPocAddressSelect::RunExecuteWithCompanyNameL()
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
*	RunExecuteWithOneEmail()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithOneEmailL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt 					search = 0,
			                index = 0;
	TPbkContactItemField* 	focusedField = NULL;
	
	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search);
	ASSERT( focusedField );

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem,focusedField,callDefaultDirectly );	
	ASSERT( ret);

	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmailFocusNullL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithOneEmailFocusNullL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					index = 0;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret);

	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmailDirectlyTrueL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithOneEmailDirectlyTrueL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TInt 					index = 0;

	SendKeyEventsL();

	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
	ASSERT( ret);

	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( index, *iContactItem );
	ASSERT( ret );
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiEmail()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithMultiEmailL()
	{	
	SetupContactItemWithTwoEmailFieldsL();
	// Set call default directly to false.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Later focus is set to EPbkFieldIdPhoneNumberHome field.
	TPbkContactItemField* 	focusedField = NULL; 
	TInt 					focusSearch = 0,
							focusIndex = 0,
	     					acceptKeyCounter = 0;
	TKeyCode 				selectionKeyCode = EKeyNull;
	
	// Give focus to every phone number field
	for (focusIndex = 0; 
		focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++)
		{
		focusedField= iContactItem->FindField( EPbkFieldIdEmailAddress, focusSearch );		
		ASSERT( focusedField );

		iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
		ASSERT( selectionKeyCode != EKeyNull );

		SendKeyEventsL( selectionKeyCode );	

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
		ASSERT( ret);

		ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( focusIndex, *iContactItem );
		ASSERT( ret );

		focusSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiEmailFocusNullL()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithMultiEmailFocusNullL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TKeyCode 				selectionKeyCode = EKeyNull;
	TInt 					acceptKeyCounter = 0,
							downKeyCounter = 0,
							findEmailAddress = 0;

	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		for ( findEmailAddress = 0; 
			findEmailAddress < iAddressAndNumberSelect->EmailAddresses(); 
			findEmailAddress++)
			{
			for( downKeyCounter = 0; downKeyCounter < findEmailAddress; downKeyCounter++ )
				{
				iSimulateKeyEvents->AddKeyEventCodeL( downCode );	
				}

			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );	

			ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
			ASSERT( ret);

			ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( findEmailAddress, *iContactItem );
			ASSERT( ret );

			ASSERT( ret );
			} // Email addresses loop ends
		}// Select keys loop ends
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiEmailAndDefaultEmail()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteWithMultiEmailAndDefaultEmailL()
	{	
	SetupContactItemWithTwoEmailFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = ETrue,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	noFocusedField = NULL;
	TInt 					focusSearch= 0,
							focusIndex = 0,
	     					acceptKeyCounter = 0;
	TKeyCode 				selectionKeyCode = EKeyNull;

	for ( focusIndex = 0; 
			focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++)
		{
		focusedField= iContactItem->FindField( EPbkFieldIdEmailAddress, focusSearch );		
		ASSERT( focusedField );

		iContactItem->SetDefaultEmailFieldL( focusedField );
		ASSERT( iContactItem->DefaultEmailField() == focusedField);

		iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
		ASSERT( selectionKeyCode != EKeyNull );

		SendKeyEventsL( selectionKeyCode );	

		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
		ASSERT( ret);
		
		ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( focusIndex, *iContactItem );
		ASSERT( ret );

		focusSearch++;
		} // focusIndex loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteMultiEmailWithDefaultEmailAndFocus()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteMultiEmailWithDefaultEmailAndFocusL()
	{	
	SetupContactItemWithTwoEmailFieldsL();
	TInt 					focusSearch = 0,
							defaultSearch = 0,
							focusIndex = 0,
							findEmailAddress = EFalse,
	     					acceptKeyCounter = 0;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField*	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TKeyCode 				selectionKeyCode = EKeyNull;

	// Give focus to every e-mail address field
	for ( focusIndex = 0; 
		focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++)
		{
		focusedField= iContactItem->FindField( EPbkFieldIdEmailAddress, focusSearch );		
		ASSERT( focusedField );

		defaultSearch = 0;
		for ( findEmailAddress = 0; 
			findEmailAddress < iAddressAndNumberSelect->EmailAddresses(); 
			findEmailAddress++)
			{
			defaultField = iContactItem->FindField( EPbkFieldIdEmailAddress, defaultSearch );		
			ASSERT( defaultField );

			iContactItem->SetDefaultEmailFieldL( defaultField );
			ASSERT( iContactItem->DefaultEmailField() == defaultField);
	
    		iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
    		ASSERT( selectionKeyCode != EKeyNull );

    		SendKeyEventsL( selectionKeyCode );	

			ret = RunExecuteL( *iContactItem, focusedField,	callDefaultDirectly );	
			ASSERT( ret);

			ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( focusIndex, *iContactItem );
			ASSERT( ret );

			defaultSearch++; // Take next e-mail address field.
			} // i loop

		focusSearch++; // update focus for the next possible search round
		} // index loop
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteMultiEmailWithFocus()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteMultiEmailWithFocusL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	TInt 					search = 0,
							acceptKeyCounter = 0,
							findEmailAddress = 0;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	defaultField = NULL;
	TKeyCode 				selectionKeyCode = EKeyNull;

	focusedField = iContactItem->FindField( EPbkFieldIdCompanyName, search );		
	ASSERT( focusedField);

	// Go through different kind of accept key events.
	for( acceptKeyCounter=1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		search=0; 
		
		for ( findEmailAddress = 0; 
			findEmailAddress< iAddressAndNumberSelect->EmailAddresses(); 
			findEmailAddress++)
			{
			defaultField = iContactItem->FindField( EPbkFieldIdEmailAddress, search );		
			ASSERT( defaultField );

			iContactItem->SetDefaultEmailFieldL( defaultField );
			ASSERT( iContactItem->DefaultEmailField() == defaultField);

			// Simulate enter key code.
			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );	
	
			ret = RunExecuteL( *iContactItem, focusedField,	callDefaultDirectly );	
			ASSERT( ret);
		
			// Check return values
			ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( findEmailAddress, *iContactItem );
			ASSERT( ret );

			search++; // Take next e-mail address field.
			}// for EmailAddresses
		}// for EmailSelectionKeys
	Teardown();
	return KErrNone;
	}
	
/*******************************************************************
*
*	RunExecuteMultiEmailWithNoFocus()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteMultiEmailWithNoFocusL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	// This test case is almost like previous 7.
	// Set call default directly to false.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	TPbkContactItemField* 	noFocusedField = NULL;
	TInt 					focusSearch = 0,
							acceptKeyCounter = 0,
							focusIndex = 0;
	TKeyCode 				selectionKeyCode = EKeyNull;

	for( acceptKeyCounter = 1; acceptKeyCounter < KSelectKeys; acceptKeyCounter++ )
		{
		focusSearch=0;
		for ( focusIndex = 0; 
			focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++)
			{
			focusedField= iContactItem->FindField( EPbkFieldIdEmailAddress, focusSearch );
			ASSERT( focusedField );
		
			iContactItem->SetDefaultEmailFieldL( focusedField );
			ASSERT( iContactItem->DefaultEmailField() == focusedField);

			iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
			ASSERT( selectionKeyCode != EKeyNull );

			SendKeyEventsL( selectionKeyCode );	
			
			ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			ASSERT( ret);
		
			ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( focusIndex, *iContactItem );
			ASSERT( ret );

			focusSearch++;
			} // focusIndex loop ends
		} // acceptKeyCounter loop ends
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteTestSoftKeys1L()
*
*******************************************************************/
TInt CMTPbkPocAddressSelect::RunExecuteTestSoftKeys1L()
	{
	SetupContactItemWithTwoEmailFieldsL();						  
	TInt 					focusIndex = 0;
	
	// Give focus to every e-mail address field
	for ( focusIndex=0; 
		  focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++ )
		{			
    	TInt defaultSearch = 0;
		TPbkContactItemField* defaultField = iContactItem->FindField( EPbkFieldIdEmailAddress, defaultSearch );		
		ASSERT( defaultField );

		iContactItem->SetDefaultEmailFieldL( defaultField );
		ASSERT( iContactItem->DefaultEmailField() == defaultField);

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
TInt CMTPbkPocAddressSelect::RunExecuteTestSoftKeys2L()
	{
	SetupContactItemWithTwoEmailFieldsL();								  
	TInt 					focusIndex = 0;

	// Give focus to every e-mail address field
	for ( focusIndex=0; 
		  focusIndex < iAddressAndNumberSelect->EmailAddresses(); focusIndex++ )
		{			
    	TInt defaultSearch = 0;
		TPbkContactItemField* defaultField = iContactItem->FindField( EPbkFieldIdEmailAddress, defaultSearch );		
		ASSERT( defaultField );

		iContactItem->SetDefaultEmailFieldL( defaultField );
		ASSERT( iContactItem->DefaultEmailField() == defaultField);

    	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		SendKeyEventsL( testedSoftKeyCode );	

    	// Set call default directly to false.
    	TBool 					callDefaultDirectly = EFalse,
    							ret = EFalse;
    	TPbkContactItemField* 	noFocusedField = NULL;
    	
		ret = RunExecuteL( *iContactItem, noFocusedField, callDefaultDirectly );	
			
		ASSERT( ret);

		ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( focusIndex, *iContactItem );
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
*	Runs CPbkPocAddressSelect's ExecuteLD method
*
*******************************************************************/
TBool CMTPbkPocAddressSelect::RunExecuteL(
				const CPbkContactItem& aItem, 
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly)
	{
	TBool	ret = EFalse;

	iPocAddressSelect = new(ELeave) CPbkPocAddressSelect;

	CPbkPocAddressSelect::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = iPocAddressSelect->ExecuteLD(params);
	//ExecuteLD deletes iPocAddressSelect but it has to be zeroed also
	iPocAddressSelect = NULL;
	return ret;

	}

/*********************** SETUP *************************************
*
* Creates and initialises one empty CPbkContactItem object.
*
*******************************************************************/
void CMTPbkPocAddressSelect::SetupEmptyContactItemL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

void CMTPbkPocAddressSelect::SetupContactItemWithCompanyNameL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactCompanyNameFieldL( *iContactItem, iContactEngine );
    }

void CMTPbkPocAddressSelect::SetupContactItemWithNameL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

void CMTPbkPocAddressSelect::SetupDefaultContactItemL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    }

void CMTPbkPocAddressSelect::SetupContactItemWithTwoEmailFieldsL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, ETrue,
			EFalse, EFalse, EFalse);				
	}

void CMTPbkPocAddressSelect::SendKeyEventsL( )
	{

	TKeyCode testedSoftKeyCode = leftSoftKeyCode;

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( testedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

void CMTPbkPocAddressSelect::SendKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}


//  [End of File] - Do not remove
