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
#include "MTPbkFetchCallbacks.h"
#include <CPbkSingleEntryFetchDlg.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>
#include <MPbkFetchCallBacks.h>
#include <e32base.h>
#include <e32def.h>
#include <avkon.rsg>    //for use of R_AVKON_SOFTKEYS_CALL_CANCEL
#include <cpbkphonenumberselect.h>

#include "CSimulateKeyEvents.h"
#include "T_AddressAndNumberSelect.h"

//Helper class

class CFetchCallback
        : public CBase,public MPbkFetchKeyCallback,public MPbkFetchDlgAccept
    {
    public:
        void SetEngine(CPbkContactEngine& aEngine) { iEngine = &aEngine; }

    private: // from MPbkFetchKeyCallback
        TResult PbkFetchKeyCallbackL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private: // from MPbkFetchDlgAccept
        TPbkFetchAccepted PbkFetchAcceptedL
            (TContactItemId aId, CContactIdArray* aMarkedEntries);

		void SendKeyEventsL( );

    private:
        // Own: Phonebook engine
        CPbkContactEngine* iEngine;
		// Simulates key events
		CSimulateKeyEvents*			iSimulateKeyEvents;
    };


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
// CMTPbkFetchCallbacks::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkFetchCallbacks::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkFetchCallbacks::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkFetchCallbacks::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "ExecuteLD_test", CMTPbkFetchCallbacks::RunExecuteL ),
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
void CMTPbkFetchCallbacks::SetupDefaultContactItemL()
	{
    iContactItem = iContactEngine->CreateEmptyContactL();
    iAddressAndNumberSelect->FillContactFieldsL( *iContactItem );
    iContactEngine->AddNewContactL(*iContactItem);
    }

/*******************************************************************
*
*	Teardown ()
*
*******************************************************************/
void CMTPbkFetchCallbacks::Teardown(  )
    {
	delete iContactItem;
	iContactItem = NULL;

    }

/*******************************************************************
*
*	RunExecuteL ()
*
*******************************************************************/
TInt CMTPbkFetchCallbacks::RunExecuteL(  )
    {
    SetupDefaultContactItemL();
    TInt                result = ETrue;

    CFetchCallback* fetchCallback = new(ELeave) CFetchCallback;
    CleanupStack::PushL(fetchCallback);

    CPbkSingleEntryFetchDlg::TParams params;
        // Use the engine's default contact array
    params.iContactView = &iContactEngine->AllContactsView();
    // this sets the key event callback
    // the key events can be checked at ::PbkFetchKeyCallbackL
    params.iFetchKeyCallback = fetchCallback;
    params.iFetchDlgAccept = fetchCallback;
    params.iCbaId = R_AVKON_SOFTKEYS_CALL_CANCEL;

    iSingleEntryFetchDlg = CPbkSingleEntryFetchDlg::NewL(params);
    iSingleEntryFetchDlg->ResetWhenDestroyed(&iSingleEntryFetchDlg);
    fetchCallback->SetEngine(*iContactEngine);

	SendKeyEventsL( );

    result = iSingleEntryFetchDlg->ExecuteLD();

    CleanupStack::PopAndDestroy(fetchCallback);

   	ASSERT( iSingleEntryFetchDlg == NULL);

   	ASSERT( result );

   	ASSERT( params.iSelectedEntry == iContactItem->Id());
	Teardown();
	return KErrNone;
    }


/*******************************************************************
*
*	SendKeyEventsL ()
*
*******************************************************************/
void CMTPbkFetchCallbacks::SendKeyEventsL( )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();

	}

/*******************************************************************
*
*	CFetchCallback -class methods
*
*******************************************************************/
MPbkFetchKeyCallback::TResult CFetchCallback::PbkFetchKeyCallbackL
        (const TKeyEvent& aKeyEvent,
        TEventCode /*aType*/)
    {
   	 ASSERT ( aKeyEvent.iCode == EKeyWasNotConsumed );

    return MPbkFetchKeyCallback::EKeyWasNotConsumed;
    }

MPbkFetchDlgAccept::TPbkFetchAccepted CFetchCallback::PbkFetchAcceptedL
        (TContactItemId aId, CContactIdArray* /*aMarkedEntries*/)
    {
	iSimulateKeyEvents = CSimulateKeyEvents::NewL();

    CPbkContactItem* item = iEngine->ReadContactLC(aId);

    // open phone number select dialog, which handles default numbers etc.
    CPbkPhoneNumberSelect* phoneNumberSelect = new(ELeave) CPbkPhoneNumberSelect;
    CPbkPhoneNumberSelect::TParams params(*item);

    SendKeyEventsL();

    TBool executed = phoneNumberSelect->ExecuteLD(params);

    TPbkFetchAccepted ret = KFetchNo;
    const TPbkContactItemField* field=params.SelectedField();
    if (executed && field && field->Text() != KNullDesC)
        {
        ret = KFetchYes;
        }
    CleanupStack::PopAndDestroy(item);

    delete iSimulateKeyEvents;
    iSimulateKeyEvents = NULL;

    return ret;
    }

/*******************************************************************
*
*	SendKeyEventsL ()
*
*******************************************************************/
void CFetchCallback::SendKeyEventsL( )
	{

	iSimulateKeyEvents->ClearAllKeyEventsL();
	iSimulateKeyEvents->AddKeyEventCodeL( leftSoftKeyCode );
	iSimulateKeyEvents->CallBackStartL();

	}

//  [End of File] - Do not remove
