/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
/*
 * ccappcommlaunchercustomlistbox.cpp
 *
 *  Created on: 2009-10-30
 *      Author: dev
 */

// System includes
#include <barsread.h>
#include <aknlayoutscalable_apps.cdl.h>


#include "ccappcommlaunchercustomlistbox.h"
#include "ccappcommlaunchercustomlistboxitemdrawer.h"
#include "ccappcommlaunchercustomlistboxdata.h"

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::CCCAppCommLauncherCustomListBox
// --------------------------------------------------------------------------
//
inline CCCAppCommLauncherCustomListBox::CCCAppCommLauncherCustomListBox
            () 
    {
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::~CCCAppCommLauncherCustomListBox
// --------------------------------------------------------------------------
//
CCCAppCommLauncherCustomListBox::~CCCAppCommLauncherCustomListBox()
    {
    
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::NewL
// --------------------------------------------------------------------------
//
CCCAppCommLauncherCustomListBox* CCCAppCommLauncherCustomListBox::NewL()
    {
    CCCAppCommLauncherCustomListBox* self =
        new ( ELeave ) CCCAppCommLauncherCustomListBox();
    
    return self;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::CreateItemDrawerL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBox::CreateItemDrawerL()
    {    
    CCCAppCommLauncherCustomListBoxData* columnData = CCCAppCommLauncherCustomListBoxData::NewL();

    CleanupStack::PushL( columnData );  
    
    iItemDrawer = new (ELeave) CCCAppCommLauncherCustomListBoxItemDrawer(
            static_cast<MTextListBoxModel*>(Model()), iCoeEnv->NormalFont(), 
            columnData);
    CleanupStack::Pop( columnData );
    
    //Ownership has been transferred to iItemDrawer
    iColumnData = columnData;
    }
// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::SizeChanged
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBox::SizeChanged()
	{
	CAknDoubleLargeStyleListBox::SizeChanged();
	TRAP_IGNORE( SizeChangedL() );
	}

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::SizeChangedL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBox::SizeChangedL()
	{
	// Text format: "0\tCommunicationName\tContent\t1\t2"
	// Icons: 
	//  0: Communication method icon
	//  1: Presence icon
	//  2: Multi indication icon 
	//
	// Layout: list_double_large_graphic_phob2_cc_pane
	// ------------------------------------------------
	// |       |                                      |
	// |       | Communication method name      [1]   |
	// |  [0]  |                                      |
	// |       | Content                        [2]   |
	// |       |                                      |
	// ------------------------------------------------
	//
	
	// Calculate listbox rect
    TAknWindowComponentLayout listPaneLayout( TAknWindowComponentLayout::Compose(
        AknLayoutScalable_Apps::phob2_cc_listscroll_pane( 0 ),
        TAknWindowComponentLayout::Compose( 
            AknLayoutScalable_Apps::phob2_cc_list_pane( 0 ), 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane( 0 ) ) ) );    
  
    TAknLayoutRect listPaneLayoutRect;
    listPaneLayoutRect.LayoutRect( TRect( Size() ), listPaneLayout.LayoutLine() );
    TRect listPaneRowRect( listPaneLayoutRect.Rect() );
    
    // Set listbox item height
    SetItemHeightL( listPaneRowRect.Height() );
    iItemDrawer->SetItemCellSize( listPaneRowRect.Size() );
    

	// Reset sub cell array	
	iColumnData->ResetSLSubCellArray();
			  
	// Comm method icon	
	TAknWindowLineLayout gl( AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g1( 0 ) );
	iColumnData->SetGraphicSubCellL( 0, gl );
	
	// Line 1 text
	TAknTextComponentLayout textLayout = AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_t1( 0 );
	iColumnData->SetTextSubCellL( 1, textLayout );
	
	// Line 2 text
	TAknTextComponentLayout textLayout2 = AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_t2( 0 );
	iColumnData->SetTextSubCellL( 2, textLayout2 );
	
	// Presence icon
	TAknWindowLineLayout g2( AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g2( 0 ) );
	iColumnData->SetGraphicSubCellL( 3, g2 );
	
	// Multi indication icon
	TAknWindowLineLayout g3( AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g3( 0 ) );
	iColumnData->SetGraphicSubCellL( 4, g3 );
	
	// Conditional subcells must be added in priority order!
	// Line 1 text variation for 1 post icon
	TAknTextComponentLayout textLayout3 = AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_t1( 2 );
	iColumnData->SetConditionalSubCellL( 3, textLayout3, 1 );
	
	// Line 2 text variation for 2 post icon
	TAknTextComponentLayout textLayout4 = AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_t2( 2 );
	iColumnData->SetConditionalSubCellL( 4, textLayout4, 2 );
	
	}
