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
* Description:  Phonebook 2 memory entry view contact navigation interface.
*
*/

#ifndef MPBK2CONTACTNAVIGATIONLOADER_H
#define MPBK2CONTACTNAVIGATIONLOADER_H

//  INCLUDES
#include <w32std.h>

//  FORWARD DECLARATIONS
class MVPbkContactLink;

//  CLASS DECLARATION

/**
 * Phonebook 2 contact navigation contact loading interface.
 */
class MPbk2ContactNavigationLoader
    {
    public: // Interface

        /**
         * Change contact to a given contact.
         *
         * @param aContactLink  Link to the contact to display.
         */
        virtual void ChangeContactL(
                const MVPbkContactLink& aContactLink ) = 0;

        /**
         * Returns a link to the current contact.
         *
         * @return  A link to the current contact.
         */
        virtual MVPbkContactLink* ContactLinkLC() const = 0;

    protected: // Disabled functions
        virtual ~MPbk2ContactNavigationLoader()
            {}
    };

#endif // MPBK2CONTACTNAVIGATIONLOADER_H

// End of File
