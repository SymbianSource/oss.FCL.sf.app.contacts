/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclSearchResultListBox.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>
#include <eikclbd.h>

#include <skinlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
// ---------------------

#include "cpbkxrclsearchresultlistbox.h"

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListBox
////////////////////////////////////////////////////////////////////////////


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::CPbkxRclSearchResultListBox
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultListBox::CPbkxRclSearchResultListBox() :
    CAknSingleStyleListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::~CPbkxRclSearchResultListBox
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultListBox::~CPbkxRclSearchResultListBox()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::HandleScrollEventL
// --------------------------------------------------------------------------
//
void CPbkxRclSearchResultListBox::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {
    // Handle showing of popupcharacter when user scrolls list using using scroll bar
    TBool prevState = iShowPopupChar;
    iShowPopupChar = (aEventType == EEikScrollThumbDragHoriz || aEventType == EEikScrollThumbDragVert);
    if( prevState != iShowPopupChar )
        {
        DrawDeferred();
        }
    
    CAknSingleStyleListBox::HandleScrollEventL( aScrollBar, aEventType );
    }

// --------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::Draw
// --------------------------------------------------------------------------
//
void CPbkxRclSearchResultListBox::Draw(const TRect& aRect ) const
    {
    CAknSingleStyleListBox::Draw(aRect);
    
    // Handle showing of popupcharacter when user scrolls list using using scroll bar  
    if( iShowPopupChar )
        {
        CWindowGc& gc = SystemGc();
           
        TRgb normal;
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), 
                   normal, 
                   KAknsIIDQsnTextColors, 
                   EAknsCIQsnTextColorsCG6 );

        TAknLayoutRect layout;
        layout.LayoutRect(Rect(), AknLayoutScalable_Apps::popup_navstr_preview_pane(0));

        TAknLayoutRect cornerRect;
        // skinned draw uses submenu popup window skin (skinned border)
        cornerRect.LayoutRect(
                layout.Rect(),
                SkinLayout::Submenu_skin_placing_Line_2() );
           
        TRect innerRect( layout.Rect() );
        innerRect.Shrink( cornerRect.Rect().Width(), cornerRect.Rect().Height() );
        
        if ( !AknsDrawUtils::DrawFrame(
                AknsUtils::SkinInstance(),
                gc,
                layout.Rect(),
                innerRect,
                KAknsIIDQsnFrPopupSub,
                KAknsIIDQsnFrPopupCenterSubmenu ) )
            {
            // skinned border failed -> black border
            gc.SetPenStyle( CGraphicsContext::ESolidPen );
            gc.SetBrushColor( KRgbBlack );
            gc.DrawRect( layout.Rect() );
            }
           
        TAknLayoutText textLayout;
        textLayout.LayoutText(layout.Rect(), AknLayoutScalable_Apps::popup_navstr_preview_pane_t1(0).LayoutLine());
           
        TPtrC desc(Model()->ItemTextArray()->MdcaPoint(View()->TopItemIndex()));
        textLayout.DrawText(gc, desc.Mid(desc.Find(_L("\t")) + 1, 1), ETrue, normal );
        }
   }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultListBox::CreateItemDrawerL()
    {
    FUNC_LOG;
    CColumnListBoxData* data = CColumnListBoxData::NewL();
    CleanupStack::PushL( data );
    iItemDrawer = new ( ELeave ) CPbkxRclSearchResultItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        data );
    data->SetSkinEnabledL( ETrue );
    CleanupStack::Pop( data );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::MakeViewClassInstanceL
// ---------------------------------------------------------------------------
//
CListBoxView* CPbkxRclSearchResultListBox::MakeViewClassInstanceL()
    {

//This strange old override below (returning class CPbkxRclSearchResultListView) causes on hardware empty 
//list text not to draw. Most likely no need for this kind override at all.
//To fix draw this override is not used (however is part of pbk2rclengine api
//and therefore not removed completely)
//    return ( new (ELeave) CPbkxRclSearchResultListView() );

    return ( new (ELeave) CAknColumnListBoxView() );    
    }

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultItemDrawer
////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::CPbkxRclSearchResultItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultItemDrawer::CPbkxRclSearchResultItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CColumnListBoxData* aColumnData ) :
    CColumnListBoxItemDrawer( aTextListBoxModel, aFont, aColumnData )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::~CPbkxRclSearchResultItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultItemDrawer::~CPbkxRclSearchResultItemDrawer()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::SetHighlightColor
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::SetHighlightColor( TRgb aColor )
    {
    FUNC_LOG;
    iHighlightColor = aColor;
    iColorsSet = iColorsSet | EHighlighColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::SetColor
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::SetColor( TRgb aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    iColorsSet = iColorsSet | EColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::ResetColors
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::ResetColors( TBool aHighlightColor )
    {
    FUNC_LOG;
    if ( aHighlightColor )
        {
        iColorsSet = iColorsSet & ( ~EHighlighColorSet );
        }
    else
        {
        iColorsSet = iColorsSet & ( ~EColorSet );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::DrawItemText(
    TInt aItemIndex,
    const TRect& aItemTextRect,
    TBool aItemIsCurrent,
    TBool aViewIsEmphasized,
    TBool aItemIsSelected ) const
    {
    FUNC_LOG;

    // force our own text colors to item drawer
    CPbkxRclSearchResultItemDrawer* ptr = 
        const_cast<CPbkxRclSearchResultItemDrawer*>( this );
    if ( iColorsSet & EColorSet )
        {
        ptr->iTextColor = iColor;
        }
    
    if ( iColorsSet & EHighlighColorSet )
        {
        ptr->iHighlightedTextColor = iHighlightColor;
        }        

    // Disable AVKON skinning so that our own color definitions apply
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) );
    CColumnListBoxItemDrawer::DrawItemText(
        aItemIndex,
        aItemTextRect,
        aItemIsCurrent,
        aViewIsEmphasized,
        aItemIsSelected );
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );
    }



//This strange old override below (CPbkxRclSearchResultListView) causes on hardware empty 
//list text not to draw. Most likely no need for this kind override at all.
//To fix draw this override is not used (however is part of pbk2rclengine api
//and therefore not removed completely)
////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListView
////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListView::DrawEmptyList
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultListView::DrawEmptyList(const TRect &aClientRect) const
    {
    // Disable AVKON skinning so that our own color definitions apply
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) );
    CAknColumnListBoxView::DrawEmptyList( aClientRect );
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );
    }

