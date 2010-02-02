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
* Description:  A command for copying contacts to the (U)SIM store.
*
*/


#ifndef CPSU2COPYTOSIMCMD_H
#define CPSU2COPYTOSIMCMD_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2ProcessDecorator.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactStore;
class MVPbkContactOperationBase;
class MVPbkFieldType;
class CVPbkContactLinkArray;
class CPbk2FieldPropertyArray;
class MPbk2FieldProperty;
class MPbk2ContactUiControl;
class CPsu2CopyToSimFieldInfoArray;
class CPsu2SimContactProcessor;
class CVPbkFieldTypeRefsList;
class CPsu2ContactCopyInspector;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 *  A command for copying contacts to the (U)SIM store.
 */
class CPsu2CopyToSimCmd :
        public CActive,
        public MPbk2Command,
        public MVPbkContactStoreObserver,
        private MVPbkSingleContactOperationObserver,
        private MVPbkContactObserver,
        private MPbk2ProcessDecoratorObserver
    {
    private:
        enum TSimSpaceStatus
            {
            ESimSpaceIsFull,
            ESimSpaceIsNotFull,
            ESimSpaceUnknown
            };
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         *
         * @param aUiControl    UI control.
         * @param aTargetStore  The target store for the copied contacts.
         * @return A new instance of this class.
         */
        static CPsu2CopyToSimCmd* NewL(
                MPbk2ContactUiControl& aUiControl,
                MVPbkContactStore& aTargetStore);

        /**
         * Destructor.
         */
        ~CPsu2CopyToSimCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);
    public: // From MVPbkContactStoreObserver
    	void StoreReady(MVPbkContactStore& aContactStore);
    	void StoreUnavailable(MVPbkContactStore& aContactStore, 
    	                TInt aReason);
    	void HandleStoreEventL(
    	                MVPbkContactStore& aContactStore, 
    	                TVPbkContactStoreEvent aStoreEvent);
            
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError);

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult);
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified);

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(TInt aCancelCode);

    private: // Implementation
        CPsu2CopyToSimCmd(
                MPbk2ContactUiControl& aUiControl,
                MVPbkContactStore& aTargetStore);
        void ConstructL();
        void IssueRequest();
        void StartL();
        void HandleCanceling();
        void RetrieveSourceContactL();
        void CreateSimContactsL();
        void StartCopyL();
        void CopyCurrentContactL();
        void ShowResultsNoteL();
        void CompleteL();
        MVPbkStoreContact* NextSimContact();
        void HandleError(TInt aError);
        void MoveToNextContact();
        void CopyNextSimContact();
        void CompleteWithResults();
        // return current sim space status
        TSimSpaceStatus GetSimSpaceStatusL();
        // Show information note about SIM is full
        void ShowSimIsFullNoteL();

    private:  // Data
        /// Ref: UI control which can store or contact control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Parent control of the store list control
        MPbk2ContactUiControl* iNameListUiControl;
        /// Ref: The target (U)SIM store
        MVPbkContactStore& iTargetStore;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Selected contacts
        CVPbkContactLinkArray* iLinkArray;
        /// Own: An array of sim copy infos
        CPsu2CopyToSimFieldInfoArray* iCopyToSimTypeInfos;
        /// Own: Processes contacts to correct form
        CPsu2SimContactProcessor* iSimContactProcessor;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: An operation to get a contact from the link
        MVPbkContactOperationBase* iContactRetriever;
        /// Own: The task that will be done next
        TInt iNextTask;
        /// Own: An index of the contact that is currently copied
        TInt iCurrentIndex;
        /// Own: The current source contact to copy
        MVPbkStoreContact* iCurrentSourceContact;
        /// Own: The current (U)SIM contact to copy
        MVPbkStoreContact* iCurrentSimContact;
        /// Own: An array of 'to be copied' sim contacts
        RPointerArray<MVPbkStoreContact> iSimContacts;
        /// Own: The number of copied contacts
        TInt iCopied;
        /// Own: The flag indicating if control should be updated after command
        ///      execution.
        TBool iUpdateControl;
        /// Own: The copy inspector
        CPsu2ContactCopyInspector* iContactInspector;
        /// Own: Flag indicating if all contacts were copied completely
        TBool iIsCopiedCompletely;
        /// Own: Flag indicating if the last contact copy was failed for memory  
        ///      full, this flag is used for the case we use the sim not 
        ///      providing its free space When it's full, its copy will fail.
        TBool iIsLastCopyFailedForSimIsFull;
        /// Own: The target store status.
        TInt iTargetStoreStatus;

        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPSU2COPYTOSIMCMD_H

// End of File
