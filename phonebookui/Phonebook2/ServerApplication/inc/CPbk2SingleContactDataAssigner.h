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
* Description:  Phonebook 2 single contact data assigner.
*
*/


#ifndef CPBK2SINGLECONTACTDATAASSIGNER_H
#define CPBK2SINGLECONTACTDATAASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2UiService.h"
#include "mpbk2serverappstartupobserver.h"
#include "MPbk2ServicePhaseObserver.h"
#include <coehelp.h>

// FORWARD DECLARATIONS
class MPbk2UiServiceObserver;
class CVPbkContactStoreUriArray;
class MPbk2SelectFieldProperty;
class CPbk2ServerAppStoreManager;
class CVPbkFieldTypeSelector;
class MPbk2ServicePhase;
class MVPbkStoreContact;
class TVPbkContactStoreUriPtr;

// CLASS DECLARATION

/**
 * Phonebook 2 single contact data assigner.
 * Responsible for driving single contact assign phases.
 */
class CPbk2SingleContactDataAssigner : public CBase,
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
        static CPbk2SingleContactDataAssigner* NewL(
                const RMessage2& aMessage,
                MPbk2UiServiceObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2SingleContactDataAssigner();

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
        CPbk2SingleContactDataAssigner(
                MPbk2UiServiceObserver& aObserver );
        void ConstructL(
                const RMessage2& aMessage );
        void LaunchFetchPhaseL();
        void LaunchSelectSinglePropertyPhaseL(
                MVPbkContactLinkArray* aContactLinks );
        void LaunchSelectCreateNewPropertyPhaseL(
                TVPbkContactStoreUriPtr savingStoreUri );
        void LaunchPrepareSingleAssignPhaseL(
                MVPbkStoreContact* aStoreContact );
        void LaunchAssignPhaseL(
                MVPbkStoreContact* aStoreContact );

    private: // Data
        /// Own: Editor help context
        TCoeHelpContext iEditorHelpContext;
        /// Own: Mime type
        TInt iMimeType;
        /// Own: Loaded contact store URIs
        CVPbkContactStoreUriArray* iStoreUris;
        /// Own: Textual data to assign
        HBufC* iDataBuffer;
        /// Own: Address select filter buffer
        HBufC8* iAddressSelectFilterBuffer;
        /// Own: Preselected contact
        MVPbkContactLinkArray* iPreselectedContact;
        /// Own: Select field property interface
        MPbk2SelectFieldProperty* iSelectFieldProperty;
        /// Own: Assign flags set by the consumer
        TUint iAssignFlags;
        /// Own: Note flags
        TUint iInformationNoteFlags;
        /// Ref: UI service observer
        MPbk2UiServiceObserver& iObserver;
        /// Own: Server app store manager
        CPbk2ServerAppStoreManager* iStoreManager;
        /// Own: Contact view filter
        CVPbkFieldTypeSelector* iContactViewFilter;
        /// Own: Fetch phase
        MPbk2ServicePhase* iFetchPhase;
        /// Own: Select property phase
        MPbk2ServicePhase* iSelectPropertyPhase;
        /// Own: Prepare assign phase
        MPbk2ServicePhase* iPrepareAssignPhase;
        /// Own: Assign data phase
        MPbk2ServicePhase* iAssignDataPhase;
        /// Own: Select field result
        TInt iSelectFieldResult;
        /// Own: Selected field index
        TInt iFieldIndex;
        /// Own: Relocation flags
        TUint iRelocationFlags;
        /// Own: ETrue if using default store configuration
        TBool iUseDeviceConfig;
    };

#endif // CPBK2SINGLECONTACTDATAASSIGNER_H

// End of File
