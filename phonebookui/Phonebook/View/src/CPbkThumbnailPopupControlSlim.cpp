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
*       Provides methods for Control for thumbnail popup window.
*
*/


// INCLUDE FILES
#include    "CPbkThumbnailPopupControlSlim.h"
#include    <PbkView.rsg>
#include    <barsread.h> // TResourceReader
#include    <coemain.h> // CCoeEnv
#include    <eikenv.h>  // CCoeMain
#include    <aknappui.h>
#include    <PbkDebug.h>

#include <aknlayout.cdl.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>


// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
inline TRect RectFromCoords(const TRect& aParent, TAknWindowLineLayout& aLayout)
    {
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(aParent, aLayout);
    return layoutRect.Rect();
    }

} // namespace

// ================= MEMBER FUNCTIONS =======================

CPbkThumbnailPopupControlSlim* CPbkThumbnailPopupControlSlim::NewL()
    {
    CPbkThumbnailPopupControlSlim* self = new (ELeave) CPbkThumbnailPopupControlSlim();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CPbkThumbnailPopupControlSlim::~CPbkThumbnailPopupControlSlim()
    {
    delete iBitmap;
    }

void CPbkThumbnailPopupControlSlim::ChangeBitmap(
    CFbsBitmap* aBitmap, 
    CEikListBox* aListBox)
    {
    
    // the is no LAF definitions for thumbnail position on landscape mode,
    // so the position of a thumbnail bitmap is calculted from the 
    // material available. Some of the calculations might look a 
    // bit odd, but those seems to work. 
    
    // When the UI specifiers add the thumbnail coordinates to 
    // LAF documents, and someone does the work on Avkon layout
    // components, these can, and should be changed to more
    // appropriate way to get right coordinates.


    if(aBitmap != iBitmap && aBitmap != NULL )
        {
        delete iBitmap;
        iBitmap = aBitmap;
        }
    
    if (iBitmap && aListBox )
        {        
        iDrawNeeded = EFalse;

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Thumbnail size(W:%d H:%d)"), 
                        iBitmap->SizeInPixels().iWidth, 
                        iBitmap->SizeInPixels().iHeight);

        TRect appRect = iAvkonAppUi->ApplicationRect();
        TAknLayoutRect statusPaneRect;
        statusPaneRect.LayoutRect(appRect, AknLayout::status_pane(appRect, 0));

        TSize bitmapSize = iBitmap->SizeInPixels();

        /// bitmap blit position
        TPoint controlPoint(0,0);

        iControlSize.iHeight = bitmapSize.iHeight;
        iControlSize.iWidth = bitmapSize.iWidth;
            

        // bitmap
        TAknWindowComponentLayout componentThumbnail = 
            AknLayoutScalable_Apps::popup_phob_thumbnail_window_g1();
            
        TAknWindowLineLayout lineThumbnail = componentThumbnail.LayoutLine();
        TAknLayoutRect aknRectThumbnail;
        aknRectThumbnail.LayoutRect( TRect(), componentThumbnail );

        // Shadow size
               
        TAknWindowComponentLayout componentThumbnailShadow = 
            AknLayoutScalable_Apps::popup_phob_thumbnail_window_g2();
        TAknWindowLineLayout lineThumbnailShadow = componentThumbnailShadow.LayoutLine();
        TAknLayoutRect aknRectThumbnailShadow;
        aknRectThumbnailShadow.LayoutRect( TRect(), componentThumbnailShadow );
            
        iShadowWidth = aknRectThumbnailShadow.Rect().iTl.iX - aknRectThumbnail.Rect().iTl.iX ;
        if ( iShadowWidth < 0 )
            {
            iShadowWidth  = aknRectThumbnail.Rect().iTl.iX - aknRectThumbnailShadow.Rect().iTl.iX;
            }
        iShadowHeight = aknRectThumbnailShadow.Rect().iTl.iY - aknRectThumbnail.Rect().iTl.iY;
        if ( iShadowHeight < 0 )
            {
            iShadowHeight  = aknRectThumbnail.Rect().iTl.iY - aknRectThumbnailShadow.Rect().iTl.iY;          
            }

        // increment shadow size
        iControlSize.iWidth += iShadowWidth;
        iControlSize.iHeight += iShadowHeight;

        // x
        TAknWindowComponentLayout layoutCompScrollPane = AknLayoutScalable_Avkon::scroll_pane_cp15(0);
        TAknWindowLineLayout layoutLineScrollPane = layoutCompScrollPane.LayoutLine();
        if ( Layout_Meta_Data::IsMirrored() )
            {
            TPoint tlListBox  = aListBox->View()->ViewRect().iTl; 
            controlPoint.iX += tlListBox.iX;
            // following "2*" is esthetic addition just to avoid unwanted
            // error reports 
            controlPoint.iX += 2 * layoutLineScrollPane.iW;  
            }
        else
            {
            TPoint brListBox  = aListBox->View()->ViewRect().iBr; 
            controlPoint.iX += brListBox.iX - iControlSize.iWidth;
            // following "2*" is esthetic addition just to avoid unwanted
            // error reports 
            controlPoint.iX -= 2 * layoutLineScrollPane.iW;              
            }
        
        // y        
        TAknWindowComponentLayout layoutCompAreaTopPane = AknLayoutScalable_Avkon::area_top_pane(1);
        TAknWindowLineLayout layoutLineAreaTopPane = layoutCompAreaTopPane.LayoutLine();
        controlPoint.iY += layoutLineAreaTopPane.iH;

        TAknWindowComponentLayout layoutCompIndicatorPane = AknLayoutScalable_Avkon::indicator_pane_g1(0);
        TAknWindowLineLayout layoutLineIndicatorPane = layoutCompIndicatorPane.LayoutLine();
        controlPoint.iY += layoutLineIndicatorPane.iH;
                
        TInt itemCount = aListBox->CurrentItemIndex() - aListBox->TopItemIndex();
        TPoint brListBox  = aListBox->View()->ViewRect().iBr; 
        TInt maxItemCount( aListBox->View()->NumberOfItemsThatFitInRect( aListBox->View()->ViewRect() ) );
        TInt itemHeight( aListBox->ItemHeight() );
        TInt yMaxSize( maxItemCount * itemHeight );
        TSize itemSize = aListBox->View()->ItemSize();
        if ( iThumbnailDown )
            {
            // Add one to itemCount so that we get also the focused one counted to size
            TInt ypos = ( itemCount + 1 ) * itemHeight;
            if ( ( yMaxSize - ypos ) < iControlSize.iHeight )
                {
                iThumbnailDown = EFalse;
                iDrawNeeded = ETrue;
                }
            }
        else
            {
            TInt ypos = itemCount * itemHeight;
            if ( ypos < iControlSize.iHeight )
                {
                iThumbnailDown = ETrue;
                iDrawNeeded = ETrue;
                }
            }

        if ( iThumbnailDown )        
            {
            controlPoint.iY += yMaxSize - iControlSize.iHeight;
            // The bitmap gets too close to the listbox's bottom line
            // so lets set the relative distance from edge to same
            // on sitaution where the bitmap is located upper corner.
            controlPoint.iY -= layoutLineIndicatorPane.iH;
            }
        
        // set the control size
        SetExtent(controlPoint, iControlSize);
        if ( iDrawNeeded )
            {
            DrawNow();        
            }
        }
    else // the else branch is for vCard viewer, it does not have a listbox.
        if (iBitmap && aBitmap )
        {
        
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Thumbnail size(W:%d H:%d)"), 
                        iBitmap->SizeInPixels().iWidth, 
                        iBitmap->SizeInPixels().iHeight);

        TRect appRect = iAvkonAppUi->ApplicationRect();
        TAknLayoutRect mainPaneRect;
        
        mainPaneRect.LayoutRect(appRect, AknLayout::main_pane(appRect, 0, 1, 1));
                
        TSize bitmapSize = iBitmap->SizeInPixels();

        /// bitmap blit position
        TPoint controlPoint;

        // bitmap
        TAknWindowComponentLayout componentThumbnail = 
            AknLayoutScalable_Apps::popup_phob_thumbnail_window_g1();
            
        // center of bitmap
        TAknWindowComponentLayout componentThumbnailCenter = 
            AknLayoutScalable_Apps::aid_phob_thumbnail_center_pane();

        TAknWindowLineLayout lineThumbnail = componentThumbnail.LayoutLine();
        TAknLayoutRect aknRectThumbnail;
        aknRectThumbnail.LayoutRect( TRect(), componentThumbnail );
        
        TAknWindowLineLayout lineThumbnailCenter = componentThumbnailCenter.LayoutLine();
        TAknLayoutRect aknRectThumbnailCenter;
        aknRectThumbnailCenter.LayoutRect( TRect(), componentThumbnailCenter );

        // Calculate Y position
        const TInt imageMaxHeight( aknRectThumbnail.Rect().Height() );
        
        if ( bitmapSize.iHeight < aknRectThumbnail.Rect().Height() )
            {
            controlPoint.iY = aknRectThumbnailCenter.Rect().Height() - bitmapSize.iHeight/2;
            if (controlPoint.iY < 0)
                {
                controlPoint.iY = 0;
                }
            iControlSize.iHeight = bitmapSize.iHeight;
            }
        else 
            {
            controlPoint.iY = 0;
            iControlSize.iHeight = imageMaxHeight;
            }
                                
        // calculate the X position of the bitmap
        const TInt imageMaxWidth( aknRectThumbnail.Rect().Width() );        
        controlPoint.iX = aknRectThumbnailCenter.Rect().Width() - bitmapSize.iWidth/2;
        
        if (controlPoint.iX <= 0)
            {
            controlPoint.iX = 0;
            iControlSize.iWidth = imageMaxWidth;
            }
        else 
            {
            iControlSize.iWidth = bitmapSize.iWidth;
            }
        
        // calculate the actual bitmap position according to LAF
        TAknWindowLineLayout popupLayout = AppLayout::popup_pbook_thumbnail_window();
        TRect popupWindowRect = RectFromCoords(mainPaneRect.Rect(), lineThumbnail);        
        
        TAknWindowComponentLayout layoutCompScrollPane = AknLayoutScalable_Avkon::scroll_pane_cp15(0);
        TAknWindowLineLayout layoutLineScrollPane = layoutCompScrollPane.LayoutLine();        
        
        // the x-position of thumbnail is calculated referring to the scroll pane, because
        // the LAF defines the position of thumbnail in landscape mode to wrong place
        // ( located on the context pane's position, and the it should be in bottom right
        // corner), so the LAF coordinates cannot be used.
        if ( Layout_Meta_Data::IsMirrored() )
            {                        
            controlPoint.iX += layoutLineScrollPane.iW + ( layoutLineScrollPane.iW / 2 );                        
            controlPoint.iY += mainPaneRect.Rect().iBr.iY;
            controlPoint.iY -= popupWindowRect.iBr.iY;                    
            }
        else
            {
            controlPoint.iX -= layoutLineScrollPane.iW + ( layoutLineScrollPane.iW / 2 );
            controlPoint += mainPaneRect.Rect().iBr;
            controlPoint -= popupWindowRect.iBr;            
            }
        
            
        // Shadow size
        TAknWindowComponentLayout componentThumbnailShadow = 
            AknLayoutScalable_Apps::popup_phob_thumbnail_window_g2();
        TAknWindowLineLayout lineThumbnailShadow = componentThumbnailShadow.LayoutLine();
        TAknLayoutRect aknRectThumbnailShadow;
        aknRectThumbnailShadow.LayoutRect( TRect(), componentThumbnailShadow );
            
        iShadowWidth = aknRectThumbnailShadow.Rect().iTl.iX - aknRectThumbnail.Rect().iTl.iX ;
        if ( iShadowWidth < 0 )
            {
            iShadowWidth  = aknRectThumbnail.Rect().iTl.iX - aknRectThumbnailShadow.Rect().iTl.iX;
            }
        iShadowHeight = aknRectThumbnailShadow.Rect().iTl.iY - aknRectThumbnail.Rect().iTl.iY;
        if ( iShadowHeight < 0 )
            {
            iShadowHeight  = aknRectThumbnail.Rect().iTl.iY - aknRectThumbnailShadow.Rect().iTl.iY;          
            }

        // increment shadow size
        iControlSize.iWidth += iShadowWidth;
        iControlSize.iHeight += iShadowHeight;
        
        if ( !Layout_Meta_Data::IsMirrored() )
            {
            controlPoint.iX -= iShadowWidth;       
            }
        
        // set the control size
        SetExtent(controlPoint, iControlSize);
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("##shadow: %d * %d"), iShadowWidth, iShadowHeight);
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("##Thumbnail control size %d * %d"), iControlSize.iWidth, iControlSize.iHeight);
        DrawNow();            
        }
    }

void CPbkThumbnailPopupControlSlim::Draw
        (const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();

    if (iBitmap)
        {   
        // draw the bitmap
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Thumbnail draw (Left:%d Top:%d Right:%d Bottom:%d)"), 
                Position().iX, 
                Position().iY, 
                Position().iX + iBitmap->SizeInPixels().iWidth,
                Position().iY + iBitmap->SizeInPixels().iHeight); 
      
        gc.BitBltMasked( TPoint(iShadowWidth, iShadowHeight), 
                         iThumbBackg, 
                         TRect(0,0,iThumbBackg->SizeInPixels().iWidth, iThumbBackg->SizeInPixels().iHeight ),
                         iThumbBackgMask, 
                         EFalse);
        gc.BitBlt(TPoint(), iBitmap);
        }
    }

CPbkThumbnailPopupControlSlim::CPbkThumbnailPopupControlSlim()
    {
    }

void CPbkThumbnailPopupControlSlim::ConstructL()
    {
    CreateWindowL();
    MakeVisible(EFalse);
    SetExtent(TPoint(0,0),TSize(0,0));
    ActivateL();
    }

void CPbkThumbnailPopupControlSlim::SetThumbnailBackg( CFbsBitmap* aThumbBackg, CFbsBitmap* aThumbBackgMask )
    {
    iThumbBackg = aThumbBackg;
    iThumbBackgMask = aThumbBackgMask;
    }

void CPbkThumbnailPopupControlSlim::MakeControlVisible( TBool aVisible )
    {
    // make control visible
    MakeVisible(aVisible);
    // MakeVisible does not seem to make sure the control is redrawn
    DrawDeferred();    
    }

//  End of File  
