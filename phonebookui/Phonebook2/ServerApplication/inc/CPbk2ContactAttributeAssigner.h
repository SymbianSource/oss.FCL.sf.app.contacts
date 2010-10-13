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
* Description:  Phonebook 2 contact attribute assigner.
*
*/


#ifndef CPBK2CONTACTATTRIBUTEASSIGNER_H
#define CPBK2CONTACTATTRIBUTEASSIGNER_H

// INCLUDES
#include "MPbk2UiService.h"
#include "mpbk2serverappstartupobserver.h"
#include "MPbk2ServicePhaseObserver.h"
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class MPbk2UiServiceObserver;
class CVPbkContactStoreUriArray;
class CPbk2ServerAppStoreManager;
class CVPbkFieldTypeSelector;
class MPbk2ServicePhase;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 contact attribute assigner.
 * Responsible for driving contact attribute assign phases.
 */
class CPbk2ContactAttributeAssigner : public CBase,
                                      public MPbk2UiService,
                                      public MPbk2ServicePhaseObserver,
                                      private MPbk2ServerAppStartupObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aMessage      Client server message.
         * @param aObserver     UI service observer.
         * @return  A new instance of this class.
         */
        static CPbk2ContactAttributeAssigner* NewL(
                const RMessage2& aMessage,
                MPbk2UiServiceObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactAttributeAssigner();

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
        CPbk2ContactAttributeAssigner(
                MPbk2UiServiceObserver& aObserver );
        void ConstructL(
                const RMessage2& aMessage );
        void BuildViewL();
        void LaunchFetchPhaseL();
        void LaunchPrepareAttributeAssignPhaseL(
                MVPbkContactLinkArray* aContactLinks );
        void LaunchAddressSelectPhaseL(
                MVPbkContactLinkArray* aContactLinks );
        void LaunchAssignPhaseL(
                MVPbkContactLinkArray* aContactLinks );

    private: // Data
        /// Own: Server app store manager
        CPbk2ServerAppStoreManager* iStoreManager;
        /// Own: Loaded contact store URIs
        CVPbkContactStoreUriArray* iStoreUris;
        /// Own: Attribute data to assign
        TPbk2AttributeAssignData iAttributeData;
        /// Own: Preselected contacts
        MVPbkContactLinkArray* iPreselectedContacts;
        /// Ref: UI service observer
        MPbk2UiServiceObserver& iObserver;
        /// Own: Remove attribute indicator
        TBool iRemoveAttribute;
        /// Own: Contact view filter
        CVPbkFieldTypeSelector* iContactViewFilter;
        /// Own: Fetch phase
        MPbk2ServicePhase* iFetchPhase;
        /// Own: Prepare phase
        MPbk2ServicePhase* iPrepareAssignPhase;
        /// Own: Address select phase
        MPbk2ServicePhase* iAddressSelectPhase;
        /// Own: Assign attribute phase
        MPbk2ServicePhase* iAssignAttributePhase;
        /// Own: Flag for using current config in fetch
        TBool iUseDeviceConfig;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2CONTACTATTRIBUTEASSIGNER_H

// End of File
