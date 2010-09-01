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
* Description:  Phonebook 2 contacts relocator observer interface.
*
*/


#ifndef MPBK2CONTACTRELOCATOROBSERVER_H
#define MPBK2CONTACTRELOCATOROBSERVER_H

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class CVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 contacts relocator observer interface..
 */
class MPbk2ContactRelocatorObserver
    {
    public: // Interface

        /**
         * Contact has been relocated.
         *
         * @param aRelocatedContact The relocated contact. Observer takes
         *                          ownership of the contact.
         */
        virtual void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact ) = 0;

        /**
         * Contact relocation has failed.
         *
         * @param aReason   Error code.
         * @param aContact  The contact that was submitted for relocation.
         *                  Observer takes ownership of the contact.
         */
        virtual void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact ) = 0;

        /**
         * Contacts relocation has failed.
         *
         * @param aReason   Error code.
         * @param aContact  The contacts that were submitted for relocation.
         *                  Observer takes ownership of the contacts.
         */
        virtual void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts ) = 0;

        /**
         * All contacts have been processed.
         * The relocated contacts have been sent to the observer with
         * ContactRelocatedL() and ContactRelocationFailed() functions.
         */
        virtual void RelocationProcessComplete() = 0;

    protected: // Disabled functions
        ~MPbk2ContactRelocatorObserver()
                {}
    };

#endif // MPBK2CONTACTRELOCATOROBSERVER_H

// End of File
