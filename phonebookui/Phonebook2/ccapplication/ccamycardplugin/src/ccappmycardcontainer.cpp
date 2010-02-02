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
* Description:  Implementation of mycard plugin control container
*
*/


// INCLUDE FILES
#include <ccaextensionfactory.h>

#include <aknlists.h>
#include <eiktxlbm.h>
#include <barsread.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknUtils.h>

#include <MPbk2ContactNameFormatter.h>
#include <MVPbkStoreContact.h>
#include <CPbk2IconArray.h>
#include <Pbk2UIControls.rsg>
#include <mpbk2command.h>
#include <pbk2menufilteringflags.hrh>
#include <mvpbkcontactlink.h>

#include "ccappmycard.hrh"
#include "ccappmycard.h"
#include "ccappmycardcontainer.h"
#include "ccappmycardcommon.h"
#include "ccappmycardheadercontrol.h"
#include "ccappmycardlistboxmodel.h"
#include "ccappmycardplugin.h"
#include "ccafactoryextensionnotifier.h"
#include "spbcontentprovider.h"

#include <MVPbkContactFieldTextData.h>
#include <ccaextensionfactory.hrh>

#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsUtils.h>
#include <ccappmycardpluginrsc.rsg>
#include <gulicon.h>
#include <phonebook2ece.mbg>
#include <VPbkEng.rsg>
#include "MVPbkFieldType.h"
#include <aknstyluspopupmenu.h>
#include <e32std.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <Pbk2PresentationUtils.h>
#include <Pbk2Presentation.rsg>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <pbk2nameordercenrep.h>
#include <centralrepository.h>
#include <Phonebook2PrivateCRKeys.h>

// unnamed namespace
namespace
{
_LIT( KMyCardIconDefaultFileName, "\\resource\\apps\\phonebook2ece.mif" );
const TText KLineChange = '\n';
const TInt KSubComponentCount = 2;
const TInt KNumberOfContacts = 1;
const TInt KNoContacts = 0;
const TInt KPbk2UiSpecNameOrderFirstNameLastName = 0;
const TInt KPbk2UiSpecNameOrderLastNameFirstName = 1;
}

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::CCCAppMyCardContainer()
// ----------------------------------------------------------------------------
//
CCCAppMyCardContainer::CCCAppMyCardContainer(
    CCCAppMyCardPlugin& aPlugin ) :
    iPlugin( aPlugin ),
    iNameOrder(KPbk2UiSpecNameOrderFirstNameLastName)
    {
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::~CCCAppMyCardContainer()
// ----------------------------------------------------------------------------
//
CCCAppMyCardContainer::~CCCAppMyCardContainer()
    {
	iPlugin.MyCard().RemoveObserver( this );
	delete iBackground;
    delete iHeaderCtrl;
    delete iListBox;
    delete iImageLoader;
    delete iFactoryExtensionNotifier;
    delete iViewLauncher;
    delete iImageSelectionPopup;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::ConstructL()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ConstructL()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardContainer::ConstructL()"));

    iPlugin.MyCard().AddObserverL( this );

    ResolveNameOrderL();
    
    // Header UI control
    iHeaderCtrl = CCCAppMyCardHeaderControl::NewL( *this );
    iHeaderCtrl->SetContainerWindowL( *this );
    iHeaderCtrl->AddObserverL( this );

    // Details list UI control
    iListBox = new(ELeave) CAknFormDoubleGraphicStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->SetContainerWindowL( *this );

    // Setup listbox
    iListBox->View()->SetListEmptyTextL( KNullDesC );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );

    // Create icons for listbox.
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_PBK2_FIELDTYPE_ICONS );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL( reader );
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
    CleanupStack::PopAndDestroy(); // reader

    // Create listbox model
    iModel = CCCAppMyCardListBoxModel::NewL(
        iPlugin.MyCard(), *iCoeEnv, *iListBox, *iconArray );
    iListBox->Model()->SetItemTextArray( iModel );

    // Get the skin background for the view
    iBackground = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, TRect(0, 0, 0, 0), EFalse );

    // Start observing now that the class is ready
    iListBox->SetListBoxObserver( this );
    iPlugin.MyCard().AddObserverL( this );


    if( iPlugin.MyCard().IsContactLinkReady() )
    	{
        CCCAppStatusControl* statusControl = iHeaderCtrl->StatusControl();
    	if( statusControl )
    	    {
            statusControl->SetContactLinkL( iPlugin.MyCard().ContactLink() );
    	    }
		iControlLink = ETrue;
    	}
    iFactoryExtensionNotifier = CCCaFactoryExtensionNotifier::NewL();

    TCallBack callBack( CCCAppMyCardContainer::CheckExtensionFactoryL, this );
    iFactoryExtensionNotifier->ObserveExtensionFactory( callBack );
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardContainer::ConstructL()"));
    }


/**
 * Gets a digit from a descriptor.
 *
 * @param aReaderToBuf  Resource reader pointed to a descriptor resource.
 * @return  The digit.
 */
inline TInt GetDigitFromDescriptorL( TResourceReader& aReaderToBuf )
    {
    HBufC* orderBuf = aReaderToBuf.ReadHBufCL();
    CleanupStack::PushL( orderBuf );

    TInt result = KPbk2UiSpecNameOrderFirstNameLastName;
    TPtrC ptr( orderBuf->Des() );
    if ( ptr.Length() > 0 && TChar(ptr[0]).IsDigit() )
        {
        result = TChar(ptr[0]).GetNumericValue();
        }

    CleanupStack::PopAndDestroy( orderBuf );
    return result;
    }


// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::ResolveNameOrderL()
// ----------------------------------------------------------------------------
//
inline void CCCAppMyCardContainer::ResolveNameOrderL()
    {
    TInt orderInCenRep = EPbk2CenRepNameOrderUndefined;

    CRepository* sortOrderSettings = CRepository::NewLC(TUid::Uid(KCRUidPhonebook));
    sortOrderSettings->Get(KPhonebookNameOrdering, orderInCenRep);
    CleanupStack::PopAndDestroy(sortOrderSettings);    
    
    
    if( orderInCenRep == EPbk2CenRepLastNameFirstName ||
        orderInCenRep == EPbk2CenRepFirstNameLastName )
        {
        iNameOrder = KPbk2UiSpecNameOrderFirstNameLastName;
        if( orderInCenRep == EPbk2CenRepLastNameFirstName )
            {
            iNameOrder = KPbk2UiSpecNameOrderLastNameFirstName;
            }
        }
    else
        {
        // Read order from resource
        
        // Open resource file for reading language specific default values
        RPbk2LocalizedResourceFile resFile;
        resFile.OpenLC( KPbk2RomFileDrive,
                KDC_RESOURCE_FILES_DIR, 
                Pbk2PresentationUtils::PresentationResourceFile() );

        // Read the name order value from the resource file
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_QTN_PHOB_NAME_ORDER ) );
        TInt digit = GetDigitFromDescriptorL( reader );
        CleanupStack::PopAndDestroy( 2 ); // reader & resFile
        if( ( digit == KPbk2UiSpecNameOrderFirstNameLastName ) ||
            ( digit == KPbk2UiSpecNameOrderLastNameFirstName ) )
            {
            iNameOrder = digit;
            }    
        }
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::Draw()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::Draw(const TRect& /*aRect*/) const
    {
    // Draw the background using skin
    if (iBackground)
        {
        AknsDrawUtils::Background(
            AknsUtils::SkinInstance(), iBackground, this, SystemGc(), Rect());
        }
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::SizeChanged()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SizeChanged()
    {
    TRect rect( Rect() );

    // Background skin
    iBackground->SetRect( rect );

    /**
     * Option0 (w button, prt)
     * Option2 (w/o button, prt)
     * Option1 (w button, lsc)
     * Option3 (w/o button, lsc)
     */
    
    const TBool isLandscape( Layout_Meta_Data::IsLandscapeOrientation() );
    
    TInt option( isLandscape ? 3 : 2 ); // (w/o button)
    CCCAppStatusControl* statusControl = iHeaderCtrl->StatusControl();
    if( statusControl && statusControl->IsVisible() )
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
        }
    
    // contact card pane layout
    TAknWindowComponentLayout contactCardPaneLayout( 
            TAknWindowComponentLayout::Compose( 
                    AknLayoutScalable_Apps::main_phob2_pane( 0 ),
                    AknLayoutScalable_Apps::phob2_contact_card_pane( 1 ) ) );
    
    // header layout
    TAknWindowComponentLayout headerPaneLayout( 
        TAknWindowComponentLayout::Compose( 
            contactCardPaneLayout,
            AknLayoutScalable_Apps::phob2_cc_data_pane( option ) ) );
    
    TAknLayoutRect headerPaneLayoutRect;
    headerPaneLayoutRect.LayoutRect( Rect(), headerPaneLayout.LayoutLine() );
    TRect headerPaneRect( headerPaneLayoutRect.Rect() );
    iHeaderCtrl->SetRect( headerPaneRect );
    
    // list layout
    TAknWindowComponentLayout listPaneLayout( 
        TAknWindowComponentLayout::Compose( 
            contactCardPaneLayout,
            AknLayoutScalable_Apps::phob2_cc_listscroll_pane( option ) ) );
    
    TAknLayoutRect listPaneLayoutRect;
    listPaneLayoutRect.LayoutRect( Rect(), listPaneLayout.LayoutLine() );
    TRect listPaneRect( listPaneLayoutRect.Rect() );
    iListBox->SetRect( listPaneRect );
    
	delete iImageSelectionPopup;
	iImageSelectionPopup = NULL;
	
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::CheckExtensionFactoryL()
// ----------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::CheckExtensionFactoryL(TAny* aPtr)
    {
    CCCAppMyCardContainer* self = static_cast<CCCAppMyCardContainer*>( aPtr );
    self->DoCheckExtensionFactoryL();
    return 0;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::DoCheckExtensionFactoryL()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::DoCheckExtensionFactoryL()
    {
    CCCAExtensionFactory* extension = iFactoryExtensionNotifier->ExtensionFactory();
    // if extension is not null, extensionfactory plugins are available ->
    // show statuscontrol
    TBool visible = EFalse;
    if( extension )
        {
        if ( !iViewLauncher )
            {
            iViewLauncher = extension->CreateViewLauncherL();
            }
        visible = ETrue;
        }
    else
        {
        delete iViewLauncher;
        iViewLauncher = NULL;
        }
    
    CCCAppStatusControl* statusControl = iHeaderCtrl->StatusControl();
    if( statusControl )
        {
        statusControl->MakeVisible( visible );
        }
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::CountComponentControls()
// ----------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::CountComponentControls() const
    {
    return KSubComponentCount;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::ComponentControl()
// ----------------------------------------------------------------------------
//
CCoeControl* CCCAppMyCardContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            {
            return iHeaderCtrl;
            }
        case 1:
        	{
			return iListBox;
			}
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::MopSupplyObject()
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCCAppMyCardContainer::MopSupplyObject(TTypeUid aId)
    {
    // For skinning
    if( iBackground && aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBackground );
        }

    return CCoeControl::MopSupplyObject(aId);
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::OfferKeyEventL()
// ----------------------------------------------------------------------------
//
TKeyResponse CCCAppMyCardContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse resp = CCCAppViewPluginAknContainer::OfferKeyEventL( aKeyEvent, aType );
    if( iListBox && resp == EKeyWasNotConsumed )
        {
        resp = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    return resp;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::MyCardEventL()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::MyCardEventL( MMyCardObserver::TEvent aEvent )
    {
    if( aEvent == MMyCardObserver::EEventContactLoaded )
        {    
        // Set own contact name
        CCCAppMyCard& mycard = iPlugin.MyCard();
          
        SetNameForHeaderControlL();
        
        // start loading image
        if( !iImageLoader )
            {
            // lazy initialization
            iImageLoader = CCCAppMyCardImageLoader::NewL(
                mycard.ContactManager(), *this );
            }
        iImageLoader->LoadContactImageL( mycard.StoreContact() );
        }

    if( iPlugin.MyCard().IsContactLinkReady() && !iControlLink )
		{
        CCCAppStatusControl* statusControl = iHeaderCtrl->StatusControl();
		if(statusControl)
		    {
            statusControl->SetContactLinkL( iPlugin.MyCard().ContactLink() );
		    }
		iControlLink = ETrue;
		}
    }


// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::ThumbnailReady()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ThumbnailReady( CFbsBitmap* aThumbnail )
    {
    // takes ownership
    TRAPD( err, iHeaderCtrl->SetPortraitBitmapL( aThumbnail ) );
    if( err )
        {
        // TODO: how to handle? ignore? show note?
        }
    // Contact Image set. Set the flag.
    iMyCardImageSet = ETrue;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::ThumbnailLoadError()
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ThumbnailLoadError( TInt /*aError*/ )
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TRAPD( err, AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KMyCardIconDefaultFileName,
        EMbmPhonebook2eceQgn_prop_pb_thump_mycard,
        EMbmPhonebook2eceQgn_prop_pb_thump_mycard_mask ) );
    if( !err )
        {
        iHeaderCtrl->SetPortraitIcon( bitmap, mask ); // takes ownership
        }
    // Default contact image used.
    iMyCardImageSet = EFalse;
    }

// ----------------------------------------------------------------------------
// CCCAppMyCardContainer::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            // start the editor and pass tapped field index
            iPlugin.EditL( FocusedFieldIndex() );
            break;
            }
        default:
            {
            // ignored
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardContainer::Plugin
// ---------------------------------------------------------------------------
//
CCCAppMyCardPlugin& CCCAppMyCardContainer::Plugin()
    {
    return iPlugin;
    }


/************************ FUNCTIONS FROM UI CONTROL ************************/

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CCCAppMyCardContainer::ParentControl() const
	{
    // Contact info control has no parent control
    return NULL;
	}



// --------------------------------------------------------------------------
// CCCAppMyCardContainer::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::NumberOfContacts() const
	{
    return KNumberOfContacts;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CCCAppMyCardContainer::FocusedContactL() const
	{
    return static_cast<MVPbkBaseContact*>( &iPlugin.MyCard().StoreContact() );
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CCCAppMyCardContainer::FocusedViewContactL() const
	{
    return NULL;
	}


// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CCCAppMyCardContainer::FocusedStoreContact() const
	{
	return &iPlugin.MyCard().StoreContact();
    }

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetFocusedContactL(
    const MVPbkBaseContact& /*aContact*/ )
	{
    // do nothing
	}


// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetFocusedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/)
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetFocusedContactL(
    const MVPbkContactLink& /*aContactLink*/ )
	{
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::FocusedContactIndex() const
	{
    return KErrNotSupported;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetFocusedContactIndexL(TInt /*aIndex*/)
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::NumberOfContactFields() const
	{
    TInt cnt = KErrNotSupported;
    cnt =  iPlugin.MyCard().StoreContact().Fields().FieldCount();
    return cnt;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CCCAppMyCardContainer::FocusedField() const
	{
	TInt index = FocusedFieldIndex();
	if( index != KErrNotFound )
		{
		return &iPlugin.MyCard().StoreContact().Fields().FieldAt(index);
		}
    return NULL;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::FocusedFieldIndex() const
	{
	if( iModel )
		{
		return iModel->FieldIndex( iListBox->CurrentItemIndex() );
		}
	return KErrNotFound;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetFocusedFieldIndex( TInt /*aIndex*/ )
	{
	// do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CCCAppMyCardContainer::ContactsMarked() const
	{
    // This control does not support contacts marking
    return EFalse;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CCCAppMyCardContainer::SelectedContactsL() const
	{
    // No selection support in contact info control
    return NULL;
	}



// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
CCCAppMyCardContainer::SelectedContactsOrFocusedContactL() const
	{
    CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();
    const MVPbkBaseContact* focusedContact = FocusedContactL();

    if( focusedContact )
    	{
        MVPbkContactLink* link = focusedContact->CreateLinkLC();
        CleanupStack::Pop(); // link
        array->AppendL(link);
    	}
    CleanupStack::Pop(array);
    return array;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
CCCAppMyCardContainer::SelectedContactsIteratorL() const
	{
    // No selection support in contact info control
    return NULL;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
CCCAppMyCardContainer::SelectedContactStoresL() const
	{
    // This is not a contact store control
    return NULL;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ClearMarks
// --------------------------------------------------------------------------
//

void CCCAppMyCardContainer::ClearMarks()
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetSelectedContactL(
    TInt /*aIndex*/,
    TBool /*aSelected*/)
	{
    // Not supported in Contact Info
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetSelectedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/,
    TBool /*aSelected*/)
	{
    // Not supported
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetSelectedContactL(
    const MVPbkContactLink& /*aContactLink*/,
    TBool /*aSelected*/)
	{
    // Not supported
	}



// --------------------------------------------------------------------------
// CCCAppMyCardContainer::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::DynInitMenuPaneL(
    TInt /*aResourceId*/,
    CEikMenuPane* /*aMenuPane*/) const
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ProcessCommandL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ProcessCommandL(TInt /*aCommandId*/) const
	{
    // Nothing to do
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::UpdateAfterCommandExecution()
	{
    if (iCommand)
    	{
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
    	}
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardContainer::GetMenuFilteringFlagsL() const
	{
    const TInt fieldCount =
		 iPlugin.MyCard().StoreContact().Fields().FieldCount();
    // If negative will leave
    User::LeaveIfError( fieldCount );

    TInt ret = KPbk2MenuFilteringFlagsNone;
    if( fieldCount > KNoContacts )
    	{
        ret |= KPbk2ListContainsItems;
    	}
    return ret;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CCCAppMyCardContainer::ControlStateL() const
	{
    return NULL;
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::RestoreControlStateL(
    CPbk2ViewState* /*aState*/)
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CCCAppMyCardContainer::FindTextL()
	{
    // No find box in this control
    return KNullDesC;
	}



// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ResetFindL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ResetFindL()
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ShowThumbnail
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::ShowThumbnail()
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::HideThumbnail
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::HideThumbnail()
	{
    // Do nothing
	}


// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetBlank
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetBlank( TBool /*aBlank*/ )
	{
    // This control does not support blanking
	}


// --------------------------------------------------------------------------
// CCCAppMyCardContainer::RegisterCommand
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::RegisterCommand( MPbk2Command* aCommand )
	{
    iCommand = aCommand;
	}


// --------------------------------------------------------------------------
// CCCAppMyCardContainer::SetTextL
// --------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetTextL( const TDesC& /*aText*/ )
	{
    // Do nothing
	}

// --------------------------------------------------------------------------
// CCCAppMyCardContainer::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CCCAppMyCardContainer::ContactUiControlExtension( TUid /*aExtensionUid*/ )
	{
    return NULL;
	}

//-----------------------------------------------------------------------------
// void CCCAppMyCardContainer::StatusClickedL()
//-----------------------------------------------------------------------------
//
void CCCAppMyCardContainer::StatusClickedL()
	{
    if ( iViewLauncher )
		{
        iViewLauncher->LaunchViewL( MCCAViewLauncher::EMyCardView );
		}		
	}

//------------------------------------------------------------------------------
// void CCCAppMyCardContainer::SetNameForHeaderContorl()
//------------------------------------------------------------------------------
//
void CCCAppMyCardContainer::SetNameForHeaderControlL()
	{
    // Set own contact name
    CCCAppMyCard& mycard = iPlugin.MyCard();
    // Get the myCard store and the field set 
    MVPbkStoreContact& storeContact = mycard.StoreContact();
    MVPbkStoreContactFieldCollection& fields = storeContact.Fields();
                    
    TInt fieldCount = fields.FieldCount();                           
    
    TPtrC firstName( KNullDesC );
    TPtrC lastName( KNullDesC );
    
    // Check all the fields and store first and last name
    for ( TInt i = 0; i < fieldCount;  ++i )
    	{
		MVPbkStoreContactField& field = fields.FieldAt( i );	
		const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
		
		if ( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_FIRSTNAME )
			{
			MVPbkContactFieldData& contactField = field.FieldData();            
			firstName.Set( MVPbkContactFieldTextData::Cast(contactField).Text());
			}
		
		if ( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_LASTNAME )
			{
			MVPbkContactFieldData& contactField = field.FieldData();			            
			lastName.Set( MVPbkContactFieldTextData::Cast(contactField).Text());
			}		
    	}
    
    switch ( iNameOrder )
        {
        case KPbk2UiSpecNameOrderFirstNameLastName:
            {
            iHeaderCtrl->SetLabel1TextL( firstName );
            iHeaderCtrl->SetLabel2TextL( lastName );
            break;
            }
        case KPbk2UiSpecNameOrderLastNameFirstName: // FALL THROUGH
        default:
            {
            iHeaderCtrl->SetLabel2TextL( firstName );
            iHeaderCtrl->SetLabel1TextL( lastName );
            break;
            }
        }
    
	}

//------------------------------------------------------------------------------
// void CCCAppMyCardContainer::MyCardHeaderControlClickL()
//------------------------------------------------------------------------------
//
void CCCAppMyCardContainer::MyCardHeaderControlClickL( TPoint aPos )
{
    if ( !iImageSelectionPopup )
        {
        iImageSelectionPopup = CAknStylusPopUpMenu::NewL( &iPlugin, aPos );
        TInt resourceReaderId = R_MYCARD_CONTACT_IMAGE_STYLUS_MENU; 
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC( reader , resourceReaderId );
        iImageSelectionPopup->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader
        }
    
    // MyCard image has been set. Dim optiosn menu items accordingly
    if( iMyCardImageSet )
        {
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusViewImageCmd, EFalse);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusChangeImageCmd, EFalse);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusRemoveImageCmd, EFalse);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusAddImageCmd, ETrue);
        }
    // MyCard image has not been set
    else
        {
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusViewImageCmd, ETrue);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusChangeImageCmd, ETrue);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusRemoveImageCmd, ETrue);
        iImageSelectionPopup->SetItemDimmed(ECCAppMyCardCmdStylusAddImageCmd, EFalse);
        } 
          
    PosToScreenCoordinates( this, aPos );
   
    iImageSelectionPopup->SetPosition( aPos );
    iImageSelectionPopup->ShowMenu();   
}

// ---------------------------------------------------------------------------
// PosToScreenCoordinates
// ---------------------------------------------------------------------------
//
void CCCAppMyCardContainer::PosToScreenCoordinates( 
        CCoeControl* aControl, TPoint& aPos )
     {
     TPoint leftUpperPos = aControl->PositionRelativeToScreen();
     aPos += leftUpperPos;
     }

// End of File
