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
#include "MTPbkAddressSelect.h"
#include "CAddressSelect.h"
#include <eikenv.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
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
#ifdef __WINSCW__
    _LIT(KTmpResource, "Z:\\resource\\PbkAddressSelect.rsc");
#else
    _LIT(KTmpResource, "C:\\resource\\PbkAddressSelect.rsc");
#endif

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
// CMTPbkAddressSelect::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkAddressSelect::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkAddressSelect::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkAddressSelect::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "Size_of_Class_not_less_than_in_v3.0", CMTPbkAddressSelect::RunTest_ClassSizeL ),
        ENTRY( "SizeofClass", CMTPbkAddressSelect::RunTest_ClassSizeL ),
        ENTRY( "Testwithcontact", CMTPbkAddressSelect::RunExecuteWithContactNameL ),
        ENTRY( "Test_with_one_e-mail_address", CMTPbkAddressSelect::RunExecuteWithOneEmailL ),
        ENTRY( "Test_focus_on_phone_number", CMTPbkAddressSelect::RunExecuteWithMultiEmailFocusNullL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMTPbkAddressSelect::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMTPbkAddressSelect::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;
	
	delete iAddressAndNumberSelect;
	iAddressAndNumberSelect = NULL;
	delete iKeyEventSimulator;
	iKeyEventSimulator = NULL;
    }

/*******************************************************************
*
*	RunTest_ClassSizeL()
*
*******************************************************************/
TInt CMTPbkAddressSelect::RunTest_ClassSizeL()
    {
    SetupContactItemWithNameL();
    static const TInt sizeof_class_CPbkAddressSelect_30 = 24;

    RDebug::Printf("Class: sizeof(class CPbkAddressSelect)=%d\n", 
                sizeof(class CPbkAddressSelect));

    ASSERT( sizeof(class CPbkAddressSelect) >= 
                sizeof_class_CPbkAddressSelect_30 );
	Teardown();
	return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteWithContactName()
*
*******************************************************************/
TInt CMTPbkAddressSelect::RunExecuteWithContactNameL()
	{
	SetupContactItemWithNameL();
	TBool 					callDefaultDirectly = EFalse,
							ret = ETrue;
	// Focus to first name field
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	SetContactReturnValue ( EFalse );

	//focus on contacts first name
	focusedField = iContactItem->FindField( EPbkFieldIdFirstName, search);
	ASSERT( focusedField );
	//EUNIT_ASSERT_DESC( focusedField, "Contact's first name wasn't found for focus" );

	ret = RunExecuteL(*iContactItem,focusedField,callDefaultDirectly );
	ASSERT(!ret);
	//EUNIT_ASSERT_DESC( ret == EFalse, "Email address wasn't found for existing contact" );

	//EUNIT_ASSERT_DESC( iIsSame, "Contact used in ExecuteLD wans't same as parameter contact");
	//EUNIT_ASSERT_DESC( iSelectedField, "Focused field was different than selected field");
	ASSERT(iIsSame);
	ASSERT(iSelectedField);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithOneEmail()
*
*******************************************************************/
TInt CMTPbkAddressSelect::RunExecuteWithOneEmailL()
	{
	SetupDefaultContactItemL();
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;
	TInt 					search = 0;
	TPbkContactItemField* 	focusedField = NULL;

	SetContactReturnValue ( EFalse );

	focusedField = iContactItem->FindField( EPbkFieldIdEmailAddress, search);
	//EUNIT_ASSERT_DESC( focusedField, "Company's name wasn't found for focus" );
	ASSERT(focusedField);
	ret = RunExecuteL( *iContactItem,focusedField,callDefaultDirectly );
	//EUNIT_ASSERT_DESC( ret, "Email address wasn't found for existing contact" );
	ASSERT(ret);
	ret = iAddressAndNumberSelect->IsFirstEmailAddressL( *iContactItem );
	//EUNIT_ASSERT_DESC( ret, "First email address wasn't found for existing contact" );
	ASSERT(ret);
	//EUNIT_ASSERT_DESC( iIsSame, "Contact used in ExecuteLD wans't same as parameter contact");
	//EUNIT_ASSERT_DESC( iSelectedField, "Focused field was different than selected field");
	ASSERT(iIsSame);
	ASSERT(iSelectedField);
	Teardown();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteWithMultiEmailFocusNullL()
*
*******************************************************************/
TInt CMTPbkAddressSelect::RunExecuteWithMultiEmailFocusNullL()
	{
	SetupContactItemWithTwoEmailFieldsL();
	TFileName path( _L("c:\\abc.txt"));
	RFile rfile;
	RFs fileSession;
	TInt err = fileSession.Connect(); 

	err = rfile.Open(fileSession, path, EFileStreamText | EFileWrite);
	if ( err != KErrNone)
			err = rfile.Create(fileSession,path, EFileStreamText|EFileWrite|EFileShareAny);
	   
	TFileText filetext;
	filetext.Set( rfile );

	TBuf <64> rscFileName;
	rscFileName.Copy(KTmpResource);
	
	CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);


	TPbkContactItemField* 	focusedField = NULL;
	TBool 					callDefaultDirectly = EFalse,
							ret = EFalse;

	const TInt findEmailAddress = iAddressAndNumberSelect->EmailAddresses() - 1; // zero-based
	iKeyEventSimulator->ClearAllKeyEventsL();
	
	for( TInt downKeyCounter = 0; downKeyCounter < findEmailAddress; downKeyCounter++ )
		{
		iKeyEventSimulator->AddKeyEventCodeL( downCode );	
		}

	TKeyCode KLSK( EKeyDevice0 );
	SendKeyEventsL( *iKeyEventSimulator, KLSK );	// LSK
	filetext.Write(_L(" rfile created "));
	ret = RunExecuteL( *iContactItem, focusedField, callDefaultDirectly );
	TBuf<32> buffer;
	buffer.Num(ret);
	filetext.Write(_L("result of RunExecuteL is : "));
	filetext.Write(buffer);
	//EUNIT_ASSERT_DESC( ret, "Email address wasn't found for existing contact" );
	ASSERT(ret);
	ret = iAddressAndNumberSelect->IsCorrectEmailAddressL( findEmailAddress, *iContactItem );
	//EUNIT_ASSERT_DESC( ret, "Email address wasn't correct for existing contact" );

	ASSERT( ret );
	ptrEikonEnv->DeleteResourceFile(offsetRsc);
	Teardown();
	rfile.Close();
	fileSession.Close();
	return KErrNone;
	}

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TBool CMTPbkAddressSelect::RunExecuteL(
				const CPbkContactItem& aItem,
                const TPbkContactItemField* aFocusedField,
                const TBool& aCallDefaultDirectly)
	{
	TBool	ret = EFalse;

    iIsSame = EFalse;
    iSelectedField = EFalse;
    
	CAddressSelect* addressSelect = new (ELeave) CAddressSelect;

	CAddressSelect::TParams params(aItem);
	params.SetFocusedField(aFocusedField);
	params.SetUseDefaultDirectly(aCallDefaultDirectly);

	ret = addressSelect->ExecuteLD(params, *this);
	//ExecuteLD deletes addressSelect

	return ret;
	}

/*********************** SETUP **************************************/

void CMTPbkAddressSelect::SetupContactItemWithNameL()
	{
	iAddressAndNumberSelect = new (ELeave) T_AddressAndNumberSelect();
	iContactItem = iContactEngine->CreateEmptyContactL();
	iAddressAndNumberSelect->FillContactNameFieldL( *iContactItem, iContactEngine );
	}

void CMTPbkAddressSelect::SetupDefaultContactItemL()
	{
	iAddressAndNumberSelect = new (ELeave) T_AddressAndNumberSelect();
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    }

void CMTPbkAddressSelect::SetupContactItemWithTwoEmailFieldsL()
	{    
	iAddressAndNumberSelect = new (ELeave) T_AddressAndNumberSelect();
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    iAddressAndNumberSelect->FillContactWithMultipleFieldsL(
			*iContactItem, iContactEngine, ETrue,
			EFalse, EFalse, EFalse);		
	
	iKeyEventSimulator = CSimulateKeyEvents::NewL();
	}

void CMTPbkAddressSelect::SendKeyEventsL
		( CSimulateKeyEvents& aSimulator, TKeyCode& aTestedSoftKeyCode )
	{
	aSimulator.AddKeyEventCodeL( aTestedSoftKeyCode );	
	aSimulator.CallBackStartL();
	}

void CMTPbkAddressSelect::SetContactReturnValue ( const TBool& aIsSame)
    {
    iIsSame = aIsSame;
    }

void CMTPbkAddressSelect::SetSelectedAndFocusedField ( const TBool& aSelectedField)
    {
    iSelectedField = aSelectedField;
    }

//  [End of File] - Do not remove
