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
* Description:  The virtual phonebook view implementation
*
*/


#ifndef VPBKSIMSTORE_CCONTACTVIEW_H
#define VPBKSIMSTORE_CCONTACTVIEW_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactViewFiltering.h>
#include <CVPbkAsyncOperation.h>
#include <MVPbkSimViewObserver.h>
#include "MParentViewForFiltering.h"

// FORWARD DECLARATIONS
class MVPbkContactViewObserver;
class CVPbkSortOrder;
class CVPbkContactViewDefinition;
template<class MVPbkContactViewObserver> class CVPbkAsyncObjectOperation;
template<class MFilteredViewSupportObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;
class CViewContact;

/**
*  The virtual phonebook view implementation
*
*/
NONSHARABLE_CLASS( CContactView ): 
        public CBase,
        public MParentViewForFiltering,
        protected MVPbkSimViewObserver,
        protected MVPbkContactViewFiltering
    {
    public: // Types
        /**
        * States of the view
        */
        enum TViewState
            {
            ENotReady,
            EReady,
            ENotAvailable
            };

    public: // Construction and destruction

        /**
        * Two-phased constructor.
        * @param aParentStore the parent store of the view
        * @param aSortOrder the VPbk sort order
        * @param aViewDefinition the defintion for the view creation
        * @return a new instance of this class
        */
        static CContactView* NewLC(MVPbkContactViewObserver& aObserver,
            CContactStore& aParentStore, 
            const MVPbkFieldTypeList& aSortOrder,
            const CVPbkContactViewDefinition& aViewDefinition);

        /**
        * Destructor
        */
        ~CContactView();

    public: // New functions

        /**
        * Returns the contact store
        * @return the contact store
        */
        inline CContactStore& Store();

        /**
        * Returns the current state of the view
        */
        inline TViewState State() const;
        
        /**
        * Return the native view
        */
        inline MVPbkSimCntView& NativeView() const;

    public: // Functions from base classes
    
        /**
        * From MVPbkObjectHierarchy
        */
        MVPbkObjectHierarchy& ParentObject() const;

        /**
        * From MVPbkContactViewBase
        */
        void RefreshL();

        /**
        * From MVPbkContactViewBase
        */
        TVPbkContactViewType Type() const;

        /**
        * From MVPbkContactViewBase
        */
        void ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder);

        /**
        * From MVPbkContactViewBase
        */
        const MVPbkFieldTypeList& SortOrder() const;

        /**
        * From MVPbkContactViewBase
        */
        TInt ContactCountL() const;

        /**
        * From MVPbkContactViewBase
        */
        const MVPbkViewContact& ContactAtL(TInt aIndex) const;

        /**
        * From MVPbkContactViewBase
        */
        MVPbkContactLink* CreateLinkLC( TInt aIndex ) const;

        /**
        * From MVPbkContactViewBase
        */
        TInt IndexOfLinkL( const MVPbkContactLink& aContactLink ) const;

        /**
        * From MVPbkContactViewBase
        */
        void AddObserverL(MVPbkContactViewObserver& aObserver);

        /**
        * From MVPbkContactViewBase
        */
        void RemoveObserver(MVPbkContactViewObserver& aObserver);
        
        /**
        * From MVPbkContactViewBase
        */
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        
        /**
        * From MVPbkContactViewBase
        */
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
        
        /**
        * From MVPbkContactViewBase
        */
        MVPbkContactBookmark* CreateBookmarkLC(TInt aIndex) const;
        
        /**
        * From MVPbkContactViewBase
        */
        TInt IndexOfBookmarkL(
            const MVPbkContactBookmark& aContactBookmark) const;
                            
        /**
        * From MVPbkContactViewBase
        */
        MVPbkContactViewFiltering* ViewFiltering();
        
        /**
        * From MParentViewForFiltering
        */
        void AddFilteringObserverL( MFilteredViewSupportObserver& aObserver );
        
        /**
        * From MParentViewForFiltering
        */
        void RemoveFilteringObserver( MFilteredViewSupportObserver& aObserver );
        
        /**
        * From MParentViewForFiltering
        */
        TBool IsNativeMatchingRequestActive();
         
        /**
        * From MVPbkContactViewFiltering
        */
        MVPbkContactViewBase* CreateFilteredViewLC( 
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        
        /**
        * From MVPbkContactViewFiltering
        */
        void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
            
    protected:  // Functions from base classes

        /**
        * From MVPbkSimViewObserver
        */
        void ViewReady( MVPbkSimCntView& aView );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewError( MVPbkSimCntView& aView, TInt aError );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewNotAvailable( MVPbkSimCntView& aView );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewContactEvent( TEvent aEvent, TInt aIndex, TInt aSimIndex );

    private: // Construction

        /**
        * C++ constructor
        */
        CContactView( CContactStore& aParentStore );

        /**
        * 2nd phase contstructor
        */
        void ConstructL(const MVPbkFieldTypeList& aSortOrder,
            const CVPbkContactViewDefinition& aViewDefinition);
    
    private:    // New functions
        /// Gets sim index of the view contact
        TInt InspectSortOrderL();
        TInt MapViewIndexToSimIndexL( TInt aViewIndex ) const;
        void DoAddObserverL(
                MVPbkContactViewObserver& aObserver );
        void AddObserverError(
                MVPbkContactViewObserver& aObserver, TInt aError );
        void DoAddFilteringObserverL(
                MFilteredViewSupportObserver& aObserver );
        void DoAddFilteringObserverError(
                MFilteredViewSupportObserver& aObserver, TInt aError );
        void PurgeObserverCallbacks();
        void SendViewStateEvent();
        void SendViewErrorEvent( TInt aError );
        void LeaveIfIncorrectSortOrderL( 
                const MVPbkFieldTypeList& aSortOrder );
        
    private: // Data
        CContactStore& iParentStore;
        /// An array for the observers of view
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Ref: an array of observers that filters this view.
        RPointerArray<MFilteredViewSupportObserver> iFilteringObservers;
        /// Own: the remote view
        MVPbkSimCntView* iNativeView;
        /// Own: the current view contact
        CViewContact* iCurrentContact;
        MVPbkViewContact* iDummyContact;
        /// Own: the sortorder of the view
        CVPbkSortOrder* iSortOrder;
        /// Own: an async operation for MVPbkContactViewObserver
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>* iObserverOp;
        /// Own: an async operation for MFilteredViewSupportObserver
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>* iFilterObsOp;
        /// The current state of the view
        TViewState iViewState;
        /// Own: Saved view definition information
        CVPbkContactViewDefinition* iViewDefinition;
    };

// INLINE FUNCTIONS
inline CContactStore& CContactView::Store()
    {
    return iParentStore;
    }

inline CContactView::TViewState CContactView::State() const
    {
    return iViewState;
    }
    
inline MVPbkSimCntView& CContactView::NativeView() const
    {
    return *iNativeView;
    }
} // namespace VPbkSimStore

#endif // VPBKSIMSTORE_CCONTACTVIEW_H

// End of file
