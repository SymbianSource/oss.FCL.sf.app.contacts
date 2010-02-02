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
* Description:  Phonebook 2 contact store list validity informer interface.
*
*/


#ifndef MPBK2STOREVALIDITYINFORMER_H
#define MPBK2STOREVALIDITYINFORMER_H

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;

// CLASS DECLARATION

/**
 * Phonebook 2 contact store list validity informer interface.
 */
class MPbk2StoreValidityInformer
    {
    public: // Interface

        /**
         * Returns the stores that are currently available and open.
         * In practise this means the stores which make the names list
         * view and in addition the default saving store.
         *
         * @return  Currently valid stores.
         */
        virtual CVPbkContactStoreUriArray* CurrentlyValidStoresL() const = 0;

        /**
         * Returns the stores that are currently available, open and
         * shown in all contacts view.
         *
         * @return  Currently valid shown stores.
         */
        virtual CVPbkContactStoreUriArray* CurrentlyValidShownStoresL() const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Store manager extension.
         */
        virtual TAny* StoreValidityInformerExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2StoreValidityInformer()
                {}
    };

#endif // MPBK2STOREVALIDITYINFORMER_H

// End of File
