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
* Description:  Phonebook 2 UI control substitute.
*
*/


// INCLUDE FILES
#include <CPbk2ContactUiControlSubstitute.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkBaseContact.h>

// System includes
#include <e32base.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KOnlyOneContactStore = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::CPbk2ContactUiControlSubstitute
// --------------------------------------------------------------------------
//
inline CPbk2ContactUiControlSubstitute::CPbk2ContactUiControlSubstitute() :
    iFocusedFieldIndex( KErrNotFound )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::~CPbk2ContactUiControlSubstitute
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactUiControlSubstitute::~CPbk2ContactUiControlSubstitute()
    {
    delete iBgSkinContext;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactUiControlSubstitute*
        CPbk2ContactUiControlSubstitute::NewL()
    {
    CPbk2ContactUiControlSubstitute* self =
        new ( ELeave ) CPbk2ContactUiControlSubstitute;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactUiControlSubstitute::ConstructL()
    {
    // Create background control context for skinning the background
    iBgSkinContext = CAknsBasicBackgroundControlContext::NewL
        ( KAknsIIDQsnBgAreaMain, Rect(), EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetParentUiControl
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactUiControlSubstitute::SetParentUiControl
        ( MPbk2ContactUiControl& aParentUiControl )
    {
    iParentUiControl = &aParentUiControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetSelectedStore
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactUiControlSubstitute::SetSelectedStore
        ( MVPbkContactStore& aContactStore )
    {
    iContactStore = &aContactStore;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedStoreContact
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactUiControlSubstitute::SetFocusedStoreContact
        ( const MVPbkStoreContact& aContact )
    {
    iStoreContact = &aContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedField
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactUiControlSubstitute::SetFocusedField
        ( const MVPbkBaseContactField& aContactField )
    {
    iContactField = &aContactField;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactUiControlSubstitute::SetFocusedFieldIndex
        ( TInt aFieldIndex )
    {
    iFocusedFieldIndex = aFieldIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2ContactUiControlSubstitute::ParentControl() const
    {
    return iParentUiControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlSubstitute::NumberOfContacts() const
    {
    return KOnlyOneContactStore;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact*
        CPbk2ContactUiControlSubstitute::FocusedContactL() const
    {
    return static_cast<const MVPbkBaseContact*>( iStoreContact );
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact*
        CPbk2ContactUiControlSubstitute::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact*
        CPbk2ContactUiControlSubstitute::FocusedStoreContact() const
    {
    return iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetFocusedContactL
        ( const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetFocusedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetFocusedContactL(
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlSubstitute::FocusedContactIndex() const
    {
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetFocusedContactIndexL
        ( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlSubstitute::NumberOfContactFields() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField*
        CPbk2ContactUiControlSubstitute::FocusedField() const
    {
    return iContactField;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlSubstitute::FocusedFieldIndex() const
    {
    return iFocusedFieldIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2ContactUiControlSubstitute::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2ContactUiControlSubstitute::SelectedContactsL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2ContactUiControlSubstitute::
        SelectedContactsOrFocusedContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
    CPbk2ContactUiControlSubstitute::SelectedContactsIteratorL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2ContactUiControlSubstitute::SelectedContactStoresL() const
    {
    CArrayPtr<MVPbkContactStore>* array =
        new CArrayPtrFlat<MVPbkContactStore>( KOnlyOneContactStore );
    CleanupStack::PushL( array );
    array->AppendL( iContactStore );
    CleanupStack::Pop( array );
    return array;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::ClearMarks()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetSelectedContactL
        ( TInt /*aIndex*/, TBool /*aSelected*/ )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Not supported
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Not supported
    }

TInt CPbk2ContactUiControlSubstitute::CommandItemCount() const
	{
	return 0; // No command items.
	}

const MPbk2UiControlCmdItem& CPbk2ContactUiControlSubstitute::CommandItemAt(
	TInt /*aIndex*/ ) const
	{
	// There are no command items in this control
	__ASSERT_ALWAYS( EFalse, User::Panic( _L("Pbk2"), 5)); //TODO
	MPbk2UiControlCmdItem* item = NULL; // For compiler only
	return *item; // For compiler only
	}
	
const MPbk2UiControlCmdItem* CPbk2ContactUiControlSubstitute::FocusedCommandItem() const
	{
	// No command items in this control.
	return NULL;
	}

void CPbk2ContactUiControlSubstitute::DeleteCommandItemL( TInt /*aIndex*/ )
	{
	// Ignore
	}

void CPbk2ContactUiControlSubstitute::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{
	// Ignore
	}	

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ ) const
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::ProcessCommandL
        ( TInt /*aCommandId*/ ) const
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::UpdateAfterCommandExecution()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlSubstitute::GetMenuFilteringFlagsL() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2ContactUiControlSubstitute::ControlStateL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::RestoreControlStateL
        ( CPbk2ViewState* /*aState*/ )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactUiControlSubstitute::FindTextL()
    {
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::ShowThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::HideThumbnail()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetBlank( TBool /*aBlank*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::RegisterCommand(
        MPbk2Command* /*aCommand*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::SetTextL(
        const TDesC& /*aText*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactUiControlSubstitute::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }    

// --------------------------------------------------------------------------
// CPbk2ContactUiControlSubstitute::Draw
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlSubstitute::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    // Set the layout rectangle for the skin bitmap
    iBgSkinContext->SetRect( aRect );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgSkinContext, this, gc, aRect,
        KAknsDrawParamNoClearUnderImage );
    }

//  End of File
