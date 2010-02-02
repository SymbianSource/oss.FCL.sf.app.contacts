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
* Description:  CContactView implementation
*
*/


// INCLUDE FILES
#include "contactview.h"
#include "contactlink.h"
#include "contactstore.h"
#include "storebookmark.h"
#include "viewcontact.h"

#include <cvpbksortorder.h>
#include <vpbkcontactview.hrh>
#include <mvpbkcontactviewobserver.h>
#include <mvpbkcontactbookmark.h>

// CONSTANTS
static const TInt KObserverArrayGranularity = 8;

// -----------------------------------------------------------------------------
// LDAPNotifyObservers
// Contact view state notifier callback
// -----------------------------------------------------------------------------
//
static TInt LDAPNotifyObservers(TAny* aThis)
    {
    if (aThis)
        {
        return static_cast<LDAPStore::CContactView*>(aThis)->NotifyObservers();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

/**
*  TContactViewObserver - contact view observer class.
*  
*/
class TContactViewObserver
    {
    public:     // TContactViewObserver public constructor
        /**
        @function   TContactViewObserver
        @discussion TContactViewObserver contructor
        @param      aObserver View observer
        */
        inline TContactViewObserver(MVPbkContactViewObserver& aObserver)
            : iNotifiedState(ENotReady), iObserver(aObserver)
            {
            };
    public:     // TContactViewObserver public members

        /**
        @function   NotifiedState
        @discussion Returns notified state
        @return     Notified view state
        */
        inline TViewState NotifiedState() const {
            return iNotifiedState;
        }
        /**
        @function   SetNotifiedState
        @discussion Returns notified state
        @parame     aState Notified view state
        */
        inline void SetNotifiedState(TViewState aState) {
            iNotifiedState = aState;
        }

        /**
        @function   Observer
        @discussion Returns observer
        @return     Observer
        */
        inline MVPbkContactViewObserver& Observer() const {
            return iObserver;
        }

    private:    // TContactViewObserver private members

        // Notified state
        TViewState iNotifiedState;

        // Observer
        MVPbkContactViewObserver& iObserver;
    };

// -----------------------------------------------------------------------------
// CContactView::CContactView
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactView::CContactView(CContactStore& aStore,TVPbkContactViewType aType)
:   iStore(aStore), iType(aType), iViewState(ENotReady)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CContactView::ConstructL
// CContactView constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactView::ConstructL(const MVPbkFieldTypeList& aSortOrder)
    {
    // Observers
    iObservers = new (ELeave)
        CArrayPtrFlat<TContactViewObserver>(KObserverArrayGranularity);

    // Copy the sort order
    iSortOrder = CVPbkSortOrder::NewL(aSortOrder);

    // Create contact
    iContact = CViewContact::NewL(*this,*iSortOrder);

    // Create view state notify handler
    iIdle = CIdle::NewL(CActive::EPriorityIdle);

    // Store initialized and folding view
    if (iStore.IsInitialized() && iType == EVPbkFoldingView)
        {
        // View is ready when store is
        iViewState = EReady;
        }
    else
        {
        // Add store observer
        iStore.AddObserverL(*this);
        }
    }

// -----------------------------------------------------------------------------
// CContactView::NewLC
// CContactView two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactView* CContactView::NewLC(
    MVPbkContactViewObserver& aObserver,
    CContactStore&               aStore,
    const MVPbkFieldTypeList& aSortOrder,
    TVPbkContactViewType           aType
)
    {
    CContactView* self = new (ELeave)CContactView(aStore,aType);
    CleanupStack::PushL(self);
    self->ConstructL(aSortOrder);
    self->AddObserverL(aObserver);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactView::NewL
// CContactView two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactView* CContactView::NewL(
    MVPbkContactViewObserver& aObserver,
    CContactStore&               aStore,
    const MVPbkFieldTypeList& aSortOrder,
    TVPbkContactViewType           aType
)
    {
    CContactView* self = CContactView::NewLC(aObserver,aStore,aSortOrder,aType);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactView::~CContactView
// CContactView Destructor
// -----------------------------------------------------------------------------
//
CContactView::~CContactView()
    {
    // Callback
    if (iIdle)
        {
        // Always cancel pending operations
        iIdle->Cancel();
        // Delete
        delete iIdle;
        iIdle = NULL;
        }
    // Store contact
    if (iContact)
        {
        delete iContact;
        iContact = NULL;
        }
    // Sort order
    if (iSortOrder)
        {
        delete iSortOrder;
        iSortOrder = NULL;
        }
    // Observers
    if (iObservers)
        {
        iObservers->ResetAndDestroy();
        delete iObservers;
        iObservers = NULL;
        }
    }

// -----------------------------------------------------------------------------
//                      CContactView public methods
// -----------------------------------------------------------------------------
// CContactView::NotifyObservers
// Notifies observers about view state changes
// -----------------------------------------------------------------------------
//
TInt CContactView::NotifyObservers()
    {
    TInt status = ETrue;

    // Observers
    const TInt count = (iObservers) ? iObservers->Count() : 0;

    // View state
    TViewState state = State();
    if (state != EReady)
        {
        // Store has contacts - view ready
        if (iStore.ContactCount() > 0 &&
            iStore.State() == CContactStore::ERetrieved)
            {
            // Set ready
            iViewState = EReady; state = EReady;
            }
        }

    // Notify all
    for (TInt loop = iObservers->Count() - 1; loop >= 0;loop--)
        {
        // This observer
        TContactViewObserver* op = iObservers->At(loop);
        if (op && op->NotifiedState() != state)
            {
            // View ready
            if (state == EReady)
                {
                op->Observer().ContactViewReady(*this);
                }
            else
            // Not available
            if (state == ENotAvailable)
                {
                op->Observer().ContactViewUnavailable(*this);
                }
            // Set state
            op->SetNotifiedState(state);            
            }
        }

    // Observers
    if (count == 0)
        {
        status = EFalse;
        }
    // True to continue notification or false when break
    return status;
    }

// -----------------------------------------------------------------------------
// CContactView::State
// Returns view state
// -----------------------------------------------------------------------------
//
TViewState CContactView::State() const
    {
    return iViewState;
    }
// -----------------------------------------------------------------------------
// CContactView::State
// Returns view store
// -----------------------------------------------------------------------------
//
CContactStore& CContactView::Store()
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactViewBase implementation
// -----------------------------------------------------------------------------
// CContactView::Type
// Returns type of this contact view.
// -----------------------------------------------------------------------------
//
TVPbkContactViewType CContactView::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CContactView::ChangeSortOrderL
// Changes sort order of the view. 
// -----------------------------------------------------------------------------
//
void CContactView::ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder)
    {
    // This is store view sort order does not apply, but
    CVPbkSortOrder* sp = CVPbkSortOrder::NewL(aSortOrder);
    delete iSortOrder;
    iSortOrder = sp;
    }
// -----------------------------------------------------------------------------
// CContactView::SortOrder
// Returns the current sort order of the view.
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactView::SortOrder() const
    {
    return *iSortOrder;
    }

// -----------------------------------------------------------------------------
// CContactView::RefreshL
// Refreshes the view contents.
// -----------------------------------------------------------------------------
//
void CContactView::RefreshL()
    {
    // Rebuild view
    }

// -----------------------------------------------------------------------------
// CContactView::ContactCountL
// Returns the number of contacts in this view
// -----------------------------------------------------------------------------
//
TInt CContactView::ContactCountL() const
    {
    // Contact count
    TInt count = 0;
    // Contacts view
    if (iType == EVPbkContactsView)
        {
        // Get from store
        count = iStore.ContactCount();
        }
    else
    // Folding view, one store
    if (iType == EVPbkFoldingView)
        {
        count = 1;
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CContactView::ContactAtL
// Returns a contact in this view.
// -----------------------------------------------------------------------------
//
const MVPbkViewContact& CContactView::ContactAtL(TInt aIndex) const
    {
    // Contacts view
    if (iType == EVPbkContactsView)
        {
        // Set contact
        iContact->SetContactViewContact(iStore.ContactAt(aIndex));
        }
    else
    // Folding view
    if (iType == EVPbkFoldingView)
        {
        // Store name as contact
        iContact->SetFoldingViewContact(iStore.Name());
        }
    return *iContact; 
    }

// -----------------------------------------------------------------------------
// CContactView::CreateLinkLC
// Returns a contact link at index
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContactView::CreateLinkLC(TInt aIndex) const
    {
    // Create link to contact or store
    return CContactLink::NewLC(iStore,aIndex);
    }

// -----------------------------------------------------------------------------
// CContactView::IndexOfLinkL
// -----------------------------------------------------------------------------
//
TInt CContactView::IndexOfLinkL(const MVPbkContactLink& aContactLink) const
    {
    TInt index = KErrNotFound;
    // Same store
    if (&aContactLink.ContactStore() == &iStore)
        {
        // Contacts view
        if (iType == EVPbkContactsView)
            {
            // Get from link
            index = static_cast<const CContactLink&>(aContactLink).Index();
            }
        else
        // Folding view, one store
        if (iType == EVPbkFoldingView)
            {
            index = 0;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CContactView::AddObserverL
// -----------------------------------------------------------------------------
//
void CContactView::AddObserverL(MVPbkContactViewObserver& aObserver)
    {
    TInt index = FindObserver(aObserver);
    if (index == KErrNotFound && iObservers)
        {
        // New view observer
        TContactViewObserver* op = new (ELeave) TContactViewObserver(aObserver);
        CleanupStack::PushL(op);
        iObservers->AppendL(op);
        CleanupStack::Pop(op);
        }
    // Callback handler has not been started
    if (! iIdle->IsActive())
        {
        // Notify observer callback
        TCallBack callback(LDAPNotifyObservers,this);
        // Start callback
        iIdle->Start(callback);
        }
    }
// -----------------------------------------------------------------------------
// CContactView::RemoveObserver
// -----------------------------------------------------------------------------
//
void CContactView::RemoveObserver(MVPbkContactViewObserver& aObserver)
    {
    TInt index = FindObserver(aObserver);
    if (index != KErrNotFound)
        {
        // Observer
        TContactViewObserver* op = iObservers->At(index);
        // Remove from observers
        iObservers->Delete(index);
        // Delete observer
        if (op) delete op;
        }
    }

// -----------------------------------------------------------------------------
// CContactView::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CContactView::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iStore.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CContactView::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CContactView::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    return iStore.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CContactView::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CContactView::CreateBookmarkLC(TInt aIndex) const
    {
    MVPbkContactBookmark* bp = NULL;
    // Contacts view
    if (iType == EVPbkContactsView)
        {
        // Index in range
        if (aIndex >= 0 && aIndex < iStore.ContactCount())
            {
            // Bookmark contact in store
            bp = CStoreBookmark::NewLC(iStore,aIndex);
            }
        else
            {
            // Bookmark store
            bp = CStoreBookmark::NewLC(iStore);
            }
        }
    else
    // Folding view, one store
    if (iType == EVPbkFoldingView)
        {
        // Bookmark store
        bp = CStoreBookmark::NewLC(iStore);
        }
    return bp;
    }

// -----------------------------------------------------------------------------
// CContactView::IndexOfBookmarkL
// -----------------------------------------------------------------------------
//
TInt CContactView::IndexOfBookmarkL(const MVPbkContactBookmark& aBookmark) const
    {
    TInt index = KErrNotFound;
    // Contacts view
    if (iType == EVPbkContactsView)
        {
        // Cast to bookmark
        const CStoreBookmark* bp = dynamic_cast<const CStoreBookmark*>(&aBookmark);
        if ( bp && &bp->Store == &iStore )
            {
            // Bookmarked index
            TInt bookmark = (bp) ? bp->Index() : 0;
            if (bookmark >= 0 && bookmark < iStore.ContactCount())
                {
                index = bookmark;
                }            
            }
        }
    else
    // Folding view, one store
    if (iType == EVPbkFoldingView)
        {
        // Store bookmark index is zero
        index = 0;
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CContactView::ViewFiltering
// -----------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CContactView::ViewFiltering()
    {
    // TODO
    return NULL;
    }
    
// -----------------------------------------------------------------------------
//                  MVPbkObjectHierarchy implementation
// -----------------------------------------------------------------------------
// CContactView::ParentObject
// Returns the parent object of this view, contact store
// -----------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CContactView::ParentObject() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactStoreObserver implementation
// -----------------------------------------------------------------------------
// CViewContact::StoreReady
// -----------------------------------------------------------------------------
//
void CContactView::StoreReady(MVPbkContactStore& /* aContactStore */)
    {
    // Contacts view
    if (iType == EVPbkContactsView)
        {
        iViewState = EReady;
        }
    else
    // Folding view, one store
    if (iType == EVPbkFoldingView)
        {
        iViewState = EReady;
        }
    }
// -----------------------------------------------------------------------------
// CContactView::StoreUnavailable
// -----------------------------------------------------------------------------
//
void CContactView::StoreUnavailable(MVPbkContactStore& /* aContactStore */,
                                                        TInt /* aReason */)
    {
    iViewState = ENotAvailable;
    }
// -----------------------------------------------------------------------------
// CContactView::HandleStoreEventL
// -----------------------------------------------------------------------------
//
void CContactView::HandleStoreEventL(MVPbkContactStore&   /* aContactStore */,
                                     TVPbkContactStoreEvent /* aStoreEvent */)
    {
    // No need to handle this here, events are received from the view
    }

// -----------------------------------------------------------------------------
//                      CContactView private methods
// -----------------------------------------------------------------------------
// CContactView::FindObserver
// Find observer
// -----------------------------------------------------------------------------
//
TInt CContactView::FindObserver(MVPbkContactViewObserver& aObserver)
    {
    TInt found = KErrNotFound;
    // Number of observers
    const TInt count = (iObservers) ? iObservers->Count() : 0;
    // Find one
    for (TInt loop = 0; loop < count;loop++)
        {
        // This observer
        TContactViewObserver* op = iObservers->At(loop);
        if (op && &op->Observer() == &aObserver)
            {
            found = loop;
            break;
            }
        }
    return found;
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
