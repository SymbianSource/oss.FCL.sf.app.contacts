/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Server App start-up observer interface.
*
*/


#ifndef MPBK2SERVERAPPSTARTUPOBSERVER_H
#define MPBK2SERVERAPPSTARTUPOBSERVER_H

#include <MPbk2StartupObserver.h>

// CLASS DECLARATION

/**
 * Phonebook 2 Server App start-up observer interface.
 * Used for notifying user or system initiated cancel.
 * Core Phonebook 2 startup observer does not have cancel support,
 * since it uses app shutter to shut itself down. But Server App
 * cannot do that since it would shut the calling process also.
 */
class MPbk2ServerAppStartupObserver : public MPbk2StartupObserver
    {
    public: // Interface

        /**
         * Called if startup is cancelled.
         *
         * @param aError  Error code.
         */
        virtual void StartupCanceled(
                TInt aErrorCode ) = 0;

    protected: // Protected functions
        virtual ~MPbk2ServerAppStartupObserver()
            {}
    };

#endif // MPBK2SERVERAPPSTARTUPOBSERVER_H

// End of File
