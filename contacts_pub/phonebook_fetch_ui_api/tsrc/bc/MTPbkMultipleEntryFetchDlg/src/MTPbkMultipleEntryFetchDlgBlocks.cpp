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
#include "MTPbkMultipleEntryFetchDlg.h"
#include <CPbkMultipleEntryFetchDlg.h>
#include <Coemain.h>
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
// CMTPbkMultipleEntryFetchDlg::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkMultipleEntryFetchDlg::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkMultipleEntryFetchDlg::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkMultipleEntryFetchDlg::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkMultipleEntryFetchDlg::RunTest_ClassSizeL ),
        ENTRY( "ExecuteLD_test", CMTPbkMultipleEntryFetchDlg::RunExecuteL ),
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
void CMTPbkMultipleEntryFetchDlg::SetupDefaultContactItemL()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->DuplicateContactL(iContactItem->Id());
    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkMultipleEntryFetchDlg::Teardown(  )
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
TInt CMTPbkMultipleEntryFetchDlg::RunTest_ClassSizeL()
    {
    SetupDefaultContactItemL();
    static const TInt sizeof_class_CPbkMultipleEntryFetchDlg_TParams = 16;

    RDebug::Printf("Class: sizeof(class CPbkMultipleEntryFetchDlg::TParams)=%d\n", 
            sizeof(class CPbkMultipleEntryFetchDlg::TParams));

    ASSERT( sizeof(class CPbkMultipleEntryFetchDlg::TParams) >= sizeof_class_CPbkMultipleEntryFetchDlg_TParams );
    Teardown();
    return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkMultipleEntryFetchDlg::RunExecuteL(  )
    {
    SetupDefaultContactItemL();
    CContactViewBase& allContactsView = iContactEngine->AllContactsView();

    CPbkMultipleEntryFetchDlg::TParams params;
    params.iContactView = &allContactsView;

    iMultiEntryFetchDlg = CPbkMultipleEntryFetchDlg::NewL(params, *iContactEngine);

	HandleKeyEventsL( );	

    TInt result = iMultiEntryFetchDlg->ExecuteLD();
    iMultiEntryFetchDlg = NULL;

    CleanupStack::PushL(params);

    if (result)
        {
        const TContactItemId cid = (*params.iMarkedEntries)[0];
        CPbkContactItem* pbkItem = iContactEngine->ReadContactLC(cid);
        CPbkContactItem* pbkItem2 = iContactEngine->ReadContactLC(1);

        ASSERT( pbkItem->Id()==pbkItem2->Id() );
        CleanupStack::PopAndDestroy(pbkItem2);
        CleanupStack::PopAndDestroy(pbkItem);
        }

    CleanupStack::PopAndDestroy(); // params
    ASSERT( result );
    Teardown();
    return KErrNone;
    }

/*******************************************************************
*
*	HandleKeyEventsL ()
*
*******************************************************************/
void CMTPbkMultipleEntryFetchDlg::HandleKeyEventsL( )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();

	}

//  [End of File] - Do not remove
