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
#include "MTPbkSingleEntryFetchDlg.h"
#include <CPbkSingleEntryFetchDlg.h>

#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>

#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"

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
// CMTPbkSingleEntryFetchDlg::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkSingleEntryFetchDlg::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkSingleEntryFetchDlg::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkSingleEntryFetchDlg::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkSingleEntryFetchDlg::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item", CMTPbkSingleEntryFetchDlg::RunExecuteL ),
        ENTRY( "Test_with_default_contact", CMTPbkSingleEntryFetchDlg::RunExecute1L ),
        ENTRY( "Test_with_two_default_contact", CMTPbkSingleEntryFetchDlg::RunExecute2L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/*******************************************************************
*
*	SetupDefaultContactItemL ()
*
*******************************************************************/
void CMTPbkSingleEntryFetchDlg::SetupDefaultContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    iContactEngine->AddNewContactL(*iContactItem);
    }

/*******************************************************************
*
*	SetupEmptyContactItemL ()
*

*******************************************************************/
void CMTPbkSingleEntryFetchDlg::SetupEmptyContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);
    iContactItem = iContactEngine->CreateEmptyContactL();
    iContactEngine->AddNewContactL(*iContactItem);
    }

/*******************************************************************
*
*	SetupTwoDefaultContactItemL ()
*
*******************************************************************/
void CMTPbkSingleEntryFetchDlg::SetupTwoDefaultContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->DuplicateContactL(iContactItem->Id(),EFalse);
    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkSingleEntryFetchDlg::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
	delete iContactEngine;
	iContactEngine = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkSingleEntryFetchDlg::RunTest_ClassSizeL()
    {
    SetupDefaultContactItemL();
    static const TInt sizeof_class_CPbkSingleEntryFetchDlg_TParams = 28;

    RDebug::Printf("Class: sizeof(class CPbkSingleEntryFetchDlg::TParams)=%d\n", 
                sizeof(class CPbkSingleEntryFetchDlg::TParams));

    ASSERT( sizeof(class CPbkSingleEntryFetchDlg::TParams) >= 
                sizeof_class_CPbkSingleEntryFetchDlg_TParams );
	Teardown();
   	return KErrNone;         
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleEntryFetchDlg::RunExecuteL(  )
    {
    SetupEmptyContactItemL();

    CPbkSingleEntryFetchDlg::TParams params;
        // Use the engine's default contact array
    params.iContactView = &iContactEngine->AllContactsView();

    iSingleEntryFetchDlg = CPbkSingleEntryFetchDlg::NewL(params);
    iSingleEntryFetchDlg->ResetWhenDestroyed(&iSingleEntryFetchDlg);

	HandleKeyEventsL( );	
    
	TInt                result = ETrue;
    result = iSingleEntryFetchDlg->ExecuteLD();

   	ASSERT( iSingleEntryFetchDlg == NULL );

   	ASSERT( params.iSelectedEntry == iContactItem->Id());
   	Teardown();
   	return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleEntryFetchDlg::RunExecute1L(  )
    {
    SetupDefaultContactItemL();
    TInt                result = ETrue;

    CPbkSingleEntryFetchDlg::TParams params;
        // Use the engine's default contact array
    params.iContactView = &iContactEngine->AllContactsView();

    iSingleEntryFetchDlg = CPbkSingleEntryFetchDlg::NewL(params);
    iSingleEntryFetchDlg->ResetWhenDestroyed(&iSingleEntryFetchDlg);

	HandleKeyEventsL( );	
    
    result = iSingleEntryFetchDlg->ExecuteLD();

   	ASSERT( iSingleEntryFetchDlg == NULL );

   	ASSERT( params.iSelectedEntry == iContactItem->Id());
   	Teardown();
   	return KErrNone;
    }
    
/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleEntryFetchDlg::RunExecute2L(  )
    {
    SetupTwoDefaultContactItemL();
    TInt                result = ETrue;

    CPbkSingleEntryFetchDlg::TParams params;
        // Use the engine's default contact array
    params.iContactView = &iContactEngine->AllContactsView();

    iSingleEntryFetchDlg = CPbkSingleEntryFetchDlg::NewL(params);
    iSingleEntryFetchDlg->ResetWhenDestroyed(&iSingleEntryFetchDlg);

	HandleKeyEventsL( );	
    
    result = iSingleEntryFetchDlg->ExecuteLD();

   	ASSERT( iSingleEntryFetchDlg == NULL );

   	ASSERT( params.iSelectedEntry == iContactItem->Id());
   	Teardown();
   	return KErrNone;
    }
    

/*******************************************************************
*
*	HandleKeyEventsL ()
*
*******************************************************************/
void CMTPbkSingleEntryFetchDlg::HandleKeyEventsL( )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	//leftSoftKeyCode declared in CSimulateKeyEvents.h
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

//  [End of File] - Do not remove
