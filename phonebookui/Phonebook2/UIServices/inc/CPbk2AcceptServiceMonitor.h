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
* Description:  Phonebook 2 UI Services accept service monitor.
*
*/


#ifndef CPBK2ACCEPTSERVICEMONITOR_H
#define CPBK2ACCEPTSERVICEMONITOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RPbk2UIService;
class MPbk2ConnectionObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 UI Services accept service monitor.
 * Responsible for monitoring accept service requests
 * during service execution
 */
class CPbk2AcceptServiceMonitor : public CActive
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aPbk2AppService   Application server.
         * @param aObserver         Server app connection observer.
         * @return  A new instance of this class.
         */
        static CPbk2AcceptServiceMonitor* NewL(
                RPbk2UIService& aPbk2AppService,
                MPbk2ConnectionObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2AcceptServiceMonitor();

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // Implementation
        CPbk2AcceptServiceMonitor(
                RPbk2UIService& aPbk2AppService,
                MPbk2ConnectionObserver& aObserver );
        void ConstructL();

    private: // Data
        /// Ref: Application server service
        RPbk2UIService& iPbk2AppService;
        /// Ref: Connection observer
        MPbk2ConnectionObserver& iObserver;
        /// Own: Number of contacts in the buffer
        TInt iNumContacts;
        /// Own: Link buffer
        HBufC8* iLinkBuffer;
    };

#endif // CPBK2ACCEPTSERVICEMONITOR_H

// End of File
