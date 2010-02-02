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
* Description:  Phonebook 2 server application start-up monitor.
*
*/


#ifndef CPBK2SERVERAPPMONITOR_H
#define CPBK2SERVERAPPMONITOR_H

//  INCLUDES
#include <badesca.h>
#include <MPbk2StartupMonitor.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MPbk2AppUiExtension;
class MVPbkContactStoreList;
class MPbk2ServerAppStartupObserver;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server application start-up monitor.
 */
class CPbk2ServerAppMonitor : public CBase,
                              public MPbk2StartupMonitor,
                              private MVPbkContactStoreObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreList    List of stores to monitor.
         * @param aObserver     Observer.
         * @return  A new instance of this class.
         */
        static CPbk2ServerAppMonitor* NewL(
                const MVPbkContactStoreList& aStoreList,
                MPbk2ServerAppStartupObserver& aObserver );

        /**
         * Destructor.
         */
        virtual ~CPbk2ServerAppMonitor();

    public: // Interface

        /**
         * Call at start-up begin.
         */
        void StartupBeginsL();

    public: // From MPbk2StartupMonitor
        void HandleStartupComplete();
        void HandleStartupFailed(
                TInt aError );
        void RegisterEventsL(
                MPbk2StartupObserver& aObserver );
        void DeregisterEvents(
                MPbk2StartupObserver& aObserver );
        void NotifyViewActivationL(
                TUid aViewId );
        void NotifyViewActivationL(
                TUid aViewId, MVPbkContactViewBase& aContactView );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // Construction
        CPbk2ServerAppMonitor(
                const MVPbkContactStoreList& aStoreList,
                MPbk2ServerAppStartupObserver& aObserver );
        void ConstructL();

    private: // Implementation
        void AddUnavailableStoreNameL(
                const TDesC& aName );
        void ShowUnavailableStoresL();
        void HandleStoreNotification(
                MVPbkContactStore& aContactStore );
        void SendMessageToObservers();
        void HandleError(
                TInt aResult );

    private: // Data
        /// Own: Contact store list
        const MVPbkContactStoreList* iContactStoreList;
        /// Own: Handled stores counter
        TInt iHandledStores;
        /// Own: An array for observers
        RPointerArray<MPbk2StartupObserver> iObservers;
        /// Own: An array of unavailable store names
        CDesCArray* iUnavailableStoreNames;
        /// Ref: Server app startup observer
        MPbk2ServerAppStartupObserver& iServerAppObserver;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2SERVERAPPMONITOR_H

// End of File
