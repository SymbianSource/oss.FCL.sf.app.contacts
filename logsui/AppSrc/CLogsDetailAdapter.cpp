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
*     Adapts data from logsmodel to logs ui component (listbox) format
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <logcli.h>     // clogclient
#include <eiklabel.h>   // ceiklabel
#include <AknUtils.h>
#include <aknlists.h>   //caknsingleheadingstylelistbox

#include <Logs.rsg>     // detail view's event type texts.

#include "CLogsDetailAdapter.h"
#include "CLogsDetailView.h"
#include "MLogsEventGetter.h"
#include "MLogsGetEvent.h"
#include "CPhoneNumberFormat.h"

#include "LogsConstants.hrh"
#include "LogsConsts.h"
#include "CLogsEngine.h"
#include <LogsApiConsts.h>  //additional event uids

// CONSTANTS
//_LIT(KPanic,"LogsDetailAdapter");

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsDetailAdapter::CLogsDetailAdapter
// ----------------------------------------------------------------------------
//
CLogsDetailAdapter::CLogsDetailAdapter( 
    CAknSingleHeadingStyleListBox* aListBox, 
    CLogsDetailView* aView  ) :
        iEngine( aView->Engine()), 
        iGetter( aView->Engine()->GetEventL()), 
        iListBox( aListBox ), 
        iView( aView )
    {
    }
    
// ----------------------------------------------------------------------------
// CLogsDetailAdapter::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsDetailAdapter::ConstructL()
    {
    CEikonEnv* env = CEikonEnv::Static();
    iDirection = env->AllocReadResourceL(R_LOGS_DETAIL_DIRECTION_TEXT);
    iType = env->AllocReadResourceL(R_LOGS_DETAIL_TYPE_TEXT);
    iStatus  = env->AllocReadResourceL(R_LOGS_DETAIL_STATUS_TEXT);
    iDuration = env->AllocReadResourceL(R_LOGS_DETAIL_DURATION_TEXT);
    iAmount = env->AllocReadResourceL(R_LOGS_DETAIL_NUMBER_TEXT);

    iSentAmount = env->AllocReadResourceL(R_LOGS_DETAIL_SENT_TEXT);
    iReceivedAmount = env->AllocReadResourceL(R_LOGS_DETAIL_RECEIVED_TEXT);

    iAmountKB = env->AllocReadResourceL(R_LOGS_DETAIL_VIEW_KB_TEXT);
    iNbr = env->AllocReadResourceL(R_LOGS_DETAIL_TELNO_TEXT);
    iUnknownNumber = env->AllocReadResourceL(R_DLOGS_DETAILS_UNKNOWN_NUMBER);

    iDirectionIn = env->AllocReadResourceL(R_LOGS_DIR_IN_TEXT);
    iDirectionOut = env->AllocReadResourceL(R_LOGS_DIR_OUT_TEXT);
    iDirectionMissed = env->AllocReadResourceL(R_LOGS_DIR_MISSED_TEXT);
    iEventTypeVoice = env->AllocReadResourceL(R_LOGS_ET_CALL_TEXT);
    iEventTypeSMS = env->AllocReadResourceL(R_LOGS_ET_SMS_TEXT);
    iEventTypeMMS = env->AllocReadResourceL(R_LOGS_ET_MMS_TEXT);
    iEventTypeData = env->AllocReadResourceL(R_LOGS_ET_DATA_TEXT);
    iEventTypeGPRS = env->AllocReadResourceL(R_LOGS_ET_PACKET_TEXT); 
    iEventTypeWLAN = env->AllocReadResourceL(R_LOGS_ET_WLAN_TEXT); 
    iEventTypeFax = env->AllocReadResourceL(R_LOGS_ET_FAX_TEXT);
    iEventTypeVideo = env->AllocReadResourceL(R_LOGS_ET_VIDEO_TEXT);

    iEventTypePoC = env->AllocReadResourceL(R_LOGS_ET_POC_TEXT);
    iEventTypePoCInfo = env->AllocReadResourceL(R_LOGS_ET_POC_INFO_TEXT);    
    
    iEventTypeVoIP = env->AllocReadResourceL(R_LOGS_ET_VOIP_TEXT);

    iPoCAddr = env->AllocReadResourceL(R_LOGS_ET_POC_ADDR);
    iVoIPCallFrom = env->AllocReadResourceL(R_LOGS_ET_VOIP_CALL_FROM);
    iVoIPCallTo = env->AllocReadResourceL(R_LOGS_ET_VOIP_CALL_TO);

    //Read strings from Symbian Log engine
    CLogClient* logClient = iEngine->CLogClientRef();   
    logClient->GetString( iEventStatusPendingTxt, R_LOG_DEL_PENDING );
    logClient->GetString( iEventStatusSentTxt, R_LOG_DEL_SENT );
    logClient->GetString( iEventStatusFailedTxt, R_LOG_DEL_FAILED );
    logClient->GetString( iEventStatusNoDeliveryTxt, R_LOG_DEL_NONE );
    logClient->GetString( iEventStatusDeliveredTxt, R_LOG_DEL_DONE );
    logClient->GetString( iEventStatusNotSentTxt, R_LOG_DEL_NOT_SENT );

    iEventStatusDelivered = env->AllocReadResourceL(
                                            R_LOGS_STATUS_DELIVERED_TEXT);
    iEventStatusPending = env->AllocReadResourceL(R_LOGS_STATUS_PENDING_TEXT);
    iEventStatusFailed = env->AllocReadResourceL(R_LOGS_STATUS_FAILURE_TEXT);
    iEventStatusSent = env->AllocReadResourceL(R_LOGS_STATUS_SENT_TEXT);
    iDurationFormat = env->AllocReadResourceL(R_QTN_TIME_DURAT_LONG_WITH_ZERO);

    iNumberFirstRow = HBufC::NewL( KLogsSipUriMaxLen );
    iNumberSecondRow = HBufC::NewL( KLogsSipUriMaxLen );

    iPhoneNbrFormatter = CPhoneNumberFormat::NewL();
    LineFont();
    iTextWidth = 0;  
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::NewL
// ----------------------------------------------------------------------------
//
CLogsDetailAdapter* CLogsDetailAdapter::NewL( 
    CAknSingleHeadingStyleListBox* aListBox, 
    CLogsDetailView* aView )  
    {
    CLogsDetailAdapter* self = new (ELeave) CLogsDetailAdapter( aListBox, aView ); 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::~CLogsDetailAdapter
// ----------------------------------------------------------------------------
//
CLogsDetailAdapter::~CLogsDetailAdapter()
    {
    delete iDirection;
    delete iType;
    delete iStatus;
    delete iDuration;
    delete iAmount;
    delete iAmountKB;
    delete iNbr;
    delete iUnknownNumber;
    delete iDirectionIn;
    delete iDirectionOut;
    delete iDirectionMissed;

    delete iEventTypeVoice;
    delete iEventTypeSMS;
    delete iEventTypeMMS;
    delete iEventTypeData;
    delete iEventTypeGPRS;
    delete iEventTypeWLAN;
    delete iEventTypeFax;
    delete iEventTypeVideo; 

    delete iEventTypePoC; 
    delete iEventTypePoCInfo;     
    delete iEventTypeVoIP; 
    delete iPoCAddr;
    delete iVoIPCallFrom;
    delete iVoIPCallTo;

    delete iEventStatusDelivered;
    delete iEventStatusPending;
    delete iEventStatusFailed;
    delete iEventStatusSent;
           
    delete iDurationFormat;
    
    delete iNumberFirstRow;
    delete iNumberSecondRow;
    delete iSentAmount;
    delete iReceivedAmount;
    delete iPhoneNbrFormatter;
    
    delete iDisplayRows;    
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::UpdateL
// ----------------------------------------------------------------------------
//
void CLogsDetailAdapter::UpdateL() 
    {
    iDisplayRowNumber = 0;
    
    const MLogsEventGetter* event = iEngine->GetEventL()->Event();  
    const CLogEvent* logDbEvent = iEngine->GetEventL()->LogEvent();  

    CDesCArrayFlat* newDisplayRows = new ( ELeave ) CDesCArrayFlat( 10 );
    CleanupStack::PushL( newDisplayRows );

    //1) Rows 1 and 2 show always direction and type of event
    DirectionRow( event );
    newDisplayRows->AppendL( iBuffer );
    iDisplayRowNumber++;

    TypeRow( event );
    newDisplayRows->AppendL( iBuffer );
    iDisplayRowNumber++;

    //2) Contents of rows 3-> depend on event type (and available data)
    //Status row of message from CLogEvent (only for sms/mms)    
    if( StatusRow( logDbEvent, event ) ) 
        {
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        }
    
    //Duration. For missed and Poc calls shouldn't exist (=0), for other calls and wlan+gprs
    //duration shown if non-zero in the event (e.g. for gprs, duration is not updated
    //until context is closed, so first we don't show duration). For MO calls show even if zero.
    TBool showDuration( EFalse );
    
    if( (( event->EventUid().iUid == KLogCallEventType || //For MO voip and MO cs 
           event->LogsEventData()->VoIP() ) &&            //show duration even when
           event->Direction() == EDirOut ))               //it is zero
        {
        showDuration = ETrue;
        }
    else if( logDbEvent->Duration() > 0 )                 //For other show if available
        {
        showDuration = ETrue;
        }
        
    if( showDuration &&
        !event->LogsEventData()->PoC() &&   //Exclude Poc, missed calls, messages just in case would 
        event->Direction() != EDirMissed && //wrongly contain garbage in duration (should never happen)
        event->EventUid().iUid != KLogShortMessageEventType &&
        event->EventUid().iUid != KLogsEngMmsEventType )
        {
        DurationRow( logDbEvent );
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        }

    //Size. Number of message pdu's from CLogEvent (only for sms/mms)
    if( AmountRow( event )) //logDbEvent ) ) 
        {
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        }

    //Sent data (gprs, wlan, poc, voip)
    if( event->LogsEventData()->DataSent() > 0  ||          //voip, poc, only if > 0
        event->EventUid().iUid == KLogsEngWlanEventType ||  //for wlan & gprs show also zero 
        event->EventUid().iUid == KLogPacketDataEventType )
        {
        SentAmountRow( event );
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        }
        
    //Received data (gprs, wlan, poc, voip)
    if( event->LogsEventData()->DataReceived() > 0 ||
        event->EventUid().iUid == KLogsEngWlanEventType ||  
        event->EventUid().iUid == KLogPacketDataEventType )
        {
        ReceivedAmountRow( event );
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        }

    //Number. Show if available, however for emergency calls not shown
    if( iView->PhoneNumberAvailable( event ) && event->EventType() != ETypeEmerg)      
        {
        TBool split = SplitNumber( event, *iListboxFont, iTextWidth );
        TelRow( event );
        newDisplayRows->AppendL( iBuffer );
        iDisplayRowNumber++;
        
        if( split )
            {
            TelRow( event, ESecondRow );
            newDisplayRows->AppendL( iBuffer );
            iDisplayRowNumber++;
            }
        }

    //Uri. Show if available 
    if( iView->SipUriAvailable( event ))       
        {
        if( event->Direction() == EDirIn || event->Direction() == EDirMissed )    
            {
            event->LogsEventData()->VoIP() ? iBuffer.Copy( iVoIPCallFrom->Des() ) :
                                             iBuffer.Copy( iPoCAddr->Des() );
            }
        else 
            {
            event->LogsEventData()->VoIP() ? iBuffer.Copy( iVoIPCallTo->Des() ) :
                                             iBuffer.Copy( iPoCAddr->Des() );
            }
        iBuffer.Append( KTab );            
        
        TBool split = SplitNumber( event, *iListboxFont, iTextWidth, ETrue );            
        iBuffer.Append( *iNumberFirstRow );
        newDisplayRows->AppendL( iBuffer );                        
        iDisplayRowNumber++;

        if( split )
            {
            iBuffer.Copy( KTab );
            iBuffer.Append( *iNumberSecondRow );
            newDisplayRows->AppendL( iBuffer );
            iDisplayRowNumber++;
            }
        }

    //Update display array
    CleanupStack::Pop( newDisplayRows );            
    delete iDisplayRows;
    iDisplayRows = newDisplayRows; 
    }


// ----------------------------------------------------------------------------
// CLogsDetailAdapter::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CLogsDetailAdapter::MdcaCount() const
    {  
    return iDisplayRowNumber;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16 CLogsDetailAdapter::MdcaPoint( TInt aIndex ) const
    {
    return iDisplayRows->MdcaPoint( aIndex );
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::StatusRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::StatusRow( const CLogEvent* aCLogEvent,
                                     const MLogsEventGetter* aEvent ) const
    {
    TBool rc( EFalse );
    
    //Skip events that are not MO originated
    if( aEvent->Direction() != EDirOut &&
        aEvent->Direction() != EDirOutAlt ) 
        {
        return rc;
        }
    
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );

    des.Copy( iStatus->Des() );
    des.Append( KTab );

    switch( aCLogEvent->EventType().iUid )
        {
        case KLogShortMessageEventType:   
        case KLogsEngMmsEventType:
            {
            if( aCLogEvent->Status() == iEventStatusFailedTxt ||
                aCLogEvent->Status() == iEventStatusNotSentTxt )
                {
                des.Append( iEventStatusFailed->Des() );
                }
            else if( aCLogEvent->Status() == iEventStatusSentTxt )
                {
                des.Append( iEventStatusSent->Des() );
                }
            else if( aCLogEvent->Status() == iEventStatusDeliveredTxt )
                {
                des.Append( iEventStatusDelivered->Des() );
                }
            else
                {
                // Show "Pending" for other statuses: iEventStatusPendingTxt, 
                // iEventStatusNoDeliveryTxt, ... 
                des.Append( iEventStatusPending->Des() );
                }
            rc = ETrue;

            break;
            }
        default:
            break;
        } // switch

    return rc;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::DurationRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::DurationRow( const CLogEvent* aEvent ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );
    des.Copy( iDuration->Des() );
    des.Append( KTab );

    TInt32 duration( aEvent->Duration() );
    TInt64 seconds = MAKE_TINT64( 0, duration );  //TInt64 seconds( 0, duration );
    TTime time = seconds * 1000000;
        
    TDateTime dateTime = time.DateTime();
    
    if ( dateTime.Day() > 99 )
        {
        return EFalse;
        }

    if ( dateTime.Day() > 0 )
        {
        TBuf<KLogsDetailsListAdaptorArrayLen> buf;
        buf.AppendNum( dateTime.Day() );
        AknTextUtils::LanguageSpecificNumberConversion( buf ); 
        des.Append( buf );
        des.Append(KSpace);
        }
        
    TRAPD( err, TimeFormattingL( time ) );
    
    if( err )
        {
        des.Append( KSpace ); 
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::TimeFormattingL
// ----------------------------------------------------------------------------
//
void CLogsDetailAdapter::TimeFormattingL( TTime& aTime ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );
    TBuf<KLogsPhoneNumberMaxLen> buffer;
    aTime.FormatL( buffer, *iDurationFormat );
    AknTextUtils::LanguageSpecificNumberConversion( buffer ); 
    des.Append( buffer );
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::AmountRow
//
// Size row for SMS and MMS events 
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::AmountRow( const MLogsEventGetter* aEvent ) const
    {
    TBool rc( EFalse );
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );
    des.Copy( iAmount->Des() );
    des.Append( KTab );

    TBuf<KLogsPhoneNumberMaxLen> buf;
    buf.Zero();

    switch( aEvent->EventUid().iUid )
        {
        case KLogShortMessageEventType:   
        case KLogsEngMmsEventType:
            {
            buf.AppendNum( aEvent->LogsEventData()->MsgPartsNumber() );
            AknTextUtils::LanguageSpecificNumberConversion(buf);
            des.Append( buf );
            rc = ETrue;
            break;
            }
        default:
            break;
        } // switch

    return rc;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::TelRow
//
// Tel no for certain types of rows   
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::TelRow( const MLogsEventGetter* aEvent,
                                    TRowNumber aRowNumber ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );

    if( aRowNumber == EFirstRow )
        {
        des.Copy( iNbr->Des() );
        des.Append( KTab );
        switch( aEvent->EventType() )
            {
            //For unknown numbers we show "Tel no Unknown" row.
            case ETypeUnknown:
                des.Append( *iUnknownNumber );
                break;

			//For Private and Payphone numbers we don't show the "Tel no" row at all.. 

            case ETypeUsual:
                des.Append( *iNumberFirstRow );
                break;

            default:
                break;
            }
        }
    else
        {
        des.Copy( KTab );
        des.Append( *iNumberSecondRow );
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::SplitNumber
//
// If number is too long to fit to one row, we split it to two rows
// Sets iNumberSplitted to ETrue (+Returns true) if number was splitted
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::SplitNumber( 
    const MLogsEventGetter* aEvent,
    const CFont& aFont, 
    TInt aMaxWidthInPixels,
    TBool aPreferUri ) const
    {
    MUTABLE_CAST( CLogsDetailAdapter*, this )->iNumberSplitted = EFalse;
    TPtr firstRow( iNumberFirstRow->Des() );
    TPtr secondRow( iNumberSecondRow->Des() );
    firstRow.Zero();
    secondRow.Zero();

    TBuf<KLogsSipUriMaxLen> clippedText;

    if( iView->PhoneNumberAvailable( aEvent ) && !aPreferUri )  
        {
        TBuf<KLogsPhoneNumberMaxLen> tmp;
        iPhoneNbrFormatter->DTMFStrip( *(aEvent->Number()), tmp );   
        iPhoneNbrFormatter->PhoneNumberFormat( tmp, clippedText );
        }
    else if( iView->SipUriAvailable( aEvent ) && aPreferUri )   //If no number, show instead url if available            
        {
        iEngine->ConvertToUnicode( aEvent->LogsEventData()->Url(), clippedText );
        }
    else
        {
        clippedText.Zero();
        }

    TInt textPixels( aFont.TextWidthInPixels( clippedText ) );

    if ( textPixels <= aMaxWidthInPixels )                       //Fits to one row
        {
        firstRow.Copy( clippedText );
        return EFalse;
        }

    //Two rows needed
    MUTABLE_CAST( CLogsDetailAdapter*, this )->iNumberSplitted = ETrue;
    TInt excessPixels;
    TInt cutOff( aFont.TextCount( clippedText, aMaxWidthInPixels, excessPixels ) );
    
    //Does the remaining fit fully to two rows
    if ( textPixels + excessPixels <= aMaxWidthInPixels * 2 ||  //Phone number fits to two rows
         aPreferUri )                                           //or we use sip-uri (can be right-clipped on 2nd row)
        {
        secondRow.Copy( clippedText.Mid( cutOff ) );            // second row
        firstRow.Copy( clippedText.Left( cutOff ) );            // first row
        return ETrue;
        }
    
    //Phone number did not fit to two rows, so we need cut the first row from the beginning
    //We have to first reverse the string because we are interested in length of characters *after*
    //cutoff point (non-monospaced font, so chars are not necessarily equally wide on both sides of cutoff)
    Reverse( clippedText );
    secondRow.Copy( clippedText.Left( cutOff ) );    // second row
    Reverse( secondRow ); //convert back to normal order
    TPtrC remainingChars(clippedText.Mid(cutOff) );//first row    

    _LIT(KDots, "...");
    TInt acceptedWidth = aMaxWidthInPixels - aFont.TextWidthInPixels( KDots );

    //Create the first row in reversed order and then convert it back to normal order
    TInt cutOff2( aFont.TextCount( remainingChars, acceptedWidth ));
    firstRow.Append( remainingChars.Left( cutOff2 ) );
    if( firstRow.Length() + 4 < firstRow.MaxLength() )    //Magic number 4=length of KDots + length of Directionality Marker
        {
        firstRow.Append( 0x202A );   //A&H: In mirrored layout we need this Directionality Marker to render the three dots 
                                     //     correctly on left side of the number string. Similar issue as in SAKA-6689Q4.
        firstRow.Append( KDots );                                                                 
        }
    Reverse( firstRow );//convert back to normal order

    return ETrue;
    }


// ----------------------------------------------------------------------------
// CLogsDetailAdapter::DirectionRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::DirectionRow( const MLogsEventGetter* aEvent ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );

    des.Copy( iDirection->Des() );
    des.Append( KTab );

    switch( aEvent->Direction() )
        {
        case EDirIn:
            des.Append( iDirectionIn->Des() );
            break;
        case EDirOut:
            des.Append( iDirectionOut->Des() );
            break;
        case EDirMissed:
            des.Append( iDirectionMissed->Des() );
            break;
        default:
            break;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::TypeRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::TypeRow( const MLogsEventGetter* aEvent ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&,
                              iBuffer );

    des.Copy( iType->Des() );
    des.Append( KTab );

     switch( aEvent->EventUid().iUid )
        {
        case KLogCallEventType:
            //Currently Voice, VT, PoC and VoIP are shown as mutually exclusive. This may change in the future, e.g.
            //VoIP and VT types may be both in the same event 
            if( aEvent->LogsEventData()->VT() )      
                {
                des.Append( iEventTypeVideo->Des() );
                }
            else if( aEvent->LogsEventData()->PoC() )
                {
                des.Append( iEventTypePoC->Des() );
                }
            else if( aEvent->LogsEventData()->VoIP() )
                {
                des.Append( iEventTypeVoIP->Des() );
                }
            else
                {
                des.Append( iEventTypeVoice->Des() );
                }

            break;

        case KLogsEngPocInfoEventType:            
            des.Append( iEventTypePoCInfo->Des() );
            break;

        case KLogDataEventType:            
            des.Append( iEventTypeData->Des() );
            break;

        case KLogFaxEventType:             
            des.Append( iEventTypeFax->Des() );
            break;
                
        case KLogShortMessageEventType:           
            des.Append( iEventTypeSMS->Des() );
            break;

        case KLogsEngMmsEventType:
            des.Append( iEventTypeMMS->Des() );
            break;
               
        case KLogPacketDataEventType:             
            des.Append( iEventTypeGPRS->Des() );
            break;

        case KLogsEngWlanEventType:
            des.Append( iEventTypeWLAN->Des() );
            break;

        default:
            break;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::SentAmountRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::SentAmountRow( const MLogsEventGetter* aEvent ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&, iBuffer );

    des.Copy( iSentAmount->Des() ); 
    des.Append( KTab );

    TBuf<KLogsPhoneNumberMaxLen> buf;
    buf.Zero();
    
    buf.Format( *iAmountKB, I64INT(aEvent->LogsEventData()->DataSent() / 1024) ); //iAmountKB: R_LOGS_DETAIL_VIEW_KB_TEXT
    AknTextUtils::LanguageSpecificNumberConversion( buf ); 
    des.Append( buf );
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::ReceivedAmountRow
// ----------------------------------------------------------------------------
//
TBool CLogsDetailAdapter::ReceivedAmountRow( const MLogsEventGetter* aEvent ) const
    {
    TDes& des = MUTABLE_CAST( TBuf<KLogsDetailsListAdaptorArrayLen>&, iBuffer );

    des.Copy( iReceivedAmount->Des() );
    des.Append( KTab );

    TBuf<KLogsPhoneNumberMaxLen> buf;
    buf.Zero();

    buf.Format( *iAmountKB, aEvent->LogsEventData()->DataReceived() / 1024 ); //iAmountKB: R_LOGS_DETAIL_VIEW_KB_TEXT
    AknTextUtils::LanguageSpecificNumberConversion( buf ); 
    des.Append( buf );
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::SetLineWidth
//
// Sets the maximum width in pixels for the detail line. This needs to be called from controller's
// SizeChanged() method.
// ----------------------------------------------------------------------------
//
void CLogsDetailAdapter::SetLineWidth(TInt aTextWidth)
    {
    iTextWidth = aTextWidth;
    iListboxFont = LineFont();  //Update font too as probably it has also changed.
    }
    
// ----------------------------------------------------------------------------
// CLogsDetailAdapter::LineFont
// ----------------------------------------------------------------------------
//
const CFont* CLogsDetailAdapter::LineFont()
    {
    TAknTextLineLayout line = AknLayout::List_pane_texts__single_heading__Line_2(0 );  //Same in CLogsDetailControlContainer::SizeChanged
    TAknLayoutText text;
    text.LayoutText( iListBox->Rect(), line );  
    return text.Font();   // text.Font() not owned
    } 

// ----------------------------------------------------------------------------
// CLogsDetailAdapter::Reverse
// ----------------------------------------------------------------------------
//
void CLogsDetailAdapter::Reverse(TDes& aText ) const
    {
    for(int i=0,j=aText.Length() - 1; i < j ; i++, j--)
        {
        TUint16 tmp = aText[i];
        aText[i] =  aText[j];
        aText[j] = tmp;
        }
    }

//  End of File  
