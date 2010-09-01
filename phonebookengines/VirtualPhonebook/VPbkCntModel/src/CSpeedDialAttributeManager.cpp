/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook speed dial attribute manager
*
*/
 

#include "CSpeedDialAttributeManager.h"
#include "CSpeedDialAttributeOperation.h"

// From Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkContactOperation.h>
#include <VPbkStoreUriLiterals.h>
#include <MVPbkContactStoreList.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactFieldCollection.h>

// From VPbkCntModel
#include "CContactStore.h"
#include "CContact.h"

// From Contacts Model
#include <cntitem.h>


namespace VPbkCntModel {

const TInt KMaxSDValue(9);
static const TInt speedDialUids[] =
   {0, // to make this array 1-based
    KUidSpeedDialOneValue,
    KUidSpeedDialTwoValue,
    KUidSpeedDialThreeValue,
    KUidSpeedDialFourValue,
    KUidSpeedDialFiveValue,
    KUidSpeedDialSixValue,
    KUidSpeedDialSevenValue,
    KUidSpeedDialEightValue,
    KUidSpeedDialNineValue};

CSpeedDialAttributeManager::CSpeedDialAttributeManager(
        CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager)
    {
    }

inline void CSpeedDialAttributeManager::ConstructL()
    {
    }

CSpeedDialAttributeManager* CSpeedDialAttributeManager::NewL(TAny* aParam)
    {
    TParam& param = *static_cast<TParam*>(aParam);
    CSpeedDialAttributeManager* self = new(ELeave) CSpeedDialAttributeManager(
        param.iContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSpeedDialAttributeManager::~CSpeedDialAttributeManager()
    {
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::ListContactsL(
        TUid /*aAttributeType*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::ListContactsL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }

MVPbkContactOperation* CSpeedDialAttributeManager::CreateListContactsOperationL(
            TUid aAttributeType,
            MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttributeType == CVPbkSpeedDialAttribute::Uid())
        {
        CVPbkSpeedDialAttribute* attribute = CVPbkSpeedDialAttribute::NewL(
            CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined);
        CleanupStack::PushL(attribute);
        result = CreateListContactsOperationL(*attribute, aObserver);
        CleanupStack::PopAndDestroy(attribute);
        }

    return result;
    }

MVPbkContactOperation* CSpeedDialAttributeManager::CreateListContactsOperationL(
        const MVPbkContactAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
        {
        const CVPbkSpeedDialAttribute& attribute =
                static_cast<const CVPbkSpeedDialAttribute&>(aAttribute);
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                   KVPbkDefaultCntDbURI());
        if (store)
            {
            CContactStore& cntStore = *static_cast<CContactStore*>(store);
            result = CSpeedDialAttributeOperation::NewListLC(
                cntStore,
                attribute,
                aObserver);
            // No need to start operation because it is suboperation for
            // attribute contact manager's find operation and all suboperations
            // are started by main operation.
            CleanupStack::Pop();
            }
        }

    return result;
    }

TBool CSpeedDialAttributeManager::HasContactAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContact& aContact) const
    {
    TBool result = EFalse;

    if (dynamic_cast<const CContact*>(&aContact))
        {
        if (aAttributeType == CVPbkSpeedDialAttribute::Uid())
            {
            MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                       KVPbkDefaultCntDbURI());
            if (store)
                {
                CContactStore* cntStore = static_cast<CContactStore*>(store);
                if (&aContact.ContactStore() == cntStore)
                    {
                    const CContact& contact = static_cast<const CContact&>(aContact);
                    CContactItemFieldSet& fields = contact.NativeContact()->CardFields();
                    const TInt count = fields.Count();
                    // Loop for every field in contact
                    for (TInt i = 0; i < count && !result; ++i)
                        {
                        // loop for every speed dial index
                        for (TInt n = 1; n <= KMaxSDValue; ++n)
                            {
                            TBuf<KSpeedDialPhoneLength> phoneNumber;
                            TContactItemId cid = cntStore->NativeDatabase().
                                    GetSpeedDialFieldL(n, phoneNumber);

                            if (cid == contact.NativeContact()->Id() &&
                                fields[i].ContentType().
                                    ContainsFieldType(TFieldType::Uid(speedDialUids[n])))
                                {
                                result = ETrue;
                                break;
                                }
                            }
                        }
                    }
                }
            }
        }
    return result;
    }

TBool CSpeedDialAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContact& aContact) const
    {
    TBool result = EFalse;

    if (dynamic_cast<const CContact*>(&aContact))
        {
        if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
            {
            const CVPbkSpeedDialAttribute& attribute =
                    static_cast<const CVPbkSpeedDialAttribute&>(aAttribute);
            if (attribute.Index() == CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined)
                {
                result = HasContactAttributeL(aAttribute.AttributeType(), aContact);
                }
            else
                {
                MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                           KVPbkDefaultCntDbURI());
                if (store)
                    {
                    CContactStore* cntStore = static_cast<CContactStore*>(store);
                    if (&aContact.ContactStore() == cntStore)
                        {
                        TBuf<KSpeedDialPhoneLength> dummyBuf;
                        TContactItemId cid = cntStore->NativeDatabase().
                                GetSpeedDialFieldL(attribute.Index(), dummyBuf);
                        const CContact& contact = static_cast<const CContact&>(aContact);
                        if (contact.NativeContact()->Id() == cid)
                            {
                            result = ETrue;
                            }
                        }
                    }
                }
            }
        }
    return result;
    }

TBool CSpeedDialAttributeManager::HasFieldAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContactField& aField) const
    {
    TBool result = EFalse;

    if (dynamic_cast<const TContactField*>(&aField))
        {
        if (aAttributeType == CVPbkSpeedDialAttribute::Uid())
            {
            MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                       KVPbkDefaultCntDbURI());
            if (store)
                {
                CContactStore* cntStore = static_cast<CContactStore*>(store);
                if (&aField.ContactStore() == cntStore)
                    {
                    for (TInt i = 1; i <= KMaxSDValue && !result; ++i)
                        {
                        TBuf<KSpeedDialPhoneLength> phoneNumber;
                        TContactItemId cid = cntStore->NativeDatabase().
                                GetSpeedDialFieldL(i, phoneNumber);
                        const TContactField& field = static_cast<const TContactField&>(aField);
                        if (cid == static_cast<CContact&>(aField.ParentContact()).
                                NativeContact()->Id() &&
                            field.NativeField()->ContentType().
                                ContainsFieldType(TFieldType::Uid(speedDialUids[i])))
                            {
                            result = ETrue;
                            }
                        }
                    }
                }
            }
        }
    return result;
    }

TBool CSpeedDialAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContactField& aField) const
    {
    TBool result = EFalse;

    if (dynamic_cast<const TContactField*>(&aField))
        {
        if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
            {
            const CVPbkSpeedDialAttribute& attribute =
                    static_cast<const CVPbkSpeedDialAttribute&>(aAttribute);
            if (attribute.Index() == CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined)
                {
                result = HasFieldAttributeL(aAttribute.AttributeType(), aField);
                }
            else
                {
                MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                           KVPbkDefaultCntDbURI());
                if (store)
                    {
                    CContactStore* cntStore = static_cast<CContactStore*>(store);
                    if (&aField.ContactStore() == cntStore)
                        {
                        TBuf<KSpeedDialPhoneLength> phoneNumber;
                        TContactItemId cid = cntStore->NativeDatabase().
                                GetSpeedDialFieldL(attribute.Index(), phoneNumber);
                        const TContactField& field = static_cast<const TContactField&>(aField);
                        if ((cid == static_cast<CContact&>(
                                    aField.ParentContact()).NativeContact()->Id()) &&
                            (field.NativeField()->ContentType().ContainsFieldType(
                               TFieldType::Uid(speedDialUids[attribute.Index()]))))
                            {
                            result = ETrue;
                            }
                        }
                    }
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::SetContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::SetFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (dynamic_cast<const TContactField*>(&aField))
        {
        if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
            {
            const CVPbkSpeedDialAttribute& attribute =
                    static_cast<const CVPbkSpeedDialAttribute&>(aAttribute);
            if (attribute.Index() == CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined)
                {
                User::Leave(KErrArgument);
                }
            else
                {
                MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                           KVPbkDefaultCntDbURI());
                if (store)
                    {
                    VPbkCntModel::CContactStore& cntStore = *static_cast<CContactStore*>(store);
                    result = CSpeedDialAttributeOperation::NewSetLC(
                            cntStore, aField, attribute, aObserver);
                    result->StartL();
                    CleanupStack::Pop(); // result
                    }
                else
                    {
                    User::Leave(KErrNotSupported);
                    }
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if (aAttributeType == CVPbkSpeedDialAttribute::Uid())
        {
        User::Leave(KErrNotSupported);
        }

    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (dynamic_cast<TContactField*>(&aField))
        {
        if (aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
            {
            const CVPbkSpeedDialAttribute& attribute =
                    static_cast<const CVPbkSpeedDialAttribute&>(aAttribute);

            MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                       KVPbkDefaultCntDbURI());
            if (store)
                {
                VPbkCntModel::CContactStore& cntStore = *static_cast<CContactStore*>(store);
                result = CSpeedDialAttributeOperation::NewRemoveLC(
                        cntStore, aField, attribute, aObserver);
                result->StartL();
                CleanupStack::Pop(); // result
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& aObserver)
    {
    MVPbkContactOperationBase* result = NULL;

    if (dynamic_cast<TContactField*>(&aField))
        {
        if (aAttributeType == CVPbkSpeedDialAttribute::Uid())
            {
            CVPbkSpeedDialAttribute* attribute = CVPbkSpeedDialAttribute::NewL();
            CleanupStack::PushL(attribute);
            result = RemoveFieldAttributeL(aField, *attribute, aObserver);
            CleanupStack::PopAndDestroy(attribute);
            }
        }
    return result;
    }

MVPbkStoreContactFieldCollection* CSpeedDialAttributeManager::FindFieldsWithAttributeLC(
        TUid aAttributeType,
        MVPbkStoreContact& aContact) const
    {
    CVPbkContactFieldCollection* result = NULL;

    if (dynamic_cast<CContact*>(&aContact) &&
        aAttributeType == CVPbkSpeedDialAttribute::Uid())
        {
        result = CVPbkContactFieldCollection::NewLC(aContact);
        const TInt count = aContact.Fields().FieldCount();
        for (TInt i = 0; i < count; ++i)
            {
            const MVPbkStoreContactField& field = aContact.Fields().FieldAt(i);
            if (HasFieldAttributeL(aAttributeType, field))
                {
                MVPbkStoreContactField* clone = field.CloneLC();
                result->AppendL(clone);
                CleanupStack::Pop(); // clone
                }
            }
        }

    return result;
    }

MVPbkStoreContactFieldCollection* CSpeedDialAttributeManager::FindFieldsWithAttributeLC(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact) const
    {
    CVPbkContactFieldCollection* result = NULL;

    if (dynamic_cast<CContact*>(&aContact) &&
        aAttribute.AttributeType() == CVPbkSpeedDialAttribute::Uid())
        {
        result = CVPbkContactFieldCollection::NewLC(aContact);
        const TInt count = aContact.Fields().FieldCount();
        for (TInt i = 0; i < count; ++i)
            {
            const MVPbkStoreContactField& field = aContact.Fields().FieldAt(i);
            if (HasFieldAttributeL(aAttribute, field))
                {
                MVPbkStoreContactField* clone = field.CloneLC();
                result->AppendL(clone);
                CleanupStack::Pop(); // clone
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::HasContactAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::HasFieldAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::FindFieldsWithAttributeL(
        TUid /*aAttributeType*/,
        MVPbkStoreContact& /*aContact*/,
        MVPbkMultiAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CSpeedDialAttributeManager::FindFieldsWithAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkStoreContact& /*aContact*/,
        MVPbkMultiAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

} // namespace VPbkCntModel

// End of File
