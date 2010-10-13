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
* Description:  Phonebook 2 names list empty state.
*
*/


// INCLUDE FILES
#include "CPbk2NamesListEmptyState.h"

// Phonebook 2
#include "CPbk2ContactViewListBox.h"
#include "CPbk2ThumbnailLoader.h"
#include "CPbk2NamesListControl.h"
#include <Pbk2MenuFilteringFlags.hrh>
#include "PmuCommands.hrh"  // For accessing EPmuCmdCascadingBackup
#include "Pbk2USimUI.hrh"   // For accessing EPsu2CmdCascadingSimMemory
#include <Pbk2Commands.hrh>                                               
#include <Pbk2InternalUID.h>
#include <pbk2doublelistboxcmditemextension.h>

// virtual phonebook
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>

// System includes
#include <eikmenup.h>
#include <aknsfld.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KNumberOfContacts = 0;
const TInt KNumberOfControls = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::CPbk2NamesListEmptyState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListEmptyState::CPbk2NamesListEmptyState(
        const CCoeControl* aParent,
        CPbk2ContactViewListBox& aListBox,
        CAknSearchField* aFindBox,
        CPbk2ThumbnailLoader& aThumbnailLoader,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems ) :   
    iParent( *aParent ),
    iListBox( aListBox ),
    iFindBox( aFindBox ),
    iThumbnailLoader( aThumbnailLoader ),
    iCommandItems( aCommandItems )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::~CPbk2NamesListEmptyState
// --------------------------------------------------------------------------
//
CPbk2NamesListEmptyState::~CPbk2NamesListEmptyState()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListEmptyState* CPbk2NamesListEmptyState::NewLC
        ( const CCoeControl* aParent, CPbk2ContactViewListBox& aListBox,
          CAknSearchField* aFindBox, CPbk2ThumbnailLoader& aThumbnailLoader,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems )  
    {
    CPbk2NamesListEmptyState* self = new ( ELeave ) CPbk2NamesListEmptyState
        ( aParent, aListBox, aFindBox, aThumbnailLoader, aCommandItems ); 
    CleanupStack::PushL( self );
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::UnsubscribeCmdItemsVisibility
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::UnsubscribeCmdItemsVisibility() const
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( NULL );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SubscribeCmdItemsVisibility
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SubscribeCmdItemsVisibility()
    {
    for ( TInt i = 0; i < iCommandItems.Count(); ++i )
        {
        iCommandItems[i]->SetVisibilityObserver( this );
        }
    }


// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ActivateStateL()
    {
    AllowCommandsToShowThemselves( ETrue );  
	
	// hide findbox since there is no contact
    if ( iFindBox && iFindBox->IsVisible() )
        {
        iFindBox->MakeVisible( EFalse );                
        }

    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::DeactivateState()
    {
    AllowCommandsToShowThemselves( EFalse );  	
	
    if ( iFindBox && !iFindBox->IsFocused() )
        {
        iFindBox->SetFocus( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListEmptyState::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return &iListBox;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SizeChanged()
    {
    const TRect rect(iParent.Rect());
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListEmptyState::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    AknLayoutUtils::LayoutControl(&iListBox, rect,
        AKN_LAYOUT_WINDOW_list_gen_pane(0));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::MakeComponentsVisible(TBool aVisible)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListEmptyState::MakeComponentsVisible(0x%x,%d)"),
        this, aVisible);

    iListBox.MakeVisible(aVisible);

   CEikScrollBar *scrollBar = iListBox.ScrollBarFrame()->GetScrollBarHandle
                                                           (CEikScrollBar::EVertical);
   if ( scrollBar )
       {
       scrollBar->MakeVisible(aVisible);
       }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ActivateL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::Draw
        (const TRect& aRect, CWindowGc& aGc) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListEmptyState::Draw(0x%x, TRect(%d,%d,%d,%d))"),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);

    // Draw the skin background
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

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListEmptyState::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse result( EKeyWasNotConsumed );
    // Offer the event to listbox
    result = iListBox.OfferKeyEventL(aKeyEvent, aType);
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HandlePointerEventL
        ( const TPointerEvent& aPointerEvent )
    {
    iListBox.HandlePointerEventL( aPointerEvent );  
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateEmpty;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HandleContactViewEventL
        ( TInt aEvent, TInt aIndex )
    {
    // need to calculate list index from contact
    // view index
    TInt listIndex = CommandItemCount() + aIndex;
    HandleListboxEventL(aEvent, listIndex, EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HandleCommandEventL
        (TInt aEvent, TInt /* aListboxIndex */)
    {
    HandleListboxEventL(aEvent, 0, ETrue );     
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HandleListboxEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HandleListboxEventL
        (TInt aEvent, TInt aListboxIndex,
		TBool /*aMantainFocus*/) 
    {
    iListBox.HandleEventL( aEvent, aListboxIndex );
    iParent.DrawDeferred();
    }
// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListEmptyState::CoeControl()
    {
    return const_cast<CCoeControl&>(iParent);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HandleControlEventL( CCoeControl* /*aControl*/,
        MCoeControlObserver::TCoeEvent /*aEventType*/, TInt /*aParam */)
    {
    // Do nothing in empty state
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListEmptyState::TopContactL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListEmptyState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::NumberOfContacts() const
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
// CPbk2NamesListEmptyState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListEmptyState::FocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListEmptyState::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2NamesListEmptyState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetFocusedContactL
        ( const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetFocusedContactL
        ( const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::FocusedContactIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetFocusedContactIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CPbk2NamesListEmptyState::FocusedField() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::FocusedFieldIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListEmptyState::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListEmptyState::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListEmptyState::SelectedContactsOrFocusedContactL() const
    {
	CVPbkContactLinkArray* array = NULL;
	// my card is showing on empty list, check if the wanted contact is it
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
						array = CVPbkContactLinkArray::NewLC();
						array->AppendL( link->CloneLC() );
						CleanupStack::Pop( 2 );	// array, link
						}
					}
				}
			}
		}
	return array;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListEmptyState::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListEmptyState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetSelectedContactL(
        TInt /*aIndex*/, TBool /*aSelected*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::CommandItemCount
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::CommandItemCount() const
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
// CPbk2NamesListEmptyState::CommandItemAt
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem&
CPbk2NamesListEmptyState::CommandItemAt( TInt aIndex ) const
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
// CPbk2NamesListEmptyState::FocusedCommandItem
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem*
CPbk2NamesListEmptyState::FocusedCommandItem() const
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
// CPbk2NamesListEmptyState::DeleteCommandItemL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::DeleteCommandItemL( TInt /*aIndex*/ )
	{
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::AddCommandItemL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
    {
    // ownership & management of iCommandItems is wasted in names list
    // control. Do nothing here
	}	

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenuPane, &iListBox);

    //No contacts visible in this state
    TInt pos; // Stores the position of the searched menu item.
            // This position is not needed or used anywhere
    
    if (aMenuPane->MenuItemExists(EAknUnmarkAll, pos))
        {
        aMenuPane->SetItemDimmed(EAknUnmarkAll, ETrue);
        }
    if (aMenuPane->MenuItemExists(EAknMarkAll, pos))
        {
        aMenuPane->SetItemDimmed(EAknMarkAll, ETrue);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::GetMenuFilteringFlagsL() const
    {
    return KPbk2MenuFilteringFlagsNone | KPbk2ListContainsNoMarkedItems;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListEmptyState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    // State objects do not handle control state changes
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListEmptyState::FindTextL()
    {
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetBlank( TBool aBlank )
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
// CPbk2NamesListEmptyState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::RegisterCommand(
        MPbk2Command* /*aCommand*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::SetTextL( const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListEmptyState::ContactUiControlExtension(TUid aExtensionUid )
    {
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        {
        return static_cast<MPbk2ContactUiControl2*>( this );
        }

    return NULL;
    }
// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListEmptyState::FocusedItemPointed()
    {
    return iListBox.FocusedItemPointed();  
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListEmptyState::FocusableItemPointed()
    {
    return iListBox.FocusableItemPointed(); 
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListEmptyState::SearchFieldPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::TopViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::TopViewChangedL(
        MVPbkContactViewBase& /*aOldView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::TopViewUpdatedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::TopViewUpdatedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::BaseViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::BaseViewChangedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ViewStackError
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ViewStackError( TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CPbk2NamesListEmptyState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::IsContactAtListboxIndex
// Tells whether the listbox line contains a contact or not.
// Does not check that the aListboxIndex is within range of listbox.
// --------------------------------------------------------------------------
//    
TBool CPbk2NamesListEmptyState::IsContactAtListboxIndex( TInt aListboxIndex ) const
    {
    // There can be command items at the top of the list.
    // The command items are not contacts.
    const TInt enabledCommandCount = CommandItemCount();
    return aListboxIndex >= enabledCommandCount;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::AllowCommandsToShowThemselves
// --------------------------------------------------------------------------
//    
void CPbk2NamesListEmptyState::AllowCommandsToShowThemselves( TBool aVisible )  
    {
    if ( aVisible )
        {        
        // ownership not transferred
        iListBox.SetListCommands( &iCommandItems );
        SubscribeCmdItemsVisibility();        
        UpdateCommandEnabled( EPbk2CmdRcl, ETrue ); 			
        UpdateCommandEnabled( EPbk2CmdAddFavourites, EFalse ); 
		//MyCard is stored separately and may also contain data         
        UpdateCommandEnabled( EPbk2CmdOpenMyCard, ETrue ); 
        }
    else
        {
        UnsubscribeCmdItemsVisibility();        
        iListBox.SetListCommands( NULL );        
        }
    }
// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::CmdItemVisibilityChanged
// --------------------------------------------------------------------------
//    
void CPbk2NamesListEmptyState::CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible ) 
    {
    TInt cmdItemIndex = FindCommand(aCmdItemId);
    TRAP_IGNORE( HandleCommandEventL(
                (aVisible ? EItemAdded : EItemRemoved),
                 cmdItemIndex) );		
    }

// --------------------------------------------------------------------------
// CPbk2NamesListEmptyState::UpdateCommandEnabled
// --------------------------------------------------------------------------
//    
void CPbk2NamesListEmptyState::UpdateCommandEnabled( TInt aCommandId, TBool aEnabled )
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
// CPbk2NamesListEmptyState::IsCommandEnabled
// --------------------------------------------------------------------------
//    
TBool CPbk2NamesListEmptyState::IsCommandEnabled(TInt aCommandId) const
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
// CPbk2NamesListEmptyState::FindCommand
// Search for a command in iCommandItems and returns its index
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListEmptyState::FindCommand(TInt aCommandId) const
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
// CPbk2NamesListEmptyState::CalculateListboxIndex
// --------------------------------------------------------------------------
//    
TInt CPbk2NamesListEmptyState::CalculateListboxIndex(TInt aCommandIndex) const
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
// CPbk2NamesListEmptyState::UnmarkCommands
// --------------------------------------------------------------------------
//        
void CPbk2NamesListEmptyState::UnmarkCommands() const
    {
    for ( TInt i = 0; i < CommandItemCount(); i++ )
        {
        iListBox.View()->DeselectItem( i );
        }
    }

//  End of File
