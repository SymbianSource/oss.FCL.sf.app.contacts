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
#include "BCTestSpeedDial.h"

#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <f32file.h>

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
// CBCTestSpeedDial::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCTestSpeedDial::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCTestSpeedDial::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCTestSpeedDial::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "ExampleForTestSpdialL", CBCTestSpeedDial::ExampleForTestSpdialL ),
        ENTRY( "TestDialogsNewL", CBCTestSpeedDial::TestDialogsNewLL ),
        ENTRY( "TestShowRemove", CBCTestSpeedDial::TestShowRemoveL ),
        ENTRY( "TestDialogsCancel", CBCTestSpeedDial::TestDialogsCancelL )
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCTestSpeedDial::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCTestSpeedDial::ExampleForTestSpdialL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KBCTestSpeedDial, "BCTestSpeedDial" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KBCTestSpeedDial, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KBCTestSpeedDial, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


// test case of Speeddial dialogs.
TInt CBCTestSpeedDial::TestDialogsNewLL( CStifItemParser& aItem )
    {
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL(uriArray);
    CVPbkContactManager* contactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(2);
    CSpdiaDialogs* dialog = NULL;
    TRAPD( err, dialog = CSpdiaDialogs::NewL( *contactManager ) );
    
    delete dialog;
    dialog = NULL;
    delete contactManager;
    contactManager = NULL;
    
    return err;
    }
    
TInt CBCTestSpeedDial::TestShowRemoveL( CStifItemParser& aItem )
    {
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL(uriArray);
    CVPbkContactManager* contactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(2);
    CSpdiaDialogs* dialog = CSpdiaDialogs::NewL( *contactManager );
    TInt result = dialog->ShowRemove( 5 );
    
    delete dialog;
    dialog = NULL;
    delete contactManager;
    contactManager = NULL;
    return KErrNone;
    }    
    
TInt CBCTestSpeedDial::TestDialogsCancelL( CStifItemParser& aItem )
    {
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL(uriArray);
    CVPbkContactManager* contactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(2);
    CSpdiaDialogs* dialog = CSpdiaDialogs::NewL( *contactManager );
    TInt result = dialog->Cancel();
    
    delete dialog;
    dialog = NULL;
    delete contactManager;
    contactManager = NULL;
    
    if ( result >= 0 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }    
    
TInt CBCTestSpeedDial::DeleteSelf( CStifItemParser& aItem )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBCTestSpeedDial::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCTestSpeedDial::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
