/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command that deletes a contact from the (U)SIM using ETel
*                RMobilePhoneStore Delete
*
*/



// INCLUDE FILES
#include "CDeleteCmd.h"

#include "VPbkSimStoreImplError.h"
#include "CStoreBase.h"
#include "CContactArray.h"
#include "CETelStoreNotification.h"

#include <MVPbkSimContactObserver.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkSimCommand.h>

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDeleteCmd::CDeleteCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDeleteCmd::CDeleteCmd( CStoreBase& aStore, 
        MVPbkSimContactObserver& aObserver  )
        :   CActive( EPriorityStandard ),
            iStore( aStore ),
            iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDeleteCmd::ConstructL( RVPbkStreamedIntArray& aIndexArray )
    {
    TInt lastSimIndex = iStore.Contacts().Size();
    const TInt indexCount = aIndexArray.Count();
    for ( TInt i = 0; i < indexCount; ++i )
        {
        TInt simIndex = aIndexArray[i];
        // Check the validity of SIM index and append only indexes that
        // contain a contact.
        if ( simIndex > 0 && simIndex <= lastSimIndex )
            {
            iIndexArray.AppendL( simIndex );
            }
        }
        
    if ( iStore.ETelNotification() )
        {
        // listen to store events to consume the event if it's caused
        // by this command
        iStore.ETelNotification()->SetHighPriorityObserver( *this );
        }
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDeleteCmd* CDeleteCmd::NewL( CStoreBase& aStore, 
                              RVPbkStreamedIntArray& aIndexArray,
                              MVPbkSimContactObserver& aObserver )
    {
    CDeleteCmd* self = 
        new( ELeave ) CDeleteCmd( aStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aIndexArray );
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CDeleteCmd::~CDeleteCmd()
    {
    Cancel();
    RemoveNotificationObserver();
    delete iUpdateCommand;
    iIndexArray.Close();
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::Execute
// -----------------------------------------------------------------------------
//
void CDeleteCmd::Execute()
    {
    if ( iIndexArray.Count() == 0 )
        {
        // There was no valid indexes in client's array. Use KErrArgument and
        // Complete asynchronously.
        CompleteRequest( KErrArgument );
        }
    else
        {
        TRAPD( res, DeleteNextL() );
        if ( res != KErrNone )
            {
            // Complete asynchronously
            CompleteRequest( res );
            }
        }
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::RunL
// -----------------------------------------------------------------------------
//
void CDeleteCmd::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Delete h%d complete %d"),
        iStore.ETelStoreBase().SubSessionHandle(),result);

    switch ( result )
        {
        case KErrNone:
            {
            iUpdateCommand->Execute();                
            break;
            }
        case KErrNotFound:
            {
            // Contact was not there -> it's ok because this is deleting
            // Try to delete next one.
            TInt simIndex = NextContactIndex();
            if ( simIndex != KErrNotFound )
                {
                DeleteNextL( simIndex );
                }
            else
                {
                CompleteCommand();
                }
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
// CDeleteCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CDeleteCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Delete h%d Cancel"),
        iStore.ETelStoreBase().SubSessionHandle());
    iStore.ETelStoreBase().CancelAsyncRequest( EMobilePhoneStoreDelete );
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::DoCancel
// -----------------------------------------------------------------------------
//
TInt CDeleteCmd::RunError( TInt aError )
    {
    HandleError( aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CDeleteCmd::ETelStoreChanged
// Consume the notification if it's same index that is being modified by
// this command. Otherwise the event must not be consumed because some
// other component caused the event by using ETel API and not VPbk API
// -----------------------------------------------------------------------------
//
TBool CDeleteCmd::ETelStoreChanged( TInt aSimIndex, TUint32 /*aEvents*/ )
    {
    if ( iCurSimIndex == aSimIndex )
        {
        return ETrue;
        }
    return EFalse;
    }
  
// -----------------------------------------------------------------------------
// CDeleteCmd::ETelStoreChangeError
// -----------------------------------------------------------------------------
//
TBool CDeleteCmd::ETelStoreChangeError( TInt /*aError*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::CommandDone
// -----------------------------------------------------------------------------
//
void CDeleteCmd::CommandDone( MVPbkSimCommand& /*aCommand*/ )
    {
    TRAPD( res, DeleteNextL() );
    if ( res != KErrNone )
        {
        HandleError( res );
        }
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::CommandError
// -----------------------------------------------------------------------------
//
void CDeleteCmd::CommandError( MVPbkSimCommand& /*aCommand*/, TInt aError )
    {
    HandleError( aError );
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::DeleteNextL
// -----------------------------------------------------------------------------
//
void CDeleteCmd::DeleteNextL()
    {
    // Index validity has been checked in ConstructL. Find first slot
    // that contains a contact
    TInt simIndexToDelete = NextContactIndex();
    if ( simIndexToDelete != KErrNotFound )
        {
        DeleteNextL( simIndexToDelete );
        }
    else
        {
        // If nothing to delete then this operation has done it's job
        // successfully.
        CompleteRequest( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::DeleteNextL
// -----------------------------------------------------------------------------
//
void CDeleteCmd::DeleteNextL( TInt aSimIndex )
    {
    // Delete the previous update command
    delete iUpdateCommand;
    iUpdateCommand = NULL;
    // Reserve memory for an update command that will read the slot 
    // and update other store information after deletion
    iUpdateCommand = iStore.CreateUpdateCommandL( aSimIndex );
    // Add observer so that the this command will be notified
    // after the update is done.
    iUpdateCommand->AddObserverL( *this );
    
    // Finally the contact can be deleted
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Delete h%d slot %d"), 
        iStore.ETelStoreBase().SubSessionHandle(), aSimIndex);
    iStore.ETelStoreBase().Delete( iStatus, aSimIndex );
    iCurSimIndex = aSimIndex;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::NextContactIndex
// -----------------------------------------------------------------------------
//
TInt CDeleteCmd::NextContactIndex()
    {
    // Index validity has been checked in ConstructL. Find first slot
    // that contains a contact
    TInt simIndexToDelete = KErrNotFound;
    while ( simIndexToDelete == KErrNotFound && iIndexArray.Count() > 0 )
        {
        if ( iStore.Contacts().At( iIndexArray[0] ) )
            {
            simIndexToDelete = iIndexArray[0];
            }
        iIndexArray.Remove( 0 );
        }
    return simIndexToDelete;
    }
    
// -----------------------------------------------------------------------------
// CDeleteCmd::CompleteRequest
// -----------------------------------------------------------------------------
//
void CDeleteCmd::CompleteRequest( TInt aRequestResult )
    {
    // Delete request was not done so complete with internal result
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CDeleteCmd::CompleteRequest dummy request %d"), 
        aRequestResult );
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aRequestResult );
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CDeleteCmd::CompleteCommand
// -----------------------------------------------------------------------------
//
void CDeleteCmd::CompleteCommand()
    {
    RemoveNotificationObserver();
    iObserver.ContactEventComplete( MVPbkSimContactObserver::EDelete, NULL );
    }
    
// -----------------------------------------------------------------------------
// CDeleteCmd::HandleError
// -----------------------------------------------------------------------------
//
void CDeleteCmd::HandleError( TInt aError )
    {
    RemoveNotificationObserver();
    iObserver.ContactEventError( MVPbkSimContactObserver::EDelete, NULL, 
        aError );
    }

// -----------------------------------------------------------------------------
// CDeleteCmd::RemoveNotificationObserver
// -----------------------------------------------------------------------------
//
void CDeleteCmd::RemoveNotificationObserver()
    {
    if ( iStore.ETelNotification() )
        {
        iStore.ETelNotification()->RemoveHighPriorityObserver( *this );
        }
    }

} // namespace VPbkSimStoreImpl
//  End of File 
