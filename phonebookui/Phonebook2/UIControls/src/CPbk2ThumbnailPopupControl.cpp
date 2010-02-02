/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 thumbnail popup control.
*
*/


// INCLUDE FILES
#include "CPbk2ThumbnailPopupControl.h"

// System includes
#include <eiklbx.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <eikenv.h>
#include <eikappui.h>
#include <AknUtils.h>


// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CPbk2ThumbnailPopupControl::CPbk2ThumbnailPopupControl( TPbk2ThumbnailLocation aThumbnailLocation,
                                                        const CEikListBox* aListBox )
    : iThumbnailLocation( aThumbnailLocation ),
      iListBox( aListBox )
    {
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CPbk2ThumbnailPopupControl::~CPbk2ThumbnailPopupControl()
    {
    AknsUtils::DeregisterControlPosition( this );
    CEikonEnv::Static()->EikAppUi()->RemoveFromStack( this );
    delete iBitmap;
    delete iBgContext;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CPbk2ThumbnailPopupControl* CPbk2ThumbnailPopupControl::NewL(
        TPbk2ThumbnailLocation aThumbnailLocation,
        const CEikListBox* aListBox )
    {
    CPbk2ThumbnailPopupControl* self =
        new( ELeave )CPbk2ThumbnailPopupControl( aThumbnailLocation, aListBox );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::ConstructL()
    {
    SetupSkinContext(); // we need to be able to clear the background using skin

    CreateWindowL( iListBox );

    MakeVisible( EFalse );
    SetFocusing( EFalse );
    SetPointerCapture( EFalse );

    // add to stack to get resource change notifications,
    // we must refuse focus and all input to prevent breaking the UI below
    CEikonEnv::Static()->EikAppUi()->AddToStackL(
            this,
            ECoeStackPriorityDefault,
            ECoeStackFlagRefusesAllKeys|ECoeStackFlagRefusesFocus );
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::SetBitmap(CFbsBitmap* aBitmap)
    {
    if( aBitmap != iBitmap )
        {
        delete iBitmap;
        iBitmap = aBitmap;
        }
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::SetThumbnailBackg( CFbsBitmap* aThumbBackg,
                                                    CFbsBitmap* aThumbBackgMask )
    {
    iThumbSize=aThumbBackg->SizeInPixels();
    iThumbBackg = aThumbBackg;
    iThumbBackgMask = aThumbBackgMask;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::MakeControlVisible(TBool aVisible)
    {
    // hide the thumbnail if the highlighted listbox item isn't visible anymore
    // (in ELocationUpDown mode the thumbnail is always visible)
    if( iThumbnailLocation == ELocationHanging &&
        !iListBox->View()->ItemIsVisible( iListBox->CurrentItemIndex() ) )
        {
        aVisible = EFalse;
        }

    if( aVisible )
        {
        Layout();
        ActivateL();
        }
    MakeVisible( aVisible );
    DrawDeferred(); // won't draw if invisible (or inactive)
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    if ( iBitmap )
        {
        const TInt orientation( Layout_Meta_Data::IsLandscapeOrientation() ? 0 : 1 );
        TAknLayoutRect thumbBgLayoutRect, thumbLayoutRect;
        thumbBgLayoutRect.LayoutRect(
                Rect(),
                AknLayoutScalable_Apps::bg_popup_preview_window_pane_cp01( orientation ) );
        thumbLayoutRect.LayoutRect(
                Rect(),
                AknLayoutScalable_Apps::popup_phob_thumbnail2_window_g1( orientation ) );

        AknsDrawUtils::BackgroundBetweenRects( AknsUtils::SkinInstance(), iBgContext, this, gc, Rect(), thumbLayoutRect.Rect() );

        thumbBgLayoutRect.DrawImage( gc, iThumbBackg, iThumbBackgMask );
        thumbLayoutRect.DrawImage( gc, iBitmap, NULL );

        }
    else
        {
        AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, Rect() );
        }
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
inline void CPbk2ThumbnailPopupControl::Layout()
    {
    // calculate indentation (0.5u) in pixels (aid_value_unit2 is a 10ux10u rectangle)
    TAknWindowComponentLayout unit( AknLayoutScalable_Avkon::aid_value_unit2() );
    const TInt indent( unit.LayoutLine().iW / 10 / 2 ); // 0.5u

    // background bitmap was already sized according to the thumbnail size and LAF specification
    // -> set window size to match the size of the bg bitmap
    TSize thumbWindowSize;
    if (iThumbBackg)
        {
        thumbWindowSize= iThumbSize;    
        }
    

    // calculate the thumbnail window X co-ordinate using listbox layout
    TAknLayoutRect listPaneLayoutRect; // for horizontal positioning
    listPaneLayoutRect.LayoutRect( iListBox->View()->ViewRect(), AknLayoutScalable_Avkon::list_gen_pane( 0 ) );

    // window position:
    // normal   - right side of the listbox -(thubnail window idth + indent)
    // mirrored - left side of the listbox + indent
    TPoint thumbWindowPt( Layout_Meta_Data::IsMirrored() ?
                            listPaneLayoutRect.Rect().iTl.iX + indent :
                            listPaneLayoutRect.Rect().iBr.iX - (indent + thumbWindowSize.iWidth),
                          0 );

    // thumbnail Y co-ordinate depends on the listbox focus position
    const TPoint focusPt( iListBox->View()->ItemPos( iListBox->CurrentItemIndex() ) );
    const TInt yMax( iListBox->View()->ViewRect().iBr.iY );

    if ( iThumbnailLocation == ELocationHanging )
    // thumbnail moves with the focus
    	{
    	// bottom Y of the focused listbox item
        const TInt focusBottom( focusPt.iY + iListBox->ItemHeight() );

        // place the thumbnail below the focus if there is enough space
        if ( yMax - focusBottom >= thumbWindowSize.iHeight + indent )
            {
            thumbWindowPt.iY = focusBottom - iListBox->ItemHeight()/5;
            }
        else // otherwise place it above the focus
            {
            thumbWindowPt.iY = focusPt.iY + iListBox->ItemHeight()/4 - thumbWindowSize.iHeight;
            }
    	}
    else if ( iThumbnailLocation == ELocationUpDown )
    // thumbnail is placed at the top or bottom corner
    	{
    	thumbWindowPt.iY = indent; // default to top

        // thumbnail is always at the top if focus is not visible
    	if( iListBox->View()->ItemIsVisible( iListBox->CurrentItemIndex() ) )
    	    {
        	const TRect focusRect( focusPt, iListBox->View()->ItemSize() );
        	const TRect thumbWindowRect( thumbWindowPt, thumbWindowSize );

        	// place the thumbnail at the bottom if it overlaps the focus
        	if( thumbWindowRect.Intersects( focusRect ) )
        		{
        		thumbWindowPt.iY = yMax - (indent + thumbWindowSize.iHeight);
        		}
            }
    	}

    SetExtent( thumbWindowPt, thumbWindowSize );
    AknsUtils::RegisterControlPosition( this );
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::HandleResourceChange(TInt aType)
	{
	CCoeControl::HandleResourceChange( aType );

	if( aType == KEikDynamicLayoutVariantSwitch ||
        aType == KAknsMessageSkinChange )
		{
		SetupSkinContext();
		// This control may be invalid when it's invisible
		// e.g. iThumbBackg,iThumbBackgMaksk may be destroyed
		if ( IsVisible() )
			{	
			Layout();
			}
		}
	}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CPbk2ThumbnailPopupControl::SetupSkinContext()
    {
    delete iBgContext;
    iBgContext = NULL;
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain, iListBox->Rect(), ETrue );
    iBgContext->SetParentPos( iListBox->PositionRelativeToScreen() );
    }


//  End of File
