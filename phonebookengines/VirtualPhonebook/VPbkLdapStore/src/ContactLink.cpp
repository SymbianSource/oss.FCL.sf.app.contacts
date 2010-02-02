/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactLink - Contact link implementation
*
*/


// INCLUDE FILES
#include "contactlink.h"
#include "contact.h"
#include "contactstore.h"
#include "contactview.h"
#include "viewcontact.h"
#include <s32strm.h>            // Streaming

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactLink::CContactLink
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactLink::CContactLink(CContactStore& aStore,TInt aIndex,TInt aType)
:   iStore(aStore), iIndex(aIndex), iType(aType)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CContactLink::NewLC
// CContactLink two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactLink* CContactLink::NewLC(CContactStore& aStore,TInt aIndex,TInt aType)
    {
    CContactLink* self = new (ELeave) CContactLink(aStore,aIndex,aType);
    CleanupStack::PushL(self);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactLink::NewL
// CContactLink two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactLink* CContactLink::NewL(CContactStore& aStore,TInt aIndex,TInt aType)
    {
    CContactLink* self = CContactLink::NewLC(aStore,aIndex,aType);
    CleanupStack::Pop(self);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactLink::~CContactLink
// CContactLink Destructor
// -----------------------------------------------------------------------------
//
CContactLink::~CContactLink()
    {
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactLink implementation
// -----------------------------------------------------------------------------
// CContactLink::Index
// Returns linked contact index.
// -----------------------------------------------------------------------------
//
TInt CContactLink::Index() const
    {
    return iIndex;
    }

// -----------------------------------------------------------------------------
// CContactLink::Type
// Returns linked contact field type id.
// -----------------------------------------------------------------------------
//
TInt CContactLink::Type() const
    {
    return iIndex;
    }

// -----------------------------------------------------------------------------
// CContactLink::Store
// Returns linked store.
// -----------------------------------------------------------------------------
//
CContactStore& CContactLink::Store() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactLink implementation
// -----------------------------------------------------------------------------
// CContactLink::ContactStore
// Returns linked store.
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContactLink::ContactStore() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
// CContactLink::IsSame
// Returns true if this link refers to passed link contact.
// -----------------------------------------------------------------------------
//
TBool CContactLink::IsSame(const MVPbkContactLink& aOther) const
    {
    TBool same = EFalse;
    // From same store
    if (&ContactStore() == &aOther.ContactStore())
        {
        // Cast to contact link
        const CContactLink& other = static_cast<const CContactLink&>(aOther);
        // Same index
        same = (other.Index() == Index());
        }
    return same;
    }

// -----------------------------------------------------------------------------
// CContactLink::RefersTo
// Returns true if this link refers to the contact.
// -----------------------------------------------------------------------------
//
TBool CContactLink::RefersTo(const MVPbkBaseContact& aContact) const
    {
    TBool same = EFalse;
    // Contact
    if (const CContact* contact = dynamic_cast<const CContact*>(&aContact))
        {
        // From same store
        if (&ContactStore() == &contact->ParentStore())
            {
            // Same index
            same = (contact->Index() == Index());
            }
        }
    else
    // View contact
    if (const CViewContact* contact = dynamic_cast<const CViewContact*>(&aContact))
        {
        // From same store
        if (&ContactStore() == &contact->ContactView().Store())
            {
            // Same index
            same = (contact->ContactIndex() == Index());
            }
        }
    return same;
    }

// -----------------------------------------------------------------------------
// CContactLink::Packing
// Returns packing interface
// -----------------------------------------------------------------------------
//
const MVPbkContactLinkPacking& CContactLink::Packing() const
    {
    return *this;
    }
    
// -----------------------------------------------------------------------------
// CContactLink::CloneLC
// Clones this link
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContactLink::CloneLC() const
    {
    return CContactLink::NewLC(iStore,iIndex,iType);
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactLinkPacking implementation
// -----------------------------------------------------------------------------
// CContactLink::InternalPackedSize
// Returns the packed size of internals.
// -----------------------------------------------------------------------------
//
TInt CContactLink::InternalPackedSize() const
    {
    // Integer index + integer type
    return (4 + 4);
    }
// -----------------------------------------------------------------------------
// CContactLink::PackInternalsL
// Packs internals
// -----------------------------------------------------------------------------
//
void CContactLink::PackInternalsL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iIndex);
    aStream.WriteInt32L(iType);
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
