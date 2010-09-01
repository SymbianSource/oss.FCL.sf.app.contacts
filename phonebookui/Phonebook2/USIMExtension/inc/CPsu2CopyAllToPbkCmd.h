/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPSU2COPYALLTOPBKCMD_H
#define CPSU2COPYALLTOPBKCMD_H

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
class CVPbkContactStoreUriArray;
class MVPbkContactView;
class CVPbkContactLinkArray;
class CVPbkContactCopier;
class CVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 copy "new SIM" contacts to phone memory.
 */
class CPsu2CopyAllToPbkCmd : public CActive,
                                public MPbk2Command,
                                private MVPbkContactStoreObserver,
                                private MVPbkContactViewObserver,
                                private MVPbkBatchOperationObserver,
                                private MPbk2ProcessDecoratorObserver
    {
    public: // Constructors and destructor
    
        /**
         * Creates a new instance of this class.
         * @return  A new instance of this class.
         */
        static CPsu2CopyAllToPbkCmd* NewL( );

        /**
         * Destructor.
         */
        ~CPsu2CopyAllToPbkCmd();

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
        /// Tasks
        enum TNextTask
            {
            EOpenStores,
            ECheckSimStorePrerequisites,
            ELaunchCopyAllSIMContactsQueryL,
            ECreateSimViews,
            ECopyContacts,
            EComplete
            };
    
        CPsu2CopyAllToPbkCmd( );
        void ConstructL();
        void OpenStoresL();
        void CheckSimStorePrerequisitesL();
        void LaunchCopyAllSIMContactsQueryL();
        void CreateSimViewsL();
        void CopyContactsL();
        void ShowResultsL();
        void Complete();
        void CompleteWithError(
                TInt aError );
        void CloseStores();
        
        void Finish( TInt aReason );
        
        void StartNext( TNextTask aNextTask );
  

    private: // Data
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        TBool iDecoratorShown;
        /// Own: Indicates next task
        TInt iNextTask;
        /// Own: URI array that contains the SIM source stores
        CVPbkContactStoreUriArray* iValidSourceStoreUris;
        /// Ref: Array of source store references
        RPointerArray<MVPbkContactStore> iSourceStores;
        /// Target store reference
        MVPbkContactStore* iTargetStore;
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
        /// Own: If there is just one contact being copied, store the title
        /// for copy results note
        HBufC* iTheOnlyContactTitle;
    };

#endif // CPSU2COPYALLTOPBKCMD_H

// End of File
