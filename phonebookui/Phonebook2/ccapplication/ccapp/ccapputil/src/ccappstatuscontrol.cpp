/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UI status control
*
*/


#include "ccappstatuscontrol.h"
#include "MVPbkContactLink.h"
#include "TPbk2IconId.h"
#include <eikimage.h>
#include <eiklabel.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <StringLoader.h>
#include <AknsFrameBackgroundControlContext.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <touchfeedback.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <Pbk2PresentationUtils.h>

namespace {
/// Amount of child UI components
const TInt KControlCount = 3; // image & label x 2
/// Max amount of lines used to show status text. Rest will be clipped
const TInt KStatusTextLines = 2;
const TText KGraphicReplaceCharacter    = ' ';

inline CGulIcon* CreateEmptyIconL()
    {               
    CFbsBitmap* image = NULL;
    CFbsBitmap* mask = NULL;
    AknIconUtils::CreateIconL( image, mask, 
            AknIconUtils::AvkonIconFileName(),
            EMbmAvkonQgn_prop_empty, 
            EMbmAvkonQgn_prop_empty_mask );                             
    return CGulIcon::NewL( image, mask );
    } 

inline void ReSizeIcon( CGulIcon* aIcon, const TSize& aSize )
    {
    if( aIcon )
        {
        CFbsBitmap* bitmap = aIcon->Bitmap();
        CFbsBitmap* mask = aIcon->Mask();
        if( bitmap )
            {
            AknIconUtils::SetSize( 
                    bitmap, aSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
            }
        if( mask )
            {
            AknIconUtils::SetSize( 
                    mask, aSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
            }
        }
    }

}   // namespace

// ---------------------------------------------------------------------------
// CCCAppStatusControl::NewL
// ---------------------------------------------------------------------------
//                            
EXPORT_C CCCAppStatusControl* CCCAppStatusControl::NewL( 
    CSpbContentProvider& aContentProvider, 
    MCCAStatusControlObserver& aObserver )
    {
    CCCAppStatusControl* self= new(ELeave) CCCAppStatusControl( 
            aContentProvider, aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::CCCAppStatusControl
// ---------------------------------------------------------------------------
//
CCCAppStatusControl::CCCAppStatusControl( 
    CSpbContentProvider& aContentProvider, 
    MCCAStatusControlObserver& aObserver ) : 
    iContentProvider( aContentProvider ),
    iObserver( aObserver ),
    iState( EStateUndefined )
    {		
    } 

// ---------------------------------------------------------------------------
// CCCAppStatusControl::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::ConstructL() 	
    {            	
	// Create status icon image
    iStatusImage = new( ELeave ) CEikImage;		
    iStatusImage->SetAlignment( EHCenterVCenter );
    iStatusImage->SetPictureOwnedExternally( ETrue );    	
    iStatusImage->SetBrushStyle( CGraphicsContext::ENullBrush ); // transparent
    iStatusImage->MakeVisible( EFalse );
	
	// Create status text label
	iStatusLabel1 = new( ELeave ) CEikLabel;
	iStatusLabel1->SetAlignment( EHLeftVCenter );
	iStatusLabel1->SetTextL( KNullDesC() ); // CEikLabel will panic without text
    iStatusLabel1->MakeVisible( EFalse );

    iStatusLabel2 = new( ELeave ) CEikLabel;
    iStatusLabel2->SetAlignment( EHLeftVCenter );
    iStatusLabel2->SetTextL( KNullDesC() ); // CEikLabel will panic without text
    iStatusLabel2->MakeVisible( EFalse );
    
	iStatusText = KNullDesC().AllocL();
		
	iContentProvider.AddObserverL( *this );
	
    iBgContext = CAknsFrameBackgroundControlContext::NewL(
        KAknsIIDNone, TRect(), TRect(), EFalse );	
    
    // normal state by default
    iBgContext->SetFrame( KAknsIIDQsnFrButtonNormal );
    iBgContext->SetCenter( KAknsIIDQsnFrButtonCenterNormal );
    
    // stylus feedback support
    iTouchFeedBack = MTouchFeedback::Instance();
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::~CCCAppStatusControl
// ---------------------------------------------------------------------------
//
CCCAppStatusControl::~CCCAppStatusControl()
    {	
    if( iTouchFeedBack )
        {
        iTouchFeedBack->RemoveFeedbackForControl( this );
        }
	iContentProvider.RemoveObserver( *this );
	
    delete iStatusImage;
    delete iStatusIcon;
    delete iStatusLabel1;    
    delete iStatusLabel2;    
    delete iStatusText;    
	delete iDefaultIcon;
    delete iBgContext; 
    delete iDefaultStatusText;
    delete iLink;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SetPressed
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::SetPressed( TBool aPressed )
    {
    if( iPressed != aPressed )
        {
        iPressed = aPressed;
        if( iPressed )
            {
            // pressed
            iBgContext->SetFrame( KAknsIIDQsnFrButtonPressed );
            iBgContext->SetCenter( KAknsIIDQsnFrButtonCenterPressed );
            DrawNow(); // respond quickly to user event
            }
        else
            {
            // normal
            iBgContext->SetFrame( KAknsIIDQsnFrButtonNormal );
            iBgContext->SetCenter( KAknsIIDQsnFrButtonCenterNormal );
            DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::CountComponentControls
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppStatusControl::SetContactLinkL( MVPbkContactLink& aLink )
	{
	delete iLink;
	iLink = NULL;
	iLink = aLink.CloneLC();
	CleanupStack::Pop(); // iLink
	
	delete iStatusText;
	iStatusText = NULL;
	TPbk2IconId iconId;
    CSpbContentProvider::TSpbContentType type = CSpbContentProvider::ETypeNone; 
	iContentProvider.GetContentL( *iLink, iStatusText, iconId, type );			
	}

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SetDefaultStatusIconL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppStatusControl::SetDefaultStatusIconL( 
        CGulIcon* aDefaultStatusIcon )
	{
	delete iDefaultIcon;
	iDefaultIcon = aDefaultStatusIcon;

	if( iStatusIconSize != TSize() && iState == EStateDefaultContent )
        {
        ShowDefaultContentL();
        }
	}

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SetDefaultStatusTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppStatusControl::SetDefaultStatusTextL( 
        HBufC* aDefaultStatusText ) 		
	{
	delete iDefaultStatusText;
	iDefaultStatusText = aDefaultStatusText;
	
	if( iState == EStateDefaultContent )
	    {
        ShowDefaultContentL();
	    }
	}

// ---------------------------------------------------------------------------
// CCCAppStatusControl::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CCCAppStatusControl::CountComponentControls() const
    {
    return KControlCount;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CCCAppStatusControl::ComponentControl( TInt aIndex )  const
    {
    switch( aIndex )
        {
        case 0:
            {
            return  iStatusImage;
            }
        case 1:
            {
            return iStatusLabel1;
            }        
        case 2:
            {
            return iStatusLabel2;
            }        
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SetVariableLayouts
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::SetVariableLayouts( TInt aOption )
    {
    const TRect rect(Rect());
    // set background graphics layout
    const TAknWindowComponentLayout innerLayout(
            AknLayoutScalable_Apps::bg_button_pane_cp033( aOption ) );
    
    TAknLayoutRect innerLayoutRect;
    innerLayoutRect.LayoutRect( rect, innerLayout.LayoutLine() );
    const TRect innerRect( innerLayoutRect.Rect() );
    iBgContext->SetFrameRects( rect, innerRect );      
                       
    // status icon
    const TAknWindowComponentLayout statusIconLayout( 
            AknLayoutScalable_Apps::phob2_cc_button_pane_g1( aOption ) );
    
    TAknLayoutRect statusIconLayoutRect;
    statusIconLayoutRect.LayoutRect( Rect(), statusIconLayout.LayoutLine() );
    const TRect statusIconRect( statusIconLayoutRect.Rect() );
    iStatusIconSize = statusIconRect.Size();
    iStatusImage->SetRect( statusIconRect );
      
    // status label 1
    AknLayoutUtils::LayoutLabel( 
            iStatusLabel1, 
            rect, 
            AknLayoutScalable_Apps::phob2_cc_button_pane_t1( aOption ) ); 

	TRgb color;
    AknsUtils::GetCachedColor( 
            AknsUtils::SkinInstance(), 
            color, 
            KAknsIIDQsnTextColors, 
            EAknsCIQsnTextColorsCG6 );
    
    TRAP_IGNORE(
        {
        iStatusLabel1->OverrideColorL( EColorLabelText, color );        
        iStatusLabel2->OverrideColorL( EColorLabelText, color );
        } );
			
    // resize images for new resolution
    ReSizeIcon( iStatusIcon, iStatusIconSize );
    ReSizeIcon( iDefaultIcon, iStatusIconSize );
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SizeChanged
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::SizeChanged()
    {
    const TRect rect( Rect() );
    
    // update stylus feedback
    if( iTouchFeedBack )
        {
        iTouchFeedBack->ChangeFeedbackArea( this, 0, rect );
        }

    // status label 2
    AknLayoutUtils::LayoutLabel( 
            iStatusLabel2, 
            rect, 
            AknLayoutScalable_Apps::phob2_cc_button_pane_t2( 0 ) );
    
    TInt count(0);
    // If this leaves, it means we're out of memory. Nothing we can do.
    TRAP_IGNORE( count = RewrapStatusTextL() ); 
    
    TInt option( 1 );
    if( count > 1 )
        {
        option = 0;
        }
    
    SetVariableLayouts( option );
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
    	{
        SetPressed( ETrue );
    	}
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        SetPressed( EFalse );
        if( Rect().Contains( aPointerEvent.iPosition ) )
            {
            // stylus released inside the button area
            iObserver.StatusClickedL();
            }
        }
    else if( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        if( Rect().Contains( aPointerEvent.iPosition ) )
            {
            // stylus moved inside the button area
            SetPressed( ETrue );
            }
        else
            {
            // stylus moved outside the button area
            SetPressed( EFalse );
            }
        }    
  	}

// ---------------------------------------------------------------------------
// CCCAppStatusControl::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::SetContainerWindowL( 
    const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );

    // Assign window for child controls too
    const TInt childCount = CountComponentControls();
    for( TInt i = 0; i < childCount; ++i )
        {
        ComponentControl( i )->SetContainerWindowL( *this );
        }
    
    // stylus feedback can be set only after we have parent control (window owning)
    if( iTouchFeedBack )
        {
        CFeedbackSpec* spec = CFeedbackSpec::New();
        if( spec )
            {
            spec->AddFeedback( ETouchEventStylusDown, ETouchFeedbackBasicButton );
            iTouchFeedBack->SetFeedbackArea( this, 0, Rect(), spec );
            delete spec;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::ContentUpdated
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::ContentUpdated( MVPbkContactLink& aLink,
	MSpbContentProviderObserver::TSpbContentEvent aEvent )
	{
	TRAP_IGNORE( DoStatusUpdateL( aLink, aEvent ) );
	}

// ---------------------------------------------------------------------------
// CCCAppStatusControl::DoStatusUpdateL
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::DoStatusUpdateL( MVPbkContactLink& aLink,
    MSpbContentProviderObserver::TSpbContentEvent aEvent )
    {
    if( aEvent == EContentNotAvailable )
        {
		ShowDefaultContentL();
        }
    else if( iLink && iLink->IsSame( aLink ) )
        {
        iState = EStateStatusContent;
        delete iStatusText;
        iStatusText = NULL;
        
        TPbk2IconId iconId;
        CSpbContentProvider::TSpbContentType type = 
                CSpbContentProvider::ETypeNone; 
        iContentProvider.GetContentL( aLink, iStatusText, iconId, type );
        
        const TInt count( RewrapStatusTextL() );
        TInt option( 1 );
        if( count > 1 )
            {
            option = 0;
            }
        SetVariableLayouts( option );
              
        CGulIcon* icon = NULL;
        TRAP_IGNORE( 
                {
                icon = iContentProvider.CreateServiceIconLC( iconId );
                CleanupStack::Pop( icon );
                } );
        if( !icon )
            {
            icon = CreateEmptyIconL();
            }
        
        delete iStatusIcon;
        // iStatusIcon owns bitmaps 
        iStatusIcon = icon;
        ReSizeIcon( iStatusIcon, iStatusIconSize );
        
        iStatusImage->SetPicture( iStatusIcon->Bitmap(), iStatusIcon->Mask() );
        iStatusImage->MakeVisible( ETrue );
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::RewrapStatusTextToArrayL
// ---------------------------------------------------------------------------
//
inline void CCCAppStatusControl::RewrapStatusTextToArrayL( 
        TDes& aStatusText, 
        CArrayFix<TPtrC>& aTxtArray )
    {
    // generate array of line lengths 
    CArrayFixFlat<TInt>* array = 
        new(ELeave) CArrayFixFlat<TInt>( KStatusTextLines );
    CleanupStack::PushL( array );
    
    // check width and font from label 2, because label 2 layout is 
    // set before this function is called
    const TInt width( iStatusLabel2->Size().iWidth );
    array->AppendL( width );
    array->AppendL( width );
    
    const CFont* font = iStatusLabel2->Font();
   
    // wrap text
    AknTextUtils::WrapToArrayAndClipL( 
            aStatusText,             
            *array, 
            *font,                     
            aTxtArray );
    
    CleanupStack::PopAndDestroy( array );
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::RewrapStatusTextL
// ---------------------------------------------------------------------------
//
TInt CCCAppStatusControl::RewrapStatusTextL()
    {
    TInt txtCount(0);
    if( iStatusText )
        {
        HBufC* statusTxtBuffer = iStatusText->AllocLC();
        TPtr statusTxt( statusTxtBuffer->Des() );
    
        // replace non-allowed characters with ' '
        Pbk2PresentationUtils::ReplaceNonGraphicCharacters(
                statusTxt, KGraphicReplaceCharacter );
        AknTextUtils::ReplaceCharacters(
                statusTxt, 
                KAknCommonWhiteSpaceCharacters, 
                KGraphicReplaceCharacter );
    
        // generate array of line lengths 
        CArrayFixFlat<TPtrC>* txtArray = 
                new (ELeave) CArrayFixFlat<TPtrC>(KStatusTextLines);
        CleanupStack::PushL( txtArray );
        RewrapStatusTextToArrayL( statusTxt, *txtArray );
        
        // assign new text
        TPtrC label1(KNullDesC);
        TPtrC label2(KNullDesC);
        
        txtCount = txtArray->Count();
        switch (txtCount)
            {
            case 1:
                {
                label1.Set( txtArray->At(0) );
                iStatusLabel1->MakeVisible( ETrue );
                iStatusLabel2->MakeVisible( EFalse );
                break;
                }
            case 2:
                {
                label1.Set( txtArray->At(0) );
                label2.Set( txtArray->At(1) );
                iStatusLabel1->MakeVisible( ETrue );
                iStatusLabel2->MakeVisible( ETrue );
                break;
                }
            default:
                {
                }
            }
        
        iStatusLabel1->SetTextL( label1 );
        iStatusLabel2->SetTextL( label2 );
        CleanupStack::PopAndDestroy( 2, statusTxtBuffer ); // txtArray
        iStatusImage->MakeVisible( ETrue );
        DrawDeferred();
        }
    return txtCount;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::Draw
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::Draw( const TRect& /*aRect*/ ) const
    {
    const TRect rect( Rect() );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CWindowGc& gc = SystemGc();
    
    // draw skinned background
    if( !AknsDrawUtils::Background( skin, iBgContext, NULL, 
            gc, rect, KAknsDrawParamNoClearUnderImage ) )
        {
        // if skin fails then draw solid colors
        gc.SetBrushColor( iPressed ? KRgbDarkGray : KRgbGray );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( rect );
        }    
    }

// ---------------------------------------------------------------------------
// CCCAppStatusControl::ShowDefaultContentL
// ---------------------------------------------------------------------------
//
void CCCAppStatusControl::ShowDefaultContentL()
	{
    iState = EStateDefaultContent;
    
	if( iDefaultIcon && ( iDefaultIcon->Bitmap() != iStatusImage->Bitmap() ) )
		{
        ReSizeIcon( iDefaultIcon, iStatusIconSize );
        iStatusImage->SetPicture( iDefaultIcon->Bitmap(), iDefaultIcon->Mask() );
        iStatusImage->DrawDeferred();
		}
	
	if( iDefaultStatusText )
		{
        if( !iStatusText || iDefaultStatusText->CompareC( *iStatusText ) != 0 )
            {
            // update text
            HBufC* txt = iDefaultStatusText->AllocL();
            delete iStatusText;
            iStatusText = txt;
            const TInt count( RewrapStatusTextL() );
            TInt option( 1 );
            if( count > 1 )
                {
                option = 0;
                }
            SetVariableLayouts( option );
            }
		}
	}

// End of file
