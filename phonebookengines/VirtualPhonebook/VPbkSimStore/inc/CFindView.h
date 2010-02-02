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


#ifndef VPBKSIMSTORE_CFINDVIEW_H
#define VPBKSIMSTORE_CFINDVIEW_H

// INCLUDES
#include "CFindViewBase.h"

#include <MVPbkSimViewFindObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactFindPolicy;
class MVPbkSimStoreOperation;
class RFs;

namespace VPbkSimStore {

class CContactView;

NONSHARABLE_CLASS( CFindView ): public CFindViewBase,
                                public MVPbkSimViewFindObserver                 
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFindStrings              
         * @param aAllContactsView
         * @param aExternalViewObserver
         * @param aAlwaysIncludedContacts
         * @param aRFs
         * @return  A new instance of this class.
         */
        static CFindView* NewLC(    
                const MDesCArray& aFindStrings,
                CContactView& aAllContactsView,
                MVPbkContactViewObserver& aExternalViewObserver,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
                RFs& aRFs );
        /**
         * Destructor.
         */
        ~CFindView();    
    
    private: // From MVPbkContactViewObserver
        void ContactViewReady( MVPbkContactViewBase& aView );
        void ContactViewReadyForFiltering( 
                MParentViewForFiltering& aView );
                
    private: // From CFindViewBase
        void MatchL( RPointerArray<MVPbkSimContact>& aMatchedContacts );
        void DoContactAddedToViewL(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink,
                RPointerArray<MVPbkSimContact>& aMatchedContacts );
        
    private: // From MVPbkSimViewFindObserver
        void ViewFindCompleted( MVPbkSimCntView& aSimCntView,
            const RVPbkStreamedIntArray& aSimIndexArray );
        void ViewFindError( MVPbkSimCntView& aSimCntView, TInt aError );

    private: // From MVPbkContactViewFiltering
        void UpdateFilterL( 
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        TBool IsNativeMatchingRequestActive();
            
    private: // From MAlwaysIncludedContacts    
        TBool IsContactAlwaysIncluded( const CViewContact& aContact ) const;
                                                        
    private: // Implementation
        CFindView( CContactView& aContactView );
        void ConstructL( const MDesCArray& aFindStrings,
                MVPbkContactViewObserver& aExternalViewObserver,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
                RFs& aRFs );
        void SetAlwaysIncludedContactsL( 
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        void ViewFindCompletedL( MVPbkSimCntView& aSimCntView,
                const RVPbkStreamedIntArray& aSimIndexArray );
        void MatchContactL( const CViewContact& aViewContact,
            RPointerArray<MVPbkSimContact>& aMatchedContacts );
        void RemoveFromMatchArrayIfFound( const CViewContact& aViewContact );
        TBool IsSIMMatchL( const CViewContact& aContact,
            TInt& aMatchArrayIndex ) const;
        TInt FindFromMatchArray( const CViewContact& aContact ) const;
                            
    private: // Data
        /// Own: Contact matching operation
        MVPbkSimStoreOperation* iFindOperation;
        /// Own: the find policy
        CVPbkContactFindPolicy* iFindPolicy;
        /// Own: always included contacts
        RArray<TInt> iAlwaysIncluded;
        /// Own: match results (SIM indexes) from native view
        RArray<TInt> iSIMMatchedContacts;
    };
    
} // namespace VPbkSimStore    

#endif // VPBKSIMSTORE_CFINDVIEW_H

// End of File
