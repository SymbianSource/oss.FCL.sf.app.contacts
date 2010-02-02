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


#ifndef VPBKCNTMODEL_CFINDVIEWBASE_H
#define VPBKCNTMODEL_CFINDVIEWBASE_H

// INCLUDES

#include <e32base.h>
#include <cntdef.h>
#include <badesca.h>
#include <cntviewbase.h>

// From VPbkEng
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactViewObserver.h>

// From VPbkCntModel
#include "MParentViewForFiltering.h"

// FORWARD DECLARATIONS
class CVPbkFieldTypeRefsList;
class MVPbkContactFindPolicy;
class CContactIdArray;
template<class MVPbkContactViewObserver> class CVPbkAsyncObjectOperation;
template<class MFilteredViewSupportObserver> class CVPbkAsyncObjectOperation;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CViewContact;
class CViewBase;

/**
 * An interface for checking if the contact is one of the
 * always included contacts
 */
NONSHARABLE_CLASS( MAlwaysIncludedContacts )
    {
    public: 
        /**
         * Checks if contact is always included
         * @param aContact Contact to be checked
         * @return ETrue if aContact is always included
         */
        virtual TBool IsContactAlwaysIncluded( 
            const CViewContact& aContact ) const = 0;
    
    protected:
        ~MAlwaysIncludedContacts() {}
    };

/**
 * Declared only for using static protected CompareFieldsL in
 * Symbian's CContactViewBase.
 */
NONSHARABLE_CLASS( CCompareView ) : public CContactViewBase
    {
    public:
        
        /**
         * Compare function for Contacts Model view contacts.
         * Forwards request to CContactViewBase of Contacts Model.
         * @param aFirst  First view contact to be compared
         * @param aSecond Second view contact to be compared
         * @return Result of CompareFieldsL from cntviewbase
         */
        static TInt CompareFieldsL( const ::CViewContact &aFirst,
            const ::CViewContact &aSecond );
            
    private:
        /**
         * Dummy constructor. Not to be used.
         */
        CCompareView( const CContactDatabase& aDb );
    };

/**
 * A base class for find views that implements MVPbkContactView
 */    
NONSHARABLE_CLASS( CFindViewBase ): public CBase,
                                    public MParentViewForFiltering,
                                    public MVPbkContactViewFiltering,
                                    public MFilteredViewSupportObserver,
                                    protected MAlwaysIncludedContacts
    {
    public: // Constructor and destructor
        /**
         * Destructor.
         */
        ~CFindViewBase();   
        
    public: //New functions
    
        /**
         * Acticates the match asynchrnously. View ready event
         * will be send when the find view is ready
         */
        void ActivateContactMatchL();
                        
    public: // From MVPbkContactView
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
        
    public: // From MVPbkContactViewFiltering
        MVPbkContactViewBase* CreateFilteredViewLC( 
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        /// Subclasses must implement this
        virtual void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts ) = 0;
    
    public: // From MVPbkContactViewObserver
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
                TBool aErrorNotified );                
        // From MFilteredViewSupportObserver
        void ContactViewReadyForFiltering( 
            MParentViewForFiltering& aView );
        void ContactViewUnavailableForFiltering( 
                MParentViewForFiltering& aView );

        
    protected: // Implementation
    
        /**
         * @param aParentView the view whose contacts are filtered
         * @param aBaseView the all contacts view.
         * @param aOwnsMatchedContacts ETrue if this instance owns
         *          contacts in iMatchedContacts
         */
        CFindViewBase( MParentViewForFiltering& aParentView,
            CViewBase& aBaseView,
            TBool aOwnsMatchedContacts );
        
        /**
         * @param aFindString the find words for filtering
         * @param aFindPolicy a policy for matching.
         * @param aExternalViewObserver an observer that was given
         *        by the Virtual Phonebook client
         */
        void BaseConstructL( const MDesCArray& aFindStrings,
            MVPbkContactFindPolicy& aFindPolicy,
            MVPbkContactViewObserver& aExternalViewObserver );
        
        /**
         * Checks if aViewContact matches to iFindStrings
         *
         * @param aViewContact the contact to be checked
         */
        TBool IsMatchL( const MVPbkViewContact& aViewContact );
        
        /**
         * Returns the find words used in filtering
         *
         * @return the find words used in filtering
         */
        const MDesCArray& FindStrings() const;
        
        /**
         * Deletes the old find strings and allocates new ones
         * based on aFindStrings
         *
         * @param aFindStrings new find strings
         */
        void SetFindStringsL( const MDesCArray& aFindStrings );
        
        /**
         * Calls ViewReady or Unavailable depending on state
         */
        void SendViewStateEventToObservers();
        
        /**
         * a type definition for Contacts Model view contact for
         * separating it clearly from VPbkCntModel::CViewContact
         */
        typedef ::CViewContact CCntModelViewContact;
        
    private: // New functions
        virtual void MatchL( 
            RPointerArray<CCntModelViewContact>& aMatchedContacts ) = 0;
        virtual void DoContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink,
                RPointerArray<CCntModelViewContact>& aMatchedContacts ) = 0;
            
    private: // implementation
        void DoContactViewReadyForFilteringL( MVPbkContactViewBase& aView );
        void HandleContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink );
        void HandleContactRemovedFromViewL( MVPbkContactViewBase& aView, 
                TInt aIndex,  const MVPbkContactLink& aContactLink );
        void DoAddObserver( MVPbkContactViewObserver& aObserver );
        void DoAddObserverError( MVPbkContactViewObserver& aObserver,
                TInt aError );
        void DoAddFilteringObserverL( 
                MFilteredViewSupportObserver& aObserver );
        void DoAddFilteringObserverError( 
                MFilteredViewSupportObserver& aObserver, TInt aError );
        void ContactMatchL();
        void ResetContacts();
        void SendContactViewErrorEvent( TInt aError, TBool aErrorNotified );
    
    protected: // Data
        /// Ref: The view that cretead this filtered view
        MParentViewForFiltering& iParentView;
        /// Ref: The all contacts view
        CViewBase& iBaseView;
        
    private: // Data                  
        /// Own: A matched contacts         
        RPointerArray<CCntModelViewContact> iMatchedContacts;
        /// Ref: other observer added by AddObserverL
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Ref: an array of observers that filters this view.
        RPointerArray<MFilteredViewSupportObserver> iFilteringObservers;
        /// Own: find words for filtering
        CDesCArrayFlat* iFindStrings;        
        /// Own: a field type list for name construction policy
        CVPbkFieldTypeRefsList* iFieldTypeRefsList;
        /// Ref: a policy for match
        MVPbkContactFindPolicy* iContactFindPolicy;
        /// Own: a view contact for ContactAtL
        CViewContact* iViewContact;
        /// Own: an async operation for MVPbkContactViewObserver
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>* iObserverOp;
        /// Own: an async operation for MFilteredViewSupportObserver
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>* iFilterObsOp;
        /// Own: ETrue if this view owns matched contacts
        TBool iOwnsContacts;
        /// Own: ETrue if this view is ready
        TBool iViewReady;
    };
} // namespace VPbkCntModel    

#endif // VPBKCNTMODEL_CFINDVIEWBASE_H

// End of File
