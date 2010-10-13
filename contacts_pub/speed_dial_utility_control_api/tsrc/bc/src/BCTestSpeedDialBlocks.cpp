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

#include <spdiacontrol.h>
#include <cpbkcontactengine.h> 
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
        ENTRY( "TestNewL", CBCTestSpeedDial::TestNewLL ),
        ENTRY( "TestNewL2", CBCTestSpeedDial::TestNewL2L ),
        ENTRY( "TestAssignDialNumberL", CBCTestSpeedDial::TestAssignDialNumberLL ),
        ENTRY( "TestExecuteLD", CBCTestSpeedDial::TestExecuteLDL ),
        ENTRY( "TestDialNumberL", CBCTestSpeedDial::TestDialNumberLL ),
        ENTRY( "TestVoiceMailL", CBCTestSpeedDial::TestVoiceMailLL ),
        ENTRY( "TestPbkEngine", CBCTestSpeedDial::TestPbkEngineL ),
        ENTRY( "TestIndex", CBCTestSpeedDial::TestIndexL ),
        ENTRY( "TestNumber", CBCTestSpeedDial::TestNumberL ),
        ENTRY( "TestContactId", CBCTestSpeedDial::TestContactIdL ),
        ENTRY( "TestPhoneDialNumber", CBCTestSpeedDial::TestPhoneDialNumberL ),
        ENTRY( "TestPhoneNumber", CBCTestSpeedDial::TestPhoneNumberL ),
        ENTRY( "TestThumbIndex", CBCTestSpeedDial::TestThumbIndexL ),
        ENTRY( "TestIconIndex", CBCTestSpeedDial::TestIconIndexL ),
        ENTRY( "TestNumberType", CBCTestSpeedDial::TestNumberTypeL ),
        ENTRY( "TestIconArray", CBCTestSpeedDial::TestIconArrayL ),
        ENTRY( "TestCheckingIfPopUpNeeded", CBCTestSpeedDial::TestCheckingIfPopUpNeededL ),
        ENTRY( "TestRemoveDialIndexL", CBCTestSpeedDial::TestRemoveDialIndexLL ),
        ENTRY( "TestVMBoxPosition", CBCTestSpeedDial::TestVMBoxPositionL ),
        ENTRY( "TestSpdIconIndex", CBCTestSpeedDial::TestSpdIconIndexL ),
        ENTRY( "TestDeleteIconArray", CBCTestSpeedDial::TestDeleteIconArrayL ),
        ENTRY( "TestReloadIconArray", CBCTestSpeedDial::TestReloadIconArrayL ),
        ENTRY( "TestVoiceMailType", CBCTestSpeedDial::TestVoiceMailTypeL ),
        ENTRY( "TestGetSpdCtrlLastError", CBCTestSpeedDial::TestGetSpdCtrlLastErrorL )
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


TInt CBCTestSpeedDial::TestNewLL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KBCConnSettingsUITest, "BCTestSpeedDial" );
    _LIT( KNew, "In NewL" );
    TestModuleIf().Printf( 0, KBCConnSettingsUITest, KNew );

    // Print to log file
    iLog->Log( KNew );

    TInt err = KErrNone;
    CSpdiaControl* spd = NULL;
    TRAP( err, spd =  CSpdiaControl::NewL() );
    delete spd;
    
    return err;
    }

TInt CBCTestSpeedDial::TestNewL2L( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );    
    
    TInt err = KErrNone;
    CSpdiaControl* spd = NULL;
    TRAP( err, spd =  CSpdiaControl::NewL( *pbk ) );     
    delete spd;
    
    delete pbk;
    pbk = 0;
  
    return err;
    }

TInt CBCTestSpeedDial::TestAssignDialNumberLL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL();
    
    TRAPD( err, spd->AssignDialNumberL( 5 ) );

    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
        
    return err;
    }

TInt CBCTestSpeedDial::TestExecuteLDL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TRAPD( err, spd->ExecuteLD( 1, 1 ) );
            
    spd = 0;
    delete pbk;
    pbk = 0;            
    
    return err;
    }

TInt CBCTestSpeedDial::TestDialNumberLL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TBuf<100> voice;
    TRAPD( err,spd->DialNumberL( 5, voice ) );
    
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;            
    return err;
    }

TInt CBCTestSpeedDial::TestVoiceMailLL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL();
    TBuf<100> voice;
    
    TRAPD( err, spd->VoiceMailL( voice ) );

    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;            
    
    return err;
    }

TInt CBCTestSpeedDial::TestPbkEngineL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    CPbkContactEngine* ptest = spd->PbkEngine();
    
    TInt reValue = KErrNone;
    
    if ( !ptest )
        {
        reValue =  KErrAbort;
        }
    
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;            
    return reValue;
    }

TInt CBCTestSpeedDial::TestIndexL( CStifItemParser& aItem )
    {
    CSpdiaControl* spd = CSpdiaControl::NewL();
    TInt result = spd->Index( 5 );

    delete spd;
    spd = 0;
    if ( result > -2 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBCTestSpeedDial::TestNumberL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    
    TInt result = spd->Number( 5 );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if ( result > -2 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBCTestSpeedDial::TestContactIdL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    
    TContactItemId result;
    result = spd->ContactId( 4 );    
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if ( result > -2 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBCTestSpeedDial::TestPhoneDialNumberL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TDesC telnum = spd->PhoneDialNumber( 5 );    
    
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if(&telnum)
        {                
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }            

    }

TInt CBCTestSpeedDial::TestPhoneNumberL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TDesC telnum = spd->PhoneNumber( 5 );    
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if(&telnum)
        {                
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBCTestSpeedDial::TestThumbIndexL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt index = spd->ThumbIndex( 5 );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if( index > -2 && index < 9 )
        {
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }
    }
    
TInt CBCTestSpeedDial::TestIconIndexL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt index = spd->IconIndex( 5 );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    if( index > -2 && index < 9 )
        {
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }
    }
    
TInt CBCTestSpeedDial::TestNumberTypeL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt numberType = spd->NumberType( 5 );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    return KErrNone;
    }
    
TInt CBCTestSpeedDial::TestIconArrayL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    CArrayPtr<CGulIcon>* result = NULL;
    result = spd->IconArray();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    STIF_ASSERT_NOT_NULL( result );
    return KErrNone;
    }
    
TInt CBCTestSpeedDial::TestCheckingIfPopUpNeededL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL( &iSession );
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TBool result = spd->CheckingIfPopUpNeeded();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    if( result >= 0 )
        {
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBCTestSpeedDial::TestRemoveDialIndexLL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TRAPD( err, spd->RemoveDialIndexL( 5, 0 ) );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    return err;
    }    
    
TInt CBCTestSpeedDial::TestVMBoxPositionL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt result = spd->VMBoxPosition();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    if ( result >= 0 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }    
    
TInt CBCTestSpeedDial::TestSpdIconIndexL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt result = spd->SpdIconIndex( 5 );
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    if( result > -2 && result < 9 )
        {
        return KErrNone;    
        }
    else
        {
        return KErrNotFound;
        }
    }    
    
TInt CBCTestSpeedDial::TestDeleteIconArrayL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    spd->DeleteIconArray();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    return KErrNone;
    }    
    
TInt CBCTestSpeedDial::TestReloadIconArrayL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    spd->ReloadIconArray();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    return KErrNone;
    }    
    
TInt CBCTestSpeedDial::TestVoiceMailTypeL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt result = spd->VoiceMailType();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    if ( result >= 0 )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }    
    
TInt CBCTestSpeedDial::TestGetSpdCtrlLastErrorL( CStifItemParser& aItem )
    {
    CPbkContactEngine* pbk = CPbkContactEngine::NewL(&iSession);
    CSpdiaControl* spd = CSpdiaControl::NewL( *pbk );
    TInt result = spd->GetSpdCtrlLastError();
    delete spd;
    spd = 0;
    delete pbk;
    pbk = 0;
    
    if ( result <= 0 )
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
    if ( iControl )
        {
        delete iControl;
        iControl = NULL;
        }
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
