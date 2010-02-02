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
#include    "CPbkThumbnailPopupControl.h"
#include    <PbkView.rsg>
#include    <barsread.h> // TResourceReader
#include    <coemain.h> // CCoeEnv
#include    <eikenv.h>  // CCoeMain
#include    <aknappui.h>
#include    <PbkDebug.h>

#include <aknlayout.cdl.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>

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

CPbkThumbnailPopupControl* CPbkThumbnailPopupControl::NewL()
    {
    CPbkThumbnailPopupControl* self = new (ELeave) CPbkThumbnailPopupControl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CPbkThumbnailPopupControl::~CPbkThumbnailPopupControl()
    {
    delete iBitmap;
    }

void CPbkThumbnailPopupControl::ChangeBitmap(
    CFbsBitmap* aBitmap, 
    CEikListBox* /*aListBox*/)
    {

    if ( !aBitmap )
        {
        return;
        }
    
    if(aBitmap != iBitmap && aBitmap != NULL )
        {
        delete iBitmap;
        iBitmap = aBitmap;
        }
    

    if (iBitmap)
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Thumbnail size(W:%d H:%d)"), 
                        iBitmap->SizeInPixels().iWidth, 
                        iBitmap->SizeInPixels().iHeight);

        TRect appRect = iAvkonAppUi->ApplicationRect();
        TAknLayoutRect statusPaneRect;
        statusPaneRect.LayoutRect(appRect, AknLayout::status_pane(appRect, 0));

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
        if ( bitmapSize.iHeight/2 < aknRectThumbnailCenter.Rect().Height() )
            {
            controlPoint.iY = aknRectThumbnailCenter.Rect().Height() - bitmapSize.iHeight/2;
            iControlSize.iHeight = bitmapSize.iHeight;
            }
        else 
            {
            iCrop.iY = (bitmapSize.iHeight - imageMaxHeight )/2;
            controlPoint.iY = 1;
            iControlSize.iHeight = imageMaxHeight;
            }
                                
        // calculate the X position of the bitmap
        const TInt imageMaxWidth( aknRectThumbnail.Rect().Width() );
        controlPoint.iX = aknRectThumbnailCenter.Rect().Width() - bitmapSize.iWidth/2;
        if (controlPoint.iX <= 0)
            {
            controlPoint.iX = 0;
            iCrop.iX = (bitmapSize.iWidth - imageMaxWidth)/2;
            iControlSize.iWidth = imageMaxWidth;
            }
        else 
            {
            iCrop.iX = 0;
            iControlSize.iWidth = bitmapSize.iWidth;
            }

        // x-coordinates of aknRectThumbnail's rect is calculated wrong in 
        // Arab&Hebrew case. If Arab/Hebrew layout is used, update only
        // y-coordinate to control point.
        if (aknRectThumbnail.Rect().iTl.iX > 0)
            {
            controlPoint += aknRectThumbnail.Rect().iTl;    
            }
        else
            {
            controlPoint.iY += aknRectThumbnail.Rect().iTl.iY;    
            }

        // calculate the actual bitmap position according to LAF
        TAknWindowLineLayout popupLayout = AppLayout::popup_pbook_thumbnail_window();
        TRect popupWindowRect = RectFromCoords(statusPaneRect.Rect(), popupLayout);
        controlPoint += popupWindowRect.iTl;
        
            
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

        // set the control size
        SetExtent(controlPoint, iControlSize);
        }

    DrawNow();
    }

void CPbkThumbnailPopupControl::Draw
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

CPbkThumbnailPopupControl::CPbkThumbnailPopupControl()
    {
    }

void CPbkThumbnailPopupControl::ConstructL()
    {
    CreateWindowL();
    MakeVisible(EFalse);
    SetExtent(TPoint(0,0),TSize(0,0));
    ActivateL();
    }

void CPbkThumbnailPopupControl::SetThumbnailBackg( CFbsBitmap* aThumbBackg, CFbsBitmap* aThumbBackgMask )
    {
    iThumbBackg = aThumbBackg;
    iThumbBackgMask = aThumbBackgMask;
    }
    
void CPbkThumbnailPopupControl::MakeControlVisible( TBool aVisible )
    {
    // make control visible
    MakeVisible(aVisible);
    // MakeVisible does not seem to make sure the control is redrawn
    DrawDeferred();    
    }


//  End of File  
