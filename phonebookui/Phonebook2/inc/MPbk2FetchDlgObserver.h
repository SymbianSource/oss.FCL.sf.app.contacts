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
* Description:  Phonebook 2 fetch dialog observer interface.
*
*/


#ifndef MPBK2FETCHDLGOBSERVER_H
#define MPBK2FETCHDLGOBSERVER_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog observer interface.
 */
class MPbk2FetchDlgObserver
    {
    public: // Interface

        /**
         * Fetch accept selection values.
         */
        enum TPbk2FetchAcceptSelection
            {
            /// Fetch selection is accepted
            EFetchYes = 0,
            /// Fetch selection is rejected
            EFetchNo,
            /// Fetch selection is delayed
            EFetchDelayed
            };

        /**
         * Called by a fetch dialog when user makes a selection.
         *
         * @param aNumMarkedEntries Number of selected entries.
         * @param aLastSelection    Lastly selected contact.
         * @return  Fetch accept selection value.
         */
        virtual TPbk2FetchAcceptSelection AcceptFetchSelectionL(
                TInt aNumMarkedEntries,
                MVPbkContactLink& aLastSelection ) = 0;

        /**
         * Called when the fetch dialog is closed.
         *
         * @param aMarkedEntries    Link array of marked contacts.
         *                          Do not take ownership of the links!
         */
        virtual void FetchCompletedL(
                MVPbkContactLinkArray* aMarkedEntries ) = 0;

        /**
         * Called when fetch is canceled.
         */
        virtual void FetchCanceled() = 0;

        /**
         * Called when fetch is aborted.
         */
        virtual void FetchAborted() = 0;

        /**
         * Queries is it ok to exit.
         *
         * @return  ETrue if it is ok to exit
         *          EFalse otherwise.
         */
        virtual TBool FetchOkToExit() = 0;

    protected: // Protected functions

        /**
         * Protected destructor. Observers are not meant to be
         * destructed via this interface.
         */
        ~MPbk2FetchDlgObserver()
                {}
    };

#endif // MPBK2FETCHDLGOBSERVER_H

// End of File
