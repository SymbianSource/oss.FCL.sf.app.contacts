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
#include "CPbk2NamesListHiddenState.h"

// Phonebook 2
#include "CPbk2NamesListControl.h"
#include <Pbk2MenuFilteringFlags.hrh>

// Debugging headers
#include <Pbk2Debug.h>


// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KNumberOfContacts = 0;
const TInt KNumberOfControls = 0;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::CPbk2NamesListHiddenState
// --------------------------------------------------------------------------
//
inline CPbk2NamesListHiddenState::CPbk2NamesListHiddenState()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::~CPbk2NamesListHiddenState
// --------------------------------------------------------------------------
//
CPbk2NamesListHiddenState::~CPbk2NamesListHiddenState()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListHiddenState* CPbk2NamesListHiddenState::NewLC()
    {
    CPbk2NamesListHiddenState* self =
        new (ELeave) CPbk2NamesListHiddenState;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ActivateStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ActivateStateL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::DeactivateState
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::DeactivateState()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListHiddenState::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SizeChanged()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::MakeComponentsVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::MakeComponentsVisible
        ( TBool /*aVisible*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ActivateL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::Draw
        (const TRect& /*aRect*/, CWindowGc& /*aGc*/) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListHiddenState::OfferKeyEventL
        (const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::HandlePointerEventL
        ( const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::NamesListState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::NamesListState() const
    {
    return CPbk2NamesListControl::EStateHidden;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::HandleContactViewEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::HandleContactViewEventL
        ( TInt /*aEvent*/, TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::HandleCommandEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::HandleCommandEventL
        (TInt /*aEvent*/, TInt /*aListboxIndex*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::CoeControl
// --------------------------------------------------------------------------
//
CCoeControl& CPbk2NamesListHiddenState::CoeControl()
    {
    CCoeControl* ctrl = NULL;
    return const_cast<CCoeControl&>( *ctrl );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::HandleControlEventL(
        CCoeControl* /*aControl*/,
        MCoeControlObserver::TCoeEvent /*aEventType*/, TInt /*aParam */ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::TopContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListHiddenState::TopContactL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListHiddenState::ParentControl() const
    {
    // Names list control does not have a parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::NumberOfContacts() const
    {
    return KNumberOfContacts;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListHiddenState::FocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact*
        CPbk2NamesListHiddenState::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact*
        CPbk2NamesListHiddenState::FocusedStoreContact() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetFocusedContactL
        ( const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetFocusedContactL
        ( const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::FocusedContactIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField*
        CPbk2NamesListHiddenState::FocusedField() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::FocusedFieldIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListHiddenState::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListHiddenState::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2NamesListHiddenState::
        SelectedContactsOrFocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListHiddenState::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListHiddenState::SelectedContactStoresL() const
    {
    // This is not a state of the contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetSelectedContactL(
        TInt /*aIndex*/, TBool /*aSelected*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

TInt CPbk2NamesListHiddenState::CommandItemCount() const
    {
    return 0; // No command items in this state.
    }

const MPbk2UiControlCmdItem&
CPbk2NamesListHiddenState::CommandItemAt( TInt /*aIndex*/ ) const
    {
    // There are no command items in this state.
    __ASSERT_ALWAYS( EFalse, User::Panic( _L("Pbk2"), 2)); //TODO

    MPbk2UiControlCmdItem* item = NULL; /// For compiler only
    return *item; // For compiler only
    }

const MPbk2UiControlCmdItem*
CPbk2NamesListHiddenState::FocusedCommandItem() const
    {
    // There shouldn't be any command items during this state.
    return NULL;
    }

void CPbk2NamesListHiddenState::DeleteCommandItemL( TInt /*aIndex*/ )
    {
    //Do nothing, since there shouldn't be any command items in this state.
    }

void CPbk2NamesListHiddenState::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
    {   
    //Do nothing, since there shouldn't be any command items in this state.
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::DynInitMenuPaneL(
        TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListHiddenState::GetMenuFilteringFlagsL() const
    {
    return KPbk2ListContainsNoMarkedItems;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListHiddenState::ControlStateL() const
    {
    // State objects do not handle control state changes
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    // State objects do not handle control state changes
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListHiddenState::FindTextL()
    {
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetBlank(
        TBool /*aBlank*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::RegisterCommand(
        MPbk2Command* /*aCommand*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListHiddenState::SetTextL( const TDesC& /*aText*/ )
    {
    // Do nothing
    }
// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListHiddenState::ContactUiControlExtension(TUid aExtensionUid )
    {
    if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
       { 
       return static_cast<MPbk2ContactUiControl2*>( this );
       }
       
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListHiddenState::FocusedItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListHiddenState::FocusableItemPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListHiddenState::SearchFieldPointed()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::TopViewChangedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListHiddenState::TopViewChangedL( 
        MVPbkContactViewBase& /*aOldView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::TopViewUpdatedL
// --------------------------------------------------------------------------
//    
void CPbk2NamesListHiddenState::TopViewUpdatedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::BaseViewChangedL
// --------------------------------------------------------------------------
//     
void CPbk2NamesListHiddenState::BaseViewChangedL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ViewStackError
// --------------------------------------------------------------------------
//    
void CPbk2NamesListHiddenState::ViewStackError( TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListHiddenState::ContactAddedToBaseView
// --------------------------------------------------------------------------
//    
void CPbk2NamesListHiddenState::ContactAddedToBaseView(
        MVPbkContactViewBase& /*aBaseView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }    
    
//  End of File
