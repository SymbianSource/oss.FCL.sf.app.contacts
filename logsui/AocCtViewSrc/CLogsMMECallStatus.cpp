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
*     Call active notifier: either for data or voice line, multi mode ETEL version.
*
*/


//  INCLUDE FILES  
#include <e32svr.h>
#include <etel.h>
#include <etelmm.h>
#include <mmtsy_names.h>
#include "CLogsMMECallStatus.h"
#include "MLineStatusObserver.h"

// CONSTANTS
 
// ================= MEMBER FUNCTIONS =======================

//  Standard creation function.
CLogsCallStatus* CLogsCallStatus::NewLC( const TLineStatus aLine )
    {
    CLogsCallStatus* self = new ( ELeave ) CLogsCallStatus( aLine );
    CleanupStack::PushL( self );
	self->ConstructL();
    return self;
    }

CLogsCallStatus::~CLogsCallStatus()
    {
    Cancel();
    iLine.Close();
    CloseEtelConnection();
    delete iDataLine;
    delete iSecondaryLine;
    }

void CLogsCallStatus::ConstructL()
    {
    CActiveScheduler::Add(this);
    TInt ret = OpenEtelConnection();
    User::LeaveIfError( ret );
    
    switch( iSelectedLine )
        {
        case ECallAndDataLines:  // FALLTHROUGH
            {
            // Voice and Data lines should be monitored, but only check data lines if they are
            // supported by the TSY
            TBool dataSupported(EFalse);

            // ignore errors... on error, dataSupported should remain EFalse
            iServer.IsSupportedByModule( KMmTsyModuleName, KETelFuncMobileDataCall, dataSupported );

            if ( dataSupported ) //For data we need another instance of this
                {
                CLogsCallStatus* tmp = CLogsCallStatus::NewLC( EDataLine );
                CleanupStack::Pop(); // tmp
                iDataLine = tmp;
                }
            iSelectedLine = ECallLine;
            }
        case ECallLine:
            {
            TBool alsSupport(EFalse); //ALS alternate line service
            // ignore errors... on error, alsSupport should remain EFalse
            iServer.IsSupportedByModule( KMmTsyModuleName, KETelFuncMobileAlternateLineService, alsSupport );
            if ( alsSupport ) //For secondary line, we need yet another instance of this
                {
                CLogsCallStatus* tmp = CLogsCallStatus::NewLC( ESecondaryLine );
                CleanupStack::Pop(); // tmp
                iSecondaryLine = tmp;
                }

            
            ret = iLine.Open( iPhone, KMmTsyVoice1LineName );
            break;
            }
        case ESecondaryLine:
            {
            ret = iLine.Open( iPhone, KMmTsyVoice2LineName );
            break;
            }
        case EDataLine:
            {
            ret = iLine.Open( iPhone, KMmTsyDataLineName );
            break;
            }
        default:
            {
            CloseEtelConnection();
            User::Leave( KErrCorrupt );
            break;
            }
        }
    if( ret != KErrNone )
        {
        CloseEtelConnection();
        User::LeaveIfError( ret );
        }
    }

//  Default c++ constructor
CLogsCallStatus::CLogsCallStatus( const TLineStatus aLine ):   
    iSelectedLine( aLine )
    {
    }

void CLogsCallStatus::RunL()
    {
    if( iObserver )
        {
        iObserver->LineStatusChangedL();
        }
    if( !IsActive() )
        {
        Cancel();
        iLine.NotifyMobileLineStatusChange( iStatus, iLineStatus );
        SetActive();
        }
    }

void CLogsCallStatus::DoCancel()
    {
    iLine.CancelAsyncRequest( EMobileLineNotifyMobileLineStatusChange );
    }


TInt CLogsCallStatus::CallIsActive( TBool& aCallIsActive )
    {                
    aCallIsActive = EFalse;
    TInt ret( KErrNone );
    ret = iLine.GetMobileLineStatus( iLineStatus );

    if( ret )
        {
        return ret;
        }

    if( iLineStatus == RMobileCall::EStatusConnected )
        { 
        aCallIsActive = ETrue;
        }

    ////////////Fix for PNUN-666BDH
    //Let's also check do we have active data call going on (e.g video calls are data calls, not voice calls)
    TBool activeDataCall( EFalse );
    if( iDataLine ) 
        {
        iDataLine->CallIsActive( activeDataCall );  
        if ( activeDataCall )
            {
            aCallIsActive = ETrue;
            }
        }
    //////////////
    
    ////////////Fix for MVHA-6YUEXZ
    //Let's also check wether we have an active call on the secondary line (ALS)
    TBool secondaryLineActive( EFalse );
    if( iSecondaryLine ) 
        {
        iSecondaryLine->CallIsActive( secondaryLineActive );  
        if ( secondaryLineActive )
            {
            aCallIsActive = ETrue;
            }
        }
    //////////////

    return KErrNone;
    }


//Called by observers wanting to receive line status change notifications  
//(e.g CCtControlContainer)
void CLogsCallStatus::SetObserver( MLineStatusObserver* aObserver )
    {
    /// composite objects ////
    if( iDataLine ) 
        {
        iDataLine->SetObserver( aObserver );
        }
    if( iSecondaryLine ) 
        {
        iSecondaryLine->SetObserver( aObserver );
        }
    ////////////////////////
        
    if( aObserver )
        {
        Cancel();
        iLine.NotifyMobileLineStatusChange( iStatus, iLineStatus );
        SetActive();
    	iObserver = aObserver;
        }
    else
        {
        Cancel();
        iObserver = aObserver;
        }
    }

//Returns duration of an ongoing call (in seconds)
TInt CLogsCallStatus::ActiveCallDuration(TTimeIntervalSeconds& aCallDuration)
    {
    TInt ret( KErrNone );

    ret = iLine.GetMobileLineStatus( iLineStatus );

    if( ret == KErrNone && iLineStatus == RMobileCall::EStatusConnected )
        { 
        switch( iSelectedLine )
            {
            case ECallLine:// FALLTHROUGH
            case EDataLine:
                {
                
                TInt callCount;
                ret = iLine.EnumerateCall( callCount );
                if( ret || callCount == 0)
                    {
                    break;
                    }
                for(TInt i = 0; i < callCount; i++)
                    {
                    RLine::TCallInfo lineInfo;
                    ret = iLine.GetCallInfo( i , lineInfo );
                    if( ret )
                        {
                        break;
                        }

                    if( lineInfo.iStatus == RCall::EStatusConnected )
                        {
                        RMobileCall call;
                        ret = call.OpenExistingCall( iLine, lineInfo.iCallName );
                        if( ret )
                            {
                            call.Close();
                            break;
                            }

                        RMobileCall::TMobileCallInfoV1 callInfo;
                        RMobileCall::TMobileCallInfoV1Pckg callInfoPkg(callInfo);
                        ret = call.GetMobileCallInfo( callInfoPkg );
                        call.Close();
                        aCallDuration = callInfo.iDuration;
                        }
                    }
                }
                break;
            
            default:
                ret = KErrCorrupt;
                break;
            }
        }
    return ret;
    }

// End of file
