/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Find contact view
*
*/


#ifndef CVPBKFINDVIEW_H
#define CVPBKFINDVIEW_H

#include <e32base.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactViewFiltering.h>
#include <badesca.h>

namespace VPbkEngUtils { class CVPbkAsyncOperation; }

class CVPbkFieldTypeRefsList;
class CVPbkContactNameConstructionPolicy;
class CVPbkContactFindPolicy;

/**
 * Virtual Phonebook Find view
 *
 * This class can be used for find from an existing view. A client
 * gives the view for find.
 */
NONSHARABLE_CLASS( CVPbkFindView ): public CBase,
                                    public MVPbkContactViewBase,
                                    public MVPbkContactViewObserver,
                                    public MVPbkContactViewFiltering
	{
    public:
        /**
         * Two-phase constructor.
         *
         * @param aBaseView The view to filter.
         * @param aObserver an observer that is notified when this
         *                  view is ready for use.
         * @param aFindWords an array words that must match to the contact.
         * @param aAlwaysIncludedContacts contacts that must be in the view
         *                                even they don't match to aFindWords.
         *                                This an optional parameter. Give
         *                                NULL if not needed.
         * @param aMasterFieldTypeList Field type list
         * @return a new instance of this class         
         */
        static CVPbkFindView* NewLC(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
                const MVPbkFieldTypeList& aMasterFieldTypeList );
        ~CVPbkFindView();

    protected: // From MVPbkContactViewBase
        TVPbkContactViewType Type() const;
        void ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder);
        const MVPbkFieldTypeList& SortOrder() const;
        void RefreshL();
        TInt ContactCountL() const;
        const MVPbkViewContact& ContactAtL(TInt aIndex) const;
        MVPbkContactLink* CreateLinkLC(TInt aIndex) const;
        TInt IndexOfLinkL(const MVPbkContactLink& aContactLink) const;
        void AddObserverL(MVPbkContactViewObserver& aObserver);
        void RemoveObserver(MVPbkContactViewObserver& aObserver);
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
        MVPbkContactBookmark* CreateBookmarkLC(TInt aIndex) const;
        TInt IndexOfBookmarkL(
            const MVPbkContactBookmark& aContactBookmark) const;
        MVPbkContactViewFiltering* ViewFiltering();
    
    private: // From MVPbkContactViewFiltering    
        MVPbkContactViewBase* CreateFilteredViewLC( 
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
            
        void UpdateFilterL( 
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        
    private: // From MVPbkContactViewObserver
        void ContactViewReady(MVPbkContactViewBase& aView);
        void ContactViewUnavailable(MVPbkContactViewBase& aView);
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

    private: // Implementation
        CVPbkFindView(
                MVPbkContactViewBase& aBaseView,                
                const MVPbkFieldTypeList* aMasterFieldTypeList );
        void ConstructL(
                const MDesCArray& aFindWords,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        void BuildViewMappingL();
        void HandleBuildViewMapping();
        void DoAddObserverL(MVPbkContactViewObserver& aObserver);
        void AddObserverError(MVPbkContactViewObserver& aObserver, TInt aError);
        TBool ContactMatchRefineL(         
                const MVPbkViewContact& aViewContact,
                TPtrC aFindWord );
        void SetFindStringsL( const MDesCArray& aFindWords );
        void SetAlwaysIncludedContactsL( 
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        void SendViewEventToObservers( 
            void( MVPbkContactViewObserver::* aObserverFunc)
            ( MVPbkContactViewBase& ) );
        void SendViewErrorEventToObservers( TInt aError, TBool 
            aErrorNotified );

    private: // Data
        ///Own: indicated whether view is ready
        TBool iIsReady;
        ///Ref: contact base view
        MVPbkContactViewBase& iBaseView;
        ///Ref: Master field type list
        const MVPbkFieldTypeList* iMasterFieldTypeList;
        ///Own: Asynchronous operation
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOperation;
        ///Own: mapping between base view and filtered view
        RArray<TInt> iContactMapping;
        ///Ref: array to view observers
        RPointerArray<MVPbkContactViewObserver> iObservers;
        /// Own: find words for filtering
        CDesCArrayFlat* iFindStrings;
        /// Own: a field type list for name construction policy
        CVPbkFieldTypeRefsList* iFieldTypeRefsList;
        /// Own: a policy for name formatting
        CVPbkContactNameConstructionPolicy* iNameConstructionPolicy;
        /// Own: a policy for match
        CVPbkContactFindPolicy* iContactFindPolicy;
        /// Own: always included contacts
        const MVPbkContactBookmarkCollection* iAlwaysIncludedContacts;
    };

#endif // CVPBKFINDVIEW_H

//End of file


