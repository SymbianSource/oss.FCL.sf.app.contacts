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
* Description:  The virtual phonebook contact link
*
*/


// INCLUDES
#include "CContactLink.h"
#include "CContactStore.h"
#include "CContact.h"
#include "CViewContact.h"
#include "CViewBase.h"

#include <cntitem.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkStoreContact.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactStoreProperties.h>

namespace {

/**
 * Link version supported by this version.
 */
const TUint8 KContactLinkVersion = 1;

} // namespace

namespace VPbkCntModel {

// CContactLink implementation
CContactLink::CContactLink(CContactStore& aContactStore) :
    iContactStore(aContactStore),
    iFieldId(KErrNotFound)
    {
    }

void CContactLink::CommonConstructL()
    {
    iLinkArray = CVPbkContactLinkArray::NewL();
    iLinkArray->AppendL(this);
    }
    
/**
 * The Contact model link internal specification.
 * Specifies link content and order.
 * 1st item: version numer (byte)
 * 2nd item: contact item id (word)
 * 3rd item: contact item field id (word)
 */
inline void CContactLink::ConstructL(
        RReadStream& aStream)
    {
    CommonConstructL();
    /*TUint8 version = */aStream.ReadUint8L();
    iId = aStream.ReadInt32L(); // TContactItemId
    iFieldId = aStream.ReadInt32L(); // TInt
    
    }

inline void CContactLink::ConstructL(
        TContactItemId aId,
        TInt aFieldId)
    {
    CommonConstructL();
    iId = aId;
    iFieldId = aFieldId;
    }

CContactLink* CContactLink::NewLC
        (CContactStore& aContactStore,
        RReadStream& aStream)
    {
    CContactLink* self = new(ELeave) CContactLink(aContactStore);
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    return self;
    }

CContactLink* CContactLink::NewLC
        (CContactStore& aContactStore, 
        TContactItemId aId,
        TInt aFieldId /* = KErrNotFound */)
    {
    CContactLink* self = new(ELeave) CContactLink(aContactStore);
    CleanupStack::PushL(self);
    self->ConstructL(aId, aFieldId);
    return self;
    }

CContactLink::~CContactLink()
    {
    if (iLinkArray)
        {
        iLinkArray->Reset();
        delete iLinkArray;
        }
    }

TBool CContactLink::Matches(
        const TDesC& aStoreUri, 
        const MVPbkContactLink& aLink,
        TVPbkContactStoreUriPtr::TVPbkContactStoreUriComponent aComponent)
    {
    if (aLink.ContactStore().StoreProperties().Uri().Compare(aStoreUri, aComponent) == 0) 
        {
        return ETrue;
        }
    return EFalse;
    }

TBool CContactLink::Matches(
        const TVPbkContactStoreUriPtr& aStoreUri,
        const MVPbkContactLink& aLink)
    {
    if (aLink.ContactStore().StoreProperties().Uri().Compare(aStoreUri, 
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0) 
        {
        return ETrue;
        }
    return EFalse;
    }

TContactItemId CContactLink::ContactId() const
    {
    return iId;
    }

TInt CContactLink::ContactFieldId() const
    {
    return iFieldId;
    }

void CContactLink::SetContactId(TContactItemId aContactId)
    {
    iId = aContactId;
    }

void CContactLink::SetContactFieldId(TInt aContactFieldId)
    {
    iFieldId = aContactFieldId;
    }

const TPtrC CContactLink::ContactDbFileName() const
    {
    return iContactStore.StoreProperties().Uri().Component(
            TVPbkContactStoreUriPtr::EContactStoreUriStoreLocation);
    }

MVPbkContactStore& CContactLink::ContactStore() const
    {
    return iContactStore;
    }

TBool CContactLink::IsSame(const MVPbkContactLink& aOther) const
    {
    TBool result = EFalse;
    if (&ContactStore() == &aOther.ContactStore())
        {
        const CContactLink& other = 
            static_cast<const CContactLink&>(aOther);
        result = (other.ContactId() == ContactId() &&
                  other.ContactFieldId() == ContactFieldId());
        }
    return result;
    }
    
TBool CContactLink::RefersTo(const MVPbkBaseContact& aContact) const
    {
    TBool result = EFalse;
    if (const CContact* contact = dynamic_cast<const CContact*>(&aContact))
        {
        if (&ContactStore() == &contact->ParentStore())
            {
            result = (contact->NativeContact()->Id() == ContactId());
            }
        }
    else if(const CViewContact* contact = dynamic_cast<const CViewContact*>(&aContact))
        {
        if (&ContactStore() == &contact->View().ContactStore())
            {
            result = (contact->Id() == ContactId());
            }
        }
    return result;
    }

const MVPbkStreamable* CContactLink::Streamable() const
    {
    return this;
    }

const MVPbkContactLinkPacking& CContactLink::Packing() const
    {
    return *this;
    }
    
MVPbkContactLink* CContactLink::CloneLC() const
    {
    return CContactLink::NewLC(iContactStore, iId, iFieldId);
    }

void CContactLink::ExternalizeL(
        RWriteStream& aStream) const
    {
    HBufC8* buf = iLinkArray->PackLC();
    aStream.WriteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

TInt CContactLink::ExternalizedSize() const
    {
    return iLinkArray->PackedBufferSize();
    }

TInt CContactLink::InternalPackedSize() const
    {
    return (1 /*Link version number */ +
            4 /* Contact item id */ +
            4 /* Contact field id */);
    }

void CContactLink::PackInternalsL(RWriteStream& aStream) const
    {
    aStream.WriteUint8L(KContactLinkVersion); // version number
    aStream.WriteInt32L(iId); // contact item id
    aStream.WriteInt32L(iFieldId);
    }

} // namespace VPbkCntModel

// end of file
