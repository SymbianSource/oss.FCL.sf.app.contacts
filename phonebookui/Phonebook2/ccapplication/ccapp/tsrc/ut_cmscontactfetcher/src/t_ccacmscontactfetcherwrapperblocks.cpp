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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <e32property.h>

#include "t_ccacmscontactfetcherwrapper.h"
#include "ccapputilheaders.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateAndDelete", 
                T_CCCACmsContactFetcherWrapper::CreateAndDeleteL ),
        ENTRY( "CreateWithContent", 
                T_CCCACmsContactFetcherWrapper::CreateWithContentL ),
        ENTRY( "CreateWithContactLink", 
                T_CCCACmsContactFetcherWrapper::CreateWithContactLinkL ),
        ENTRY( "CreateWithMSISDN", 
                T_CCCACmsContactFetcherWrapper::CreateWithMSISDNL ),
        ENTRY( "CreateWithEmail", 
                T_CCCACmsContactFetcherWrapper::CreateWithEmailL ),
        ENTRY( "NoDataFields", 
                T_CCCACmsContactFetcherWrapper::NoDataFieldsL ),
        ENTRY( "ErrorInContactOpen", 
                T_CCCACmsContactFetcherWrapper::ErrorInContactOpenL ),
        ENTRY( "ErrorInContactFetch", 
                T_CCCACmsContactFetcherWrapper::ErrorInContactFetchL ),
        ENTRY( "CreateWithManyItems", 
                T_CCCACmsContactFetcherWrapper::CreateWithManyItemsL ),
        ENTRY( "HandlePhonebookNotification", 
                T_CCCACmsContactFetcherWrapper::HandlePhonebookNotificationL ),
        ENTRY( "TestContactStoreApiL", 
                T_CCCACmsContactFetcherWrapper::TestContactStoreApiL ),
        ENTRY( "FindFromOtherStoresL", 
                T_CCCACmsContactFetcherWrapper::FindFromOtherStoresL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo ); 

    return RunInternalL( KFunctions, count, aItem );  

    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateAndDeleteL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateAndDeleteL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateAndDelete, "CreateAndDelete" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateAndDelete );
    // Print to log file
    iLog->Log( KCreateAndDelete );

    User::LeaveIfError( SetupL( KTestCreateAndDelete, MCCAParameter::EContactId ) );
    
    TInt leaveCode( KErrNone );
    CCCAppCmsContactFetcherWrapper* leave = NULL;
    TRAP( leaveCode, leave = CCCAppCmsContactFetcherWrapper::InstanceL( ) ); 
    T1L( KErrNotFound, leaveCode );
    TL( leave == NULL );
          
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
                CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    wrapper->AddObserverL( *this );
    
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    CCCAppCmsContactFetcherWrapper* wrapper2 = 
                CCCAppCmsContactFetcherWrapper::InstanceL( ); 
    wrapper2->AddObserverL( *this );
    wrapper2->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper2->Release();
    wrapper2 = NULL;
    
    CCCAppCmsContactFetcherWrapper* leaveAgain = NULL;
    TRAP( leaveCode, leaveAgain = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ) ); 
    T1L( KErrAlreadyExists, leaveCode );
    TL( leaveAgain == NULL );
  
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );

    return KErrNone;

    }
    
 // -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::NoDataFieldsL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::NoDataFieldsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KNoDataFields, "NoDataFields" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KNoDataFields );
    // Print to log file
    iLog->Log( KNoDataFields );
    
    User::LeaveIfError( SetupL( KTestNoDataFields, MCCAParameter::EContactId ) );
        
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 1, iObserverCalledCount );

    return KErrNone;

    }
// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateWithContentL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateWithContentL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateWithContent, "CreateWithContent" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateWithContent );
    // Print to log file
    iLog->Log( KCreateWithContent );

    
    User::LeaveIfError( SetupL( 
            KTestOneContactFieldItem, MCCAParameter::EContactId ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );


    TInt count = 1; 
    for ( TInt i(0); i < count; i++ )
        {
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        }

    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 1, iObserverCalledCount );

    return KErrNone;

    }
// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateWithManyItemsL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateWithManyItemsL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateWithManyItems, "CreateWithManyItems" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateWithManyItems );
    // Print to log file
    iLog->Log( KCreateWithManyItems );

    
    User::LeaveIfError( SetupL( 
            KTestManyContactFieldItems, MCCAParameter::EContactId ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );

    TInt count = 3;
    for ( TInt i(0); i < count; i++ )
        {
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        }

    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 3, iObserverCalledCount );

    return KErrNone;

    }
// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateWithContactLinkL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateWithContactLinkL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateWithContactLink, "CreateWithContactLink" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateWithContactLink );
    // Print to log file
    iLog->Log( KCreateWithContactLink );
 
    User::LeaveIfError( SetupL( KTestContactLink, MCCAParameter::EContactLink ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );
    

    TInt count = 1; 
    for ( TInt i(0); i < count; i++ )
        {
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        }

    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 1, iObserverCalledCount );    

    return KErrNone;

    }
    
 // -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateWithMSISDNL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateWithMSISDNL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateWithMSISDN, "CreateWithMSISDN" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateWithMSISDN );
    // Print to log file
    iLog->Log( KCreateWithMSISDN );
   
    User::LeaveIfError( SetupL( KTestMSISDN, MCCAParameter::EContactMSISDN ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );
    
    const CCmsContactFieldInfo* info = wrapper->ContactInfo();
    TL( NULL == info );
    
    CompleteRequestL( &wrapper->iStatus, KErrNone );

    TInt count = 2; 
    for ( TInt i(0); i < count; i++ )
        {
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        }
    
    info = wrapper->ContactInfo();    
    TL( NULL != info );
    
    RPointerArray<CCmsContactField>& array = wrapper->ContactFieldDataArray();
    T1L( 1, array.Count() ); 
        
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 2, iObserverCalledCount ); 

    return KErrNone;

    }
 // -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CreateWithEmailL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::CreateWithEmailL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KCreateWithEmail, "CreateWithEmail" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KCreateWithEmail );
    // Print to log file
    iLog->Log( KCreateWithEmail );

    User::LeaveIfError( SetupL( KTestEmail, MCCAParameter::EContactEmail ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    
    TInt error ( KErrNone );
    CCCAppCmsContactFetcherWrapper* wrapper = NULL;
    TRAP( error, wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ) );    
    wrapper->AddObserverL( *this );
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;
       
    CleanupStack::PopAndDestroy( param );
    
    TL( iErrorObserverCalled );
    TL( EFalse == iObserverCalled );

    return KErrNone;

    }

// --------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::ContactFieldDataObserverNotifyL
// --------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter )
    {
    iObserverCalled = ETrue;
    iObserverCalledCount++;
    iLog->Log( _L("..contact info notified") );
    
    if ( aParameter.iType == 
            MCCAppContactFieldDataObserver::TParameter::EContactsChanged )
        {
        iObserverCalledWithContactsChanged = ETrue;
        }
    else
        {
        switch ( iOngoingTestCase )
            {
            case KTestOneContactFieldItem:
            case KTestContactLink:
                {
                if ( 1 == iObserverCalledCount )
                    {
                    T1L( MCCAppContactFieldDataObserver::TParameter::EContactInfoAvailable, 
                            aParameter.iType );
                    }
                if ( 2 == iObserverCalledCount )
                    {
                    T1L( MCCAppContactFieldDataObserver::TParameter::EContactDeleted, 
                            aParameter.iType );
                    }
                break;
                }
            default:
                {
                break;
                }
            } 
        }
    
    if ( aParameter.iType == 
            MCCAppContactFieldDataObserver::TParameter::EContactsChanged )
        {
        iObserverCalledWithContactsChanged = ETrue;
        }
    
    if ( iSchedulerStarted )
        {
        iSchedulerStarted = EFalse;
        CActiveScheduler::Stop();
        }
    }
    
// --------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::ContactFieldDataObserverHandleErrorL
// --------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::ContactFieldDataObserverHandleErrorL( 
        TInt /*aState*/, TInt /*aError*/ )
    {
    iErrorObserverCalled = ETrue;

    iLog->Log( _L("hanle error notified") );
    
    switch ( iOngoingTestCase )
        {
        case KTestEmail:
            {
            break;
            }
        default:
            {
            break;
            }

        }
    if ( iSchedulerStarted )
        {
        iSchedulerStarted = EFalse;
        CActiveScheduler::Stop();
        }
    }
           
// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::ErrorInContactOpenL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::ErrorInContactOpenL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KErrorInContactOpen, "ErrorInContactOpen" );
    TestModuleIf().Printf( 
            0, KT_CCCACmsContactFetcherWrapper, KErrorInContactOpen );
    // Print to log file
    iLog->Log( KErrorInContactOpen );
    
    User::LeaveIfError( SetupL( 
            KTestCompleteOpenWithError, MCCAParameter::EContactId ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    CompleteRequestL( &wrapper->iStatus, KErrCancel );
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iErrorObserverCalled )

    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::ErrorInContactFetchL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::ErrorInContactFetchL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KErrorInContactFetch, "ErrorInContactFetch" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KErrorInContactFetch );
    // Print to log file
    iLog->Log( KErrorInContactFetch );

    
    User::LeaveIfError( SetupL( 
            KTestCompleteFetchWithError, MCCAParameter::EContactId ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    
    wrapper->AddObserverL( *this );
    TRequestStatus* status = &wrapper->iStatus;
    User::RequestComplete( status, KErrNone );
            iSchedulerStarted = ETrue;
    CStopSchedulerAfterDelay* schedulerStopper = 
            new (ELeave) CStopSchedulerAfterDelay();
    schedulerStopper->ConstructL();
    CleanupStack::PushL( schedulerStopper );
    schedulerStopper->After( 1000000 );// 1 sec
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( schedulerStopper );

    TInt count = 1; 
    for ( TInt i(0); i < count; i++ )
        {
        CompleteRequestL( &wrapper->iStatus, KErrCancel );
        }

    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iErrorObserverCalled );

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::HandlePhonebookNotificationL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::HandlePhonebookNotificationL( 
        CStifItemParser& aItem )
    {
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KHandlePhonebookNotification, "HandlePhonebookNotification" );
    TestModuleIf().Printf( 
            0, KT_CCCACmsContactFetcherWrapper, KHandlePhonebookNotification );
    // Print to log file
    iLog->Log( KHandlePhonebookNotification );
    
    User::LeaveIfError( SetupL( 
            KTestOneContactFieldItem, MCCAParameter::EContactId ) );
        
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param );
    
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    
    TInt modifiedNotDeleted;
    if ( aItem.GetNextInt( modifiedNotDeleted ) != KErrNone )
            return KErrNotFound;
    
    if (modifiedNotDeleted)
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        wrapper->HandlePhonebookNotificationL(ECmsContactModified);
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        CompleteRequestL( &wrapper->iStatus, KErrNone );
        // Observer is called 2 times when fetching contacts, 
        // and contacts are fetched again after they are changed so 1 + 2*2 = 5
        T1L( iObserverCalledCount, 4); 
        TL( iObserverCalledWithContactsChanged);
        }
    else
        {
        wrapper->HandlePhonebookNotificationL(ECmsContactDeleted);
        T1L( iObserverCalledCount, 2);
        }
        
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::TestContactStoreApiL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::TestContactStoreApiL( 
        CStifItemParser& /*aItem */)
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KDescription, "TestContactStoreApiL" );
    TestModuleIf().Printf( 0, KT_CCCACmsContactFetcherWrapper, KDescription );
    // Print to log file
    iLog->Log( KDescription );
    
    User::LeaveIfError( SetupL( KTestNoDataFields, MCCAParameter::EContactId ) );
    // setup
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
            CCCAppCmsContactFetcherWrapper::CreateInstanceL( param ); 
    wrapper->AddObserverL( *this );

    CompleteRequestL( &wrapper->iStatus, KErrNone );

    // test itself
    wrapper->iCmsContactDataFetcher.iContactStore_called = EFalse;
    TCmsContactStore cntStore = wrapper->ContactStore();
    TAL( wrapper->iCmsContactDataFetcher.iContactStore_called );

    // cleanup
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::FindFromOtherStoresL
//  test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::FindFromOtherStoresL( 
        CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KT_CCCACmsContactFetcherWrapper, "T_CCCACmsContactFetcherWrapper" );
    _LIT( KFindFromOtherStoresL, "FindFromOtherStoresL" );
    TestModuleIf().Printf( 
            0, KT_CCCACmsContactFetcherWrapper, KFindFromOtherStoresL );
    // Print to log file
    iLog->Log( KFindFromOtherStoresL );
 
    User::LeaveIfError( SetupL( KTestContactLink, MCCAParameter::EContactLink ) );
     
    CCCAParameter* param = CCCAParameter::NewL();
    CleanupStack::PushL( param );
    CCCAppCmsContactFetcherWrapper* wrapper = 
        CCCAppCmsContactFetcherWrapper::CreateInstanceL( 
             param, 
             CCCAppCmsContactFetcherWrapper::EFindContactFromOtherStores ); 
    wrapper->AddObserverL( *this );

    // Since there is not needed external notifiers for the
    // finding from other stores functionality, the testing
    // needs external way of stopping the CActiveScheduler.
    // Simple timer, CStopSchedulerAfterDelay, used here.
    TRequestStatus* status = &wrapper->iStatus;
    User::RequestComplete( status, KErrNone );
    iSchedulerStarted = ETrue;
    CStopSchedulerAfterDelay* schedulerStopper = 
        new (ELeave) CStopSchedulerAfterDelay();
    schedulerStopper->ConstructL();
    schedulerStopper->After( 1000000 );// 1 sec
    CActiveScheduler::Start();
    iSchedulerStarted = EFalse;
    delete schedulerStopper;
    schedulerStopper = NULL;
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    iSchedulerStarted = EFalse;
    CompleteRequestL( &wrapper->iStatus, KErrNone );
    wrapper->RemoveObserver( *this );
    if( wrapper->IsActive() )
        {
        TRequestStatus* status = &wrapper->iStatus;
        User::RequestComplete( status, KErrNone );
        }
    wrapper->Release(); 
    wrapper = NULL;

    CleanupStack::PopAndDestroy( param );
    
    TL( iObserverCalled );
    T1L( 1, iObserverCalledCount );    

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::SetupL
//
// -----------------------------------------------------------------------------
//
TInt T_CCCACmsContactFetcherWrapper::SetupL( TInt aTestCase, TInt aParameterType )
    {
    iErrorObserverCalled = EFalse;
    iObserverCalled = EFalse;
    iObserverCalledCount = 0;
    
    iOngoingTestCase = aTestCase;
    
    // Ongoing test case
    TInt err = RProperty::Define( 
            KTestPropertyCat, ETestCaseStateProperty, RProperty::EInt );
    if ( err != KErrAlreadyExists && err != KErrNone )
        {
        return ( err );
        }
    err = RProperty::Set( KTestPropertyCat, ETestCaseStateProperty, aTestCase );
    
    
    // Parameter type used in CCAParameter creation
    err = RProperty::Define( 
            KTestPropertyCat, ETestParameterTypeProperty, RProperty::EInt );
    if ( err != KErrAlreadyExists && err != KErrNone )
        {
        return ( err );
        }
    err = RProperty::Set( 
            KTestPropertyCat, ETestParameterTypeProperty, aParameterType );
    
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    if( !CActiveScheduler::Current() )
        {
        CActiveScheduler::Install( scheduler );
        }
    else
        {
        delete scheduler;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// T_CCCACmsContactFetcherWrapper::CompleteRequestL
//
// -----------------------------------------------------------------------------
//
void T_CCCACmsContactFetcherWrapper::CompleteRequestL( 
        TRequestStatus* aStatus, TInt aReason )
    {
    TRequestStatus* status = aStatus;
    User::RequestComplete( status, aReason );

    iSchedulerStarted = ETrue;
    
    CStopSchedulerAfterDelay* schedulerStopper = 
        new (ELeave) CStopSchedulerAfterDelay();
    schedulerStopper->ConstructL();
    CleanupStack::PushL( schedulerStopper );
    schedulerStopper->After( 1000000 );// 1 sec
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( schedulerStopper );

    }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
