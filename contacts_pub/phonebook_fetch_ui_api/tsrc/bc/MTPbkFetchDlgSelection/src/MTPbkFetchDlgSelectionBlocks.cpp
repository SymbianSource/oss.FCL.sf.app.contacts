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
#include "MTPbkFetchDlgSelection.h"
#include <CPbkMultipleEntryFetchDlg.h>
#include <Coemain.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>
#include <MPbkFetchDlgSelection.h>

#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"

const TInt KMaxAmount = 2;

const	TKeyCode    KEnterCode(EKeyDevice3);
const	TKeyCode    KDownCode(EKeyDownArrow);
const	TKeyCode    KLeftSoftKeyCode(EKeyDevice0); // Select 


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
/**
 * Phonebook test fetch selection accepter.
 */
class CFetchSelectionAccepter : 
	public CBase,
	public MPbkFetchDlgSelection
    {
    public:
        static CFetchSelectionAccepter* NewLC(TInt aMaxSelectCount);
        
  	public:	// From MPbkFetchDlgSelection
  	
       	TBool ContactSelectionAcceptedL
       		(TContactItemId aItemId, 
        	 TInt aCurrentSelectedCount) const;
	

    protected:
        void ConstructL(TInt aMaxSelectCount);
        
    private:	// data
    	TInt iMaxSelectCount;

    };


// -----------------------------------------------------------------------------
// CMTPbkFetchDlgSelection::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkFetchDlgSelection::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkFetchDlgSelection::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkFetchDlgSelection::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Testwith2selectedcontacts", CMTPbkFetchDlgSelection::RunExecuteL ),
		ENTRY( "Testwith3selectedcontacts", CMTPbkFetchDlgSelection::RunExecute1L ),        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/*******************************************************************
*
*	SetupDefaultContactItem1L ()
*
*******************************************************************/
void CMTPbkFetchDlgSelection::SetupDefaultContactItem1L()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );

    iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->DuplicateContactL(iContactItem->Id());
    iContactEngine->DuplicateContactL(iContactItem->Id());

	//choose 2 contacts
	SendKeyEventsL( 1 );	

    }

/*******************************************************************
*
*	SetupDefaultContactItem2L ()
*
*******************************************************************/
void CMTPbkFetchDlgSelection::SetupDefaultContactItem2L()
	{    
	iContactEngine = CPbkContactEngine::NewL(KTestDBName,ETrue);

    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );

    iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->DuplicateContactL(iContactItem->Id());
    iContactEngine->DuplicateContactL(iContactItem->Id());

	//choose 3 contacts
	SendKeyEventsL( 2 );	

    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkFetchDlgSelection::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;

	delete iContactEngine;
	iContactEngine = NULL;
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkFetchDlgSelection::RunExecuteL(  )
    {
    SetupDefaultContactItem1L();
    CContactViewBase& allContactsView = iContactEngine->AllContactsView();

    CFetchSelectionAccepter* accepter = CFetchSelectionAccepter::NewLC(KMaxAmount);

    CPbkMultipleEntryFetchDlg::TParams params;
    params.iContactView = &allContactsView;
    params.iFetchSelection = accepter;

    iMultiEntryFetchDlg = CPbkMultipleEntryFetchDlg::NewL(params, *iContactEngine);

    TInt result = iMultiEntryFetchDlg->ExecuteLD();
    iMultiEntryFetchDlg = NULL;

    CleanupStack::PopAndDestroy(accepter);
    
    ASSERT ( (params.iMarkedEntries->Count()) <= KMaxAmount);
    
    ASSERT( result );
    Teardown();
    return KErrNone;
    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkFetchDlgSelection::RunExecute1L(  )
    {
    SetupDefaultContactItem2L();
    CContactViewBase& allContactsView = iContactEngine->AllContactsView();

    CFetchSelectionAccepter* accepter = CFetchSelectionAccepter::NewLC(KMaxAmount);

    CPbkMultipleEntryFetchDlg::TParams params;
    params.iContactView = &allContactsView;
    params.iFetchSelection = accepter;

    iMultiEntryFetchDlg = CPbkMultipleEntryFetchDlg::NewL(params, *iContactEngine);

    TInt result = iMultiEntryFetchDlg->ExecuteLD();
    iMultiEntryFetchDlg = NULL;

    CleanupStack::PopAndDestroy(accepter);
    
    ASSERT ( (params.iMarkedEntries->Count()) <= KMaxAmount);
    
    ASSERT( result );
    Teardown();
    return KErrNone;    
    }


/*******************************************************************
*
*	SendKeyEventsL ()
*
*******************************************************************/
void CMTPbkFetchDlgSelection::SendKeyEventsL( const TInt aSelect )
	{
	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( KEnterCode );	
	
	for ( TInt i = 0; i < aSelect; i++ )
	    {
    	iSimulateKeyEvents->AddKeyEventCodeL( KDownCode );	
    	iSimulateKeyEvents->AddKeyEventCodeL( KEnterCode );	
	    }
	iSimulateKeyEvents->AddKeyEventCodeL( KLeftSoftKeyCode );	
	iSimulateKeyEvents->CallBackStartL();
	}

/*******************************************************************
*
*	Member functions
*
*******************************************************************/
CFetchSelectionAccepter* CFetchSelectionAccepter::NewLC
		(TInt aMaxSelectCount)
    {
    CFetchSelectionAccepter* self = new(ELeave) CFetchSelectionAccepter;
    CleanupStack::PushL(self);
    self->ConstructL(aMaxSelectCount);
    return self;
    }

void CFetchSelectionAccepter::ConstructL(TInt aMaxSelectCount)
    {
    iMaxSelectCount = aMaxSelectCount;
    }

TBool CFetchSelectionAccepter::ContactSelectionAcceptedL
        (TContactItemId /*aItemId*/, 
         TInt aCurrentSelectedCount) const
	{
	if (aCurrentSelectedCount < iMaxSelectCount)
		{
		return ETrue;	
		}
	else
		{
		return EFalse;	
		}
	}

//  [End of File] - Do not remove
