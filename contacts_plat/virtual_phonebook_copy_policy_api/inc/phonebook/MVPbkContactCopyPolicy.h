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
* Description:  An interface for copy policies that are implemented as
*                ECOM plugins. See CVPbkContactCopyPolicy.
*
*/


#ifndef MVPBKCONTACTCOPYPOLICY_H
#define MVPBKCONTACTCOPYPOLICY_H

// INCLUDES
#include <e32cmn.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactOperationBase;
class MVPbkContactCopyObserver;
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkFieldType;
class MVPbkContactAttribute;
class MVPbkContactFieldCopyObserver;
class MVPbkContactFieldsCopyObserver;

/**
 * Virtual Phonebook Contact copy policy interface.
 */
class MVPbkContactCopyPolicy
    {
    public:
        virtual ~MVPbkContactCopyPolicy() {}

        /**
         * Adds a new data time field to contact asynchronously.
         *
         * @param aContact The contact to add the field to.
         * @param aFieldType Type of the field to add.
         * @param aFieldData Data for the added field.
         * @param aFieldLabel a reference to the field label or
         *        NULL
         * @param aContactAttribute a reference to the contact attribute
         *        or NULL
         * @param aCopyObserver Observer for the operation
         * @return A copy field operation owned by the client.
         */
        virtual MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDateTime& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver ) = 0;

        /**
         * Adds a new text field to contact asynchronously.
         *
         * @param aContact The contact to add the field to.
         * @param aFieldType Type of the field to add.
         * @param aFieldData Data for the added field.
         * @param aFieldLabel a reference to the field label or
         *        NULL
         * @param aContactAttribute a reference to the contact attribute
         *        or NULL
         * @param aCopyObserver Observer for the operation
         * @return A copy field operation owned by the client.
         */
        virtual MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver ) = 0;

        /**
         * Adds a new binary field to contact asynchronously.
         *
         * @param aContact The contact to add the field to.
         * @param aFieldType Type of the field to add.
         * @param aFieldData Data for the added field.
         * @param aFieldLabel a reference to the field label or
         *        NULL
         * @param aContactAttribute a reference to the contact attribute
         *        or NULL
         * @param aCopyObserver Observer for the operation
         * @return A copy field operation owned by the client.
         */
        virtual MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC8& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver ) = 0;

        /**
         * Copies title fields from source contact to the target contact.
         * Policy defines the title field types. The field is not copied
         * if the contact already contains the same type of title field.
         *
         * @param aSourceContact    The source of the fields.
         * @param aTargetContact    The target of the copying.
         * @param aCopyObserver     An observer for asynchronous operation.
         * @return A copy fields operation owned by the client
         */
        virtual MVPbkContactOperationBase* CopyTitleFieldsL(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MVPbkContactFieldsCopyObserver& aCopyObserver ) = 0; 
         
        /**
        * Handles the committing of the contact. This is called
        * after adding fields to contact because it's possible
        * that the policy needs still e.g split the contact.
        *
        * @param aContact the contact to commit, NOTE: ownership is taken.
        *        Client must keep the ownership until this function returns.
        * @param aCopyObserver the observer that is called when
        *        the contact has been copied
        * @return A copy contact operation owned by the client.
        */
        virtual MVPbkContactOperationBase* CommitContactL(
                MVPbkStoreContact* aContact,
                MVPbkContactCopyObserver& aCopyObserver ) = 0;

        /**
        * Copies the contact to the target store. All possible
        * data is copied. The difference to CommitContactL is
        * that if you already have a contact that contains data
        * then you don't need call AddFieldToContactL operations
        * but this can be called instead.
        *
        * @param aContact the source contact.
        * @param aTargetStore the store in which the contact is copied to
        * @param aCopyObserver the observer that called after copying
        * @return A copy contact operation owned by the client.
        */
        virtual MVPbkContactOperationBase* CopyContactL(
                MVPbkStoreContact& aContact,
                MVPbkContactStore& aTargetStore,
                MVPbkContactCopyObserver& aCopyObserver ) = 0;

        /**
        * Returns ETrue if contact data merge is supported. This will
        * disable duplicate contact saving in clients because duplicates
        * need to be merged with new contacts.
        *
        * @return ETrue if contact data merge is supported.
        */
        virtual TBool SupportsContactMerge() const = 0;
        
        /**
        * Merges source contacts to target contact if possible. Handles
        * saving the existing contact and possible new contacts. The logic
        * is defined by the policy. Client gets the links of the contacts
        * that were actually saved.
        *
        * @param aSourceContacts    The policy removes contacts from the array
        *                           and takes the ownerships of the contacts.
        * @param aTarget            The existing contact. Not needed to lock
        *                           by the client. It's policy's job.
        * @param aCopyObserver      An observer for asynchronous operation
        * @return A copy contact operation owned by the client.
        * @see SupportsContactMerge()
        */
        virtual MVPbkContactOperationBase* MergeAndSaveContactsL(
            RPointerArray<MVPbkStoreContact>& aSourceContacts, 
            MVPbkStoreContact& aTarget,
            MVPbkContactCopyObserver& aCopyObserver ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactCopyPolicyExtension(TUid /*aExtensionUid*/)
                                                { return NULL; }
    };

#endif // MVPBKCONTACTCOPYPOLICY_H

//End of file
