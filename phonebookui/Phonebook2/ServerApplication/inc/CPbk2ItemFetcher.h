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
* Description:  Phonebook 2 application server item fetcher.
*
*/


#ifndef CPBK2ITEMFETCHER_H
#define CPBK2ITEMFETCHER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2UiService.h"
#include "mpbk2serverappstartupobserver.h"
#include "MPbk2ServicePhaseObserver.h"
#include <RVPbkContactFieldDefaultPriorities.h>
#include <AiwContactSelectionDataTypes.h>
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class MVPbkContactLinkArray;
class MPbk2UiServiceObserver;
class CVPbkFieldTypeSelector;
class CPbk2ServerAppStoreManager;

// CLASS DECLARATION

/**
 * Phonebook 2 application server item fetcher.
 * Responsible for driving the item fetch service phases.
 */
class CPbk2ItemFetcher : public CBase,
                         public MPbk2UiService,
                         public MPbk2ServicePhaseObserver,
                         private MPbk2ServerAppStartupObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aMessage      Client-Server message.
         * @param aObserver     UI service observer.
         * @param aFetchType    Fetch type.
         * @return  A new instance of this class.
         */
        static CPbk2ItemFetcher* NewL(
                const RMessage2& aMessage,
                MPbk2UiServiceObserver& aObserver,
                TPbk2FetchType aFetchType );

        /**
         * Destructor.
         */
        ~CPbk2ItemFetcher();

    public: // From MPbk2UiService
        void LaunchServiceL();
        void CancelService();
        void AcceptDelayedContactsL(
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
        CPbk2ItemFetcher(
                MPbk2UiServiceObserver& aObserver,
                TPbk2FetchType aFetchType );
        void ConstructL(
                const RMessage2& aMessage );
        void LaunchFetchPhaseL();
        void LaunchAddressSelectPhaseL(
                MVPbkContactLinkArray& aContactLinks );
        void AddXSPStoresL();
        void GetDefaultPriorities( TUint aFlags );

    private: // Data
        /// Ref: UI service observer
        MPbk2UiServiceObserver& iObserver;
        /// Own: Fetch type
        TPbk2FetchType iFetchType;
        /// Own: Loaded store URIs
        CVPbkContactStoreUriArray* iStoreUris;
        /// Own: store URIs for fetch, not include xSP stores
        CVPbkContactStoreUriArray* iContactFetchStoreUris;
        /// Own: Initially marked contact links
        MVPbkContactLinkArray* iPreselectedContacts;
        /// Own: Fetch resource id
        TInt iFetchResId;
        /// Own: Address select resource id
        TInt iAddressSelectResourceId;
        /// Own: Defaults in priority order
        RVPbkContactFieldDefaultPriorities iDefaultPriorities;
        /// Own: Contact view filter
        CVPbkFieldTypeSelector* iContactViewFilter;
        /// Own: Server app store manager
        CPbk2ServerAppStoreManager* iStoreManager;
        /// Own: Fetch phase
        MPbk2ServicePhase* iFetchPhase;
        /// Own: Address select phase
        MPbk2ServicePhase* iAddressSelectPhase;
        /// Own: Address select type
        TAiwAddressSelectType iAddressSelectType;
        /// Own: communication address select type
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
            iCommAddressSelectType;
        /// Own: ETrue if using default store configuration
        TBool iUseDeviceConfig;
        /// Own: Communication address select phase
        MPbk2ServicePhase* iCommAddressSelectPhase;
    };

#endif // CPBK2ITEMFETCHER_H

// End of File
