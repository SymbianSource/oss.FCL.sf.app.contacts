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


#ifndef VPBKSIMSTORE_CREFINEVIEW_H
#define VPBKSIMSTORE_CREFINEVIEW_H

// INCLUDES
#include "CFindViewBase.h"

namespace VPbkSimStore {


/**
 * A refined filtered view whose parent view is CFindView or CRefineView.
 *
 * This view doesn't own its contacts but only saved pointers to the
 * contacts owned by CFindView.
 */
NONSHARABLE_CLASS( CRefineView ): public CFindViewBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aParentStore  Parent contact store.
         * @param aSortOrder    Contact view sort order.                 
         * @return  A new instance of this class.
         */
        static CRefineView* NewLC(
                const MDesCArray& aFindStrings,
                MParentViewForFiltering& aParentView,
                CContactView& aAllContactsView,
                MVPbkContactViewObserver& aExternalViewObserver,
                MVPbkContactFindPolicy& aContactFindPolicy,
                MAlwaysIncludedContacts& aAlwaysIncluded );
                
        /**
         * Destructor.
         */
        ~CRefineView();
    
    private: // From MVPbkContactViewObserver
        void ContactViewReady( MVPbkContactViewBase& aView );
        void ContactViewReadyForFiltering( MParentViewForFiltering& aView );
        
    private: // From CFindViewBase
        void MatchL( RPointerArray<MVPbkSimContact>& aMatchedContacts );
        void DoContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink,
                RPointerArray<MVPbkSimContact>& aMatchedContacts );
    
    private: // From MVPbkContactViewFiltering
        void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        TBool IsNativeMatchingRequestActive();
            
    private: // From MAlwaysIncludedContacts    
        TBool IsContactAlwaysIncluded( const CViewContact& aContact ) const;
        
    private: // Implementation
        CRefineView( MParentViewForFiltering& aParentView,
            CContactView& aAllContactsView,
            MAlwaysIncludedContacts& aAlwaysIncluded );
        static TInt StartUpdateCallback( TAny* aThis );
                             
    private: // Data
        /// Ref: An interface for checking always included
        MAlwaysIncludedContacts& iAlwaysIncluded;
        /// Own: An idle object for starting async updated.
        CIdle* iIdleUpdate;
    };

} // namespace VPbkSimStore

#endif // VPBKSIMSTORE_CREFINEVIEW_H

// End of File
