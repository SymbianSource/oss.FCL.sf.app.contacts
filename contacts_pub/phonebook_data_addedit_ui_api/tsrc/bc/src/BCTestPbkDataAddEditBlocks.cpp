/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Test Case Blocks
*
*/





// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "BCTestPbkDataAddEdit.h"

#include <CPbkContactEditorDlg.h>
#include <CPbkDataSaveAppUi.h>
#include <eikmenup.h> 
#include <coecntrl.h>

//#include<pbkview.rsg>
#include <stringloader.h>
#include <eikbtgpc.h>
#include <eikrted.h>
#include <aknappui.h>
#include <avkon.rsg>
#include <avkon.hrh>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT(KTmpResource, "z:\\resource\\pbkview.rsc");

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
// CBCTestPbkDataAddEdit::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCTestPbkDataAddEdit::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCTestPbkDataAddEdit::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCTestPbkDataAddEdit::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CBCTestPbkDataAddEdit::ExampleL ),
        ENTRY( "TestCmnEdtrDlgNew", CBCTestPbkDataAddEdit::TestCmnEdtrDlgNewL ),
        ENTRY( "TestCmnEdtrResetWhenDestroyed", CBCTestPbkDataAddEdit::TestCmnEdtrResetWhenDestroyedL ),
        ENTRY( "TestCmnEdtrHideExitCommand", CBCTestPbkDataAddEdit::TestCmnEdtrHideExitCommandL ),
        ENTRY( "TestCmnEdtrSetHelpContext", CBCTestPbkDataAddEdit::TestCmnEdtrSetHelpContextL ),
        ENTRY( "TestCmnEdtrSetExitCommandId", CBCTestPbkDataAddEdit::TestCmnEdtrSetExitCommandIdL ),
        ENTRY( "TestCmnEdtrSetExitCallback", CBCTestPbkDataAddEdit::TestCmnEdtrSetExitCallbackL ),
        ENTRY( "TestCmnEdtrExecuteLD", CBCTestPbkDataAddEdit::TestCmnEdtrExecuteLDL ),
        ENTRY( "TestDataSaveNew", CBCTestPbkDataAddEdit::TestDataSaveNewL ),
        ENTRY( "TestDataSaveHandleCommand", CBCTestPbkDataAddEdit::TestDataSaveHandleCommandL ),
        ENTRY( "TestDataSaveAddMenuItems", CBCTestPbkDataAddEdit::TestDataSaveAddMenuItemsL ),
        ENTRY( "TestDataSaveDynInitMenuPane", CBCTestPbkDataAddEdit::TestDataSaveDynInitMenuPaneL ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCTestPbkDataAddEdit::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCTestPbkDataAddEdit::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KBCTestPbkDataAddEdit, "BCTestPbkDataAddEdit" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KBCTestPbkDataAddEdit, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KBCTestPbkDataAddEdit, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


TInt CBCTestPbkDataAddEdit::TestCmnEdtrDlgNewL( CStifItemParser& /*aItem*/ )
    {

    
	//TBuf <64> rscFileName;
	//rscFileName.Copy(KTmpResource);
	
	//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );
    CleanupStack::PopAndDestroy( contactEditorDlg );
    
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);
    
    return KErrNone;


    }
  
 TInt CBCTestPbkDataAddEdit::TestCmnEdtrResetWhenDestroyedL( CStifItemParser& /*aItem*/ )
    {

       
		//TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );
    
    contactEditorDlg->ResetWhenDestroyed( &contactEditorDlg );
    CleanupStack::PopAndDestroy( contactEditorDlg );
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);
    return KErrNone;

    }
  
  TInt CBCTestPbkDataAddEdit::TestCmnEdtrHideExitCommandL( CStifItemParser& /*aItem*/ )
    {
   
		//TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );
    
    contactEditorDlg->HideExitCommand();
    //dlg does not have public api to get command status, cant assert hiding
    CleanupStack::PopAndDestroy( contactEditorDlg );
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);
    
    return KErrNone;
    }
   
  TInt CBCTestPbkDataAddEdit::TestCmnEdtrSetHelpContextL( CStifItemParser& /*aItem*/ )
    {
   
		//TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );
    
    const TUid KContextMajor = TUid::Uid( 1 ); 
    TCoeHelpContext* helpContext = 
        new ( ELeave ) TCoeHelpContext( KContextMajor, KContextName );
        
    TRAPD(err, contactEditorDlg->SetHelpContext( *helpContext ) );
    //GetHelpContext has a private access, cant verify helpContext
    ASSERT(err == KErrNone);
    CleanupStack::PopAndDestroy( contactEditorDlg );
   // ptrEikonEnv->DeleteResourceFile(offsetRsc);
    
    return KErrNone;
    }
    
 TInt CBCTestPbkDataAddEdit::TestCmnEdtrSetExitCommandIdL( CStifItemParser& /*aItem*/ )
    {
   
		//TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );
    contactEditorDlg->SetExitCommandId( EPbkCmdExit );
    //dlg does not have public api to get command id, cant assert it 
    CleanupStack::PopAndDestroy( contactEditorDlg );
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);
    return	KErrNone;
    }
    
TInt CBCTestPbkDataAddEdit::TestCmnEdtrSetExitCallbackL( CStifItemParser& /*aItem*/ )
    {
   		//TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, ETrue );
    CleanupStack::PushL( contactEditorDlg );  
    
    //BCTestEditorOkToExitCallback* myCallback = new ( ELeave ) BCTestEditorOkToExitCallback;    
    //CleanupStack::PushL(myCallback);
    
    ASSERT( OkToExitL( KOKToExitCommand ) );
    
    TRAPD(err, contactEditorDlg->SetExitCallback( this ) );
    //CleanupStack::PopAndDestroy( myCallback );
    
    ASSERT( err == KErrNone );    
    
    CleanupStack::PopAndDestroy( contactEditorDlg );    
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);        
    
    return KErrNone;
    		
    }
 
 TInt CBCTestPbkDataAddEdit::TestCmnEdtrExecuteLDL( CStifItemParser& /*aItem*/ )
    {

    //TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    CPbkContactEditorDlg* contactEditorDlg;
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    contactEditorDlg = CPbkContactEditorDlg::NewL
        ( *iContactEngine, *iContactItem, EFalse );
    CleanupStack::PushL( contactEditorDlg );  
    contactEditorDlg->HideExitCommand() ; 
        
    CleanupStack::Pop( contactEditorDlg );
    const TContactItemId newContactId = contactEditorDlg->ExecuteLD();
    ASSERT( !( KNullContactId == newContactId ) );
    
    //ptrEikonEnv->DeleteResourceFile(offsetRsc);
    return KErrNone;

    } 
  
TInt CBCTestPbkDataAddEdit::TestDataSaveNewL( CStifItemParser& /*aItem*/ )
    {
		CPbkDataSaveAppUi* dataSaveAppUi = CPbkDataSaveAppUi::NewL( *iContactEngine );
    CleanupStack::PushL( dataSaveAppUi );
    dataSaveAppUi->HideEditorExitCommand();
    CleanupStack::PopAndDestroy( dataSaveAppUi );
    return KErrNone;

    }  
 
TInt CBCTestPbkDataAddEdit::TestDataSaveHandleCommandL( CStifItemParser& /*aItem*/ )
    {
    TBuf <64> rscFileName;
		rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    //int offsetRsc = iPbkViewResourceFile.AddResourceFileL(rscFileName);
    
		CPbkDataSaveAppUi* dataSaveAppUi = CPbkDataSaveAppUi::NewL( *iContactEngine );
    CleanupStack::PushL( dataSaveAppUi );
    
    const CPbkFieldsInfo& fieldsInfo = iContactEngine->FieldsInfo();
    CArrayPtrFlat<CPbkFieldInfo>* fields = new( ELeave ) CArrayPtrFlat<CPbkFieldInfo>( 1 );
    CleanupStack::PushL( fields );
    
    for (TInt i = 0; i < fieldsInfo.Count(); ++i)
        {
        CPbkFieldInfo* fieldInfo = fieldsInfo[ i ];
        if ( ( fieldInfo->FieldStorageType() == KStorageTypeText )
            && ( fieldInfo->IsPhoneNumberField() ) )
            {
            fields->AppendL( fieldInfo );
            }
        }
    
    _LIT( KPhoneNumber, "+358401234567" );
    TInt commandId = TPbkDataSaveCommandId( EPbkCmdDataSaveAddToExisting );
    TRAPD( err, dataSaveAppUi->HandleCommandL( commandId, *fields, KPhoneNumber ) );
    ASSERT(err == KErrNone);
    
    CleanupStack::PopAndDestroy( fields );
    CleanupStack::PopAndDestroy( dataSaveAppUi );
     //ptrEikonEnv->DeleteResourceFile(offsetRsc);   
    return KErrNone;

    }  
 
TInt CBCTestPbkDataAddEdit::TestDataSaveAddMenuItemsL( CStifItemParser& /*aItem*/ )
    {
    //TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);
    
		CPbkDataSaveAppUi* dataSaveAppUi = CPbkDataSaveAppUi::NewL( *iContactEngine );
    CleanupStack::PushL( dataSaveAppUi );
    
    
    CEikMenuPane* menuPane =  new ( ELeave ) CEikMenuPane(this);//This class is a Observer implementation
  	CleanupStack::PushL( menuPane );
    CEikMenuPaneItem* menuItem = new ( ELeave ) CEikMenuPaneItem();
    CleanupStack::PushL( menuItem );
    
    menuItem->iData.iCommandId = TPbkDataSaveCommandId( EPbkCmdDataSaveCreateNew );
    menuItem->iData.iCascadeId = 0;
    menuItem->iData.iFlags = 0;
    
	menuPane->AddMenuItemL( menuItem->iData );
	
	TInt commandId = TPbkDataSaveCommandId( EPbkCmdDataSaveCreateNew );
    dataSaveAppUi->AddMenuItemsL( menuPane, commandId );
    
    CleanupStack::PopAndDestroy( menuItem );
    CleanupStack::PopAndDestroy( menuPane );
    CleanupStack::PopAndDestroy( dataSaveAppUi );
    //ptrEikonEnv->DeleteResourceFile(offsetRsc); 
    return KErrNone;

    }  
    
TInt CBCTestPbkDataAddEdit::TestDataSaveDynInitMenuPaneL( CStifItemParser& /*aItem*/ )
    {
    //TBuf <64> rscFileName;
		//rscFileName.Copy(KTmpResource);
	
		//CEikonEnv *ptrEikonEnv = CEikonEnv::Static();
    //int offsetRsc = ptrEikonEnv->AddResourceFileL(rscFileName);	
    
		CPbkDataSaveAppUi* dataSaveAppUi = CPbkDataSaveAppUi::NewL( *iContactEngine );
    CleanupStack::PushL( dataSaveAppUi );
    
    
    CEikMenuPane* menuPane =  new ( ELeave ) CEikMenuPane(this);//This class is a Observer implementation
  	CleanupStack::PushL( menuPane );
    CEikMenuPaneItem* menuItem = new ( ELeave ) CEikMenuPaneItem();
    CleanupStack::PushL( menuItem );
    
    menuItem->iData.iCommandId = TPbkDataSaveCommandId( EPbkCmdDataSaveCreateNew );
    menuItem->iData.iCascadeId = 0;
    menuItem->iData.iFlags = 0;
    
		menuPane->AddMenuItemL( menuItem->iData );
	
		//2nd item
    CEikMenuPaneItem* menuItem2 = new ( ELeave ) CEikMenuPaneItem();
    CleanupStack::PushL( menuItem2 );
    menuItem2->iData.iCommandId = TPbkDataSaveCommandId( EPbkCmdDataSaveAddToExisting );
    menuItem2->iData.iCascadeId = 0;
    menuItem2->iData.iFlags = 0;    
	menuPane->AddMenuItemL( menuItem2->iData );	
	
	//If this resource id does not match with 
    //Phonebook data save submenu this function does nothing
	TInt resourceId = R_PBK_DATA_SAVE_SUBMENU_TESTING_ID;
	dataSaveAppUi->DynInitMenuPaneL( resourceId, menuPane );
    
    CleanupStack::Pop( menuItem2 );
    CleanupStack::PopAndDestroy( menuItem );
    CleanupStack::PopAndDestroy( menuPane );
    CleanupStack::PopAndDestroy( dataSaveAppUi );
    //ptrEikonEnv->DeleteResourceFile(offsetRsc); 
    return KErrNone;

    }  
   
//Observer interface dummy implementations

void CBCTestPbkDataAddEdit::HandleAttemptDimmedSelectionL(
    TInt /*aCommandId*/)
    {	
    }
TBool CBCTestPbkDataAddEdit::CheckHotKeyNotDimmedL(
    TInt /*aCommandId*/)
    {	
    return ETrue;
    }
void CBCTestPbkDataAddEdit::RestoreMenuL(
    CCoeControl* /**aMenuControl*/, TInt /*aResourceId*/, TMenuType /*aType*/)
    {
    }
void CBCTestPbkDataAddEdit::DynInitMenuPaneL(
    TInt /*aResourceId*/, CEikMenuPane* /**aMenuPane*/)
    {
    }
void CBCTestPbkDataAddEdit::DynInitMenuBarL(
    TInt /*aResourceId*/, CEikMenuBar* /**aMenuBar*/)
    {
    }
void CBCTestPbkDataAddEdit::HandleSideBarMenuL(
    TInt /*aResourceId*/, const TPoint& /*&aPos*/, TInt /*aModifiers*/, 
    const CEikHotKeyTable* /**aTable*/)
    {
    }
void CBCTestPbkDataAddEdit::OfferKeyToAppL(
    const TKeyEvent& /*&aKeyEvent*/, TEventCode /*aType*/)
    {
    }
void CBCTestPbkDataAddEdit::SetEmphasis(
    CCoeControl* /**aMenuControl*/, TBool /*aEmphasis*/)
    {
    }
CCoeControl* CBCTestPbkDataAddEdit::CreateCustomCommandControlL(
    TInt /*aControlType*/)
    {
    return NULL;
    }

//From Observer base class 
void CBCTestPbkDataAddEdit::ProcessCommandL(TInt /*aCommandId*/)
    {	
    }

TBool CBCTestPbkDataAddEdit::OkToExitL(
    TInt /*aCommandId*/)
    {	
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CBCTestPbkDataAddEdit::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCTestPbkDataAddEdit::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
