/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Sim store contact view implementation.
*
*/


#ifndef VPBKSIMSTORE_CFINDVIEWBASE_H
#define VPBKSIMSTORE_CFINDVIEWBASE_H

// INCLUDES
#include <e32base.h>

#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactViewObserver.h>
#include <badesca.h>
#include <VPbkSimCntFieldTypes.hrh>
#include "MParentViewForFiltering.h"

// FORWARD DECLARATIONS
class MVPbkSimContact;
class MVPbkSimStoreOperation;
class MVPbkContactFindPolicy;
class CVPbkFieldTypeRefsList;
template<class MVPbkContactViewObserver> class CVPbkAsyncObjectOperation;
template<class MFilteredViewSupportObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class MParentViewForFiltering;
class CContactView;
class CViewContact;

/**
 * An interface for checking if the contact is one of the
 * always included contacts
 */
NONSHARABLE_CLASS( MAlwaysIncludedContacts )
    {
    public: 
        /**
         * Return ETrue aContact is always included
         */
        virtual TBool IsContactAlwaysIncluded( 
            const CViewContact& aContact ) const = 0;
    
    protected:
        ~MAlwaysIncludedContacts() {}
    };

/**
 * A base class for filtered views that implements most of
 * the MVPbkContactView functions.
 */
NONSHARABLE_CLASS( CFindViewBase ): public CBase,
                                    public MParentViewForFiltering,
                                    public MFilteredViewSupportObserver,
                                    public MVPbkContactViewFiltering,
                                    protected MAlwaysIncludedContacts
    {
    public: // Constructor and destructor
        /**
         * Destructor.
         */
        ~CFindViewBase();    

    public: //New functions
        
        /**
         * Starts matching asynchrnously. Sends event
         * using MVPbkContactViewObserver when the find is ready.
         */ 
        void ActivateContactMatchL();
                                            
    public: // Functions from MVPbkContactView
        MVPbkObjectHierarchy& ParentObject() const;
        void RefreshL();
        TInt ContactCountL() const;
        const MVPbkViewContact& ContactAtL(
                TInt aIndex ) const;  
        MVPbkContactLink* CreateLinkLC(
                TInt aIndex ) const;
        TInt IndexOfLinkL(
                const MVPbkContactLink& aContactLink ) const;
        TVPbkContactViewType Type() const;
        void ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder);
        const MVPbkFieldTypeList& SortOrder() const;        
        void AddObserverL(MVPbkContactViewObserver& aObserver);
        void RemoveObserver(MVPbkContactViewObserver& aObserver);        
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
        MVPbkContactBookmark* CreateBookmarkLC(
                TInt aIndex ) const;
        TInt IndexOfBookmarkL(
                const MVPbkContactBookmark& aContactBookmark ) const;
        MVPbkContactViewFiltering* ViewFiltering();
    
    public: // From MParentViewForFiltering
        void AddFilteringObserverL( MFilteredViewSupportObserver& aObserver );
        void RemoveFilteringObserver( MFilteredViewSupportObserver& aObserver );
        
    public: // Functions from MVPbkContactViewFiltering
        MVPbkContactViewBase* CreateFilteredViewLC( 
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        /// Subclasses must implement these
        virtual void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts ) = 0;
        virtual TBool IsNativeMatchingRequestActive() = 0;
                
    public: // From MVPbkContactViewObserver
        virtual void ContactViewReady( MVPbkContactViewBase& aView ) = 0;
        void ContactViewUnavailable( MVPbkContactViewBase& aView );
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
                TBool aErrorNotified );
        /**
         * Subclass handles the logic after the parent view is ready
         * for filtering
         */
        virtual void ContactViewReadyForFiltering( 
                MParentViewForFiltering& aView ) = 0;
        void ContactViewUnavailableForFiltering( 
                MParentViewForFiltering& aView );
                      
    protected: // Implementation
        /**
         * @param aParentView the view whose contacts are filtered
         * @param aAllContactsView the all contacts view.
         * @param aOwnsMatchedContacts ETrue if this instance owns
         *          contacts in iMatchedContacts
         */
        CFindViewBase( MParentViewForFiltering& aParentView,
            CContactView& aAllContactsView,
            TBool aOwnsMatchedContacts );
        
        /**
         * @param aExternalViewObserver an observer that was given
         *        by the Virtual Phonebook client
         * @param aFindString the find words for filtering
         * @param aFindPolicy a policy for matching.
         */
        void BaseConstructL( MVPbkContactViewObserver& aExternalViewObserver,
            const MDesCArray& aFindStrings,
            MVPbkContactFindPolicy& aFindPolicy );
        
        /**
         * Start synchrnous contact match.
         * Calls first MatchL and then sends events to obserers.
         */
        void MatchContactsL();
        
        /**
         * Checks if aViewContact matches to iFindStrings
         *
         * @param aViewContact the contact to be cheked
         */
        TBool IsMatchL( const MVPbkViewContact& aViewContact );
        
        /**
         * Returns the find words used in filtering
         *
         * @return the find words used in filtering
         */
        const MDesCArray& FindStrings() const;
        
        /**
         * Deletes old find strings and allocates new ones based on
         * aFindStrings.
         *
         * @param aFindStrings new find strings
         */
        void SetFindStringsL( const MDesCArray& aFindStrings );
        
        /**
         * Sends ViewReady or ViewUnavailable event depending on view state
         */
        void SendViewStateEventToObservers();
                
    private: // New functions
        /**
         * Subclass defines the match logic. This is called from
         * MatchContactsL.
         *
         * @param aMatchedContacts an array that is filled by subclass.
         */
        virtual void MatchL( 
                RPointerArray<MVPbkSimContact>& aMatchedContacts ) = 0;
        
        /**
         * Subclass handles matching of added contact and adds it
         * to the correct location in aMatchedContacts.
         *
         * @see MVPbkContactViewObserver::ContactAddedToView
         * @param aMatchedContacts an array of contacts in which the new
         *                         contact is inserted if it matches.
         */
        virtual void DoContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink,
                RPointerArray<MVPbkSimContact>& aMatchedContacts ) = 0;
    
    private: // Implementation
        void DoAddObserver( MVPbkContactViewObserver& aObserver );
        void DoAddObserverError( MVPbkContactViewObserver& aObserver,
                TInt aError );
        void DoAddFilteringObserverL( MFilteredViewSupportObserver& aObserver );
        void DoAddFilteringObserverError( 
                MFilteredViewSupportObserver& aObserver, TInt aError );
        void HandleContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink );
        void HandleContactRemovedFromViewL( MVPbkContactViewBase& aView, 
                TInt aIndex, const MVPbkContactLink& aContactLink );
        void ResetContacts();
        
    protected: // Data
        /// Ref: The view that cretead this filtered view
        MParentViewForFiltering& iParentView;
        /// Ref: The all contacts view
        CContactView& iAllContactsView;
        
    private: // Data    
        /// Own: find words from the client
        CDesCArrayFlat* iFindStrings;
        /// Own or Not Own: depends on iOwnsContacts
        RPointerArray<MVPbkSimContact> iMatchedContacts;
        /// Own: the current view contact
        CViewContact* iCurrentContact;
        /// Ref: a policy for match
        MVPbkContactFindPolicy* iContactFindPolicy;
        /// Own: a field type list for name construction policy
        CVPbkFieldTypeRefsList* iFieldTypeRefsList;
        /// Ref: an array of view observers
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Ref: an array of observers that filters this view.
        RPointerArray<MFilteredViewSupportObserver> iFilteringObservers;
        /// Own: an async operation for MVPbkContactViewObserver
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>* iObserverOp;
        /// Own: an async operation for MFilteredViewSupportObserver
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>* iFilterObsOp;
        /// Own: ETrue if this view owns contacts in iMatchedContacts
        TBool iOwnsContacts;
        /// Own: ETrue if this view is ready
        TBool iViewReady;
    };
    
} // namespace VPbkSimStore    

#endif // VPBKSIMSTORE_CFINDVIEWBASE_H

// End of File
