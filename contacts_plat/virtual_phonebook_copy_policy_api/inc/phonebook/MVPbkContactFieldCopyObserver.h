/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An observer interface for importing contact from vcard or
*                compact business card
*
*/


#ifndef MVPBKCONTACTFIELDCOPYOBSERVER_H
#define MVPBKCONTACTFIELDCOPYOBSERVER_H

// INCLUDES
#include <e32cmn.h>
#include <e32def.h>

// FORWARD DECLARATIONS
class MVPbkContactOperationBase;

/**
 * Virtual Phonebook contact field copy observer interface.
 */
class MVPbkContactFieldCopyObserver
    {        
    public: // Interface
        /**
         * Called when a field has been successfully added to a contact.
         * @param aOperation The operation that this observer monitors.
         */
        virtual void FieldAddedToContact(
                MVPbkContactOperationBase& aOperation) =0;
                
        /**
         * Called when adding a field to a contact has failed.
         * @param aOperation The operation that this observer monitors.
         * @param aError Error that occured.
         *        -KErrNotSupported: the target contact doesn't support
         *         the field or doesn't have a specified field type conversion.
         *        -KErrOverflow: the contact already has maximum amount
         *         fields of given type.
         */
        virtual void FieldAddingFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError) =0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactFieldCopyObserverExtension(
            TUid /*aExtensionUid*/) { return NULL; }

    protected:  // Destructor
        virtual ~MVPbkContactFieldCopyObserver() { }

    };

/**
 * Virtual Phonebook contact fields copy observer interface.
 */
class MVPbkContactFieldsCopyObserver
    {        
    public: // Interface
        /**
         * Called when fields have been successfully added to a contact.
         * @param aOperation The operation that this observer monitors.
         */
        virtual void FieldsCopiedToContact(
                MVPbkContactOperationBase& aOperation) =0;
                
        /**
         * Called when adding fields to a contact has failed.
         * @param aOperation    The operation that this observer monitors.
         * @param aError        A system wide error code
         */
        virtual void FieldsCopyFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError) =0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactFieldsCopyObserverExtension(
                TUid /*aExtensionUid*/) { return NULL; }

    protected:  // Destructor
        virtual ~MVPbkContactFieldsCopyObserver() { }

    };
    
#endif // MVPBKCONTACTFIELDCOPYOBSERVER_H

//End of file
