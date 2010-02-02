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
* Description:  
*
*/


// INCLUDES
#include "CVPbkSimPhone.h"

#include <VPbkSimStoreTemplateFunctions.h>
#include <MVPbkSimPhoneObserver.h>
#include <VPbkSimServerOpCodes.h>
    
// MEMBER FUNCTIONS    
CVPbkSimPhone::CVPbkSimPhone()
: CActive(CActive::EPriorityStandard)
    {    
    }
    
void CVPbkSimPhone::ConstructL()
    {
    CActiveScheduler::Add(this);
    }
    
CVPbkSimPhone* CVPbkSimPhone::NewL()
    {
    CVPbkSimPhone* self = new (ELeave)CVPbkSimPhone();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CVPbkSimPhone::~CVPbkSimPhone()
    {
    Cancel();
    iObservers.Close();
    iSimStore.Close();
    }
    
// -----------------------------------------------------------------------------
// CVPbkSimPhone::AddObserverL
// -----------------------------------------------------------------------------
//
void CVPbkSimPhone::AddObserverL( MVPbkSimPhoneObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimPhone::RemoveObserver
// -----------------------------------------------------------------------------
//
void CVPbkSimPhone::RemoveObserver( MVPbkSimPhoneObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }
    
void CVPbkSimPhone::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {
        switch(iEventData.iEvent)
            {
            case EVPbkSimPhoneOpen:
                {
                VPbkSimStoreImpl::SendObserverMessageR(
                    iObservers, 
                    &MVPbkSimPhoneObserver::PhoneOpened, 
                    *this );
                break;
                }
            case EVPbkSimPhoneServiceTableUpdated:
                {
                VPbkSimStoreImpl::SendObserverMessageR(
                    iObservers, 
                    &MVPbkSimPhoneObserver::ServiceTableUpdated, 
                    iEventData.iData );
                break;
                }
            case EVPbkSimPhoneFdnStatusChanged:
                {
                VPbkSimStoreImpl::SendObserverMessageR(
                    iObservers, 
                    &MVPbkSimPhoneObserver::FixedDiallingStatusChanged,
                    iEventData.iData );
                break;
                }
            case EVPbkSimPhoneError: // FALLTHROUGH
            default:
                {
                // iEventData.iData contains the error id
                // iEventData.iOpData contains the error value
                MVPbkSimPhoneObserver::TErrorIdentifier id =
                    static_cast<MVPbkSimPhoneObserver::TErrorIdentifier>( 
                    iEventData.iData );
                VPbkSimStoreImpl::SendObserverMessageRVV(
                    iObservers,
                    &MVPbkSimPhoneObserver::PhoneError,
                    *this, 
                    id, 
                    iEventData.iOpData);
                break;
                }
            }        
        }      
    else
        {
        VPbkSimStoreImpl::SendObserverMessageRVV(
            iObservers,
            &MVPbkSimPhoneObserver::PhoneError,
            *this, 
            MVPbkSimPhoneObserver::ESystem, 
            iStatus.Int() );
        }

    // start listening events
    iSimStore.ListenToStoreEvents( iStatus, iEventData );
    SetActive();
    }
    
TInt CVPbkSimPhone::RunError(TInt aError)
    {
    VPbkSimStoreImpl::SendObserverMessageRVV(
        iObservers,
        &MVPbkSimPhoneObserver::PhoneError,
        *this, 
        MVPbkSimPhoneObserver::ESystem, 
        aError );

    return KErrNone;
    }
    
void CVPbkSimPhone::DoCancel()
    {
    iSimStore.CancelAsyncRequest( EVPbkSimSrvStoreEventNotification );
    }
    
void CVPbkSimPhone::OpenL( MVPbkSimPhoneObserver& aObserver )
    {
    AddObserverL(aObserver);
    if ( iSimStore.Handle() )
        {
        User::Leave( KErrInUse );
        }
    // Connect to sim store server
    iSimStore.ConnectToServerL();
    // Start listening to phone events
    iSimStore.ListenToStoreEvents( iStatus, iEventData );
    // Open the phone
    iSimStore.OpenPhoneL();
    SetActive();            
    }
    
void CVPbkSimPhone::Close( MVPbkSimPhoneObserver& aObserver )
    {
    if ( iSimStore.Handle() )
        {
        iSimStore.ClosePhone();
        }
    RemoveObserver(aObserver);
    }
    
TBool CVPbkSimPhone::USimAccessSupported() const
    {
    return iSimStore.USimAccessSupported();
    }
    
TUint32 CVPbkSimPhone::ServiceTable() const
    {
    return iSimStore.ServiceTable();
    }
    
MVPbkSimPhone::TFDNStatus CVPbkSimPhone::FixedDialingStatus() const
    {
    return iSimStore.FixedDialingStatus();
    }
    
// End of file
