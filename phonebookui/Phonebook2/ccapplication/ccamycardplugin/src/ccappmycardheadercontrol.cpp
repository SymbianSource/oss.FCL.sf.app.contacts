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
* Description:  Header UI control of the mycard plugin
*
*/


#include "ccappmycardheadercontrol.h"
#include "ccappmycardcommon.h"
#include <eikimage.h>
#include <eiklabel.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <avkon.mbg>
#include <aknstyluspopupmenu.h>
#include <ccappmycardpluginrsc.rsg>
#include <barsread.h>
#include <touchfeedback.h>
#include "ccappmycardplugin.h"
#include <aknlayoutscalable_apps.cdl.h>
#include "spbcontentprovider.h"
#include <CPbk2ApplicationServices.h>
#include <featmgr.h>
#include <AknIconUtils.h>
#include "ccappstatuscontrol.h"
#include <phonebook2ece.mbg>
#include <gulicon.h>
#include <layoutmetadata.cdl.h>
#include <StringLoader.h>

namespace {
/// Amount of child UI components
const TInt KControlCount = 4;
_LIT( KMyCardIconDefaultFileName, "\\resource\\apps\\phonebook2ece.mif" );
}

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCardHeaderControl* CCCAppMyCardHeaderControl::NewL( 
        MCCAStatusControlObserver& aObserver )
    {
    CCCAppMyCardHeaderControl* self = new(ELeave) CCCAppMyCardHeaderControl();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::CCCAppMyCardHeaderControl
// ---------------------------------------------------------------------------
//
CCCAppMyCardHeaderControl::CCCAppMyCardHeaderControl()
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::ConstructL( 
        MCCAStatusControlObserver& aObserver )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardHeaderControl::ConstructL()"));    
    
    iAppServices = CPbk2ApplicationServices::InstanceL();
    CVPbkContactManager& contactManager = iAppServices->ContactManager();

    FeatureManager::InitializeLibL();
    if( FeatureManager::FeatureSupported( KFeatureIdFfContactsSocial ) )
        {    
        iProvider = CSpbContentProvider::NewL( contactManager, 
            iAppServices->StoreManager(),
            CSpbContentProvider::EStatusMessage | CSpbContentProvider::EServiceIcon );
        iStatusControl = CCCAppStatusControl::NewL( *iProvider, aObserver );
        iStatusControl->SetContainerWindowL( *this );
        iStatusControl->MakeVisible( EFalse );
        
        CFbsBitmap* bmp = NULL;
        CFbsBitmap* bmpMask = NULL;
                      
        AknIconUtils::CreateIconL(
            bmp,
            bmpMask,
            KMyCardIconDefaultFileName,
            EMbmPhonebook2eceQgn_prop_wml_bm_ovi,
            EMbmPhonebook2eceQgn_prop_wml_bm_ovi_mask );    
                
        CGulIcon* guiIcon = CGulIcon::NewL( bmp, bmpMask );
        iStatusControl->SetDefaultStatusIconL( guiIcon );
        HBufC* defaultText = StringLoader::LoadL( R_QTN_CCA_MC_MY_CARD_IN_OVI );                                            
        iStatusControl->SetDefaultStatusTextL( defaultText );        
        }
    FeatureManager::UnInitializeLib();
    
    
    // Create portrait image
    iPortraitImage = new(ELeave) CEikImage;
    iPortraitImage->SetAlignment(EHCenterVCenter);
    iPortraitImage->SetBrushStyle( CGraphicsContext::ENullBrush ); // transparent
    iPortraitImage->SetPictureOwnedExternally( ETrue );
      
    // Create my name label
    iLabel1 = new(ELeave) CEikLabel;
    iLabel1->SetAlignment(EHLeftVCenter);
    iLabel1->SetTextL( KNullDesC ); // CEikLabel will panic without text
    iLabel2 = new(ELeave) CEikLabel;
    iLabel2->SetAlignment(EHLeftVCenter);
    iLabel2->SetTextL( KNullDesC );
      
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardHeaderControl::ConstructL()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::~CCCAppMyCardHeaderControl
// ---------------------------------------------------------------------------
//
CCCAppMyCardHeaderControl::~CCCAppMyCardHeaderControl()
    {
    delete iStatusControl;
    delete iProvider;
    Release( iAppServices );    
    delete iPortraitImage;
    delete iLabel1;
    delete iLabel2;
    delete iPortraitBitmap;
    delete iPortraitMask;
	iObserverArray.Reset();
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCardHeaderControl::CountComponentControls() const
    {
    if(iStatusControl)
        {
        return KControlCount;
        }
    else
        {
        return (KControlCount - 1);
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CCCAppMyCardHeaderControl::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            {
            return iPortraitImage;
            }
        case 1:
        	{
        	return iLabel1;
        	}
        case 2:
            {
            return iLabel2;
            }
        case 3:
            {
            return iStatusControl;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SizeChanged
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SizeChanged()
    {
    const TRect rect(Rect()); 
    /**
     * Option0, Parent Variety :1 (w button, lsc)
     * Option2, Parent Variety :3 (w/o button, lsc)
     * Option0, Parent Variety :0 (w button, prt)
     * Option1, Parent Variety :2 (w/o button, prt)
     */
    
    const TBool isLandscape( Layout_Meta_Data::IsLandscapeOrientation() );
    
    // (w/o button)
    TInt option( isLandscape ? 2 : 1 );
    if( iStatusControl && iStatusControl->IsVisible() )
        {
        // (w button)
        if( isLandscape )
            {
            option = 0;
            }
        else
            {
            option = 0;
            }
        
        TAknWindowComponentLayout statusPaneLayout(
            AknLayoutScalable_Apps::phob2_cc_button_pane( option ) );
        
        TAknLayoutRect statusPaneLayoutRect;
        statusPaneLayoutRect.LayoutRect( rect, statusPaneLayout.LayoutLine() );
        TRect statusPaneRect( statusPaneLayoutRect.Rect() );
        iStatusControl->SetRect( statusPaneRect );
        }
    
    
    // image
    TAknWindowComponentLayout imageLayout( 
        AknLayoutScalable_Apps::phob2_cc_data_pane_g1( option ) );    
    
    TAknLayoutRect imageLayoutRect;
    imageLayoutRect.LayoutRect( rect, imageLayout.LayoutLine() );
    TRect imageRect( imageLayoutRect.Rect() );
    
    iPortraitImage->SetRect( imageRect );
    iPortraitImageSize = imageRect.Size();
   
    // label 1
    AknLayoutUtils::LayoutLabel( 
            iLabel1, 
            rect, 
            AknLayoutScalable_Apps::phob2_cc_data_pane_t1( option ) );

    // label 2
    AknLayoutUtils::LayoutLabel( 
            iLabel2, 
            rect, 
            AknLayoutScalable_Apps::phob2_cc_data_pane_t2( option ) );
    
    // resize images for new resolution
    if( iPortraitBitmap ) 
        {
        AknIconUtils::SetSize( iPortraitBitmap, 
                iPortraitImageSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
        }
    if( iPortraitMask )
        {
        AknIconUtils::SetSize( iPortraitMask, 
            iPortraitImageSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    switch(aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
 
            // Image selection popup
        	if ( iPortraitImage->Rect().Contains(aPointerEvent.iPosition) )
        		{
				// Show the feedback
				MTouchFeedback* feedback = MTouchFeedback::Instance();
				if ( feedback )
					{
					feedback->InstantFeedback( ETouchFeedbackBasic );
					}
				// check the pointer position so that menu can be opened
				// to the correct place
				NotifyObservers( aPointerEvent.iPosition );
        		}     	
            break;    
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SetContainerWindowL( 
    const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );

    // Assign window for child controls too
    TInt childCount = CountComponentControls();
    for( TInt i = 0; i < childCount; ++i )
        {
        ComponentControl( i )->SetContainerWindowL( *this );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SetPortraitIcon
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SetPortraitIcon( 
    CFbsBitmap* aImage, CFbsBitmap* aMask )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardHeaderControl::SetPortraitIcon()"));    

    delete iPortraitBitmap;
    iPortraitBitmap = aImage;

    delete iPortraitMask;
    iPortraitMask = aMask;
           
    // resize and assign
    if( iPortraitImageSize != TSize() )
        {
        AknIconUtils::SetSize( 
            iPortraitBitmap, 
            iPortraitImageSize, 
            EAspectRatioPreservedAndUnusedSpaceRemoved );

        AknIconUtils::SetSize( 
            iPortraitMask, 
            iPortraitImageSize, 
            EAspectRatioPreservedAndUnusedSpaceRemoved );
        }
    
    iPortraitImage->SetPicture( iPortraitBitmap, iPortraitMask );
    iPortraitImage->DrawDeferred();

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardHeaderControl::SetPortraitIcon()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SetPortraitBitmapL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SetPortraitBitmapL( CFbsBitmap* aImage )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardHeaderControl::SetPortraitBitmapL()"));    

    // create scalable image
    aImage = AknIconUtils::CreateIconL( aImage );
    delete iPortraitBitmap;
    iPortraitBitmap = aImage;
    
    delete iPortraitMask;
    iPortraitMask = NULL;
    
    // resize and assign
    if( iPortraitImageSize != TSize() )
        {
        AknIconUtils::SetSize( 
            iPortraitBitmap, 
            iPortraitImageSize, 
            EAspectRatioPreservedAndUnusedSpaceRemoved );

        }
    
    iPortraitImage->SetPicture( iPortraitBitmap );
    iPortraitImage->DrawDeferred();

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardHeaderControl::SetPortraitBitmapL()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SetNameTextL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SetLabel1TextL( const TDesC& aTxt )
    {
    iLabel1->SetTextL( aTxt );
    iLabel1->CropText();
    iLabel1->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardHeaderControl::SetLabel2TextL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardHeaderControl::SetLabel2TextL( const TDesC& aTxt )
    {
    iLabel2->SetTextL( aTxt );
    iLabel2->CropText();
    iLabel2->DrawDeferred();
    }

 // ---------------------------------------------------------------------------
 // CCCAppMyCardHeaderControl::AddObserverL
 // ---------------------------------------------------------------------------
 //
 void CCCAppMyCardHeaderControl::AddObserverL( 
         MMyCardHeaderControlObserver* aObserver )
     {
     if( aObserver )
         {
		 iObserverArray.AppendL( aObserver );
         }
     }
 
 // ---------------------------------------------------------------------------
 // CCCAppMyCardHeaderControl::NotifyObservers
 // ---------------------------------------------------------------------------
 //
 void CCCAppMyCardHeaderControl::NotifyObservers( TPoint aPoint)
	 {
	 const TInt count(iObserverArray.Count());
	 for(TInt i = 0 ; i < count ; ++i)
		{
		TRAP_IGNORE( iObserverArray[i]->MyCardHeaderControlClickL( aPoint ) );
		}
	 }
// End of file
