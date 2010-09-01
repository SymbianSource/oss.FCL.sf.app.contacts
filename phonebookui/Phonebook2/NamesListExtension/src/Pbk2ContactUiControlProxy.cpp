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
* Description:  Phonebook 2 contact UI control proxy.
*
*/


// INCLUDE FILES
#include "Pbk2ContactUiControlProxy.h"

#include <MPbk2Command.h>

namespace {
    enum TPanic
        {
        ENoMPbk2ContactUiControl_CommandItemAt,
        ENoCommandItemFound_CommandItemAt
        };
        
    void Panic( TPanic aPanic )
        {
        _LIT(KPanicCat, "CPbk2ContactUiControlProxy");
        User::Panic( KPanicCat, aPanic );
        }       
}

// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::CPbk2ContactUiControlProxy
// --------------------------------------------------------------------------
//
CPbk2ContactUiControlProxy::CPbk2ContactUiControlProxy()
	{	
	}
    
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::~CPbk2ContactUiControlProxy
// --------------------------------------------------------------------------
//
CPbk2ContactUiControlProxy::~CPbk2ContactUiControlProxy()
	{
	if( iCommand )
	    {
        iCommand->ResetUiControl(*this);
        }
	}
    	
    	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetControl
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetControl( MPbk2ContactUiControl* aControl )
	{
	iControl = aControl;
	}


// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2ContactUiControlProxy::ParentControl() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->ParentControl();
	};

// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlProxy::NumberOfContacts() const 
    {
	if( !iControl )
		{
		return 0;    		
		}
    
    return iControl->NumberOfContacts();
    };

// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2ContactUiControlProxy::FocusedContactL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->FocusedContactL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2ContactUiControlProxy::FocusedViewContactL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->FocusedViewContactL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2ContactUiControlProxy::FocusedStoreContact() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->FocusedStoreContact();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetFocusedContactL(
        const MVPbkBaseContact& aContact ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetFocusedContactL( aContact );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetFocusedContactL(
        const MVPbkContactBookmark& aContactBookmark ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetFocusedContactL( aContactBookmark );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetFocusedContactL(
        const MVPbkContactLink& aContactLink ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetFocusedContactL( aContactLink );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlProxy::FocusedContactIndex() const 
	{
	if( !iControl )
		{
		return KErrNotFound;    		
		}
	
	return iControl->FocusedContactIndex();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetFocusedContactIndexL(
        TInt aIndex ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetFocusedContactIndexL( aIndex );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlProxy::NumberOfContactFields() const 
	{
	if( !iControl )
		{
		return 0;    		
		}
	
	return iControl->NumberOfContactFields();	
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CPbk2ContactUiControlProxy::FocusedField() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->FocusedField();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlProxy::FocusedFieldIndex() const 
	{
	if( !iControl )
		{
		return KErrNotFound;    		
		}
	
	return iControl->FocusedFieldIndex();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetFocusedFieldIndex(
        TInt aIndex ) 
	{
	if( !iControl )
		{
		return;    		
		}
	
	iControl->SetFocusedFieldIndex( aIndex );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2ContactUiControlProxy::ContactsMarked() const 
	{
	if( !iControl )
		{
		return EFalse;    		
		}
	
	return iControl->ContactsMarked();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2ContactUiControlProxy::SelectedContactsL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->SelectedContactsL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2ContactUiControlProxy::SelectedContactsOrFocusedContactL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->SelectedContactsOrFocusedContactL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator* CPbk2ContactUiControlProxy::SelectedContactsIteratorL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->SelectedContactsIteratorL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>* CPbk2ContactUiControlProxy::SelectedContactStoresL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->SelectedContactStoresL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::ClearMarks() 
	{
	if( !iControl )
		{
		return;    		
		}
	
	iControl->ClearMarks();
	};

// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetSelectedContactL(
        TInt aIndex,
        TBool aSelected ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetSelectedContactL( aIndex, aSelected );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetSelectedContactL(
        const MVPbkContactBookmark& aContactBookmark,
        TBool aSelected ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetSelectedContactL( aContactBookmark, aSelected );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetSelectedContactL(
        const MVPbkContactLink& aContactLink,
        TBool aSelected ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetSelectedContactL( aContactLink, aSelected );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane ) const 
    {
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->DynInitMenuPaneL( aResourceId, aMenuPane );
    };
    
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::ProcessCommandL(
        TInt aCommandId ) const 
    {
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->ProcessCommandL( aCommandId );
    };
    
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::UpdateAfterCommandExecution() 
	{
	if( iControl )
		{
		iControl->UpdateAfterCommandExecution();
		}
	
	// If iControl fails to reset the control
	if( iCommand )
        {
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
        }
    };
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactUiControlProxy::GetMenuFilteringFlagsL() const 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	return iControl->GetMenuFilteringFlagsL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2ContactUiControlProxy::ControlStateL() const 
	{
	if( !iControl )
		{
		return NULL;    		
		}
	
	return iControl->ControlStateL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::RestoreControlStateL(
        CPbk2ViewState* aState ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->RestoreControlStateL( aState );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactUiControlProxy::FindTextL() 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	return iControl->FindTextL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::ResetFindL() 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->ResetFindL();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::HideThumbnail() 
	{
	if( !iControl )
		{
		return;    		
		}
	
	iControl->HideThumbnail();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::ShowThumbnail() 
	{
	if( !iControl )
		{
		return;    		
		}
	
	iControl->ShowThumbnail();
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetBlank( TBool aBlank ) 
	{
	if( !iControl )
		{
		return;    		
		}
	
	iControl->SetBlank( aBlank );
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::RegisterCommand( MPbk2Command* aCommand ) 
	{
	if( iControl )
		{
		iControl->RegisterCommand( aCommand ); 		
		}
	iCommand = aCommand;
	};
	
// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactUiControlProxy::SetTextL( const TDesC& aText ) 
	{
	if( !iControl )
		{
		User::Leave( KErrNotReady );    		
		}
	
	iControl->SetTextL( aText );
	};


// --------------------------------------------------------------------------
// CPbk2ContactUiControlProxy::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactUiControlProxy::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }

TInt CPbk2ContactUiControlProxy::CommandItemCount() const
    {
    if( !iControl )
        {
        return 0;
        }

    TInt ret = 0;

    MPbk2ContactUiControl2* tempControl =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iControl->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    if(tempControl)
        {
        ret = tempControl->CommandItemCount();
        }
    return ret;
    }

const MPbk2UiControlCmdItem& CPbk2ContactUiControlProxy::CommandItemAt( TInt aIndex ) const
    {
    const MPbk2UiControlCmdItem* ret = NULL;

    __ASSERT_ALWAYS(iControl, Panic( ENoMPbk2ContactUiControl_CommandItemAt ) );
    
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iControl->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    if(tempControl)
        {
        ret = &(tempControl->CommandItemAt( aIndex ));	
        }	
    __ASSERT_ALWAYS(ret, Panic( ENoCommandItemFound_CommandItemAt ) );

    return *ret;
    }

const MPbk2UiControlCmdItem* CPbk2ContactUiControlProxy::FocusedCommandItem() const
    {
    if( !iControl )
        {
        return NULL;
        }

    const MPbk2UiControlCmdItem* ret = NULL;

    MPbk2ContactUiControl2* tempControl =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iControl->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    if(tempControl)
        {
        ret = tempControl->FocusedCommandItem();
    	}
    return ret;
    }

void CPbk2ContactUiControlProxy::DeleteCommandItemL( TInt aIndex )
    {
    if( !iControl )
        {
        return;   
        }
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iControl->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    if(tempControl)
        {
        return tempControl->DeleteCommandItemL( aIndex );
      }
   }

void CPbk2ContactUiControlProxy::AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex)
    {
    if( !iControl )
        {
        return;   
        }
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iControl->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    if(tempControl)
        {
        return tempControl->AddCommandItemL( aCommand, aIndex );
        }
    }    

//  End of File
