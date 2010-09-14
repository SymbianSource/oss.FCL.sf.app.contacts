/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 own numbers control.
*
*/


#include "CPsu2OwnNumberControl.h"

// From Phonebook 2
#include "CPsu2OwnNumberListBoxModel.h"
#include "Pbk2USIMUIUID.h"
#include <pbk2usimuires.rsg>
#include <Pbk2USimUI.hrh>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconFactory.h>
#include <TPbk2IconId.h>

// From VirtualPhonebook
#include <MVPbkViewContact.h>
#include <MVPbkContactViewBase.h>

// System includes
#include <aknlists.h>
#include <barsread.h>
#include <AknIconArray.h>

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::CPsu2OwnNumberControl
// --------------------------------------------------------------------------
//
CPsu2OwnNumberControl::CPsu2OwnNumberControl(
        MVPbkContactViewBase& aView ): 
    iBaseView( &aView )
    {
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::~CPsu2OwnNumberControl
// --------------------------------------------------------------------------
//
CPsu2OwnNumberControl::~CPsu2OwnNumberControl()
    {
    delete iListBox;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberControl::ConstructL
        ( MVPbkContactViewBase& aView, CCoeControl* aContainer )
    {
    SetContainerWindowL(*aContainer);
    iListBox = new (ELeave) CAknDoubleGraphicStyleListBox;
    iListBox->ConstructL( this, 0 );

    CPsu2OwnNumberListBoxModel* model = 
        CPsu2OwnNumberListBoxModel::NewL( 
            aView,
            *aContainer );
    iListBox->Model()->SetItemTextArray(model);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);

    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );
    iListBox->SetContainerWindowL(*this );

    CPbk2IconInfoContainer* iconInfoContainer =
        CPbk2IconInfoContainer::NewL(R_PSU2_OWN_NUMBER_ICON_INFO_ARRAY);
    CleanupStack::PushL(iconInfoContainer);
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( *iconInfoContainer );
    // Set icon array, use granularity of 1
    CAknIconArray* iconArray = new(ELeave) CAknIconArray(1);
    CleanupStack::PushL(iconArray);
    // The only icon in the own number list is a phone icon
    TPbk2IconId phoneIconId( TUid::Uid(KPbk2USIMExtensionImplementationUID), 
        EPsu2qgn_prop_nrtyp_phone );
    CGulIcon* icon = factory->CreateIconLC( phoneIconId );
    iconArray->AppendL( icon );
    CleanupStack::Pop(2); // icon, iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    CleanupStack::PopAndDestroy( 2 ); // factory, iconInfoContainer
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::NewL
// --------------------------------------------------------------------------
//
CPsu2OwnNumberControl* CPsu2OwnNumberControl::NewL
        ( MVPbkContactViewBase& aView, CCoeControl* aContainer )
    {
    CPsu2OwnNumberControl* self = new(ELeave)CPsu2OwnNumberControl( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aView, aContainer );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPsu2OwnNumberControl::OfferKeyEventL
        (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    // Forward all key events to the list box
    TKeyResponse response = iListBox->OfferKeyEventL(aKeyEvent,aType);
    
    // Ignore Send Key up and down events to prevent Dialer appearance
    // on top of Phonebook application.
    if ( response == EKeyWasNotConsumed )
        {
        if ( ( aType == EEventKeyDown || aType == EEventKeyUp )
            && aKeyEvent.iScanCode == EStdKeyYes )
            {
            response = EKeyWasConsumed;
            }
        }
    return response;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SizeChanged
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberControl::SizeChanged()
    {
    iListBox->SetRect(Rect());
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumberControl::CountComponentControls() const
    {
    return (iListBox ? 1 : 0);
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPsu2OwnNumberControl::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    // Return the one and only child control
    return iListBox;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusChanged
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::FocusChanged( TDrawNow aDrawNow )
    {
    iListBox->SetFocus( IsFocused(), aDrawNow );
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPsu2OwnNumberControl::ParentControl() const
    {
    // Do nothing
    return NULL;
    }
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumberControl::NumberOfContacts() const
    {
    return iListBox->Model()->NumberOfItems();
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPsu2OwnNumberControl::FocusedContactL() const
    {
    return FocusedViewContactL();
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedViewContactL
// --------------------------------------------------------------------------
//    
const MVPbkViewContact* CPsu2OwnNumberControl::FocusedViewContactL() const
    {
    const MVPbkViewContact* contact = NULL;
    
    const TInt focusIndex = iListBox->CurrentItemIndex();
    if ( focusIndex >= 0 )
        {
        contact = &iBaseView->ContactAtL( focusIndex );
        }

    return contact;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedStoreContact
// --------------------------------------------------------------------------
//    
const MVPbkStoreContact* CPsu2OwnNumberControl::FocusedStoreContact() const
    {
    return NULL;    
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetFocusedContactL
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::SetFocusedContactL(
        const MVPbkBaseContact& /*aContact*/ )
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetFocusedContactL
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::SetFocusedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetFocusedContactL
// --------------------------------------------------------------------------
//        
void CPsu2OwnNumberControl::SetFocusedContactL(
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedContactIndex
// --------------------------------------------------------------------------
//    
TInt CPsu2OwnNumberControl::FocusedContactIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::SetFocusedContactIndexL(
        TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::NumberOfContactFields
// --------------------------------------------------------------------------
//    
TInt CPsu2OwnNumberControl::NumberOfContactFields() const
    {
    // Do nothing
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedField
// --------------------------------------------------------------------------
//    
const MVPbkBaseContactField* CPsu2OwnNumberControl::FocusedField() const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FocusedFieldIndex
// --------------------------------------------------------------------------
//    
TInt CPsu2OwnNumberControl::FocusedFieldIndex() const
    {
    // Do nothing
    return KErrNotFound;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::SetFocusedFieldIndex(
        TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ContactsMarked
// --------------------------------------------------------------------------
//    
TBool CPsu2OwnNumberControl::ContactsMarked() const
    {
    // Do nothing
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPsu2OwnNumberControl::SelectedContactsL() const
    {
    // Do nothing
    return NULL;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//    
MVPbkContactLinkArray* 
CPsu2OwnNumberControl::SelectedContactsOrFocusedContactL() const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator* 
CPsu2OwnNumberControl::SelectedContactsIteratorL() const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>* 
CPsu2OwnNumberControl::SelectedContactStoresL() const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ClearMarks
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberControl::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberControl::SetSelectedContactL(
        TInt /*aIndex*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetSelectedContactL
// --------------------------------------------------------------------------
//        
void CPsu2OwnNumberControl::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetSelectedContactL
// --------------------------------------------------------------------------
//        
void CPsu2OwnNumberControl::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Do nothing
    }

TInt CPsu2OwnNumberControl::CommandItemCount() const
	{
	return 0; // No command items in this control.
	}

const MPbk2UiControlCmdItem& CPsu2OwnNumberControl::CommandItemAt(
    TInt /*aIndex*/ ) const
	{
	// No UI list command items are supported by this control.
	MPbk2UiControlCmdItem* item = NULL; // For the compiler
	return *item; // For the compiler
	}

const MPbk2UiControlCmdItem* CPsu2OwnNumberControl::FocusedCommandItem() const
	{
	return NULL;
	}

void CPsu2OwnNumberControl::DeleteCommandItemL( TInt /*aIndex*/ )
	{
	// Do nothing
	}

void CPsu2OwnNumberControl::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{	
	//Do nothing, since there shouldn't be any command items in this state.
	}	

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::DynInitMenuPaneL
// --------------------------------------------------------------------------
//        
void CPsu2OwnNumberControl::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/ ) const
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ProcessCommandL
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::ProcessCommandL( TInt /*aCommandId*/ ) const
    {
    // Do nothing
    }
  
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberControl::UpdateAfterCommandExecution()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//    
TInt CPsu2OwnNumberControl::GetMenuFilteringFlagsL() const
    {
    // Do nothing
    return 0;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ControlStateL
// --------------------------------------------------------------------------
//    
CPbk2ViewState* CPsu2OwnNumberControl::ControlStateL() const
    {
    // Do nothing
    return NULL;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::RestoreControlStateL
// --------------------------------------------------------------------------
// 
void CPsu2OwnNumberControl::RestoreControlStateL( 
        CPbk2ViewState* /*aState*/ )
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::FindTextL
// --------------------------------------------------------------------------
//         
const TDesC& CPsu2OwnNumberControl::FindTextL()
    {
    // No find box in this control
    return KNullDesC;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ResetFindL
// --------------------------------------------------------------------------
// 
void CPsu2OwnNumberControl::ResetFindL()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ShowThumbnail
// --------------------------------------------------------------------------
// 
void CPsu2OwnNumberControl::ShowThumbnail()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::HideThumbnail
// --------------------------------------------------------------------------
// 
void CPsu2OwnNumberControl::HideThumbnail()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetBlank
// --------------------------------------------------------------------------
// 
void CPsu2OwnNumberControl::SetBlank(
        TBool /*aBlank*/ )
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::RegisterCommand
// --------------------------------------------------------------------------
//         
void CPsu2OwnNumberControl::RegisterCommand(
        MPbk2Command* /*aCommand*/ )
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::SetTextL
// --------------------------------------------------------------------------
//         
void CPsu2OwnNumberControl::SetTextL( const TDesC& aText )
    {
    iListBox->View()->SetListEmptyTextL( aText );
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberControl::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPsu2OwnNumberControl::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }     

//  End of File  
