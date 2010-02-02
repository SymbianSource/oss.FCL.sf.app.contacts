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


#ifndef VPBKCNTMODEL_CREFINEVIEW_H
#define VPBKCNTMODEL_CREFINEVIEW_H

// INCLUDES
#include "CFindViewBase.h"

namespace VPbkCntModel {

/**
 * Virtual phonebook refine view. This is used based on CFindView search
 * results. The search is refined when user presses another character to
 * find pane.
 */
NONSHARABLE_CLASS( CRefineView ): public CFindViewBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFindStrings Find strings
         * @param aParentView Previous view
         * @param aBaseView Base view
         * @param aExternalViewObserver
         * @param aContactFindPolicy A policy for matching
         * @param aAlwaysIncluded Is contact one of the always incl. contacts
         * @return A new instance of this class.
         */
        static CRefineView* NewLC(
                const MDesCArray& aFindStrings,
                MParentViewForFiltering& aParentView,
                CViewBase& aBaseView,
                MVPbkContactViewObserver& aExternalViewObserver,
                MVPbkContactFindPolicy& aContactFindPolicy,
                MAlwaysIncludedContacts& aAlwaysIncluded );
                                
        /**
         * Destructor.
         */
        ~CRefineView();
        
    private: // From CFindViewBase
        void MatchL( RPointerArray<CCntModelViewContact>& aMatchedContacts );
        void DoContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink,
                RPointerArray<CCntModelViewContact>& aMatchedContacts );       
    
    private: // From MVPbkContactViewFiltering
        void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
            
    private: // From MAlwaysIncludedContacts    
        TBool IsContactAlwaysIncluded( const CViewContact& aContact ) const;
        
    private: // Implementation
        CRefineView( MParentViewForFiltering& aParentView,
            CViewBase& aBaseView,
            MAlwaysIncludedContacts& aAlwaysIncluded );
        static TInt StartUpdateCallback( TAny* aThis );
        
    private: // Data
        /// Ref: An interface for checking always included
        MAlwaysIncludedContacts& iAlwaysIncluded;
        /// Own: An idle object for starting async updated.
        CIdle* iIdleUpdate;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CREFINEVIEW_H

// End of File
