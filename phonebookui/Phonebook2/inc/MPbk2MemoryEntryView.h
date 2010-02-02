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
* Description:  Base class for Phonebook 2 memory entry views.
*
*/


#ifndef MPBK2MEMORYENTRYVIEW_H
#define MPBK2MEMORYENTRYVIEW_H

// FORWARD DECLARATIONS
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Base class for Phonebook 2 memory entry views.
 */
class MPbk2MemoryEntryView
    {
    public: // Base class interface

        /**
         * Notifies that the contact is about to change soon. The instance
         * of this class should prepare for that by, for example,
         * storing the view state.
         */
        virtual void PrepareForContactChangeL() = 0;

        /**
         * Notifies the derived class that the displayed
         * contact has changed. Ownership is transfered.
         *
         * @param aContact      The new contact to show.
         */
        virtual void ContactChangedL(
                MVPbkStoreContact* aContact) = 0;

        /**
         * Notifies the derived class that the displayed
         * contact change failed.
         */
        virtual void ContactChangeFailed() = 0;

        /**
         * Returns the contact item this view is currently displaying.
         *
         * @return  Store contact currently displayed.
         */
        virtual const MVPbkStoreContact* Contact() const = 0;

    protected: // Disabled functions
        virtual ~MPbk2MemoryEntryView()
                {}
    };

#endif // MPBK2MEMORYENTRYVIEW_H

// End of File
