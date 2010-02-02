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
#include "MTPbkSingleItemFetchDlg.h"
#include <CPbkSingleItemFetchDlg.h>

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
// CMTPbkSingleItemFetchDlg::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkSingleItemFetchDlg::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkSingleItemFetchDlg::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkSingleItemFetchDlg::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkSingleItemFetchDlg::RunTest_ClassSizeL ),
        ENTRY( "Test_with_empty_contact_item", CMTPbkSingleItemFetchDlg::RunExecuteL ),
        ENTRY( "Test_with_default_contact", CMTPbkSingleItemFetchDlg::RunExecute1L ),
        ENTRY( "Test_with_two_default_contact", CMTPbkSingleItemFetchDlg::RunExecute2L ),
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
void CMTPbkSingleItemFetchDlg::SetupDefaultContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );

    iContactEngine->AddNewContactL(*iContactItem);

    iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);    

	HandleKeyEventsL( );	
    
    }

/*******************************************************************
*
*	SetupEmptyContactItemL ()
*
*******************************************************************/
void CMTPbkSingleItemFetchDlg::SetupEmptyContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();

    iContactEngine->AddNewContactL(*iContactItem);

    iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);    

	iSimulateKeyEvents->ClearAllKeyEventsL();
	//leftSoftKeyCode defined in CSimulateKeyEvents.h
	iSimulateKeyEvents->AddKeyEventCodeL( rightSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();
    }

/*******************************************************************
*
*	SetupTwoDefaultContactItemL ()
*
*******************************************************************/
void CMTPbkSingleItemFetchDlg::SetupTwoDefaultContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );

    iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->DuplicateContactL(iContactItem->Id(),EFalse);

    iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);    

	HandleKeyEventsL( );	
    
    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkSingleItemFetchDlg::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
	
    delete iPeriodic;
    iPeriodic = NULL;

	delete iContactEngine;
	iContactEngine = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkSingleItemFetchDlg::RunTest_ClassSizeL()
    {
    SetupEmptyContactItemL();
    static const TInt sizeof_class_CPbkSingleItemFetchDlg_TParams = 24;

    RDebug::Printf("Class: sizeof(class CPbkSingleItemFetchDlg::TParams)=%d\n", 
                sizeof(class CPbkSingleItemFetchDlg::TParams));

    ASSERT( sizeof(class CPbkSingleItemFetchDlg::TParams) >= 
                sizeof_class_CPbkSingleItemFetchDlg_TParams );
	Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleItemFetchDlg::RunExecuteL(  )
    {
    SetupEmptyContactItemL();
    TInt                result = ETrue;

    CPbkFieldIdArray* fieldIds = new(ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fieldIds);
    fieldIds->AppendL(EPbkFieldIdEmailAddress);

    CPbkSingleItemFetchDlg::TParams params;
    // Use the engine's default contact array
    params.iContactView = &iContactEngine->FilteredContactsViewL(CContactDatabase::EMailable);
    params.iFieldIdArray = fieldIds;
    params.iContactItem = NULL;
    params.iContactItemField = NULL;

	iSingleItemFetchDlg = CPbkSingleItemFetchDlg::NewL(params, *iContactEngine);

    result = iSingleItemFetchDlg->ExecuteLD();
    iSingleItemFetchDlg = NULL;

   	if ( params.iContactItem )
   	    {
       	ASSERT( result );
   	    }
   	else
   	    {
       	ASSERT( !result );
   	    }

    CleanupStack::PopAndDestroy(fieldIds);  // params
	Teardown();
	return KErrNone;
    }
    
/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleItemFetchDlg::RunExecute1L(  )
    {
    SetupDefaultContactItemL();
    TInt                result = ETrue;

    CPbkFieldIdArray* fieldIds = new(ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fieldIds);
    fieldIds->AppendL(EPbkFieldIdEmailAddress);

    CPbkSingleItemFetchDlg::TParams params;
    // Use the engine's default contact array
    params.iContactView = &iContactEngine->FilteredContactsViewL(CContactDatabase::EMailable);
    params.iFieldIdArray = fieldIds;
    params.iContactItem = NULL;
    params.iContactItemField = NULL;

	iSingleItemFetchDlg = CPbkSingleItemFetchDlg::NewL(params, *iContactEngine);

    result = iSingleItemFetchDlg->ExecuteLD();
    iSingleItemFetchDlg = NULL;

   	if ( params.iContactItem )
   	    {
       	ASSERT( result );
   	    }
   	else
   	    {
       	ASSERT( !result );
   	    }

    CleanupStack::PopAndDestroy(fieldIds);  // params
	Teardown();
	return KErrNone;
    }   
    
/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkSingleItemFetchDlg::RunExecute2L(  )
    {
    SetupTwoDefaultContactItemL();
    TInt                result = ETrue;

    CPbkFieldIdArray* fieldIds = new(ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(fieldIds);
    fieldIds->AppendL(EPbkFieldIdEmailAddress);

    CPbkSingleItemFetchDlg::TParams params;
    // Use the engine's default contact array
    params.iContactView = &iContactEngine->FilteredContactsViewL(CContactDatabase::EMailable);
    params.iFieldIdArray = fieldIds;
    params.iContactItem = NULL;
    params.iContactItemField = NULL;

	iSingleItemFetchDlg = CPbkSingleItemFetchDlg::NewL(params, *iContactEngine);

    result = iSingleItemFetchDlg->ExecuteLD();
    iSingleItemFetchDlg = NULL;

   	if ( params.iContactItem )
   	    {
       	ASSERT( result );
   	    }
   	else
   	    {
       	ASSERT( !result );
   	    }

    CleanupStack::PopAndDestroy(fieldIds);  // params
	Teardown();
	return KErrNone;
    }
/*******************************************************************
*
*	HandleKeyEventsL ()
*
*******************************************************************/
void CMTPbkSingleItemFetchDlg::HandleKeyEventsL( )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	//leftSoftKeyCode defined in CSimulateKeyEvents.h
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );

    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    iPeriodic->Start(3000000, 0,TCallBack(CMTPbkSingleItemFetchDlg::TestCallBackL,this));

	iSimulateKeyEvents->CallBackStartL();

	}

TInt CMTPbkSingleItemFetchDlg::TestCallBackL(TAny* aThis)
    {
    CMTPbkSingleItemFetchDlg* singleItemFetchDlg = reinterpret_cast<CMTPbkSingleItemFetchDlg*>(aThis);
    singleItemFetchDlg->HandleKeyEvents2L();
    return EFalse;
    }

/*******************************************************************
*
*	HandleKeyEvents2L ()
*
*******************************************************************/
void CMTPbkSingleItemFetchDlg::HandleKeyEvents2L( )
	{
    //this HandleKeyEvents is created for giving key events to second
    //dialog. HandleKeyEvents passed key events only to the first dialog
    //(Select Contact dialog) and these key events are sent to second
    //dialog (Select Details dialog)
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();
    iPeriodic->Cancel();
	}

//  [End of File] - Do not remove
