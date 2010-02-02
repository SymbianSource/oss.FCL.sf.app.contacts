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
* Description:  Phonebook 2 names list filtered state.
*
*/


// INCLUDE FILES
#include "CPbk2NamesListFilteredState.h"

// Phonebook 2
#include "CPbk2ContactViewListBox.h"
#include "MPbk2FilteredViewStack.h"
#include "MPbk2UiControlEventSender.h"
#include "CPbk2NamesListControl.h"
#include "CPbk2FindStringSplitter.h"
#include <MPbk2ControlObserver.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <MPbk2Command.h>
#include "CPbk2AdaptiveSearchGridFiller.h"
#include "CPbk2PredictiveSearchFilter.h"
#include "TPbk2FindTextUtil.h"
#include "PmuCommands.hrh"  // For accessing EPmuCmdCascadingBackup
#include "Pbk2USimUI.hrh"   // For accessing EPsu2CmdCascadingSimMemory
#include <Pbk2Commands.hrh>

// System includes
#include <eikmenup.h>
#include <barsread.h>   // TResourceReader
#include <aknsfld.h>    // CAknSearchField

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KNumberOfContacts = 0;
const TInt KNumberOfControls = 2; // listbox and findbox

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicLogic_FindTextL = 1,
    EPanicLogic_ComponentControl,
    EPreCond_UpdateFindResultL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2NamesListFilteredState");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CPbk2NamesListFilteredState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListFilteredState::CPbk2NamesListFilteredState(
        const CCoeControl* aParent,
        CPbk2ContactViewListBox& aListBox,
        MPbk2FilteredViewStack& aViewStack,
        CAknSearchField* aFindBox,
        MPbk2UiControlEventSender& aEventSender,               
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2PredictiveSearchFilter& aSearchFilter,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems ) :   
    iParent(*aParent),
    iListBox(aListBox),
    iViewStack(aViewStack),
    iFindBox(aFindBox),
    iEventSender( aEventSender ),                                 
    iNameFormatter( aNameFormatter ),
    iSearchFilter( aSearchFilter ),
    iCommandItems( aCommandItems )	
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::~CPbk2NamesListFilteredState
// --------------------------------------------------------------------------
//
CPbk2NamesListFilteredState::~CPbk2NamesListFilteredState()
    {
    if (iCommand)
        {
        // inform the command that the control is deleted
        iCommand->ResetUiControl(*this);
        }
    delete iFindTextBuf;
    delete iStringArray;
    delete iStringSplitter;
    delete iAdaptiveSearchGridFiller;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListFilteredState* CPbk2NamesListFilteredState::NewLC
        ( const CCoeControl* aParent, CPbk2ContactViewListBox& aListBox,
        MPbk2FilteredViewStack& aViewStack, CAknSearchField* aFindBox,
          MPbk2UiControlEventSender& aEventSender,            
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2PredictiveSearchFilter& aSearchFilter,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems ) 
    {
    CPbk2NamesListFilteredState* self =
        new ( ELeave ) CPbk2NamesListFilteredState
            ( aParent, aListBox, aViewStack, aFindBox, aEventSender, aNameFormatter, aSearchFilter, aCommandItems ); 
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ConstructL()
    {
    iStringSplitter = CPbk2FindStringSplitter::NewL( iNameFormatter );
    if( iFindBox )
    {
    iAdaptiveSearchGridFiller = CPbk2AdaptiveSearchGridFiller::NewL( *iFindBox, iNameFormatter );
    }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::UnsubscribeCmdItemsVisibility
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::UnsubscribeCmdItemsVisibility() const
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( NULL );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SubscribeCmdItemsVisibility
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SubscribeCmdItemsVisibility()
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( this );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ActivateStateL()
    {
    AllowCommandsToShowThemselves( ETrue );  
	UpdateAdaptiveSearchGridL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::DeactivateState()
    {
    AllowCommandsToShowThemselves( EFalse );  
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::CountComponentControls() const
    {
    TInt numberOfControls( KNumberOfControls );
    if ( !iFindBox )
        {
        --numberOfControls;
        }
    return numberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListFilteredState::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
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
// CPbk2NamesListFilteredState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SizeChanged()
    {
    const TRect rect(iParent.Rect());
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListFilteredState::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    if ( iFindBox )
        {
        AknLayoutUtils::LayoutControl(&iListBox, rect,
            AKN_LAYOUT_WINDOW_list_gen_pane(1));

        AknLayoutUtils::LayoutControl( iFindBox, rect,
            AKN_LAYOUT_WINDOW_find_pane);

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
// CPbk2NamesListFilteredState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::MakeComponentsVisible(TBool aVisible)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListFilteredState::MakeComponentsVisible(0x%x,%d)"),
        this, aVisible);

    iListBox.MakeVisible(aVisible);
    if ( iFindBox )
        {
        if ( (iFindBox->IsVisible()&&!aVisible) || 
                (!iFindBox->IsVisible()&&aVisible) )
            {
            iFindBox->MakeVisible(aVisible);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ActivateL()
    {
    if ( iFindBox )
        {
        CEikEdwin& findEditor = iFindBox->Editor();
        findEditor.SetCursorPosL(findEditor.TextLength(), EFalse);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::Draw
        (const TRect& aRect, CWindowGc& /*aGc*/) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListFilteredState::Draw(0x%x, TRect(%d,%d,%d,%d))"),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListFilteredState::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    if ( iFindBox )
        {
        //If the FindBox is locked then dont offer the event to the findbox 
        if (!iSearchFilter.IsLocked())
            {            
            // Offer the key event first to findbox
            result = iFindBox->OfferKeyEventL( aKeyEvent, aType );
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
    if (result == EKeyWasNotConsumed)
        {
        result = iListBox.OfferKeyEventL(aKeyEvent, aType);
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandlePointerEventL
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
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateFiltered;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandleContactViewEventL
        ( TInt aEvent, TInt aIndex )
    {
    // need to calculate list index from contact    
    // view index
    TInt listIndex = CommandItemCount() + aIndex;
    HandleListboxEventL(aEvent, listIndex, EFalse );
    /// iListBox.HandleEventL( aEvent, aIndex );    
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandleCommandEventL  
        (TInt aEvent, TInt /* aCommandIndex */)
    {
    HandleListboxEventL(aEvent, 0, ETrue );     
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandleListboxEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandleListboxEventL
        (TInt aEvent, TInt aListboxIndex
		, TBool /*aMantainFocus*/)  
    {
    iListBox.HandleEventL( aEvent, aListboxIndex );
    iParent.DrawDeferred();
    HandleFocusChangeL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListFilteredState::CoeControl()
    {
    return const_cast<CCoeControl&>(iParent);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandleControlEventL(
        CCoeControl* aControl,
        MCoeControlObserver::TCoeEvent aEventType, TInt /*aParam*/ )
    {
    if ( aEventType == MCoeControlObserver::EEventStateChanged )
        {
        if ( aControl == iFindBox )
            {
            UpdateFindResultL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListFilteredState::TopContactL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListFilteredState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::NumberOfContacts() const
    {
    // The listbox can also have command items.
    TInt result = iListBox.NumberOfItems() - CommandItemCount();
    if ( result < 0 )
    	{
    	result = KNumberOfContacts; // No contacts 
    	}
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListFilteredState::FocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact*
        CPbk2NamesListFilteredState::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact*
        CPbk2NamesListFilteredState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetFocusedContactL
        ( const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetFocusedContactL
        ( const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::FocusedContactIndex() const
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
// CPbk2NamesListFilteredState::SetFocusedContactIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField*
        CPbk2NamesListFilteredState::FocusedField() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::FocusedFieldIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListFilteredState::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListFilteredState::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListFilteredState::
        SelectedContactsOrFocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListFilteredState::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListFilteredState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetSelectedContactL(
        TInt /*aIndex*/, TBool /*aSelected*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CommandItemCount
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::CommandItemCount() const
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

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CommandItemAt
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem& 
    CPbk2NamesListFilteredState::CommandItemAt( TInt aIndex ) const
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

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedCommandItem
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem*
    CPbk2NamesListFilteredState::FocusedCommandItem() const
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

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::DeleteCommandItemL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::DeleteCommandItemL( TInt /*aIndex*/ )
	{
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
	}

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::AddCommandItemL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{	
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
	}	


// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenuPane, &iListBox);
      
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::UpdateAfterCommandExecution()
    {
    if( iCommand )
        {
        // reset command pointer, since command has finished
        iCommand->ResetUiControl( *this );
        iCommand = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::GetMenuFilteringFlagsL() const
    {
    // Filtered state is used when the search string does not match
    // to any of the contacts.
    return KPbk2ListContainsNoMarkedItems;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListFilteredState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    // State objects do not handle control state changes
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListFilteredState::FindTextL()
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
// CPbk2NamesListFilteredState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ResetFindL()
    {
    if ( iFindBox && iFindBox->TextLength() > 0 )
        {
        iSearchFilter.ResetL();
        iFindBox->ResetL();
        iFindBox->DrawDeferred();
        iViewStack.Reset();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetBlank( TBool aBlank )
    {
    if (!aBlank)
        {
        // Redraw
        iParent.DrawDeferred();
        TRAPD( error, iListBox.UpdateScrollBarsL() );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::RegisterCommand(
        MPbk2Command* aCommand)
    {
    iCommand = aCommand;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::SetTextL( const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListFilteredState::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::UpdateFindResultL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::UpdateFindResultL()
    {
    TBuf <KSearchFieldLength> findPaneTxt( FindTextL() );
    
    //First Forward the Find Pane text to the Search Filter before sending
    //the info to the viewstack
    if ( iSearchFilter.StartFindPaneInlineEditL( findPaneTxt ) )
        {
        // If the control is in empty filtered state then the view stack must
        // contain at least one filtered view. Otherwise either the control
        // state is wrong or the stack state is not up to date.
        __ASSERT_DEBUG( iViewStack.Level() > 0,
            Panic( EPreCond_UpdateFindResultL ) );
    
        // Extract the find text into array
        MDesCArray* temp = iStringSplitter->SplitTextIntoArrayL( findPaneTxt );    
        delete iStringArray;    
        iStringArray = temp;
            
    		iViewStack.UpdateFilterL( *iStringArray, NULL, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListFilteredState::FocusedItemPointed()
    {
    return iListBox.FocusedItemPointed();  
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListFilteredState::FocusableItemPointed()
    {
    return iListBox.FocusableItemPointed(); 
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListFilteredState::SearchFieldPointed()
    {
    return iSearchFieldPointed;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::TopViewChangedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::TopViewChangedL( 
        MVPbkContactViewBase& /*aOldView*/ )
    {
    if ( iSearchFilter.IsPredictiveActivated() )
        {
        iSearchFilter.CommitFindPaneTextL( iViewStack, iNameFormatter );        
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::TopViewUpdatedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::TopViewUpdatedL()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::BaseViewChangedL
// --------------------------------------------------------------------------
//     
void CPbk2NamesListFilteredState::BaseViewChangedL()
    {
    // Do nothing
    }    

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ViewStackError
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::ViewStackError( TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }    


// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::HandleFocusChangeL
// --------------------------------------------------------------------------
//
void CPbk2NamesListFilteredState::HandleFocusChangeL()
    {
    iEventSender.SendEventToObserversL( 
        TPbk2ControlEvent::TPbk2ControlEvent::EControlFocusChanged );    
    }

    
void CPbk2NamesListFilteredState::UpdateAdaptiveSearchGridL()
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
	
    iAdaptiveSearchGridFiller->StopFilling();
    iAdaptiveSearchGridFiller->ClearCache();
    iAdaptiveSearchGridFiller->StartFilling( iViewStack.BaseView(), FindTextL() );
	}

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::IsContactAtListboxIndex
// Tells whether the listbox line contains a contact or not.
// Does not check that the aListboxIndex is within range of listbox.
// --------------------------------------------------------------------------
//    
TBool CPbk2NamesListFilteredState::IsContactAtListboxIndex( TInt aListboxIndex ) const
    {
    // There can be command items at the top of the list.
    // The command items are not contacts.
    const TInt enabledCommandCount = CommandItemCount();
    return aListboxIndex >= enabledCommandCount;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::AllowCommandsToShowThemselves
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::AllowCommandsToShowThemselves( TBool aVisible )  
    {
    if ( aVisible )
        {        
        // ownership not transferred
        iListBox.SetListCommands( &iCommandItems );
        SubscribeCmdItemsVisibility();        
        UpdateCommandEnabled( EPbk2CmdRcl, ETrue ); 			
        UpdateCommandEnabled( EPbk2CmdAddFavourites, EFalse ); 
        UpdateCommandEnabled( EPbk2CmdOpenMyCard, EFalse );
        }
    else
        {
        UnsubscribeCmdItemsVisibility();        
        iListBox.SetListCommands( NULL );        
        }
    }
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CmdItemVisibilityChanged
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible ) 
    {
    TInt cmdItemIndex = FindCommand(aCmdItemId);
    TRAP_IGNORE( HandleCommandEventL(
                (aVisible ? EItemAdded : EItemRemoved),
                 cmdItemIndex) );		
    }

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::UpdateCommandEnabled
// --------------------------------------------------------------------------
//    
void CPbk2NamesListFilteredState::UpdateCommandEnabled( TInt aCommandId, TBool aEnabled )
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
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::IsCommandEnabled
// --------------------------------------------------------------------------
//    
TBool CPbk2NamesListFilteredState::IsCommandEnabled(TInt aCommandId) const
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

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::FindCommand
// Search for a command in iCommandItems and returns its index
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListFilteredState::FindCommand(TInt aCommandId) const
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

// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::CalculateListboxIndex
// --------------------------------------------------------------------------
//    
TInt CPbk2NamesListFilteredState::CalculateListboxIndex(TInt aCommandIndex) const
    {
    TInt num(KErrNotFound); 
    for ( TInt i = 0; i < aCommandIndex; ++i )
        {
        if(iCommandItems[i]->IsEnabled())
            {
            ++num;            
            }
        }
    return num;
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListFilteredState::UnmarkCommands
// --------------------------------------------------------------------------
//        
void CPbk2NamesListFilteredState::UnmarkCommands() const
    {
    for ( TInt i = 0; i < CommandItemCount(); i++ )
        {
        iListBox.View()->DeselectItem( i );
        }
    }

//  End of File
