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
*     Adaptor to RecentModel, copies data from model listbox
*
*/


// INCLUDE FILES
#include <eiklabel.h>
#include <eikfrlbd.h>
#include <eikfrlb.h>
#include <Logs.rsg>
#include <gulicon.h>
#include <VPbkEng.rsg>
#include <PbkFields.hrh>

#include "CLogsRecentListAdapter.h"
#include "CLogsRecentListView.h"
#include "CLogsRecentListControlContainer.h"    
#include "CLogsEngine.h"    
#include "MLogsModel.h"
#include "MLogsEventGetter.h"
//For ring duation feature
#include "MLogsSharedData.h"
#include "MLogsUiControlExtension.h"
#include "CPhoneNumberFormat.h"

#include "LogsConstants.hrh"
#include "LogsConsts.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//const TInt KVtIconOffsetFromLastOwnIcon = 1;

//For ring duation feature
// One minute - 60 seconds
const TInt KLogsOneMinute = 60;

// String literal for zero
_LIT( KLogsOneZero, "0" );
 

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::NewL
// ----------------------------------------------------------------------------
//
CLogsRecentListAdapter* CLogsRecentListAdapter::NewL(
    CLogsRecentListControlContainer* aContainer,
    TInt aLastOwnIconOffset )
    {
    CLogsRecentListAdapter* self = new( ELeave ) CLogsRecentListAdapter(
        aContainer,        
        aLastOwnIconOffset );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::CLogsRecentListAdapter
// ----------------------------------------------------------------------------
//
CLogsRecentListAdapter::CLogsRecentListAdapter(
    CLogsRecentListControlContainer* aContainer,        
    TInt aLastOwnIconOffset ) :
        iModel( aContainer->View()->CurrentModel() ),
        iSharedData( *( aContainer->View()->Engine()->SharedDataL() )),
        iControlExtension( aContainer->ControlExtension() ),
        iListBox( aContainer->ListBox() ),
        iLastOwnIconOffset( aLastOwnIconOffset ),
        iEngine( aContainer->View()->Engine() )
    {
    }

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::ConstructL()
    {   
    CEikonEnv* env = CEikonEnv::Static();
    iDateResource = env->AllocReadResourceL( R_QTN_DATE_USUAL );
    iTimeResource = env->AllocReadResourceL( R_QTN_TIME_USUAL );

    iPrivateNumber = env->AllocReadResourceL(R_DLOGS_DETAILS_PRIVATE_NUMBER);
    iUnknownNumber = env->AllocReadResourceL(R_DLOGS_DETAILS_UNKNOWN_NUMBER);
    iPayphoneNumber = env->AllocReadResourceL(R_DLOGS_DETAILS_PAYPHONE_NUMBER);    
    
    iSATNumber = env->AllocReadResourceL( R_LOGS_CON_OPER_SERV );
    iGroupCall = env->AllocReadResourceL( R_LOGS_CON_GROUP_CALL );    
    iEmergencyCall = env->AllocReadResourceL( R_LOGS_EMERG_CALL );
    
    iBuffer = HBufC::NewL( KLogsBuff128 );
    iListboxFont = LineFont();

    iPhoneNumber = CPhoneNumberFormat::NewL();

    // Array for text widths for different resolutions. Initialize to zero 
    iTextWidth = new (ELeave) TInt[KMaxNbrOfRecentTrailIcons + 1];
    for(TInt trailIcons = 0; trailIcons <= KMaxNbrOfRecentTrailIcons; trailIcons++) 
        {
        iTextWidth[trailIcons] = 0;  
        }
    
    //For ring duation feature
    iShowRingDuration = iSharedData.ShowRingDuration();
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::~CLogsRecentListAdapter
// ----------------------------------------------------------------------------
//
CLogsRecentListAdapter::~CLogsRecentListAdapter()
    {
    delete[] iTextWidth;
    delete iBuffer;    
    delete iDateResource;  
    delete iTimeResource;
    delete iPrivateNumber;
    delete iUnknownNumber;
    delete iPayphoneNumber;
    delete iEmergencyCall;
    delete iSATNumber;
    delete iGroupCall;    
    delete iPhoneNumber;
    }


// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::DateTimeLocalizationL
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::DateTimeLocalizationL(
    const MLogsEventGetter* aEvent,
    TPtr& aPtr ) const
    {
    TTime time( aEvent->Time() );

    HBufC* dateOrTimeTextBuf = HBufC::NewLC( KLogsBuff128 );
    TPtr dateOrTimeText = dateOrTimeTextBuf->Des();

    // Date
    time.FormatL( dateOrTimeText, *iDateResource );

    // Arabic & Hebrew conversion if needed.
    AknTextUtils::LanguageSpecificNumberConversion(dateOrTimeText);
    aPtr.Append( dateOrTimeText );
    aPtr.Append(KSpace); 
        
    // Time
    time.FormatL( dateOrTimeText, *iTimeResource);

    // Arabic & Hebrew conversion if needed.
    AknTextUtils::LanguageSpecificNumberConversion(dateOrTimeText);
    aPtr.Append( dateOrTimeText );
    CleanupStack::PopAndDestroy( dateOrTimeTextBuf );
    }


// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::MdcaCount
// ----------------------------------------------------------------------------
//
TInt  CLogsRecentListAdapter::MdcaCount () const 
    {
    return iModel->Count();   
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16  CLogsRecentListAdapter::MdcaPoint( TInt aIndex ) const
    {
    TBuf<KNonClipDuplBufferLength> duplBuffer( KNullDesC );   
    TInt duplBufferPixels( 0 );

    // double graphic style "0\tText\tText2\t1
    TPtr ptr( iBuffer->Des() );
    const MLogsEventGetter* event = iModel->At( aIndex );


    /********************************************************************************
    //Show corresponding icon for this type of phonenumber. FieldId is in the 
    //event data, iIconIdArray contains corresponding iconId for this phonebook field.
    TInt iconIndex( KErrNotFound );

    TInt fldId = event->FieldId();
    if( fldId >= 0 && fldId <= iIconIdArray->Count() )
        {
        iconIndex = iIconIdArray->At( fldId );
        }
        
    //Default icon for phone number type. If no icon available for this field type, we use the 
    //second icon (offset 1) as default icon (offset 0 is empty icon).
    if( iconIndex == KErrNotFound && iIconIdArray->Count() >= 1 )
        {
        iconIndex = 1;  //Offset 1 icon used as default icon (offset 0 is empty icon)
        }
    ********************************************************************************/        

    //Trailing icons need to be loaded now that we know to leave enough room to them.
    //Max KMaxNbrOfRecentTrailIcons icons can be shown simultaneously 
    //on a line. Presence icon is always most right. Video telephony/ALS icon 
    //on the left side of presence icon. VT cannot be received to
    //alt line so VT and ALS are mutually exclusive
    TBuf<KRecentMaxTextLength> iconBuff(0);  //Initial size 0
    TInt numIcons(0);
    
    // ALS2 icon is last own icon in icon array      
    if( ( numIcons <= KMaxNbrOfRecentTrailIcons ) && // Append only if room for trailing icons
        ( iLastOwnIconOffset > 0 )			      && //				
        ( event->ALS() ) ) 							 // and the ALS event flag is set
          
        {
        iconBuff.Append( KTab );
        iconBuff.AppendNum( EIconAls );
        numIcons++;
        }
    
    
    // Show new missed call icon if the event is unread 
    // (set to read for received and dialed calls - see CLogsEvent.cpp)
     if (!event->Event()->IsRead()) 
        {
        if( ( numIcons <= KMaxNbrOfRecentTrailIcons) &&  //append only if room for trailing icons
        	  iLastOwnIconOffset > 0 )
        	{
        	iconBuff.Append( KTab );
        	iconBuff.AppendNum( EIconNewMissed ); // 
        	numIcons++;
        	}
        } 
 

    /********************************************************************************    
    Presence is not part of 3.x anymore
    //Presence icon
    TInt pecIconIndex( KErrNotFound );
    TBool pecRc( EFalse );
    TRAPD( pecErr, ( pecRc = iControlExtension->GetIconIndexL( 
                                event->Contact(), pecIconIndex ) ) );
    
    if( ( numIcons <= KMaxNbrOfRecentTrailIcons) &&     //append only if room for trailing icons
        pecErr == KErrNone && pecRc && pecIconIndex > 0 )
        {
        iconBuff.Append( KTab );
        iconBuff.AppendNum( pecIconIndex );
        numIcons++;
        }
    ********************************************************************************/                

    // First part: Show icon corresponding the phone number type
    if( event->LogsEventData()->PoC() )
        {        
        ptr.Num( EIconPoc );
        }
    else if( event->LogsEventData()->VoIP() )
        {        
        ptr.Num( EIconVoip );        
        }
    else if( event->LogsEventData()->VT() )
        {        
        ptr.Num( EIconVideo );        
        }
    else 
        {        
        ptr.Num( NumberIconTypeFromPbkField( event->NumberFieldType() ) );        
        }

    //Second part: Show name, number... 
    ptr.Append( KTab );

    //Duplicate count "(n)" is added to end of contact, if duplicates
    // and event is unread (new missed call)
    if( event->Duplicates() > 0 && !event->Event()->IsRead()) 
        {
        duplBuffer.Append( KOpenBracket );              
        duplBuffer.AppendNum( event->Duplicates() );
        duplBuffer.Append( KCloseBracket );              
        AknTextUtils::LanguageSpecificNumberConversion( duplBuffer );    
        duplBufferPixels = iListboxFont->TextWidthInPixels( duplBuffer );            
        }
     else
        {
        duplBufferPixels = 0;    
        }
    
     // For emergency calls, we prefer showing predefined string instead of possible 
     // remoteparty (EJYU-79BCJY).
    if ( event->EventType() == ETypeEmerg )
            {
            BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                                iEmergencyCall->Des() );
            }
    else if( event->RemoteParty() ) 
        {
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            *(event->RemoteParty()) );
        }
    else if( event->EventType() == ETypePrivate )
        {        
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            iPrivateNumber->Des() );
        }
    else if( event->EventType() == ETypePayphone )
        {        
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            iPayphoneNumber->Des() );
        }
    else if ( event->EventType() == ETypeSAT )
        {
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            iSATNumber->Des() );
        }
    else if ( event->EventType() == ETypePoCGroupCall ) //Nothing in remote party and is PoC group call, 
        {                                               //so show "Group call"
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            iGroupCall->Des() );
        }
    else if( event->Number() )                          
        {
        //We prefer showing phone number over sip uri although for making calls uri is preferred (if both
        //available, Poc has made a "best guess" for corresponding msisdn for subsequent cs calls).
        TBuf<KRecentMaxTextLength> formattedNbr(KNullDesC);
        iPhoneNumber->PhoneNumberFormat( *(event->Number()), formattedNbr ); 
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            formattedNbr,  
                            AknTextUtils::EClipFromBeginning ); 
        }
    else if( event->LogsEventData()->Url().Length() > 0 ) 
        {
        TBuf<KRecentMaxTextLength> buf;                        
        iEngine->ConvertToUnicode( event->LogsEventData()->Url(), buf ); 
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            buf );
        }
    else                                                 //Nothing to show. Display "Unknown"
        {
        BuildDisplayString( ptr, duplBufferPixels, duplBuffer, numIcons,
                            iUnknownNumber->Des() );
        }

    //Third part: Show date & time (displayed on the second double graphic style row)
    ptr.Append( KTab );
    TRAPD( err, DateTimeLocalizationL( event, ptr) );
    
    if( err ==  KErrOverflow || err == KErrGeneral )
        {
        ptr.Append(KSpace);  
        }
    else if (err)
        {
        CCoeEnv::Static()->HandleError( err );   
        }

    //Fourth part: Show trailing icons, if any.
    ptr.Append( iconBuff );
    
    //For ring duation feature
    //Fifth part: Show ring duration for missed call
    TLogsDirection dir = event->Direction();
    
    if (( iShowRingDuration ) && ( dir == EDirMissed ))
       {
       AppendRingDuration( *event, ptr );
       }

// Sawfish VoIP changes  >>>>
    // Modify the string so, that the correct icon indices appear in the string
    // Details: This function creates an icon string (e.g. '0\tLabel1\tLabel2') 
    // whereas '0' refers to the icon index in the listbox's icon array. A possible 
    // modified icon string for the example icon string given above would be: 
    // '6\tLabel1\tLabel2\t\t\t8' ('6' is the index of an icon in the A-column, '8' 
    // the index of an icon in the D-column of a listbox-entry)
    if ( iControlExtension && event )
        {        
        iControlExtension->ModifyIconString( 
            ptr, 
            *event );
        }
// <<<<  Sawfish VoIP changes  

    return *iBuffer;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::BuildDisplayString
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::BuildDisplayString(
    TDes&  aDest,
    TInt   aDuplBufPixels,
    TDesC& aDuplBuffer,
    TInt   aNumIcons,
    TPtrC  aOriginal,
    AknTextUtils::TClipDirection aClipDirection  //Beginning=>Treat this as phonenumber, 
    ) const                                      //End=>Treat as 'normal' string
    {
    TBuf<KRecentMaxTextLength> clippedText;  

    //If the original text is longer than KRecentMaxTextLength, cut it first
    TInt origLen = aOriginal.Length();  
    clippedText.Append( aOriginal.Ptr(), 
         ( origLen < KRecentMaxTextLength ) ? origLen : 
         KRecentMaxTextLength );
    
    // Lets replace paragraph delimiters with single white space, fix for EBWG-6ZPCMZ        
    TBuf<1> charsToRemove;
    charsToRemove.Append(TChar(TInt(CEditableText::EParagraphDelimiter)));
    AknTextUtils::ReplaceCharacters(clippedText, charsToRemove, TChar(TInt(CEditableText::ESpace)));
    // Lets strip control chars like tabulator marks which would break the listbox descriptor, see OJON-7CA9SU
    AknTextUtils::StripCharacters(clippedText, KAknStripListControlChars);
    
    TInt widthToClip = iTextWidth[aNumIcons]  - aDuplBufPixels;  
/*  FIXME: These rows should not be needed anymore and can be removed later. Replaced by row above  
    //TInt widthToClip = iTextWidth[aNumIcons]  - aDuplBufPixels -1;  //We need -1 pixels when there's trailing icon in mirrored layout
    // The row above did not work properly in other resolutions than 176*208, therefore temporarily replaced by the row below because we 
    // need to leave enough room for text in all layout combinations. For this reason we additionally narrow the width by 
    // amount needed by one trailing icon: (iTextWidth[1] - iTextWidth[2]). 
    TInt widthToClip = iTextWidth[aNumIcons]  - aDuplBufPixels - (iTextWidth[1] - iTextWidth[2]);
*/    

    AknTextUtils::ClipToFit
            (     clippedText
                , *iListboxFont  //current font used for rendering listbox row
                , widthToClip
                , aClipDirection //Clip from beginning or from end
                , KDefaultClipWidth
                , KThreeDots     //three dots to end or beginning of number string
            );

    //Append first part of string
    if( AknLayoutUtils::LayoutMirrored() && aClipDirection == AknTextUtils::EClipFromBeginning )   //We have phone number
        {
        //There has to be some additional room for directionality marks
        aDest.Append( 0x202A );         //LRE: Treat the following text as embedded left-to-right       
        aDest.Append( clippedText );    //Show phone number completely left to right also in A&H
        aDest.Append( 0x202C );         //PDF: Restore the bidirectional state to what it was before the last LRE, RLE, RLO, LRO
        }
     else
        {
        aDest.Append( clippedText );   
        }

    //Append second part of string (if exists)
    if (AknLayoutUtils::LayoutMirrored())
        {
        aDest.Append( 0x200F );         //This puts the following stuff to the left when mirrored layout
        aDest.Append( 0x202A );         //This keeps the brackets correctly on both sides of the duplicate number
                                        //We need this if there are western characters to be displayed with arabic layout
        }
        
    aDest.Append( aDuplBuffer );        //Finally append duplicates. We should have enough room to append the duplicate string without
                                        //another clipping (and another string of three dots) by listbox row.                                            
    }
    


// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::SetLineWidth
//
// Sets the maximum width in pixels for the detail line. This needs to be called from controller's
// SizeChanged() method.
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::SetLineWidth(
    TInt aTextWidth, 
    TInt aNumOfTrailingIcons )
    {
    iTextWidth[aNumOfTrailingIcons] = aTextWidth;
    iListboxFont = LineFont();  //Update font too as probably it has also changed.
    }


// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::LineFont
// ----------------------------------------------------------------------------
//
const CFont* CLogsRecentListAdapter::LineFont()
    {
    TAknTextLineLayout line = AknLayout::List_pane_texts__double_graphic__Line_1( 0 );
    TAknLayoutText text;
    text.LayoutText( iListBox->Rect(), line );  
    return text.Font();   // text.Font() not owned
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::AppendRingDuration
//
// For ring duation feature
// Append ring duration for missed calls 
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::AppendRingDuration( 
    const MLogsEventGetter& aEvent,
    TPtr& aPtr ) const
    {
    //Add duration if it is missed call
    TLogDuration ringDuration = aEvent.RingDuration();
    aPtr.Append( KSpace );

    // Format ring duration    
    TBuf<10> ringDurationTxt( KNullDesC );
    FormatRingDuration( ringDurationTxt, (TInt)ringDuration );
    AknTextUtils::LanguageSpecificNumberConversion( ringDurationTxt ); 
    aPtr.Append( ringDurationTxt );
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::FormatRingDuration
//
// Format ring duation as 00:00
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::FormatRingDuration(
    TDes& aDesc,
    TInt aSeconds ) const
    {
    aDesc.Zero();
    TLocale locale;
    
    TInt minutes = aSeconds / KLogsOneMinute;
    AppendNum( aDesc, minutes );
    
    AppendChar( aDesc, locale.TimeSeparator( 2 ) ); // minute seperator
    
    TInt seconds2 = aSeconds - minutes * KLogsOneMinute;
    AppendNum( aDesc, seconds2 );

    }

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::AppendNum
//
// Append 2 digit number, if it is one digit number, insert zero before the number
// ----------------------------------------------------------------------------
//
void CLogsRecentListAdapter::AppendNum(
    TDes& aDesc,
    TInt aNum ) const
    {
    if ( aNum < 0 ) // Negative value not accepted - make it zero
        {
        aNum = 0;
        }

    if ( aNum >= KLogsOneMinute ) // Too large values are not accepted
        {
        aNum = KLogsOneMinute;
        aNum--;
        }

    if ( aNum < 10 ) // Two digits
        {
        aDesc.Append( KLogsOneZero );
        }
    aDesc.AppendNum( aNum );
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::AppendChar
//
// Append char if it is valid
// ----------------------------------------------------------------------------
//
inline void CLogsRecentListAdapter::AppendChar(
    TDes& aDesc,
    TChar aCh ) const
    {
    if ( aCh ) // 0 -> no character defined
        {
        aDesc.Append( aCh );
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::NumberIconTypeFromVPbk
//
// Get the icon type mapped to vpbk field type
// ----------------------------------------------------------------------------
// no use currently, use it just incase the Phone's implementation changes
RecentListIconArrayIcons CLogsRecentListAdapter::NumberIconTypeFromVPbk( TInt aNumberFieldType ) const
    {
    RecentListIconArrayIcons iconType = EIconDefault;
       
    switch ( aNumberFieldType ) 
        {   
        case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:
            iconType = EIconMobile;           
            break;

        case R_VPBK_FIELD_TYPE_LANDPHONEHOME:
        case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
        case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
            iconType = EIconlandphone;            
            break;
                
        case R_VPBK_FIELD_TYPE_PAGERNUMBER:
            iconType = EIconPager;            
            break;
            
        case R_VPBK_FIELD_TYPE_FAXNUMBERGEN:
        case R_VPBK_FIELD_TYPE_FAXNUMBERHOME:
        case R_VPBK_FIELD_TYPE_FAXNUMBERWORK:
            iconType = EIconFax;           
            break;
             
        case R_VPBK_FIELD_TYPE_ASSTPHONE:
            iconType = EIconAsstPhone; 
            break;
  
        case R_VPBK_FIELD_TYPE_CARPHONE:
            iconType = EIconCarPhone; 
            break;                   
                     
        default:
            break;
        }
    
    return iconType;  
    }

// ----------------------------------------------------------------------------
// CLogsRecentListAdapter::NumberIconTypeFromPbkField
//
// Get the icon type mapped to phonebook field type
// ----------------------------------------------------------------------------
//
RecentListIconArrayIcons CLogsRecentListAdapter::NumberIconTypeFromPbkField( TInt aNumberFieldType ) const
    {
    RecentListIconArrayIcons iconType = EIconDefault;
        
    switch( aNumberFieldType ) 
        {   
        case EPbkFieldIdPhoneNumberMobile:
            iconType = EIconMobile;
            break;
                 
        case EPbkFieldIdPhoneNumberGeneral:
            iconType = EIconlandphone;            
            break;
                 
        case EPbkFieldIdPagerNumber:
            iconType = EIconPager;            
            break;
             
        case EPbkFieldIdFaxNumber:
            iconType = EIconFax;           
            break;
              
        case EPbkFieldIdAssistantNumber:
            iconType = EIconAsstPhone; 
            break;
   
        case EPbkFieldIdCarNumber:
            iconType = EIconCarPhone; 
            break;                   
                      
        default:
            break;
        }
     
    return iconType;  
    }

    
//  End of File  
