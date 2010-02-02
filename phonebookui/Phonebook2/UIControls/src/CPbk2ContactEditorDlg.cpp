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
* Description:  Phonebook2 contact editor dialog.
*
*/


// INCLUDE FILES
#include <CPbk2ContactEditorDlg.h>

// Phonebook 2
#include "CPbk2ContactEditorDlgImpl.h"
#include "Pbk2ContactEditorStrategyFactory.h"
#include "MPbk2ContactEditorStrategy.h"
#include <CPbk2ContactRelocator.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ContactEditorEventObserver.h>
#include <MPbk2EditedContactObserver.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2StoreSpecificFieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>
#include <barsread.h>


// System includes
#include <coemain.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    ENullPointer,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ContactEditorDlg");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::CPbk2ContactEditorDlg
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlg::CPbk2ContactEditorDlg
    ( TPbk2ContactEditorParams& aParams,
      MPbk2EditedContactObserver& aContactObserver,
      MPbk2ApplicationServices* aAppServices,
      TInt aEditorFieldsId ) :
        iParams( aParams ),
        iContactObserver( aContactObserver ),
        iAddItemFieldTypeResourceId( KErrNotFound ),
        iAppServices( aAppServices ),
        iEditorFieldsId( aEditorFieldsId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::~CPbk2ContactEditorDlg
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlg::~CPbk2ContactEditorDlg()
    {
	delete iTitleText;
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    delete iContact;
    delete iSpecificFieldProperties;
    delete iFieldProperties;
    delete iEditorStrategy;
    delete iContactRelocator;
    delete iStoreContact;
    delete iAddItemFieldTypeXspName;
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactEditorDlg* CPbk2ContactEditorDlg::NewL(
        TPbk2ContactEditorParams& aParams, MVPbkStoreContact* aContact,
        MPbk2EditedContactObserver& aContactObserver)
    {
    CPbk2ContactEditorDlg* self =
        new(ELeave) CPbk2ContactEditorDlg(aParams, aContactObserver);
    CleanupStack::PushL(self);
    self->ConstructL( aContact );
    CleanupStack::Pop(self);
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactEditorDlg* CPbk2ContactEditorDlg::NewL(
        TPbk2ContactEditorParams& aParams, 
        MVPbkStoreContact* aContact,
        MPbk2EditedContactObserver& aContactObserver,
        MPbk2ApplicationServices* aAppServices,
        HBufC* aTitleText,
        TInt aEditorFieldsId )
    {
    CPbk2ContactEditorDlg* self =
        new(ELeave) CPbk2ContactEditorDlg(aParams, aContactObserver,aAppServices,aEditorFieldsId );
    CleanupStack::PushL(self);
    self->ConstructL( aContact );
	self->iTitleText = aTitleText;
    CleanupStack::Pop(self);
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ConstructL( MVPbkStoreContact* aStoreContact )
    {
    CreatePresentationContactL( *aStoreContact );

    iEditorStrategy =
        Pbk2ContactEditorStrategyFactory::CreateL
            (iParams, iContact);

    // Take ownership of the contact after all leaving initialization has
    // been done.
    iStoreContact = aStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactEditorDlg::ExecuteLD()
    {
    CleanupStack::PushL( this );

    TBool destroyed = EFalse;
    iDestroyedPtr = &destroyed;

    // Implementation dialog must be run again e.g when relocation
    // is done due to item addition.
    do
        {
        // Reset state
        iRelocationState.ClearAll();

        iImplementation = CPbk2ContactEditorDlgImpl::NewL
            ( iParams, *iContact, *iFieldProperties, *this,
              *iEditorStrategy, *this, iAppServices, iTitleText ); 
        // ownership is passed
        iTitleText = NULL;
        iEliminator = iImplementation;
        iImplementation->ResetWhenDestroyed( &iEliminator );
        iImplementation->ExecuteLD();
        } while ( ContinueAfterRelocation() );

    if ( destroyed )
        {
        CleanupStack::Pop( this );
        }
    else
        {
        CleanupStack::PopAndDestroy( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::RequestExitL( TInt aCommandId )
    {
    __ASSERT_DEBUG( iEliminator, Panic( ENullPointer ) );
    iEliminator->RequestExitL( aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ForceExit()
    {
    if ( iEliminator )
        {
        iEliminator->ForceExit();
        }
    // The contact relocator should be destructed 
    // if the edit dialog has been destructed
    if ( iContactRelocator )
        {   	
        iRelocationState.ClearAll();
        delete iContactRelocator;
        iContactRelocator = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ResetWhenDestroyed(
        MPbk2DialogEliminator** aSelfPtr )
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr == this,
        Panic(EPanicPreCond_ResetWhenDestroyed));
    iSelfPtr = aSelfPtr;  
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactRelocatedL
        (MVPbkStoreContact* aRelocatedContact)
    {
    iStoreContact = aRelocatedContact; // take ownership
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    // Take the contact back
    iStoreContact = aContact;
    iAddItemFieldTypeResourceId = KErrNotFound;
    iAddItemFieldTypeXspName = NULL;

    if ( aReason != KErrCancel )
        {
        // Reset state
        iRelocationState.Set( ERelocationFailed );
        // No error note is to be shown to the user when she
        // manually cancels the relocation process, therefore
        // the error code must be converted
        CCoeEnv::Static()->HandleError( aReason );
        }
    else
        {
        // Relocation cancelled
        iRelocationState.Set( ERelocationCancelled );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Not called in editor case
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::RelocationProcessComplete()
    {
    TRAPD( res, HandleRelocationProcessCompleteL() );
    if ( res != KErrNone )
        {
        // Shows system messages
        CCoeEnv::Static()->HandleError( res );
        // If application is not closed then at least destroy
        // the dialog
        iRelocationState.ClearAll();
        iRelocationState.Set( ERelocationFailed );
        delete iImplementation;
        iImplementation = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::RelocateContactL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlg::RelocateContactL(
        TPbk2RelocationQyeryPolicy aQueryPolicy )
    {
    TBool result = RelocateContactL( KErrNotFound, KNullDesC, aQueryPolicy );
    // Clear the add item state because this is called in other cases than
    // relocation due to add item.
    iRelocationState.Clear( ERelocationDueToItemAddition );
    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::RelocateContactL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlg::RelocateContactL( 
        TInt aAddItemFieldTypeResourceId,
        const TDesC& aAddItemXspName,
        TPbk2RelocationQyeryPolicy aQueryPolicy )
    {
    iRelocationState.ClearAll();
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    TBool result = EFalse;
    if (iContactRelocator->IsPhoneMemoryInConfigurationL())
        {
        CPbk2ContactRelocator::TFlags flags =
            CPbk2ContactRelocator::EPbk2RelocatorExistingContact;
        if ( iParams.iFlags & TPbk2ContactEditorParams::ENewContact )
            {
            flags = CPbk2ContactRelocator::EPbk2RelocatorNewContact;
            }

        result = iContactRelocator->RelocateAndLockContactL(
            iStoreContact, *this,
            aQueryPolicy,
            flags );
        iStoreContact = NULL; // relocator took away ownership
        iAddItemFieldTypeResourceId = aAddItemFieldTypeResourceId;
        if (aAddItemXspName != KNullDesC())
            {
            delete iAddItemFieldTypeXspName;
            iAddItemFieldTypeXspName = NULL;
            iAddItemFieldTypeXspName = aAddItemXspName.AllocL();
            }
        // else not needed because iAddItemFieldTypeXspName is NULL

        /// Dialog must be started again if relocation is done due to
        /// item addition.
        if ( iContactRelocator )
            {
            iRelocationState.Set( ERelocationDueToItemAddition );
            }
        }
    return result;        
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::IsPhoneMemoryInConfigurationL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlg::IsPhoneMemoryInConfigurationL()
    {
    CPbk2ContactRelocator* contactRelocator = CPbk2ContactRelocator::NewL();
    CleanupStack::PushL( contactRelocator );
    TBool ret = contactRelocator->IsPhoneMemoryInConfigurationL();
    CleanupStack::PopAndDestroy( contactRelocator );
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::IsTemplateFieldL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlg::IsTemplateFieldL( TInt aResId,
        const TDesC& aXSpName )
    {
    TBool result( EFalse );
    const MVPbkFieldType* fieldType =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            FieldTypes().Find( aResId );
    if ( fieldType )
        {
        const MPbk2FieldProperty* fieldProp =
            iFieldProperties->FindProperty( *fieldType, aXSpName );
        if ( fieldProp
            && ( fieldProp->Flags() & KPbk2FieldFlagTemplateField ) )
            {
            result = ETrue;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::EditorReadyL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::EditorReadyL()
    {
    // Editor is ready
    if (iAddItemFieldTypeResourceId != KErrNotFound)
        {
        TPtrC xSpName = iAddItemFieldTypeXspName ? *iAddItemFieldTypeXspName :
                KNullDesC();
        // If new field is template field in relocated contact,
        // do not add new field. Just focus the field in contact.
        if ( IsTemplateFieldL( iAddItemFieldTypeResourceId, xSpName ) )
            {
            iImplementation->TryChangeFocusWithTypeIdL
                ( iAddItemFieldTypeResourceId, xSpName );
            }
        else
            {
            // Lets run the add item dialog silently
            iImplementation->AddItemToContactL
                ( iAddItemFieldTypeResourceId, xSpName );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactEditingComplete
//
// aEditedContact is commented because it is called from the
// CPbk2ContactEditorDlgImpl who actually don't own it. So against
// observer documentation ownership is not changed. This is
// an implementation detail of
// CPbk2ContactEditorDlg <-> CPbk2ContactEditorDlgImpl
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactEditingComplete(
        MVPbkStoreContact* /*aEditedContact*/ )
    {
    // Don't inform observer if the dialog need to be run again
    if ( !ContinueAfterRelocation() )
        {
        // Give ownership of the iStoreContact to the observer
        MVPbkStoreContact* contact = iStoreContact;
        iStoreContact = NULL;
        iContactObserver.ContactEditingComplete( contact );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactEditingDeletedContact
//
// aEditedContact is commented because it is called from the
// CPbk2ContactEditorDlgImpl who actually don't own it. So against
// observer documentation ownership is not changed. This is
// an implementation detail of
// CPbk2ContactEditorDlg <-> CPbk2ContactEditorDlgImpl
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactEditingDeletedContact(
    MVPbkStoreContact* /*aEditedContact*/ )
    {
    // Don't inform observer if the dialog need to be run again
    if ( !ContinueAfterRelocation() )
        {
        // Give ownership of the iStoreContact to the observer
        MVPbkStoreContact* contact = iStoreContact;
        iStoreContact = NULL;
        iContactObserver.ContactEditingDeletedContact( contact );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::ContactEditingAborted()
    {
    // Forward from implementation dialog to the actual client
    iContactObserver.ContactEditingAborted();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::HandleRelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::HandleRelocationProcessCompleteL()
    {
    if ( !iRelocationState.IsSet( ERelocationCancelled ) &&
         !iRelocationState.IsSet( ERelocationFailed ) )
        {
        // Update contact
        CreatePresentationContactL();

        // Contact has been relocated which means that the state changes
        // from creating a new contact to editing an relocated contact.

        // Remove the new contact flag...
        iParams.iFlags &=  ~TPbk2ContactEditorParams::ENewContact;
        // ... and set the editing flag
        iParams.iFlags |= TPbk2ContactEditorParams::EModified;
        // Update strategy
        delete iEditorStrategy;
        iEditorStrategy = NULL;
        iEditorStrategy = Pbk2ContactEditorStrategyFactory::CreateL
            ( iParams, iContact );

        // Set iFocusedContactField to NULL so that
        // the right field (new field) will be focused
        iParams.iFocusedContactField = NULL;

        TBool informObserver = EFalse;
        if ( !iRelocationState.IsSet( ERelocationDueToItemAddition ) )
            {
            informObserver = ETrue;
            }
        // Delete current implementation dialog nicely and create
        // a new dialog in ExecuteLD if needed.        
        iImplementation->CloseWithoutSaving( informObserver );
        }
    else
        {
        // Reset state if dialog is not closed -> continued with old contact
        iRelocationState.ClearAll();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::CreatePresentationContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::CreatePresentationContactL()
    {
    CreatePresentationContactL( *iStoreContact );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::CreatePresentationContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlg::CreatePresentationContactL(
        MVPbkStoreContact& aStoreContact)
    {
    delete iSpecificFieldProperties;
    iSpecificFieldProperties = NULL;
    delete iFieldProperties;
    iFieldProperties = NULL;
    
    const MVPbkFieldTypeList& supportedFieldTypes =
        aStoreContact.ParentStore().StoreProperties().SupportedFields();

    //If custom fields are provided (social phonebook)
    if( iEditorFieldsId != KErrNotFound )
    	{
    	HBufC8* res = CCoeEnv::Static()->AllocReadResourceAsDes8L( iEditorFieldsId );
    	CleanupStack::PushL( res );
    	TResourceReader reader;
    	reader.SetBuffer( res );
    	// Create a field property list of the provided fields on reader
		iFieldProperties = CPbk2FieldPropertyArray::NewL
		   ( supportedFieldTypes, reader, CCoeEnv::Static()->FsSession() );
		CleanupStack::PopAndDestroy( res );
		}
    else
    	{
    	// Create a field property list of the supported
		// field types of the used store
		iFieldProperties = CPbk2FieldPropertyArray::NewL
			( supportedFieldTypes, CCoeEnv::Static()->FsSession() );
    	}
    
    // if iAppServices is set. This is used if EditorDlg is used outside of Pbk2 context
    if( iAppServices )
    	{
    	iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL
			( *iFieldProperties,
			  iAppServices->StoreProperties(),
			  supportedFieldTypes, aStoreContact.ParentStore() );
    	}
    else
    	{
    	iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL
			( *iFieldProperties,
			  Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties(),
			  supportedFieldTypes, aStoreContact.ParentStore() );
    	}

    delete iContact;
    iContact = NULL;
    iContact = CPbk2PresentationContact::NewL
        ( aStoreContact, *iSpecificFieldProperties );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ContinueAfterRelocation
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlg::ContinueAfterRelocation()
    {
    TBool ret = EFalse;

    // Currently 4 ways to continue after relocation:
    // 1) User cancels relocation -> editor stays open with the OLD contact.
    // 2) Relocation done due to add item and user approves
    //    relocation -> editor stays open with the NEW contact
    // 3) Relocation done due to other reason than add item.
    //    User approves relocation -> editor is closed.
    // 4) Error case

    if ( iRelocationState.IsSet( ERelocationDueToItemAddition ) &&
         !iRelocationState.IsSet( ERelocationCancelled ) &&
         !iRelocationState.IsSet( ERelocationFailed )  )
        {
        // Continue only in case 2
        ret = ETrue;
        }

    return ret;
    }

// End of File
