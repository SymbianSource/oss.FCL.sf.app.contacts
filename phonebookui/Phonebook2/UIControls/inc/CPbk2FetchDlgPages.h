/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 fetch dialog pages.
*
*/


#ifndef CPBK2FETCHDLGPAGES_H
#define CPBK2FETCHDLGPAGES_H

// INCLUDES
#include <e32base.h>
#include "MPbk2FetchDlgPages.h"

// FORWARD DECLARATIONS
class MPbk2FetchDlgPage;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog pages.
 */
NONSHARABLE_CLASS(CPbk2FetchDlgPages) : public CBase,
                                        public MPbk2FetchDlgPages
    {
    public: // Construction and destruction

        /**
         * Constructor.
         */
        CPbk2FetchDlgPages();

        /**
         * Destructor.
         */
        ~CPbk2FetchDlgPages();

    public: // Interface

        /**
         * Adds a new page to this collection.
         * Takes ownership of the page.
         *
         * @param aPage     The page to add.
         */
        void AddL(
                MPbk2FetchDlgPage* aPage );

    public: // From MPbk2FetchDlgPages
        TInt DlgPageCount() const;
        MPbk2FetchDlgPage& DlgPageAt(
                TInt aIndex ) const;
        MPbk2FetchDlgPage* DlgPageWithId(
                TInt aPageId ) const;
        void HandlePageChangedL(
                TInt aPageId );
        TBool AllPagesReady();
        TBool IsActivePageEmpty();
        void ActiveFirstPageL();
        const MVPbkBaseContact* FocusedContactL() const;
        MPbk2FetchDlgPage& CurrentPage() const;
        void SelectContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelect );

    private: // Data
        /// Own: Array of pages
        RPointerArray<MPbk2FetchDlgPage> iPages;
        /// Ref: Currently active page
        MPbk2FetchDlgPage* iCurrentPage;
    };

#endif // CPBK2FETCHDLGPAGES_H

// End of File
