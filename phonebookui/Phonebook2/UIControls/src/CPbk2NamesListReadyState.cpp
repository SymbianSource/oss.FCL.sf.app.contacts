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
* Description:  Phonebook 2 names list ready state.
*
*/


// INCLUDE FILES
#include "CPbk2NamesListReadyState.h"

// Phonebook 2
#include "CPbk2AdaptiveSearchGridFiller.h"
#include "CPbk2ThumbnailLoader.h"
#include "CPbk2ContactViewListBox.h"
#include "CPbk2ContactViewIterator.h"
#include "MPbk2UiControlEventSender.h"
#include "CPbk2FindDelay.h"
#include "CPbk2NamesListControl.h"
#include "CPbk2FindStringSplitter.h"
#include <CPbk2ViewState.h>
#include <MPbk2ControlObserver.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include "TPbk2FindTextUtil.h"
#include <CPbk2AppViewBase.h>

#include "PmuCommands.hrh"  // For accessing EPmuCmdCascadingBackup
#include "Pbk2USimUI.hrh"   // For accessing EPsu2CmdCascadingSimMemory
#include <Pbk2InternalUID.h>
#include <pbk2doublelistboxcmditemextension.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactBookmark.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactFindPolicy.h>
#include <CVPbkContactManager.h>

// System includes
#include <akntoolbar.h>
#include <AknsDrawUtils.h>
#include <barsread.h>
#include <aknsfld.h>
#include <badesca.h>
#include <aknViewAppUi.h>
#include <aknappui.h>
#include <eikmenub.h>
#include <eikdialg.h>

// Debugging headers
#include <Pbk2Debug.h>
#include "CPbk2PredictiveSearchFilter.h"
#include "cpbk2predictiveviewstack.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KNumberOfControls = 2;               // listbox and findbox
const TInt KFindDelayTime = 700000;             // microseconds
const TInt KFindDelayThresholdContacts = 1000;  // amount of contacts to
                                                // activate find delay

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicLogic_ComponentControl = 1,
    EPanicLogic_FindTextL,
    EPanicLogic_PreConds_SetCurrentItemIndexAndDraw,
    EPanicLogic_Command_index_out_of_bounds
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2NamesListReadyState");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


NONSHARABLE_CLASS(TCleanupEnableListBoxViewRedraw)
    {
    public: // Interface

        /**
         * Constructor.
         *
         * @param aListBoxView  The list box view
         */
        inline TCleanupEnableListBoxViewRedraw( CListBoxView& aListBoxView )
            : iCleanupItem( CleanupOp, &aListBoxView )
            {
            }

        /**
         * Returns the cleanup item.
         */
        inline operator TCleanupItem() const
            {
            return iCleanupItem;
            }

    private:  // Implementation
        static void CleanupOp(
                TAny* aPtr );

    private:  // Data
        /// Own: Cleanup item
        TCleanupItem iCleanupItem;
    };

// --------------------------------------------------------------------------
// TCleanupEnableListBoxViewRedraw::CleanupOp
// --------------------------------------------------------------------------
//
void TCleanupEnableListBoxViewRedraw::CleanupOp( TAny* aPtr )
    {
    static_cast<CListBoxView*>(aPtr)->SetDisableRedraw(EFalse);
    }


/**
 * Resolves the next index of the listbox according the key event.
 * If key event is UP, the index is decreased from current index.
 * If key event is DOWN, the index is increased from current index.
 * If current index is last & event is DOWN, the new index is set to first.
 * If current index is first & event is UP, the new index is set to last.
 *
 * @param aKeyEvent     Key event.
 * @param aListBox      List box.
 * @return  Next index of the list item.
 */
TInt NextFocusIndex
        ( const TKeyEvent& aKeyEvent, CEikListBox& aListBox )
    {
    TInt result( KErrNotFound );
    const TInt KFirstItemIndex( 0 );
    TInt itemCount( aListBox.Model()->NumberOfItems() );
    const TInt KLastItemIndex( itemCount - 1 );
    TInt currentItemIndex( aListBox.CurrentItemIndex() );
    if ( aKeyEvent.iCode == EKeyDownArrow )
        {
        result = currentItemIndex + 1;
        if ( result > KLastItemIndex )
            {
            // Set to first
            result = KFirstItemIndex;
            }
        }
    else if ( aKeyEvent.iCode == EKeyUpArrow )
        {
        result = currentItemIndex - 1;
        if ( result < KFirstItemIndex )
            {
            // Set to last
            result = KLastItemIndex;
            }
        }
    else
        {
        result = currentItemIndex;
        }
    return result;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::CPbk2NamesListReadyState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListReadyState::CPbk2NamesListReadyState(
        const CCoeControl& aParent,
        CPbk2ContactViewListBox& aListBox,
        MPbk2FilteredViewStack& aViewStack,
        CAknSearchField* aFindBox,
        CPbk2ThumbnailLoader& aThumbnailLoader,
        MPbk2UiControlEventSender& aEventSender,
        MPbk2ContactNameFormatter& aNameFormatter,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
        CPbk2PredictiveSearchFilter& aSearchFilter ) :
    iParent( aParent ),
    iListBox( aListBox ),
    iViewStack( aViewStack ),
    iFindBox( aFindBox ),
    iThumbnailLoader( aThumbnailLoader ),
    iEventSender( aEventSender ),
    iNameFormatter( aNameFormatter ),
    iCommandItems( aCommandItems ),
    iSearchFilter(aSearchFilter)
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::~CPbk2NamesListReadyState
// --------------------------------------------------------------------------
//
CPbk2NamesListReadyState::~CPbk2NamesListReadyState()
    {
    UnsubscribeCmdItemsVisibility();
    iListBox.SetListCommands( NULL );

    delete iAdaptiveSearchGridFiller;
    delete iFindPolicy;
    delete iFindDelay;
    delete iFindTextBuf;
    iSelectedItems.Close();
    delete iStringArray;
    delete iStringSplitter;
    iIteratorIndexes.Close();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListReadyState* CPbk2NamesListReadyState::NewLC
        ( const CCoeControl* aParent, CPbk2ContactViewListBox& aListBox,
          MPbk2FilteredViewStack& aViewStack, CAknSearchField* aFindBox,
          CPbk2ThumbnailLoader& aThumbnailLoader,
          MPbk2UiControlEventSender& aEventSender,
          MPbk2ContactNameFormatter& aNameFormatter,
          RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
          CPbk2PredictiveSearchFilter& aSearchFilter )
    {
    CPbk2NamesListReadyState* self = new (ELeave) CPbk2NamesListReadyState(
        *aParent, aListBox, aViewStack, aFindBox, aThumbnailLoader,
        aEventSender, aNameFormatter, aCommandItems, aSearchFilter );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListReadyState::ConstructL()
    {
    if ( iFindBox )
        {
        if (!iFindBox->IsFocused() && iParent.IsFocused() )
            {
            iFindBox->SetFocus(ETrue);
            }
        iFindBox->ResetL();
        }

    iFindDelay = CPbk2FindDelay::NewL( *this );

    // Create contact find policy
    CVPbkContactManager& manager =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager();
    CVPbkContactFindPolicy::TParam param =
        CVPbkContactFindPolicy::TParam(
            manager.FieldTypes(),
            manager.FsSession() );
    iFindPolicy = CVPbkContactFindPolicy::NewL( param );

    if( iFindBox )
        {
        iFindBox->AddAdaptiveSearchTextObserverL( this );
        iAdaptiveSearchGridFiller = CPbk2AdaptiveSearchGridFiller::NewL( *iFindBox, iNameFormatter );
        }
    }

void CPbk2NamesListReadyState::UnsubscribeCmdItemsVisibility() const
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( NULL );
        }
    }

void CPbk2NamesListReadyState::SubscribeCmdItemsVisibility()
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ActivateStateL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::ActivateStateL(0x%x)"), this );

    AllowCommandsToShowThemselves( ETrue );  
    UpdateAdaptiveSearchGridL( EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::DeactivateState()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::DeactivateState(0x%x)"), this );

    AllowCommandsToShowThemselves( EFalse );  
    HideThumbnail();
    // Stop creating adaptive search grid.
    if (iAdaptiveSearchGridFiller)
        {
        if (iAdaptiveSearchGridFiller->IsActive())
            {
            iAdaptiveSearchGridFiller->StopFilling();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::CountComponentControls() const
    {
    TInt numberOfControls( KNumberOfControls );
    if ( !iFindBox )
        {
        --numberOfControls;
        }
    return numberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListReadyState::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            {
            return &iListBox;
            }
        case 1:
            {
            return iFindBox;
            }
        default:
            {
            // Illegal state
            __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_ComponentControl));
            return NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SizeChanged()
    {
    const TRect rect(iParent.Rect());
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    if ( iFindBox )
        {
        AknLayoutUtils::LayoutControl(&iListBox, rect,
        AKN_LAYOUT_WINDOW_list_gen_pane(1));

        AknLayoutUtils::LayoutControl( iFindBox, rect,
            AKN_LAYOUT_WINDOW_find_pane );
        if (iFindBox->IsVisible() && iListBox.IsVisible())
            {
            iFindBox->SetLinePos(EABColumn);
            }
        }
    else
        {
        AknLayoutUtils::LayoutControl(&iListBox, rect,
        AKN_LAYOUT_WINDOW_list_gen_pane(0));
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::MakeComponentsVisible(TBool aVisible)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::MakeComponentsVisible(0x%x,%d)"),
        this, aVisible);

    iListBox.MakeVisible(aVisible);

    CEikScrollBar *scrollBar = iListBox.ScrollBarFrame()->GetScrollBarHandle
                                                           (CEikScrollBar::EVertical);
    if ( scrollBar )
       {
        scrollBar->MakeVisible(aVisible);
        }

    if ( iFindBox )
        {
        if ( (iFindBox->IsVisible()&&!aVisible) || (!iFindBox->IsVisible()&&aVisible) )
            {
            iFindBox->MakeVisible(aVisible);
            }
        }

    // Have to call to make sure that scrollbar is updated when state of
    // control is changing.
    TRAP_IGNORE( iListBox.UpdateScrollBarsL() );

    // Change visible state
    iVisible = aVisible;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ActivateL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::ActivateL(0x%x)"), this );

    if ( iFindBox )
        {
        // Adjust find box cursor
        CEikEdwin& findEditor = iFindBox->Editor();
        findEditor.SetCursorPosL(findEditor.TextLength(), EFalse);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::Draw
        ( const TRect& aRect, CWindowGc& aGc ) const
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2NamesListReadyState::Draw(0x%x, TRect(%d,%d,%d,%d))" ),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );

    if ( !iVisible )
        {
        // If control is not visible (blanked) draw a blank background
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext* cc =
            AknsDrawUtils::ControlContext( &iListBox );

        if ( !AknsDrawUtils::Background( skin, cc, &iListBox, aGc, aRect ) )
            {
            // Blank background if no skin present
            aGc.SetPenStyle( CGraphicsContext::ENullPen );
            aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            aGc.DrawRect( aRect );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::DialogMenuBar
// --------------------------------------------------------------------------
//
CEikMenuBar* CPbk2NamesListReadyState::DialogMenuBar(CAknAppUi* aAppUi)
    {
    CEikMenuBar* menuBar = NULL;

    // Check for an active dialog and fetch its menu bar
    if (aAppUi->IsDisplayingMenuOrDialog())
        {
        CCoeControl* ctrl = aAppUi->TopFocusedControl();
        if (ctrl)
            {
            CEikDialog* dialog = ctrl->MopGetObjectNoChaining(dialog);
            if (dialog)
                {
                dialog->MopGetObject(menuBar);
                }
            }
        }
    return menuBar;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ViewMenuBar
// --------------------------------------------------------------------------
//
CEikMenuBar* CPbk2NamesListReadyState::ViewMenuBar(CAknAppUi* aAppUi)
    {
    CEikMenuBar* menuBar = NULL;

    // Check for an active view
    TVwsViewId uid;
    if (aAppUi->GetActiveViewId(uid) == KErrNone)
        {
        if ( uid.iAppUid != uid.iViewUid )
            {
            CAknView* view = 
                static_cast<CAknViewAppUi*> (aAppUi)->View(uid.iViewUid);
            if ( view )
                {
                menuBar = view->MenuBar();
                }
            }
        }
    return menuBar;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::MenuBar
// --------------------------------------------------------------------------
//
CEikMenuBar* CPbk2NamesListReadyState::MenuBar(CAknAppUi* aAppUi)
    {
    // Try fetching a menu bar from currently displayed dialog
    CEikMenuBar* menuBar = DialogMenuBar( aAppUi );

    if ( !menuBar ) // Menu bar was not dialog based
        {
        // Try fetching a menu bar from an active view
        menuBar = ViewMenuBar(aAppUi);
        }

    if ( !menuBar ) // Menu bar was not view based
        {
        // Finally, try fetching a menu bar from AppUi
        menuBar = CEikonEnv::Static()->AppUiFactory()->MenuBar();
        }
    return menuBar;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListReadyState::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // Remember current index and selection before
    // listbox has handled the event.
    TInt oldItemIndex( iListBox.CurrentItemIndex() );
    TBool oldItemMarkedBefore
        ( iListBox.View()->ItemIsSelected( oldItemIndex ) );

    // Remember next index based on key event (UP, DOWN)
    // and selection before listbox has handled the event.
    TInt newItemIndex( NextFocusIndex( aKeyEvent, iListBox ) );
    TBool newItemMarkedBefore
        ( iListBox.View()->ItemIsSelected( newItemIndex ) );

    TKeyResponse result( EKeyWasNotConsumed );
    if ( iFindBox )
        {
        //If the FindBox is locked then dont offer the event to the findbox
        if (!iSearchFilter.IsLocked())
            {
            // use the "C" key to delete the Adaptive search character must
            // change the focus to Adaptive search pane
            if ( aKeyEvent.iCode == EKeyBackspace && iAdaptiveSearchGridFiller )
                {
                iAdaptiveSearchGridFiller->SetFocusToAdaptiveSearchGrid();
                }

            // Offer the key event first to findbox
            result = iFindBox->OfferKeyEventL( aKeyEvent, aType );
		    
			CPbk2PredictiveViewStack& predictiveViewStack = static_cast<CPbk2PredictiveViewStack&>( iViewStack );
        
			TInt contactItemIndex = newItemIndex - CommandItemCount();
			
            if ( contactItemIndex >= 0 && oldItemIndex != newItemIndex && iSearchFilter.IsPredictiveActivated() && iViewStack.Level() && 
                !predictiveViewStack.IsNonMatchingMarkedContact( contactItemIndex ) )
		        {
		        iSearchFilter.CommitFindPaneTextL( iViewStack, iNameFormatter, contactItemIndex );
		        }
				
            }
        else
            {
            //If FindPane is locked, then forward only the backspace key.
            if ( aKeyEvent.iCode == EKeyBackspace )
                {
                // Offer the key event first to findbox
                result = iFindBox->OfferKeyEventL( aKeyEvent, aType );
                }
            }
        }

    // And then offer the event to listbox if necessary
    if ( result == EKeyWasNotConsumed )
        {
        // Use hash key to mark and unmark list item
        if ( (aKeyEvent.iScanCode == EStdKeyHash) 
            && MenuBar( iAvkonAppUi )->ItemSpecificCommandsEnabled() )
            {
            if ( aType == EEventKeyUp )
                {
                if ( newItemMarkedBefore )
                    {
                    iListBox.View()->DeselectItem( newItemIndex );
                    }
                else
                    {
                    iListBox.View()->SelectItemL( newItemIndex );
                    }
                }
            result = EKeyWasConsumed;
            }
        else
            {
            result = iListBox.OfferKeyEventL( aKeyEvent, aType );
            }
        }
    else
        {
        // Find box consumed the key event, abort abort
        return result;
        }

    // If listbox handles the event, test if the selections have changed.
    if ( result == EKeyWasConsumed )
        {
        // If newItemIndex is found (it is not KErrNotFound) and
        // if newItemIndex is not the same as oldItemIndex
        if ( newItemIndex != KErrNotFound
            && newItemIndex != oldItemIndex )
            {
            // Check if the new item index has marked afterwards.
            TBool newItemMarkedAfter
                ( iListBox.View()->ItemIsSelected( newItemIndex ) );

            // If new item selection has changed, send event.
            if ( newItemMarkedBefore != newItemMarkedAfter )
                {
                // Send event about changed state
                TPbk2ControlEvent
                    event( TPbk2ControlEvent::EContactSelected );
                event.iInt = newItemIndex;
                if ( !newItemMarkedAfter )
                    {
                    event.iEventType =
                        TPbk2ControlEvent::EContactUnselected;
                    }
                iEventSender.SendEventToObserversL( event );

                //if commands marked, unmark it back
                if ( !IsContactAtListboxIndex( newItemIndex ) && newItemMarkedAfter )
                    {
                    UnmarkCommands();
                    event.iEventType = TPbk2ControlEvent::EContactUnselected;
                    iEventSender.SendEventToObserversL( event );
                    }
                }
            }

        // Check if old item's selection has changed and send event.
        TBool oldItemMarkedAfter
            ( iListBox.View()->ItemIsSelected( oldItemIndex ) );
        if ( oldItemMarkedBefore != oldItemMarkedAfter )
            {
            // Send event about changed state
            TPbk2ControlEvent
                event( TPbk2ControlEvent::EContactSelected );
            event.iInt = oldItemIndex;
            if ( !oldItemMarkedAfter )
                {
                event.iEventType =
                    TPbk2ControlEvent::EContactUnselected;
                }
            iEventSender.SendEventToObserversL( event );

            //if commands marked, unmark it back
            if ( !IsContactAtListboxIndex( oldItemIndex ) && oldItemMarkedAfter )
                {
                UnmarkCommands();
                event.iEventType = TPbk2ControlEvent::EContactUnselected;
                iEventSender.SendEventToObserversL( event );
                }
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandlePointerEventL
// Event sending differs from OfferKeyEventL's solution on purpose.
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandlePointerEventL
        ( const TPointerEvent& aPointerEvent )
    {
    iSearchFieldPointed = EFalse;
    if ( iFindBox )
        {
        if ( iFindBox->Rect().Contains( aPointerEvent.iPosition ) )
            {
            iFindBox->HandlePointerEventL( aPointerEvent );
            iSearchFieldPointed = ETrue;
            }
        }

    TInt previousItemIndex = iListBox.CurrentItemIndex();
    iListBox.HandlePointerEventL( aPointerEvent );

    // Handle focus change when dragging
    if ( previousItemIndex != iListBox.CurrentItemIndex() &&
         aPointerEvent.iType == TPointerEvent::EDrag )
        {
        HandleFocusChangeL();
        }

    if ( !iSearchFieldPointed && FocusableItemPointed() &&
        aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Send selection events based on marking
        TInt focusIndex = iListBox.CurrentItemIndex();
        TInt commands = CommandItemCount();
        const TBool markedAfter = iListBox.View()->ItemIsSelected( focusIndex );

        TPbk2ControlEvent event( TPbk2ControlEvent::EContactSelected );
        event.iInt = focusIndex;
        if ( !markedAfter )
            {
            event.iEventType = TPbk2ControlEvent::EContactUnselected;
            }
        else if ( focusIndex <  commands)
            {
            UnmarkCommands();
            }
        iEventSender.SendEventToObserversL( event );

        }
    
    TInt currentIndex = iListBox.CurrentItemIndex();
    CPbk2PredictiveViewStack& predictiveViewStack = static_cast<CPbk2PredictiveViewStack&>( iViewStack );
  
    TInt contactItemIndex = currentIndex - CommandItemCount();
    if ( contactItemIndex >= 0 && iFindBox && iSearchFilter.IsPredictiveActivated() && iViewStack.Level() &&
            !predictiveViewStack.IsNonMatchingMarkedContact( contactItemIndex ) )
        {
        iSearchFilter.CommitFindPaneTextL( iViewStack, iNameFormatter, contactItemIndex );
        }
        
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateReady;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandleContactViewEventL
        (TInt aEvent, TInt aIndex)
    {
    // need to calculate list index from contact
    // view index
    TInt listIndex = CommandItemCount() + aIndex;
    HandleListboxEventL(aEvent, listIndex, EFalse );
    
    TInt currentFocusIndex = iListBox.CurrentItemIndex();
    TBool marked = iListBox.View()->ItemIsSelected( currentFocusIndex );

    //update findpane according to the new focused contact after delete operation.
    //Do nothing if the next focused contact is marked because it would not be found. 
    if ( aEvent == MPbk2NamesListState::EItemRemoved 
    		&& iSearchFilter.IsPredictiveActivated() 
    		&& iViewStack.Level()
    		&& iFindBox
    		&& iFindBox->TextLength() != 0 
    		&& !marked)
    	{
    	iSearchFilter.CommitFindPaneTextL( iViewStack, iNameFormatter, currentFocusIndex );
    	}
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandleCommandEventL
        (TInt aEvent, TInt /* aCommandIndex*/ )
    {
    HandleListboxEventL(aEvent, 0, ETrue );    
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandleListboxEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandleListboxEventL
        (TInt aEvent, TInt aListboxIndex, TBool aMantainFocus )
    {
    switch (aEvent)
        {
        case EItemAdded:
            {
            //contact added, grid might not have all necessary characters.
            //this could be optimized, we could just check the added contact and
            //update map if the performance is not enough
            UpdateAdaptiveSearchGridL( ETrue );

            // Inform find box and list box
            UpdateFindBoxL();

            TInt topIndex = iListBox.TopItemIndex();
            iListBox.HandleEventL(aEvent, aListboxIndex);

            // Maintain focus
            // TODO: similar focus handling code in case item delete in iListBox.HandleEventL
            // to consider to move this focus code there as well
            if (aListboxIndex >= 0 && aListboxIndex < iListBox.NumberOfItems() )
                {
                if ( aMantainFocus )
                    {
                    //keep focused item and top item if its not 0
                    if ( aListboxIndex <= iListBox.CurrentItemIndex() &&
                         ( iListBox.CurrentItemIndex() + 1 ) < iListBox.NumberOfItems() )
                        {
                        iListBox.SetCurrentItemIndex(
                                iListBox.CurrentItemIndex() + 1 );
                        if ( topIndex > 0 && ( topIndex + 1 ) < iListBox.NumberOfItems() )
                            {
                            iListBox.SetTopItemIndex( topIndex + 1 );
                            }
                        }
                    }
                else
                    {
                    //change focus to the added item
                    if ( aListboxIndex != iListBox.CurrentItemIndex() )
                        {
                        iListBox.SetCurrentItemIndex( aListboxIndex );
                        }
                    }
                }

            iParent.DrawDeferred();
            HandleFocusChangeL();
            break;
            }
        case EItemRemoved:
            {
            //contact removed, grids might have unecessary characters
            UpdateAdaptiveSearchGridL( ETrue );

            // Inform find box and list box
            UpdateFindBoxL();
            iListBox.HandleEventL(aEvent, aListboxIndex);

            // Maintain focus
            if (aListboxIndex >= 0 && aListboxIndex < iListBox.NumberOfItems() )
                {
                if ( aMantainFocus )
                    {
                    //no op
                    }
                else
                    {
                    //change focus 
                    if ( aListboxIndex != iListBox.CurrentItemIndex() )
                        {
                        iListBox.SetCurrentItemIndex( aListboxIndex );
                        }
                    }
                }            

            iParent.DrawDeferred();
            HandleFocusChangeL();
            break;
            }
        case EContactViewUpdated:
            {
            UpdateAdaptiveSearchGridL( ETrue );
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
// CPbk2NamesListReadyState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListReadyState::CoeControl()
    {
    CCoeControl* tmp = NULL;
    return *static_cast<CCoeControl*>(tmp);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandleControlEventL( CCoeControl* aControl,
        MCoeControlObserver::TCoeEvent aEventType, TInt aParam )
    {
    if ( aEventType == MCoeControlObserver::EEventStateChanged )
        {
        // Find box
        if ( aControl == iFindBox )
            {
            if( aParam )
                {
                //means the search came from adaptive search grid
                if( iAdaptiveSearchGridFiller )
                    {
                    iAdaptiveSearchGridFiller->InvalidateAdaptiveSearchGrid();
                    }
                }
            
            if ( iFindDelay->IsActive() )
                {
                iFindDelay->Cancel();
                }
            //if aParam is ETrue, it means that event came from adaptive search, which means
            //we do not want to delay filtering
            if ( ( !aParam ) &&
                 ( iFindDelay ) &&
                 ( NumberOfContacts() >= KFindDelayThresholdContacts ) )
                {
                iFindDelay->After( TTimeIntervalMicroSeconds32( KFindDelayTime ) );
                }
            else
                {
                UpdateFindResultL();
                }
            }
        // List
        else if ( aControl == &iListBox )
            {
            HandleFocusChangeL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListReadyState::TopContactL()
    {
    const MVPbkViewContact* contact = NULL;

    // There is an offset between listbox indexes and view indexes.
    const TInt topListboxIndex = iListBox.TopItemIndex();
    const TInt enabledCommandCount = CommandItemCount();
    // Check whether the index is valid. Also check whether the top
    // really is a contact.
    if ( topListboxIndex >= 0 && topListboxIndex >= enabledCommandCount  )
        {
        // Map from listbox index to view index
        contact = &iViewStack.ContactAtL(
            topListboxIndex - enabledCommandCount );
        }
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListReadyState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::NumberOfContacts() const
    {
    // The listbox can also have command items.
    TInt result = iListBox.NumberOfItems() - CommandItemCount();
    if ( result < 0 )
        {
        result = 0; // No contacts
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListReadyState::FocusedContactL() const
    {
    return FocusedViewContactL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListReadyState::FocusedViewContactL() const
    {
    const MVPbkViewContact* contact = NULL;

    // There is an offset between listbox indexes and view indexes.
    const TInt focusListboxIndex = iListBox.CurrentItemIndex();
    const TInt enabledCommandCount = CommandItemCount();
    // Is the focus on a contact. Check...
    if (focusListboxIndex >= 0 && focusListboxIndex >= enabledCommandCount )
        {
        // Yes, the focus is on a contact.
        // Map from listbox index to view index
        contact = &iViewStack.ContactAtL(
            focusListboxIndex - enabledCommandCount );
        }
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2NamesListReadyState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedContactL
        ( const MVPbkBaseContact& aContact )
    {
    SetFocusedContactL( KErrNotSupported, &aContact );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedContactL
        ( const MVPbkContactBookmark& aContactBookmark )
    {
    TInt viewIndex = iViewStack.IndexOfBookmarkL( aContactBookmark );
    SetFocusedContactL( viewIndex, NULL );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedContactL
        ( const MVPbkContactLink& aContactLink )
    {
    TInt viewIndex = iViewStack.IndexOfLinkL( aContactLink );
    SetFocusedContactL( viewIndex, NULL );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::FocusedContactIndex() const
    {
    // The result is a view index. There is an offset between listbox indexes
    // and view indexes.

    TInt viewIndex = KErrNotFound;
    const TInt currentListboxIndex = iListBox.CurrentItemIndex();
    const TInt commandCount = CommandItemCount();
    // Is the focus on a contact or on a command item? Check:
    if ( currentListboxIndex >= commandCount )
        {
        // Yes, the focus is on a contact.
        // Need to map from listbox indexes to view indexes.
        viewIndex = currentListboxIndex - commandCount;
        }
    return viewIndex;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedContactIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedContactIndexL( TInt aIndex )
    {
    // The aIndex is a view index. There is an offset between listbox indexes
    // and view indexes.

    if ( aIndex < 0 )
        {
        aIndex = 0;
        }
    const TInt enabledCommandCount = CommandItemCount();
    if ( aIndex > iListBox.BottomItemIndex() - enabledCommandCount )
        {
        aIndex = iListBox.BottomItemIndex() - enabledCommandCount;
        }

    SetFocusedContactL( aIndex, NULL );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CPbk2NamesListReadyState::FocusedField() const
    {
    // There is no field level focus
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::FocusedFieldIndex() const
    {
    // There is no field level focus
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::ContactsMarked() const
    {
    return iListBox.ContactsMarked();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListReadyState::SelectedContactsL() const
    {
    // Get listbox selection indexes, a reference only
    const CListBoxView::CSelectionIndexArray* selArray =
        iListBox.SelectionIndexes();


    TInt count = 1; // initialize to one, because if there are
                    // no selections we operate with one focused contact
    if (selArray)
        {
        count = selArray->Count();
        }

    CVPbkContactLinkArray* linkArray = NULL;

    if (count > 0)
        {
        linkArray = CVPbkContactLinkArray::NewLC();

        // Loop through the selections and construct a link for
        // each of the contacts
        const TInt commandItemCount = CommandItemCount();
        for (TInt i=0; i<count; ++i)
            {
            const TInt singleSelection = selArray->At(i);
            if ( IsContactAtListboxIndex( singleSelection ) )
                {
                // Take the view contact from the view, the ownership
                // stays at the view
                const MVPbkViewContact* contact =
                    &iViewStack.ContactAtL( singleSelection - commandItemCount );
                MVPbkContactLink* link = contact->CreateLinkLC();
                if (link)
                    {
                    linkArray->AppendL(link);
                    CleanupStack::Pop(); // the array takes ownership of the link
                    }
                }
            }

        CleanupStack::Pop(linkArray);
        }

    return linkArray;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListReadyState::SelectedContactsOrFocusedContactL() const
    {
    MVPbkContactLinkArray* result = NULL;
    result = SelectedContactsL();

    if ( !result )
        {
        CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();
        const MVPbkBaseContact* focusedContact = FocusedContactL();
        if ( focusedContact )
            {
            MVPbkContactLink* link = focusedContact->CreateLinkLC();
            CleanupStack::Pop(); // link
            array->AppendL( link );
            }
        else
        	{
			// there was no focused contact, check if this is the my card
			AppendMyCardLinkIfExistL( *array );
        	}
        CleanupStack::Pop(); // array
        result = array;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::AppendMyCardLinkIfExistL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::AppendMyCardLinkIfExistL( CVPbkContactLinkArray& aArray ) const
	{
	TInt currentItem = iListBox.CurrentItemIndex();
	// if focused index is command item
	if( currentItem < CommandItemCount() )
		{
		// check if the command is my card ( this search is copied from CommandItemAt - function to avoid const cast)
		// Some of the commands might be disabled. Skip those.
	    TInt enabledCount = 0;
	    TInt indexOfResult = KErrNotFound;
	    for ( TInt n = 0; n < iCommandItems.Count() && indexOfResult == KErrNotFound; ++n )
			{
			if ( iCommandItems[ n ]->IsEnabled() )
				{
				enabledCount++;
				if ( enabledCount-1 == currentItem )
					{
					indexOfResult = n;
					}
				}
			}
	    MPbk2UiControlCmdItem* item = iCommandItems[ indexOfResult ];
	    // check if the command item was a my card
		if( item->CommandId() == EPbk2CmdOpenMyCard )
			{
			// get extension point and my card link
			TAny* object = item->ControlCmdItemExtension( TUid::Uid( KPbk2ControlCmdItemExtensionUID ) );
			if(  object )
				{
				MPbk2DoubleListboxCmdItemExtension* extension = 
						static_cast<MPbk2DoubleListboxCmdItemExtension*>( object );
				// if extension exists
				if( extension )
					{
					const MVPbkContactLink* link = extension->Link();
					// if link exists, add it to the array
					if( link )
						{
						aArray.AppendL( link->CloneLC() );
						CleanupStack::Pop();
						}
					}
				}
			}
		}
	}


// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListReadyState::SelectedContactsIteratorL() const
    {
    // Get listbox selection indexes, a reference only
    const CListBoxView::CSelectionIndexArray* selArray =
        iListBox.SelectionIndexes();

    TInt count = 0;
    if ( selArray )
        {
        count = selArray->Count();
        }

    MPbk2ContactLinkIterator* iterator = NULL;

    if ( count > 0 )
        {
        // Need to map the listbox indexes to view indexes.
        // Also need to filter out cmd items.
        iIteratorIndexes.Reset();
        const TInt commandItemCount = CommandItemCount();
        const TInt selectionCount = selArray->Count();
        for ( TInt n = 0; n < selectionCount; ++n )
            {
            const TInt listboxIndex =  selArray->At( n );
            if ( listboxIndex >= commandItemCount)
                {
                // It is a contact.
                // Do a mapping from listbox index to view index
                iIteratorIndexes.AppendL( listboxIndex - commandItemCount );
                }
            }
        iterator = CPbk2ContactViewIterator::NewLC
            ( iViewStack, iIteratorIndexes.Array() );
        CleanupStack::Pop(); // iterator
        }

    return iterator;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListReadyState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ClearMarks()
    {
    iListBox.View()->ClearSelection();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetSelectedContactL
        ( TInt aIndex, TBool aSelected )
    {
    if ( aSelected )
        {
        iListBox.View()->SelectItemL( aIndex );
        }
    else
        {
        iListBox.View()->DeselectItem( aIndex );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetSelectedContactL(
        const MVPbkContactBookmark& aContactBookmark,
        TBool aSelected )
    {
    TInt cmdItemCount = CommandItemCount();
    TInt index = iViewStack.IndexOfBookmarkL( aContactBookmark );
    if ( index != KErrNotFound )
        {
        SetSelectedContactL( cmdItemCount+index, aSelected );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetSelectedContactL(
        const MVPbkContactLink& aContactLink,
        TBool aSelected )
    {
    TInt cmdItemCount = CommandItemCount();
    TInt index = iViewStack.IndexOfLinkL( aContactLink );
    if ( index != KErrNotFound )
        {
        SetSelectedContactL( index+cmdItemCount, aSelected );
        }
    }


TInt CPbk2NamesListReadyState::CommandItemCount() const
    {
    // Some of the commands might be disabled. Don't count those.
	TInt enabledCommandCount = 0;
	for ( TInt n = 0; n < iCommandItems.Count(); ++n )  
		{
		if ( iCommandItems[ n ]->IsEnabled() )
			{
			enabledCommandCount++;
			}
		}
	return enabledCommandCount;
	}

const MPbk2UiControlCmdItem&
CPbk2NamesListReadyState::CommandItemAt( TInt aIndex ) const
    {
    // Some of the commands might be disabled. Skip those.
    TInt enabledCount = 0;
    TInt indexOfResult = KErrNotFound;

	for ( TInt n = 0; n < iCommandItems.Count()&& indexOfResult == KErrNotFound; ++n )
		{
		if ( iCommandItems[ n ]->IsEnabled() )
			{
			enabledCount++;
			if ( enabledCount-1 == aIndex )
				{
				indexOfResult = n;
				}
			}
		}

	return *iCommandItems[ indexOfResult ];
	}

const MPbk2UiControlCmdItem*
CPbk2NamesListReadyState::FocusedCommandItem() const
	{
	const MPbk2UiControlCmdItem* cmdItem = NULL;
	// Is the focus on a command item:
    TInt focusListIndex = iListBox.CurrentItemIndex();
    const TInt commandItemCount = CommandItemCount();
    if ( focusListIndex != KErrNotFound && focusListIndex < commandItemCount )
        {
        // Yes it's a command item.
        cmdItem = &CommandItemAt(focusListIndex);
        }
    return cmdItem;
    }

void CPbk2NamesListReadyState::DeleteCommandItemL( TInt /*aIndex*/ )
    {
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
    }

void CPbk2NamesListReadyState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
    {
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenuPane, &iListBox);
    
    TInt focusedItem = iListBox.View()->CurrentItemIndex();
    TBool markHidden = iListBox.View()->ItemIsSelected( focusedItem );
    TBool unmarkHidden = !iListBox.View()->ItemIsSelected( focusedItem );
    TBool markAllHidden = ( iListBox.Model()->NumberOfItems() == 0 ) || 
        ( iListBox.SelectionIndexes()->Count() == iListBox.Model()->NumberOfItems() );
    TBool unmarkAllHidden = ( iListBox.Model()->NumberOfItems() == 0 ) || 
        ( iListBox.SelectionIndexes()->Count() == 0 );
    
    TInt position;
    if (aMenuPane->MenuItemExists(EAknCmdMark, position))
        {
        aMenuPane->SetItemDimmed(EAknCmdMark, markHidden);
        }
    if (aMenuPane->MenuItemExists(EAknCmdUnmark, position))
        {
        aMenuPane->SetItemDimmed(EAknCmdUnmark, unmarkHidden);
        }
    if (aMenuPane->MenuItemExists(EAknMarkAll, position))
        {
        aMenuPane->SetItemDimmed(EAknMarkAll, markAllHidden);
        }
    if (aMenuPane->MenuItemExists(EAknUnmarkAll, position))
        {
        aMenuPane->SetItemDimmed(EAknUnmarkAll, unmarkAllHidden);
        }
    
    // When all contacts are marked in the listbox, the command items are not marked.
    // This code snippet dims out the  Mark All menu item which is shown since the
    // list box cannot differentiate a command and a contact item

    TInt commandItemCount = CommandItemCount();


    if ((iListBox.SelectionIndexes()->Count() + commandItemCount ) == iListBox.Model()->NumberOfItems())
        {
        TInt i; // Stores the position of the searched menu item.
                // This position is not needed or used anywhere
        if (aMenuPane->MenuItemExists(EAknMarkAll, i))
            {
            aMenuPane->SetItemDimmed(EAknMarkAll, ETrue);
            }
        }

    // If there's any, command items are always placed at the top of the list box.
    // By comparing the list box current item index with the command item count, we are trying to find out
    // if the current focused item is command item or not.

    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ProcessCommandL
        (TInt aCommandId) const
    {
    AknSelectionService::HandleMarkableListProcessCommandL
        (aCommandId, &iListBox);

    // Send event
    TPbk2ControlEvent event( TPbk2ControlEvent::EContactSelected );
    event.iInt = iListBox.CurrentItemIndex();

    switch( aCommandId )
        {
        case EAknCmdMark:
        case EAknMarkAll:
            {
            // Send event about changed state
            iEventSender.SendEventToObserversL( event );
            // commands should stay unmarked
            UnmarkCommands();
            break;
            }

        case EAknCmdUnmark:
            {
            event.iEventType = TPbk2ControlEvent::EContactUnselected;
            iEventSender.SendEventToObserversL( event );
            break;
            }

        case EAknUnmarkAll:
            {
            // Send event about changed state
            event.iEventType = TPbk2ControlEvent::EContactUnselectedAll;
            iEventSender.SendEventToObserversL( event );
            break;
            }

        default:;
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListReadyState::GetMenuFilteringFlagsL() const
    {
    TInt ret = KPbk2ListContainsItems;
    if (ContactsMarked())
        {
        ret |= KPbk2ListContainsMarkedItems;
        }
    else
        {
        ret |= KPbk2ListContainsNoMarkedItems;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListReadyState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetFocusedContactL( TInt aIndex,
        const MVPbkBaseContact* aContact )
    {
    // The aIndex is a view index. There is an offset between listbox indexes
    // and view indexes.

    TInt viewIndex = KErrNotReady;

    if ( aContact )
        {
        MVPbkContactBookmark* bookmark = aContact->CreateBookmarkLC();
        TInt contactIndex = iViewStack.IndexOfBookmarkL( *bookmark );
        CleanupStack::PopAndDestroy(); // bookmark

        if ( contactIndex >= 0 &&
                contactIndex < iViewStack.ContactCountL() )
            {
            viewIndex = contactIndex;
            }
        }

    if ( viewIndex == KErrNotReady )
        {
        if ( aIndex >= 0 &&
                aIndex <  iViewStack.ContactCountL() )
            {
            viewIndex = aIndex;
            }
        }

    // Need to map current listbox index to view indexes.
    // But at the same time check whether the current item is a contact.
    const TInt enabledCommandCount = CommandItemCount();
    TInt currentContactAsViewIndex = KErrNotReady;
    if ( iListBox.CurrentItemIndex() >= enabledCommandCount )
        {
        // Yes, the current item is a contact.
        currentContactAsViewIndex =
            iListBox.CurrentItemIndex() - enabledCommandCount;
        }

    // If index is valid, and not already the current one, then use it to set
    // the current item.
    if ( viewIndex != KErrNotReady && viewIndex != currentContactAsViewIndex )
        {
        iListBox.SetCurrentItemIndex( viewIndex + enabledCommandCount );
        // Force hiding of thumbnail from previously focused contact
        HideThumbnail();
        HandleFocusChangeL();
        }

    // Should be drawn always to be sure that UI is updated
    iParent.DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::RestoreControlStateL
        (CPbk2ViewState* aState)
    {
    TBool redraw = EFalse;
    if (aState->Flags() & CPbk2ViewState::EInitialized)
        {
        iListBox.Reset();
        redraw = ETrue;
        }
    else
        {
        if (iViewStack.ContactCountL() > 0)
            {
            if (aState->Flags() & CPbk2ViewState::EFocusFirst)
                {
                iListBox.SetCurrentItemIndex(0);
                }
            else if (aState->Flags() & CPbk2ViewState::EFocusLast)
                {
                iListBox.SetCurrentItemIndex(iListBox.Model()->NumberOfItems()-1);
                }
            else
                {
                // Restore top contact
                const MVPbkContactLink* topContact = aState->TopContact();
                if (topContact)
                    {
                    const TInt viewIndex = iViewStack.IndexOfLinkL(*topContact);
                    if (viewIndex >= 0)
                        {
                        const TInt prevListboxIndex = iListBox.TopItemIndex();
                        // There is an offset between listbox indexes
                        // and view indexes.
                        const TInt listboxIndex = viewIndex + CommandItemCount();
                        iListBox.SetTopItemIndex(listboxIndex);
                        iListBox.FixTopItemIndex();
                        if (listboxIndex != prevListboxIndex)
                            {
                            redraw = ETrue;
                            }
                        }
                    }

                const MVPbkContactLink* focusedContact = aState->FocusedContact();
                if (focusedContact)
                    {
                    const TInt viewIndex =
                        iViewStack.IndexOfLinkL(*focusedContact);
                    if (viewIndex >= 0)
                        {
                        const TInt prevListboxIndex = iListBox.CurrentItemIndex();
                        // There is an offset between listbox indexes
                        // and view indexes.
                        const TInt listboxIndex = viewIndex + CommandItemCount();
                        iListBox.SetCurrentItemIndex(listboxIndex);
                        if (listboxIndex != prevListboxIndex)
                            {
                            redraw = ETrue;
                            }
                        }
                    }
                }
            // Restore selections
            const MVPbkContactLinkArray* markedContacts =
                aState->MarkedContacts();
            if (RestoreMarkedContactsL(markedContacts))
                {
                redraw = ETrue;
                }
            }
        }
    if (redraw)
        {
        iParent.DrawDeferred();
        iListBox.UpdateScrollBarsL();
        HandleFocusChangeL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListReadyState::FindTextL()
    {
    if ( iFindBox )
        {
        TPbk2FindTextUtil::EnsureFindTextBufSizeL(
                *iFindBox,
                &iFindTextBuf );
        TPtr bufPtr = iFindTextBuf->Des();
        iFindBox->GetSearchText(bufPtr);
        return *iFindTextBuf;
        }

    return KNullDesC;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ResetFindL()
    {
    if ( iFindBox && iFindBox->TextLength() > 0 )
        {
        iSearchFilter.ResetL();
        iFindBox->ResetL();
        iViewStack.Reset();
        iFindBox->DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HideThumbnail()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ShowThumbnail()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetBlank(TBool aBlank)
    {
    if (!aBlank)
        {
        // Redraw
        iParent.DrawDeferred();
        TRAPD( error,
            iListBox.UpdateScrollBarsL();
            HandleFocusChangeL();
            );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::RegisterCommand(
        MPbk2Command* /*aCommand*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetTextL( const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListReadyState::ContactUiControlExtension(TUid aExtensionUid )
    {
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        {
        return static_cast<MPbk2ContactUiControl2*>( this );
        }

    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::FocusedItemPointed()
    {
    return iListBox.FocusedItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::FocusableItemPointed()
    {
    return iListBox.FocusableItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::SearchFieldPointed()
    {
    return iSearchFieldPointed;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::TopViewChangedL
//
// This is called when top view is changed. However this happens before .e.g 
// the view containing top contacts is ready and therefore in this point of 
// execution we don't know whether there will be e.g. "Add to favorites"
// command item visible or not
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::TopViewChangedL(
        MVPbkContactViewBase& aOldView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::TopViewChangedL begin") );

    // Switch view
    TInt countBefore = aOldView.ContactCountL();
    TInt countAfter = iViewStack.ContactCountL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::TopViewChangedL:before=%d,now=%d"),
        countBefore, countAfter );

    // Markings must be checked before HandleItemAdditionL or
    // HandleItemRemovalL because they reset markings
    TBool marked = ContactsMarked();

    // Promotion item "Add to favourites" and "My card" should be visible,
    // the conditions are as following:  
    // 1. Top view is base view (iViewStack.Level() == 0).
    // 2. There is no text entered in find box.
    // 3. There is text entered in find box, but the text is space.
    // When promotion item "Add to favourites" and "My card" are visible,
    // remote search should not be shown.
    TBool showFavAndMyCard = ETrue;
    if ( iFindBox )
        {
        RBuf searchText;
        searchText.CreateL ( iFindBox->TextLength() );
        iFindBox->GetSearchText( searchText ); 
        CleanupClosePushL( searchText );
        searchText.TrimLeft();
        if ( searchText.Length() != 0 )
            {
            showFavAndMyCard = EFalse;
            }
        CleanupStack::PopAndDestroy( &searchText );
        }
    if ( iViewStack.Level() != 0 )
        {
        showFavAndMyCard = EFalse;
        }
    
    // Update the list box.
    UpdateCommandEnabled( EPbk2CmdAddFavourites, showFavAndMyCard );
    UpdateCommandEnabled( EPbk2CmdRcl, !showFavAndMyCard );    
    UpdateCommandEnabled( EPbk2CmdOpenMyCard, showFavAndMyCard );

    if ( countAfter > countBefore )    //count does not contain command items 
        {
        iListBox.HandleItemAdditionL();
        }
    if ( countAfter < countBefore )
        {
        iListBox.HandleItemRemovalL();
        }
    
    UpdateAdaptiveSearchGridL( EFalse );

    if ( marked )
        {
        // update markings to the new view
        RestoreMarkedItemsL( iSelectedItems );
        }

    if ( iSearchFilter.IsPredictiveActivated() )
        {
        TInt indexOfMatchedItem = iSearchFilter.CommitFindPaneTextL( iViewStack, iNameFormatter );
        if ( indexOfMatchedItem == KErrNotFound )
            {
            indexOfMatchedItem = 0;
            }
        iListBox.SetCurrentItemIndexAndDraw(indexOfMatchedItem);
        }
    else
        {
        SetCurrentItemIndexAndDrawL();
        }

    HandleFocusChangeL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::TopViewChangedL end") );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::TopViewUpdatedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::TopViewUpdatedL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::TopViewUpdatedL begin") );

    // Promotion item "Add to favourites" should not be visible if top view is 
	// not base view (iViewStack.Level() !== 0). Remote search may be shown when there
	// is text entered in find box. MyCard enabled similarly as add favorites.
    TBool addFavoOk = iViewStack.Level() == 0 && !(iFindBox && iFindBox->TextLength());	 

    UpdateCommandEnabled( EPbk2CmdAddFavourites, addFavoOk ); 
    UpdateCommandEnabled( EPbk2CmdRcl, !addFavoOk );          
    UpdateCommandEnabled( EPbk2CmdOpenMyCard, addFavoOk );
    UpdateAdaptiveSearchGridL( ETrue );

    if ( ContactsMarked() )
        {
        // The view didn't change so we have to restore the items that
        // were saved last time the find box was updated.
        RestoreMarkedItemsL( iSelectedItems );
        SetCurrentItemIndexAndDrawL();
        }
    HandleFocusChangeL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::BaseViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::BaseViewChangedL()
    {
    HandleFocusChangeL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ViewStackError
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ViewStackError( TInt /*aError*/ )
    {
    // CPbk2NameListControl is listening to view stack errors and handles
    // them. This is UI control's internal design and the state can
    // ignore the error
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/, TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // CPbk2NameListControl handles this.
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::FindDelayComplete
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::FindDelayComplete()
    {
    TRAPD( error, UpdateFindResultL() );
    if ( error != KErrNone )
        {
        CCoeEnv::Static()->HandleError( error );
        iViewStack.Reset();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::AdaptiveSearchTextChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::AdaptiveSearchTextChanged( CAknSearchField* aSearchField )
    {
    //Add this line to call CPbk2AdaptiveSearchGridFiller::InvalidateAdaptiveSearchGrid()
    //to control GRID must be updated always after user has pressed a key, even if the keymap does not changed
    HandleControlEventL( aSearchField, MCoeControlObserver::EEventStateChanged, ETrue );

    //this callback method is only used to notice when language has changed and
    //to update adaptive search grid.

    if( aSearchField->LanguageChanged() )
        {
        UpdateAdaptiveSearchGridL( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::CmdItemVisibilityChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible )
    {
    TInt cmdItemIndex = FindCommand(aCmdItemId);
    TRAP_IGNORE( HandleCommandEventL(
                (aVisible ? EItemAdded : EItemRemoved),
                cmdItemIndex) );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::RestoreMarkedContactsL
// Marks specified contacts in the listbox.
//
// @param aMarkedContacts   Contacts to mark.
// @return  True if any contacts were marked in the list.
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::RestoreMarkedContactsL
        (const MVPbkContactLinkArray* aMarkedContacts)
    {
    TBool result = EFalse;
    DisableRedrawEnablePushL();
    iListBox.ClearSelection();
    if (aMarkedContacts)
        {
        const TInt count = aMarkedContacts->Count();
        for (TInt i=0; i < count; ++i)
            {
            const MVPbkContactLink& contact = aMarkedContacts->At(i);
            const TInt viewIndex = iViewStack.IndexOfLinkL(contact);
            if (viewIndex >= 0)
                {
                const TInt listboxIndex = viewIndex + CommandItemCount();
                iListBox.View()->SelectItemL(listboxIndex);
                result = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy();  // DisableRedrawEnablePushL
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::RestoreMarkedItemsL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::RestoreMarkedItemsL(
        const MVPbkContactBookmarkCollection& aSelectedItems )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::RestoreMarkedItemsL: %d items"),
        aSelectedItems.Count() );

    DisableRedrawEnablePushL();
    iListBox.ClearSelection();
    const TInt count = aSelectedItems.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        TInt index = iViewStack.IndexOfBookmarkL( aSelectedItems.At( i ) ) + CommandItemCount();
        if ( index >= 0 )
            {
            iListBox.View()->SelectItemL( index );
            }
        }
    CleanupStack::PopAndDestroy();  // DisableRedrawEnablePushL
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::DisableRedrawEnablePushL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::DisableRedrawEnablePushL()
    {
    CListBoxView& listBoxView = *iListBox.View();
    listBoxView.SetDisableRedraw(ETrue);
    CleanupStack::PushL(TCleanupEnableListBoxViewRedraw(listBoxView));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HandleFocusChangeL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::HandleFocusChangeL()
    {
    iEventSender.SendEventToObserversL(
        TPbk2ControlEvent::TPbk2ControlEvent::EControlFocusChanged );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::UpdateFindResultL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::UpdateFindResultL()
    {
    TBuf <KSearchFieldLength> findPaneTxt( FindTextL() );

    if ( iSearchFilter.StartFindPaneInlineEditL( findPaneTxt ) )
        {
        TBool selectionChanged = HasSelectedItemsChangedL();

        MVPbkContactBookmarkCollection* marked = NULL;
        if ( ContactsMarked() )
            {
            marked = &SelectedItemsL( iViewStack );
            }
        else
            {
            // User has removed all the marks so update also bookmarks.
            iSelectedItems.RemoveAndDeleteAll();
            }

        // Extract the find text into array
        if ( !iStringSplitter )
            {
            iStringSplitter = CPbk2FindStringSplitter::NewL( iNameFormatter );
            }
        MDesCArray* temp = iStringSplitter->SplitTextIntoArrayL( findPaneTxt );
        delete iStringArray;
        iStringArray = temp;

        iViewStack.UpdateFilterL( *iStringArray, marked, selectionChanged );

        UpdateAdaptiveSearchGridL( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::UpdateFindBoxL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::UpdateFindBoxL()
    {
    if ( iFindBox )
        {
        if ( iViewStack.ContactCountL() > 0 && iParent.IsFocused() )
            {
            iFindBox->SetFocusing( ETrue );
            // If findbox is focusing, SetFocus would clear some flag of Fep, 
            // and it will cause 'b','c',''e',...could not be entered for the 
            // first character by V-ITUT.
            if ( !iFindBox->IsFocused() )
            	{
            	iFindBox->SetFocus( ETrue, EDrawNow );
            	}
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SelectedItemsL
// --------------------------------------------------------------------------
//
MVPbkContactBookmarkCollection& CPbk2NamesListReadyState::SelectedItemsL(
        MVPbkContactViewBase& aCurrentView )
    {
    // Get listbox selection indexes, a reference only
    const CListBoxView::CSelectionIndexArray* selArray =
        iListBox.SelectionIndexes();

    if ( selArray )
        {
        iSelectedItems.RemoveAndDeleteAll();
        const TInt count = selArray->Count();
        for ( TInt i = 0; i < count; ++i )
            {

            iSelectedItems.AppendL(
                aCurrentView.CreateBookmarkLC( selArray->At(i) - CommandItemCount() ) );
            CleanupStack::Pop();
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::SelectedItemsL:%d marked"),
        iSelectedItems.Count() );

    return iSelectedItems;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::HasSelectedItemsChangedL
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListReadyState::HasSelectedItemsChangedL()
    {
    TBool result = EFalse;

    // Get listbox selection indexes, a reference only
    const CListBoxView::CSelectionIndexArray* selArray =
        iListBox.SelectionIndexes();

    if ( selArray )
        {
        TInt bookmarkCount = iSelectedItems.Count();
        TInt markedCount = selArray->Count();
        // Changed if number of items are different
        result = bookmarkCount != markedCount;
        if ( !result )
            {
            // There are equeal amount of items. Compare as long
            // as the difference is found
            for ( TInt i = 0; i < bookmarkCount && !result; ++i )
                {
                result = selArray->At(i) !=
                    iViewStack.IndexOfBookmarkL( iSelectedItems.At(i) );
                }
            }
        }
    else if ( iSelectedItems.Count() > 0 )
        {
        result = ETrue;
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListReadyState::HasSelectedItemsChangedL: %d "),
        result );

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListReadyState::SetCurrentItemIndexAndDrawL
// --------------------------------------------------------------------------
//
void CPbk2NamesListReadyState::SetCurrentItemIndexAndDrawL()
    {
    __ASSERT_DEBUG( &iListBox && iFindPolicy,
        Panic(EPanicLogic_PreConds_SetCurrentItemIndexAndDraw) );

    if ( iStringArray && iStringArray->MdcaCount() > 0 )
        {
        TBool isCurrentItemSet( EFalse );
        const TInt count( iViewStack.ContactCountL() );
        for ( TInt i(0); i < count; ++i )
            {
            const MVPbkViewContact& contact = iViewStack.ContactAtL( i );
            if ( iFindPolicy->MatchContactNameL( *iStringArray, contact ) )
                {
                isCurrentItemSet = ETrue;
                iListBox.SetCurrentItemIndexAndDraw(i);
                break;
                }
            }
        if ( !isCurrentItemSet )
            {
            iListBox.SetCurrentItemIndexAndDraw(0);
            }
        }
    }

void CPbk2NamesListReadyState::UpdateAdaptiveSearchGridL( TBool aClearCache )
    {
    if( ( !iAdaptiveSearchGridFiller ) || ( !iFindBox ) )
        {
        return;
        }

    CAknSearchField::TSearchFieldStyle searchStyle = iFindBox->SearchFieldStyle();

    if ( searchStyle != CAknSearchField::EAdaptiveSearch )
        {
        delete iAdaptiveSearchGridFiller;
        iAdaptiveSearchGridFiller = NULL;
        return;
        }

    iAdaptiveSearchGridFiller->StartFillingL( iViewStack, FindTextL(), aClearCache );
    }

/**
 * Tells whether the listbox line contains a contact or not.
 * Does not check that the aListboxIndex is within range of listbox.
 * @param aListboxIndex The position in the listbox to be checked.
 */
TBool CPbk2NamesListReadyState::IsContactAtListboxIndex( TInt aListboxIndex ) const
    {
    // There can be command items at the top of the list.
    // The command items are not contacts.
    const TInt enabledCommandCount = CommandItemCount();
    return aListboxIndex >= enabledCommandCount;
    }

void CPbk2NamesListReadyState::AllowCommandsToShowThemselves( TBool aVisible ) 
    {
    if ( aVisible )
        {
        // ownership not transferred
        iListBox.SetListCommands( &iCommandItems );
        SubscribeCmdItemsVisibility();
        UpdateCommandEnabled( EPbk2CmdAddFavourites, ETrue ); //By default add favorites may be shown
        UpdateCommandEnabled( EPbk2CmdOpenMyCard, ETrue ); 
        }
    else
        {
        UpdateCommandEnabled( EPbk2CmdRcl, EFalse );           
        UpdateCommandEnabled( EPbk2CmdAddFavourites, EFalse ); 
        UpdateCommandEnabled( EPbk2CmdOpenMyCard, EFalse ); 
        UnsubscribeCmdItemsVisibility();        
        iListBox.SetListCommands( NULL );        
        }
    }

void CPbk2NamesListReadyState::UpdateCommandEnabled( TInt aCommandId, TBool aEnabled )
    {
    for ( TInt i = 0; i < iCommandItems.Count(); i++ )
        {
        if ( iCommandItems[i]->CommandId() == aCommandId )
            {
            iCommandItems[i]->SetEnabled( aEnabled );
            break;
            }
        }
    }

TBool CPbk2NamesListReadyState::IsCommandEnabled(TInt aCommandId) const
    {
    TBool res = EFalse;
    for ( TInt i = 0; i < iCommandItems.Count(); i++ )
        {
        if ( iCommandItems[i]->CommandId() == aCommandId )
            {
            res = iCommandItems[i]->IsEnabled();
            break;
            }
        }
    return res;
    }

//
// Search for a command in iCommandItems and returns its index
//
TInt CPbk2NamesListReadyState::FindCommand(TInt aCommandId) const
    {
    TInt result = KErrNotFound;
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        if ( iCommandItems[i]->CommandId() == aCommandId )
            {
            result = i;
            break;
            }

        }
    return result;
    }

TInt CPbk2NamesListReadyState::CalculateListboxIndex(TInt aCommandIndex) const
    {
    //TInt num = 0;
    TInt num(KErrNotFound); 
    for ( TInt i = 0; i <= aCommandIndex; ++i )  
        {
        if(iCommandItems[i]->IsEnabled())
            {
            ++num;
            }
        }
    return num;
    }


void CPbk2NamesListReadyState::UnmarkCommands() const
    {
    for ( TInt i = 0; i < CommandItemCount(); i++ )
        {
        iListBox.View()->DeselectItem( i );
        }
    }

TInt CPbk2NamesListReadyState::EnabledCommandCount() const
    {
    TInt result = 0;
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        if(iCommandItems[i]->IsEnabled())
            {
            result++;
            }
        }
    return result;    
    }

//  End of File
