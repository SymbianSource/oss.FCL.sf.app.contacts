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
* Description:     This class provides drawing a shadow.
*
*/




// INCLUDE FILES
#include "SpdiaControl.hrh"
#include "SpdiaControl.h"
#include "SpdiaGrid.h" 
#include "Speeddial.laf" 

#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <Spdctrl.rsg>
const TInt KItemPrimary(3);
const TInt KItemSecondary(3);

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CSpdiaGridDlg::NewL()
// 
// ---------------------------------------------------------
CSpdiaGrid* CSpdiaGrid::NewL(const CSpdiaControl& aControl)
    {
    CSpdiaGrid* self = new (ELeave) CSpdiaGrid(aControl);
    return self;
    }

CSpdiaGrid::CSpdiaGrid()
    {
    }

CSpdiaGrid::CSpdiaGrid(const CSpdiaControl& aControl)
    {
    iControl = CONST_CAST(CSpdiaControl*, &aControl);
    }

CSpdiaGrid::~CSpdiaGrid()
    {
    }

void CSpdiaGrid::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    AknsDrawUtils::Background(
       AknsUtils::SkinInstance(),
       AknsDrawUtils::ControlContext( iControl ),
       this,
       gc,
       iControl->Rect() );
    if ( iControl )
        {
        iControl->DrawShadow( gc );
        }
    CAknGrid::Draw(aRect);
    }

void CSpdiaGrid::SizeChanged()
    {
    TRAP_IGNORE( SizeChangedL() );
    }
    
void CSpdiaGrid::SizeChangedL()
    {
    //Scalable UI.
    TAknLayoutRect areaLayout;
    TRect mainPaneRect ;
    TRect statusPaneRect;
   		
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TAknLayoutRect mainQdialPaneRect;
        TAknLayoutRect gridQdialPaneRect;
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
            mainPaneRect.iTl = statusPaneRect.iTl;
            }
        else
            {
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
            mainPaneRect.iTl= statusPaneRect.iTl;
            }
        mainQdialPaneRect.LayoutRect(mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane().LayoutLine());
        gridQdialPaneRect.LayoutRect(mainQdialPaneRect.Rect(),AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine());
        TAknWindowLineLayout area = AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine();
        areaLayout.LayoutRect(mainQdialPaneRect.Rect() ,area);
        }
    else
        {
        TAknWindowLineLayout area = AppLayout::Speed_Dial_descendants_Line_1();
        areaLayout.LayoutRect(Rect() ,area);
        }

    TInt areaWidth = areaLayout.Rect().Width();
    TInt areaHeight = areaLayout.Rect().Height();
    TInt areaIl = areaLayout.Rect().iTl.iX;
    TInt areaIt = areaLayout.Rect().iTl.iY;
    TAknLayoutRect r;
    r.LayoutRect( Rect(), AknLayoutScalable_Apps::main_qdial_pane() );
    r.LayoutRect( r.Rect(), AknLayoutScalable_Apps::grid_qdial_pane() );
    SetPosition( r.Rect().iTl );
    
    SetSizeWithoutNotification( MinimumSize() ); 
    
    CFormattedCellListBoxData* data = ItemDrawer()->FormattedCellData();

    // set skin style to samre rect...
    data->SetBackgroundSkinStyle(&KAknsIIDQsnBgAreaMainQdial, TRect(r.Rect()));

    CAknQdialStyleGrid::SizeChanged();
    //Scalable UI 
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TAknLayoutRect mainQdialPaneRect;
        TAknLayoutRect gridQdialPaneRect;
        TAknLayoutRect callQdialPaneRect;
        TAknLayoutRect areaRect;

        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneRect );
        mainPaneRect.iTl = statusPaneRect.iTl;
        mainQdialPaneRect.LayoutRect( mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane().LayoutLine() );
        gridQdialPaneRect.LayoutRect( mainQdialPaneRect.Rect(), AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine() );
        TAknWindowComponentLayout areaNew = AknLayoutScalable_Apps::cell_qdial_pane( 0, 0 ).LayoutLine();	     
        areaRect.LayoutRect( mainQdialPaneRect.Rect(), areaNew.LayoutLine() );

        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            areaWidth = areaRect.Rect().Width();
            areaHeight = areaRect.Rect().Height();
            }
        else
            { 
            areaWidth  = ( mainPaneRect.Width() / 3 ) - 1;
            areaHeight = ( mainPaneRect.Height() / 3 ) - 7;
            }
        }
    else
        {
        TAknWindowLineLayout area = AppLayout::cell_qdial_pane( 0, 0 );
        areaLayout.LayoutRect( Rect(), area );
        areaWidth = areaLayout.Rect().Width();
        areaHeight = areaLayout.Rect().Height();
        }

    TInt language = User::Language();
    TBool LeftToRight = ETrue;
    
    // According to the UI Spec:
    // When Arabic and Hebrew input is in use, the Arabic and Hebrew browsing order
    // is not followed. Instead, the browsing order is the same as in Western variants.
    if ( language == ELangUrdu /*|| language == ELangArabic*/ )
        {
        LeftToRight = EFalse;
        }
    TRAP_IGNORE( SetLayoutL(
            EFalse,          // aVerticalOrientation
            LeftToRight,     // aLeftToRight
            ETrue,           // aTopToBottom
            KItemPrimary,    // aNumOfItemsInPrimaryOrient
            KItemSecondary,  // aNumOfItemsInSecondaryOrient
            TSize( areaWidth, areaHeight )  // NewScalableUI changes          
                                            // aWidthOfSpaceBetweenItems = 0 		
            )
        );	 

    data->SetTransparentSubCellL( SDM_TN_CIF_TURNED, ETrue );  //  5
    data->SetTransparentSubCellL( SDM_TN_CIF,        ETrue );  //  7
    data->SetTransparentSubCellL( SDM_TN_VGA_TURNED, ETrue );  //  9
    data->SetTransparentSubCellL( SDM_TN_VGA,        ETrue );  // 11
    data->SetTransparentSubCellL( SDM_TN_OTHER,      ETrue );  // 13
    
    ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
    data->SetBackgroundSkinStyle(&KAknsIIDQsnBgAreaMainQdial, mainPaneRect);
    iControl->SetLayout( Rect() );
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        CEikStatusPane *sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
        CAknTitlePane* title =  STATIC_CAST(CAknTitlePane*, sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle) ) );
        HBufC* prompt = StringLoader::LoadLC(R_QTN_SPDIA_TITLE, iCoeEnv);
        title->SetTextL(*prompt);
        title->MakeVisible(ETrue);
        title->DrawNow();                              	
        CleanupStack::PopAndDestroy(prompt);
        CEikonEnv::Static()->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
        }
    }


// ---------------------------------------------------------
// CSpdiaGrid::MopSupplyObject()
// Pass skin information if need.
// ---------------------------------------------------------
//
TTypeUid::Ptr CSpdiaGrid::MopSupplyObject(TTypeUid aId)
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, AknsDrawUtils::ControlContext(iControl) );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// End of File
