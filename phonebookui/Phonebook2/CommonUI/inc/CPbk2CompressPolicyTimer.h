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
* Description:  Phonebook 2 database compression timeout timer.
*
*/


#ifndef CPBK2COMPRESSPOLICYTIMER_H
#define CPBK2COMPRESSPOLICYTIMER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>
#include "MPbk2CompressPolicy.h"

// FORWARD DECLARATIONS
class MPbk2CompressPolicyManager;
class MVPbkContactStore;
class MPbk2StoreObservationRegister;

// CLASS DECLARATION

/**
 * Phonebook 2 database compression timeout timer.
 * Responsible for measuring time since last contact store event.
 * If the specified time interval is achieved with no interrupting store
 * events, tells the compress manager to start the compress.
 */
NONSHARABLE_CLASS(CPbk2CompressPolicyTimer) :
        public CTimer,
        public MPbk2CompressPolicy,
        public MVPbkContactStoreObserver

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreObservationRegister     Store observation register.
         * @param aObserver                     Observer.
         * @param aTimeout                      Compression time interval.
         * @return  A new instance of this class.
         */
        static CPbk2CompressPolicyTimer* NewL(
                MPbk2StoreObservationRegister& aStoreObservationRegister,
                MPbk2CompressPolicyManager& aObserver,
                TTimeIntervalSeconds aTimeout );

        /**
         * Destructor.
         */
        ~CPbk2CompressPolicyTimer();

    public: // From MPbk2CompressPolicy
        void Start();
        void Stop();

    public: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // From CTimer
        void RunL();
        TInt RunError(
                TInt aError );

    private: // Implementation
        CPbk2CompressPolicyTimer(
                MPbk2StoreObservationRegister& aStoreObservationRegister,
                MPbk2CompressPolicyManager& aObserver,
                TTimeIntervalSeconds aTimeout );
        void ConstructL();
        void StartTimer();
        void RegisterToListenEventsSafely();

    private: // Data
        /// Ref: Store observation register
        MPbk2StoreObservationRegister& iStoreObservationRegister;
        /// Ref: Compress policy manager
        MPbk2CompressPolicyManager& iManager;
        /// Own: Time out
        TTimeIntervalMicroSeconds32 iTimeout;
        /// Own: Restart indicator
        TBool iRestarted;
    };

#endif // CPBK2COMPRESSPOLICYTIMER_H

// End of File
