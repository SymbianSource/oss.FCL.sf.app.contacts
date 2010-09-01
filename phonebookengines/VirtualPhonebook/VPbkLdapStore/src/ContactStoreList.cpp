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
* Description:  CContactStoreList implementation
*
*/


// INCLUDE FILES
#include "contactstorelist.h"
#include "contactstore.h"

#include <mvpbkcontactstorelistobserver.h>
#include <mvpbkcontactstoreproperties.h>

#include <tvpbkcontactstoreuriptr.h>
#include <cvpbkcontactstoreuriarray.h>

// CONSTANTS
_LIT(KLDAPDomain,"ldap");

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactStoreList::CContactStoreList
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStoreList::CContactStoreList(CContactStoreDomain& aStoreDomain)
    : iStoreDomain(aStoreDomain)
    {
    }

// -----------------------------------------------------------------------------
// CContactStoreList::ConstructL
// CContactStoreList constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactStoreList::ConstructL(const CVPbkContactStoreUriArray& aURIList)
    {
    // Initialize stores in list
    if (aURIList.Count() > 0)
        {
        // Load stores
        for (TInt loop = 0;loop < aURIList.Count();loop++)
            {
            // Is this ours
            if (aURIList[loop].Compare(KLDAPDomain, 
                TVPbkContactStoreUriPtr::EContactStoreUriStoreType) == 0)
                {
                // Our store
                CContactStore* store = CContactStore::NewLC(aURIList[loop],iStoreDomain);
                iStores.AppendL(store);
                CleanupStack::Pop(store);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CContactStoreList::NewLC
// CContactStoreList two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreList* CContactStoreList::NewLC
(
    const CVPbkContactStoreUriArray& aURIList,
    CContactStoreDomain& aStoreDomain
)
    {
    CContactStoreList* self = new (ELeave)CContactStoreList(aStoreDomain);
    CleanupStack::PushL(self);
    self->ConstructL(aURIList);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactStoreList::NewL
// CContactStoreList two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreList* CContactStoreList::NewL
(
    const CVPbkContactStoreUriArray& aURIList,
    CContactStoreDomain& aStoreDomain
)
    {
    CContactStoreList* self = CContactStoreList::NewLC(aURIList,aStoreDomain);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::~CContactStoreList
// CContactStoreList Destructor
// -----------------------------------------------------------------------------
//
CContactStoreList::~CContactStoreList()
    {
    iStores.ResetAndDestroy();
    iStores.Close();
    }

// -----------------------------------------------------------------------------
//                      CContactStoreList public methods
// -----------------------------------------------------------------------------
// CContactStoreList::AppendL
// -----------------------------------------------------------------------------
//
void CContactStoreList::AppendL(CContactStore* aContactStore)
    {
    iStores.AppendL(aContactStore);    
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactStoreList implmentation
// -----------------------------------------------------------------------------
// CContactStoreList::Count
// Returns number of stores in this list
// -----------------------------------------------------------------------------
//
TInt CContactStoreList::Count() const
    {
    return iStores.Count();
    }

// -----------------------------------------------------------------------------
// CContactStoreList::At
// Returns contact store at index
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContactStoreList::At(TInt aIndex) const
    {
    return *iStores[aIndex];
    }

// -----------------------------------------------------------------------------
// CContactStoreList::Find
// Finds contact store from this list
// -----------------------------------------------------------------------------
//
MVPbkContactStore* CContactStoreList::Find(const TVPbkContactStoreUriPtr& aUri) const
    {
    MVPbkContactStore* store = NULL;
    // Stores in list
    const TInt count = iStores.Count();
    // Find matching url
    for (TInt loop = 0;loop < count;loop++)
        {
        // Url matches
        if (iStores[loop]->StoreProperties().Uri().Compare(
                aUri,TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            store = iStores[loop];
            break;
            }
        }
    return store;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::OpenAllL
// Opens all contact stores in this list
// -----------------------------------------------------------------------------
//
void CContactStoreList::OpenAllL(MVPbkContactStoreListObserver& aObserver)
    {
    // Already opening
    if (iObserver)
        {
        User::Leave(KErrInUse);
        }
    // Set oberver
    iObserver = &aObserver;
    iOpenCount = 0;

    // Open all
    const TInt count = Count();
    for (TInt loop = 0; loop < count;loop++)
        At(loop).OpenL(*this);
    }

// -----------------------------------------------------------------------------
// CContactStoreList::CloseAll
// Closes all contact stores in this list
// -----------------------------------------------------------------------------
//
void CContactStoreList::CloseAll(MVPbkContactStoreListObserver& /*aObserver*/)
    {
    // Close all
    const TInt count = Count();
    for (TInt loop = 0;loop < count;loop++)
        At(loop).Close(*this);
    // Clear observer
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactStoreObserver implmentation
// -----------------------------------------------------------------------------
// CContactStoreList::StoreReady
// Called when a contact store in this list is ready to use.
// -----------------------------------------------------------------------------
//
void CContactStoreList::StoreReady(MVPbkContactStore& aContactStore)
    {
    iOpenCount++;
    iObserver->StoreReady(aContactStore);
    NotifyIfAllStoresReady();
    }

// -----------------------------------------------------------------------------
// CContactStoreList::StoreUnavailable
// Called when a contact store becomes unavailable.
// -----------------------------------------------------------------------------
//
void CContactStoreList::StoreUnavailable
(
    MVPbkContactStore& aContactStore,
    TInt aReason
)
    {
    iOpenCount++;
    iObserver->StoreUnavailable(aContactStore,aReason);
    NotifyIfAllStoresReady();
    }

// -----------------------------------------------------------------------------
// CContactStoreList::HandleStoreEventL
// Called when changes occur in the contact store.
// -----------------------------------------------------------------------------
//
void CContactStoreList::HandleStoreEventL
(
    MVPbkContactStore&   /* aContactStore */,
    TVPbkContactStoreEvent /* aStoreEvent */
)
    {
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactOperationFactory implmentation
// -----------------------------------------------------------------------------
// CContactStoreList::CreateContactRetrieverL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreList::CreateContactRetrieverL
( 
    const MVPbkContactLink& aLink,
    MVPbkSingleContactOperationObserver& aObserver
)
    {
    // Store count
    const TInt count = iStores.Count();
    // Find the store
    for (TInt loop = 0;loop < count;loop++)
        {
        // Create operation
        MVPbkContactOperation* op =
            iStores[loop]->CreateContactRetrieverL(aLink,aObserver);
        if (op)
            {
            return op;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreList::CreateDeleteContactsOperationL
(
    const MVPbkContactLinkArray& /* aContactLinks */, 
    MVPbkBatchOperationObserver& /* aObserver */
)
    {
    // LDAP Store is read only
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::CreateCommitContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreList::CreateCommitContactsOperationL
(
    const TArray<MVPbkStoreContact*>& /* aContacts */,
    MVPbkBatchOperationObserver&      /* aObserver */
)
    {
    // LDAP Store is read only
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreList::CreateMatchPhoneNumberOperationL
(
    const TDesC&           /* aPhoneNumber */,
    TInt                /* aMaxMatchDigits */,
    MVPbkContactFindObserver& /* aObserver */
)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStoreList::CreateFindOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreList::CreateFindOperationL
(
    const TDesC&            /* aSearchString */,
    const MVPbkFieldTypeList& /* aFieldTypes */,
    MVPbkContactFindObserver& /*   aObserver */
)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
//                      CContactStoreList private methods
// -----------------------------------------------------------------------------
// CContactStoreList::NotifyIfAllStoresReady
// -----------------------------------------------------------------------------
//
void CContactStoreList::NotifyIfAllStoresReady()
    {
    if (iOpenCount >= Count())
        {
        iObserver->OpenComplete();
        iObserver = NULL;
        }
    }

} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
