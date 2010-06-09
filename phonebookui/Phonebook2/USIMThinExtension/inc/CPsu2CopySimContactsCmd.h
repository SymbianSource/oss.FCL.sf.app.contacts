/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 copy "new SIM" contacts to phone memory.
*
*/


#ifndef CPSU2COPYSIMCONTACTSCMD_H
#define CPSU2COPYSIMCONTACTSCMD_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MPbk2ProcessDecorator.h>
#include <RPbk2LocalizedResourceFile.h>

// FORWARD DECLARATIONS
class MVPbkContactStore;
class MPbk2StartupMonitor;
class CVPbkContactStoreUriArray;
class MVPbkContactView;
class CVPbkContactLinkArray;
class CVPbkContactCopier;
class CVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 copy "new SIM" contacts to phone memory.
 */
class CPsu2CopySimContactsCmd : public CActive,
                                public MPbk2Command,
                                private MVPbkContactStoreObserver,
                                private MVPbkContactViewObserver,
                                private MVPbkBatchOperationObserver,
                                private MPbk2ProcessDecoratorObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aStartupMonitor   Startup monitor.
         * @return  A new instance of this class.
         */
        static CPsu2CopySimContactsCmd* NewL(
                MPbk2StartupMonitor& aStartupMonitor );

        /**
         * Destructor.
         */
        ~CPsu2CopySimContactsCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

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

    private: // From MVPbkBatchOperationObserver
        void StepComplete(
                MVPbkContactOperationBase& aOperation,
                TInt aStepSize );
        TBool StepFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aStepSize,
                TInt aError );
        void OperationComplete(
                MVPbkContactOperationBase& aOperation );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // Implementation
        CPsu2CopySimContactsCmd(
                MPbk2StartupMonitor& aStartupMonitor );
        void ConstructL();
        void IssueRequest();
        void IssueRequest(
                TInt aResult );
        void CheckCopyPrerequisitesL();
        void OpenSimStoresL();
        void CheckSimStorePrerequisitesL();
        void LaunchNewSIMInsertedQueryL();
        void SetShowSIMContactsL( TBool aShow );
        void CreateSimViewsL();
        void CopyContactsL();
        void ShowResultsL();
        void CompleteL();
        void PublishOpenCompleteL();
        void CompleteWithError(
                TInt aError );
        TBool ValidStoreConfigurationL();
        TBool NewSimCardForPhonebook2AppL();
        void CloseStores();
        void SetNewSimCardKeyL();

    private: // Data
        /// Ref: Startup monitor
        MPbk2StartupMonitor& iStartupMonitor;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Thin extension resource file
        RPbk2LocalizedResourceFile iResourceFile;
        /// Own: Indicates next task
        TInt iNextTask;
        /// Own: URI array that contains the SIM source stores
        CVPbkContactStoreUriArray* iValidSourceStoreUris;
        /// Ref: Array of source store references
        RPointerArray<MVPbkContactStore> iSourceStores;
        /// Own: Counter for opened SIM stores and views
        TInt iCounter;
        /// Own: Array of SIM contact views
        RPointerArray<MVPbkContactView> iSourceViews;
        /// Own: Source contact links
        CVPbkContactLinkArray* iSourceContactLinks;
        /// Own: Copy operation
        MVPbkContactOperationBase* iCopyOperation;
        /// Own: Number of successfully copied contacts
        TInt iCopiedSuccessfully;
        /// Own: The virtual phonebook contact copier
        CVPbkContactCopier* iCopier;
        /// Own: An array for copy results
        CVPbkContactLinkArray* iCopiedContacts;
    };

#endif // CPSU2COPYSIMCONTACTSCMD_H

// End of File
