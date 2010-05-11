/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares container control for application.
*
*/






// INCLUDE FILES
#include <AknGrid.h>
#include <eikapp.h>
#include <gulicon.h>
#include <avkon.rsg>
#include <aknkeys.h>
#include <eikmenub.h>
#include <eikimage.h>
#include <csxhelp/sdm.hlp.hrh>
#include <bldvariant.hrh>

#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <SpdCtrl.rsg>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <aknViewAppUi.h>
#include <AknFepInternalCRKeys.h>
#include <aknlists.h>

#include <SpeedDial.rsg>

#include "speeddial.hrh" 
#include "SpdiaControl.hrh"
#include "speeddialprivate.h"
#include "SpdiaView.h"
#include "SpdiaContainer.h"
#include "SpdiaApplication.h"
#include "SpdiaNote.h"
#include "Speeddial.laf"
#include <applayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <vmnumber.h>
#include "SpdiaAppUi.h"

#include <featmgr.h>
#include <NumberGrouping.h>

#include <CVPbkContactStoreUriArray.h>
#include <CPbk2StoreConfiguration.h>
#include <voicemailboxdomaincrkeys.h>
#include <telvmbxsettingscrkeys.h>


// LOCAL CONSTANTS AND MACROS
const TUint KShortcutKey0('0');
const TInt KCellRowCount(3);
const TInt KCellColCount(3);
const TInt KMaxIndex(8);
const TInt KInvalidIndex(10);
const TInt KPhoneNumberMaxLen = 100;

// ================= MEMBER FUNCTIONS =======================
// Constructor
CSpdiaContainer::CSpdiaContainer(CSpeedDialPrivate *aSpeedPrivate):iSpeedPrivate( aSpeedPrivate ),
    iLongTapUsed( EFalse ), iNoAssignedShown( EFalse ), iButton1DownIndex( 0 )
    {
    }

// ---------------------------------------------------------
// CSpdiaContainer::ConstructL
// Symbian two phased constructor
// 
// ---------------------------------------------------------
//
void CSpdiaContainer::ConstructL(CAknView* aView)
    {
    CreateWindowL();

    iView = STATIC_CAST(CSpdiaView*, aView);

    CSpdiaAppUi* spdialUi = static_cast< CSpdiaAppUi* >( CEikonEnv::Static()->AppUi() );
    if ( !spdialUi->IsSettingType() )
        {
        iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_EXIT_OPTION );
        }
    iAlreadyActive = EFalse;

    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdPhoneNumberGrouping ) )
        {
        iNumberGrouping = CPNGNumberGrouping::NewL( KPhoneNumberMaxLen );
        }
    // Create Grid
    iGrid = new(ELeave) CAknQdialStyleGrid;
    iGrid->SetContainerWindowL( *this );
    iGrid->ConstructL( this, CEikListBox::EMultipleSelection );
    iGrid->SetBorder(TGulBorder::ENone);

    iGrid->CreateScrollBarFrameL(ETrue);
    iGrid->SetListBoxObserver( this );
    // Create Grid-info

    TInt error = iSpeedPrivate->GetSpdCtrlLastError();
    if ( error != KErrNone )
        {
        CSpdiaAppUi* spdialUi = static_cast< CSpdiaAppUi* >( CEikonEnv::Static()->AppUi() );
        spdialUi->LaunchInfoNoteL();
        return ;
        }
    // Using KInvalidIndex here is not to init index in CreateGridDataL().
    iSpeedPrivate->CreateGridDataL( iGrid, KInvalidIndex );
    iSpeedPrivate->SetContainerWindowL( *this );
    iSpeedPrivate->SetRefreshObserver( this );

    // shortcut value must be set after adding items the the list.
    iGrid->SetShortcutValueFromPrevList( iGrid->ShortcutValueForNextList() );
    
    SetStatusPaneL();

    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iGrid->SetCurrentDataIndex(0);
        }
    ActivateL();
    iAlreadyActive = EFalse;
    ivmbxvariation = CRepository::NewL( KCRUidVideoMailbox );
    ivmbxvariation->Get( KVideoMbxSupport, iVmbxsupported );
    delete ivmbxvariation;
    ivmbxvariation = NULL;
    LongTapDetectorL();
    }

// ---------------------------------------------------------
// CSpdiaContainer::RefreshGrid
// It calls spdctrl CreateGridDataL to draw the grid on skin change.
// 
// ---------------------------------------------------------
void CSpdiaContainer::RefreshGrid()
    {
		TRAP_IGNORE( iSpeedPrivate->CreateGridDataL( iGrid, -1 ) );
    }

// Destructor
CSpdiaContainer::~CSpdiaContainer()
    {
    delete iGrid;
    if ( iSpeedPrivate )
        {    
        iSpeedPrivate->SetGridDataAsNull();
        }
    delete iNumberGrouping;        
    FeatureManager::UnInitializeLib();

    if ( iDialog )
        {
        delete iDialog;
        }
    delete iLongTapDetector;
    }

// ---------------------------------------------------------
// CSpdiaContainer::Control
// Get a grid-model reference
// ---------------------------------------------------------
CSpeedDialPrivate& CSpdiaContainer::Control()
    {
    return *iSpeedPrivate;
    }

// ---------------------------------------------------------
// CSpdiaContainer::IsVoiceMailBox
// Check Voice Mail Box number
// ---------------------------------------------------------
TBool CSpdiaContainer::IsVoiceMailBox(TInt aIndex) const
    {
    if (aIndex < 0)
        {
        aIndex = iGrid->CurrentDataIndex();
        }
    return  iSpeedPrivate->VMBoxPosition() == aIndex? ETrue: EFalse;
    }
// ---------------------------------------------------------
// CSpdiaContainer::IsVdoMailBox
// Check Voice Mail Box number
// ---------------------------------------------------------
TBool CSpdiaContainer::IsVdoMailBox(TInt aIndex) const
    {
    if ( !iVmbxsupported )
        {
        // Video Mailbox feature is disabled
        return EFalse;
        }
        
    if ( aIndex < 0 )
        {
        aIndex = iGrid->CurrentDataIndex();
        }

    return iSpeedPrivate->VdoMBoxPosition() == aIndex ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// CSpdiaContainer::CurGridPosition
// Get Current grid position
// ---------------------------------------------------------
TInt CSpdiaContainer::CurGridPosition() const
    {
   	return iGrid->CurrentDataIndex();
    }

// ----------------------------------------------------
// CSpdiaContainer::UpdatePhoneNumberL
// 
// ----------------------------------------------------
//
void CSpdiaContainer::UpdatePhoneNumberL(TInt aIndex)
    {
    //STATIC_CAST(CDesCArray* ,iGrid->Model()->ItemTextArray())->Reset();
    iSpeedPrivate->CreateGridDataL(iGrid, aIndex);
    iView->LoadCba(MenuResourceID());
    }

// ---------------------------------------------------------
// CSpdiaContainer::GetHelpContext
// This function is called when Help application is launched.  
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpdiaContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KUidSpdia;
    aContext.iContext = KSDM_HLP_APP;
    }
// ---------------------------------------------------------
// CSpdiaContainer::HandleResourceChange()
// Handle layout and skin change event.  
// ---------------------------------------------------------
//
void CSpdiaContainer::HandleResourceChange( TInt aType )
    {   
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Refresh when grid is in foreground 
        if ( iSpeedPrivate->State() == CSpeedDialPrivate::STATE_IDLE && 
             iSpeedPrivate->GridStatus() == CSpeedDialPrivate::EGridUse )
            {
            // if refresh grid here, termination will be interrupt when edit type is full screen QWERTY.
            RefreshGrid();
            iGrid->HandleResourceChange( aType );

            // Reload menubar when swap between landscape and portrait mode.
            iView->LoadCba( MenuResourceID() );
            DrawDeferred();
            }
        TRAP_IGNORE( SetStatusPaneL() );
        }
    else
        {
        CCoeControl::HandleResourceChange( aType );
        TRAP_IGNORE( SetLayoutL() );
        }

    // To hide the scrollbar of the grid
    TRAP_IGNORE( iGrid->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff ) );
    }

// ---------------------------------------------------------
// CSpdiaContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSpdiaContainer::SizeChanged()
    {
    iView->iStatusPane->DrawNow();
    
    TRAP_IGNORE( SetLayoutL() );
    DrawNow();
    }
// ---------------------------------------------------------
// CSpdiaContainer::SetLayoutL
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSpdiaContainer::SetLayoutL()
    {
    TAknLayoutRect areaRect;
    TAknLayoutRect gridQdialPaneRect;
    TAknLayoutRect mainQdialPaneRect;
    TRect        mainPaneRect ;
    TRect        statusPaneRect;
    TRect        mainRect;
    TInt         width = 0;
    TInt         height =0;
 	
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneRect );
            if( !AknLayoutUtils::PenEnabled() )
                {
                mainPaneRect.iTl = statusPaneRect.iTl;
                mainRect = mainPaneRect;
                mainPaneRect.iBr.iY = mainPaneRect.iBr.iY - statusPaneRect.iTl.iY;
                }
            else
                {
                mainPaneRect.iTl.iY -= mainPaneRect.iTl.iY;
                mainPaneRect.iTl.iX -=statusPaneRect.iTl.iY;
                mainRect = mainPaneRect;
                }
            mainQdialPaneRect.LayoutRect( mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane().LayoutLine() );
            gridQdialPaneRect.LayoutRect( mainQdialPaneRect.Rect(),AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine() );
            AknLayoutUtils::LayoutControl( iGrid, mainQdialPaneRect.Rect(), AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine() );
            TAknWindowComponentLayout areaNew = AknLayoutScalable_Apps::cell_qdial_pane( 0, 0 ).LayoutLine();
  
            areaRect.LayoutRect( mainQdialPaneRect.Rect() ,areaNew.LayoutLine() );
		 
            width  = areaRect.Rect().Width();
            height = areaRect.Rect().Height();
            }
        else
            {
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneRect );
            mainPaneRect.iTl= statusPaneRect.iTl;
            mainRect = mainPaneRect;
            mainQdialPaneRect.LayoutRect( mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane().LayoutLine() );
            gridQdialPaneRect.LayoutRect( mainQdialPaneRect.Rect(),AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine() );
            AknLayoutUtils::LayoutControl( iGrid, mainQdialPaneRect.Rect(), AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine() );
            TAknWindowComponentLayout areaNew = AknLayoutScalable_Apps::cell_qdial_pane( 0, 0 ).LayoutLine();
            areaRect.LayoutRect( mainQdialPaneRect.Rect() ,areaNew.LayoutLine() );
		 
            width  = ( mainPaneRect.Width() / 3 ) - 1;
            height = ( mainPaneRect.Height() / 3 ) - 7;
            }		
        }
    else
        {
        AknLayoutUtils::LayoutControl( iGrid, Rect(), AppLayout::Speed_Dial_descendants_Line_1() );
        TAknWindowLineLayout area = AppLayout::cell_qdial_pane( 0, 0 );		        
        areaRect.LayoutRect( Rect(), area );			
        }
    TInt language = User::Language();
    TBool LeftToRight( ETrue );

    // When Arabic and Hebrew input is in use, the Arabic and Hebrew browsing order
    // is not followed. Instead, the browsing order is the same as in Western variants.
    if ( language == ELangUrdu /*|| language == ELangArabic*/ )
        {
        LeftToRight = EFalse;
        }
    TRAP_IGNORE( iGrid->SetLayoutL(
            EFalse,                // aVerticalOrientation
            LeftToRight,           // aLeftToRight
            ETrue,                 // aTopToBottom
            KCellRowCount,         // aNumOfItemsInPrimaryOrient
            KCellColCount,         // aNumOfItemsInSecondaryOrient
            TSize( width, height ) // NewScalableUI changes
            )
        );
      
    TRAP_IGNORE( iGrid->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff ) );
    iGrid->ScrollBarFrame()->VerticalScrollBar()->SetRect( TRect( 0,0,0,0 ) );
    CFormattedCellListBoxData* data = iGrid->ItemDrawer()->FormattedCellData();

    data->SetTransparentSubCellL( SDM_TN_CIF_TURNED, ETrue );  //5
    data->SetTransparentSubCellL( SDM_TN_CIF, ETrue );         //7
    data->SetTransparentSubCellL( SDM_TN_VGA_TURNED, ETrue );  //9
    data->SetTransparentSubCellL( SDM_TN_VGA, ETrue );         //11
    data->SetTransparentSubCellL( SDM_TN_OTHER, ETrue );       //13

    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        data->SetBackgroundSkinStyle( &KAknsIIDQsnBgAreaMainQdial, mainRect );
        data->SetSkinStyle( &KAknsIIDQsnBgAreaMainQdial, mainRect );
        iSpeedPrivate->SetLayout( mainPaneRect );
        }
    else
        {
        data->SetBackgroundSkinStyle( &KAknsIIDQsnBgAreaMainQdial, Rect() );
        data->SetSkinStyle( &KAknsIIDQsnBgAreaMainQdial, Rect() );
        iSpeedPrivate->SetLayout( Rect() );
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::CountComponentControls
//
//  
// ---------------------------------------------------------
//
TInt CSpdiaContainer::CountComponentControls() const
    {
    return 2; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CSpdiaContainer::ComponentControl
//
// ---------------------------------------------------------
//
CCoeControl* CSpdiaContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iSpeedPrivate;
        case 1:
            return iGrid;

        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::Draw
// 
// ---------------------------------------------------------
//
void CSpdiaContainer::Draw(const TRect& aRect) const
    {
    iSpeedPrivate->Draw( aRect );
    }

// ---------------------------------------------------------
// CSpeedContainer::OfferKeyEventL
// processing of a key event
// ---------------------------------------------------------
//
TKeyResponse CSpdiaContainer::OfferKeyEventL(
                    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if ( iSpeedPrivate->GetWait()->IsStarted() )
        {    
        return EKeyWasConsumed;
        }

    TInt index( 0 );
    
    if ( ( iView->IsFocused() ) && ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyDevice3 ) /*|| aKeyEvent.iCode == EKeyDevice3*/ )
        {
        index = iGrid->CurrentDataIndex();
        if ( !IsVoiceMailBox() && !IsVdoMailBox() && !iSpeedPrivate->Contact( index ) )
            {
            iView->CmdAssignNumberL();
            }
        else
            {
            CEikMenuBar* menuBar = iView->MenuBar();
            TInt barId( R_SPDIA_MENUBAR_VIEW_OK_ASSIGN );
            if ( IsVoiceMailBox() || IsVdoMailBox() )
                {
                barId = R_SPDIA_MENUBAR_VIEW_OK_CALL;
                }
            else if ( iSpeedPrivate->Contact( index ) )
                {
                barId = R_SPDIA_MENUBAR_VIEW_OK_CALL_VIDEO;                
                }
            menuBar->SetMenuType( CEikMenuBar::EMenuContext );
            menuBar->SetMenuTitleResourceId( barId );

            menuBar->TryDisplayMenuBarL();
            // For the reason at the time of being canceled, a menu is changed.
            menuBar->SetMenuTitleResourceId( MenuResourceID() );
            menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
            }
        return EKeyWasConsumed;
        }
    // Check whether the focus exists.
    // If not, updated the option menu
    else if ( !iView->IsFocused() )
    	{
    	// LOGTEXT( _L("CR addition: handle HW key shortcuts event in OfferKeyEventL()") );
    	iGrid->SetCurrentDataIndex( 0 );
        iView->LoadCba( MenuResourceID() );
        MiddleSoftKeyL();
    	}

    TKeyResponse exitCode( EKeyWasNotConsumed );
    TChar code( aKeyEvent.iCode );
    MiddleSoftKeyL();
    TInt language = User::Language();
    // When Arabic and Hebrew input is in use, the Arabic and Hebrew browsing order
    // is not followed. Instead, the browsing order is the same as in Western variants.
    if ( language == ELangUrdu /*|| language == ELangArabic*/ )
        {
        if ( code == EKeyLeftArrow )
            {
            index = iGrid->CurrentDataIndex();
            if ( index == KMaxIndex )
                {
                iGrid->SetCurrentDataIndex( 0 );
                iView->LoadCba( MenuResourceID() );
                return EKeyWasConsumed; 
                }
            }
		
        if ( code == EKeyRightArrow )
            {
            index = iGrid->CurrentDataIndex();
            if ( index == 0 )
                {
                iGrid->SetCurrentDataIndex( KMaxIndex );
                iView->LoadCba( MenuResourceID() );
                return EKeyWasConsumed; 
                }
            }	
        }
    else
        {
        if ( code == EKeyRightArrow )
            {
            index = iGrid->CurrentDataIndex();
            if ( index == KMaxIndex )
                {
                iGrid->SetCurrentDataIndex( 0 );
                iView->LoadCba( MenuResourceID() );
                return EKeyWasConsumed; 
                }
            }
	
        if ( code == EKeyLeftArrow )
            {
            index = iGrid->CurrentDataIndex();
            if ( index == 0 )
                {
                iGrid->SetCurrentDataIndex( KMaxIndex );
                iView->LoadCba( MenuResourceID() );
                return EKeyWasConsumed; 
                }
            }
        }

    if ( aKeyEvent.iScanCode == EStdKeyYes )
        {
        // Send key is pressed, when menu or dialog is displayed, the call won't be created.
        if ( aType == EEventKey 
		     && !static_cast<CSpdiaAppUi*>( CEikonEnv::Static()->AppUi() )->IsDisplayingDialog() 
		     && !iView->MenuBar()->IsDisplayed() )
            {
            CallCreateL();
            }
        return EKeyWasConsumed;
        }
        
    exitCode = iGrid->OfferKeyEventL( aKeyEvent, aType );
    if ( exitCode != EKeyWasNotConsumed )
        {
        DrawDeferred();
        if ( code.IsDigit() )
            {
            if ( aKeyEvent.iCode - KShortcutKey0 > 0 )
                {
                iGrid->SetCurrentDataIndex( 
                        iSpeedPrivate->Index( aKeyEvent.iCode - KShortcutKey0 ) );
                MiddleSoftKeyL();
                iView->LoadCba( MenuResourceID() );
                exitCode = EKeyWasConsumed;
                }
            }
        else if ( code == EKeyLeftArrow ||
                code == EKeyRightArrow ||
                code == EKeyUpArrow ||
                code == EKeyDownArrow )
            {
            iView->LoadCba( MenuResourceID() );
            MiddleSoftKeyL();
            }
        }
    return exitCode;
    }

// ---------------------------------------------------------
// CSpdiaContainer::HandleDatabaseEventL
// Handles an database event of type aEventType.
// ---------------------------------------------------------
//
void CSpdiaContainer::HandleDatabaseEventL(
         TContactDbObserverEvent aEvent)
    {
    switch( aEvent.iType )
        {
        case EContactDbObserverEventContactChanged:
        case EContactDbObserverEventContactDeleted:
        case EContactDbObserverEventContactAdded:
            break;
        case EContactDbObserverEventSpeedDialsChanged:
            if ( iDialog )
                {
                UpdatePhoneNumberL();
                }
            iNeedUpdate = ETrue;
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::CallCreateL
// Handles send key press.
// ---------------------------------------------------------
//
void CSpdiaContainer::CallCreateL()
    {
    TPhCltTelephoneNumber number;
    TInt index( iGrid->CurrentDataIndex() );
    TInt numberType( ESpDialPbkFieldIdNone );
    TBool call( EFalse );

    // voice mailbox
    if ( IsVoiceMailBox() || IsVdoMailBox() )
        {
        //  call = iSpeedPrivate->VoiceMailL(number);
        iView->CmdNumberCallL(KAiwCmdCall, ETrue );
        TInt type = iSpeedPrivate->VoiceMailType();
        if( type == EVmbxIP )
            {
            numberType = ESpDialPbkFieldIdVOIP;
            }
        else if( type == EVmbxVideo )
            {
            numberType = ESpDialPbkFieldIdPhoneNumberVideo;
            }
        }
    else
        {
        // if contact does not exit call should not be created.
        if ( iSpeedPrivate->Contact( index ) != NULL )
            {
            call = ETrue;
            }
        
        if ( !iNoAssignedShown && iSpeedPrivate->Contact( index ) == NULL )
            {
            CAknQueryDialog* dlg = 
                        CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
            iNoAssignedShown = ETrue;
            
            // Assign it now?
            if ( !dlg->ExecuteLD( R_SPDIA_QUERY_ASSIGN ) || 
                                    !iView->CmdAssignNumberL() )
                {
                call = EFalse;
                }
            else
                {
                call = ETrue;
                }
            
            iNoAssignedShown = EFalse;
            }
        if ( call )
            {
            number = iSpeedPrivate->PhoneNumber( index );
            numberType = iSpeedPrivate->NumberType( index );
            }
        }

    if ( call )
        {
        iView->NumberCallL( number, numberType);
        }
    }

// ----------------------------------------------------
// CSpdiaContainer::MenuResourceID
// 
// ----------------------------------------------------
//
TInt CSpdiaContainer::MenuResourceID()
    {
    TInt id( R_SPDIA_MENUBAR_VIEW_VOICE );
    TInt index( iGrid->CurrentDataIndex() );

    if ( ( !IsVoiceMailBox() ) && ( !IsVdoMailBox() ) )
        {
        id = iSpeedPrivate->Contact(index) != NULL ?
            R_SPDIA_MENUBAR_VIEW : R_SPDIA_MENUBAR_VIEW_ASSIGN;
        }
    return id;
    }

// ---------------------------------------------------------
// CSpdiaContainer::ShowViewNumberL
// 
// ---------------------------------------------------------
void CSpdiaContainer::ShowViewNumberL()
    {
    TInt gridIndex( CurGridPosition() );
    TInt textWidth( SDM_VIEW_NUMBER_TEXT_MAXWIDTH );
    MVPbkStoreContact* contact( iSpeedPrivate->Contact( gridIndex ) );
    
    iSpeedPrivate->SetCurrentGridIndex( gridIndex );

    if ( contact == NULL )
        {
        if( iDialog )
            {
            delete iDialog;
            }
        iDialog = NULL;
        iDialog = new( ELeave ) CSpdiaNoteDialog( &iDialog );
        iDialog->PrepareLC( R_SPDIA_NOTE_VIEWNUMBER );
        iDialog->RunLD();
        if ( iDialog )
            {
            delete iDialog;
            }
        iDialog = NULL;
        return;
        }	
    // The assign was deleted while the view number was displayed.
    delete iDialog;
    iDialog = NULL;

    iDialog = new(ELeave) CSpdiaNoteDialog( &iDialog );
    iDialog->PrepareLC( R_SPDIA_NOTE_VIEWNUMBER );

    CArrayPtr<CGulIcon>* imgArray = iSpeedPrivate->IconArray();

    // icon
    TInt imgIndex( iSpeedPrivate->SpdIconIndex(gridIndex) );

    if(imgIndex >= 0)
        {
        iDialog->SetIconL( CreateBitmapL(*(*imgArray)[imgIndex]) );
        textWidth -= SDM_ADDITIONAL_MARGIN_FOR_NUMBER_TYPE_ICON; 
        }

    // thumbnail
    imgIndex = iSpeedPrivate->ThumbIndex( gridIndex );
    CEikImage* image;
		if ( imgIndex >= 0 )
        {
        image = CreateScaledBitmapL( *(*imgArray)[imgIndex] );
        iDialog->SetImageL( image );
        TSize size(image->Bitmap()->SizeInPixels());
        textWidth -= size.iWidth + SDM_MARGIN_BETWEEN_TEXT_AND_IMAGE;
        }

    HBufC* title = iSpeedPrivate->ContactTitleL( contact, ETrue);
    CleanupStack::PushL(title);

    // Information
    CDesCArray* locStr = new(ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( locStr );
    const CFont* font = iEikonEnv->NormalFont();
    if ( AknLayoutUtils::Variant() == EApacVariant )
        {
        font = ApacPlain16();
        }

    TPtr ptr = title->Des();
    TAknLayoutRect dialogRect;
    TAknLayoutText textLayout;
    TAknTextComponentLayout area;
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TAknWindowLineLayout tempWidth = AknLayoutScalable_Avkon::popup_note_image_window().LayoutLine();
        dialogRect.LayoutRect(Rect(), tempWidth);
        area = AknLayoutScalable_Avkon::popup_note_image_window_t2((imgIndex >= 0) ? 2 : 0 );
			
        textLayout.LayoutText( dialogRect.Rect() ,area );
        textWidth = textLayout.TextRect().Width();
        TextUtils::ClipToFit ( ptr, *AknLayoutUtils::FontFromId( area.Font()), textWidth );
        }
    else
        {
        TextUtils::ClipToFit ( ptr, *font,textWidth);
        }
    locStr->AppendL( title->Des() );

    const TDesC& phoneNumber = iSpeedPrivate->PhoneNumber( gridIndex );
    HBufC* phoneNumberBuf;
	
    if ( iNumberGrouping )
        {
        iNumberGrouping->Set(phoneNumber);
        phoneNumberBuf = iNumberGrouping->FormattedNumber().AllocLC();			
        }        
    else
        {
        phoneNumberBuf = phoneNumber.AllocLC();
        }
    TPtr phoneNumberPtr = phoneNumberBuf->Des();
    AknTextUtils::LanguageSpecificNumberConversion( phoneNumberPtr );

    locStr->AppendL( phoneNumberPtr );
    HBufC* addprompt = StringLoader::LoadLC(R_SPDIA_TEXT_PROMPT, *locStr);
    
    HBufC* prompt = StringLoader::LoadL( R_SPDIA_TEXT_VIEWNUM, iSpeedPrivate->Number(gridIndex) );
    prompt = prompt->ReAlloc( prompt->Length() + addprompt->Length() );
    CleanupStack::PushL(prompt);
    prompt->Des().Append( addprompt->Des() );

    iDialog->SetTextL( *prompt );
    iDialog->SetPhoneNumberL( (*locStr)[1] );

    CleanupStack::PopAndDestroy( 5 ); // title, locStr, prompt, addprompt, phoneNumberBuf
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TAknWindowLineLayout tempWidth = AknLayoutScalable_Avkon::popup_note_image_window().LayoutLine();
        dialogRect.LayoutRect(Rect(), tempWidth);
        area = AknLayoutScalable_Avkon::popup_note_image_window_t3((imgIndex >= 0) ? 2 : 0 );
		
        textLayout.LayoutText(dialogRect.Rect() ,area );
        textWidth = textLayout.TextRect().Width();
        }
    iDialog->ClipPhoneNumberFromBeginningL( textWidth , 0 );
    iDialog->RunLD();
    }

// ---------------------------------------------------------
// CSpdiaContainer::DoUpdate
// 
// ---------------------------------------------------------
void CSpdiaContainer::DoUpdateL()
    {
    MiddleSoftKeyL();
    if ( iSpeedPrivate && iSpeedPrivate->UpdateFlag() )
        { 
        DrawNow();        
        if ( iDialog )
            {
            ShowViewNumberL();   
            iSpeedPrivate->SetUpdateFlag( EFalse );
            }     
        iView->LoadCba( MenuResourceID() );
        }   
    }

// ---------------------------------------------------------
// CSpdiaContainer::Grid
// 
// ---------------------------------------------------------
CAknQdialStyleGrid* CSpdiaContainer::Grid()
    {
    return iGrid;
    }

// ---------------------------------------------------------
// CSpdiaContainer::CreateBitmapL
// 
// ---------------------------------------------------------
CEikImage* CSpdiaContainer::CreateBitmapL(const CGulIcon& aSrc)
    {
    if( aSrc.Bitmap() == NULL || aSrc.Mask() == NULL )
    	{
    	return NULL;
    	}
    CEikImage* image = new(ELeave) CEikImage;
    image->SetPictureOwnedExternally(EFalse);
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    image->SetBitmap(bitmap);
    User::LeaveIfError(bitmap->Duplicate(aSrc.Bitmap()->Handle()));
    CFbsBitmap* bmpMask = aSrc.Mask();
    if ( bmpMask )
        {
        CFbsBitmap* mask = new(ELeave) CFbsBitmap;
        image->SetMask(mask);
        User::LeaveIfError( mask->Duplicate( aSrc.Mask()->Handle() ) );
        }
    return image;
    }
    
// ---------------------------------------------------------
// CSpdiaContainer::FocusChanged
// 
// ---------------------------------------------------------
void CSpdiaContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if( iGrid )
        {
        iGrid->SetFocus( IsFocused() );
        }
    }
// ---------------------------------------------------------
// CSpdiaContainer::CreateScaledBitmapL
// 
// ---------------------------------------------------------
CEikImage* CSpdiaContainer::CreateScaledBitmapL(const CGulIcon& aSrc)
    {
    TRect 		   mainPaneRect ;
    TRect 		   statusPaneRect;
    TRect 		   mainRect;
    TAknLayoutRect windowParentRect;
    TAknLayoutRect imageRect;
    
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
        mainPaneRect.iTl = statusPaneRect.iTl;
        mainRect = mainPaneRect;
        mainPaneRect.iBr.iY = mainPaneRect.iBr.iY - statusPaneRect.iTl.iY;
        windowParentRect.LayoutRect(mainRect, AknLayoutScalable_Avkon::popup_note_image_window(0));
        imageRect.LayoutRect(windowParentRect.Rect(), AknLayoutScalable_Avkon::popup_note_image_window_g2(1));
        }
    else
        {	
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
        mainPaneRect.iTl= statusPaneRect.iTl;
        mainRect = mainPaneRect;
        windowParentRect.LayoutRect(mainRect, AknLayoutScalable_Avkon::popup_note_image_window(0));
        imageRect.LayoutRect(windowParentRect.Rect(), AknLayoutScalable_Avkon::popup_note_image_window_g2(1));;
        }
     
    TSize imagesize(imageRect.Rect().Size());
     
    CFbsBitmap* bmporg= new (ELeave) CFbsBitmap();
    bmporg->Create(aSrc.Bitmap()->SizeInPixels(), aSrc.Bitmap()->DisplayMode() );
    bmporg->Duplicate(aSrc.Bitmap()->Handle());
       
    CAknIcon* tmpIcon = CAknIcon::NewL();
    tmpIcon->SetBitmap( bmporg);
    CAknIcon* scaledIcon = AknIconUtils::CreateIconL(tmpIcon);
    AknIconUtils::SetSize(scaledIcon->Bitmap(),imagesize,EAspectRatioNotPreserved);

    CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
    TInt err1 = bmp->Duplicate( scaledIcon->Bitmap()->Handle() );

    // CEikImage	
    CEikImage* image = new(ELeave) CEikImage;
    image->SetPictureOwnedExternally( EFalse );
    image->SetBitmap( bmp );
    delete scaledIcon;
  
    return image;
    }

// ---------------------------------------------------------
// CSpdiaContainer::VoiceMailDialogLaunched(
// 
// ---------------------------------------------------------
void CSpdiaContainer::VoiceMailDialogLaunched(TBool aBool)
    {
    iVoiceDialog = aBool;
    }
// ------------------------------------------------------
// CSpdiaContainer::MiddleSoftKeyL
// 
// ----------------------------------------------------------

void CSpdiaContainer::MiddleSoftKeyL()
    {
    TInt index( Grid()->CurrentDataIndex() );
    if ( ( index < 0 ) || ( index > 8 ) )
        {
        return;
        }
    if ( IsVoiceMailBox() || IsVdoMailBox() )
        {
        //If cba is not changed back here, changing to context sensitive menu does not work
        if ( iView->IsSettingType() )
            {
            iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_BACK_OPTION );
            }
        else
            {
            iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_EXIT_OPTION ); 
            }
        iView->MenuBar()->SetContextMenuTitleResourceId( R_SPDIA_MENUBAR_VIEW_OK_CALL );
        }
    else if ( Control().Contact( index ) != NULL )
        {
        if ( iView->IsSettingType() )
            {
            iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_BACK_OPTION );
            }
        else
            {
            iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_EXIT_OPTION ); 
            }
        iView->MenuBar()->SetContextMenuTitleResourceId( R_SPDIA_MENUBAR_VIEW_OK_CALL_VIDEO );
        }
    else
        {
        //when number not assigned in 5.0, MSK versions, display "Assign"
        if ( AknLayoutUtils::MSKEnabled() )
            {
            if ( iView->IsSettingType()  )
                {
                iView->SetCba( R_SPDIA_VIEW_OPTIONS_BACK_ASSIGN );
                }
            else
                {
                iView->SetCba( R_SPDIA_VIEW_OPTIONS_EXIT_ASSIGN ); 
                }
            }
        else if(AknLayoutUtils::PenEnabled())
            {
            if ( iView->IsSettingType() )
                {
                iView->SetCba( R_SPDIA_VIEW_OPTIONS_BACK );
                }
            else
                {
                iView->SetCba( R_SPDIA_VIEW_OPTIONS_EXIT ); 
                }
            }
        else
            {
            iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_EXIT_OPTION );
            if ( iView->IsSettingType()  )
                {
                iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_BACK_OPTION );
                }
            else
                {
                iView->SetCba( R_SPDIA_SOFTKEYS_OPTION_EXIT_OPTION ); 
                }
            iView->MenuBar()->SetContextMenuTitleResourceId( R_SPDIA_MENUBAR_VIEW_OK_ASSIGN );
            }
        }
    }
		
// ----------------------------------------------------------------------------
// CSpdiaContainer::HandlePointerEventL
// Function to handle all touch (pointer events)
// ----------------------------------------------------------------------------
void CSpdiaContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if ( !iView->IsAssignCompleted() )
        {
        return;
        }

    // check if the position is in grid view
    CAknGridView* gridView = iGrid->GridView();
    TInt itemIndex = 0;        
    TRect visibleItemsRect(gridView->ViewRect().iTl, 
                 TSize(gridView->ItemSize(itemIndex).iWidth * gridView->NumberOfColsInView(), 
                 gridView->ItemSize(itemIndex).iHeight * gridView->NumberOfRowsInView()));
    if (!visibleItemsRect.Contains(aPointerEvent.iPosition))
        {
        return;
        }

    // Check if touch is enabled or not.
    if ( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
  
    if ( ( aPointerEvent.iType == TPointerEvent::EButton1Down ) ||
             ( aPointerEvent.iType == TPointerEvent::EButton1Up ) )
    	{
        if ( iLongTapDetector )
            {
            iLongTapDetector->PointerEventL( aPointerEvent );
            }
    	}
    // Process only once when the grid control has been pressed
    if ( ( aPointerEvent.iType == TPointerEvent::EButton1Down ) ||
         ( aPointerEvent.iType == TPointerEvent::EDrag ) ||
         ( aPointerEvent.iType == TPointerEvent::EButton1Up ) )
        {        
        // Ask the grid to process the handlepointerevent
        iGrid->HandlePointerEventL( aPointerEvent );
        
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        	{
        	iButton1DownIndex = iGrid->CurrentDataIndex();
        	}
        // If already highlighted, activate the grid element
        if ( iGrid->CurrentDataIndex() == iButton1DownIndex ) 
            {
            TInt pointedItemIndex;
            if ( ( aPointerEvent.iType == TPointerEvent::EButton1Up ) &&
                iGrid->View()->XYPosToItemIndex(
                    aPointerEvent.iPosition, pointedItemIndex ) &&
                ( pointedItemIndex == iButton1DownIndex ) )
                {
                if ( iLongTapUsed == EFalse )
                    {
                    // Need filtrate IsVoiceMailBox and IsVdoMailBox
                    if ( !IsVoiceMailBox() && !IsVdoMailBox() 
                         && iSpeedPrivate->Contact( iButton1DownIndex ) == NULL )
                        {
                        // Open phonebook fetch list
                        iView->CmdAssignNumberL();	
                        // No need to launch the menu
                        return;
                        }
                    else if ( iSpeedPrivate->Contact( iButton1DownIndex ) != NULL )
                        {
                        CallCreateL( );
                        return;
                        }
                    CEikMenuBar* menuBar = iView->MenuBar();
                    TInt barId(R_SPDIA_MENUBAR_VIEW_OK_ASSIGN);
            
                    // If focus is on voicemailbox
                    if ( IsVoiceMailBox() || IsVdoMailBox() )
                        {
                        barId = R_SPDIA_MENUBAR_VIEW_OK_CALL;
                        }
                    // If a number is assigned then provide call option
                    else if ( iSpeedPrivate->Contact( iButton1DownIndex ) != NULL )
                        {
                        barId = R_SPDIA_MENUBAR_VIEW_OK_CALL_VIDEO;
                        }
                    menuBar->StopDisplayingMenuBar();
                    menuBar->SetMenuType( CEikMenuBar::EMenuContext );
                    menuBar->SetMenuTitleResourceId( barId );
                    menuBar->TryDisplayMenuBarL();
                    // At the time of being canceled, a menu is changed.
                    menuBar->SetMenuTitleResourceId( MenuResourceID() );
                    menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
                    }
                iLongTapUsed = EFalse;
                }
            }
        // Else just reload the cba buttons based on the context
        else 
            {
            MiddleSoftKeyL();
            iView->LoadCba( MenuResourceID() );
            }
        }
    }

// --------------------------------------------------------------------------
// CSpdiaContainer::LongTapDetectorL
// --------------------------------------------------------------------------
//    
CAknLongTapDetector& CSpdiaContainer::LongTapDetectorL()
    {
    if ( !iLongTapDetector )
        {
        iLongTapDetector = CAknLongTapDetector::NewL( this );
        }
    return *iLongTapDetector;
    }
    
// --------------------------------------------------------------------------
// CSpdiaContainer::HandleLongTapEventL
// --------------------------------------------------------------------------
//    
 void CSpdiaContainer::HandleLongTapEventL(
        const TPoint& aPenEventLocation, 
        const TPoint& /*aPenEventScreenLocation*/ )
    {
    // Get the Current Data Index
    TInt itemIndex( KErrNotFound );

    // Get position when user press screen
    iGrid->View()->XYPosToItemIndex( aPenEventLocation, itemIndex );

    // Compare two index
    if ( AknLayoutUtils::PenEnabled() && ( itemIndex == iGrid->CurrentDataIndex() ) )
        {
        iLongTapUsed = ETrue;   	    
        TInt index = iGrid->CurrentDataIndex();
        if ( IsVoiceMailBox() || IsVdoMailBox())
            {
            CEikMenuBar* menuBar = iView->MenuBar();
            TInt barId = R_SPDIA_MENUBAR_VIEW_OK_CALL;
            
            menuBar->StopDisplayingMenuBar();
            menuBar->SetMenuType(CEikMenuBar::EMenuContext);
            menuBar->SetMenuTitleResourceId(barId);
            menuBar->TryDisplayMenuBarL();
            //For the reason at the time of being canceled, a menu is changed.
            menuBar->SetMenuTitleResourceId(MenuResourceID());
            menuBar->SetMenuType(CEikMenuBar::EMenuOptions);
            }
        }
    }
    
// --------------------------------------------------------------------------
// CSpdiaContainer::SetStatusPaneL
// --------------------------------------------------------------------------
//    
 void CSpdiaContainer::SetStatusPaneL()
     {
     CEikStatusPane* statusPane = iView->iStatusPane;

     if ( Layout_Meta_Data::IsLandscapeOrientation() )
         {
         statusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
         statusPane->MakeVisible( ETrue );
         statusPane->DrawNow();
         }
     else
         {
         TInt resouceId = statusPane->CurrentLayoutResId();

         if ( resouceId != R_AVKON_STATUS_PANE_LAYOUT_USUAL && 
             resouceId != R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT )
             {
             statusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
             }

         if ( statusPane->IsVisible()&&(!iView->IsShowVmbxDlg()) )
             {
             statusPane->MakeVisible( EFalse );
             }

         TRect mainPaneRect;
         AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );

         TRect statusPaneRect;
         AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneRect );
         if ( !iView->IsShowMsgDlg() && !iView->IsShowVmbxDlg())
             {
             mainPaneRect.iTl = statusPaneRect.iTl;
             }
         SetRect( mainPaneRect );
         }
     }

 // --------------------------------------------------------------------------
 // CSpdiaContainer::RefreshDialog
 // from MRefreshObserver
 // --------------------------------------------------------------------------
 // 
 void CSpdiaContainer::RefreshDialog()
     {
     if ( iDialog )
         {
         TRAP_IGNORE( DoUpdateL() );
         }
     }
 
 // --------------------------------------------------------------------------
 // CSpdiaContainer::RefreshDialog
 // From MEikListBoxObserver, Handles an event of listbox.
 // --------------------------------------------------------------------------
 // 
 void CSpdiaContainer::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    switch(aEventType)
        {
        case EEventPenDownOnItem:
            iView->LoadCba( MenuResourceID() );
            MiddleSoftKeyL();
            break;
        }
    }
 
 // --------------------------------------------------------------------------
 // CSpdiaContainer::GetViewDialogStatus
 // Check whether the view note is displaying when making speeddial to background.
 // --------------------------------------------------------------------------
 // 
 CSpdiaNoteDialog* CSpdiaContainer::GetViewDialogStatus()
    {
    return iDialog;
    }
 // End of File  
