/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An active object that completes when there is a change in
*                ETel/TSY store
*
*/


// INCLUDE FILES
#include "CETelStoreNotification.h"

#include <VPbkDebug.h>
#include "CStoreBase.h"
#include "METelStoreNotificationObserver.h"
#include "VPbkSimStoreImplError.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CETelStoreNotification::CETelStoreNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CETelStoreNotification::CETelStoreNotification( CStoreBase& aStore )
:   CActive( EPriorityStandard ),
    iStore( aStore )
    {
    CActiveScheduler::Add( this );
    }
    
// Destructor
CETelStoreNotification::~CETelStoreNotification()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CETelStoreNotification::Activate
// -----------------------------------------------------------------------------
//
void CETelStoreNotification::Activate()
   {
   VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::NotifyStoreEvent h%d request"),
        iStore.ETelStoreBase().SubSessionHandle());
   iStore.ETelStoreBase().NotifyStoreEvent( iStatus, iEvents, iIndex );
   SetActive();
   }

// -----------------------------------------------------------------------------
// CETelStoreNotification::SetHighPriorityObserver
// -----------------------------------------------------------------------------
//
void CETelStoreNotification::SetHighPriorityObserver(
    METelStoreNotificationObserver& aHighPriorityObserver )
    {
    __ASSERT_DEBUG( !iHighPriorityObserver,
        VPbkSimStoreImpl::Panic(ECommandObserverAlreadySetInETelNotification));
    
    iHighPriorityObserver = &aHighPriorityObserver;
    }
       
// -----------------------------------------------------------------------------
// CETelStoreNotification::RemoveHighPriorityObserver
// -----------------------------------------------------------------------------
//
void CETelStoreNotification::RemoveHighPriorityObserver(
    METelStoreNotificationObserver& aHighPriorityObserver)
    {
    if ( iHighPriorityObserver == &aHighPriorityObserver )
        {
        iHighPriorityObserver = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CETelStoreNotification::RunL
// -----------------------------------------------------------------------------
//
void CETelStoreNotification::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::NotifyStoreEvent h%d complete %d\
        events=%b,slot=%d"),
        iStore.ETelStoreBase().SubSessionHandle(), result,iEvents,iIndex);

    // Activate the notication again
    Activate();

    if ( !NotifyObserver( iHighPriorityObserver, result ) )
        {
        NotifyObserver( &iStore, result );
        }
    }

// -----------------------------------------------------------------------------
// CETelStoreNotification::DoCancel
// -----------------------------------------------------------------------------
//
void CETelStoreNotification::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::NotifyStoreEvent h%d Cancel"),
        iStore.ETelStoreBase().SubSessionHandle());
    iStore.ETelStoreBase().CancelAsyncRequest( 
        EMobilePhoneStoreNotifyStoreEvent );
    }

// -----------------------------------------------------------------------------
// CETelStoreNotification::NotifyObserver
// -----------------------------------------------------------------------------
//    
TBool CETelStoreNotification::NotifyObserver( 
    METelStoreNotificationObserver* aObserver, TInt aResult )
    {
    if ( !aObserver )
        {
        return EFalse;
        }
    if ( aResult == KErrNone )
        {
        return aObserver->ETelStoreChanged( iIndex, iEvents );
        }
    else
        {
        return aObserver->ETelStoreChangeError( aResult );
        }
    }
} // namespace VPbkSimStoreImpl
//  End of File  
