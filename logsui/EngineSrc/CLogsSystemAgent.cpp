/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implements the functionality of the System Agent observer
*
*/


//  INCLUDE FILES  
#include <e32svr.h>

#include <PSVariables.h>
#include <coreapplicationuisdomainpskeys.h>

#include "CLogsSystemAgent.h"
#include "MLogsObserver.h"
#include "MLogsCallObserver.h"

#include <etel.h>
//#include <etelmm.h>
#include <mmtsy_names.h>


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsSystemAgent::CLogsSystemAgent
// ----------------------------------------------------------------------------
//
CLogsSystemAgent::CLogsSystemAgent(
    MLogsObserver* aObserver,
    MLogsCallObserver* aCallObserver ) :
        CActive( EPriorityStandard )        
    {
    iObserver = aObserver;
    iCallObserver = aCallObserver;    
    }

// ----------------------------------------------------------------------------
// CLogsSystemAgent::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSystemAgent::ConstructL()
    {
    // CActiveScheduler::Add( this );

    //Attach to neeeded  Publish and Subscribe properties        
    TInt ret = iPropertyGprs.Attach( KUidSystemCategory, KPSUidGprsStatusValue );   //TUid aCategory, TUint aKey
    User::LeaveIfError( ret );    
    
    ret = iPropertyWcdma.Attach( KUidSystemCategory, KPSUidWcdmaStatusValue );   
    User::LeaveIfError( ret );    

    // ret = iPropertyCallHandling.Attach( KPSUidTelephonyCallHandling, KTelephonyCallState ); 
    // User::LeaveIfError( ret );

    // ret = iPropertyCallStatus.Attach( KPSUidCallState, KPSUidCallStateValue );
    // User::LeaveIfError( ret );    

    //Attach to Supplementary Services Settings
    User::LeaveIfError( iSSSettings.Open() );

    // RunL();
    }

// ----------------------------------------------------------------------------
// CLogsSystemAgent::NewL
// ----------------------------------------------------------------------------
//
CLogsSystemAgent* CLogsSystemAgent::NewL(
    MLogsObserver* aObserver,
    MLogsCallObserver* aCallObserver ) 
    {
    CLogsSystemAgent* self = new ( ELeave ) CLogsSystemAgent( aObserver, aCallObserver );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CLogsSystemAgent::~CLogsSystemAgent
// ----------------------------------------------------------------------------
//
CLogsSystemAgent::~CLogsSystemAgent()
    {
    // Cancel();

    iPropertyGprs.Close();
    iPropertyWcdma.Close();    
    //iPropertyCallHandling.Close();    
    //iPropertyCallStatus.Close();    
    iSSSettings.Close();
    }

// ----------------------------------------------------------------------------
// CLogsSystemAgent::AlsSupport
// ----------------------------------------------------------------------------
//
TBool CLogsSystemAgent::AlsSupport()
    {
    TInt value;
    
    if( iSSSettings.Get( ESSSettingsAls, value ) )
        {
        return EFalse;
        }
    if( value == ESSSettingsAlsNotSupported ) 
        {
        return EFalse;
        }
    return ETrue;
  
    }

// ----------------------------------------------------------------------------
// CLogsSystemAgent::GprsConnectionActive
// ----------------------------------------------------------------------------
//
TBool CLogsSystemAgent::GprsConnectionActive()
    {
    if( IsGprs() )
        {
        TInt stateGprs(0);        
        iPropertyGprs.Get( KUidSystemCategory, KPSUidGprsStatusValue, stateGprs );  //TUid aCategory, TUint aKey, TInt& aValue

        if( stateGprs  == EPSGprsContextActive      ||  
            stateGprs  == EPSGprsSuspend            ||
            stateGprs  == EPSGprsContextActivating  ||
            stateGprs  == EPSGprsMultibleContextActive )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        TInt stateWcdma(0);        
        iPropertyWcdma.Get( KUidSystemCategory, KPSUidWcdmaStatusValue, stateWcdma );   

        if( stateWcdma == EPSWcdmaContextActive     ||  
            stateWcdma == EPSWcdmaSuspend           ||
            stateWcdma == EPSWcdmaContextActivating ||
            stateWcdma == EPSWcdmaMultipleContextActive )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
        

    }
    
/*****************************************************
TBool CLogsSystemAgent::CallActive( TBool &aValue )
    {
    TInt state(0);
    TInt ret = iPropertyCallHandling.Get( KPSUidTelephonyCallHandling, KTelephonyCallState, state ); //TUid aCategory, TUint aKey, TInt& aValue
    
    //See TelephonyInternalPSKeys.h    
    if( state == EPSTelephonyCallStateConnected )   //This covers voice, video, voip etc call types
        {
        aValue =  ETrue;
        }
    else
        {
        aValue =  EFalse;
        }

    return ret;        
    }
*****************************************************/
    
// ----------------------------------------------------------------------------
// CLogsSystemAgent::IsGprs
// ----------------------------------------------------------------------------
//
TBool CLogsSystemAgent::IsGprs() 
    {
    RTelServer telServer;
    RMobilePhone mobilePhone;
    TInt error = telServer.Connect();

    if( error != KErrNone )
        {
        return ETrue;
        }
    error = telServer.LoadPhoneModule( KMmTsyModuleName );

    if( error != KErrNone )
        {
        telServer.Close();
        return ETrue;
        }

    RTelServer::TPhoneInfo phoneInfo;
    error = telServer.GetPhoneInfo( 0, phoneInfo );

    if( error != KErrNone )
        {
        telServer.Close();
        return ETrue;            
        }
    error = mobilePhone.Open( telServer, phoneInfo.iName );

    if( error != KErrNone )
        {
        telServer.Close();
        return ETrue;            
        }

    RMobilePhone::TMobilePhoneNetworkMode networkMode = RMobilePhone::ENetworkModeGsm;
    error = mobilePhone.GetCurrentMode( networkMode );
    mobilePhone.Close();
    telServer.Close();

    if( error == KErrNone && networkMode == RMobilePhone::ENetworkModeWcdma )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
    
    
//Reset missed Poc call notification
// ----------------------------------------------------------------------------
// CLogsSystemAgent::ResetNewMissedPocCalls
// ----------------------------------------------------------------------------
//
void CLogsSystemAgent::ResetNewMissedPocCalls()
    {
    RProperty property;
    
    if ( property.Attach( KPSUidCoreApplicationUIs, KCoreAppUIsPoCMissedIndicator ) == KErrNone )
        {
        property.Set( ECoreAppUIsPocMissedIndicatorOff );
        property.Close();
        }
        
    }


// ----------------------------------------------------------------------------
// CLogsSystemAgent::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsSystemAgent::SetObserver( MLogsObserver* /*aObserver*/ )
    {
    }
    
    

// ----------------------------------------------------------------------------
// CLogsSystemAgent::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsSystemAgent::DoCancel()
    {     
    // iProperty.Cancel();
    // FLOG( _L("SystemAgent: DoCancel"));       
    }


// ----------------------------------------------------------------------------
// CLogsSystemAgent::RunL
// ----------------------------------------------------------------------------
//
void CLogsSystemAgent::RunL()
    {
    //(If telephone status changes need to be observed, below can be added code to call the 
    // iCallObserver->CallStateChangedL similarly as is in 2.8)
    //    
    //        // Resubscribe before processing new value to prevent missing updates
    //        iPropertyCallStatus.Subscribe( iStatus );
    //        SetActive();
    //    
    //        // Property updated, try to get new value
    //        TInt state = 0;
    //    
    //        if( iCallObserver && iPropertyCallStatus.Get( state ) == KErrNone )
    //            {
    //            FLOG_1( _L("SystemAgent: iPropertyCallStatus.Get() %d"), state );                           
    //            iCallObserver->CallStateChangedL( state );    //EPSCallStateRinging etc...
    //            }
    }

    
    

// End of File
