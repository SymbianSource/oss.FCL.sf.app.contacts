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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include <e32math.h>

#define __COMMLAUNCHERPLUGINUNITTESTMODE
#include "t_commlaunchercontacthandler.h"
#include "t_testsingleton.h"

#include "ccappcommlauncherheaders.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo T_CCCAppCommLauncherContactHandler::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    t_commlaunchercontacthandler.cpp file and to t_commlaunchercontacthandler.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // T_CCCAppCommLauncherContactHandler::PrintTest. Otherwise the compiler
        // gives errors.
        
        FUNCENTRY( T_CCCAppCommLauncherContactHandler::PrintTest ),
        //ENTRY( "Loop test", T_CCCAppCommLauncherContactHandler::LoopTest ),
        ENTRY( "CreateAndDelete", T_CCCAppCommLauncherContactHandler::TestNewL ),
        ENTRY( "RequestContactDataL", T_CCCAppCommLauncherContactHandler::TestRequestContactDataL ),
        ENTRY( "ContactFieldDataObserverNotifyL", TestContactFieldDataObserverNotifyL ),
        //ENTRY( "CalculationAndGetters", TestCalculationAndGettersL ), Seems to be not valid any more
        //ENTRY( "Updates", TestUpdatesL ),Seems to be not valid any more
        ENTRY( "DefaultAttributes", TestDefaultAttributesL ),
        ENTRY( "TestContactStoreApiL", TestContactStoreApiL ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", T_CCCAppCommLauncherContactHandler::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( T_CCCAppCommLauncherContactHandler::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid  
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::
//
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::TestRequestContactDataL( TTestResult& aResult )
    {
    
    CCCAppCommLauncherPlugin* plugin =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view, *plugin );
    CleanupStack::PushL( handler );
    
    T_CTestSingleton* singleton = T_CTestSingleton::InstanceL();
    singleton->SetValue( KTestContactInfoNull );

    view->ResetVariables();
    handler->RequestContactDataL();
    TL( EFalse == view->ContactChangedNotifyCalled() );
    
    view->ResetVariables();
    singleton->SetValue( KTestNormal );
    handler->RequestContactDataL();
    TL( view->ContactChangedNotifyCalled() );
    
    singleton->Release();
    singleton = NULL;
    
    CleanupStack::PopAndDestroy( 3 );

    _LIT( KDescription, "TestRequestContactDataL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestContactFieldDataObserverNotifyL
//
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::TestContactFieldDataObserverNotifyL ( TTestResult& aResult )
    {
    CCCAppCommLauncherPlugin* plugin =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view, *plugin );
    CleanupStack::PushL( handler );
   
    CCmsContactFieldInfo* contactInfo = CCmsContactFieldInfo::NewL();
    CleanupStack::PushL( contactInfo );
    CCmsContactField* contactField = new (ELeave) CCmsContactField ( VOIP );
    CleanupStack::PushL( contactField );
   
    MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
    handler->ContactFieldDataObserverNotifyL( param );

    view->ResetVariables();
    param.iType = MCCAppContactFieldDataObserver::TParameter::EContactInfoAvailable;
    handler->ContactFieldDataObserverNotifyL( param );
    TL( EFalse == view->ContactEnabledFieldsChangedNotifyCalled() );
    
    view->ResetVariables();
    param.iContactInfo = contactInfo;
    handler->ContactFieldDataObserverNotifyL( param );
    TL( view->ContactEnabledFieldsChangedNotifyCalled() );
    
    view->ResetVariables();
    param.iType = MCCAppContactFieldDataObserver::TParameter::EContactDataFieldAvailable;
    handler->ContactFieldDataObserverNotifyL( param );
    TL( EFalse == view->ContactChangedNotifyCalled() );
    
    view->ResetVariables();
    param.iContactField = contactField;
    handler->ContactFieldDataObserverNotifyL( param );
    TL( view->ContactChangedNotifyCalled() );
    
    CleanupStack::PopAndDestroy( 5 ); 
        
    _LIT( KDescription, "TestContactFieldDataObserverNotifyL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }
    
    
// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::PrintTest( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );
           
    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );
    
    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for( TInt i=0; i<10; i++)
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i);
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestNewL
//
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::TestNewL( TTestResult& aResult )
    {    
    __UHEAP_MARK;
    CCCAppCommLauncherPlugin* plugin =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view, *plugin );
    CleanupStack::PushL( handler );
    CleanupStack::PopAndDestroy( 3 );
    __UHEAP_MARKEND;
    
    
    __UHEAP_MARK;
    CCCAppCommLauncherPlugin* plugin1 =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view2 = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view2 );
    CCCAppCommLauncherContactHandler* handler2 = CCCAppCommLauncherContactHandler::NewL( *view2, *plugin1 );
    CleanupStack::PushL( handler2 );
    CleanupStack::Pop( 3 );
    delete view2;
    delete handler2;
    __UHEAP_MARKEND;
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "NewL test passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }
  

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestCalculationAndGettersL
//
// -----------------------------------------------------------------------------
//
/*TInt T_CCCAppCommLauncherContactHandler::TestCalculationAndGettersL( TTestResult& aResult )
    {
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view );
    CleanupStack::PushL( handler );
    
    handler->ContactFieldDataObserverHandleErrorL( 1, -3 );
    
    CCmsContactFieldInfo* fieldInfo = CCmsContactFieldInfo::NewL();
    CleanupStack::PushL( fieldInfo );
    
    handler->CalculateAddressAmountsFromEnabledFields( *fieldInfo );
    
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    T1L( 2, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EUniEditorSelector ));
    fieldInfo->AddMoreFields();
    
    handler->CalculateAddressAmountsFromEnabledFields( *fieldInfo );
    
    T1L( 4, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 3, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 2, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    T1L( 6, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EUniEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EInstantMessagingSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EURLSelector ));
    
    RPointerArray<CCmsContactField> array = handler->ContactFieldDataArray();
    T1L( 1, array.Count( ) );
    
    HBufC8* test = handler->ContactIdentifierLC( ECmsPackedContactLinkArray );
    T1L( 0, test->Compare( KTestString ) );
    CleanupStack::PopAndDestroy( test );
    
    CleanupStack::PopAndDestroy( 3 );

    _LIT( KDescription, "TestCalculationAndGettersL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }  
    */
// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestUpdatesL
//
// -----------------------------------------------------------------------------
//
/*TInt T_CCCAppCommLauncherContactHandler::TestUpdatesL( TTestResult& aResult )
    {

    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view );
    CleanupStack::PushL( handler );
    
    CCmsContactFieldInfo* fieldInfo = CCmsContactFieldInfo::NewL();
    CleanupStack::PushL( fieldInfo );
    handler->CalculateAddressAmountsFromEnabledFields( *fieldInfo );

    CCmsContactField* emailField = new (ELeave) CCmsContactField ( EMAIL );
    CleanupStack::PushL( emailField );
    handler->UpdateAddressAmounts( *emailField );
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( emailField );
    
    CCmsContactField* phoneField = new (ELeave) CCmsContactField ( PHONE );
    CleanupStack::PushL( phoneField );
    handler->UpdateAddressAmounts( *phoneField );
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( phoneField );
    
    CCmsContactField* voipField = new (ELeave) CCmsContactField ( VOIP );
    CleanupStack::PushL( voipField );
    handler->UpdateAddressAmounts( *voipField );
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( voipField );
    
    CCmsContactField* emailField2 = new (ELeave) CCmsContactField ( EMAIL2 );
    CleanupStack::PushL( emailField2 );    
    handler->UpdateAddressAmounts( *emailField2 );
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( emailField2 );
    
    CCmsContactField* phoneField2 = new (ELeave) CCmsContactField ( PHONE2 );
    CleanupStack::PushL( phoneField2 );   
    handler->UpdateAddressAmounts( *phoneField2 );   
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( phoneField2 );
    
    CCmsContactField* voipField2 = new (ELeave) CCmsContactField ( VOIP2 );
    CleanupStack::PushL( voipField2 );    
    handler->UpdateAddressAmounts( *voipField2 );  
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( voipField2 );
    
    CCmsContactField* name = new (ELeave) CCmsContactField ( NAME );
    CleanupStack::PushL( name );
    handler->UpdateAddressAmounts( *name );    
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ));
    T1L( 1, handler->AddressAmount( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ));
    CleanupStack::PopAndDestroy( name );
    
    CleanupStack::PopAndDestroy( 3 );

    _LIT( KDescription, "TestCalculationAndGettersL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    } 
    */
// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestDefaultAttributesL
//
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::TestDefaultAttributesL( TTestResult& aResult )
    {
    CCCAppCommLauncherPlugin* plugin =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view, *plugin );
    CleanupStack::PushL( handler );
    
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EUniEditorSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) );
                
    CCmsContactField* emailField = new (ELeave) CCmsContactField ( EMAIL );
    CleanupStack::PushL( emailField );
    handler->UpdateDefaultAttributes( *emailField );
    
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EUniEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) );
    
    CleanupStack::PopAndDestroy( emailField );
    
    CCmsContactField* voipField = new (ELeave) CCmsContactField ( VOIP );
    CleanupStack::PushL( voipField );
    handler->UpdateDefaultAttributes( *voipField );
    
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EUniEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) );
    
    CleanupStack::PopAndDestroy( voipField );
    
    CCmsContactField* phoneField = new (ELeave) CCmsContactField ( PHONE );
    CleanupStack::PushL( phoneField );
    handler->UpdateDefaultAttributes( *phoneField );
    
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) );
    TL( EFalse == handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EUniEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) );
    
    CleanupStack::PopAndDestroy( phoneField );
    
    CCmsContactField* mmsField = new (ELeave) CCmsContactField ( MMS );
    CleanupStack::PushL( mmsField );
    handler->UpdateDefaultAttributes( *mmsField );
    
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EUniEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EEmailEditorSelector ) );
    TL( handler->HasDefaultAttribute( VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) );
    
    CleanupStack::PopAndDestroy( mmsField );
    
    CleanupStack::PopAndDestroy( 3 );

    _LIT( KDescription, "TestCalculationAndGettersL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }   

// -----------------------------------------------------------------------------
// T_CCCAppCommLauncherContactHandler::TestContactStoreApiL
//
// -----------------------------------------------------------------------------
//
TInt T_CCCAppCommLauncherContactHandler::TestContactStoreApiL( TTestResult& aResult )
    {
    // setup
    CCCAppCommLauncherPlugin* plugin =
                new (ELeave) CCCAppCommLauncherPlugin;
    CleanupStack::PushL( plugin );
    CCCAppCommLauncherView* view = CCCAppCommLauncherView::NewL();
    CleanupStack::PushL( view );
    CCCAppCommLauncherContactHandler* handler = CCCAppCommLauncherContactHandler::NewL( *view, *plugin );
    CleanupStack::PushL( handler );
    
    // test itself
    handler->iCmsWrapper->iContactStore_called = EFalse;
    TCmsContactStore cntStore = handler->ContactStore();
    TAL( handler->iCmsWrapper->iContactStore_called );
    // no need to test cntStore, since the functionality is just a
    // getter from a separate class

    // cleanup
    CleanupStack::PopAndDestroy( 3 );

    _LIT( KDescription, "TestContactStoreApiL passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

//  [End of File] - do not remove


