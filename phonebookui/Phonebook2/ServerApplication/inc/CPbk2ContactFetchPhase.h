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
* Description:  Phonebook 2 server app contact fetch phase.
*
*/


#ifndef CPBK2CONTACTFETCHPHASE_H
#define CPBK2CONTACTFETCHPHASE_H

// INCLUDES
#include <e32base.h>
#include <CPbk2FetchDlg.h>
#include <MPbk2FetchDlgObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ExitCallback.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class CPbk2FetchDlg;
class MPbk2DialogEliminator;
class MPbk2ServicePhaseObserver;
class CVPbkFieldTypeSelector;
class CVPbkContactStoreUriArray;
class CVPbkFieldTypeSelector;
class MVPbkContactViewBase;
class CPbk2ServerAppStoreManager;
class CPbk2StoreManager;

// CLASS DECLARATION

/**
 * Phonebook 2 server app contact fetch phase.
 * Responsible for fetching contacts.
 */
class CPbk2ContactFetchPhase : public CBase,
                               public MPbk2ServicePhase,
                               private MPbk2FetchDlgObserver,
                               private MPbk2ExitCallback,
                               private MVPbkSingleContactOperationObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aStoreManager         Store manager.
         * @param aFetchParams          Fetch parameters.
         * @param aFetchOkToExit        Indicates whether it is ok
         *                              to exit fetch right away.
         * @param aFetchAcceptPolicy    Default fetch accept policy.
         * @param aViewFilter           Reference to field type selector.
         * @param aUsingDefaultConfig   ETrue if using device store URIs.
         * @param aStoreUris            Reference to store URIs.
         * @return  A new instance of this class.
         */
        static CPbk2ContactFetchPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                CPbk2ServerAppStoreManager& aStoreManager,
                CPbk2FetchDlg::TParams aFetchParams,
                TBool aFetchOkToExit,
                MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection aFetchAcceptPolicy,
                CVPbkFieldTypeSelector* aViewFilter,
                TBool aUsingDefaultConfig,
                const CVPbkContactStoreUriArray* aStoreUris );

        /**
         * Destructor.
         */
        ~CPbk2ContactFetchPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        void DenyDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        TInt ExtraResultData() const;
        MVPbkStoreContact* TakeStoreContact();
        HBufC* FieldContent() const;

    private: // From MPbk2FetchDlgObserver
        TPbk2FetchAcceptSelection AcceptFetchSelectionL(
                TInt aNumMarkedEntries,
                MVPbkContactLink& aLastSelection );
        void FetchCompletedL(
                MVPbkContactLinkArray* aMarkedEntries );
        void FetchCanceled();
        void FetchAborted();
        TBool FetchOkToExit();

    private: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2ContactFetchPhase(
                MPbk2ServicePhaseObserver& aObserver,
                CPbk2ServerAppStoreManager& aStoreManager,
                CPbk2FetchDlg::TParams aFetchParams,
                TBool aFetchOkToExit,
                MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection aFetchAcceptPolicy,
                CVPbkFieldTypeSelector* aViewFilter,
                TBool aUsingDefaultConfig,
                const CVPbkContactStoreUriArray* aStoreUris );
        void HandleContactOperationCompleteL(
                MVPbkStoreContact* aContact );
        void RetrieveContactL();
        void VerifyContactL(
                MVPbkStoreContact* aContact );

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Ref: Server app store manager
        CPbk2ServerAppStoreManager& iServerAppStoreManager;
        /// Own: Fetch results
        CVPbkContactLinkArray* iFetchResults;
        /// Ref: Fetch dialog
        CPbk2FetchDlg* iFetchDlg;
        /// Ref: For fetch dialog's exit handling
        MPbk2DialogEliminator* iFetchDialogEliminator;
        /// Own: Fetch parameters
        CPbk2FetchDlg::TParams iFetchParams;
        /// Own: Indicates whether it is ok to exit fetch right away
        TBool iFetchOkToExit;
        /// Own: Default fetch accept policy
        MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection iFetchAcceptPolicy;
        /// Ref: View filter reference for fetch dialog builder
        CVPbkFieldTypeSelector* iViewFilter;
        /// Ref: Device config flag for reference for fetch dialog builder
        TBool iUsingDefaultConfig;
        /// Ref: Store uris reference for fetch dialog builder
        const CVPbkContactStoreUriArray* iStoreUris;
        /// Own: View for fetch names list
        MVPbkContactViewBase* iFetchNamesListView;
        /// Own: View for fetch groups list
        MVPbkContactViewBase* iFetchGroupsListView;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
    };

#endif // CPBK2CONTACTFETCHPHASE_H

// End of File
