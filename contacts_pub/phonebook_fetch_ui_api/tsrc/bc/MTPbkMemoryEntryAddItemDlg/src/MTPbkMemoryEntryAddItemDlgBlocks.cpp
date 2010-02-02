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
#include "MTPbkMemoryEntryAddItemDlg.h"
#include <CPbkMemoryEntryAddItemDlg.h>
#include <COEMAIN.h>
#include <avkon.rsg>
#include <eikenv.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include "cntdef.h"
#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"
#include "CPbkFieldsInfo.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
#define R_QTN_PHOB_QTL_PB_INFO                    0x116a2046

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
// CMTPbkMemoryEntryAddItemDlg::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkMemoryEntryAddItemDlg::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkMemoryEntryAddItemDlg::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkMemoryEntryAddItemDlg::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class_", CMTPbkMemoryEntryAddItemDlg::RunTest_ClassSizeL ),
        ENTRY( "ResetWhenDestroyed_test_", CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ResetWhenDestroyedL ),
        ENTRY( "ExecuteLD_with_field_info_", CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLDL ),
        ENTRY( "ExecuteLD_1_with_field_info_and_cbabuttons_", CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLD1L ),
        ENTRY( "ExecuteLD_2_with_field_info_cba_buttons_and_popup_name_", CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLD2L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMTPbkMemoryEntryAddItemDlg::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*******************************************************************
*
*	SetupL()
*
*******************************************************************/

void CMTPbkMemoryEntryAddItemDlg::SetupL(  )
    {
	iMemoryEntryAddItemDlg = new (ELeave) CPbkMemoryEntryAddItemDlg();

	iFieldInfos = new (ELeave) CArrayPtrFlat<CPbkFieldInfo>(1);
	iKeyEventSimulation = CSimulateKeyEvents::NewL();

    }

/*******************************************************************
*
*	Teardown()
*
*******************************************************************/
void CMTPbkMemoryEntryAddItemDlg::Teardown(  )
    {
	delete iFieldInfos;
	iFieldInfos = NULL;
	delete iKeyEventSimulation;
	iKeyEventSimulation = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkMemoryEntryAddItemDlg::RunTest_ClassSizeL()
    {
    SetupL();
    static const TInt 
            sizeof_class_CPbkMemoryEntryAddItemDlg_30 = 24;
    static const TInt 
            sizeof_class_CPbkItemTypeSelectCreateNew = 32;
    static const TInt 
            sizeof_class_CPbkItemTypeSelectAddToExisting = 32;

    RDebug::Printf("Class: sizeof(class CPbkMemoryEntryAddItemDlg)=%d\n", 
                    sizeof(class CPbkMemoryEntryAddItemDlg));
    RDebug::Printf("Class: sizeof(class CPbkItemTypeSelectCreateNew)=%d\n", 
            sizeof(class CPbkItemTypeSelectCreateNew));
    RDebug::Printf("Class: sizeof(class CPbkItemTypeSelectAddToExisting)=%d\n", 
                    sizeof(class CPbkItemTypeSelectAddToExisting));

    ASSERT( sizeof(class CPbkMemoryEntryAddItemDlg) >= 
                sizeof_class_CPbkMemoryEntryAddItemDlg_30 );
    ASSERT( sizeof(class CPbkItemTypeSelectCreateNew) >= 
                sizeof_class_CPbkItemTypeSelectCreateNew );
    ASSERT( sizeof(class CPbkItemTypeSelectAddToExisting) >= 
                sizeof_class_CPbkItemTypeSelectAddToExisting );
    Teardown();
    return KErrNone;
    }

/*******************************************************************
*
*	MT_CPbkMemoryEntryAddItemDlg_ResetWhenDestroyedL()
*
*******************************************************************/
TInt CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ResetWhenDestroyedL(  )
    {
	SetupL();
    iMemoryEntryAddItemDlg->ResetWhenDestroyed(&iMemoryEntryAddItemDlg);
    
    delete iMemoryEntryAddItemDlg;

    ASSERT( iMemoryEntryAddItemDlg == NULL );
	Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	MT_CPbkMemoryEntryAddItemDlg_ExecuteLDL()
*
*******************************************************************/
TInt CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLDL(  )
    {
	SetupL();
	CPbkFieldInfo*  result = NULL;
    CPbkFieldInfo*  fields = NULL;
    TBool           isSame = EFalse;
    
    fields = iContactEngine->FieldsInfo().Find(EPbkFieldIdEmailAddress);
	ASSERT( fields );
    iFieldInfos->AppendL(fields);

	HandleKeyEventsL();

    iMemoryEntryAddItemDlg->ResetWhenDestroyed(&iMemoryEntryAddItemDlg);
    
    result = iMemoryEntryAddItemDlg-> ExecuteLD( *iFieldInfos );
	ASSERT( result  );

    CPbkFieldInfo* fieldType(iFieldInfos->At(0));
    isSame = result->IsSame( *fieldType );

	ASSERT( isSame  );
		Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	MT_CPbkMemoryEntryAddItemDlg_ExecuteLD1L()
*
*******************************************************************/
TInt CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLD1L(  )
    {
	SetupL();
	CPbkFieldInfo*  result = NULL;
    CPbkFieldInfo*  fields = NULL;
    TBool           isSame = EFalse;
    
    fields = iContactEngine->FieldsInfo().Find(EPbkFieldIdLastName);
	ASSERT( fields  );
    iFieldInfos->AppendL(fields);

	HandleKeyEventsL();

    iMemoryEntryAddItemDlg->ResetWhenDestroyed(&iMemoryEntryAddItemDlg);
    
    result = iMemoryEntryAddItemDlg->ExecuteLD( *iFieldInfos, R_AVKON_SOFTKEYS_YES_NO );
	ASSERT( result );
	
    CPbkFieldInfo* fieldType(iFieldInfos->At(0));
    isSame = result->IsSame( *fieldType );

	ASSERT( isSame );
		Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	MT_CPbkMemoryEntryAddItemDlg_ExecuteLD2L()
*
*******************************************************************/
TInt CMTPbkMemoryEntryAddItemDlg::MT_CPbkMemoryEntryAddItemDlg_ExecuteLD2L(  )
    {
    SetupL();
	CPbkFieldInfo* 	result = NULL;
	CPbkFieldInfo*  fields = NULL;
    HBufC*			titleBuf;
    TBool           isSame = EFalse;

    titleBuf = CCoeEnv::Static()->AllocReadResourceL(R_QTN_PHOB_QTL_PB_INFO);

    fields = iContactEngine->FieldsInfo().Find(EPbkFieldIdFirstName);
	ASSERT( fields  );
    iFieldInfos->AppendL(fields);

	HandleKeyEventsL();

    iMemoryEntryAddItemDlg->ResetWhenDestroyed(&iMemoryEntryAddItemDlg);
    
    result = iMemoryEntryAddItemDlg->ExecuteLD( 
    						*iFieldInfos, R_AVKON_SOFTKEYS_YES_NO, *titleBuf );
	ASSERT( result  );
	
    CPbkFieldInfo* fieldType(iFieldInfos->At(0));
    isSame = result->IsSame( *fieldType );

	ASSERT( isSame    );
		Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	HandleKeyEventsL()
*
*******************************************************************/
void CMTPbkMemoryEntryAddItemDlg::HandleKeyEventsL(  )
	{

	TKeyCode testedSoftKeyCode = leftSoftKeyCode;
		
	iKeyEventSimulation->ClearAllKeyEventsL();
	iKeyEventSimulation->AddKeyEventCodeL( testedSoftKeyCode );	
	iKeyEventSimulation->CallBackStartL();

	}

//  [End of File] - Do not remove
