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
#include "MTPbkPhoneNumberSelectBase.h"
#include <eikenv.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <coneresloader.h>

#include "CPhoneNumberSelectBase.h"
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
// CMTPbkPhoneNumberSelectBase::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkPhoneNumberSelectBase::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkPhoneNumberSelectBase::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkPhoneNumberSelectBase::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Testwithcontactitemwithnamefields", CMTPbkPhoneNumberSelectBase::RunExecuteWithContactNameL ),
        ENTRY( "Testtwophonenumbersandkeyeventsimulation", CMTPbkPhoneNumberSelectBase::RunExecuteWithMultiNumberAndFocusL ),
        ENTRY( "Testtwophonenumbersandkeyeventsimulation", CMTPbkPhoneNumberSelectBase::RunExecuteWithMultiNumberAndNoFocusL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkPhoneNumberSelectBase::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
    }

/*******************************************************************
*
*	RunExecuteWithContactName()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelectBase::RunExecuteWithContactNameL()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	// Focus to first name field
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	//focus on contacts first name
	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField  );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT( ret == EFalse  );
    Teardown();
    return KErrNone;
	}


/*******************************************************************
*
*	RunExecuteWithMultiNumberAndFocus()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelectBase::RunExecuteWithMultiNumberAndFocusL()
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
		ASSERT( focusedField  );

		ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );	
       	ASSERT( ret );
		
		ret = iAddressAndNumberSelect->IsCorrectPhoneNumberL( 
		                        type, focusIndex, *iContactItem );
       	ASSERT( ret);

		focusSearch++;
		} // focusIndex loop
	    Teardown();
    return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiNumberAndNoFocus()
*
*******************************************************************/
TInt CMTPbkPhoneNumberSelectBase::RunExecuteWithMultiNumberAndNoFocusL()
	{	
	SetupContactItemWithTwoPhoneNumberFieldsL();
	// Set call default directly to true.
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	// Set No focused field
	TPbkContactItemField* 	focusedField = NULL;
	
	TInt 					type = EPbkFieldIdPhoneNumberGeneral,                            
                            phoneNumbers = 0,
                            acceptKeyCounter = 0;
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
        	ASSERT( ret);

			} // Email addresses loop ends
        } // Select keys loop ends
    Teardown();
    return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TBool CMTPbkPhoneNumberSelectBase::RunExecuteL(
				const CPbkContactItem& aItem,
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly)
	{
	TBool	ret = EFalse;

	iPhoneNumberSelectBase = new(ELeave) CPhoneNumberSelectBase ( *this );

	CPhoneNumberSelectBase::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = iPhoneNumberSelectBase->ExecuteLD(params);
	//ExecuteLD deletes iPhoneNumberSelectBase but it has to be zeroed also
	iPhoneNumberSelectBase = NULL;

	return ret;
	}

/*********************** SETUP **************************************/

void CMTPbkPhoneNumberSelectBase::SetupContactItemWithNameL()
	{
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

void CMTPbkPhoneNumberSelectBase::SetupContactItemWithTwoPhoneNumberFieldsL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, EFalse,
			EFalse, EFalse, ETrue);
	}

/*******************************************************************
*
*	HandleKeyEventsL()
*
*******************************************************************/
void CMTPbkPhoneNumberSelectBase::HandleKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

//  [End of File] - Do not remove
