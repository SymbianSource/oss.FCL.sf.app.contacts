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
* Description:  An active object that completes when bluetooth SIM access
*                profile status changes
*
*/


// INCLUDE FILES
#include "CSimStatusNotification.h"

// From Virtual Phonebook
#include "MSimStatusObserver.h"
#include "VPbkSimStoreImplError.h"
#include <CVPbkSimStateInformation.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <VPbkSimStateDefinitions.h>

// System includes
#include <featmgr.h>

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimStatusNotification::CSimStatusNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimStatusNotification::CSimStatusNotification()
:   CActive( EPriorityStandard ),
    iSimStatus( VPbkSimStoreImpl::SimUninitializedStatus )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::ConstructL
// -----------------------------------------------------------------------------
//
void CSimStatusNotification::ConstructL()
    {
    VPBK_DEBUG_PRINT(
    VPBK_DEBUG_STRING("VPbkSimStoreImpl::CSimStatusNotification::ConstructL") );    
    User::LeaveIfError( iSimStatusProperty.Attach(
        KVPbkSimStatusPSCategory, VPbkSimStoreImpl::SimStatusPSKey ) );    
    // Init state
    iSimStatus = ReadState();
    DoActivate();
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::NewL
// -----------------------------------------------------------------------------
//    
CSimStatusNotification* CSimStatusNotification::NewL()
    {
    CSimStatusNotification* self = 
        new( ELeave ) CSimStatusNotification();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CSimStatusNotification::~CSimStatusNotification()
    {
    Cancel();
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::ActivateL
// -----------------------------------------------------------------------------
//
TInt CSimStatusNotification::CurrentStatus() const
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl::CSimStatusNotification::CurrentStatus(%d)"), 
        iSimStatus);   
    return iSimStatus;   
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::AddObserverL
// -----------------------------------------------------------------------------
//
void CSimStatusNotification::AddObserverL( MSimStatusObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }
       
// -----------------------------------------------------------------------------
// CSimStatusNotification::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSimStatusNotification::RemoveObserver( MSimStatusObserver& aObserver )
    {
    TInt pos = iObservers.Find( &aObserver );
    if (pos != KErrNotFound )
        {
        iObservers.Remove( pos );
        }
    }
    
// -----------------------------------------------------------------------------
// CSimStatusNotification::RunL
// -----------------------------------------------------------------------------
//
void CSimStatusNotification::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CSimStatusNotification::RunL result %d"), result );

    // KErrNone means that there was a change in SIM status state.
    // KErrNotFound means that the PS key was removed.
    // From VPbkSimServer point of view KErrNotFound means that
    // we check the state normally and act accoring to that.
    if ( result == KErrNone || result == KErrNotFound )
        {
        TInt status = ReadState();
        if ( status != iSimStatus )
            {
            iSimStatus = status;
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
               "VPbkSimStoreImpl: CSimStatusNotification::RunL activated %d" ), 
                iSimStatus);
                
            if ( iSimStatus == VPbkSimStoreImpl::SimOkStatus )
                {
                SendObserverMessage( 
                    iObservers, &MSimStatusObserver::SimStatusOk );
                }
            else if ( iSimStatus == VPbkSimStoreImpl::SimUninitializedStatus )
                {
                SendObserverMessage( 
                    iObservers, &MSimStatusObserver::SimStatusUninitialized );
                }
            else
                {
                SendObserverMessage( 
                    iObservers, &MSimStatusObserver::SimStatusNotOk );
                }            
            }                
        }
    else
        {
        SendObserverMessageR( iObservers, 
            &MSimStatusObserver::SimStatusNotificationError, result );    
        }    
    
    // Activate the notification again
    DoActivate();
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::DoCancel
// -----------------------------------------------------------------------------
//
void CSimStatusNotification::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CSimStatusNotification::DoCancel"));
    iSimStatusProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::RunError
// -----------------------------------------------------------------------------
//    
TInt CSimStatusNotification::RunError( TInt aError )
    {
    SendObserverMessageR( iObservers, 
        &MSimStatusObserver::SimStatusNotificationError, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSimStatusNotification::DoActivate
// -----------------------------------------------------------------------------
//    
void CSimStatusNotification::DoActivate()
    {
    iSimStatusProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSimStatusNotification::ReadState
// -----------------------------------------------------------------------------
//     
TInt CSimStatusNotification::ReadState()
    {
    TInt status = KErrNotFound;
    iSimStatusProperty.Get(
            KVPbkSimStatusPSCategory, 
            VPbkSimStoreImpl::SimStatusPSKey, 
            status );    
            
    return status;
    }
} // namespace VPbkSimStoreImpl
//  End of File  
