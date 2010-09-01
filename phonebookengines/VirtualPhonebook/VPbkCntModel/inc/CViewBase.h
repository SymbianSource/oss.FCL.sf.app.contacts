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
* Description:  Contacts Model store contact view implementation.
*
*/


#ifndef VPBKCNTMODEL_CVIEWBASE_H
#define VPBKCNTMODEL_CVIEWBASE_H

// INCLUDES
#include <e32base.h>
#include <cntviewbase.h>
#include <MVPbkContactView.h>
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactStoreObserver.h>
#include "CViewContact.h"
#include "MCustomContactViewObserver.h"
#include "MParentViewForFiltering.h"

// FORWARD DECLARATIONS
class MVPbkContactViewObserver;
class CVPbkSortOrder;
class CVPbkContactViewDefinition;
template<class MVPbkContactViewObserver> class CVPbkAsyncObjectOperation;
template<class MFilteredViewSupportObserver> class CVPbkAsyncObjectOperation;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;
class CContactLink;

/**
 * Contacts Model store contact view implementation
 */
NONSHARABLE_CLASS( CViewBase ): public CBase,
                                public MParentViewForFiltering,
                                public MVPbkContactViewFiltering,
                                public MVPbkContactStoreObserver,
                                public MContactViewObserver,
                                protected MCustomContactViewObserver
    {
    public: // Destructor

        /**
         * Destructor.
         */
        virtual ~CViewBase();

    public: // Interface

        /**
         * Returns parent contact store.
         *
         * @return  Parent contact store.
         */
        CContactStore& Store() const;

        /**
         * Returns native view.
         *
         * @return  Native view.
         */
        CContactViewBase& NativeView() const;
        
        /**
         * Return view's state
         *
         * @return ETrue is view is ready other EFalse
         */
        TBool State() const;
        
        /**
         * Returns view definition
         *
         * @return View definition
         */
        CVPbkContactViewDefinition& ViewDefinition() const;
        
    protected: // Interface
        /**
         * Change sort order
         * @param aSortOrder
         */
        void ChangeSortOrderL( RContactViewSortOrder& aSortOrder );

    public: // From MVPbkContactViewBase
        void ChangeSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
        const MVPbkFieldTypeList& SortOrder() const;
        void RefreshL();
        virtual TInt ContactCountL() const;
        virtual const MVPbkViewContact& ContactAtL(
                TInt aIndex ) const;
        MVPbkContactLink* CreateLinkLC(
                TInt aIndex ) const;
        TInt IndexOfLinkL(
                const MVPbkContactLink& aContactLink ) const;
        void AddObserverL(
                MVPbkContactViewObserver& aObserver );
        void RemoveObserver(
                MVPbkContactViewObserver& aObserver );
        TBool MatchContactStore(
                const TDesC& aContactStoreUri ) const;
        TBool MatchContactStoreDomain(
                const TDesC& aContactStoreDomain ) const;
        MVPbkContactBookmark* CreateBookmarkLC(
                TInt aIndex ) const;
        TInt IndexOfBookmarkL(
                const MVPbkContactBookmark& aContactBookmark ) const;
        MVPbkContactViewFiltering* ViewFiltering();
   
   public: // From MParentViewForFiltering
        void AddFilteringObserverL( MFilteredViewSupportObserver& aObserver );
        void RemoveFilteringObserver( MFilteredViewSupportObserver& aObserver );
        
   public: // From MVPbkContactViewFiltering         
        MVPbkContactViewBase* CreateFilteredViewLC( 
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        void UpdateFilterL( 
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );

    public: // From MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    public: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent );

    protected: // From MContactViewObserver
        void HandleContactViewEvent(
                const CContactViewBase& aView,
                const TContactViewEvent& aEvent );

    private: // From MCustomContactViewObserver
        void HandleCustomContactViewEvent(
                const CContactViewBase& aView,
                const TContactViewEvent& aEvent );
    
    protected: // Implementation
        CViewBase(
                CContactStore& aParentStore );
        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver, 
                const MVPbkFieldTypeList& aSortOrder );
        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                const MVPbkFieldTypeList& aSortOrder );
        void DoUpdateTypeListL();                
    private: // Implementation
        RContactViewSortOrder CreateSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
        void DoAddObserverL(
                MVPbkContactViewObserver& aObserver );
        void AddObserverError(
                MVPbkContactViewObserver& aObserver, TInt aError );
        void DoAddFilteringObserverL(
                MFilteredViewSupportObserver& aObserver );
        void DoAddFilteringObserverError(
                MFilteredViewSupportObserver& aObserver, TInt aError );
        void InitializeViewL(
                const CVPbkContactViewDefinition& aViewDefinition,
                const MVPbkFieldTypeList& aSortOrder );
        void TeardownView();
        void SendViewErrorEvent( TInt aError );
        void SendViewStateEvent();
        void PurgeAsyncOperations();
        void LeaveIfIncorrectSortOrderL( 
                const MVPbkFieldTypeList& aSortOrder );
        
        virtual void DoInitializeViewL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& viewSortOrder ) = 0;
        virtual void DoTeardownView() = 0;
        virtual TBool DoChangeSortOrderL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aSortOrder ) = 0;

    protected: // Data
        /// Own: Contacts view
        CContactViewBase* iView;
        /// Own: Array of observers
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Ref: an array of observers that filters this view.
        RPointerArray<MFilteredViewSupportObserver> iFilteringObservers;
        /// Own: View sort order
        CVPbkSortOrder* iSortOrder;
        
    private: // Data
        /// Ref: Parent contact store
        CContactStore& iParentStore;
        /// Own: The defintion of the view from client
        CVPbkContactViewDefinition* iViewDefinition;
        /// Own: Link to a contact associated with last event
        CContactLink* iEventLink;
        /// Own: View ready indicator
        TBool iViewReady;
        /// Own: an async operation for MVPbkContactViewObserver
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>* iObserverOp;
        /// Own: an async operation for MFilteredViewSupportObserver
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>* iFilterObsOp;
        /// Own: A contact last queried
        mutable CViewContact* iCurrentContact;
    };


// INLINE FUNCTIONS

inline CContactStore& CViewBase::Store() const
    {
    return iParentStore;
    }

inline CContactViewBase& CViewBase::NativeView() const
    {
    return *iView;
    }
    
inline TBool CViewBase::State() const
    {
    return iViewReady;
    }
    
inline void CViewBase::ChangeSortOrderL( RContactViewSortOrder& aSortOrder )
    {
    DoChangeSortOrderL( *iViewDefinition, aSortOrder );
    }

inline CVPbkContactViewDefinition& CViewBase::ViewDefinition() const
	{
	return *iViewDefinition;
	}
} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CVIEWBASE_H

// End of File
