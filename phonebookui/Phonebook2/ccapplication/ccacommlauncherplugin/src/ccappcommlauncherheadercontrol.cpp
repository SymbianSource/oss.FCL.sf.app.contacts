/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the header part control(s) of the comm launcher view
*
*/


#include "ccappcommlauncherheadercontrol.h"
#include "CPbkThumbnailManager.h"
#include <phonebook2ece.mbg>
#include <aknstyluspopupmenu.h>
#include <CPbk2ContactRelocator.h>
#include <touchfeedback.h>
#include <w32std.h>

const TInt KLabelLineCount = 3;

// ---------------------------------------------------------------------------
// NewL, two-phase construction
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderControl* CCCAppCommLauncherHeaderControl::NewL( 
        CCCAppCommLauncherPlugin& aPlugin )
    {
    CCCAppCommLauncherHeaderControl* self= new (ELeave) CCCAppCommLauncherHeaderControl( aPlugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ (first phase) constructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderControl::CCCAppCommLauncherHeaderControl( CCCAppCommLauncherPlugin& aPlugin )
    : iHasContactImage(EFalse), iPlugin( aPlugin )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// ConstructL, second phase constructor
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ConstructL()
    { 
    // Create the header image
    iImage = new (ELeave) CEikImage();
    iImage->SetPictureOwnedExternally(ETrue);
    iImage->SetAlignment(EHCenterVCenter);
    
    // Create the header labels
    for (TInt i=0; i < KLabelLineCount; i++)
        {
        CEikLabel* label = new(ELeave) CEikLabel;
        CleanupStack::PushL(label);
        iLabels.AppendL(label);
        CleanupStack::Pop(label);
        iLabels[i]->SetTextL(KNullDesC());
        iLabels[i]->SetAlignment(EHLeftVCenter);
        iLabels[i]->CropText();
        }
    iContactImageSize = KPbkPersonalImageSize;
    // Create the text order
    iTextOrder = CCCAppCommLauncherHeaderTextOrder::NewL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderControl::~CCCAppCommLauncherHeaderControl()
    {
    delete iContactImageFullName;
    delete iContactThumbnailData;
    
    iCmsContactFields.Close(); 
    iLabels.ResetAndDestroy();
    if (iImage)
        {
        delete iImage;
        iImage = NULL;
        }
    if (iImageDecoding)
        {
        if (iImageDecoding->IsActive())
            {
            iImageDecoding->Cancel();
            }
        delete iImageDecoding;
        iImageDecoding = NULL;
        }
    if (iTextOrder)
        {
        delete iTextOrder;
        iTextOrder = NULL;
        }
    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if (iMask)
        {
        delete iMask;
        iMask = NULL;
        }
    if ( iImageSelectionPopup )
        {
        delete iImageSelectionPopup;
        iImageSelectionPopup = NULL;
        }
    if( iPbkCmd )
        {
        delete iPbkCmd;
        iPbkCmd = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl, returns the number of child controls to the framework
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherHeaderControl::CountComponentControls() const
    {
    return 1 + iLabels.Count();
    }

// ---------------------------------------------------------------------------
// From CCoeControl, returns the child controls to the framework by index
// ---------------------------------------------------------------------------
//
CCoeControl* CCCAppCommLauncherHeaderControl::ComponentControl(TInt aIndex)  const
    {
    if (aIndex == 0)
        {
        return iImage;
        }
    else if (aIndex - 1 < iLabels.Count())
        {
        return iLabels[aIndex-1];
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl, called when the control's size changes, handles portrait-landscape switch
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::SizeChanged()
    {
    TInt isLandscape = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    AknLayoutUtils::LayoutImage(
            iImage, Rect(), AknLayoutScalable_Apps::cl_header_pane_g1(isLandscape));
   
    TPoint position = iImage->Position();
    TSize size = iImage->Size();
    
    const TPoint newImagePosition = TPoint( position.iX/2, position.iY/2 );
    iImage->SetPosition( newImagePosition );
    
    const TSize newImageSize = TSize( size.iWidth + 2*( position.iX - newImagePosition.iX ) ,
            size.iHeight + 2*( position.iY - newImagePosition.iY ) );
    
    iImage->SetSize( newImageSize );
    
    iContactImageSize = newImageSize;
    
    if ( iImageDecoding )
        {
        // if bitmap was set resize it
        if (iImageDecoding->IsActive())
            {
            iImageDecoding->Cancel();
            }
        TRAP_IGNORE( iImageDecoding->StartL( iContactImageSize ) );
        }
    
    TAknLayoutRect labelsRect;
    labelsRect.LayoutRect(Rect(), AknLayoutScalable_Apps::cl_header_name_pane(isLandscape));
    
    AknLayoutUtils::LayoutLabel(iLabels[0], labelsRect.Rect(), AknLayoutScalable_Apps::cl_header_name_pane_t1(2));
    AknLayoutUtils::LayoutLabel(iLabels[1], labelsRect.Rect(), AknLayoutScalable_Apps::cl_header_name_pane_t2(2));
    AknLayoutUtils::LayoutLabel(iLabels[2], labelsRect.Rect(), AknLayoutScalable_Apps::cl_header_name_pane_t3(2));
    
    if( iImageSelectionPopup )
        {
        delete iImageSelectionPopup;
        iImageSelectionPopup = NULL;
        }
    
    TRAP_IGNORE(LabelsSizeChangedL());
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderControl::LabelsSizeChangedL()
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::LabelsSizeChangedL()
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color;
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
        
    if (iLabels.Count() > 0)
        {
        iTextOrder->ClipL(*iLabels[0]->Font(), iLabels[0]->Rect().Width());
        for (TInt i=0; i < iLabels.Count(); i++)
            {
            iLabels[i]->SetTextL(iTextOrder->GetTextForRow(i));
            iLabels[i]->OverrideColorL( EColorLabelText, color );
            }
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl, overridden to set the container windows for the child
// controls of this control (labels and image)
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CCoeControl::SetContainerWindowL(aContainer);
    iImage->SetContainerWindowL(aContainer);
    for (TInt i=0; i < iLabels.Count(); i++)
        {
        iLabels[i]->SetContainerWindowL(aContainer);
        }
    }

// ---------------------------------------------------------------------------
// The container calls this when its ContactInfoFetchedNotifyL (from 
// MCCAppContactHandlerNotifier) is called. Info about the contact is received
// here.
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ContactInfoFetchedNotifyL( 
    const CCmsContactFieldInfo& aContactFieldInfo )
    {
    }

// ---------------------------------------------------------------------------
// The container calls this when its ContactFieldFetchedNotifyL (from 
// MCCAppContactHandlerNotifier) is called. Data about the contact is received
// here.
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ContactFieldFetchedNotifyL( 
        const CCmsContactField& aContactField )
    {
    TInt field = aContactField.Type();
    if ( aContactField.Type() == CCmsContactFieldItem::ECmsImageName )
    	{
    	TPtrC data( aContactField.ItemL( 0 ).Data() );
        if ( data.Length() > 0 )
        	{
        	delete iContactImageFullName;
        	iContactImageFullName = NULL;
        	iContactImageFullName = data.AllocL();
        	}
    	}
    else if ( aContactField.Type() == CCmsContactFieldItem::ECmsThumbnailPic )
        {
        TPtrC8 data( aContactField.ItemL( 0 ).BinaryData() );
        if ( data.Length() > 0 )
        	{
        	delete iContactThumbnailData;
        	iContactThumbnailData = NULL;
        	iContactThumbnailData = data.AllocL();
        	}
        else
            {
            // When removing header image from the detail view of a contact,
            // making the picture as NULL firstly.
            iImage->SetPicture(NULL, NULL);
            delete iBitmap;
            iBitmap = NULL;
            delete iMask;
            iMask = NULL;
            }
        }
    else
        {
        iCmsContactFields.AppendL(aContactField);		
        }
    }

// ---------------------------------------------------------------------------
// Performs processing of retrieved contact fields 
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ContactFieldFetchingCompletedL()
    {
    ProcessContactImageDisplayL();
    iTextOrder->ProcessContactFieldsL(iCmsContactFields); 
    iCmsContactFields.Reset(); 
    
    if (iLabels.Count() > 0)
        {
        iTextOrder->ClipL(*iLabels[0]->Font(), iLabels[0]->Rect().Width());
        }
    for (TInt i = 0; i < iLabels.Count(); i++)
        {
        iLabels[i]->SetTextL(iTextOrder->GetTextForRow(i));
        }
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// Sets the bitmap shown in the header image 
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::SetBitmap(CFbsBitmap* aBmp)
    {
    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if (iMask)
        {
        delete iMask;
        iMask = NULL;
        }
    iBitmap = aBmp;
    iImage->SetPicture(aBmp, NULL);
    iImage->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// Clears the header texts to blank
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ClearL()
    {
    for (TInt i = 0; i < iLabels.Count(); i++)
        {
        iLabels[i]->SetTextL(KNullDesC());
        }
    iTextOrder->Reset();
    }

// ---------------------------------------------------------------------------
// SetContactStoreL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::SetContactStoreL(TCmsContactStore aContactStore)
    {
    if (!iBitmap && !iMask)
        {
        switch (aContactStore)
            {
            case ECmsContactStorePbk:
                AknIconUtils::CreateIconL( 
                    iBitmap, iMask, KPbk2ECEIconFileName, 
                    EMbmPhonebook2eceQgn_prop_pb_thumb_unknown, EMbmPhonebook2eceQgn_prop_pb_thumb_unknown_mask );
                iImage->SetPicture(iBitmap, iMask);
                SizeChanged();
                break;                
            
            case ECmsContactStoreSim:
                AknIconUtils::CreateIconL( 
                    iBitmap, iMask, KPbk2ECEIconFileName, 
                    EMbmPhonebook2eceQgn_note_sim, EMbmPhonebook2eceQgn_note_sim_mask );
                iImage->SetPicture(iBitmap, iMask);
                SizeChanged();
                break;
                
            case ECmsContactStoreSdn:
                AknIconUtils::CreateIconL( 
                    iBitmap, iMask, KPbk2ECEIconFileName, 
                    EMbmPhonebook2eceQgn_menu_simin, EMbmPhonebook2eceQgn_menu_simin_mask );
                iImage->SetPicture(iBitmap, iMask);
                SizeChanged();
                break;
            }
        }
     }

// ---------------------------------------------------------------------------
// HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    switch(aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:

            // Image selection popup
            if( iPlugin.ContactHandler().ContactStore() != ECmsContactStoreSdn 
                && IsPhoneMemoryInConfigurationL() 
                && iImage->Rect().Contains(aPointerEvent.iPosition) )
                {
                // Show the feedback
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( ETouchFeedbackBasic );
                    }
                LaunchStylusPopupL( aPointerEvent );
                }
            break;
            
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// ProcessCommandL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::ProcessCommandL(TInt aCommandId)
     {
     switch(aCommandId)
         {
         case ECCAppCommLauncherStylusViewImageCmd:
             DoViewImageCmdL();
             break;
             
         case ECCAppCommLauncherStylusChangeImageCmd:
             DoChangeImageCmdL();
             break;
             
         case ECCAppCommLauncherStylusRemoveImageCmd:
             DoRemoveImageCmdL();
             break;
             
         case ECCAppCommLauncherStylusAddImageCmd:
             DoAddImageCmdL();
             break;
             
         default:
             break;
         }
     }
 
// ---------------------------------------------------------------------------
// SetEmphasis
// ---------------------------------------------------------------------------
//
 void CCCAppCommLauncherHeaderControl::SetEmphasis(
         CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
     {
     // No implementation
     }

 // ---------------------------------------------------------------------------
 // PosToScreenCoordinates
 // ---------------------------------------------------------------------------
 //
 void CCCAppCommLauncherHeaderControl::PosToScreenCoordinates( 
         CCoeControl* aControl, TPoint& aPos )
      {
      TPoint leftUpperPos = aControl->Parent()->PositionRelativeToScreen();
      aPos += leftUpperPos;
      }
 
 // ---------------------------------------------------------------------------
 // LaunchStylusPopupL
 // ---------------------------------------------------------------------------
 //
void CCCAppCommLauncherHeaderControl::LaunchStylusPopupL( const TPointerEvent& aPointerEvent )
     {
     iPos = TPoint(0,0);
     if ( !iImageSelectionPopup )
         {
         iImageSelectionPopup = CAknStylusPopUpMenu::NewL( this, iPos, NULL );
         TInt resourceReaderId = R_CCACOMMLAUNCHER_CONTACT_IMAGE_STYLUS_MENU; 
         TResourceReader reader;
         iCoeEnv->CreateResourceReaderLC( reader , resourceReaderId );
         iImageSelectionPopup->ConstructFromResourceL( reader );
         CleanupStack::PopAndDestroy(); // reader
         }
     
     if( iHasContactImage )
         {
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusViewImageCmd, EFalse);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusChangeImageCmd, EFalse);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusRemoveImageCmd, EFalse);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusAddImageCmd, ETrue);
         }
     else
         {
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusViewImageCmd, ETrue);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusChangeImageCmd, ETrue);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusRemoveImageCmd, ETrue);
         iImageSelectionPopup->SetItemDimmed(ECCAppCommLauncherStylusAddImageCmd, EFalse);
        }
        
     iPos.iY = aPointerEvent.iPosition.iY;
     iPos.iX = aPointerEvent.iPosition.iX;
     PosToScreenCoordinates(this, iPos);
     
    

     iImageSelectionPopup->SetPosition(iPos);
     iImageSelectionPopup->ShowMenu();
     }

// ---------------------------------------------------------------------------
// DoViewImageCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderControl::DoViewImageCmdL()
    {
    if( !iPbkCmd )
        iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
        
    iPbkCmd->ExecutePbk2CmdViewImageL(
            *iPlugin.ContactHandler().ContactIdentifierLC() );

    CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC
    }

// --------------------------------------------------------------------------- 
// DoChangeImageCmdL 
// --------------------------------------------------------------------------- 
// 
void CCCAppCommLauncherHeaderControl::DoChangeImageCmdL() 
     { 
     if( !iPbkCmd ) 
         iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin ); 
          
     iPbkCmd->ExecutePbk2CmdChangeImageL( 
             *iPlugin.ContactHandler().ContactIdentifierLC() ); 
   
     CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC 
     } 
   
 // --------------------------------------------------------------------------- 
 // DoRemoveImageCmdL 
 // --------------------------------------------------------------------------- 
 // 
 void CCCAppCommLauncherHeaderControl::DoRemoveImageCmdL() 
     { 
     if( !iPbkCmd ) 
         iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin ); 
          
     iPbkCmd->ExecutePbk2CmdRemoveImageL( 
             *iPlugin.ContactHandler().ContactIdentifierLC() ); 
   
     CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC 
     } 
   
 // --------------------------------------------------------------------------- 
 // DoAddImageCmdL 
 // --------------------------------------------------------------------------- 
 // 
 void CCCAppCommLauncherHeaderControl::DoAddImageCmdL() 
     { 
     if( !iPbkCmd ) 
         iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin ); 
          
     iPbkCmd->ExecutePbk2CmdAddImageL( 
             *iPlugin.ContactHandler().ContactIdentifierLC() ); 
   
     CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC 
     } 
   
 // --------------------------------------------------------------------------
 // CCCAppCommLauncherHeaderControl::IsPhoneMemoryInConfigurationL
 // --------------------------------------------------------------------------
 //
 TBool CCCAppCommLauncherHeaderControl::IsPhoneMemoryInConfigurationL()
     {
     CPbk2ContactRelocator* contactRelocator = CPbk2ContactRelocator::NewL();
     CleanupStack::PushL( contactRelocator );
     TBool ret = contactRelocator->IsPhoneMemoryInConfigurationL();
     CleanupStack::PopAndDestroy( contactRelocator );
     return ret;
     }
 
 TKeyResponse CCCAppCommLauncherHeaderControl::OfferKeyEventL(
         const TKeyEvent& aKeyEvent, TEventCode aType)
     {
     TKeyResponse ret( EKeyWasNotConsumed );
     
     // If there is a command running in the background
     // the send key is automatically consumed. This 
     // avoids e.g launching of a call
     if ( iPbkCmd && iPbkCmd->IsPbk2CommandRunning() )
         {
         if (EKeyYes == aKeyEvent.iCode)
             {
             ret = EKeyWasConsumed;
             }
         }
     return ret;
     }
 
 void CCCAppCommLauncherHeaderControl::ProcessContactImageDisplayL()
	 {
	 iHasContactImage = EFalse;
	 
     if (iImageDecoding)
         {
         if (iImageDecoding->IsActive())
             {
             iImageDecoding->Cancel();
             }
         delete iImageDecoding;
         iImageDecoding = NULL;
         }
     
     if ( iContactThumbnailData )
    	 {
         iImageDecoding = CCCAppCommLauncherImageDecoding::NewL(this, iContactThumbnailData, iContactImageFullName );
         iImageDecoding->StartL( iContactImageSize );
         iHasContactImage = ETrue;
         }

     delete iContactImageFullName;
     iContactImageFullName = NULL;
     delete iContactThumbnailData;
     iContactThumbnailData = NULL;   
	 }
