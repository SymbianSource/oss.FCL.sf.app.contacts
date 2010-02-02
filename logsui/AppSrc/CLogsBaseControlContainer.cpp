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
*     Logs Application view control container superclass
*
*/


// INCLUDE FILES
#include <AknIconArray.h>
#include <gulicon.h>    // CGulIcon
#include <barsread.h>   // resource reader
#include <akntitle.h>   // Title
#include <aknnavide.h>  // CAknNavigationDecorator
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>  //KDC_APP_BITMAP_DIR and KDC_SHARED_LIB_DIR
#include <akncontext.h>  
#include <AknDef.h>   //KEikDynamicLayoutVariantSwitch
#include <StringLoader.h>
#include <Logs.rsg>

#include "CLogsBaseControlContainer.h"
#include "MLogsKeyProcessor.h"

#include "LogsConstants.hrh"    // Global constants.

#include "MLogsNaviDecoratorWrapper.h"

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
// CLogsBaseControlContainer::CLogsBaseControlContainer
// ----------------------------------------------------------------------------
//
CLogsBaseControlContainer::CLogsBaseControlContainer( 
    MLogsKeyProcessor* aKeyProcessor ):
    iKeyProcessor( aKeyProcessor )
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::CLogsBaseControlContainer
// ----------------------------------------------------------------------------
//
CLogsBaseControlContainer::CLogsBaseControlContainer()
    {
    }
   
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::~CLogsBaseControlContainer
// ----------------------------------------------------------------------------
//
CLogsBaseControlContainer::~CLogsBaseControlContainer()
    {
	//delete iLongTapDetector; 
    }
 
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::BaseConstructL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::BaseConstructL()
    {
    CreateWindowL();    
    
	//iLongTapDetector = CAknLongTapDetector::NewL( this ); 
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeTitleL
//
// Makes the title of the view
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::MakeTitleL( TInt aResourceText )
    {
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceText );
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy();  // reader
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeTitleL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::MakeTitleL( const TDesC& aText )
    {
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( aText );
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::SetTitlePaneTextToDefaultL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::SetTitlePaneTextToDefaultL(TLogsViewIds aViewId)
    {
    //  Well handle the Logs recent calls views and the Log view here.
    switch( aViewId )
        {
        case EStmMissedListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_MISSED );                       
            break;

        case EStmReceivedListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_RECEIVED );
            break;

        case EStmDialledListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_DIALLED );
            break;
            
        case ELogEventListViewId:    
            MakeTitleL( R_LOGS_TITLE_TEXT );
            break;
            
        default:
            {
            // not needed for other views.
            break;
            }
        }
    }



// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::SetContextPanePictureToDefaultL
//    
// Restore original Logs icon back to Context Pane
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::SetContextPanePictureToDefaultL()
    {
    CCoeControl* ctrl  = StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidContext ) );
    CAknContextPane* ctx = static_cast<CAknContextPane*>( ctrl );
    ctx->SetPictureToDefaultL();
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::StatusPane
// ----------------------------------------------------------------------------
//
CEikStatusPane* CLogsBaseControlContainer::StatusPane()
    {
    return iEikonEnv->AppUiFactory()->StatusPane();
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::NaviPaneL
// ----------------------------------------------------------------------------
//
CAknNavigationControlContainer* CLogsBaseControlContainer::NaviPaneL()
    {
    return static_cast<CAknNavigationControlContainer*>( StatusPane()->
                    ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::NavigationTabGroupL
//
// Creates navigation tab group 
// ----------------------------------------------------------------------------
//
CAknNavigationDecorator* CLogsBaseControlContainer::NavigationTabGroupL( TInt aResource, 
                                                                         MLogsNaviDecoratorWrapper*  aNaviDecoratorWrapper )
    {
    CAknNavigationDecorator* naviDecorator = NULL;
    naviDecorator = aNaviDecoratorWrapper->GetNaviDecoratorL(NaviPaneL(),
                                                             iKeyProcessor->TabObserver(),
                                                             aResource);                                                         
    return naviDecorator;
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::OfferKeyEventL
//
// Called by framework (CCoeControlStack) when a key press happens. This function offers the key press 
// further to subclass for processing.
// ----------------------------------------------------------------------------
//
TKeyResponse CLogsBaseControlContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CAknNavigationDecorator* decoratedTabGroup = NULL;
    decoratedTabGroup = NaviPaneL()->Top();
    TKeyResponse keyResponse = EKeyWasNotConsumed;

    //Check first is this keypress intended for changing the view using left/right key
    if (decoratedTabGroup && 
        decoratedTabGroup->ControlType() == CAknNavigationDecorator::ETabGroup)
        {
        keyResponse = decoratedTabGroup->DecoratedControl()->OfferKeyEventL( aKeyEvent, aType );
        }

    //If not, then offer this to subclass for possible processing (e.g. press of Send key to make a call)
    if( (keyResponse == EKeyWasNotConsumed) && iKeyProcessor )
        {
        if( iKeyProcessor->ProcessKeyEventL( aKeyEvent, aType ) )
            {
            return EKeyWasConsumed;
            }
        }

    return ComponentControl( 0 )->OfferKeyEventL( aKeyEvent, aType );
    }


// *** Long tap detection disabled ***
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::HandleLongTapEventL
//
// Default callback function for the Avkon CAknLongTapDetector object.
// Pointer events are passed to the long tap detector (see HandlePointerEventL below)
// and the callback is initiated when it detects a long tap. 
// 
// ----------------------------------------------------------------------------
//void CLogsBaseControlContainer::HandleLongTapEventL( const TPoint& /* aPenEventLocation */, 
//                            	  					 const TPoint& /* aPenEventScreenLocation */ )
//{
// By default do nothing. Subclasses override this if special functionality is needed.
// }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::ListBox
//
// Default implementation returns NULL. 
// ----------------------------------------------------------------------------
//
CEikListBox* CLogsBaseControlContainer::ListBox()
    {
  	return NULL;
    }

// *** Pointer event handling disabled ****
// Touch support implemented in CLogsBaseView::HandleListBoxEventL instead.
//
// In case long tap event handling is needed in Logs, we need to catch only them
// here and just forward the pointer events to the listbox instances. 
// 
// In case the long tap functionality is needed in just some specific view. 
// implement HandlePointerEventL in that view only.
// 
//----------------------------------------------------------------------------
// CLogsBaseControlContainer::HandlePointerEventL
//
// Called by framework (CCoeControlStack) when a pointer event happens (touch ui).
// ----------------------------------------------------------------------------
//
	/*
void CLogsBaseControlContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	
	if (!iLongTapDetector)
		{
		iLongTapDetector = CAknLongTapDetector::NewL( this );
		}
		
	if (AknLayoutUtils::PenEnabled())
		{
		// Get a reference to the correct listbox
		CEikListBox* listbox = ListBox(); // calls the actual subclass implementation
		
    	// Pass event to the LongTapDetector
    	// Long tap detector not needed in Logs at the moment
    	// For possible future needs we just comment out the line below 
	  	//iLongTapDetector->PointerEventL( aPointerEvent ); 
	  	
		switch (aPointerEvent.iType)
			{
			case TPointerEvent::EButton1Down:
				// Get the index of the current item (note: before the possible focus change)
				iLastIndex = listbox->CurrentItemIndex();
 				// Pass event to the listbox - handles selection and moves focus 
 				// to correct item 
				listbox->HandlePointerEventL(aPointerEvent);
				break;
			
			case TPointerEvent::EDrag:
				// Pass EDrag to the listbox to handle scrolling
				listbox->HandlePointerEventL(aPointerEvent);
				// If dragging resulted on focus change, set iDrag true
				if ( iLastIndex != listbox->CurrentItemIndex())
				    {
				    iDrag = ETrue;
				    }			    
				break;
				
			case TPointerEvent::EButton1Up:	
			    // If current view wants a single tap to perform the 
			    // same as double tap. Currently this is needed in Logs
			    // Detail view where there is no focus and a single tap
			    // opens contex sensitive menu
                iKeyProcessor->FocusChangedL();			    
                
			    if (iSingleTapActivates)
			        {
			        iKeyProcessor->ProcessPointerEventL(iLastIndex);
			        }

				// Check if the current focused item was tapped (second tap)
				// and call the views ProcessPointerEventL  
				else if (iLastIndex == listbox->CurrentItemIndex() && iDrag == EFalse)
					{
					iKeyProcessor->ProcessPointerEventL(iLastIndex);
					}
				// If dragging happened, we don't call the event processor, but just 
			    else if ( iDrag )
			        {
			        iLastIndex = listbox->CurrentItemIndex();
			        iDrag = EFalse;
			        }
				break;

			default:
				break;
		
			}	
				
		CCoeControl::HandlePointerEventL(aPointerEvent);
		}
	}*/
		
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::HandleControlEventL
//
// Dummy HandleControlEventL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,
    TCoeEvent /*aEventType*/ )
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::CountComponentControls
//
// Returns the count of child controls
// ----------------------------------------------------------------------------
//
TInt CLogsBaseControlContainer::CountComponentControls() const
    {
    return 1;
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::AddColorIconL
//
// Adds color icon based on default path to the icon file.
// Used for D-column icons to fix error JPYO-76PDP9.
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::AddColorIconL(
    CAknIconArray* aIcons,
    const TDesC& aIconFile,   //File name without path
    TAknsItemID aSkinnedIcon,
    TInt aIconGraphicsIndex,
    TInt aIconGraphicsMaskIndex )
    {
    // Set up filenames for Logs and Avkon bitmaps
    TFileName iconFileWithPath;
    iconFileWithPath += KLogsIconFileDrive;

    iconFileWithPath += KDC_APP_BITMAP_DIR;          //Directory in new secure builds 

    iconFileWithPath += aIconFile;  

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();  //AknsUtils.h
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknsUtils::CreateColorIconL( skin, aSkinnedIcon,
                                KAknsIIDQsnIconColors,EAknsCIQsnIconColorsCG13,
                                bitmap, mask, 
                                iconFileWithPath, aIconGraphicsIndex, 
                                aIconGraphicsMaskIndex, KRgbBlack  );
    CleanupStack::PushL( bitmap );  //Ownership here
    CleanupStack::PushL( mask );    //Ownership here

    // Append it to icons array
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 );         // mask, bitmap. icon has now ownership
    CleanupStack::PushL( icon );
    aIcons->AppendL( icon );
    CleanupStack::Pop( ); // icon
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::AddIconL
//
// Adds icon based on default path to the icon file
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::AddIconL(
    CAknIconArray* aIcons,
    const TDesC& aIconFile,   //File name without path
    TAknsItemID aSkinnedIcon,
    TInt aIconGraphicsIndex,
    TInt aIconGraphicsMaskIndex )
    {
    // Set up filenames for Logs and Avkon bitmaps
    TFileName iconFileWithPath;
    iconFileWithPath += KLogsIconFileDrive;

    iconFileWithPath += KDC_APP_BITMAP_DIR;          //Directory in new secure builds 

    iconFileWithPath += aIconFile;  

    AddIconWithPathL(   aIcons,
                iconFileWithPath,
                aSkinnedIcon,
                aIconGraphicsIndex,
                aIconGraphicsMaskIndex );
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::AddIconWithPathL
//
// Adds icon based on full path to the icon file
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::AddIconWithPathL(
    CAknIconArray* aIcons,
    const TDesC& aIconFileWithPath,   //File name with full path
    TAknsItemID aSkinnedIcon,
    TInt aIconGraphicsIndex,
    TInt aIconGraphicsMaskIndex )
    {

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();  //AknsUtils.h
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknsUtils::CreateIconL( skin, aSkinnedIcon, bitmap, mask, 
                            aIconFileWithPath, aIconGraphicsIndex, 
                            aIconGraphicsMaskIndex );
    CleanupStack::PushL( bitmap );  //Ownership here
    CleanupStack::PushL( mask );    //Ownership here

    // Append it to icons array
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 );         // mask, bitmap. icon has now ownership
    CleanupStack::PushL( icon );
    aIcons->AppendL( icon );
    CleanupStack::Pop( ); // icon
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeListBoxLineL
//
// Makes views one line with the given information
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::MakeListBoxLineL(
    CDesCArrayFlat* aItems,
    const TDesC& aText,
    TInt aResourceText,
    const TDesC& aSecondLineText )
    {
    // Place where to put text to display
    HBufC* textBuf = HBufC::NewLC( KLogsBuff128 ); 

    // temporary place for the resource texts
    HBufC* textFromResourceFile;

    textBuf->Des().Format( aText );  // First icon & tab
    textFromResourceFile = iCoeEnv->AllocReadResourceLC( aResourceText );
    textBuf->Des().AppendFormat( *textFromResourceFile );
    textBuf->Des().AppendFormat( KTab );
    textBuf->Des().AppendFormat( aSecondLineText );

    aItems->AppendL( *textBuf );
    CleanupStack::PopAndDestroy( 2 );  // textFromResourceFile. textBuf
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeEmptyTextListBoxL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::MakeEmptyTextListBoxL(
    CEikListBox* aListBox,
    TInt aResourceText )
    {
    if( aResourceText == KErrNotFound )
        {
        //When reader deleted / not active, don't show any misleading texts when
        //list is empty
        aListBox->View()->SetListEmptyTextL( KNullDesC );            
        }
    else if (aResourceText == R_QTN_LOGS_LOGGING_OFF)    
        {
        HBufC* loggingOffText;
        loggingOffText = MakeNoLogTextLC();
        // If log duration is set to "no log", show informative text
        aListBox->View()->SetListEmptyTextL( *loggingOffText );
        CleanupStack::Pop(); // loggingOffText.
        delete loggingOffText;
        }   
    else
        {
        HBufC* textFromResourceFile;
        textFromResourceFile = iCoeEnv->AllocReadResourceLC( aResourceText );
        // Set the view's listbox empty text.
        aListBox->View()->SetListEmptyTextL( *textFromResourceFile );
        CleanupStack::PopAndDestroy();  // textFromResourceFile.        
        }

    //Set rect to draw just a smaller part of screen only.    
    // TRect rect;
    // AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    // SetRect( rect );
    // DrawNow();  //This contributes to flickering in certain cases and seems to be unnecessary too
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeNoLogTextL
// ----------------------------------------------------------------------------
//
HBufC* CLogsBaseControlContainer::MakeNoLogTextLC(void)
    {
    HBufC* text = NULL;  
    
    // Read primary text        
    text = StringLoader::LoadLC( R_QTN_LOGS_LOGGING_OFF );

    // Append secondary text           
    _LIT( KNewline, "\n" );
    HBufC* secondaryText = StringLoader::LoadLC( R_QTN_LOGS_LOGGING_OFF_SECONDARY );
    TInt newLength = text->Length()
                   + KNewline().Length()
                   + secondaryText->Length();
    text = text->ReAllocL( newLength );
    TPtr ptr = text->Des();
    ptr.Append( KNewline );
    ptr.Append( *secondaryText );
    
    CleanupStack::PopAndDestroy( secondaryText );
    
    return text;
    }
         
// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::MakeScrollArrowsL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::MakeScrollArrowsL( CEikListBox* aListBox )
    {
    // functions for scrollbars
    CEikScrollBarFrame* sBFrame = aListBox->CreateScrollBarFrameL( ETrue );
    sBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,    //no horisontal scrollbar
                                      CEikScrollBarFrame::EAuto ); //vertical
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::HandleResourceChange( TInt aType )
    {
	CCoeControl::HandleResourceChange(aType);    

    if( aType == KAknsMessageSkinChange )
        {
        //Basically there in this case should already be container controls loaded that
        //are first released in subclass's implementation of AddControlContainerIconsL. So we
        //should have the same memory available. However, if we would run out of memory, there's 
        //not so much we could do. So we'll just trap the possible leave.
        TInt err;
        TRAP( err, AddControlContainerIconsL() );
        if( err ) 
            {
            CCoeEnv::Static()->HandleError( err );
            }
            
        Window().Invalidate();
        }

    //Handle change in layout orientation
    if (aType == KEikDynamicLayoutVariantSwitch)
        {

        //TRect rect = iAvkonAppUi->ClientRect();
        // The line above provides too big rectangle in the bottom causing cba's overdrawn by blank area.
        // Correct way to do this is below.
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);
		DrawNow();
		}
    }

// ----------------------------------------------------------------------------
// CLogsBaseControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CLogsBaseControlContainer::AddControlContainerIconsL()
    {
    return;
    }

//  End of File
