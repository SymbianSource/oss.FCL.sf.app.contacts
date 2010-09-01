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
#include "MTPbkSelectFieldDlg.h"
#include <eikenv.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <e32base.h>
#include <avkon.rsg> // R_AVKON_SOFTKEYS_SELECT_CANCEL;
#include <CPbkFieldInfo.h>


#include "T_AddressAndNumberSelect.h"
#include "CPbkSelectFieldDlg.h"
//#include "MPbkControlKeyObserver.h"
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
// CMTPbkSelectFieldDlg::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkSelectFieldDlg::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkSelectFieldDlg::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkSelectFieldDlg::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkSelectFieldDlg::RunTest_ClassSizeL ),
        ENTRY( "Test_with_one_email_address", CMTPbkSelectFieldDlg::RunExecuteWithMultipleEmailL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkSelectFieldDlg::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
	
	if ( iSelectFieldDlg )
	    {
	    delete iSelectFieldDlg;
	    iSelectFieldDlg = NULL;
	    }
    }
/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkSelectFieldDlg::RunTest_ClassSizeL()
    {
    SetupContactItemWithTwoEmailFieldsL();
    static const TInt sizeof_class_CPbkSelectFieldDlg_30 = 28;

    RDebug::Printf("Class: sizeof(class CPbkSelectFieldDlg)=%d\n", sizeof(class CPbkSelectFieldDlg));

    ASSERT( sizeof(class CPbkSelectFieldDlg) >= sizeof_class_CPbkSelectFieldDlg_30 );
    Teardown();
    return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteWithMultipleEmailL()
*
*******************************************************************/
TInt CMTPbkSelectFieldDlg::RunExecuteWithMultipleEmailL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	TInt 					search = 1;

	TPbkContactItemField* defaultField = iContactItem->FindField( 
	                        EPbkFieldIdEmailAddress, search );		
	ASSERT( defaultField);

	iContactItem->SetDefaultEmailFieldL( defaultField );
	ASSERT( iContactItem->DefaultEmailField() == defaultField);
								

	TInt		            acceptKeyCounter = 0;
	TKeyCode 				selectionKeyCode = EKeyNull;
	
    // Simulate enter key code.
	iSimulateKeyEvents->EmailSelectKeyCodeL( acceptKeyCounter, selectionKeyCode );
	ASSERT( selectionKeyCode != EKeyNull);

	HandleKeyEventsL( selectionKeyCode );	
	
	TBool ret = RunExecuteL( *iContactItem,defaultField );
	ASSERT( ret);
	Teardown();
    return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TBool CMTPbkSelectFieldDlg::RunExecuteL(
				const CPbkContactItem& aItem,
           		const TPbkContactItemField* aFocusedField)
	{
    const TInt granularity = 8;

    // No direct call to focused or default number, build an array
    // of applicable fields
    CPbkFieldArray* fields = new(ELeave) CPbkFieldArray( granularity );
    CleanupStack::PushL(fields);

    CPbkFieldArray& fieldSet = aItem.CardFields();

    const TInt fieldCount = fieldSet.Count();

    TInt defaultNumberIndex = -1;
    
    for (TInt i=0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = fieldSet[i];
        if (AddressField(field))
            {
            // Append all accepted address fields
            fields->AppendL(field);
            if (aFocusedField && field.IsSame(*aFocusedField))
                {
                // Last appended field was the default, store its
                // index so it can be focused later
                 defaultNumberIndex = fields->Count()-1;
                }
            }
	    }
    
    TBool   ret = EFalse;

    if (fields->Count() > 1)
        {
        TPbkContactItemField*   focusedField = NULL;
        
        // Run the address selection dialog.
        const TDesC& title = *aItem.GetContactTitleL();
        iSelectFieldDlg = new(ELeave) CPbkSelectFieldDlg();
        iSelectFieldDlg->ResetWhenDestroyed(&iSelectFieldDlg);
        TPbkContactItemField* field = iSelectFieldDlg->ExecuteLD( 
                *fields, R_AVKON_SOFTKEYS_SELECT_CANCEL, title, defaultNumberIndex);

    	ASSERT( iSelectFieldDlg == NULL);

        if (field)
            {
            // Field was selected, return it
            focusedField = aItem.FindSameField(*field);
            }

    	ASSERT( focusedField == aFocusedField);
    	ret = ETrue;
        }
    else
        {
    	ASSERT( fields->Count() < 2);
        }

    CleanupStack::PopAndDestroy(fields);
	return ret;
	}

TBool CMTPbkSelectFieldDlg::AddressField(const TPbkContactItemField& aField) const
    {
    // Return true for non-empty email fields
    return (aField.FieldInfo().FieldId() == EPbkFieldIdEmailAddress &&
            !aField.IsEmptyOrAllSpaces());
    }

/*********************** SETUP **************************************/

void CMTPbkSelectFieldDlg::SetupContactItemWithTwoEmailFieldsL()
	{    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, ETrue,
			EFalse, EFalse, EFalse);				
	}

void CMTPbkSelectFieldDlg::HandleKeyEventsL( TKeyCode& aTestedSoftKeyCode )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( aTestedSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}


//  [End of File] - Do not remove
