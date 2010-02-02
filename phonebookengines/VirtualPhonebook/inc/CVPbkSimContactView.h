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
* Description:  The contact view
*
*/



#ifndef CVPBKSIMCONTACTVIEW_H
#define CVPBKSIMCONTACTVIEW_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCntView.h>
#include <MVPbkSimStoreObserver.h>
#include <RVPbkStreamedIntArray.h>
#include <VPbkSimStoreCommon.h>

// FORWARD DECLARATIONS
class MVPbkSimViewObserver;
class CVPbkSimContactBuf;
class CVPbkSimFieldTypeFilter;
class MVPbkSimViewFindObserver;
class MDesCArray;
class MVPbkSimContact;
template <class MVPbkSimViewObserver> class CVPbkAsyncObjectOperation;

// FORWARD DECLARATIONS
namespace VPbkSimStoreImpl {
class CSimCntSortUtil;
}

// CLASS DECLARATION

/**
 *  A class for a sorted view from a store. View can be optionally filtered
 *  and it also can have name that identifies it.
 */
class CVPbkSimContactView :     
        public CBase,
        public MVPbkSimCntView,
        private MVPbkSimStoreObserver
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         *
         * @param aSortOrder            The sort order for the view.
         * @param aConstructionPolicy   Defines how the view is constructed.
         * @param aParentStore          The parent store of the view.
         * @param aViewName             The name of the view. Use KNullDesC
         *                              for unnamed view.
         * @param aFilter               Field type filter.
         * @return  A new instance of this class.
         */
        IMPORT_C static CVPbkSimContactView* NewLC(
                const RVPbkSimFieldTypeArray& aSortOrder,
                TVPbkSimViewConstructionPolicy aConstructionPolicy,
                MVPbkSimCntStore& aParentStore, 
                const TDesC& aViewName,
                CVPbkSimFieldTypeFilter* aFilter );
        
        /**
         * Destructor.
         */
        virtual ~CVPbkSimContactView();
            
    public: // From MVPbkSimCntView
        const TDesC& Name() const;
        MVPbkSimCntStore& ParentStore() const;
        void OpenL(
                MVPbkSimViewObserver& aObserver );
        void Close(
                MVPbkSimViewObserver& aObserver );
        TInt CountL() const;
        MVPbkSimContact& ContactAtL(
                TInt aIndex );
        void ChangeSortOrderL(
                const RVPbkSimFieldTypeArray& aSortOrder );
        TInt MapSimIndexToViewIndexL(
                TInt aSimIndex );
        MVPbkSimStoreOperation* ContactMatchingPrefixL(
                const MDesCArray& aFindStrings, 
                MVPbkSimViewFindObserver& aObserver );
        const RVPbkSimFieldTypeArray& SortOrderL() const;
    
    private: // From MVPbkSimStoreObserver
        void StoreReady(
                MVPbkSimCntStore& aStore );
        void StoreError(
                MVPbkSimCntStore& aStore, TInt aError );
        void StoreNotAvailable(
                MVPbkSimCntStore& aStore );
        void StoreContactEvent(
                TEvent aEvent, TInt aSimIndex );

    private: // Types
        /// A simple contact for the view.
        /// The view is a list of sim indexes in certain order.
        struct TViewContact
            {
            TViewContact( TInt aSimIndex ) : iSimIndex( aSimIndex ) {}
            TInt iSimIndex;
            };
            
    private: // Implementation
        CVPbkSimContactView( MVPbkSimCntStore& aParentStore,
                TVPbkSimViewConstructionPolicy aConstructionPolicy );
        void ConstructL(
                const RVPbkSimFieldTypeArray& aSortOrder, 
                const TDesC& aViewName );
        /// Calls SortL in TRAP
        void Sort();
        /// Does the view sorting
        void SortL();
        /// Handle SortL failure
        TInt SortError( TInt aError );
        TBool IsSorting() const;
        /// Starts asynchronous sorting
        void StartSorting();
        /// A handler for StoreContactEvent
        void HandleStoreContactEventL( TEvent aEvent, TInt aSimIndex );
        void HandleContactAddedL( TInt aSimIndex );
        void HandleContactRemoved( TInt aSimIndex );
        void HandleContactChangedL( TInt aSimIndex );
        /// Inserts a new view contact to the correct place in the array
        TInt InsertViewContactL(
                const MVPbkSimContact& aContact );
        /// Finds the index for the new contact if the view is sorted
        TInt SortedIndexL(
                const MVPbkSimContact& aLeft );
        /// Finds the index for the new contact if the view is not sorted
        TInt UnsortedIndex(
                const MVPbkSimContact& aLeft );
        /// Removes the view contact to aSimIndex, Returns removed view index.
        TInt RemoveViewContact(
                TInt aSimIndex );
        /// Resets the current sort order and copies types from the source
        void ResetAndCopySortOrderL(
                const RVPbkSimFieldTypeArray& aSource );
        /// Resets the view
        void Reset();
        /// Inspects whether the contact passes the filtering criteria
        TBool PassesFilter(
                const MVPbkSimContact& aContact,
                const CVPbkSimFieldTypeFilter* aFilter ) const;
        // View state changes
        void SetToReadyState();
        void SetToNotReadyState();
        void SetToUnavailableState();
        TBool ViewStateReady() const;
        TBool ViewStateUnavailable() const;
        TBool ViewStateNotReady() const;
        /// A function called by iAsyncOperation
        void DoViewOpenCallbackL( MVPbkSimViewObserver& aObserver );
        /// A function called by iAsyncOperation
        void DoViewUnavailableCallbackL( MVPbkSimViewObserver& aObserver );
        /// A function called by iAsyncOperation
        void DoViewErrorCallback( MVPbkSimViewObserver& aObserver, 
                TInt aError );
        static TBool CompareViewContactSimIndex( const TViewContact& aLeft,
                const TViewContact& aRight );
        static TInt IdleSortCallback( TAny* aThis );
        
    private: // Data
        /// Own: The name of view that is given by the client
        HBufC* iViewName;
        /// Ref: The parent store of the view
        MVPbkSimCntStore& iParentStore;
        /// Own: The sort order of this view
        RVPbkSimFieldTypeArray iSortOrder;
        /// Own: The observer of the view
        RPointerArray<MVPbkSimViewObserver> iObservers;
        /// Own: An array of view contacts
        RArray<TViewContact> iViewContacts;
        /// Own: The sorting utility
        VPbkSimStoreImpl::CSimCntSortUtil* iSortUtil;
        /// Own: The current sim contact read with ContactAtL
        CVPbkSimContactBuf* iCurrentContact;
        /// Own: Defines how the view is created
        TVPbkSimViewConstructionPolicy iConstructionPolicy;
        /// Own: State indicator
        TInt iViewState;
        /// Own: Async operations for the opening the view
        CVPbkAsyncObjectOperation<MVPbkSimViewObserver>* iAsyncOperation;
        /// Own: Field type filter
        CVPbkSimFieldTypeFilter* iFilter;
        /// Own: A contact that can be used
        CVPbkSimContactBuf* iTempContactForSorting;
        /// Own: An idle for starting async sorting.
        CIdle* iIdleSort;
    };

#endif  // CVPBKSIMCONTACTVIEW_H

// End of File
