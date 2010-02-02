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
* Description:  Phonebook 2 start-up observer interface.
*
*/


#ifndef MPBK2STARTUPOBSERVER_H
#define MPBK2STARTUPOBSERVER_H

// INCLUDES
#include <e32def.h>
#include <mpbk2startupmonitor.h>

// CLASS DECLARATION

/**
 * Phonebook 2 start-up observer interface.
 */
class MPbk2StartupObserver
    {
    public: // Interface

        /**
         * Called after the contact UI is ready.
         *
         * @param aStartupMonitor   The start-up monitor.
         */
        virtual void ContactUiReadyL(
                MPbk2StartupMonitor& aStartupMonitor ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2StartupObserver()
                {}
    };

#endif // MPBK2STARTUPOBSERVER_H

// End of File
