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
*     Adapter to EventList copies data from model listbox
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <eiklabel.h>   // ceiklabel
#include <AknUtils.h>   // akntextutils
#include <eikclb.h>     // ceikcolumnlistbox

#include <logs.rsg>

#include "CLogsEventListAdapter.h"
#include "CLogsEventListControlContainer.h"
#include "CLogsEventListView.h"
#include "MLogsSharedData.h"
#include "MLogsModel.h"
#include "MLogsEventGetter.h"
#include "CPhoneNumberFormat.h"

#include "LogsConstants.hrh"
#include <LogsApiConsts.h>  //additional event uids
#include "LogsConsts.h"


//[FIXME: Remove the following after mmsEventType is defined in Symbian include file]
#include "LogsEngConsts.h"  //klogsengmmseventtype

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KEventListSubCellId = 2;
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES


// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsEventListAdapter::NewL
//
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLogsEventListAdapter* CLogsEventListAdapter::NewL( CLogsEventListControlContainer* aContainer )
    {
    CLogsEventListAdapter* self = new( ELeave ) CLogsEventListAdapter( aContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsEventListAdapter::CLogsEventListAdapter
// ----------------------------------------------------------------------------
//
CLogsEventListAdapter::CLogsEventListAdapter( CLogsEventListControlContainer* aContainer ) : 
    iEngine( aContainer->View()->Engine() ),
    iModel( aContainer->View()->Engine()->Model( ELogsMainModel )),
    //FIXME: this can be removed as Phone takes care of leavin csd numbers unlogged if preferred                                         
    // iShowCsd( aContainer->View()->Engine()->SharedDataL()->ShowCsdNumber() ),
    iListBox( aContainer->ListBox() )
    {
    }

// ----------------------------------------------------------------------------
// CLogsEventListAdapter::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEventListAdapter::ConstructL()
    {
    CEikonEnv* env = CEikonEnv::Static();

    iBuffer = HBufC::NewL( KLogsBuff128 );
    iCallText = env->AllocReadResourceL( R_LOGS_ET_CALL_TEXT );
    iDataText = env->AllocReadResourceL( R_LOGS_ET_DATA_TEXT );
    iSMSText = env->AllocReadResourceL( R_LOGS_ET_SMS_TEXT );
    iMMSText = env->AllocReadResourceL( R_LOGS_ET_MMS_TEXT );
    iPacketText = env->AllocReadResourceL( R_LOGS_ET_PACKET_TEXT );
    iWLANText = env->AllocReadResourceL( R_LOGS_ET_WLAN_TEXT );
    iFaxText = env->AllocReadResourceL( R_LOGS_ET_FAX_TEXT);
    iVideoText = env->AllocReadResourceL( R_LOGS_ET_VIDEO_TEXT );
    iPoCText = env->AllocReadResourceL( R_LOGS_ET_POC_TEXT );
    iPoCTextInfo = env->AllocReadResourceL( R_LOGS_ET_POC_INFO_TEXT );    

    iVoIPText = env->AllocReadResourceL( R_LOGS_ET_VOIP_TEXT);

    iPrivateNumber = env->AllocReadResourceL( R_LOGS_CON_PRIVATE_NUM );
    iUnknownNumber = env->AllocReadResourceL( R_LOGS_CON_UNKNOWN_NUM ); 
    iPayphoneNumber = env->AllocReadResourceL( R_LOGS_CON_PAYPHONE_NUM );	 
    
    iSATNumber = env->AllocReadResourceL( R_LOGS_CON_OPER_SERV );
    iGroupCall = env->AllocReadResourceL( R_LOGS_CON_GROUP_CALL );
    iEmergencyCall = env->AllocReadResourceL( R_LOGS_EMERG_CALL );
    iCsdNumber = env->AllocReadResourceL( R_LOGS_DETAILS_CSD_NUMBER );

    iLabel = new (ELeave) CEikLabel();

    iPhoneNumber = CPhoneNumberFormat::NewL();
    }
    
// Destructor
// ----------------------------------------------------------------------------
// CLogsEventListAdapter::~CLogsEventListAdapter
// ----------------------------------------------------------------------------
//
CLogsEventListAdapter::~CLogsEventListAdapter()
    {
    delete iBuffer;
    delete iCallText;
    delete iDataText;
    delete iSMSText;
    delete iMMSText;
    delete iPacketText;
    delete iWLANText;
    delete iFaxText;
    delete iVideoText;
    delete iPoCText;
    delete iPoCTextInfo;    
    delete iVoIPText;

    delete iPrivateNumber;
    delete iUnknownNumber;
    delete iPayphoneNumber;    
    
    delete iSATNumber;
    delete iGroupCall; 
    delete iEmergencyCall;
    delete iCsdNumber;
    delete iLabel;

    delete iPhoneNumber;
    }


// ----------------------------------------------------------------------------
// CLogsEventListAdapter::MdcaCount
//
// from MDesCArray
// ----------------------------------------------------------------------------
//
TInt  CLogsEventListAdapter::MdcaCount () const 
    {
    return iModel->Count();   
    }
    
// ----------------------------------------------------------------------------
// CLogsEventListAdapter::MdcaPoint
//
// from MDesCArray
// ----------------------------------------------------------------------------
//
TPtrC16  CLogsEventListAdapter::MdcaPoint( TInt aIndex ) const
    {
    TPtr des( iBuffer->Des() );
    // Single Graphics Style "0\tHead\tText"
    // or single graphic heading style "0\tHead\tLabel\t1\t2"
        
    const MLogsEventGetter* event = iModel->At( aIndex );

    TLogsDirection dir = event->Direction();    
    
    des.Num( static_cast<TInt>( dir ) );
    des.Append( KTab );

    TUid uid;
    uid.iUid = event->EventUid().iUid;

    switch( uid.iUid )
        {
        case KLogCallEventType:

            //Currently Voice, VT, PoC and VoIP are shown as mutually exclusive. This may change in the future, e.g.
            //VoIP and VT types may beboth in the same event 
            if( event->LogsEventData()->VT() )
                {
                des.Append( iVideoText->Des() );
                }
            else if( event->LogsEventData()->PoC() )
                {
                des.Append( iPoCText->Des() );
                }
            else if( event->LogsEventData()->VoIP() )
                {
                des.Append( iVoIPText->Des() );
                }
            else
                {
                des.Append( iCallText->Des() );
                }
            break;
            
        case KLogsEngPocInfoEventType:            
            des.Append( iPoCTextInfo->Des() );
            break;
            
        case KLogDataEventType:   
            des.Append( iDataText->Des() );
            break;
            
        case KLogShortMessageEventType:   
            des.Append( iSMSText->Des() );
            break;
            
        case KLogsEngMmsEventType:   
            des.Append( iMMSText->Des() );
            break;
            
        case KLogPacketDataEventType:   
            des.Append( iPacketText->Des() );
            break;
            
        case KLogsEngWlanEventType:   
            des.Append( iWLANText->Des() );
            break;
            
        case KLogFaxEventType:   
            des.Append( iFaxText->Des() );
            break;
            
        default:
            break;
        } // switch

    des.Append( KTab );
    
    //if( ! iShowCsd && //FIXME: this can be removed as Phone takes care of leavin csd numbers unlogged if preferred
    if( !event->Number() && !event->RemoteParty() &&
        uid.iUid == KLogDataEventType &&
        ( dir == EDirOutAlt || dir == EDirOut ) )
        {
        // hide csd numbers if required
        des.Append( *iCsdNumber );
        }
    else
        {
        switch ( event->EventType() )
            {
            case ETypeUsual:
                if( event->RemoteParty() )
                    {
                    // Lets strip control chars like tabulator marks which would break 
                    // the listbox descriptor, see OJON-7CA9SU
                    TPtr remoteParty = event->RemoteParty()->Des();
                    AknTextUtils::StripCharacters(remoteParty, KAknStripListControlChars);
                    des.Append( remoteParty );  
                    }
                else if( event->Number() )
                    //We prefer showing phone number over sip uri although for making calls uri is preferred (if both
                    //available, Poc has made a "best guess" for corresponding msisdn for subsequent cs calls).
                    {
                    TBuf<KLogsPhoneNumberMaxLen> tmp;
                    TBuf<KLogsPhoneNumberMaxLen> clippedNumber;
                    
                    CPhoneNumberFormat::DTMFStrip( *(event->Number()), tmp );

                    iPhoneNumber->PhoneNumberFormat
                                    (   tmp
                                    ,   clippedNumber
                                    ,   AknTextUtils::EClipFromBeginning
                                    ,   iListBox
                                    ,   aIndex
                                    ,   KEventListSubCellId
                                    );

                    des.Append( clippedNumber );
                    }
                else if( event->LogsEventData()->Url().Length() > 0 ) 
                    {
                    TBuf<KLogsSipUriMaxLen> buf; 
                    
                    if ( iEngine->ConvertToUnicode( event->LogsEventData()->Url(), buf ) )
                        {
                        buf.Zero(); //Converting failed
                        }
                    else
                        {
                        AknTextUtils::StripCharacters(buf, KAknStripListControlChars);
                        des.Append( buf );                      
                        }
                    }
                break;

            case ETypePoCGroupCall:                
                if( event->RemoteParty() )
                    {
                    // Lets strip control chars like tabulator marks which would break 
                    // the listbox descriptor, see OJON-7CA9SU
                    TPtr remoteParty = event->RemoteParty()->Des();
                    AknTextUtils::StripCharacters(remoteParty, KAknStripListControlChars);
                    des.Append( remoteParty );  
                    }
                else                                //if no remote party data available, show
                    {                               //localised text "group call"
                    des.Append( *iGroupCall );  
                    }
                break;                
                       
            case ETypePrivate:
                des.Append( *iPrivateNumber );
                break;

            case ETypeUnknown:
                des.Append( *iUnknownNumber );
                break;
                
            case ETypePayphone:
                des.Append( *iPayphoneNumber );
                break;

            case ETypeSAT:
                if( event->RemoteParty()) 
                    { 
                    // sat event with contact
                    // Lets strip control chars like tabulator marks which would break 
                    // the listbox descriptor, see OJON-7CA9SU
                    TPtr remoteParty = event->RemoteParty()->Des();
                    AknTextUtils::StripCharacters(remoteParty, KAknStripListControlChars);
                    des.Append( remoteParty ); 
                    }
                else 
                    {
                    des.Append( *iSATNumber );
                    }
                break;

            case ETypeEmerg:
                des.Append( *iEmergencyCall ); 
                break;

            default:
                break;
            }
        }
     
    // Finally, lets replace paragraph delimiters with single white space, fix for EBWG-6ZPCMZ 
    TBuf<1> charsToRemove;
    charsToRemove.Append(TChar(TInt(CEditableText::EParagraphDelimiter)));
    AknTextUtils::ReplaceCharacters(des, charsToRemove, TChar(TInt(CEditableText::ESpace)));
    
    return *iBuffer;
    }

//  End of File  
