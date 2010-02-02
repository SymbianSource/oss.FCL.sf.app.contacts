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


#ifndef VPBKCNTMODEL_CFINDVIEW_H
#define VPBKCNTMODEL_CFINDVIEW_H

// INCLUDES
#include "CFindViewBase.h"

// FORWARD DECLARATIONS
class CVPbkContactFindPolicy;

namespace VPbkCntModel {


/**
 * Virtual phonebook view that holds the search results from Contact model
 */
NONSHARABLE_CLASS( CFindView ): public CFindViewBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFindStrings Search words
         * @param aBaseView Virtual phonebook's view where the search
         *                  is pointed
         * @param aExternalViewObserver An observer that was given
         *        by the Virtual Phonebook client
         * @param aAlwaysIncludedContacts A collection of contacts that
         *        must be included even they don't match to find words.
         * @param aRFs File session
         * @return  A new instance of this class.
         */
        static CFindView* NewLC(
                const MDesCArray& aFindStrings,
                CViewBase& aBaseView,
                MVPbkContactViewObserver& aExternalViewObserver,
                const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
                RFs& aRFs );

        /**
         * Destructor.
         */
        ~CFindView();

    private: // From CFindViewBase
        void MatchL( RPointerArray<CCntModelViewContact>& aMatchedContacts );
        void DoContactAddedToViewL( MVPbkContactViewBase& aView,
                TInt aIndex, const MVPbkContactLink& aContactLink,
                RPointerArray<CCntModelViewContact>& aMatchedContacts );

    private: // From MVPbkContactViewFiltering
        /// Subclasses must implement this
        void UpdateFilterL(
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );

    private: // From MAlwaysIncludedContacts
        TBool IsContactAlwaysIncluded( const CViewContact& aContact ) const;

    private: // Implementation
        CFindView( CViewBase& aBaseView );
        void ConstructL( const MDesCArray& aFindStrings,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
            MVPbkContactViewObserver& aExternalViewObserver, RFs& aRFs );
        void SetAlwaysIncludedContactsL(
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );
        void MatchContactL( const CViewContact& aViewContact,
            RPointerArray<CCntModelViewContact>& aMatchedContacts );
        TBool IsContactsModelMatchL( const CViewContact& aContact,
            TInt& aMatchArrayIndex ) const;
        void RemoveFromMatchArrayIfFound( const CViewContact& aContact );
        TInt FindFromMatchArray( const CViewContact& aContact ) const;

    private: // Data
        /// Own: the find policy
        CVPbkContactFindPolicy* iFindPolicy;
        /// Own: always included contacts
        RArray<TContactItemId> iAlwaysIncluded;
        /// Own: Matched contacts from Contacts Model
        RPointerArray<CCntModelViewContact> iContactsModelMatchContacts;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CFINDVIEW_H

// End of File
