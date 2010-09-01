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
* Description:  Contact model store Voice tag attribute manager
*
*/


#include "csindvoicetagattributemanager.h"
#include "csindvoicetagattributelistoperation.h"
#include "csindvoicetagattributefindoperation.h"

// From Virtual Phonebook
#include <cvpbkcontactmanager.h>
#include <cvpbkvoicetagattribute.h>
#include <vpbkstoreuriliterals.h>

// From VPbkCntModel
#include "ccontactstore.h"

namespace VPbkCntModel {

/**
 * Factory function.
 */
CVoiceTagAttributeManager* CVoiceTagAttributeManager::NewL(
        TAny* aParam)
    {
    return CSindVoiceTagAttributeManager::NewL(aParam);
    }

/**
 * SIND voice tag attribute manager.
 */
CSindVoiceTagAttributeManager::CSindVoiceTagAttributeManager(
        CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager)
    {
    }

inline void CSindVoiceTagAttributeManager::ConstructL()
    {
    // create Vas db manager
    iVasDbManager = CNssVASDBMgr::NewL();
    const TInt err = iVasDbManager->InitializeL();
    if ( err != KErrNone)
        {
        // DbManager returns own error codes that are success or
        // failed, so better to return just KErrGeneral on failure
        User::Leave( KErrGeneral );
        }

    // Ownership not transferred
    iNssContextManager= iVasDbManager->GetContextMgr();
    // Ownership not transferred
    iTagManager = iVasDbManager->GetTagMgr();
    }

CSindVoiceTagAttributeManager* CSindVoiceTagAttributeManager::NewL(TAny* aParam)
    {
    TParam& param = *static_cast<TParam*>(aParam);
    CSindVoiceTagAttributeManager* self = new(ELeave) CSindVoiceTagAttributeManager(
        param.iContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSindVoiceTagAttributeManager::~CSindVoiceTagAttributeManager()
    {
    delete iVasDbManager;
    delete iContext;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::ListContactsL(
        TUid /*aAttributeType*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::ListContactsL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }

MVPbkContactOperation* CSindVoiceTagAttributeManager::CreateListContactsOperationL(
            TUid aAttributeType,
            MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        CVPbkVoiceTagAttribute* attribute = CVPbkVoiceTagAttribute::NewL();
        CleanupStack::PushL(attribute);
        result = CreateListContactsOperationL(*attribute, aObserver);
        CleanupStack::PopAndDestroy(attribute);
        }

    return result;
    }

MVPbkContactOperation* CSindVoiceTagAttributeManager::CreateListContactsOperationL(
        const MVPbkContactAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
            KVPbkDefaultCntDbURI() );
        if (store)
            {
            CContactStore& cntStore = *static_cast<CContactStore*>(store);
            result = CSindVoiceTagAttributeListOperation::NewListLC(
                cntStore,
                *iNssContextManager,
                *iTagManager,
                aObserver);
            CleanupStack::Pop();
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }

    return result;
    }


TBool CSindVoiceTagAttributeManager::HasContactAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContact& /*aContact*/) const
    {
    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return EFalse;    
    }

TBool CSindVoiceTagAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContact& /*aContact*/) const
    {
    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return EFalse;    
    }

TBool CSindVoiceTagAttributeManager::HasFieldAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContactField& /*aField*/) const
    {
    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return EFalse;    
    }

TBool CSindVoiceTagAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContactField& /*aField*/) const
    {
    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return EFalse;    
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::SetContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::SetFieldAttributeL(
        MVPbkStoreContactField& /*aField*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& /*aField*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& /*aField*/,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkStoreContactFieldCollection* CSindVoiceTagAttributeManager::FindFieldsWithAttributeLC(
        TUid aAttributeType,
        MVPbkStoreContact& /*aContact*/) const
    {
    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return NULL;
    }

MVPbkStoreContactFieldCollection* CSindVoiceTagAttributeManager::FindFieldsWithAttributeLC(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& /*aContact*/) const
    {
    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }
    return NULL;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::HasContactAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::HasFieldAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::FindFieldsWithAttributeL(
        TUid aAttributeType,
        MVPbkStoreContact& aContact,
        MVPbkMultiAttributePresenceObserver& aObserver) const
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttributeType == CVPbkVoiceTagAttribute::Uid())
        {
        CVPbkVoiceTagAttribute* attribute = CVPbkVoiceTagAttribute::NewL();
        CleanupStack::PushL(attribute);
        result = FindFieldsWithAttributeL(*attribute, aContact, aObserver);
        CleanupStack::PopAndDestroy(attribute);
        }

    return result;
    }

MVPbkContactOperationBase* CSindVoiceTagAttributeManager::FindFieldsWithAttributeL(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact,
        MVPbkMultiAttributePresenceObserver& aObserver) const
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid())
        {
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
            KVPbkDefaultCntDbURI() );
        if (store)
            {
            CContactStore& cntStore = *static_cast<CContactStore*>(store);
            result = CSindVoiceTagAttributeFindOperation::NewFindLC(
                cntStore,
                *iNssContextManager,
                *iTagManager,
                aContact,
                iContactManager,
                aObserver);
            CleanupStack::Pop();
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }

    return result;
    }

} // namespace VPbkCntModel

// End of File
