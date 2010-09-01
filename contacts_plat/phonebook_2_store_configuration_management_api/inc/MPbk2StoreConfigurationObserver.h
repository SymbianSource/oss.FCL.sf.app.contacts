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
* Description:  Phonebook 2 contact store configuration
*                observer interface.
*
*/

#ifndef MPBK2STORECONFIGURATIONOBSERVER_H
#define MPBK2STORECONFIGURATIONOBSERVER_H

/**
 * Phonebook 2 contact store configuration observer interface.
 *
 * @see CPbk2StoreConfiguration
 */
class MPbk2StoreConfigurationObserver
    {
    public: // Interface

        /**
         * Called when the store configuration changes.
         */
        virtual void ConfigurationChanged() =0;

        /**
         * Called when the configuration changed event
         * has been delivered to all observers.
         */
        virtual void ConfigurationChangedComplete() =0;

    protected: // Protected functions
        virtual ~MPbk2StoreConfigurationObserver()
                {}
    };

#endif // MPBK2STORECONFIGURATIONOBSERVER_H

// End of File
