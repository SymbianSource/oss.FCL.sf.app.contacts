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
* Description:  Phonebook 2 fetch results observer interface.
*
*/


#ifndef MPBK2FETCHRESULTSOBSERVER_H
#define MPBK2FETCHRESULTSOBSERVER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch results observer interface.
 */
class MPbk2FetchResultsObserver
    {
    public: // Interface

        /**
         * Informs the observer that contact was selected.
         *
         * @param aLink         The contact that was selected/deselected.
         * @param aSelected     ETrue if contact was selected,
         *                      EFalse if it was deselected.
         */
        virtual void ContactSelected(
                const MVPbkContactLink& aLink,
                TBool aSelected ) = 0;

        /**
         * Informs the observer that contact selection failed.
         */
        virtual void ContactSelectionFailed() = 0;

    protected: // Disabled functions
        virtual ~MPbk2FetchResultsObserver()
            {};
    };

#endif // MPBK2FETCHRESULTSOBSERVER_H

// End of File
