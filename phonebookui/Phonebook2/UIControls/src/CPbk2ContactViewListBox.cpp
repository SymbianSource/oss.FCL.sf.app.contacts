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
* Description:  Phonebook 2 contact view list box.
*
*/


// INCLUDE FILES
#include "CPbk2ContactViewListBox.h"

// Phonebook 2
#include "MPbk2ContactViewListBoxModel.h"
#include "CPbk2IconArray.h"
#include "MPbk2NamesListState.h"
#include "cpbk2contactviewdoublelistboxmodel.h"
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2ContactUiControlExtension.h>
#include "CPbk2ListboxModelCmdDecorator.h"
#include <CPbk2ContactIconsUtils.h>
#include <Pbk2UIControls.hrh>

// Virtual Phonebook
#include <CVPbkContactManager.h>

// System includes
#include <barsread.h>
#include <eikclbd.h>

#include <aknappui.h>
#include <aknViewAppUi.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <featmgr.h>

#include <skinlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include "cpbk2contactviewcustomlistboxitemdrawer.h"
#include "cpbk2contactviewcustomlistboxdata.h"
#include "CPbk2NamesListControl.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL FUNCTIONS

/**
 * Returns icon array from given list box.
 *
 * @param aListBox  List box.
 * @return  Icon array in Phonebook 2 format.
 */
inline CPbk2IconArray* IconArray( CEikColumnListBox& aListBox )
    {
    return static_cast<CPbk2IconArray*>(
        aListBox.ItemDrawer()->ColumnData()->IconArray() );
    }

/**
 * Sets given icon array as given listbox's icon array.
 *
 * @param aListBox      List box.
 * @param aIconArray    Icon array.
 */
inline void SetIconArray( CEikColumnListBox& aListBox,
        CPbk2IconArray* aIconArray )
    {
    aListBox.ItemDrawer()->ColumnData()->SetIconArray( aIconArray );
    }

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_CreateListBoxModelL = 1
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2ContactViewListBox");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG
} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::CPbk2ContactViewListBox
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBox::CPbk2ContactViewListBox
            (   CCoeControl& aContainer,
                MPbk2ContactUiControlExtension* aUiExtension,
                CPbk2PredictiveSearchFilter &aSearchFilter ) :
                iContainer(aContainer), 
                iChangedIndexes( 1 ), // allocation granularity
                iUiExtension( aUiExtension ),
                iSearchFilter( aSearchFilter )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::~CPbk2ContactViewListBox
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBox::~CPbk2ContactViewListBox()
    {
    delete iListboxModelDecorator;
    delete iModel;
    FeatureManager::UnInitializeLib();
    CCoeEnv::Static()->RemoveForegroundObserver( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::ConstructL(
        CCoeControl& aContainer, TResourceReader& aResourceReader,
        const CVPbkContactManager& aManager, MVPbkContactViewBase& aView,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2StorePropertyArray& aStoreProperties )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBox::ConstructL"));

    FeatureManager::InitializeLibL();

    // Preallocate space for the one required integer
    iChangedIndexes.AppendL( KErrNotFound );

    // Read flags from resources
    iResourceData.iFlags = aResourceReader.ReadUint32();    // flags

    SetContainerWindowL( aContainer );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBox::ConstructL about to resource construct"));

    // Let Avkon read it's share of the resources
    ConstructFromResourceL( aResourceReader );

    // Remove the platform default "No data" text
    View()->SetListEmptyTextL( KNullDesC );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBox::ConstructL about to create icon array"));

    CPbk2IconArray* iconArray =
        CPbk2ContactIconsUtils::CreateListboxIconArrayL(
                aResourceReader.ReadInt32());

    SetIconArray( *this, iconArray );

    iResourceData.iEmptyIconId =
        TPbk2IconId( aResourceReader ); // empty icon
    iResourceData.iDefaultIconId =
        TPbk2IconId( aResourceReader ); // default icon

    CreateScrollBarFrameL( ETrue );
    ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    MakeVisible( EFalse );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBox::ConstructL about to create model"));

    CreateListBoxModelL( aManager, aView,
        aNameFormatter, aStoreProperties );
    
    CCoeEnv::Static()->AddForegroundObserverL( *this );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBox::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBox* CPbk2ContactViewListBox::NewL(
        CCoeControl& aContainer, TResourceReader& aResourceReader,
        const CVPbkContactManager& aManager, MVPbkContactViewBase& aView,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2StorePropertyArray& aStoreProperties,
        MPbk2ContactUiControlExtension* aUiExtension,
        CPbk2PredictiveSearchFilter &aSearchFilter )
    {
    CPbk2ContactViewListBox* self =
        new ( ELeave ) CPbk2ContactViewListBox( aContainer, aUiExtension, aSearchFilter );
    CleanupStack::PushL( self );
    self->ConstructL( aContainer, aResourceReader, aManager,
        aView, aNameFormatter, aStoreProperties );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::NumberOfItems
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewListBox::NumberOfItems() const
    {
    TInt result = 0;
    if ( iListboxModelDecorator )
        {
        result = iListboxModelDecorator->MdcaCount();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::ContactsMarked() const
    {
    const CListBoxView::CSelectionIndexArray* selections =
        View()->SelectionIndexes();
    TBool notEmpty = selections && selections->Count() > 0;
    TBool containsContacts = EFalse;
    for (TInt n = 0; notEmpty && n < selections->Count() && !containsContacts; ++n )
        {
        const TInt index = selections->At( n );
        // The item is either a contact or a command.
        containsContacts = IsContact( index );
        }
    return containsContacts;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandleEventL
        ( TInt aEvent, TInt aIndex )
    {
    switch ( aEvent )
        {
        case MPbk2NamesListState::EItemAdded:
            {
            iChangedIndexes[0] = aIndex;
            HandleItemAdditionL( iChangedIndexes );
            break;
            }
        case MPbk2NamesListState::EItemRemoved:
            {
            // Deselect item
            CListBoxView& listBoxView = *View();
            listBoxView.SetDisableRedraw( ETrue );
            listBoxView.DeselectItem( aIndex );
            listBoxView.SetDisableRedraw( EFalse );

            TInt focusIndex = CurrentItemIndex();
            TInt topIndex = TopItemIndex();
            iChangedIndexes[0] = aIndex;
            HandleItemRemovalL( iChangedIndexes );

            // The deduction of 1 is for adapting with the different bases
            const TInt lastIndex = Model()->NumberOfItems() - 1;
            if ( lastIndex == KErrNotFound )
                {
                Reset();
                }
            else
                {
                if ( aIndex < focusIndex )
                    {
                    // If item was removed above focused item, maintain
                    // focus by moving it up by one
                    --focusIndex;
                    }
                if ( focusIndex > lastIndex || focusIndex < 0 )
                    {
                    focusIndex = lastIndex;
                    }
                const TInt numVisibleItems =
                    View()->NumberOfItemsThatFitInRect(
                        View()->ViewRect() );
                if ( topIndex + numVisibleItems > lastIndex )
                    {
                    topIndex = Max( lastIndex - numVisibleItems + 1, 0 );
                    }
                if ( topIndex >= 0 )
                    {
                    SetTopItemIndex( topIndex );
                    }
                if ( focusIndex >= 0 )
                    {
                    SetCurrentItemIndex( focusIndex );
                    }
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::FixTopItemIndex
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::FixTopItemIndex()
    {
    TInt index = TopItemIndex();
    const TInt height = View()->NumberOfItemsThatFitInRect(
        View()->ViewRect() );
    const TInt numItems = Model()->NumberOfItems();
    if ( index + height > numItems )
        {
        index += numItems - ( height + index );
        if ( index < 0 )
            {
            index = 0;
            }
        if ( index != TopItemIndex() )
            {
            SetTopItemIndex( index );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::CheckFocusIndex
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::CheckFocusIndex()
    {
    if ( CurrentItemIndex() == KErrNotFound )
        {
        SetCurrentItemIndex( 0 );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandlePointerEventL
        ( const TPointerEvent& aPointerEvent )
    {
    TInt oldIndex = CurrentItemIndex();

    // Base class handles the pointer event
    CAknSingleGraphicStyleListBox::HandlePointerEventL( aPointerEvent );

    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            TInt pointedItemIndex;
            iFocusableContactPointed = View()->XYPosToItemIndex(
                aPointerEvent.iPosition, pointedItemIndex );
            
            //no prior focus needed
            iFocusedContactPointed = iFocusableContactPointed;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::Flags
// --------------------------------------------------------------------------
//
TUint CPbk2ContactViewListBox::Flags() const
    {
    return iResourceData.iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::SetScrollEventObserver
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::SetScrollEventObserver(MEikScrollBarObserver* aObserver)
    {
    iScrollEventObserver = aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::IsSelectionListBox
// This function is non const because IsMultiselection is non const.
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::IsSelectionListBox()
    {
    // Try firs the CEikListBox method for multiselection
    TBool result = IsMultiselection();

    if ( !result )
        {
        // Then try other flags
        TUint32 markableFlags =
            EAknListBoxStylusMarkableList |
            EAknListBoxStylusMultiselectionList |
            EAknListBoxMarkableList |
            EAknListBoxMultiselectionList;

        // Compare CEikListBox flags to Avkon markable list flags
        result = iListBoxFlags & markableFlags;
        }
    return result;
    }

void CPbk2ContactViewListBox::SetListCommands(
    const RPointerArray<MPbk2UiControlCmdItem>* aCommands )
    {
    iListboxModelDecorator->SetListCommands( aCommands );
    iColumnData->SetListCommands( aCommands );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::ClipFromBeginning
        ( TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber )
    {
    return AknTextUtils::ClipToFit( aBuffer,
        AknTextUtils::EClipFromBeginning, this,  aItemIndex,
        aSubCellNumber );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::FocusedItemPointed()
    {
    return iFocusedContactPointed;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::FocusableItemPointed()
    {
    return iFocusableContactPointed;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::SearchFieldPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandleScrollEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {   
    // Handle showing of popupcharacter when user scrolls list using using scroll bar
    TBool prevState = iShowPopupChar;

    iShowPopupChar = ( aEventType == EEikScrollThumbDragHoriz || aEventType == EEikScrollThumbDragVert );
    
    if( prevState != iShowPopupChar )
        {
        DrawDeferred();
        }

    CAknSingleGraphicStyleListBox::HandleScrollEventL( aScrollBar, aEventType );
    if( iScrollEventObserver )
        {
        iScrollEventObserver->HandleScrollEventL( aScrollBar, aEventType );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::CreateListBoxModelL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::CreateListBoxModelL
        ( const CVPbkContactManager& aManager, MVPbkContactViewBase& aView,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2StorePropertyArray& aStoreProperties )
    {
    __ASSERT_DEBUG( !iModel && !iListboxModelDecorator,
        Panic( EPanicPreCond_CreateListBoxModelL ) );

    // Set up listbox model
    CPbk2ContactViewListBoxModel::TParams params;
    params.iContactManager = &aManager;
    params.iView = &aView;
    params.iStoreProperties = &aStoreProperties;
    params.iNameFormatter = &aNameFormatter;
    params.iIconArray = IconArray( *this );
    params.iEmptyId = iResourceData.iEmptyIconId;
    params.iDefaultId = iResourceData.iDefaultIconId;
    params.iClipListBoxText = this;
    params.iUiExtension = iUiExtension;
    params.iMultiselection = IsMultiselection();
    CPbk2ContactViewListBoxModel* model = DoCreateModelL( params );
    model->SetUnnamedText( iResourceData.iUnnamedText );
    iModel = model;

    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
        {
        iColumnData->SetDataModel( iModel );
        }
    // Wrap the original model.
    iListboxModelDecorator = DoCreateDecoratorL( 
    		*IconArray( *this ),
			iResourceData.iEmptyIconId,
			iResourceData.iDefaultIconId );
    
    
    iListboxModelDecorator->SetDecoratedModel( *iModel );
    Model()->SetItemTextArray( iListboxModelDecorator );
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::DoCreateModelL
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBoxModel* CPbk2ContactViewListBox::DoCreateModelL(
                CPbk2ContactViewListBoxModel::TParams params )
    {
    return CPbk2ContactViewListBoxModel::NewL( params );
    }


// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::DoCreateDecoratorL
// --------------------------------------------------------------------------
//
CPbk2ListboxModelCmdDecorator* CPbk2ContactViewListBox::DoCreateDecoratorL( 
        		const CPbk2IconArray& aIconArray,
        		TPbk2IconId aEmptyIconId,
        		TPbk2IconId aDefaultIconId )
	{
	return CPbk2ListboxModelCmdDecorator::NewL(
				aIconArray,
				aEmptyIconId,
				aDefaultIconId );
	}


// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::IsContact
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBox::IsContact( TInt aIndex ) const
    {
    // The command items are always at the top of the list.
    return aIndex >= iListboxModelDecorator->NumberOfEnabledCommands();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::Draw
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::Draw( const TRect& aRect ) const
    {
    CAknSingleGraphicStyleListBox::Draw(aRect);
    HandlePopupCharacter(NULL, Rect());
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::CreateItemDrawerL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::CreateItemDrawerL()
    {
    CPbk2ContactViewCustomListBoxData* columnData =
                    CPbk2ContactViewCustomListBoxData::NewL( iSearchFilter );

    CleanupStack::PushL( columnData );

    iItemDrawer=new(ELeave) CPbk2ContactViewCustomListBoxItemDrawer(
            static_cast<MTextListBoxModel*>(Model()), iCoeEnv->NormalFont(),
            columnData);
    CleanupStack::Pop( columnData );

    //Ownership has been transferred to iItemDrawer
    iColumnData = columnData;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandleGainingForeground
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandleGainingForeground()
    {
	DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandleLosingForeground
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandleLosingForeground()
    {
    if ( iShowPopupChar )
        {
        //Simulate an EButton1Up event to force a DrawDeferred() calling
        //in case of the scrollbar¡¯s at the background that can¡¯t receive the EButton1Up event.
        TPointerEvent PointEvent;
        PointEvent.iType = TPointerEvent::EButton1Up ;
        TRAP_IGNORE( ScrollBarFrame()->VerticalScrollBar()->HandlePointerEventL( PointEvent ) );
        DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBox::HandlePopupCharacter
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBox::HandlePopupCharacter( CWindowGc* aGc, const TRect& aRectOfListBoxItem ) const
    {
    CPbk2NamesListControl& listControl = static_cast<CPbk2NamesListControl&>(iContainer);
    TInt itemIndex = TopItemIndex() - listControl.CommandItemCount();
   
    // itemIndex >= 0 filter out command item
    if ( itemIndex >= 0 && iShowPopupChar )
        {
        TAknLayoutRect layout;
        layout.LayoutRect(Rect(), AknLayoutScalable_Apps::popup_navstr_preview_pane(0));
        
        if (!layout.Rect().Intersects(aRectOfListBoxItem))
            {
            // If the rect of PopupCharacter not overlapped
            // with aRectOfListBoxItem, do nothing.
            return;
            }

        // Handle showing of popupcharacter when user scrolls list using using scroll bar
        CWindowGc* gc = aGc;
        if (!gc)
            {
            gc = &SystemGc();
            }

        TRgb normal;
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                normal,
                KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6 );

        TAknLayoutRect cornerRect;
        // skinned draw uses submenu popup window skin (skinned border)
        cornerRect.LayoutRect(
            layout.Rect(),
            SkinLayout::Submenu_skin_placing_Line_2() );

        TRect innerRect( layout.Rect() );
        innerRect.Shrink( cornerRect.Rect().Width(), cornerRect.Rect().Height() );

        if ( !AknsDrawUtils::DrawFrame(
                AknsUtils::SkinInstance(),
                *gc,
                layout.Rect(),
                innerRect,
                KAknsIIDQsnFrPopupSub,
                KAknsIIDQsnFrPopupCenterSubmenu ) )
            {
            // skinned border failed -> black border
            gc->SetPenStyle( CGraphicsContext::ESolidPen );
            gc->SetBrushColor( KRgbBlack );
            gc->DrawRect( layout.Rect() );
            }

        TAknLayoutText textLayout;
        textLayout.LayoutText(layout.Rect(), AknLayoutScalable_Apps::popup_navstr_preview_pane_t1(0).LayoutLine());

        TPtrC desc(Model()->ItemTextArray()->MdcaPoint(View()->TopItemIndex()));

        HBufC* buf = desc.Mid(desc.Find(_L("\t")) + 1, 1).AllocL();
        TPtr textPtr = buf->Des();
        textPtr.UpperCase();
        
        textLayout.DrawText(*gc, textPtr, ETrue, normal );
        
        delete buf;
        }
    }
// End of File
