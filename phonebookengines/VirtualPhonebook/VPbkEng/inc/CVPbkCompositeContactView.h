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
* Description:  Composite contact view.
*
*/


#ifndef CVPBKCOMPOSITECONTACTVIEW_H
#define CVPBKCOMPOSITECONTACTVIEW_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactViewBaseChildAccessExtension.h>
#include <VPbkContactView.hrh>
#include <CVPbkAsyncOperation.h>
#include "CVPbkEventArrayItem.h"

// FORWARD DECLARATIONS
class MVPbkContactViewSortPolicy;
class CVPbkEventArrayItem;
class MVPbkCompositeContactViewPolicy;


/**
 * Virtual Phonebook abstract composite contact view.
 * Composite contact views can be used to compose a view from
 * multiple sub views.
 */
NONSHARABLE_CLASS(CVPbkCompositeContactView) :
        public CBase,
        public MVPbkContactViewBase,
        public MVPbkContactViewObserver,
        public MVPbkContactViewObserverExtension,
        protected MVPbkContactViewFiltering,
        public MVPbkContactViewBaseChildAccessExtension
	{
    public: // Constructors and destructor
        /**
         * Destructor.
         */
        ~CVPbkCompositeContactView();

    public: // Interface
        /**
         * Adds subview to this composite view. Ownership is transferred
         * if leave does not occur.
         * NOTE: This composite view must already be an observer of aSubView
         *       when calling this function.
         *
         * @param aSubView Subview to add to this composite view.
         * @param aViewId ViewId of added subview
         */
        void AddSubViewL(
                MVPbkContactViewBase* aSubView, TInt  aViewId = KVPbkDefaultViewId);

        /**
         * ActualContactCountL returns the number of contact's in subviews.
         * It might be that all subview events haven't
         * arrived yet to this composite so ActualContactCountL is not always
         * same as CompositeContactCountL
         *
         * @return the contact count calculated from subviews.
         */
        TInt ActualContactCountL() const;

        /**
         * @return The number of contacts in iContactMapping
         */
        inline TInt CompositeContactCountL() const;

        /**
         * If the parent of this view is also a composite view
         * it calls this to configure this view as an internal view
         * that has no external observers.
         */
        void ApplyInternalCompositePolicyL();

        void SetViewId(TInt aViewId);

    private: // Abstract interface for composite implementations
        /**
         * Builds view mapping.
         */
        virtual void DoBuildContactMappingL() = 0;

        /**
         * Handles contact addition to the view.
         *
         * @param aSubViewIndex     Sub view index.
         * @param aIndex            Index of the contact in the sub view.
         * @return  Composite contact view index of the added contact.
         */
        virtual TInt DoHandleContactAdditionL(
                TInt aSubViewIndex,
                TInt aIndex ) = 0;

    public: // From MVPbkContactViewBase
        TVPbkContactViewType Type() const;
        void ChangeSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
        const MVPbkFieldTypeList& SortOrder() const;
        void RefreshL();
        TInt ContactCountL() const;
        const MVPbkViewContact& ContactAtL(
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
        TAny* ContactViewBaseExtension(TUid aExtensionUid);

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified);
        TAny* ContactViewObserverExtension( TUid aExtensionUid );

    private: // From MVPbkContactViewObserverExtension
        void FilteredContactRemovedFromView(
			 	MVPbkContactViewBase& aView );

    private: // From MVPbkContactViewFiltering
        /// Composite implementations must implement this
        virtual MVPbkContactViewBase* CreateFilteredViewLC(
            MVPbkContactViewObserver& aObserver,
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts ) = 0;
        void UpdateFilterL(
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );

    private: // From MVPbkContactViewBaseChildAccessExtension
        TInt ChildViewCount() const;
        MVPbkContactViewBase& ChildViewAt(TInt aIndex);
        MVPbkContactViewBase* GetChildViewById(TInt aId);
        TInt GetViewId();

    protected: // Types
        class CSubViewData;

        /**
         * Contact mapping structure. Mapping supports 128 subviews
         * and over 8 million contacts per sub view.
         */
        struct TContactMapping
            {
            /// Own: View index
            TInt iViewIndex: 8;
            /// Own: Contact index
            TInt iContactIndex: 24;
            };

    protected: // Implementation
        CVPbkCompositeContactView();
        void BaseConstructL(
                const MVPbkFieldTypeList& aSortOrder );
        static TBool CompareMappings(
                const TContactMapping& aLhs,
                const TContactMapping& aRhs );
        TBool IsCompositeReady() const;
        /// Returns the composite's view policy.
        MVPbkCompositeContactViewPolicy& CompositePolicy() const;

    private: // Implementation
        TInt FindSubViewIndex(MVPbkContactViewBase& aView) const;
        TBool AllSubViewsKnown() const;
        TBool AnySubViewReady() const;
        void ResetContactMapping();
        void RemoveContactMappingsFromView(MVPbkContactViewBase& aView);
        void HandleContactViewReadyL( MVPbkContactViewBase& aView );
        void HandleContactViewUnavailableL( MVPbkContactViewBase& aView );
        TInt HandleContactRemoval(
                MVPbkContactViewBase& aView,
                TInt aIndex );
        TInt HandleContactAddition(
                MVPbkContactViewBase& aView,
                TInt aIndex );
        void DoAddObserverL(MVPbkContactViewObserver& aObserver);
        void AddObserverError(
                MVPbkContactViewObserver& aObserver,
                TInt aError );
        void SendViewReadyEvent();
        void SendViewUnavailableEvent();
        void SendViewErrorEvent( TInt aError, TBool aErrorNotified );
        void UpdateSortOrderL();

    protected: // Data
        /// Own: Array of subviews
        RPointerArray<CSubViewData> iSubViews;
        /// Own: Array of contact mappings
        RArray<TContactMapping> iContactMapping;

    private: // Data
        /// Ref: Async operation
        VPbkEngUtils::CVPbkAsyncOperation iAsyncOperation;
        /// Own: Sort order
        MVPbkFieldTypeList* iSortOrder;
        /// Own: Array of observers
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Own: A policy for composite's internal functionality
        MVPbkCompositeContactViewPolicy* iCompositePolicy;
        /// Current view identifier
        TInt iViewId;
    };

inline TInt CVPbkCompositeContactView::CompositeContactCountL() const
    {
    return iContactMapping.Count();
    }

/**
 * Class to hold subview data.
 */
NONSHARABLE_CLASS( CVPbkCompositeContactView::CSubViewData ):
        public CBase
    {
    public: // Types
        /**
         * View state enumeration.
         */
        enum TViewState
            {
            ENotKnown,
            EUnavailable,
            EReady
            };

    public: // Constructors and destructor
        /**
         * Constructor.
         *
         * @param aState    View state.
         */
        CSubViewData(TViewState aState) : iState( aState ) {}

        /**
         * Destructor.
         */
        ~CSubViewData();

    public: // Data
        /// Ref: Contact view
        MVPbkContactViewBase* iView;
        /// Own: View state
        TViewState iState;
        /// Ref: Cursor pointer, used when merging
        const MVPbkViewContact* iContactCursor;
        /// Own: Cursor
        TInt iCursor;
        /// view identifier of iView member variable
        TInt  iViewId;
    };

/**
 * Internal interface for different behavior of exterior and interior
 * composite views.
 */
NONSHARABLE_CLASS(MVPbkCompositeContactViewPolicy)
    {
    public:
        /**
         * Destructor
         */
        virtual ~MVPbkCompositeContactViewPolicy() {}

        /**
         * Handles contact added & removed events.
         */
        virtual void HandleViewEventsL(
            CVPbkEventArrayItem::TViewEventType aEvent,
            MVPbkContactViewBase& aSubview,
            TInt aIndex, const
            MVPbkContactLink& aContactLink ) = 0;

        /**
         * Reset policy if it has cached data.
         */
        virtual void Reset() = 0;

        /**
         * Returns the contact count in composite or 0 if
         * composite is not up to date with leaf views.
         * @param aCompositeCount The count according to composite view.
         * @return aCompositeCount or 0.
         */
        virtual TInt ContactCountL() const = 0;

        /**
         * Returns ETrue if this is an internal composite policy
         * @return ETrue if this is an internal composite policy
         */
        virtual TBool InternalPolicy() const = 0;
    };

/**
 * An implementation for exterior composite view
 */
NONSHARABLE_CLASS(CVPbkExternalCompositeViewPolicy) :
        public CBase,
        public MVPbkCompositeContactViewPolicy
    {
    public: // Construction and destruction
        static CVPbkExternalCompositeViewPolicy* NewL(
                CVPbkCompositeContactView& aCompositeView,
                RPointerArray<MVPbkContactViewObserver>& aObservers );
        ~CVPbkExternalCompositeViewPolicy();

    private: // From MVPbkCompositeContactViewPolicy
        void HandleViewEventsL(
            CVPbkEventArrayItem::TViewEventType aEvent,
            MVPbkContactViewBase& aSubview,
            TInt aIndex, const
            MVPbkContactLink& aContactLink );
        void Reset();
        TInt ContactCountL() const;
        TBool InternalPolicy() const;

    private: // Implementation
        CVPbkExternalCompositeViewPolicy(
            CVPbkCompositeContactView& aCompositeView,
            RPointerArray<MVPbkContactViewObserver>& aObservers );

    private: // Data
        /// Ref: The parent of this policy
        CVPbkCompositeContactView& iCompositeView;
        /// Ref: Array of observers
        RPointerArray<MVPbkContactViewObserver>& iObservers;
        /// Own: Event array for 'Contact added or removed' events
        RPointerArray<CVPbkEventArrayItem> iEventArray;
    };

/**
 * An implementation for interior composite view
 */
NONSHARABLE_CLASS(CVPbkInternalCompositeViewPolicy) :
        public CBase,
        public MVPbkCompositeContactViewPolicy
    {
    public: // Construction and destruction
        static CVPbkInternalCompositeViewPolicy* NewL(
                CVPbkCompositeContactView& aCompositeView,
                RPointerArray<MVPbkContactViewObserver>& aObservers );

    private: // From MVPbkCompositeContactViewPolicy
        void HandleViewEventsL(
            CVPbkEventArrayItem::TViewEventType aEvent,
            MVPbkContactViewBase& aSubview,
            TInt aIndex, const
            MVPbkContactLink& aContactLink );
        void Reset();
        TInt ContactCountL() const;
        TBool InternalPolicy() const;

    private: // Implementation
        CVPbkInternalCompositeViewPolicy(
            CVPbkCompositeContactView& aCompositeView,
            RPointerArray<MVPbkContactViewObserver>& aObservers );

    private: // Data
        /// Ref: The parent of this policy
        CVPbkCompositeContactView& iCompositeView;
        /// Ref: Array of observers
        RPointerArray<MVPbkContactViewObserver>& iObservers;
    };

#endif // CVPBKCOMPOSITECONTACTVIEW_H

// End of File
