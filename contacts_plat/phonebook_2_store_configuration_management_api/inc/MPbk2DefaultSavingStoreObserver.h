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
* Description:  Phonebook 2 default contact store configuration
*                observer interface.
*
*/


#ifndef MPBK2DEFAULTSAVINGSTOREOBSERVER_H
#define MPBK2DEFAULTSAVINGSTOREOBSERVER_H

/**
 * Phonebook 2 default saving store configuration observer interface.
 */
class MPbk2DefaultSavingStoreObserver
    {
    public: // Interface

        /**
         * Called when the default saving store setting changes.
         */
        virtual void SavingStoreChanged() =0;

    protected: // Disabled functions
        virtual ~MPbk2DefaultSavingStoreObserver()
                {}
    };

#endif // MPBK2DEFAULTSAVINGSTOREOBSERVER_H

// End of File
