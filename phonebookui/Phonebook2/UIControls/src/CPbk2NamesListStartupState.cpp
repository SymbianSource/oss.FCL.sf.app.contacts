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
* Description:  Phonebook 2 names list not ready state.
*
*/


// INCLUDE FILES
#include "CPbk2NamesListStartupState.h"

// Phonebook 2
#include "CPbk2ContactViewListBox.h"
#include "CPbk2NamesListControl.h"
#include <Pbk2MenuFilteringFlags.hrh>

// System includes
#include <AknsDrawUtils.h>
#include <aknsfld.h>    // CAknSearchField

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KNumberOfContacts = 0;
const TInt KNumberOfControls = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::CPbk2NamesListStartupState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListStartupState::CPbk2NamesListStartupState(
        const CCoeControl& aParent,
        CPbk2ContactViewListBox& aListBox,
        CAknSearchField* aFindBox ) :
            iParent(aParent),
            iListBox(aListBox),
            iFindBox( aFindBox )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::~CPbk2NamesListStartupState
// --------------------------------------------------------------------------
//
CPbk2NamesListStartupState::~CPbk2NamesListStartupState()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListStartupState* CPbk2NamesListStartupState::NewLC
        ( const CCoeControl* aParent, CPbk2ContactViewListBox& aListBox,
          CAknSearchField* aFindBox )
    {
    CPbk2NamesListStartupState* self =
        new (ELeave) CPbk2NamesListStartupState(
                            *aParent, aListBox, aFindBox );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ActivateStateL()
    {
    // Set findbox not focused to prevent cursor blinking in list view.
    if ( iFindBox && iFindBox->IsFocused() )
        {
        iFindBox->SetFocus( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::DeactivateState()
    {
    if ( iFindBox && !iFindBox->IsFocused() )
        {
        iFindBox->SetFocus( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListStartupState::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return &iListBox;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SizeChanged()
    {
    const TRect rect(iParent.Rect());
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListStartupState::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    AknLayoutUtils::LayoutControl(&iListBox, rect,
        AKN_LAYOUT_WINDOW_list_gen_pane(0));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::MakeComponentsVisible
        ( TBool aVisible )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListStartupState::MakeComponentsVisible(0x%x,%d)"),
            this, aVisible);

    iListBox.MakeVisible(aVisible);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ActivateL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::Draw
        (const TRect& aRect, CWindowGc& /*aGc*/) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListStartupState::Draw(0x%x, TRect(%d,%d,%d,%d))"),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListStartupState::OfferKeyEventL
        (const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::HandlePointerEventL
        ( const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateStartup;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::HandleContactViewEventL
        ( TInt /*aEvent*/, TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::HandleCommandEventL
        (TInt /*aEvent*/, TInt /*aListboxIndex*/)
    {
    // Do nothing
    }


// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListStartupState::CoeControl()
    {
    return const_cast<CCoeControl&>(iParent);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::HandleControlEventL(
        CCoeControl* /*aControl*/,
        MCoeControlObserver::TCoeEvent /*aEventType*/, TInt /*aParam*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListStartupState::TopContactL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListStartupState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::NumberOfContacts() const
    {
    return KNumberOfContacts;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListStartupState::FocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact*
        CPbk2NamesListStartupState::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact*
        CPbk2NamesListStartupState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetFocusedContactL
        ( const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetFocusedContactL
        ( const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::FocusedContactIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField*
        CPbk2NamesListStartupState::FocusedField() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::FocusedFieldIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListStartupState::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListStartupState::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListStartupState::SelectedContactsOrFocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListStartupState::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListStartupState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetSelectedContactL(
        TInt /*aIndex*/, TBool /*aSelected*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

TInt CPbk2NamesListStartupState::CommandItemCount() const
	{
	return 0; // No command items in this state.
	}

const MPbk2UiControlCmdItem& CPbk2NamesListStartupState::CommandItemAt( TInt /*aIndex*/ ) const
	{
	// There are no command items in this state.
	__ASSERT_ALWAYS( EFalse, User::Panic( _L("Pbk2"), 1)); //TODO

	MPbk2UiControlCmdItem* item = NULL; /// For compiler only
	return *item; // For compiler only
	}

const MPbk2UiControlCmdItem* CPbk2NamesListStartupState::FocusedCommandItem() const
	{
	// TODO
	return NULL;
	}

void CPbk2NamesListStartupState::DeleteCommandItemL( TInt /*aIndex*/ )
	{
	// do nothing
	}

void CPbk2NamesListStartupState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{	
	// do nothing
	}	

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenuPane, &iListBox);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStartupState::GetMenuFilteringFlagsL() const
    {
    return KPbk2ListContainsNoMarkedItems;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListStartupState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    // State objects do not handle control state changes
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListStartupState::FindTextL()
    {
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetBlank( TBool aBlank )
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
// CPbk2NamesListStartupState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::RegisterCommand(
        MPbk2Command* /*aCommand*/)
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStartupState::SetTextL( const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }
// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListStartupState::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListStartupState::FocusedItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListStartupState::FocusableItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListStartupState::SearchFieldPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::TopViewChangedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListStartupState::TopViewChangedL( 
        MVPbkContactViewBase& /*aOldView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::TopViewUpdatedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListStartupState::TopViewUpdatedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::BaseViewChangedL
// --------------------------------------------------------------------------
//     
void CPbk2NamesListStartupState::BaseViewChangedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ViewStackError
// --------------------------------------------------------------------------
//    
void CPbk2NamesListStartupState::ViewStackError( TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStartupState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//    
void CPbk2NamesListStartupState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

//  End of File
