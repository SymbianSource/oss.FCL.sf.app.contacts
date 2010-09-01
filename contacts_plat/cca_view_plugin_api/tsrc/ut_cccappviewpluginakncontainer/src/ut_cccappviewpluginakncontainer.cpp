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

#include "ut_CCCAppViewPluginAknContainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::ut_CCCAppViewPluginAknContainer
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknContainer::ut_CCCAppViewPluginAknContainer( 
    CTestModuleIf& aTestModuleIf ): 
        CScriptBase( aTestModuleIf )
    {
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::ConstructL
// --------------------------------------------------------------------------
//
void ut_CCCAppViewPluginAknContainer::ConstructL()
    {
    iLog = CStifLogger::NewL( Kt_testiLogPath, 
                          Kt_testiLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::NewL
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknContainer* ut_CCCAppViewPluginAknContainer::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    ut_CCCAppViewPluginAknContainer* self = new (ELeave) ut_CCCAppViewPluginAknContainer( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::~ut_CCCAppViewPluginAknContainer
// --------------------------------------------------------------------------
//
ut_CCCAppViewPluginAknContainer::~ut_CCCAppViewPluginAknContainer()
    { 
    delete iLog;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::RunMethodL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "Example", ut_CCCAppViewPluginAknContainer::ExampleL ),
        ENTRY( "SetupL", ut_CCCAppViewPluginAknContainer::SetupL ),
        ENTRY( "Teardown", ut_CCCAppViewPluginAknContainer::Teardown ),
        ENTRY( "Test_BaseConstructL", ut_CCCAppViewPluginAknContainer::Test_BaseConstructL ),
        ENTRY( "Test_OfferKeyEventL", ut_CCCAppViewPluginAknContainer::Test_OfferKeyEventL ),
        ENTRY( "Test_HandleResourceChange", ut_CCCAppViewPluginAknContainer::Test_HandleResourceChange )
        };
    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::SetupL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::SetupL( CStifItemParser& aItem )
    {
    iLog->Log( _L("#### Setup CCCAppViewPluginAknContainer") );

    TPtrC description;
    if ( aItem.GetNextString( description ) != KErrNone )
        return KErrNotFound;
    iLog->Log( _L("Testing: %S"), &description );
    
    iTestedClass = new (ELeave) CTestedClass();
    iCCaAppUi = new (ELeave) CCCAAppAppUi();
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::Teardown
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::Teardown( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("#### Teardown CCCAppViewPluginAknContainer") );

    delete iTestedClass;
    delete iCCaAppUi;
         
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::Test_BaseConstructL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::Test_BaseConstructL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("Test_BaseConstructL") );

    TRect testTRect( 1, 1, 2, 2 );
    iTestedClass->BaseConstructL( testTRect, *iCCaAppUi );

    TAL( iTestedClass->AppUi() == iCCaAppUi );
    T1L( iTestedClass->iCreateWindowL_called, 1 );
    T1L( iTestedClass->iConstructL_called, 1 );
    TAL( iTestedClass->iSetRect_called_with_param_aRect == testTRect );
    T1L( iTestedClass->iActivateL_called, 1 );
    
    iLog->Log( _L("Test_BaseConstructL -> passed") );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::Test_OfferKeyEventL
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::Test_OfferKeyEventL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("Test_OfferKeyEventL") );

    TKeyEvent testKeyEvent;
    testKeyEvent.iCode = 42;
    const TKeyEvent constTestKeyEvent( testKeyEvent );
    
    TKeyResponse returnedKeyResponse = EKeyWasConsumed;
    
    for ( TInt i = 0; i < 1000; i++ )
        {
        iCCaAppUi->iHandleKeyEventL_called = EFalse;
        returnedKeyResponse = iTestedClass->OfferKeyEventL( constTestKeyEvent, (TEventCode)i );
        T1L( returnedKeyResponse, EEventKey == i ? EKeyWasConsumed : EKeyWasNotConsumed );
        T1L( iCCaAppUi->iHandleKeyEventL_called, EEventKey == i ? 1 : 0 );
        }
    
    iLog->Log( _L("Test_OfferKeyEventL -> passed") );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// ut_CCCAppViewPluginAknContainer::Test_HandleResourceChange
// --------------------------------------------------------------------------
//
TInt ut_CCCAppViewPluginAknContainer::Test_HandleResourceChange( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("Test_HandleResourceChange") );

    // gather array of HandleResourceChange -types
    RArray<TInt> idArray;
    TUid uid;
    uid = KAknsSkinInstanceTls;
    idArray.Append( uid.iUid );
    uid = KAknsAppUiParametersTls;
    idArray.Append( uid.iUid );
    idArray.Append( KUidValueAknsSkinChangeEvent );
    idArray.Append( KAknsMessageSkinChange );
    idArray.Append( KEikMessageWindowsFadeChange );
    idArray.Append( KEikMessageCaptionedControlEditableStateChange );
    idArray.Append( KEikMessageCaptionedControlNotEditableStateChange );
    idArray.Append( KEikMessageCaptionedControlEditableStateChangeWideWithGraphic );
    idArray.Append( KEikMessageCaptionedControlEditableStateChangeWideWithoutGraphic );
    idArray.Append( KEikMessageCaptionedControlNotEditableStateChangeWideWithGraphic );
    idArray.Append( KEikMessageCaptionedControlNotEditableStateChangeWideWithoutGraphic );
    idArray.Append( KEikClearPartialForegroundState );
    idArray.Append( KEikPartialForeground );
    idArray.Append( KEikDynamicLayoutVariantSwitch );
    idArray.Append( KEikDynamicLayoutVariantSwitch_APAC );
    idArray.Append( KEikInputLanguageChange );
    idArray.Append( KAknHardwareLayoutSwitch );
    idArray.Append( KAknDelayedLayoutSwitch );
    idArray.Append( KAknLocalZoomLayoutSwitch );
    idArray.Append( KAknFullOrPartialForegroundGained );
    idArray.Append( KAknFullOrPartialForegroundLost );
    idArray.Append( KAknShutOrHideApp );
    
    // do the testing
    while ( idArray.Count() )
        {
        iTestedClass->HandleResourceChange( idArray[0] );
        
        T1L( iTestedClass->iHandleResourceChange_called_with_param_aType, idArray[0] );
        T1L( iTestedClass->iSetRect_called,
             (KAknsMessageSkinChange == idArray[0] 
              || KEikDynamicLayoutVariantSwitch == idArray[0]) ? 1 : 0 );

        iTestedClass->iSetRect_called = EFalse;    
        idArray.Remove( 0 );
        };
    
    iLog->Log( _L("Test_HandleResourceChange -> passed") );
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
    return ( CScriptBase* ) ut_CCCAppViewPluginAknContainer::NewL( aTestModuleIf );
    }


//  End of File
