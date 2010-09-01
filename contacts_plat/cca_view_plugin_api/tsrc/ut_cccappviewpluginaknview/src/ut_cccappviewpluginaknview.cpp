/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <StifTestModule.h>
#include <StifLogger.h>

#include "ut_cccappviewpluginaknview.h"

// ============================ MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::ut_CCCAppViewPluginAknView
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknView::ut_CCCAppViewPluginAknView( 
    CTestModuleIf& aTestModuleIf ): 
        CScriptBase( aTestModuleIf )
    {
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::ConstructL
// --------------------------------------------------------------------------
//
void ut_CCCAppViewPluginAknView::ConstructL()
    {
    iLog = CStifLogger::NewL( Kt_testiLogPath, 
                          Kt_testiLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::NewL
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknView* ut_CCCAppViewPluginAknView::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    ut_CCCAppViewPluginAknView* self = new (ELeave) ut_CCCAppViewPluginAknView( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::~ut_CCCAppViewPluginAknView
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknView::~ut_CCCAppViewPluginAknView()
    { 
    delete iLog;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::RunMethodL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "Example", ut_CCCAppViewPluginAknView::ExampleL ),
        ENTRY( "SetupL", SetupL ),
        ENTRY( "Teardown", Teardown ),
        ENTRY( "Test_DoActivateL", Test_DoActivateL ),
        ENTRY( "Test_DoDeactivate", Test_DoDeactivate ),
        ENTRY( "Test_HandleCommandL", Test_HandleCommandL )
        };
    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::SetupL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::SetupL( CStifItemParser& aItem )
    {
    iLog->Log( _L("#### Setup CCCAppViewPluginAknContainer") );

    TPtrC description;
    if ( aItem.GetNextString( description ) != KErrNone )
        return KErrNotFound;
    iLog->Log( _L("Testing: %S"), &description );
    
    iTestedClass = new (ELeave) CTestedClass();
    iCCaAppUi = new (ELeave) CCCAAppAppUi();
    iTestedClass->iAppUi = iCCaAppUi;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::Teardown
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::Teardown( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("#### Teardown CCCAppViewPluginAknContainer") );

    delete iTestedClass;
    delete iCCaAppUi;
         
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::Test_DoActivateL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::Test_DoActivateL( CStifItemParser& aItem )
    {
    iLog->Log( _L("Test_DoActivateL") );

    TInt leaveCode;
    if ( aItem.GetNextInt( leaveCode ) != KErrNone )
        return KErrNotFound;    
    iTestedClass->iLeaveCode = leaveCode;
    
    iTestedClass->DoActivateL();
    
    TBool containerAvailable = (TBool)iTestedClass->Container();
    TAL( leaveCode ? !containerAvailable : containerAvailable );
    if ( !leaveCode )
        {
        CCCAppViewPluginAknContainer* container = iTestedClass->Container();
        TAL( container->iSetMopParent_called );
        TAL( container->iBaseConstructL_called );
        }
    
    TAL( leaveCode ? 
        (iCCaAppUi->iRecoverFromBadPluginL_called && !iCCaAppUi->iAddToViewStackL_called) :
        (iCCaAppUi->iAddToViewStackL_called && !iCCaAppUi->iRecoverFromBadPluginL_called ))
    
    iLog->Log( _L("Test_DoActivateL -> passed") );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::Test_DoDeactivate
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::Test_DoDeactivate( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("Test_DoDeactivate") );
    
    iTestedClass->DoActivateL();
    TAL( iTestedClass->Container() );
    iTestedClass->DoDeactivate();
    TAL( !iTestedClass->Container() );
    TAL( iCCaAppUi->iRemoveFromViewStack_called );
    iCCaAppUi->iRemoveFromViewStack_called = EFalse;
    iTestedClass->DoDeactivate();
    TAL( !iCCaAppUi->iRemoveFromViewStack_called );
    
    iLog->Log( _L("Test_DoDeactivate -> passed") );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknView::Test_HandleCommandL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknView::Test_HandleCommandL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("Test_HandleCommandL") );

    iTestedClass->HandleCommandL( 42 );
    TAL( iCCaAppUi->iHandleCommandL_called );
    
    iLog->Log( _L("Test_HandleCommandL -> passed") );
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    return ( CScriptBase* ) ut_CCCAppViewPluginAknView::NewL( aTestModuleIf );
    }


//  End of File
