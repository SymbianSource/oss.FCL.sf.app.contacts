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
* Description:  Phonebook 2 fetch dialog pages interface.
*
*/


#ifndef MPBK2FETCHDLGPAGES_H
#define MPBK2FETCHDLGPAGES_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkBaseContact;
class MPbk2FetchDlgPage;
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog page collection interface.
 */
class MPbk2FetchDlgPages
    {
    public:  // Interface

        /**
         * Destructor
         */
        virtual ~MPbk2FetchDlgPages()
            {}

        /**
         * Returns the number of pages in the collection.
         *
         * @return  Page count.
         */
        virtual TInt DlgPageCount() const = 0;

        /**
         * Returns the page at given index.
         *
         * @param aIndex    The index to inspect.
         * @return  Page at given index.
         */
        virtual MPbk2FetchDlgPage& DlgPageAt(
                TInt aIndex ) const = 0;

        /**
         * Returns a page with given id or NULL if not found.
         *
         * @param aPageId   Id of the page to search for.
         * @return  Page with given id or NULL.
         */
        virtual MPbk2FetchDlgPage* DlgPageWithId(
                TInt aPageId ) const = 0;

        /**
         * Activates the page with given id and deactives the previous one.
         *
         * @param aPageId   Id of the page to activate.
         */
        virtual void HandlePageChangedL(
                TInt aPageId ) = 0;

        /**
         * Returns ETrue if all pages are ready otherwise EFalse.
         *
         * @return  ETrue if all pages are ready.
         */
        virtual TBool AllPagesReady() = 0;

        /**
         * Returns ETrue if the currently active page is empty.
         *
         * @return  ETrue if currently active page is empty.
         */
        virtual TBool IsActivePageEmpty() = 0;

        /**
         * Actives the first page.
         */
        virtual void ActiveFirstPageL() = 0;

        /**
         * Returns the currently focused contact on an active page.
         *
         * @return  Focused contact.
         */
        virtual const MVPbkBaseContact* FocusedContactL() const = 0;

        /**
         * Returns the current page.
         *
         * @return  Current page.
         */
        virtual MPbk2FetchDlgPage& CurrentPage() const = 0;

        /**
         * Selects contact in the UI control
         *
         * @param aContactLink      Link to the contact to select.
         * @param aSelect           ETrue if contact is to be selected,
         *                          EFalse if deselected.
         */
        virtual void SelectContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelect ) = 0;
    };

#endif // MPBK2FETCHDLGPAGES_H

// End of File
