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
#include "CBtSapNotification.h"

// From Virtual Phonebook
#include "MBtSapObserver.h"
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
// CBtSapNotification::CBtSapNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBtSapNotification::CBtSapNotification( CVPbkSimStateInformation& aSimStateInfo )
:   CActive( EPriorityStandard ),
    iSimStateInfo( aSimStateInfo )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CBtSapNotification::ConstructL
// -----------------------------------------------------------------------------
//
void CBtSapNotification::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

// -----------------------------------------------------------------------------
// CBtSapNotification::NewL
// -----------------------------------------------------------------------------
//    
CBtSapNotification* CBtSapNotification::NewL( 
        CVPbkSimStateInformation& aSimStateInfo )
    {
    CBtSapNotification* self = 
        new( ELeave ) CBtSapNotification( aSimStateInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CBtSapNotification::~CBtSapNotification()
    {
    Cancel();
    iObservers.Close();
    delete iDeactivationIdle;
    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CBtSapNotification::ActivateL
// -----------------------------------------------------------------------------
//
void CBtSapNotification::ActivateL()
   {
   VPBK_DEBUG_PRINT(
    VPBK_DEBUG_STRING("VPbkSimStoreImpl::CBtSapNotification::ActivateL") );
   
   if ( !iDeactivationIdle )
       {
       iDeactivationIdle = CIdle::NewL( EPriorityIdle );
       }
       
   if ( FeatureManager::FeatureSupported( KFeatureIdBtSap ) )
        {
        User::LeaveIfError( iBtSapProperty.Attach(
            KVPbkBTSapStatusPSCategory, BTSapStatusPSKey ) );    
        DoActivate();
        }
   }

// -----------------------------------------------------------------------------
// CBtSapNotification::AddObserverL
// -----------------------------------------------------------------------------
//
void CBtSapNotification::AddObserverL( MBtSapObserver& aObserver )
    {
    if (iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }
       
// -----------------------------------------------------------------------------
// CBtSapNotification::RemoveObserver
// -----------------------------------------------------------------------------
//
void CBtSapNotification::RemoveObserver( MBtSapObserver& aObserver )
    {
    TInt pos = iObservers.Find( &aObserver );
    if (pos != KErrNotFound )
        {
        iObservers.Remove( pos );
        }
    }
    
// -----------------------------------------------------------------------------
// CBtSapNotification::RunL
// -----------------------------------------------------------------------------
//
void CBtSapNotification::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CBtSapNotification::RunL result %d"), result );

    // KErrNone means that there was a change in BT SAP state.
    // KErrNotFound means that the PS key was removed.
    // From VPbkSimServer point of view KErrNotFound means that
    // we check the state normally and act accoring to that.
    if ( result == KErrNone || result == KErrNotFound )
        {
        TBool active = iSimStateInfo.BTSapEnabledL();
        // If the state didn't change then do nothing
        if ( iActive != active )
            {
            // BT SAP state changed -> inform observers
            iActive = active;
        
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
                "VPbkSimStoreImpl: CBtSapNotification::RunL activated %d" ), 
                iActive);
                
            if ( iActive )
                {
                SendObserverMessage( iObservers, &MBtSapObserver::BtSapActivated );
                }
            else
                {
                SendObserverMessage( iObservers, &MBtSapObserver::BtSapDeactivated );
                }
            }
                
        }
    else
        {
        SendObserverMessageR( iObservers, 
            &MBtSapObserver::BtSapNotificationError, result );    
        }    
    
    // Activate the notification again
    DoActivate();
    }

// -----------------------------------------------------------------------------
// CBtSapNotification::DoCancel
// -----------------------------------------------------------------------------
//
void CBtSapNotification::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CBtSapNotification::DoCancel"));
    iBtSapProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CBtSapNotification::RunError
// -----------------------------------------------------------------------------
//    
TInt CBtSapNotification::RunError( TInt aError )
    {
    SendObserverMessageR( iObservers, 
        &MBtSapObserver::BtSapNotificationError, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBtSapNotification::DoActivate
// -----------------------------------------------------------------------------
//    
void CBtSapNotification::DoActivate()
    {
    iBtSapProperty.Subscribe( iStatus );
    SetActive();
    }
} // namespace VPbkSimStoreImpl
//  End of File  
