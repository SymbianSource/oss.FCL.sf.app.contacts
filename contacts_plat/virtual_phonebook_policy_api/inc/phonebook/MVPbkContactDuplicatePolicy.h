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
* Description:  An interface for duplicate contact find policy
*
*/


#ifndef MVPBKCONTACTDUPLICATEPOLICY_H
#define MVPBKCONTACTDUPLICATEPOLICY_H

// INCLUDES
#include <e32cmn.h>
#include <e32def.h>

class MVPbkContactOperationBase;
class MVPbkBaseContact;
class MVPbkContactStore;
class MVPbkStoreContact;
class MVPbkContactFindObserver;

/**
 * Virtual Phonebook interface for duplicate contact find policy.
 */
class MVPbkContactDuplicatePolicy
    {
    public:
        /**
         * Destructor
         */
        virtual ~MVPbkContactDuplicatePolicy() {};

        /**
         * Starts asynchronous duplicate find.
         *
         * @param aContact      The contact whose duplicates are searched for.
         * @param aTargetStore  The target store for duplicate find.
         * @param aDuplicates   An array that contains the duplicate contacts
         *                      after operation is ready.
         * @param aObserver     The observer that is called after find is done.
         * @param aMaxDuplicatesToFind The limit of the contacts that are in 
         *                             a worst case returned as a result. 
         *                             Usually 1 is enough.
         * @return An operation instance that client owns.
         */
        virtual MVPbkContactOperationBase* FindDuplicatesL( 
            const MVPbkBaseContact& aContact, 
            MVPbkContactStore& aTargetStore,
            RPointerArray<MVPbkStoreContact>& aDuplicates,
            MVPbkContactFindObserver& aObserver,
            TInt aMaxDuplicatesToFind ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactDuplicatePolicyExtension( 
                TUid /*aExtensionUid*/ ) { return NULL; }
    };

#endif // MVPBKCONTACTDUPLICATEPOLICY_H

// End of file
