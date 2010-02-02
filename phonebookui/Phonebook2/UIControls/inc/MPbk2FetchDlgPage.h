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
* Description:  Phonebook 2 fetch dialog page interface.
*
*/


#ifndef MPBK2FETCHDLGPAGE_H
#define MPBK2FETCHDLGPAGE_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkBaseContact;
class MVPbkContactLink;
class MPbk2ContactUiControl;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog page interface.
 */
class MPbk2FetchDlgPage
    {
    public: // Interface

        /**
         * Destructor
         */
        virtual ~MPbk2FetchDlgPage()
            {}

        /**
         * Returns this page's id.
         *
         * @return  Page id.
         */
        virtual TInt FetchDlgPageId() const = 0;

        /**
         * Activates this dialog page. Page can set internal state according
         * to gives state parameter.
         */
        virtual void ActivateFetchDlgPageL() = 0;

        /**
         * Deactivates this dialog page.
         */
        virtual void DeactivateFetchDlgPage() = 0;

        /**
         * Returns the currently focused contact on this page.
         *
         * @return  Focused contact.
         */
        virtual const MVPbkBaseContact* FocusedContactL() const = 0;

        /**
         * Returns the UI control of the page.
         *
         * @return  UI control.
         */
        virtual MPbk2ContactUiControl& Control() const = 0;

        /**
         * Returns the view of the page.
         *
         * @return  View.
         */
        virtual MVPbkContactViewBase& View() const = 0;

        /**
         * Sets the view of the page.
         *
         * @param aView     The view to set.
         */
        virtual void SetViewL(
                MVPbkContactViewBase& aView ) = 0;

        /**
         * Returns ETrue if the page initialisation is complete
         * and the page can be used.
         *
         * @return  ETrue if the page is ready.
         */
        virtual TBool DlgPageReady() const = 0;

        /**
         * Returns ETrue if this fetch dialog page is empty.
         *
         * @return  ETrue if the page is empty.
         */
        virtual TBool DlgPageEmpty() const = 0;

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

        /**
         * Handles a resource change.
         *
         * @param aType     Resource change type.
         */
        virtual void HandleResourceChange(
                TInt aType ) = 0;
    };

#endif // MPBK2FETCHDLGPAGE_H

// End of File
