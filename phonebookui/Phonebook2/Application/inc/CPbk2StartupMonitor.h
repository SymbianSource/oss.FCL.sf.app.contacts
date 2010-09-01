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
* Description:  Phonebook 2 start-up monitor.
*
*/


#ifndef CPBK2STARTUPMONITOR_H
#define CPBK2STARTUPMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <MPbk2StartupMonitor.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MPbk2ProcessDecorator.h>
#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class MPbk2AppUiExtension;
class CVPbkContactStoreUriArray;
class CPbk2StorePropertyArray;
class CPbk2StoreConfiguration;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 start-up monitor.
 * Responsible for opening wanted stores and waiting that all stores
 * are open before continuing.
 */
class CPbk2StartupMonitor : public CBase,
                            public MPbk2StartupMonitor,
                            public MPbk2StartupMonitorExtension,
                            private MVPbkContactStoreObserver,
                            private MVPbkContactViewObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aAppUiExtension       Application UI extension.
         * @param aStoreProperties      Contact store properties.
         * @param aStoreConfiguration   Contact store configuration.
         * @param aContactManager       Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2StartupMonitor* NewL(
                MPbk2AppUiExtension& aAppUiExtension,
                CPbk2StorePropertyArray& aStoreProperties,
                CPbk2StoreConfiguration& aStoreConfiguration,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        virtual ~CPbk2StartupMonitor();

    public: // Interface

        /**
         * Calls start-up begins for extensions.
         */
        void StartupBeginsL();

        /**
         * Restart monitoring.
         */
        void RestartStartupL();

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
                TUid aViewId,
                MVPbkContactViewBase& aContactView );
        TAny* StartupMonitorExtension( TUid aExtensionUid );
        
    public: // From MPbk2StartupMonitorExtension
        
        /**
         * Disables next wait note launch.
         */
        void DisableMonitoring();

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );

    private: // Implementation
        CPbk2StartupMonitor(
                MPbk2AppUiExtension& aAppUiExtension,
                CPbk2StorePropertyArray& aStoreProperties,
                CPbk2StoreConfiguration& aStoreConfiguration,
                CVPbkContactManager& aContactManager );
        void ConstructL();
        void AddUnavailableStoreNameL(
                const TDesC& aName );
        void ShowUnavailableStoresL();
        void HandleStoreUnavailableL(
                MVPbkContactStore& aContactStore );
        void HandleStoreNotificationL(
                MVPbkContactStore& aContactStore );
        void HandleContactViewReadyEventL(
                MVPbkContactViewBase& aContactView );
        static TInt SendMessageToObservers(
                TAny* aSelf );
        void HandleError(
                TInt aResult );
        void StopWaiter();
        void RegisterStoreEventsForViewL(
                MVPbkContactViewBase& aContactView );
        void DeregisterStoreEventsL(
                TVPbkContactStoreUriPtr aUri );
        void StartAsyncCompletionNotification();
        TBool IsNativePhoneBookView( TUid aActiveViewId );

    private: // Data
        /// Ref: Application UI extension
        MPbk2AppUiExtension& iAppUiExtension;
        /// Ref: Contact store properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Ref: Contact store configuration
        CPbk2StoreConfiguration& iStoreConfiguration;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: The view id of the view that is activated first
        TUid iFirstViewId;
        /// Own: Stores that are listened by the monitor
        CVPbkContactStoreUriArray* iStoreUris;
        /// Ref: An array of observers
        RPointerArray<MPbk2StartupObserver> iObservers;
        /// Own: Start-up process waiter
        class CPbk2StartupWaiter;
        CPbk2StartupWaiter* iWaiter;
        /// Own: An array of unavailable store names
        CDesCArray* iUnavailableStoreNames;
        /// Own: For notifying observers when all contacts view is ready
        CIdle* iIdleNotifier;
    };

#endif // CPBK2STARTUPMONITOR_H

// End of File
