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
* Description:  Manages adding new items to the contact
*
*/



// INCLUDE FILES
#include "CPbk2AddItemManager.h"

// Phonebook 2
#include "MPbk2ContactEditorFieldArray.h"
#include "MPbk2ContactEditorField.h"
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2AddItemToContactDlg.h>
#include <TPbk2AddItemWrapper.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2ContactRelocator.h>
#include <CPbk2FieldGroupPropertyArray.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <MPbk2StoreValidityInformer.h>
#include <CPbk2FieldPropertyGroup.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_MakePhoneContactL = 1,
    EPanicPreCond_RetryFieldsAdditionL
    };
#endif // _DEBUG

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AddItemManager");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Checks is the store valid.
 *
 * @param aInformer         Store validity informer.
 * @param aStoreUri         URI of the store to inspect.
 * @return  ETrue if store is valid.
 */
TBool IsValidStoreL
        ( MPbk2StoreValidityInformer& aInformer, TVPbkContactStoreUriPtr aStoreUri )
    {
    TBool isValid = EFalse;
    CVPbkContactStoreUriArray* currentlyValidStores =
        aInformer.CurrentlyValidStoresL();
    isValid = currentlyValidStores->IsIncluded( aStoreUri );
    delete currentlyValidStores;

    return isValid;
    }
   
}  /// namespace

// --------------------------------------------------------------------------
// CPbk2AddItemManager::CPbk2AddItemManager
// --------------------------------------------------------------------------
//
CPbk2AddItemManager::CPbk2AddItemManager
        (CPbk2PresentationContact& aContact,
        MPbk2ContactEditorFieldArray& aFieldArray, 
        TPbk2ContactEditorParams& aParams,
        MPbk2ApplicationServices* aAppServices ) :
            iContact(&aContact),
            iFieldArray(&aFieldArray),
            iParams(aParams),
            iAppServices( aAppServices ),
            iProperties(&aContact.PresentationFields().FieldProperties() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::~CPbk2AddItemManager
// --------------------------------------------------------------------------
//
CPbk2AddItemManager::~CPbk2AddItemManager()
    {
    delete iTemporaryPresentationContact;
    delete iTemporaryStoreContact;
    delete iGroupProperties;
    delete iXSpName;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::NewL
// --------------------------------------------------------------------------
//
CPbk2AddItemManager* CPbk2AddItemManager::NewL
        ( CPbk2PresentationContact& aContact,
          MPbk2ContactEditorFieldArray& aFieldArray, 
          TPbk2ContactEditorParams& aParams,
          MPbk2ApplicationServices* aAppServices )
    {
    CPbk2AddItemManager* self = 
        new ( ELeave ) CPbk2AddItemManager( aContact, aFieldArray, aParams, aAppServices );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::AddFieldsL
// --------------------------------------------------------------------------
//
CPbk2AddItemManager::TReturnValue CPbk2AddItemManager::AddFieldsL
 (TInt& aFieldTypeResId, TPtrC& aFieldTypeXspName )
    {
    CArrayPtr<const MPbk2FieldProperty>* propArray = 
        iContact->AvailableFieldsToAddL();
    CleanupStack::PushL(propArray);
    
    CPbk2ContactRelocator* contactRelocator = CPbk2ContactRelocator::NewL();
    CleanupStack::PushL( contactRelocator );

    if (!contactRelocator->IsPhoneMemoryContact( *iContact ) &&
            contactRelocator->IsPhoneMemoryInConfigurationL() &&
            IsValidStoreL ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreValidityInformer(), VPbkContactStoreUris::DefaultCntDbUri() ) )
        {
        // Expand the property array with phone memory field types
        AddPhoneMemoryFieldTypesL(*propArray);
        }

    CleanupStack::PopAndDestroy( contactRelocator );

    
    // Create group properties
    if ( iGroupProperties )
        {
        delete iGroupProperties;
        iGroupProperties = NULL;                
        }
        
    iGroupProperties = CPbk2FieldGroupPropertyArray::NewL( *iProperties );

    TReturnValue ret = DoAddFieldsL(*propArray, aFieldTypeResId, aFieldTypeXspName);
            
    CleanupStack::PopAndDestroy(propArray);
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::AddFieldsToUiAndContactL
// --------------------------------------------------------------------------
//
inline CPbk2AddItemManager::TReturnValue CPbk2AddItemManager::AddFieldsToUiAndContactL(
    const TPbk2AddItemWrapper& aWrapper)
    {
    TReturnValue ret;
    TInt controlId = KErrUnknown;
    const TInt count = aWrapper.PropertyCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MPbk2FieldProperty& prop = aWrapper.PropertyAt(i);


        MPbk2FieldProperty2* fieldPropertyExtension =
            reinterpret_cast<MPbk2FieldProperty2*>(
                const_cast<MPbk2FieldProperty&>( prop ).
                    FieldPropertyExtension(
                        KMPbk2FieldPropertyExtension2Uid ) );
        User::LeaveIfNull( fieldPropertyExtension ); 
        
        TInt tmpId = iFieldArray->AddNewFieldL( prop.FieldType(),
            fieldPropertyExtension->XSpName() );
        if (controlId == KErrUnknown)
            {
            controlId = tmpId;
            }
        }
    
    if( aWrapper.Group() )
    	{
    	ret.iGruopId = aWrapper.Group()->GroupId();
    	}
    
    ret.iControlId = controlId;
    return ret;
    }
    
// --------------------------------------------------------------------------
// CPbk2AddItemManager::RunDialogAndAddFieldsL
// --------------------------------------------------------------------------
//
inline CPbk2AddItemManager::TReturnValue CPbk2AddItemManager::RunDialogAndAddFieldsL
        ( RArray<TPbk2AddItemWrapper>& aWrappers,
          TInt& aFieldTypeResId, 
          TPtrC& aFieldTypeXspName )
    {
    TReturnValue ret;
    ret.iControlId = KErrUnknown;
    TInt selectionIndex( KErrNotFound );
    
    if (aWrappers.Count() > 0)
        {
        if ( aFieldTypeResId != KErrNotFound )
            {
            // Make a conversion from ResId to selection
            TInt wrapCount( aWrappers.Count() );
            for ( TInt i(0); i < wrapCount && selectionIndex == KErrNotFound; ++i)
                {
                TPbk2AddItemWrapper* wrapper = &aWrappers[i];
                TInt propCount( wrapper->PropertyCount() );
                for ( TInt j(0); j < propCount && selectionIndex == KErrNotFound; ++j )
                    {
                    const MPbk2FieldProperty& prop = wrapper->PropertyAt(j);
                    MPbk2FieldProperty2* fieldPropertyExtension =
                        reinterpret_cast<MPbk2FieldProperty2*>(
                            const_cast<MPbk2FieldProperty&>( prop ).
                                FieldPropertyExtension(
                                    KMPbk2FieldPropertyExtension2Uid ) );
                    User::LeaveIfNull( fieldPropertyExtension );
                    if ( (aFieldTypeResId == prop.FieldType().FieldTypeResId() ) &&
                            !aFieldTypeXspName.CompareF(fieldPropertyExtension->XSpName()) )
                        {
                        selectionIndex = i;
                        }
                    }
                }
            if ( selectionIndex == KErrNotFound )
                {
                // Field cannot be added to current contact
                // propably it already exist and this field type cannot be 
                // more than once in one contact
                ret.iControlId = KErrAlreadyExists;
                }
            }
        
        if( ret.iControlId == KErrUnknown )
            {
            if ( selectionIndex == KErrNotFound )
                {
                CPbk2AddItemToContactDlg* dlg = CPbk2AddItemToContactDlg::NewL( iAppServices );
                TPbk2AddItemDialogParams params(NULL, 0, selectionIndex);
                selectionIndex = dlg->ExecuteLD(aWrappers, &params);
                }
                
            if (selectionIndex != KErrCancel)
                {
                TPbk2AddItemWrapper* wrapper = &aWrappers[selectionIndex];
                MPbk2FieldProperty2* fieldPropertyExtension =
                    reinterpret_cast<MPbk2FieldProperty2*>(
                        const_cast<MPbk2FieldProperty&>( wrapper->PropertyAt(0) ).
                            FieldPropertyExtension(
                                KMPbk2FieldPropertyExtension2Uid ) );
                User::LeaveIfNull( fieldPropertyExtension );
                
                aFieldTypeResId = wrapper->PropertyAt(0).FieldType().FieldTypeResId();
                aFieldTypeXspName.Set(fieldPropertyExtension->XSpName());
                delete iXSpName;
                iXSpName = NULL;
                if (aFieldTypeXspName != KNullDesC)
                    {
                    iXSpName = fieldPropertyExtension->XSpName().AllocL();
                    }
                aFieldTypeXspName.Set(iXSpName ? *iXSpName : KNullDesC());
                TRAPD(err, ret = AddFieldsToUiAndContactL(*wrapper));
                
                if (err == KErrNotSupported)
                    {
                    // KErrNotSupported means that the current
                    // contact does not support the selected detail.
                    // When receiving this error code the caller
                    // may choose to relocate the contact and try again.
                    ret.iControlId = KErrNotSupported;
                    }
                else
                    {
                    User::LeaveIfError(err);
                    }
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::AddPhoneMemoryFieldTypesL
// --------------------------------------------------------------------------
//
inline void CPbk2AddItemManager::AddPhoneMemoryFieldTypesL
        (CArrayPtr<const MPbk2FieldProperty>& aProperties)
    {
    // Make a duplicate phone contact instance out of the current contact
    MakePhoneContactL(*iContact);
    
    // Get available fields to add to the phone contact
    CArrayPtr<const MPbk2FieldProperty>* phonePropArray =
        iTemporaryPresentationContact->AvailableFieldsToAddL();
    CleanupStack::PushL(phonePropArray);
    
    // Append new properties to the old array
    TInt currentPropertyCount = aProperties.Count();
    TInt phonePropertyCount = phonePropArray->Count();
    for (TInt i=0; i<phonePropertyCount; ++i)
        {
        TBool found = EFalse;
        const MPbk2FieldProperty* phoneProp = phonePropArray->At(i);
        for (TInt j=0; j<currentPropertyCount; ++j)
            {
            const MPbk2FieldProperty* prop = aProperties.At(j);
            if (prop->IsSame(*phoneProp))
                {
                found = ETrue;
                break;
                }
            }
        // Append the property only if not found already
        if ( !found && iParams.iActiveView == TPbk2ContactEditorParams::EEditorView )
            {
            aProperties.AppendL(phoneProp);
            }
        }

    CleanupStack::PopAndDestroy(phonePropArray);
    
    // Switch properties member to reference the properties of
    // the temporary contact
    iProperties = &iTemporaryPresentationContact->PresentationFields().
        FieldProperties();
    }
    
// --------------------------------------------------------------------------
// CPbk2AddItemManager::DoAddFieldsL
// --------------------------------------------------------------------------
//
inline CPbk2AddItemManager::TReturnValue CPbk2AddItemManager::DoAddFieldsL
        (CArrayPtr<const MPbk2FieldProperty>& aProperties,
        TInt& aFieldTypeResId,
        TPtrC& aFieldTypeXspName )
    {
    RArray<TPbk2AddItemWrapper> wrappers;
    CleanupClosePushL(wrappers);
   
    RPointerArray<const CPbk2FieldPropertyGroup> addedGroups;
    CleanupClosePushL(addedGroups);
    
    const CPbk2FieldPropertyGroup* address = NULL;
    TInt countAddr = 0;
    const TInt count = aProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const MPbk2FieldProperty& prop = *(aProperties)[i];
        const CPbk2FieldPropertyGroup* group = 
            iGroupProperties->FindGroupForProperty(prop);
        
        TPbk2FieldGroupId propGroup = EPbk2FieldGroupIdNone; 
        if( group )
        	{
        	propGroup = group->GroupId();
        	}

        if ( propGroup != EPbk2FieldGroupIdNone )
        	{
        	 if( addedGroups.Find(group) == KErrNotFound  )
	            {
	            addedGroups.AppendL(group); 
	            if (iContact->PresentationFields().ContainsFieldFromGroup(
	                    *group))
	                {
	                if( propGroup != EPbk2FieldGroupIdPostalAddress &&
	            		propGroup != EPbk2FieldGroupIdHomeAddress &&
	            		propGroup != EPbk2FieldGroupIdCompanyAddress || 
	            		aFieldTypeResId > 0 )
	                	{
		                // Item belongs to the group but there are already
		                // items from the same group in the contact
		                wrappers.AppendL(TPbk2AddItemWrapper(prop));
	                	}
	                }
	            else
	                {
	                // Add the whole group of fields
	                wrappers.AppendL(TPbk2AddItemWrapper(*group));
	            	}
	            }
            }
        else
        	{
            // Item doesn't belong to group
            wrappers.AppendL(TPbk2AddItemWrapper(prop));
        	}
        }
    
    CleanupStack::PopAndDestroy(); // addedGroups

    TReturnValue ret = RunDialogAndAddFieldsL(wrappers, aFieldTypeResId,
            aFieldTypeXspName);
    CleanupStack::PopAndDestroy(); // wrappers
    
    return ret;
    }
    
// --------------------------------------------------------------------------
// CPbk2AddItemManager::IsFieldAdditionPossibleL
// --------------------------------------------------------------------------
//
inline TBool CPbk2AddItemManager::IsFieldAdditionPossibleL
        (const CPbk2PresentationContact& aContact,
        const MPbk2FieldProperty& aProperty)
    {
    TBool ret = EFalse;
    // To be able to add field requires that the property must fit
    // either into phone memory contact or into the current
    // memory contact
    if ((aContact.IsFieldAdditionPossibleL(aProperty.FieldType())) &&
          (aProperty.Flags() & KPbk2FieldFlagUserCanAddField))
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemManager::MakePhoneContactL
// --------------------------------------------------------------------------
//
void CPbk2AddItemManager::MakePhoneContactL
        (const CPbk2PresentationContact& aContact)
    {
    // Get phone memory contact store
    MVPbkContactStore* phoneStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            ContactStoresL().Find
                (VPbkContactStoreUris::DefaultCntDbUri());
            
    __ASSERT_DEBUG(phoneStore,
        Panic(EPanicPreCond_MakePhoneContactL));

    // Create a temporary contact to phone memory store
    iTemporaryStoreContact = phoneStore->CreateNewContactLC();
    CleanupStack::Pop(); // CreateNewContactLC

    // Make a presentation contact out of it
    iTemporaryPresentationContact = CPbk2PresentationContact::NewL
        (*iTemporaryStoreContact,
        Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties());
    
    // Add the fields of the current store contact in to it
    const TInt count(aContact.Fields().FieldCount());
    for (TInt i = 0; i<count; ++i)
        {
        CPbk2PresentationContactField& field =
            aContact.PresentationFields().At(i);
        
        if (iTemporaryPresentationContact->IsFieldAdditionPossibleL
                (field.FieldProperty().FieldType()))
            {
            MVPbkStoreContactField* newField =
                iTemporaryPresentationContact->CreateFieldLC(
                    field.FieldProperty().FieldType());
            iTemporaryPresentationContact->AddFieldL(newField);
            CleanupStack::Pop(); // newField
            }
        }
    }

//  End of File
