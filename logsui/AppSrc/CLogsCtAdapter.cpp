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
*     Adaptor to EventList, copies data from model listbox
*
*/


// INCLUDE FILES
#include <eikenv.h>

#include <logs.rsg>
#include <AknUtils.h>	// AknTextUtils

#include "CLogsCtAdapter.h"
#include "MLogsSharedData.h"

#include "MLogsCallStatus.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT(KFormat,"%02d%c%02d%c%02d");
_LIT(KActiveCallIcon,"0");
_LIT(KLastCallIcon,"1");
_LIT(KDialledCallIcon,"2");
_LIT(KReceivedCallIcon,"3");
_LIT(KAllCallIcon,"4");
_LIT(KTab,"\t");
const TInt KTimeLen = 20;       //Same as in CCtControlContainer.cpp
const TInt KMaxValueOfTheTimer = 359999;
const TInt KCtAdapterMdcaCount = 4;
_LIT(KPanicMsg,"CLogsCtAdapter");
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES



// ----------------------------------------------------------------------------
// LanguageConversionAndAppend
//
// Static
// ----------------------------------------------------------------------------
//
TInt LanguageConversionAndAppend( 
    HBufC* aBuffer, 
    TDes& aDestDes )
    {
    TPtr ptr = aBuffer->Des();

    AknTextUtils::LanguageSpecificNumberConversion(ptr);
    if( ptr.Length() > aDestDes.MaxLength() )
        {
        aDestDes.Append( ptr.Left( aDestDes.MaxLength() ) );
        return KErrOverflow;
        }
    aDestDes.Append( ptr );
    return KErrNone;
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsCtAdapter::NewL
// ----------------------------------------------------------------------------
//
CLogsCtAdapter* CLogsCtAdapter::NewL(
    MLogsSharedData* aSharedData, 
    const TAlsEnum& aSelectedAlsLine,
    MLogsCallStatus& aCallStatus )
    {
    CLogsCtAdapter* self = new( ELeave ) 
                                CLogsCtAdapter( 
                                    aSharedData, 
                                    aSelectedAlsLine,
                                    aCallStatus);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ----------------------------------------------------------------------------
// CLogsCtAdapter::CLogsCtAdapter
// ----------------------------------------------------------------------------
//
CLogsCtAdapter::CLogsCtAdapter( 
    MLogsSharedData* aSharedData, 
    const TAlsEnum& aSelectedAlsLine,
    MLogsCallStatus& aCallStatus) :
        iSharedData( aSharedData ),
        iSelectedAlsLine( aSelectedAlsLine ),
        iCallStatus( aCallStatus )
    {
    }


// ----------------------------------------------------------------------------
// CLogsCtAdapter::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsCtAdapter::ConstructL()
    {
    CEikonEnv* env = CEikonEnv::Static();

    iActiveCallText = env->AllocReadResourceL( R_AOCCT_ACTIVE_CALL_TIMER_TEXT );
    iLastCallText = env->AllocReadResourceL( R_AOCCT_LAST_CALL_TIMER_TEXT );
    iDialledCallsText = env->AllocReadResourceL( R_AOCCT_DIALLED_CALLS_TIMER_TEXT );
    iReceivedCallsText = env->AllocReadResourceL( R_AOCCT_RECEIVED_CALLS_TIMER_TEXT );
    iAllCallsText = env->AllocReadResourceL( R_AOCCT_ALL_CALLS_TIMER_TEXT );       
    iActiveCallTime = HBufC::NewL (KTimeLen);
    iLastCallTime = HBufC::NewL (KTimeLen);
    iDialledCallsTime = HBufC::NewL (KTimeLen);
    iReceivedCallsTime = HBufC::NewL (KTimeLen);
    iAllCallsTime = HBufC::NewL (KTimeLen);

    //These need to have non-zero initial values
    iPreviousActiveTime = KErrNotFound;
    iPreviousAllCallsTime = KErrNotFound;
    
    //Local variation setting for showing/not showing active call duration
    iShowCallDurationLogs = iSharedData->ShowCallDurationLogsL();
    }
    

// ----------------------------------------------------------------------------
// CLogsCtAdapter::~CLogsCtAdapter
// ----------------------------------------------------------------------------
//
CLogsCtAdapter::~CLogsCtAdapter()
    {
    delete iActiveCallText;
    delete iLastCallText;
    delete iDialledCallsText;
    delete iReceivedCallsText;
    delete iAllCallsText;
    delete iActiveCallTime; 
    delete iLastCallTime; 
    delete iDialledCallsTime; 
    delete iReceivedCallsTime;
    delete iAllCallsTime; 

    }


// ----------------------------------------------------------------------------
// CLogsCtAdapter::MdcaCount
// ----------------------------------------------------------------------------
//
TInt  CLogsCtAdapter::MdcaCount () const 
    {
    return KCtAdapterMdcaCount;
    }


// ----------------------------------------------------------------------------
// CLogsCtAdapter::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16  CLogsCtAdapter::MdcaPoint( TInt aIndex ) const
    {
    TDes& des = CONST_CAST( TBuf<KLogsCtAdapterLen>&,
                              iBuffer );
    des.Zero();
    switch( aIndex )
        { // "0\tFirstLine\tSecondLine"
        case 0:
            {
            TBool activeCall( EFalse );
            iCallStatus.CallIsActive( activeCall );
            
            //Local variation / active call duration:
            //Show active call duration only if active call is ongoing and we are also requested to 
            // show the active call duration. Otherwise we show only the last ended call's duration
            // also in cases in which we are in midst of one or more active call.
            if( activeCall && iShowCallDurationLogs )
                {   // Call is active and we are requested to show active call duration
                des.Append( KActiveCallIcon );
                des.Append( KTab );
                des.Append( *iActiveCallText );
                des.Append( KTab );
                LanguageConversionAndAppend( iActiveCallTime, des );
                }
            else
                {
                des.Append( KLastCallIcon );
                des.Append( KTab );
                des.Append( *iLastCallText );
                des.Append( KTab );
                LanguageConversionAndAppend( iLastCallTime, des );
                }
            break;
            }
        case 1:
            des.Append( KDialledCallIcon );
            des.Append( KTab );
            des.Append( *iDialledCallsText );
            des.Append( KTab );
            LanguageConversionAndAppend( iDialledCallsTime, des );
            break;
        case 2:
            des.Append( KReceivedCallIcon );
            des.Append( KTab );
            des.Append( *iReceivedCallsText );
            des.Append( KTab );
            LanguageConversionAndAppend( iReceivedCallsTime, des );
            break;
        case 3:
            des.Append( KAllCallIcon );
            des.Append( KTab );
            des.Append( *iAllCallsText );
            des.Append( KTab );
            LanguageConversionAndAppend( iAllCallsTime, des );
            break;
        default:
            User::Panic(KPanicMsg, KErrCorrupt);
            break;
        }

    return iBuffer;
    }

// ----------------------------------------------------------------------------
// CLogsCtAdapter::UpdateDataL
// ----------------------------------------------------------------------------
//
TBool CLogsCtAdapter::UpdateDataL()
    {
    TBool retVal = ETrue; //By default indicate that a timer value has changed

    TInt activeTime( ActiveTime() );  //Note! This retrieves only cellular call duration
    TInt allCallstime( iSharedData->AllCallsTimer( iSelectedAlsLine, EVoipIncluded) );

    //Format time to time strings
    ToTimeFormat( activeTime, iActiveCallTime );                    //Duration of active on-going call. NOTE! Only voice call supported
                                                                    // as we don't get anywhere information of ongoing voip call status.
                                                                    
    ToTimeFormat( iSharedData->LastCallTimer(                       //Duration of last already ended call (voice or voip)
                        iSelectedAlsLine, EVoipIncluded ), iLastCallTime  );  
    ToTimeFormat( iSharedData->DialledCallsTimer(
                        iSelectedAlsLine, EVoipIncluded ), iDialledCallsTime  );
    ToTimeFormat( iSharedData->ReceivedCallsTimer(
                        iSelectedAlsLine, EVoipIncluded ), iReceivedCallsTime  );
    ToTimeFormat( allCallstime, iAllCallsTime  );

    //Check has any value changed. If active or all calls time has changed from previous call,
    // then return true, otherwise false. KErrNotFound needed as initial values of members.
    if( activeTime == iPreviousActiveTime  && allCallstime == iPreviousAllCallsTime ) 
        {
        retVal = EFalse;    //No value has changed after previous call of this function.
        }

    iPreviousActiveTime = activeTime;
    iPreviousAllCallsTime = allCallstime;
    return retVal;
    }

TInt CLogsCtAdapter::ActiveTime()
    {
    TTimeIntervalSeconds secondsInterval;
    if( iCallStatus.ActiveCallDuration( secondsInterval ) ) //This retrieves only cellular call duration, see
        {                                                   // CLogsCallStatus in CLogsMMECallStatus.cpp 
        return 0;
        }
    return secondsInterval.Int();
    }

// ----------------------------------------------------------------------------
// CLogsCtAdapter::ToTimeFormat
//
// Converts seconds to proper time format
// ----------------------------------------------------------------------------
//
void CLogsCtAdapter::ToTimeFormat( 
    TInt aSeconds, 
    HBufC* aText )
    {
    TLocale locale;

    if( aSeconds > KMaxValueOfTheTimer )
        {
        TInt HowManyTimes( aSeconds / KMaxValueOfTheTimer );
        aSeconds = aSeconds - HowManyTimes * KMaxValueOfTheTimer;
        }

    TInt seconds( aSeconds % 60 );
    TInt hours( aSeconds / 3600 );
    TInt minutes( ( aSeconds - hours * 3600 ) / 60 );
    TInt hoursSeparator = locale.TimeSeparator( 1 );
    TInt minuteSeparator = locale.TimeSeparator( 2 );
    
    /// Localization of the time
    TPtr ptr = aText->Des();

    ptr.Format( KFormat, hours, hoursSeparator, minutes, minuteSeparator, seconds );
               
    }


// ----------------------------------------------------------------------------
// CLogsCtAdapter::LastCallTimeStringL
//
// Functions to provide formatted call time strings for container (called from CCtControlContainer)
// No need to call UpdateDataL() to get up-to-date values
// ----------------------------------------------------------------------------
//
TInt CLogsCtAdapter::LastCallTimeStringL( 
    TDes& aDes, 
    const TAlsEnum& aSelectedLine, 
    const TVoipEnum aVoip )  
    {
    HBufC* buf = HBufC::NewLC( KTimeLen );    
    ToTimeFormat( iSharedData->LastCallTimer( aSelectedLine, aVoip ), buf );   //Duration of last already ended call. 
    TInt ret = LanguageConversionAndAppend( buf, aDes );    
    CleanupStack::PopAndDestroy( buf );
    return ret;    
    }
    
// ----------------------------------------------------------------------------
// CLogsCtAdapter::ReceivedCallsTimeStringL
// ----------------------------------------------------------------------------
//
TInt CLogsCtAdapter::ReceivedCallsTimeStringL( 
    TDes& aDes, 
    const TAlsEnum& aSelectedLine, 
    const TVoipEnum aVoip )
    {
    HBufC* buf = HBufC::NewLC( KTimeLen );    
    ToTimeFormat( iSharedData->ReceivedCallsTimer( aSelectedLine, aVoip ), buf );
    TInt ret = LanguageConversionAndAppend( buf, aDes );    
    CleanupStack::PopAndDestroy( buf );
    return ret;    
    }

// ----------------------------------------------------------------------------
// CLogsCtAdapter::DialledCallsTimeStringL
// ----------------------------------------------------------------------------
//
TInt CLogsCtAdapter::DialledCallsTimeStringL( 
    TDes& aDes, 
    const TAlsEnum& aSelectedLine, 
    const TVoipEnum aVoip )
    {
    HBufC* buf = HBufC::NewLC( KTimeLen );    
    ToTimeFormat( iSharedData->DialledCallsTimer( aSelectedLine, aVoip ), buf );
    TInt ret = LanguageConversionAndAppend( buf, aDes );    
    CleanupStack::PopAndDestroy( buf );
    return ret;    
    }
   
// ----------------------------------------------------------------------------
// CLogsCtAdapter::AllCallsTimeStringL
// ----------------------------------------------------------------------------
//
TInt CLogsCtAdapter::AllCallsTimeStringL( 
    TDes& aDes, 
    const TAlsEnum& aSelectedLine, 
    const TVoipEnum aVoip )
    {
    HBufC* buf = HBufC::NewLC( KTimeLen );    
    ToTimeFormat( iSharedData->AllCallsTimer( aSelectedLine, aVoip ), buf );
    TInt ret = LanguageConversionAndAppend( buf, aDes );    
    CleanupStack::PopAndDestroy( buf );
    return ret;    
    }

   
   
   
//  End of File  
