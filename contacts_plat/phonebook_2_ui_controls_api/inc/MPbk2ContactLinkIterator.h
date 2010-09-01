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
* Description:  Phonebook 2 contact link iterator interface.
*
*/


#ifndef MPBK2CONTACTLINKITERATOR_H
#define MPBK2CONTACTLINKITERATOR_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Phonebook 2 contact link iterator interface.
 * Contact link iterators can be used to iterate a set of contact
 * links.
 */
class MPbk2ContactLinkIterator
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactLinkIterator()
            {}

        /**
         * Returns ETrue if this list iterator has more links when
         * traversing the list into forward direction.
         *
         * @return  ETrue if the iterator has more links.
         */
        virtual TBool HasNext() const = 0;

        /**
         * Returns ETrue if this iterator has more links when traversing
         * the list into reverse direction.
         *
         * @return  ETrue if the iterator has more links.
         */
        virtual TBool HasPrevious() const = 0;

        /**
         * Returns the next link in the list.
         * Ownership of the MVPbkContactLink is transferred to the client.
         *
         * @return  Next link.
         */
        virtual MVPbkContactLink* NextL() = 0;

        /**
         * Returns the previous link in the list.
         * Ownership of the MVPbkContactLink is transferred to the client.
         *
         * @return Previous link.
         */
        virtual MVPbkContactLink* PreviousL() = 0;

        /**
         * Sets the iterator to point to the first element in the list.
         */
        virtual void SetToFirst() = 0;

        /**
         * Sets the iterator to point to the last element in the list.
         */
        virtual void SetToLast() = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactLinkIteratorExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTLINKITERATOR_H

// End of File
