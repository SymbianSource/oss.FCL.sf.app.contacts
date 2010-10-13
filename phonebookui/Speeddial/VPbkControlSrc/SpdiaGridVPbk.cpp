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
#include "speeddialprivate.h"
#include "SpdiaGridVPbk.h" 
#include "Speeddial.laf" 

#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <SpdCtrl.rsg>
const TInt KItemPrimary(3);
const TInt KItemSecondary(3);

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CSpdiaGridVPbk::NewL()
// 
// ---------------------------------------------------------
CSpdiaGridVPbk* CSpdiaGridVPbk::NewL(const CSpeedDialPrivate& aControl)
{
	CSpdiaGridVPbk* self = new (ELeave) CSpdiaGridVPbk(aControl);
	return self;
}

// ---------------------------------------------------------
// CSpdiaGridVPbk::CSpdiaGridVPbk()
// 
// ---------------------------------------------------------
CSpdiaGridVPbk::CSpdiaGridVPbk()
{
}

// ---------------------------------------------------------
// CSpdiaGridVPbk::CSpdiaGridVPbk()
// 
// ---------------------------------------------------------
CSpdiaGridVPbk::CSpdiaGridVPbk(const CSpeedDialPrivate& aControl)
{
	iControl = CONST_CAST(CSpeedDialPrivate*, &aControl);
}

// ---------------------------------------------------------
// CSpdiaGridVPbk::~CSpdiaGridVPbk()
// 
// ---------------------------------------------------------
CSpdiaGridVPbk::~CSpdiaGridVPbk()
{
}
// ---------------------------------------------------------
// CSpdiaGridVPbk::Draw()
// 
// ---------------------------------------------------------
void CSpdiaGridVPbk::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();
    AknsDrawUtils::Background(
       AknsUtils::SkinInstance(),
       AknsDrawUtils::ControlContext( iControl ),
       this,
       gc,
       iControl->Rect() );
    if(iControl)
	{
		iControl->DrawShadow( gc );
	}

    CAknGrid::Draw(aRect);
}

// ---------------------------------------------------------
// CSpdiaGridVPbk::SizeChanged()
// 
// ---------------------------------------------------------
void CSpdiaGridVPbk::SizeChanged()
{
	TRAP_IGNORE(SizeChangedL());
}
// ---------------------------------------------------------
// CSpdiaGridVPbk::SizeChangedL
// 
// ---------------------------------------------------------
void CSpdiaGridVPbk::SizeChangedL()
{
	//Scalable UI.
	TAknLayoutRect areaLayout;
	TRect mainPaneRect ;
    TRect statusPaneRect;
   		
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
	 {
		TAknLayoutRect mainQdialPaneRect;
   		TAknLayoutRect gridQdialPaneRect;
  	 if (Layout_Meta_Data::IsLandscapeOrientation())
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
	//SetPosition( TPoint(spdia_main_pane_grid_l, spdia_main_pane_grid_t) );
	TInt areaWidth = areaLayout.Rect().Width();
    TInt areaHeight = areaLayout.Rect().Height();
	TInt areaIl = areaLayout.Rect().iTl.iX;
	TInt areaIt = areaLayout.Rect().iTl.iY;
	SetPosition( TPoint(areaIl, areaIt) );
    //SetSizeWithoutNotification( TSize(spdia_main_pane_grid_w, spdia_main_pane_grid_h) );
	SetSizeWithoutNotification( TSize(areaWidth , areaHeight)); 

    CAknQdialStyleGrid::SizeChanged();
	//Scalable UI 
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		TAknLayoutRect mainQdialPaneRect;
   		TAknLayoutRect gridQdialPaneRect;
   		TAknLayoutRect callQdialPaneRect;
   		
		if (Layout_Meta_Data::IsLandscapeOrientation())
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
    	areaLayout.LayoutRect(gridQdialPaneRect.Rect(), AknLayoutScalable_Apps::cell_qdial_pane(0,0).LayoutLine());
	
		}
	 else
		{
		 TAknWindowLineLayout area = AppLayout::cell_qdial_pane(0,0);
		 areaLayout.LayoutRect(Rect() ,area);
		}
	areaWidth = areaLayout.Rect().Width();
    areaHeight = areaLayout.Rect().Height();
    if(AknLayoutUtils::LayoutMirrored())
    {
    TRAP_IGNORE(SetLayoutL(
			EFalse,  // aVerticalOrientation
			EFalse,  // aLeftToRight
			ETrue,  // aTopToBottom
			KItemPrimary,  // aNumOfItemsInPrimaryOrient
			KItemSecondary,  // aNumOfItemsInSecondaryOrient
			TSize(areaWidth, areaHeight )  //NewScalableUI changes          // aWidthOfSpaceBetweenItems=0 
			
		)
	);	
    	
    }
    else
    {
    TRAP_IGNORE(SetLayoutL(
			EFalse,  // aVerticalOrientation
			ETrue,  // aLeftToRight
			ETrue,  // aTopToBottom
			KItemPrimary,  // aNumOfItemsInPrimaryOrient
			KItemSecondary,  // aNumOfItemsInSecondaryOrient
			TSize(areaWidth, areaHeight )  //NewScalableUI changes          // aWidthOfSpaceBetweenItems=0 
			
		)
	);
    }
	
	CFormattedCellListBoxData* data = ItemDrawer()->FormattedCellData();

	data->SetTransparentSubCellL(SDM_TN_CIF_TURNED, ETrue);  //  5
    data->SetTransparentSubCellL(SDM_TN_CIF,        ETrue);  //  7
    data->SetTransparentSubCellL(SDM_TN_VGA_TURNED, ETrue);  //  9
    data->SetTransparentSubCellL(SDM_TN_VGA,        ETrue);  // 11
    data->SetTransparentSubCellL(SDM_TN_OTHER,      ETrue);  // 13
    
    ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
	data->SetBackgroundSkinStyle(&KAknsIIDQsnBgAreaMainQdial, mainPaneRect);
	iControl->SetLayout( Rect() );
    if (Layout_Meta_Data::IsLandscapeOrientation())
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
// CSpdiaGridVPbk::MopSupplyObject()
// Pass skin information if need.
// ---------------------------------------------------------
//
TTypeUid::Ptr CSpdiaGridVPbk::MopSupplyObject(TTypeUid aId)
{
    if(aId.iUid == MAknsControlContext::ETypeId)
    {
        return MAknsControlContext::SupplyMopObject( aId, AknsDrawUtils::ControlContext(iControl) );
    }
    return CCoeControl::MopSupplyObject( aId );
}

// End of File
