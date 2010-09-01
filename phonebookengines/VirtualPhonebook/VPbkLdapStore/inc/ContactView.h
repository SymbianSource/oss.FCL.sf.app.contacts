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
* Description:  CContactView - Contact store view.
*
*/


#ifndef __CONTACTVIEW_H__
#define __CONTACTVIEW_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkcontactview.h>
#include <mvpbkcontactstoreobserver.h>

// FORWARD DECLARATIONS
class CVPbkSortOrder;
class TVPbkContactStoreEvent;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// View state
enum TViewState
    {
    ENotReady,
    EReady,
    ENotAvailable
    };

// FORWARD DECLARATIONS
class CContactStore;
class CViewContact;
class TContactViewObserver;

/**
*  CContactView - contact store view.
*  
*/
class CContactView : public CBase,
                     public MVPbkContactView,
                     public MVPbkContactStoreObserver
    {
    public:     // CContactView public constructors and destructor
        /**
        @function   NewL
        @discussion Create CContactView object
        @param      aObserver  View observer
        @param      aStore     View store
        @param      aSortOrder Sort order
        @param      aType      View type
        @return     Pointer to instance of CContactView
        */
        static CContactView* NewL(MVPbkContactViewObserver& aObserver,
            CContactStore& aStore,const MVPbkFieldTypeList& aSortOrder,
            TVPbkContactViewType aType);

        /**
        @function   NewLC
        @discussion Create CContactView object
        @param      aObserver  View observer
        @param      aStore     View store
        @param      aSortOrder Sort order
        @param      aType      View type
        @return     Pointer to instance of CContactView
        */
        static CContactView* NewLC(MVPbkContactViewObserver& aObserver,
            CContactStore& aStore,const MVPbkFieldTypeList& aSortOrder,
            TVPbkContactViewType aType);

        /**
        @function   ~CContactView
        @discussion CContactView destructor
        */
        ~CContactView();

    public:     // CContactView public methods

        /**
        @function   NotifyObservers
        @discussion Notify observers view state changes
        @return     True or false
        */
        TInt NotifyObservers();

        /**
        @function   State
        @discussion Returns view state
        @return     View state
        */
        TViewState State() const;

        /**
        @function   Store
        @discussion Returns view store
        @return     View store
        */
        CContactStore& Store();

    public:     // Methods from MVPbkContactViewBase

        /**
        @function   Type
        @discussion Returns type of this contact view.
        @return     Type of this contact view.
        */
        TVPbkContactViewType Type() const;

        /**
        @function   ChangeSortOrderL
        @discussion Changes sort order of the view.
        @param      aSortOrder New sort order for this view.
        */
        void ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder);

        /**
        @function   SortOrder
        @discussion Returns the current sort order of the view.
        @return     The sort order of this contact view.
        */
        const MVPbkFieldTypeList& SortOrder() const;

        /**
        @function   RefreshL
        @discussion Refreshes the view contents.
        */
        void RefreshL();

        /**
        @function   ContactCountL
        @discussion Returns the number of contacts in this view.
        @return     Number of contacts in this view.
        */
        TInt ContactCountL() const;

        /**
        @function   ContactAtL
        @discussion Returns a contact in this view.
        @param      aIndex Index in this view of the contact to return.
        @return     Contact at index
        */
        const MVPbkViewContact& ContactAtL(TInt aIndex) const;

        /**
        @function   CreateLinkLC
        @discussion Returns a contact link at index
        @param      aIndex Index in this view of the contact to return.
        @return     Contact link at index
        */
        MVPbkContactLink* CreateLinkLC(TInt aIndex) const;

        /**
        @function   IndexOfLinkL
        @discussion Returns index of contact link
        @param      aContactLink Link whose index is searched.
        @return     Index of the link or KErrNotFound.
        */
        TInt IndexOfLinkL(const MVPbkContactLink& aContactLink) const;

        /**
        @function   AddObserverL
        @discussion Adds an observer to this contact view.
        @param      aObserver New observer
        */
        void AddObserverL(MVPbkContactViewObserver& aObserver);

        /**
        @function   RemoveObserver
        @discussion Removes observer from this contact view
        @param      aObserver Observer to be removed
        */
        void RemoveObserver(MVPbkContactViewObserver& aObserver);

        /**
        @function   MatchContactStore
        @discussion Checks whether this view is from given store.
        @param      aContactStoreUri the URI of the store to compare
        @return     ETrue if the view was from the given store
        */
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;

        /**
        @function   MatchContactStoreDomain
        @discussion Checks whether this view is from given store domain.
        @param      aContactStoreDomain the the store domain
        @return     ETrue if the view was from the same store domain.
        */
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;

        /**
        @function   CreateBookmarkLC
        @discussion Creates and returns bookmark to the item at given index.
        @param      aIndex Index of the item in the view.
        @return     New bookmark to the view item.
        */
        MVPbkContactBookmark* CreateBookmarkLC(TInt aIndex) const;

        /**
        @function   IndexOfBookmarkL
        @discussion Returns an index of contact in the view.
        @param      aContactBookmark Bookmark to get index
        @return     Index of the contact bookmark points to.
        */
        TInt IndexOfBookmarkL(
            const MVPbkContactBookmark& aContactBookmark) const;
        
        /**
        * TODO comment
        */    
        MVPbkContactViewFiltering* ViewFiltering();
            

    public:     // Methods from MVPbkObjectHierarchy

        /**
        @function   ParentObject
        @discussion Returns the parent object of this view.
        @return     Parent object
        */
        MVPbkObjectHierarchy& ParentObject() const;

    public:     // Methods from MVPbkContactStoreObserver

        /**
        @function   StoreReady
        @discussion Called when contact store is ready to use.
        @param      aContactStore Store
        */
        void StoreReady(MVPbkContactStore& aContactStore);

        /**
        @function   StoreUnavailable
        @discussion Called when contact store becomes unavailable.
        @param      aContactStore Store
        @param      aReason       Reason
        */
        void StoreUnavailable(MVPbkContactStore& aContactStore,TInt aReason);

        /**
        @function   HandleStoreEventL
        @discussion Called when contact store becomes unavailable.
        @param      aContactStore Store
        @param      aStoreEvent   Event
        */
        void HandleStoreEventL(MVPbkContactStore& aContactStore,
                            TVPbkContactStoreEvent aStoreEvent);

    private:    // CContactView private constructors

        /**
        @function   CContactView
        @discussion CContactView default contructor
        @param      aStore View store
        @param      aType  View type
        */
        CContactView(CContactStore& aStore,TVPbkContactViewType aType);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactView 
        @param      aSortOrder View sort order
        */
        void ConstructL(const MVPbkFieldTypeList& aSortOrder);

    private:    // CContactView private methods

        /**
        @function   FindObserver
        @discussion Finds observer 
        @param      aObserver Observer to find
        @return     Index of found observer or not found error.
        */
        TInt FindObserver(MVPbkContactViewObserver& aObserver);

    private:    // CContactView private member variables

        /// View store reference
        CContactStore& iStore;

        /// Sort order
        CVPbkSortOrder* iSortOrder;

        /// Contact
        CViewContact *iContact;

        /// View observers
        CArrayPtr<TContactViewObserver>* iObservers;

        /// View type
        TVPbkContactViewType iType;

        /// View state
        TViewState iViewState;

        /// For view state change callback
        CIdle* iIdle;
    };

} // End of namespace LDAPStore
#endif // __CONTACTVIEW_H__
