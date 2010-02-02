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
* Description:  Phonebook 2 contact store list observer
*              : registration interface.
*
*/


#ifndef MPBK2STOREOBSERVATIONREGISTER_H
#define MPBK2STOREOBSERVATIONREGISTER_H

// FORWARD DECLARATIONS
class MVPbkContactStoreObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 contact store list observer registration interface.
 * Virtual Phonebook accepts only one contact store list observer,
 * and the concrete class offering this interface is that observer.
 * But via this interface it shares those contact store list events
 * to other interested observers too.
 */
class MPbk2StoreObservationRegister
    {
    public: // Interface

        /**
         * Registers the given observer to receive contact store events.
         * Store events are sent for stores belonging to current
         * names list configuration.
         *
         * @param aObserver     Contact store observer to register.
         */
        virtual void RegisterStoreEventsL(
                const MVPbkContactStoreObserver& aObserver ) = 0;

        /**
         * Deregisterers the given observer from receiving
         * contact store events.
         *
         * @param aObserver     Contact store observer to deregister.
         */
        virtual void DeregisterStoreEvents(
                const MVPbkContactStoreObserver& aObserver ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Store manager extension.
         */
        virtual TAny* StoreObservationRegisterExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2StoreObservationRegister()
                {}
    };

#endif // MPBK2STOREOBSERVATIONREGISTER_H

// End of File
