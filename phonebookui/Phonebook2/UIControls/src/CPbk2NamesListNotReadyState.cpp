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
#include "CPbk2NamesListNotReadyState.h"

// Phonebook 2
#include "CPbk2ContactViewListBox.h"
#include "CPbk2NamesListControl.h"
#include <Pbk2MenuFilteringFlags.hrh>
#include <pbk2uicontrols.rsg>

// System includes
#include <AknsDrawUtils.h>
#include <aknsfld.h>    // CAknSearchField
#include <StringLoader.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KNumberOfContacts = 0;
const TInt KNumberOfControls = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::CPbk2NamesListNotReadyState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListNotReadyState::CPbk2NamesListNotReadyState(
        const CCoeControl& aParent,
        CPbk2ContactViewListBox& aListBox,
        CAknSearchField* aFindBox ) :
            iParent( aParent ),
            iListBox( aListBox ),
            iFindBox( aFindBox )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::~CPbk2NamesListNotReadyState
// --------------------------------------------------------------------------
//
CPbk2NamesListNotReadyState::~CPbk2NamesListNotReadyState()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListNotReadyState* CPbk2NamesListNotReadyState::NewLC
        ( const CCoeControl* aParent, CPbk2ContactViewListBox& aListBox,
          CAknSearchField* aFindBox )
    {
    CPbk2NamesListNotReadyState* self =
        new (ELeave) CPbk2NamesListNotReadyState(
            *aParent, aListBox, aFindBox );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ActivateStateL()
    {
    // Set findbox not focused to prevent cursor blinking in list view.
    if ( iFindBox && iFindBox->IsFocused() )
        {
        iFindBox->SetFocus( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::DeactivateState()
    {
    if ( iFindBox && !iFindBox->IsFocused() )
        {
        iFindBox->SetFocus( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListNotReadyState::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return &iListBox;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SizeChanged()
    {
    const TRect rect(iParent.Rect());
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListNotReadyState::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    AknLayoutUtils::LayoutControl(&iListBox, rect,
        AKN_LAYOUT_WINDOW_list_gen_pane(0));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::MakeComponentsVisible
        ( TBool aVisible )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListNotReadyState::MakeComponentsVisible(0x%x,%d)"),
            this, aVisible);

    iListBox.MakeVisible(aVisible);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ActivateL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::Draw
        (const TRect& aRect, CWindowGc& /*aGc*/) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListNotReadyState::Draw(0x%x, TRect(%d,%d,%d,%d))"),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListNotReadyState::OfferKeyEventL
        (const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::HandlePointerEventL
        ( const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateNotReady;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::HandleContactViewEventL
        ( TInt /*aEvent*/, TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::HandleCommandEventL
        (TInt /*aEvent*/, TInt /*aListboxIndex*/)
    {
    // do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListNotReadyState::CoeControl()
    {
    return const_cast<CCoeControl&>(iParent);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::HandleControlEventL(
        CCoeControl* /*aControl*/,
        MCoeControlObserver::TCoeEvent /*aEventType*/, TInt /*aParam */ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListNotReadyState::TopContactL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListNotReadyState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::NumberOfContacts() const
    {
    return KNumberOfContacts;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListNotReadyState::FocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact*
        CPbk2NamesListNotReadyState::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact*
        CPbk2NamesListNotReadyState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetFocusedContactL
        ( const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetFocusedContactL
        ( const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::FocusedContactIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField*
        CPbk2NamesListNotReadyState::FocusedField() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::FocusedFieldIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListNotReadyState::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListNotReadyState::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListNotReadyState::
        SelectedContactsOrFocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListNotReadyState::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListNotReadyState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetSelectedContactL(
        TInt /*aIndex*/, TBool /*aSelected*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

TInt CPbk2NamesListNotReadyState::CommandItemCount() const
	{
	return 0; // No command items in this state.
	}

const MPbk2UiControlCmdItem&
CPbk2NamesListNotReadyState::CommandItemAt( TInt /*aIndex*/ ) const
	{
	// There are no command items in this state.
	__ASSERT_ALWAYS( EFalse, User::Panic( _L("Pbk2"), 2)); //TODO

	MPbk2UiControlCmdItem* item = NULL; /// For compiler only
	return *item; // For compiler only
	}

const MPbk2UiControlCmdItem*
CPbk2NamesListNotReadyState::FocusedCommandItem() const
	{
	// There shouldn't be any command items during this state.
	return NULL;
	}

void CPbk2NamesListNotReadyState::DeleteCommandItemL( TInt /*aIndex*/ )
	{
	//Do nothing, since there shouldn't be any command items in this state.
	}

void CPbk2NamesListNotReadyState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{	
	//Do nothing, since there shouldn't be any command items in this state.
	}

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenuPane, &iListBox);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListNotReadyState::GetMenuFilteringFlagsL() const
    {
    return KPbk2ListContainsNoMarkedItems;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListNotReadyState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    // State objects do not handle control state changes
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListNotReadyState::FindTextL()
    {
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetBlank(
        TBool aBlank )
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
// CPbk2NamesListNotReadyState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::RegisterCommand(
        MPbk2Command* /*aCommand*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListNotReadyState::SetTextL( const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }
// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListNotReadyState::ContactUiControlExtension(TUid aExtensionUid )
	{
	 if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
	}

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListNotReadyState::FocusedItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListNotReadyState::FocusableItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListNotReadyState::SearchFieldPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::TopViewChangedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListNotReadyState::TopViewChangedL( 
        MVPbkContactViewBase& /*aOldView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::TopViewUpdatedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListNotReadyState::TopViewUpdatedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::BaseViewChangedL
// --------------------------------------------------------------------------
//     
void CPbk2NamesListNotReadyState::BaseViewChangedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ViewStackError
// --------------------------------------------------------------------------
//    
void CPbk2NamesListNotReadyState::ViewStackError( TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListNotReadyState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//    
void CPbk2NamesListNotReadyState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }    
    
//  End of File
