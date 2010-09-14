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
*     Adapts data from Shared data GPRS counters to listbox
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <AknUtils.h>
#include <logs.rsg>

#include "CLogsGprsCtAdapter.h"
#include "MLogsSharedData.h"
#include "LogsConsts.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES


// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::NewL
// ----------------------------------------------------------------------------
//
CLogsGprsCtAdapter* CLogsGprsCtAdapter::NewL( MLogsSharedData* aSharedData )
    {
    CLogsGprsCtAdapter* self = new( ELeave ) CLogsGprsCtAdapter( aSharedData );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::CLogsGprsCtAdapter
// ----------------------------------------------------------------------------
//
CLogsGprsCtAdapter::CLogsGprsCtAdapter( 
    MLogsSharedData* aSharedData ) :
        iSharedData( aSharedData )
    {
    }


// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsGprsCtAdapter::ConstructL()
    {
    iSentTitle = StringLoader::LoadL( R_LOGS_GRPS_SENT_TEXT );
    iReceivedTitle = StringLoader::LoadL( R_LOGS_GRPS_RECEIVED_TEXT );
    iBytes = StringLoader::LoadL(  R_LOGS_GRPS_B_TEXT, KNullDesC );
    iKBytes = StringLoader::LoadL(  R_LOGS_GRPS_KB_TEXT, KNullDesC );
    iMBytes = StringLoader::LoadL(  R_LOGS_GRPS_MB_TEXT, KNullDesC );
    iGBytes = StringLoader::LoadL(  R_LOGS_GRPS_GB_TEXT, KNullDesC );
    }

    
// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::~CLogsGprsCtAdapter
// ----------------------------------------------------------------------------
//
CLogsGprsCtAdapter::~CLogsGprsCtAdapter()
    {
    delete iSentTitle;
    delete iReceivedTitle;
    delete iBytes;
    delete iKBytes;
    delete iMBytes;
    delete iGBytes;
    }


// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::MdcaCount
// ----------------------------------------------------------------------------
//
TInt  CLogsGprsCtAdapter::MdcaCount () const 
    {
    return KAmountOfGprsCounters;   
    }
    
// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16  CLogsGprsCtAdapter::MdcaPoint( TInt aIndex ) const
    {
    // double graphic style "0\tText\tText2\t1

    TDes& des = MUTABLE_CAST( TBuf<KLogsBuff128>&, iBuffer );
                                  
    // icon
    des.Num( aIndex );
    des.Append( KTab );

    switch( aIndex )
        {
        case 0:
            des.Append( *iSentTitle );
            des.Append( KTab );
            ConstructAmountDescriptor( EGprsDirectionSent );                                         
            break;

        case 1:
            des.Append( *iReceivedTitle );
            des.Append( KTab );
            ConstructAmountDescriptor( EGprsDirectionReceived );                                        
            break;

        default:
            break;
        }
    return des;
    }

// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::ConstructAmountDescriptor
// ----------------------------------------------------------------------------
//
void CLogsGprsCtAdapter::ConstructAmountDescriptor(
    TGprsDirectionType aDirection ) const
    {
    const TInt64 KBytesInKilobyte = MAKE_TINT64( 0, 1024 );      //const TInt64 KBytesInKilobyte( 1024 );
    const TInt64 KBytesInMegabyte = MAKE_TINT64( 0, 1048576 );   //const TInt64 KBytesInMegabyte( 1048576 );
    const TInt64 KBytesInGigabyte = MAKE_TINT64( 0, 1073741824 );//const TInt64 KBytesInGigabyte( 1073741824 );
    TInt64 bytes = MAKE_TINT64( 0, 0 );                          //TInt64 bytes( 0 );

    TDes& des = MUTABLE_CAST( TBuf<KLogsBuff128>&, iBuffer );

    // ask shared data for info
    switch( aDirection )
        {
        case EGprsDirectionSent:
            bytes = iSharedData->GprsSentCounter();
            break;

        case EGprsDirectionReceived:
            bytes = iSharedData->GprsReceivedCounter();
            break;

        default:
            break;
        }

    if( bytes < 0 )
        {
        bytes = 0;
        }

    if( bytes > ( KBytesInGigabyte - 1 ) )
        {
        CalcPercentage( bytes, KBytesInGigabyte );
        des.Append( *iGBytes );
        }
    else if( bytes > ( KBytesInMegabyte - 1 ) )
        {
        CalcPercentage( bytes, KBytesInMegabyte );
        des.Append( *iMBytes );
        }
    else if( bytes > ( KBytesInKilobyte - 1 ) )
        {
        CalcPercentage( bytes, KBytesInKilobyte );
        des.Append( *iKBytes );
        }
    else
        {
        TBuf<KLogsBuff128> buf;
        buf.AppendNum( bytes );
        AknTextUtils::LanguageSpecificNumberConversion( buf ); 
        des.Append( buf );
        des.Append( *iBytes );
        }

    }


// ----------------------------------------------------------------------------
// CLogsGprsCtAdapter::CalcPercentage
// ----------------------------------------------------------------------------
//
void CLogsGprsCtAdapter::CalcPercentage(
    TInt64 aDividend, 
    TInt64 aDivisor ) const
    {
    TLocale locale;
    TInt64 integer  = MAKE_TINT64( 0, 0 );  //TInt64 integer( 0 );
    TInt64 fraction = MAKE_TINT64( 0, 0 );  //TInt64 fraction( 0 );
    TInt64 percent  = MAKE_TINT64( 0, 0 );  //TInt64 percent( 0 );
    TBuf<KLogsBuff128> buf;

    

    integer = aDividend / aDivisor;
    buf.AppendNum( integer );

    fraction = aDividend % aDivisor;
    
    buf.Append( locale.DecimalSeparator() );  
    percent = ( fraction * KMaxPercentage ) / aDivisor;

    //buf.AppendNumFixedWidth( percent.Low(), EDecimal, KDigitsAfterDot ); [FIXME: remove this line]
    buf.AppendNumFixedWidth( I64LOW(percent), EDecimal, KDigitsAfterDot ); 

    AknTextUtils::LanguageSpecificNumberConversion( buf ); 

    TDes& des = MUTABLE_CAST( TBuf<KLogsBuff128>&, iBuffer );
    des.Append( buf );
        

    }
           
//  End of File  
