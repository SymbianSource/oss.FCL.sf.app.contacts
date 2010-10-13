/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of commlauncher plugin control container
*
*/


// INCLUDE FILES
#include "ccappcommlauncherheaders.h"
#include "spbcontentprovider.h"
#include "ccappcommlaunchercustomlistbox.h"
#include <Pbk2PresentationUtils.h>
#include <CPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <utf.h>
#include <gulicon.h>
#include "ccaextensionfactory.h"
#include "ccaextensionfactory.hrh"
#include <phonebook2ece.mbg>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsUtils.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include "ccafactoryextensionnotifier.h"
#include <featmgr.h>

namespace {
const TInt KSocialPhonebookDisabled = 2;
const TInt KSocialPhonebookEnabled = 3;
_LIT( KCcaIconDefaultFileName, "\\resource\\apps\\phonebook2ece.mif" );
}

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CCCAppCommLauncherContainer()
// ----------------------------------------------------------------------------
//
CCCAppCommLauncherContainer::CCCAppCommLauncherContainer(
    CCCAppCommLauncherPlugin& aPlugin)
    : iPlugin (aPlugin)
    {
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::~CCCAppCommLauncherContainer()
// ----------------------------------------------------------------------------
//
CCCAppCommLauncherContainer::~CCCAppCommLauncherContainer()
    {    
    delete iListBox;
    delete iBackground;
    delete iModel;
    delete iHeaderCtrl;
    delete iPbkCmd;
    delete iStatusControl;
    delete iProvider;
    Release( iAppServices );
    delete iViewLauncher;
    delete iLongTapDetector;
    delete iFactoryExtensionNotifier;
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ConstructL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ConstructL()
    {
    // Create the listbox
    CreateListboxControlL();
    // Create the header
    iHeaderCtrl = CCCAppCommLauncherHeaderControl::NewL( iPlugin );
    iHeaderCtrl->SetContainerWindowL(*this);
    
    // create status control  
    CreateStatusControlL();     
    
    // Get the skin background for the view
    iBackground = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, TRect(0, 0, 0, 0), EFalse);
    LongTapDetectorL();    
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CreateListboxControlL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::CreateListboxControlL()
    {
    // Use customized list box 
    iListBox = CCCAppCommLauncherCustomListBox::NewL();
    iListBox->ConstructL(this, EAknListBoxLoopScrolling);
    iListBox->SetContainerWindowL(*this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    // Set empty text
    iListBox->View()->SetListEmptyTextL(KNullDesC());

    // Creating the model
    iModel = CCCAppCommLauncherLPadModel::NewL(*this, *iListBox, iPlugin);
    iListBox->Model()->SetItemTextArray(iModel);
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iModel->IconArray());

    //Set listbox observer
    iListBox->SetListBoxObserver(this);
    iListBox->ActivateL();
    }

//-----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CreateStatusControlL()
//-----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::CreateStatusControlL() 
    {
    FeatureManager::InitializeLibL();
    const TBool isFeatureIdFfContactsSocial =
            FeatureManager::FeatureSupported( KFeatureIdFfContactsSocial );
    FeatureManager::UnInitializeLib();

    if( isFeatureIdFfContactsSocial )
        {
        MVPbkContactLink* link = NULL;
        MVPbkContactLinkArray* contactArray = NULL;
        TInt isSame = KErrNotFound;

        if( !iAppServices )
            {
            iAppServices = CPbk2ApplicationServices::InstanceL();  
            }
        if( !iProvider )
            {
            iProvider = CSpbContentProvider::NewL( iAppServices->ContactManager(), 
                    iAppServices->StoreManager(),
                    CSpbContentProvider::EStatusMessage | 
					    CSpbContentProvider::EServiceIcon );
            }
        HBufC& contactData = iPlugin.AppEngine()->Parameter().ContactDataL();
        HBufC8* contactData8 = HBufC8::NewLC( contactData.Size() );
        TPtr8 contactData8Ptr( contactData8->Des() );
		contactData8Ptr.Copy( contactData.Des() ); 
        
        CVPbkContactManager* vPbkContactManager = 
                &iAppServices->ContactManager();
        
        if( vPbkContactManager )
            {
            contactArray = vPbkContactManager->CreateLinksLC( contactData8Ptr );

           if( contactArray->Count() > 0 )
                {
                link = contactArray->At( 0 ).CloneLC();
                }				
                            
            if ( link )
                {    
                const MVPbkContactStoreProperties& storeProperties = 
                    link->ContactStore().StoreProperties();
                TVPbkContactStoreUriPtr uri = storeProperties.Uri();
            
                isSame = uri.Compare( VPbkContactStoreUris::DefaultCntDbUri(), 
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents );
                }
            }
            
        if( isSame == 0 )
            {		
            iStatusControl = CCCAppStatusControl::NewL( *iProvider, *this, CCCAppStatusControl::ENormalContact );
            iStatusControl->SetContainerWindowL( *this );                   
            iStatusControl->MakeVisible( EFalse );
            iHeaderCtrl->SetStatusButtonVisibility( EFalse );
            CFbsBitmap* bmp = NULL;
            CFbsBitmap* bmpMask = NULL;

            AknsUtils::CreateIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnPropSocialCommunities,
                bmp,
                bmpMask,
                KCcaIconDefaultFileName,
                EMbmPhonebook2eceQgn_prop_social_communities,
                EMbmPhonebook2eceQgn_prop_social_communities_mask );    
        
            CGulIcon* guiIcon = CGulIcon::NewL( bmp, bmpMask );
            iStatusControl->SetDefaultStatusIconL( guiIcon );
            HBufC* defaultText = 
                    StringLoader::LoadL( R_QTN_CCA_SOCIAL_NETWORKS );                                            
            iStatusControl->SetDefaultStatusTextL( defaultText );        
            iStatusControl->SetContactLinkL( *link );
	
            iFactoryExtensionNotifier = CCCaFactoryExtensionNotifier::NewL();
            TCallBack callBack( 
                    CCCAppCommLauncherContainer::CheckExtensionFactoryL, this );
            iFactoryExtensionNotifier->ObserveExtensionFactoryL( callBack );
            }
        
        if( link )
            {
            CleanupStack::PopAndDestroy(); //link
            }
        
        if( contactArray )
            {
            CleanupStack::PopAndDestroy(); // contactArray
            }
                        
        CleanupStack::PopAndDestroy(); // contactData8	
		} //  KFeatureIdFfContactsSocial	
	}
	
	
// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::Draw()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::Draw(const TRect& /*aRect*/) const
    {
    // Draw the background using skin
    if (iBackground)
        {
        AknsDrawUtils::Background(
            AknsUtils::SkinInstance(), iBackground, this, SystemGc(), Rect());
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::SizeChanged()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::SizeChanged()
    {
    const TRect rect(Rect());
    const TBool isLandscape(Layout_Meta_Data::IsLandscapeOrientation());

    // contact card pane layout
    TAknWindowComponentLayout contactCardPaneLayout( 
            TAknWindowComponentLayout::Compose( 
                    AknLayoutScalable_Apps::main_phob2_pane( 0 ),
                    AknLayoutScalable_Apps::phob2_contact_card_pane( 1 ) ) );

    
    TInt option( isLandscape ? 3 : 2 ); // (w/o button)
    if( iStatusControl && iStatusControl->IsVisible()  )
        {
        // (w button)
        if( isLandscape )
            {
            option = 1;
            }
        else
            {
            option = 0;
            }
			
        // Status control layout
        TAknWindowComponentLayout statusPaneLayout(
            TAknWindowComponentLayout::Compose(
                TAknWindowComponentLayout::Compose( 
                    contactCardPaneLayout,
                    AknLayoutScalable_Apps::phob2_cc_data_pane( option ) ),
                AknLayoutScalable_Apps::phob2_cc_button_pane( option ) ) );
        
        TAknLayoutRect statusPaneLayoutRect;
        statusPaneLayoutRect.LayoutRect( rect, statusPaneLayout.LayoutLine() );
        TRect statusPaneRect( statusPaneLayoutRect.Rect() );
        iStatusControl->SetRect( statusPaneRect );
        }
    // Header and Listbox
    
    // header layout
    TAknWindowComponentLayout headerPaneLayout( 
        TAknWindowComponentLayout::Compose( 
            contactCardPaneLayout,
            AknLayoutScalable_Apps::phob2_cc_data_pane( option ) ) );
    
    TAknLayoutRect headerPaneLayoutRect;
    headerPaneLayoutRect.LayoutRect( rect, headerPaneLayout.LayoutLine() );
    TRect headerPaneRect( headerPaneLayoutRect.Rect() );
    iHeaderCtrl->SetRect( headerPaneRect );
    
    // list layout
    TAknWindowComponentLayout listPaneLayout( 
        TAknWindowComponentLayout::Compose( 
            contactCardPaneLayout,
            AknLayoutScalable_Apps::phob2_cc_listscroll_pane( option ) ) );
    
    TAknLayoutRect listPaneLayoutRect;
    listPaneLayoutRect.LayoutRect( rect, listPaneLayout.LayoutLine() );
    TRect listPaneRect( listPaneLayoutRect.Rect() );
    iListBox->SetRect( listPaneRect );

    // Background skin
    if (iBackground)
        {
        iBackground->SetRect(rect);
        }   
    
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandleResourceChange()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandleResourceChange(TInt aType)
	{
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{
		TRAP_IGNORE( iModel->ResourceChangedL() );		 
		}
	CCCAppViewPluginAknContainer::HandleResourceChange(aType);
	}

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CountComponentControls()
// ----------------------------------------------------------------------------
//
TInt CCCAppCommLauncherContainer::CountComponentControls() const
    {
	if( iStatusControl )
		{
		return KSocialPhonebookEnabled;
		}
	else
		{
		return KSocialPhonebookDisabled;
		}
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ComponentControl()
// ----------------------------------------------------------------------------
//
CCoeControl* CCCAppCommLauncherContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iHeaderCtrl;
        case 1:
        	if( iStatusControl )
        		{
				return iStatusControl;
        		}
        	else
        		{
				return iListBox;
        		}
        case 2:
        	if( iStatusControl )
        		{
				return iListBox;
        		}
        	else
        		{
				return NULL;
        		}
        default:
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::OfferKeyEventL()
// ----------------------------------------------------------------------------
//
TKeyResponse CCCAppCommLauncherContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // Make sure timer can be canceled before user do any operation.
    iPlugin.CancelTimer();
    
    // Forward the key-event 1st to base-class
    TKeyResponse returnValue =
        CCCAppViewPluginAknContainer::OfferKeyEventL(aKeyEvent, aType);

    // Offer event to header control.
    if (EEventKey == aType
        && EKeyWasNotConsumed == returnValue)
        {
        returnValue = iHeaderCtrl->OfferKeyEventL(aKeyEvent, aType);
        }

	/* Uncomment this code if it is necessary to enable navigation hardware keys
     * on devices which have these keys (its Map Extension feature)
    if( aKeyEvent.iScanCode == EStdKeyApplicationD && aType == EEventKeyUp )
        {
        DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionAssignFromMap );
        returnValue = EKeyWasConsumed;
        }

    if( aKeyEvent.iScanCode == EStdKeyApplicationE && aType == EEventKeyUp )
        {
        DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionShowOnMap );
        returnValue = EKeyWasConsumed;
        }*/

    if (EEventKey == aType
        && EKeyWasNotConsumed == returnValue)
        {
        // Make send key work as the select key for the list
        TKeyEvent keyToList = aKeyEvent;
        if (EKeyYes == aKeyEvent.iCode && IsFocused())
            {
            keyToList.iCode = EKeyEnter;
            }
        returnValue = iListBox->OfferKeyEventL(keyToList, aType);
        // Update popup when moving up or down in the list
        switch(aKeyEvent.iCode)
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                returnValue = EKeyWasConsumed;
                break;
            }
        }

    return returnValue;
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::MopSupplyObject()
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCCAppCommLauncherContainer::MopSupplyObject(TTypeUid aId)
    {
    // For skinning
    if (iBackground && aId.iUid == MAknsControlContext::ETypeId)
        {
        return MAknsControlContext::SupplyMopObject(aId, iBackground);
        }

    return CCoeControl::MopSupplyObject(aId);
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::FocusChanged()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::FocusChanged(TDrawNow aDrawNow)
    {
    // For enabling animated hilight (if in skin supports it)
    CCoeControl::FocusChanged(aDrawNow);
    if(iListBox)
        {
        iListBox->SetFocus(IsFocused(), aDrawNow);
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandlePointerEventL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    // Make sure timer can be canceled before user do any operation.
    iPlugin.CancelTimer();
    
    TInt index;
    // start iLongTapDetector in listbox area and handle PointerEvent when dragged
    if ( iListBox->View()->XYPosToItemIndex( aPointerEvent.iPosition, index )
        || ( aPointerEvent.iType == TPointerEvent::EDrag ) )
        {
        if ( iLongTapDetector )
            {
            iLongTapDetector->PointerEventL( aPointerEvent );
            }
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            // Pressed Down Effect
            iListBox->View()->ItemDrawer()->SetFlags(
                    CListItemDrawer::EPressedDownState );
            }
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandleLongTapEventL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/, 
                                 	const TPoint& /*aPenEventScreenLocation*/ )
    {
    if ( iPlugin.CommandState().IsRunning() )
        {
        return;
        }
    else if ( CommMethodsAvailable() )
        {
        TPtrC fullName;
        iPlugin.ContactHandler().ContactFieldItemDataL(
            CCmsContactFieldItem::ECmsFullName, fullName );

        TUint paramFlag = 0;//CCAContactorService::TCSParameter::EEnableDefaults;
    
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
            contactActionType = iPlugin.Container().SelectedCommunicationMethod();
        
        if ( !iLongTap && contactActionType
                        == VPbkFieldTypeSelectorFactory::EFindOnMapSelector )
            {  
            iLongTap = ETrue;    
            DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionShowOnMap );
            }
        else
            {
            CCAContactorService::TCSParameter param(
                contactActionType,
                *iPlugin.ContactHandler().ContactIdentifierLC(),//contactlinkarray
                paramFlag, 
                fullName );
               
            iPlugin.ExecuteServiceL( param );   
            
            //The Timer can be started after user selected any call item
            if ( iPlugin.ContactorService()->IsSelected() && 
                (contactActionType == VPbkFieldTypeSelectorFactory::EVoiceCallSelector ||
                 contactActionType == VPbkFieldTypeSelectorFactory::EVideoCallSelector ||
                 contactActionType == VPbkFieldTypeSelectorFactory::EVOIPCallSelector) )
                {
                iPlugin.StartTimerL();
                }
            
            
            CleanupStack::PopAndDestroy( 1 );// contactlinkarray
            }
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    // Handle pointer events
    TBool executeContactAction = EFalse;

    if (aEventType == EEventItemDraggingActioned)
        {
        iHasBeenDragged = ETrue;
        }
    else if (aEventType == EEventItemSingleClicked)
    	{
        if (!iHasBeenDragged)
            {
            executeContactAction = ETrue;
            }
        else
            {
            iHasBeenDragged = EFalse;
            }
        }
    else if (aEventType == EEventEnterKeyPressed)
        {
        executeContactAction = ETrue;
        iHasBeenDragged = EFalse;
        }

    if ( executeContactAction && iPlugin.CommandState().IsRunning() )
        {
        executeContactAction = EFalse;
        }

    if (executeContactAction)
        {
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
            contactActionType = iPlugin.Container().SelectedCommunicationMethod();
        
        if ( contactActionType
        		== VPbkFieldTypeSelectorFactory::EFindOnMapSelector )
        	{
            HandleFindOnMapContactActionL();           
        	}
        else
        	{
            HandleGenericContactActionL( contactActionType );
            }
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandleFindOnMapContactActionL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandleFindOnMapContactActionL()
    {
    if ( !iLongTap )
        {
        DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionShowOnMap );
        }
    else
        {
        iLongTap = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HandleGenericContactActionL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::HandleGenericContactActionL(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType )
    {
    TPtrC fullName;
    iPlugin.ContactHandler().ContactFieldItemDataL(
        CCmsContactFieldItem::ECmsFullName, fullName );  
    
    //contactlinkarray
    HBufC8* contactIdentifier = iPlugin.ContactHandler().ContactIdentifierLC();
    
    TBool fieldSelectionReq(ETrue);
    TPtrC selectedField;
    if ( VPbkFieldTypeSelectorFactory::EVoiceCallSelector == aActionType )
        {
        fieldSelectionReq = IsVoiceCallFieldSelectionAmbiguous( aActionType, selectedField );        
        }   
    
    
    if( fieldSelectionReq ) 
        {
        ExecuteContactActionServiceWithFieldSelectionL( 
            aActionType, 
            *contactIdentifier,
            fullName );
        }
    else
        {
        // Speed up contact action by skipping slow field selection operation
        ExecuteContactActionServiceWithoutFieldSelectionL(
            aActionType,
            *contactIdentifier,
            fullName,
            selectedField );          
        }
    
    //The Timer can be started after user selected any call item
    if ( iPlugin.ContactorService()->IsSelected() && 
        ( aActionType == VPbkFieldTypeSelectorFactory::EVoiceCallSelector ||
          aActionType == VPbkFieldTypeSelectorFactory::EVideoCallSelector ||
          aActionType == VPbkFieldTypeSelectorFactory::EVOIPCallSelector) )
        {
        iPlugin.StartTimerL();
        }
    
    CleanupStack::PopAndDestroy(1);// contactlinkarray
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::IsVoiceCallFieldSelectionAmbiguous()
// ----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContainer::IsVoiceCallFieldSelectionAmbiguous(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
    TPtrC& aSelectedField )
    {
    CCCAppCommLauncherContactHandler& handler = iPlugin.ContactHandler();
    
    if ( HasContactSingleAddress( aActionType, handler ) )
        {
        RPointerArray<CCmsContactField>& fields = handler.ContactFieldDataArray();
        
        for( TInt i=0; i < fields.Count(); i++ ) 
            {
            CCmsContactField* field = fields[i];
            
            if ( HasFieldOnlyOneItem( *field ) &&
                 IsVoiceCallType( *field ) ) 
                {
                const CCmsContactFieldItem* fieldItem = NULL;
                
                TInt error = KErrNone;
                TRAP( error, fieldItem = &field->ItemL( 0 ) );
                
                if( error == KErrNone ) 
                    {
                    aSelectedField.Set(fieldItem->Data());
                    //Its safe to exit here
                    //Making of Voice Call is not tedious
                    //we have only one number
                    return EFalse;
                    }                   
                }           
            }
        }
    
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HasContactSingleAddress()
// ----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContainer::HasContactSingleAddress(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
    CCCAppCommLauncherContactHandler& aHandler )
    {
    TInt amount = aHandler.AddressAmount( aActionType );
    if( 1 == amount )
        {
        return ETrue;
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::HasFieldOnlyOneItem()
// ----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContainer::HasFieldOnlyOneItem( const CCmsContactField& field ) const
    {
    const RPointerArray<CCmsContactFieldItem>& items = field.Items();
    
    if ( 1 == items.Count() )
        {
        return ETrue;
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::IsVoiceCallType()
// ----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContainer::IsVoiceCallType( const CCmsContactField& field ) const
    {
    CCmsContactFieldItem::TCmsContactField fieldType = field.Type();
    
    if( fieldType == CCmsContactFieldItem::ECmsLandPhoneGeneric ||
        fieldType == CCmsContactFieldItem::ECmsLandPhoneHome ||
        fieldType == CCmsContactFieldItem::ECmsLandPhoneWork ||
        fieldType == CCmsContactFieldItem::ECmsMobilePhoneGeneric ||
        fieldType == CCmsContactFieldItem::ECmsMobilePhoneHome ||
        fieldType == CCmsContactFieldItem::ECmsMobilePhoneWork  )
        {    
        return ETrue;
        }
    
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ExecuteContactActionServiceWithFieldSelectionL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ExecuteContactActionServiceWithFieldSelectionL( 
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
    TDesC8& aContactIdentifier,
    TDesC& aFullName )
    {
    TUint paramFlag = CCAContactorService::TCSParameter::EEnableDefaults;

    CCAContactorService::TCSParameter param(
        aActionType,
        aContactIdentifier,
        paramFlag,
        aFullName );
    
    iPlugin.ExecuteServiceL(param);    
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ExecuteContactActionServiceWithoutFieldSelectionL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ExecuteContactActionServiceWithoutFieldSelectionL( 
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
    TDesC8& aContactIdentifier,
    TDesC& aFullName,
    TDesC& aSelectedField )
    {
    CCAContactorService::TCSParameter param(
        aActionType,
        aContactIdentifier,
        CCAContactorService::TCSParameter::EUseFieldParam,
        aFullName,
        aSelectedField );
    
    iPlugin.ExecuteServiceL(param);
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactInfoFetchedNotifyL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ContactInfoFetchedNotifyL(
    const CCmsContactFieldInfo& aContactFieldInfo)
    {
    // update buttons
    iModel->UpdateAddressesValidationL( aContactFieldInfo );
    iModel->FillButtonArrayL();
    iListBox->HandleItemAdditionL();
    iHeaderCtrl->ContactInfoFetchedNotifyL(aContactFieldInfo);

    if (!CommMethodsAvailable())
        {
        // update the empty string now to avoid flickering
        HBufC* string = StringLoader::LoadLC(
            R_QTN_CCA_NO_COMMUNICATION_METHODS, iCoeEnv);
        iListBox->View()->SetListEmptyTextL(*string);
        CleanupStack::PopAndDestroy(string);
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactFieldFetchedNotifyL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ContactFieldFetchedNotifyL(
    const CCmsContactField& aContactField)
    {
    // Forwarding to header-part
    iHeaderCtrl->ContactFieldFetchedNotifyL(aContactField);
    // Forwarding to listbox-model
    iModel->ContactFieldFetchedNotifyL(aContactField);
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactFieldFetchedNotifyL()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ContactFieldFetchingCompletedL()
    {
    //PERFORMANCE LOGGING: 12. Plugin ready
    WriteToPerfLog();
    iHeaderCtrl->ContactFieldFetchingCompletedL();

    iHeaderCtrl->SetContactStoreL(iPlugin.ContactHandler().ContactStore());

    //Reset focused row in listbox if items removed/added.
    TInt mdlCount = iModel->MdcaCount();
    if (iMdlRowCount != mdlCount)
        {
        iListBox->SetCurrentItemIndexAndDraw(0);
        }
    iListBox->DrawDeferred();
    
    iMdlRowCount = mdlCount;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::Plugin
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPlugin& CCCAppCommLauncherContainer::Plugin()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherContainer::Plugin()"));
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CommMethodsAvailable
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContainer::CommMethodsAvailable()
    {
    return (KErrNotFound == iListBox->CurrentItemIndex())
        ? EFalse : ETrue;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::IsHighlight
// ---------------------------------------------------------------------------
//
const TBool CCCAppCommLauncherContainer::IsListBoxHighlightEnabled()
	{
    return !( iListBox->ItemDrawer()->Flags() & 
    		CListItemDrawer::ESingleClickDisabledHighlight );
	}


// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::SelectedCommunicationMethod
// ---------------------------------------------------------------------------
//
VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
    CCCAppCommLauncherContainer::SelectedCommunicationMethod()
    {
    return iModel->ButtonData(iListBox->CurrentItemIndex()).iContactAction;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactsChangedL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ContactsChangedL()
    {
    iModel->Reset();
    iListBox->HandleItemRemovalL();
    iHeaderCtrl->ClearL();

    if (!iStatusControl && iPlugin.ContactHandler().ContactStore() == ECmsContactStorePbk)
        {
        // create status control 
        CreateStatusControlL();
        //reset control's rect
        SizeChanged();
        }
	
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactPresenceChangedL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::ContactPresenceChangedL(
    const CCmsContactField& aContactField)
    {
    iModel->ContactPresenceChangedL(aContactField);
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::MapCommMethodToClipDirection
// ---------------------------------------------------------------------------
//
AknTextUtils::TClipDirection CCCAppCommLauncherContainer::MapCommMethodToClipDirection(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction)
    {
    switch (aContactAction)
        {
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            return AknTextUtils::EClipFromBeginning;
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            return AknTextUtils::EClipFromEnd;
        default:
            return AknTextUtils::EClipFromEnd;
        }
    }

// ----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    aContext.iMajor.iUid = KPbk2UID3;
    aContext.iContext = KHLP_CCA_LAUNCHER;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::DoShowMapCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::DoShowMapCmdL( TPbk2CommandId aCommandId )
    {
    if( !iPbkCmd )
    	{
        iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
    	}

   
    iPbkCmd->ExecutePbk2CmdShowMapL(
            iPlugin.ContactHandler().ContactIdentifierLC(), aCommandId );

    CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC
   
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContainer::LongTapDetectorL
// --------------------------------------------------------------------------
//    
CAknLongTapDetector& CCCAppCommLauncherContainer::LongTapDetectorL()
    {
    if ( !iLongTapDetector )
        {
        iLongTapDetector = CAknLongTapDetector::NewL( this );
        }
    return *iLongTapDetector;
    }

//-----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::StatusClicked()
//-----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::StatusClickedL()
	{
	if ( iViewLauncher )
		{
        HBufC& contactData = iPlugin.AppEngine()->Parameter().ContactDataL();
        HBufC8* contactData8 = HBufC8::NewLC( contactData.Size() );
        TPtr8 contactData8Ptr( contactData8->Des() );
        contactData8Ptr.Copy( contactData.Des() );
        iViewLauncher->LaunchViewL( MCCAViewLauncher::ECCAView, &contactData8Ptr );
        CleanupStack::PopAndDestroy( contactData8 );
		}
	}

//-----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::CheckExtensionFactoryL()
//-----------------------------------------------------------------------------
//
TInt CCCAppCommLauncherContainer::CheckExtensionFactoryL(TAny* aPtr)
    {
    CCCAppCommLauncherContainer* self = static_cast<CCCAppCommLauncherContainer*>( aPtr );
    self->DoCheckExtensionFactoryL();
    return 0;
    }

//-----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::DoCheckExtensionFactoryL()
//-----------------------------------------------------------------------------
//
void CCCAppCommLauncherContainer::DoCheckExtensionFactoryL()
    {
    CCCAExtensionFactory* extension = iFactoryExtensionNotifier->ExtensionFactory();
    
    MCCAStatusProvider* ccaStatusProvider = NULL;
    
    // if extension is not null, extensionfactory plugins are available ->
    // show statuscontrol        
    if( extension )
        {
        if ( !iViewLauncher )
            {
            iViewLauncher = extension->CreateViewLauncherL();
            }
        if( iStatusControl )
            {        
			if( extension )
				{				
				TAny* factoryExtension = extension->FactoryExtension( KCCAExtensionFactoryStatusProviderCreatorUid );        	
				
				 if( factoryExtension )
					 {
					 MCCAExtensionFactoryStatusProviderCreator* statusProviderCreator =
							 static_cast<MCCAExtensionFactoryStatusProviderCreator*>( factoryExtension );

					   if( statusProviderCreator )
						   {
                           ccaStatusProvider = statusProviderCreator->CreateStatusProviderL();						   
						   }
					 }
				}
        
			iStatusControl->SetStatusProvider( ccaStatusProvider );
            iStatusControl->MakeVisible( ETrue );
            iHeaderCtrl->SetStatusButtonVisibility( ETrue );
            }
        }
    else
        {
        delete iViewLauncher;
        iViewLauncher = NULL;
        if( iStatusControl )
            {
            iStatusControl->MakeVisible( EFalse );
            iHeaderCtrl->SetStatusButtonVisibility( EFalse );
            }
        }
    
    }

//-----------------------------------------------------------------------------
// CCCAppCommLauncherContainer::GetListBoxItemAmount()
//-----------------------------------------------------------------------------
//
const TInt CCCAppCommLauncherContainer::GetListBoxItemAmount() const
	{
	return iModel->MdcaCount();
	}
// End of File
