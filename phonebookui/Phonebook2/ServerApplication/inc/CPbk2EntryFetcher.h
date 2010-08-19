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
* Description:  Phonebook 2 application server entry fetcher.
*
*/


#ifndef CPBK2ENTRYFETCHER_H
#define CPBK2ENTRYFETCHER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2UiService.h"
#include "mpbk2serverappstartupobserver.h"
#include "MPbk2ServicePhaseObserver.h"

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class MPbk2UiServiceObserver;
class CVPbkFieldTypeSelector;
class CPbk2ServerAppStoreManager;
class MPbk2ServicePhase;

// CLASS DECLARATION

/**
 * Phonebook 2 application server entry fetcher.
 * Responsible for driving the entry fetch service phases.
 */
class CPbk2EntryFetcher : public CBase,
                          public MPbk2UiService,
                          public MPbk2ServicePhaseObserver,
                          private MPbk2ServerAppStartupObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aMessage      Client-server message.
         * @param aObserver     UI service observer.
         * @param aFetchType    Fetch type.
         * @return  A new instance of this class.
         */
        static CPbk2EntryFetcher* NewL(
                const RMessage2& aMessage,
                MPbk2UiServiceObserver& aObserver,
                TPbk2FetchType aFetchType );

        /**
         * Destructor.
         */
        ~CPbk2EntryFetcher();

    public: // From MPbk2UiService
        void LaunchServiceL();
        void CancelService();
        void AcceptDelayedContactsL(
                const TDesC8& aContactLinkBuffer );
        void DenyDelayedContactsL(
                const TDesC8& aContactLinkBuffer );
        void ExitServiceL(
                TInt aExitCommandId );
        void ServiceResults(TServiceResults* aResults) const;

    public: // From MPbk2ServicePhaseObserver
        void NextPhase(
                MPbk2ServicePhase& aPhase );
        void PreviousPhase(
                MPbk2ServicePhase& aPhase );
        void PhaseCanceled(
                MPbk2ServicePhase& aPhase );
        void PhaseAborted(
                MPbk2ServicePhase& aPhase );
        void PhaseError(
                MPbk2ServicePhase& aPhase,
                TInt aErrorCode );
        TBool PhaseOkToExit(
                MPbk2ServicePhase& aPhase,
                TInt aCommandId );
        void PhaseAccept(
                MPbk2ServicePhase& aPhase,
                const TDesC8& aMarkedEntries,
                const TDesC8& aLinkData );

    private: // From MPbk2ServerAppStartupObserver
        void ContactUiReadyL(
                MPbk2StartupMonitor& aStartupMonitor );
        void  StartupCanceled(
                TInt aErrorCode );

    private: // Implementation
        CPbk2EntryFetcher(
                MPbk2UiServiceObserver& aObserver,
                TPbk2FetchType aFetchType );
        void ConstructL(
                const RMessage2& aMessage );
        void LaunchFetchPhaseL();

    private: // Data
        /// Ref: UI service observer
        MPbk2UiServiceObserver& iObserver;
        /// Own: Fetch type
        TPbk2FetchType iFetchType;
        /// Own: Loaded store URIs
        CVPbkContactStoreUriArray* iStoreUris;
        /// Own: Initially marked contact links
        MVPbkContactLinkArray* iPreselectedContacts;
        /// Own: Fetch resource id
        TInt iFetchResId;
        /// Own: Contact view filter
        CVPbkFieldTypeSelector* iContactViewFilter;
        /// Own: Server app store manager
        CPbk2ServerAppStoreManager* iStoreManager;
        /// Own: Fetch phase
        MPbk2ServicePhase* iFetchPhase;
        /// Own: ETrue if using default store configuration
        TBool iUseDeviceConfig;
    };

#endif // CPBK2ENTRYFETCHER_H

// End of File
