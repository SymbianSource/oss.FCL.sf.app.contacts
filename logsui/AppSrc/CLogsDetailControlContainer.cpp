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
*     Logs "Detail" view container control class implementation
*
*/


// INCLUDE FILES
#include <aknnavide.h>
#include <akntabgrp.h>
#include <applayout.cdl.h>    //Dynamic layout functions
#include <logs.rsg>
#include <AknUtils.h>

#include "CLogsDetailControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsDetailAdapter.h"
#include "CLogsDetailView.h"
#include "CLogsEngine.h"
#include "MLogsGetEvent.h"
#include "MLogsEventGetter.h"
#include "MLogsModel.h"
#include "MLogsSharedData.h"
#include "CPhoneNumberFormat.h"

#include "LogsConstants.hrh"

#include "LogsUID.h"
#include <csxhelp/log.hlp.hrh>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsDetailControlContainer* CLogsDetailControlContainer::NewL
        ( CLogsDetailView* aView, const TRect& aRect )
    {
    CLogsDetailControlContainer* self = new( ELeave )
                                CLogsDetailControlContainer( aView );
    self->SetMopParent( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::~CLogsDetailControlContainer
// ----------------------------------------------------------------------------
//
CLogsDetailControlContainer::~CLogsDetailControlContainer()
    {
    delete iListBox;
    delete iDecoratedTabGroup;
    delete iPrivateNumber;
    delete iUnknownNumber;
    delete iPayphoneNumber;    
    delete iEmergencyCall;
    delete iTitleLabel;
    delete iCsdNumber;
    delete iOperServNumber;
    delete iGroupCall;
    delete iPhoneNumber;
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::CLogsDetailControlContainer
// ----------------------------------------------------------------------------
//
CLogsDetailControlContainer::CLogsDetailControlContainer
    (   CLogsDetailView* aView
    )
    : CLogsBaseControlContainer( aView )
    , iView( aView )
    {
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::ConstructL( const TRect& aRect )
    {
    BaseConstructL();

    //FIXME: this can be removed as Phone takes care of leavin csd numbers unlogged if preferred                                             
    // iShowCsd = iView->Engine()->SharedDataL()->ShowCsdNumber(); 
    
    iListBox = new( ELeave ) CAknSingleHeadingStyleListBox;
    iListBox->ConstructL( this, EEikListBoxMultipleSelection
                                | EAknListBoxViewerFlags );

    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );   //Ownership of iAdapter to iListBox

    iAdapter = CLogsDetailAdapter::NewL( iListBox, iView );    
    
    iListBox->Model()->SetItemTextArray( iAdapter );            //Ownership of iAdapter transferred to iListBox
    
    MakeEmptyTextListBoxL( iListBox, R_LOGS_EVENTVIEW_FETCHING_TEXT );    

    iPrivateNumber = iCoeEnv->AllocReadResourceL( 
                                    R_DLOGS_DETAILS_PRIVATE_NUMBER );
    iUnknownNumber = iCoeEnv->AllocReadResourceL( 
                                    R_DLOGS_DETAILS_UNKNOWN_NUMBER );
    iPayphoneNumber = iCoeEnv->AllocReadResourceL( 
                                    R_DLOGS_DETAILS_PAYPHONE_NUMBER ); 
                                    
    iEmergencyCall = iCoeEnv->AllocReadResourceL( 
                                    R_LOGS_EMERG_CALL );
    iCsdNumber = iCoeEnv->AllocReadResourceL( 
                                    R_LOGS_DETAILS_CSD_NUMBER );
    iOperServNumber = iCoeEnv->AllocReadResourceL( 
                                    R_LOGS_DETAILS_OPER_SERV );
    iGroupCall = iCoeEnv->AllocReadResourceL( 
                                    R_LOGS_CON_GROUP_CALL );                                        
    iTitleLabel = new (ELeave) CEikLabel();

    iPhoneNumber = CPhoneNumberFormat::NewL();

    iDecoratedTabGroup = NaviPaneL()->CreateTabGroupL();

    iDecoratedTabGroup->SetNaviDecoratorObserver(iView);

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( 
                        iDecoratedTabGroup->DecoratedControl() );
    tabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );
    tabGroup->AddTabL( 0, KSpace );

    SetRect( aRect );
    ActivateL();     
    }
   

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::InitNaviPaneL
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::InitNaviPaneL()
    {
    const MLogsEventGetter* event = iView->Engine()->GetEventL()->Event();

    TLogsDirection dir = event->Direction();            
    TUid uid;
    uid.iUid = event->EventUid().iUid;

    //if( ! iShowCsd &&  //FIXME: this can be removed as Phone takes care of leavin csd numbers unlogged if preferred
    // (replaced by row below)
    if( !event->Number() && !event->RemoteParty() &&
        uid.iUid == KLogDataEventType &&
        ( dir == EDirOutAlt || dir == EDirOut ) )
        {
        //CSD-numbers: some operators don't want to show csd-number. In those cases Phone does not log the phone number 
        //and may not even log access point to RemoteParty, i.e. the entry is ETypeUnknown. However, for those entries 
        //show iCsdNumber instad of iUnknownNumber.
        MakeTitleL( *iCsdNumber );
        }
    else
        {
        switch( event->EventType() )
            {
            case ETypeUsual:
                 if( event->RemoteParty() )         //If remote party provided. Title pane shows
                    {                               //on two rows if does not fit to one row
                    // lets replace paragraph delimiters with single white space, fix for EBWG-6ZPCMZ 
                    TPtr titleText = event->RemoteParty()->Des();
                    TBuf<1> charsToRemove;
                    charsToRemove.Append(TChar(TInt(CEditableText::EParagraphDelimiter)));
                    AknTextUtils::ReplaceCharacters(titleText, charsToRemove, TChar(TInt(CEditableText::ESpace)));
                    MakeTitleL(titleText);  
                    }
                else if( event->Number() )          //Phone number. Shown on one line only,
                    {                               //so needs to be clipped if does not fit
                    //We prefer showing phone number over sip uri. If this order is changed, change it 
                    //also to CLogsBaseView::CmdSaveToVirtualPhonebookL and other views too
                    
                    //First calculate max width in pixels available in title pane
                    TAknTextLineLayout line = AknLayout::Title_pane_texts_Line_1(0, 0); 
                    TRect titlePane;
                    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::ETitlePane, titlePane);
                    TAknLayoutText text;
                    text.LayoutText( titlePane, line);  
                    TInt maxTextWidthPixels = text.TextRect().Width();   
                    //Then format the phone number to fit in available space
                    TBuf<KLogsPhoneNumberMaxLen> tmp;
                    TBuf<KLogsPhoneNumberMaxLen> clippedNumber;
                    iPhoneNumber->DTMFStrip( *(event->Number()), tmp );
                    iPhoneNumber->PhoneNumberFormat
                                    (   tmp
                                    ,   clippedNumber
                                    ,   iTitleLabel->Font()
                                    ,   maxTextWidthPixels // text.iW  
                                    );
                    MakeTitleL( clippedNumber );
                    }                    
                else if( event->LogsEventData()->Url().Length() > 0 ) 
                    {
                    TBuf<KLogsSipUriMaxLen> buf; 
                    
                    if ( iView->Engine()->ConvertToUnicode( event->LogsEventData()->Url(), buf ) )
                        {
                        MakeTitleL( KNullDesC );    //Converting failed
                        }
                    else
                        {
                        MakeTitleL( buf );
                        }
                    }
                break;

            case ETypePoCGroupCall:                
                 if( event->RemoteParty() )         //If remote party provided. Title pane shows
                    {                               //on two rows if does not fit to one row
                    MakeTitleL( *(event->RemoteParty()) );  
                    }
                else                                //if no remote party data available, show
                    {                               //localised text "group call"
                    MakeTitleL( *iGroupCall );  
                    }
                break;                

            case ETypePrivate:
                MakeTitleL( *iPrivateNumber );
                break;

            case ETypeUnknown:
                MakeTitleL( *iUnknownNumber );
                break;

            case ETypePayphone:
                MakeTitleL( *iPayphoneNumber );
                break;

            case ETypeEmerg:
                MakeTitleL( *iEmergencyCall );
                break;

            case ETypeSAT:
                 if( event->RemoteParty() )         
                    {                               
                    // sat event with contact
                    MakeTitleL( *(event->RemoteParty()) );  
                    }
                else 
                    {
                    MakeTitleL( *iOperServNumber );        
                    }                    
                break;

            default:            
                __ASSERT_DEBUG( EFalse, User::Panic( KPanicText_DetailsCC, KErrCorrupt ));
                break;
            }
        }

    TTime time( event->Time() );

    HBufC* dateTextBuf = HBufC::NewLC( KLogsBuff128 );
    TPtr dateText( dateTextBuf->Des() );
    // Get date string in the format that current localisation requires.
    // Date "12/12"
    HBufC* textFromResourceFile = iCoeEnv->AllocReadResourceLC(
                                  R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );

    time.FormatL( dateText, *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile

    HBufC* timeTextBuf = HBufC::NewLC( KLogsBuff128 );
    TPtr timeText( timeTextBuf->Des() );
    // Get time string in the format that current localisation requires.
    // Time "hh:mm" +PM/AM if needed
    textFromResourceFile = iCoeEnv->AllocReadResourceLC(
                                                R_QTN_TIME_USUAL_WITH_ZERO );

    time.FormatL( timeText, *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile

    TBuf<KLogsBuff256> wholeTextBuf;
    wholeTextBuf.Append( dateText );
    wholeTextBuf.Append( KSpace );
    wholeTextBuf.Append( timeText );
    
    AknTextUtils::LanguageSpecificNumberConversion( wholeTextBuf ); 

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( 
                                    iDecoratedTabGroup->
                                    DecoratedControl() );
    
    tabGroup->ReplaceTabL( 0, wholeTextBuf );

    // timeTextBuf, dateTextBuf
    CleanupStack::PopAndDestroy( 2 );
    tabGroup->SetActiveTabByIndex( 0 );  /// Activating the tab to show

    //If mirrored display right and left arrow dimmed vice versa to western display
    enum CAknNavigationDecorator::TScrollButton fwrd = 
        AknLayoutUtils::LayoutMirrored() ? CAknNavigationDecorator::ELeftButton : 
                                           CAknNavigationDecorator::ERightButton;  
    enum CAknNavigationDecorator::TScrollButton back = 
        AknLayoutUtils::LayoutMirrored() ? CAknNavigationDecorator::ERightButton : 
                                           CAknNavigationDecorator::ELeftButton;  

    //Fix for ESZG-6SYDD6. In 3.1 the platfrom seems wrongly to undim scroll buttons 
    //when only one entry and navigating left/right.
    if ( iView->Engine()->Model( ELogsMainModel )->Count() == 1 ) 
        {
        iDecoratedTabGroup->MakeScrollButtonVisible( EFalse );
        }
    else
        {
        iDecoratedTabGroup->MakeScrollButtonVisible( ETrue );
        }

    //Set scroll arrows
    if ( iView->LogsAppUi()->EventListViewCurrent() == 0 )
        {
        iDecoratedTabGroup->SetScrollButtonDimmed(back, ETrue);
        }
    else
        {
        iDecoratedTabGroup->SetScrollButtonDimmed(back, EFalse);
        }

    if ( iView->LogsAppUi()->EventListViewCurrent() == 
                iView->Engine()->Model( ELogsMainModel )->Count() - 1 )
        {
        iDecoratedTabGroup->SetScrollButtonDimmed(fwrd, ETrue);
        }
    else
        {
        iDecoratedTabGroup->SetScrollButtonDimmed(fwrd, EFalse);
        }

    /// Makes the whole thing visible
    NaviPaneL()->PushL( *( iDecoratedTabGroup ) );
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsDetailControlContainer::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }


// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        TInt textWidth = 0;

        //parent rectangle
        iListBox->SetRect( Rect() );  

        //calculate max width in pixels available for a row
        TRect main_pane = iListBox->Rect();
        TAknWindowLineLayout line3 = AknLayout::list_gen_pane(0);
        TAknLayoutRect rect2;
        rect2.LayoutRect(main_pane, line3);
        TRect list_gen_pane = rect2.Rect();
        TAknWindowLineLayout line2 = AknLayout::list_single_heading_pane(0);
        TAknLayoutRect rect;
        rect.LayoutRect(list_gen_pane, line2);
        TRect list_single_heading_pane = rect.Rect();

        TAknTextLineLayout line = AknLayout::List_pane_texts__single_heading__Line_2(0); //Same in CLogsDetailAdapter::LineFont
        TAknLayoutText text;
        text.LayoutText(list_single_heading_pane, line);  
        textWidth = text.TextRect().Width();     

        // Then update this value to adapter as it needs to truncate shown data if necessary
        iAdapter->SetLineWidth(textWidth);  
        }                                       
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknSingleHeadingStyleListBox* CLogsDetailControlContainer::ListBox()
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::UpdateL
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::UpdateL()
    { 
    iAdapter->UpdateL();
    InitNaviPaneL();
    iListBox->HandleItemAdditionL();  //...MdcaPoint will be called later... 
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KLOGS_HLP_DETAIL;
    }

// ----------------------------------------------------------------------------
// CLogsDetailControlContainer::FocusChanged
//    
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsDetailControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }    

//  End of File
