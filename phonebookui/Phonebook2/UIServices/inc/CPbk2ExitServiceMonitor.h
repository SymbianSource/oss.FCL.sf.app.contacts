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
* Description:  Phonebook 2 UI Services exit service monitor.
*
*/


#ifndef CPBK2EXITSERVICEMONITOR_H
#define CPBK2EXITSERVICEMONITOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RPbk2UIService;
class MPbk2ConnectionObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 UI Services exit service monitor.
 * Responsible for monitoring ok to exit requests
 * during service execution.
 */
class CPbk2ExitServiceMonitor : public CActive
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aPbk2AppService   Application server interface.
         * @param aObserver         Server app connection observer.
         * @return  A new instance of this class.
         */
        static CPbk2ExitServiceMonitor* NewL(
                RPbk2UIService& aPbk2AppService,
                MPbk2ConnectionObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ExitServiceMonitor();

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // Implementation
        CPbk2ExitServiceMonitor(
                RPbk2UIService& aPbk2AppService,
                MPbk2ConnectionObserver& aObserver );
        void ConstructL();

    private: // Data
        /// Ref: Application server service
        RPbk2UIService& iPbk2AppService;
        /// Ref: Connection observer
        MPbk2ConnectionObserver& iObserver;
        /// Own: Id of the exit command
        TInt iExitCommandId;
    };

#endif // CPBK2EXITSERVICEMONITOR_H

// End of File
