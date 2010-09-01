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
* Description:  A command that writes a contact to (U)SIM card using
*                ETel RMobilePhoneBookStore Write
*
*/



// INCLUDE FILES
#include "CWriteCmdBase.h"

#include <VPbkDebug.h>
#include <MVPbkSimContactObserver.h>
#include <MVPbkSimCommand.h>
#include "CETelStoreNotification.h"
#include "VPbkSimStoreImplError.h"
#include <gsmerror.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWriteCmdBase::CWriteCmdBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWriteCmdBase::CWriteCmdBase( CStoreBase& aStore, TInt& aSimIndex,
    MVPbkSimContactObserver& aObserver )
    : CActive( EPriorityStandard ),
      iSimIndex( aSimIndex ),
      iStoreBase( aStore ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::BaseConstructL()
    {
    iUpdateCommand = iStoreBase.CreateUpdateCommandUsingRefL( iSimIndex );
    iUpdateCommand->AddObserverL( *this );
    }

// Destructor
CWriteCmdBase::~CWriteCmdBase()
    {
    Cancel();
    RemoveNotificationObserver();
    delete iUpdateCommand;
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::RunL
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::RunL()
    {
    TInt result = iStatus.Int();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhone(Book)Store::Write h%d complete %d"),
        iStoreBase.ETelStoreBase().SubSessionHandle(),result);

    switch ( result )
        {
        // These errors are handled when update is done
        // In those cases contact is partially saved and detail
        // which did not fit to SIM is simply discarded
        case KErrGsmSimServSneFull: // FALLTHROUGH
        case KErrGsmSimServAnrFull: // FALLTHROUGH
        case KErrGsmSimServEmailFull: 
        case KErrNone:
            {
            iUpdateCommand->Execute();
            break;
            }
        default:
            {
            HandleError( result );
            break;
            }
       }
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::DoCancel
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Write h%d Cancel"),
        iStoreBase.ETelStoreBase().SubSessionHandle());
    iStoreBase.ETelStoreBase().CancelAsyncRequest( EMobilePhoneStoreWrite );
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::ETelStoreChanged
// Consume the notification if it's same index that is being modified by
// this command. Otherwise the event must not be consumed because some
// other component caused the event by using ETel API and not VPbk API
// -----------------------------------------------------------------------------
//
TBool CWriteCmdBase::ETelStoreChanged( TInt aSimIndex, TUint32 /*aEvents*/ )
    {
    if ( iSimIndex == aSimIndex )
        {
        return ETrue;
        }
    return EFalse;
    }
  
// -----------------------------------------------------------------------------
// CWriteCmdBase::ETelStoreChangeError
// -----------------------------------------------------------------------------
//
TBool CWriteCmdBase::ETelStoreChangeError( TInt /*aError*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::CommandDone
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::CommandDone( MVPbkSimCommand& /*aCommand*/ )
    {
    RemoveNotificationObserver();
    TInt result = iStatus.Int();
    if ( result != KErrNone )
        {
        iObserver.ContactEventError( MVPbkSimContactObserver::ESave, NULL, 
            result ); 
        }
    else
        {
        iObserver.ContactEventComplete( MVPbkSimContactObserver::ESave, NULL );
        }

    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::CommandError
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::CommandError( MVPbkSimCommand& /*aCommand*/, TInt aError )
    {
    HandleError( aError );
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::Activate
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::Activate()
    {
    if ( iStoreBase.ETelNotification() )
        {
        // listen to store events to consume the event if it's caused
        // by this command
        iStoreBase.ETelNotification()->SetHighPriorityObserver( *this );
        }
        
    SetActive();
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::HandleError
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::HandleError( TInt aError )
    {
    RemoveNotificationObserver();
    iObserver.ContactEventError( MVPbkSimContactObserver::ESave, NULL, 
        aError ); 
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::RemoveNotificationObserver
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::RemoveNotificationObserver()
    {
    if ( iStoreBase.ETelNotification() )
        {
        iStoreBase.ETelNotification()->RemoveHighPriorityObserver( *this );
        }
    }

// -----------------------------------------------------------------------------
// CWriteCmdBase::CompleteCommandAsynchronously
// -----------------------------------------------------------------------------
//
void CWriteCmdBase::CompleteCommandAsynchronously( TInt aResult )
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, aResult );
    SetActive();
    }
} // namespace VPbkSimStoreImpl
//  End of File  
