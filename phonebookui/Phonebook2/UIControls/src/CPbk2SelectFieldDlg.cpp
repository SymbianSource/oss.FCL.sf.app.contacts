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
* Description:  Phonebook 2 field selection dialog.
*
*/


#include "CPbk2SelectFieldDlg.h"

// Phonebook 2
#include "MPbk2ControlKeyObserver.h"
#include "MPbk2ClipListBoxText.h"
#include <CPbk2IconArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2PresenceIconInfo.h>
#include "CPbk2FieldAnalyzer.h"
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactLink.h>

// System includes
#include <avkon.rsg>
#include <aknPopup.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <calslbs.h>
#include <barsread.h>
#include <MPbk2AppUi.h>
#include "CPbk2IconInfo.h"
#include <CPbk2ServiceManager.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ApplicationServices2.h>
#include <aknlayoutscalable_avkon.cdl.h>

/// Unnamed namespace for local definitions
namespace {

//uid for presence icons
const TInt32 KPbk2PrecenceIconsId = 0x2001E63D;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_AttemptExitL,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSelectFieldDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


/**
 * Creates and returns an icon array with Phonebook 2
 * default and extension icons.
 *
 * @param aPresenceIconsArray Presence icons array
 * @return  An array filled with icons.
 */
CPbk2IconArray* CreateListBoxIconArrayL(
    const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray,
    RArray<CPbk2FieldListBoxModel::TPresenceIconPosInIconArray>&
        aPresenceIconsPos )
    {
    // Create icon array
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader,
        R_PBK2_FIELDTYPE_ICONS );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL( reader );
    CleanupStack::PushL( iconArray );
    
    //Calculate preferred size for xsp service icons 
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
      
    // Add xsp service icons
    CPbk2ApplicationServices* appServices =
        CPbk2ApplicationServices::InstanceLC();
    MPbk2ApplicationServices2* servicesExtension = 
        reinterpret_cast<MPbk2ApplicationServices2*>
            ( appServices->MPbk2ApplicationServicesExtension(
                    KMPbk2ApplicationServicesExtension2Uid ) );
    CleanupStack::PopAndDestroy(); // appServices
    CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        if ( service.iBitmapId && service.iBitmap )
            {
        	AknIconUtils::SetSize(
        	           service.iBitmap,
        	           size );
        	AknIconUtils::SetSize(
        	           service.iMask,
        	           size );
            
            TPbk2IconId id = TPbk2IconId( uid, services[i].iBitmapId );
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
                id, services[i].iBitmap, services[i].iMask, size );
            iconArray->AppendIconL(info);
            CleanupStack::Pop(info);
            }
        }    

    // Add icons from UI extensions
    CPbk2UIExtensionManager* extManager = CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    extManager->IconSupportL().AppendExtensionIconsL( *iconArray );
    CleanupStack::PopAndDestroy(); // extManager
    
    // add presence icons if exist
    if ( aPresenceIconsArray != NULL )
        {
        TUid presenceIconsUid;
        presenceIconsUid.iUid = KPbk2PrecenceIconsId;
        TInt count = aPresenceIconsArray->Count();
        for ( TInt j = 0; j < count; j++ )
            {
            TPbk2IconId presenceIconId = TPbk2IconId( presenceIconsUid, j );
            CPbk2IconInfo* presenceIconInfo = CPbk2IconInfo::NewLC(
                presenceIconId,
                aPresenceIconsArray->operator[](j)->IconBitmap(),
                aPresenceIconsArray->operator[](j)->IconBitmapMask(),
                size );
            iconArray->AppendIconL( presenceIconInfo );
            CleanupStack::Pop( presenceIconInfo );
            
            // save presence icon position
            CPbk2FieldListBoxModel::TPresenceIconPosInIconArray presenceIconPos( 
                aPresenceIconsArray->operator[](j)->ServiceName() );
            presenceIconPos.iIconId = presenceIconId;
            aPresenceIconsPos.AppendL( presenceIconPos );
            }
        }
    
    CleanupStack::Pop( iconArray );
    CleanupStack::PopAndDestroy(); // reader

    return iconArray;
    }

} /// namespace


/**
 * Specialized popup list class.
 */
NONSHARABLE_CLASS( CPbk2SelectFieldDlg::CPopupList ) :
        public CAknPopupList
    {
    public: // Interface

        /**
         * Like CAknPopupList::NewL.
         * @see CAknPopupList::NewL
         */
        static CPopupList* NewL(
                CEikListBox* aListBox,
                TInt aCbaResource,
                AknPopupLayouts::TAknPopupLayouts aType =
                    AknPopupLayouts::EMenuWindow );

        /**
         * Sets the listbox item to focus initially.
         * By default focus is on the first item.
         *
         * @param aFocusIndex  Index of the listbox item to focus initially.
         */
        inline void SetInitialFocus(
                TInt aFocusIndex );

        /**
         * Like CAknPopupList::AttemptExitL.
         * @see CAknPopupList::AttemptExitL
         */
        inline void AttemptExitL(
                TBool aAccept );

    private: // From CAknPopupList
        void SetupWindowLayout(
                AknPopupLayouts::TAknPopupLayouts aType );
        
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,TEventCode aType );

    private: // Implementation
        inline CPopupList();

    private: // Data
        /// Own: Focus index
        TInt iFocusIndex;
    };

/**
 * Specialized listbox class.
 */
NONSHARABLE_CLASS( CPbk2SelectFieldDlg::CListBox ) :
        public CAknDoubleGraphicPopupMenuStyleListBox,
        public MPbk2ClipListBoxText
    {
    public: // Interface

        /**
         * Constructor.
         */
        inline CListBox();

        /**
         * Sets the observer.
         *
         * @param aKeyObserver      The observer to set.
         */
        inline void SetObserver(
                MPbk2ControlKeyObserver* aKeyObserver );

        /**
         * Returns CONE environment.
         *
         * @return  CONE environment.
         */
        inline CCoeEnv* CoeEnv();

    private: // From CAknDoubleGraphicPopupMenuStyleListBox
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

    private: // From MPbk2ClipListBoxText
        TBool ClipFromBeginning(
            TDes& aBuffer,
            TInt aItemIndex,
            TInt aSubCellNumber );

    private: // Data
        /// Ref: Observer
        MPbk2ControlKeyObserver* iKeyObserver;
    };


// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::CPopupList
// --------------------------------------------------------------------------
//
inline CPbk2SelectFieldDlg::CPopupList::CPopupList() :
        iFocusIndex( KErrNotFound )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectFieldDlg::CPopupList* CPbk2SelectFieldDlg::CPopupList::NewL
        ( CEikListBox* aListBox,  TInt aCbaResource,
          AknPopupLayouts::TAknPopupLayouts aType )
    {
    CPopupList* self = new ( ELeave ) CPopupList();
    CleanupStack::PushL( self );
    self->ConstructL( aListBox, aCbaResource, aType );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::SetInitialFocus
// --------------------------------------------------------------------------
//
inline void CPbk2SelectFieldDlg::CPopupList::SetInitialFocus
        ( TInt aFocusIndex )
    {
    iFocusIndex = aFocusIndex;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CPopupList::AttemptExitL
// --------------------------------------------------------------------------
//
inline void CPbk2SelectFieldDlg::CPopupList::AttemptExitL( TBool aAccept )
    {
    CAknPopupList::AttemptExitL( aAccept );
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::SetupWindowLayout
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::CPopupList::SetupWindowLayout
        ( AknPopupLayouts::TAknPopupLayouts aType )
    {
    // Call base class
    CAknPopupList::SetupWindowLayout( aType );
    // Set initial focus
    if ( iFocusIndex >= 0 )
        {
        ListBox()->SetCurrentItemIndex( iFocusIndex );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2SelectFieldDlg::CPopupList::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // The focus can display after clicking on navigation key and MSK key.
    // Then, both LSK and RSK should be visible.
    // Check whether LSK is hidden, if yes, make it visible
    if ( !ButtonGroupContainer()->IsCommandVisible( EAknSoftkeySelect ) )
    	{
        // Whether navigation key or MSK key is pressed.
    	if ( aKeyEvent.iCode == EKeyUpArrow  
          || aKeyEvent.iCode == EKeyDownArrow 
          || aKeyEvent.iCode == EKeyDevice3 )
            {
            ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeySelect, ETrue );
            }
    	}
    TKeyResponse response = ListBox()->OfferKeyEventL( aKeyEvent, aType );
    return response;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::CListBox
// --------------------------------------------------------------------------
//
inline CPbk2SelectFieldDlg::CListBox::CListBox()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::SetObserver
// --------------------------------------------------------------------------
//
inline void CPbk2SelectFieldDlg::CListBox::SetObserver
        ( MPbk2ControlKeyObserver* aKeyObserver )
    {
    iKeyObserver = aKeyObserver;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::CoeEnv
// --------------------------------------------------------------------------
//
inline CCoeEnv* CPbk2SelectFieldDlg::CListBox::CoeEnv()
    {
    return iCoeEnv;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2SelectFieldDlg::CListBox::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse ret = EKeyWasNotConsumed;

    if ( iKeyObserver &&
         iKeyObserver->Pbk2ControlKeyEventL( aKeyEvent,aType )
            == EKeyWasConsumed )
        {
        ret = EKeyWasConsumed;
        }
    else
        {
        ret = CAknDoubleGraphicPopupMenuStyleListBox::OfferKeyEventL
            ( aKeyEvent, aType );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CListBox::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CPbk2SelectFieldDlg::CListBox::ClipFromBeginning
        ( TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber )
    {
    return AknTextUtils::ClipToFit
        ( aBuffer, AknTextUtils::EClipFromBeginning, this,
          aItemIndex, aSubCellNumber );
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CPbk2SelectFieldDlg
// --------------------------------------------------------------------------
//
CPbk2SelectFieldDlg::CPbk2SelectFieldDlg()
    {
    __ASSERT_DEBUG(
        !iListBox && !iPopupList && !iKeyObserver && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::~CPbk2SelectFieldDlg
// --------------------------------------------------------------------------
//
CPbk2SelectFieldDlg::~CPbk2SelectFieldDlg()
    {
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    if ( iPopupList )
        {
        iPopupList->CancelPopup();
        }
    
    delete iFieldAnalyzer;
    delete iListBox;
    iFieldCollectionArray.ResetAndDestroy();
    iPresenceIconsPos.Reset();
    
    delete iContactLink;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::StoreReady( 
        MVPbkContactStore& /*aContactStore*/ )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::HandleStoreEventL( 
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent aStoreEvent )
    {
    if ( iContactLink && 
            aStoreEvent.iContactLink &&
            aStoreEvent.iContactLink->IsSame( *iContactLink ) )
        {
        switch( aStoreEvent.iEventType )
            {
            case TVPbkContactStoreEvent::EContactDeleted:
                {
                iPopupList->CancelPopup();
                break;
                }
                
            default:
                break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::ExecuteLCD
// --------------------------------------------------------------------------
//
const MVPbkStoreContactField* CPbk2SelectFieldDlg::ExecuteLCD
        ( MVPbkStoreContactFieldCollection& aFieldCollection,
          RPointerArray<CVPbkFieldFilter>& aFieldCollectionArray,
          const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray,
          const CVPbkContactManager& aContactManager,
          const MPbk2FieldPropertyArray& aFieldProperties,
          TInt aCbaResourceId, 
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod,
          const TDesC& aHeading /*=KNullDesC*/,
          TInt aFocusIndex /*=KErrNotFound*/ )
    {
    __ASSERT_DEBUG(!iListBox && !iPopupList, Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL( this );
    TBool thisDestroyed = EFalse;
    // Destructor will set thisDestroyed to ETrue if this object is destroyed
    iDestroyedPtr = &thisDestroyed;

    CPbk2PresentationContactFieldCollection* fields =
        CPbk2PresentationContactFieldCollection::NewL(
            aFieldProperties,
            aFieldCollection,
            aFieldCollection.ParentStoreContact() );
    CleanupStack::PushL( fields );

    if ( aFocusIndex != KErrNotFound )
        {
        // The focus index is from store contact field order and
        // we have to map it to presentation contact field order
        MVPbkStoreContactField* initiallyFocusedField =
            aFieldCollection.FieldAtLC( aFocusIndex );
        for ( TInt i( 0 ); i < fields->FieldCount(); ++i )
            {
            if ( fields->FieldAt( i ).IsSame( *initiallyFocusedField ) )
                {
                // Fix the focus index to match with
                // presentation contact fields
                aFocusIndex = i;
                break;
                }
            }
        CleanupStack::PopAndDestroy(); // initiallyFocusedField
        }

    // Create presentation fields for additional contacts
    if ( aFieldCollectionArray.Count() > 0 )
        {
        TInt count = aFieldCollectionArray.Count();
        for( TInt i = 0; i < count; i++ )
            {
            CPbk2PresentationContactFieldCollection* xspFields =
                CPbk2PresentationContactFieldCollection::NewL(
                    aFieldProperties,
                    *aFieldCollectionArray[i],
                    aFieldCollectionArray[i]->ParentStoreContact() );
            CleanupStack::PushL( xspFields );

            iFieldCollectionArray.AppendL( xspFields );
            CleanupStack::Pop(); //xspFields
            }
        }

    // Create list box
    CreateListBoxL( *fields, aPresenceIconsArray, aContactManager, 
        aFieldProperties, aCbaResourceId, aHeading, aFocusIndex, aCommMethod );
                    
    // Show the list query, if there are more than 1 field in the listbox
    TInt dlgResult = 0;
    if ( iListBoxModel->MdcaCount() > 1 )
        {
        MVPbkBaseContact& contact = fields->ParentContact();
        MVPbkContactLink* link = contact.CreateLinkLC();
        MVPbkContactStore& store = link->ContactStore();
        store.OpenL( *this );
        delete iContactLink;
        iContactLink = link;
        CleanupStack::Pop(); // link
        
        TRAPD( err, dlgResult = iPopupList->ExecuteLD() );
        store.Close( *this );
        
        if ( !thisDestroyed )
            {
            iPopupList = NULL;
            }
        if ( err != KErrNone )
            {
            User::Leave( err );
            }
        }
   else
       {
       dlgResult = 1; // simulate user's selection
       delete iPopupList;
       iPopupList = NULL;
       }

    // Get the return value
    MVPbkStoreContactField* result = NULL;
    if ( thisDestroyed )
        {
        // This object has been destroyed
        CleanupStack::PopAndDestroy( fields );
        CleanupStack::Pop();  // this
        }
    else
        {
        if ( dlgResult )
            {
            TInt index = iListBoxModel->GetModelIndex(
                iListBox->CurrentItemIndex() );
            if ( index < fields->FieldCount() )
                {
                // field from the main contact
                result = fields->FieldAt( index ).CloneLC();
                CleanupStack::Pop();
                }
            else if ( iFieldCollectionArray.Count() > 0 )
                {
                // field from the addiotnal contact
                TInt offset = fields->FieldCount();
                for ( TInt i = 0; i < iFieldCollectionArray.Count(); i++ )
                    {
                    TInt xspFieldCount = iFieldCollectionArray.operator[](i)->
                        FieldCount();
                    if ( index < offset + xspFieldCount )
                        {
                        // selected field belongs to iFieldCollectionArray[i] contact
                        result = iFieldCollectionArray.operator[](i)->FieldAt(
                            index - offset ).CloneLC();
                        CleanupStack::Pop();
                        break;
                        }
                    else
                        {
                        offset += xspFieldCount;
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( fields );
        CleanupStack::PopAndDestroy(); // this
        }

    CleanupDeletePushL( result );
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::SetObserver
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::SetObserver
        ( MPbk2ControlKeyObserver* aKeyObserver )
    {
    iKeyObserver = aKeyObserver;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::AttemptExitL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::AttemptExitL( TBool aAccept )
    {
    __ASSERT_DEBUG( iPopupList, Panic( EPanicPreCond_AttemptExitL ) );
    iPopupList->MakeVisible( EFalse );
    iPopupList->AttemptExitL( aAccept );
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::ResetWhenDestroyed
        ( CPbk2SelectFieldDlg** aSelfPtr )
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this,
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldDlg::CreateListBoxL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldDlg::CreateListBoxL
        ( const CPbk2PresentationContactFieldCollection& aFieldCollection,
          const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray,
          const CVPbkContactManager& aContactManager,
          const MPbk2FieldPropertyArray& aFieldProperties,
          TInt aCbaResourceId, const TDesC& aHeading,
          TInt aFocusIndex,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod)
    {
    // Create a list box
    iListBox = new(ELeave) CListBox;

    // Create a popup list
    CPopupList* popupList = CPopupList::NewL(
        iListBox,
        aCbaResourceId,
        AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
    CleanupStack::PushL( popupList );

    // Init list box
    iListBox->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    iListBox->SetObserver( iKeyObserver );

    // Set title of popuplist
    if ( aHeading.Length() > 0 )
        {
        popupList->SetTitleL( aHeading );
        }

    // There is no focus at the initial phase, hence, LSK is not necessary.
    popupList->ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeySelect, EFalse );

    // Create icon array
    CPbk2IconArray* iconArray = CreateListBoxIconArrayL( aPresenceIconsArray,
        iPresenceIconsPos );
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

    // Create list box model
    HBufC* timeFormat =
        iListBox->CoeEnv()->AllocReadResourceLC( R_QTN_DATE_USUAL );

    iFieldAnalyzer = CPbk2FieldAnalyzer::NewL( const_cast<CVPbkContactManager&>( aContactManager ) );
    
    CPbk2FieldListBoxModel::TParams params( 
        aFieldCollection,
        aContactManager,
        aFieldProperties,
        *timeFormat,
        *iconArray,
        iFieldAnalyzer,
        NULL,
        ETrue,
		aCommMethod );
    CPbk2FieldListBoxModel* listBoxModel = CPbk2FieldListBoxModel::NewL(
        params, &iFieldCollectionArray.Array(), &iPresenceIconsPos.Array() );
    CleanupStack::PushL( listBoxModel );
    listBoxModel->FormatFieldsL();
    CleanupStack::Pop( listBoxModel );
    CleanupStack::PopAndDestroy();  // timeFormat
    listBoxModel->SetClipper( *iListBox );
    iListBox->Model()->SetItemTextArray( listBoxModel );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->Reset();

    if ( aFocusIndex >= 0 && aFocusIndex <
            iListBox->Model()->NumberOfItems() )
        {
        popupList->SetInitialFocus( aFocusIndex );
        }

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    CleanupStack::Pop();  // listBox
    iPopupList = popupList;
    
    iListBoxModel = listBoxModel;
    }

// End of File
