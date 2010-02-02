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
* Description:  Phonebook 2 contact editing observer.
*
*/


#ifndef MPBK2EDITEDCONTACTOBSERVER_H
#define MPBK2EDITEDCONTACTOBSERVER_H

//  FORWARD DECLARATIONS
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editing observer.
 */
class MPbk2EditedContactObserver
    {
    public: // Interface

        /**
         * Informs editor observer that the contact editing is completed.
         *
         * @param aEditedContact    Edited store contact. Ownership is given.
         */
        virtual void ContactEditingComplete(
                MVPbkStoreContact* aEditedContact ) = 0;

        /**
         * Informs editor observer that the contact editing is finished,
         * and that the contact was deleted in the editor.
         *
         * @param aEditedContact    Edited store contact. Ownership is given.
         */
        virtual void ContactEditingDeletedContact(
                MVPbkStoreContact* aEditedContact ) = 0;

        /**
         * Informs editor observer that contact editing is aborted
         * without saving the changes.
         */
        virtual void ContactEditingAborted() = 0;

    protected: // Disabled functions
        ~MPbk2EditedContactObserver()
                {}
    };

#endif // MPBK2EDITEDCONTACTOBSERVER_H

// End of File

