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
* Description:  Phonebook 2 copy from Phonebook to SIM command object.
*
*/


#ifndef CPSU2COPYFROMPBKCMD_H
#define CPSU2COPYFROMPBKCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2FetchDlgObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class MVPbkContactStore;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkBaseContact;
class CPsu2CopyToSimFieldInfoArray;
class MVPbkBaseContactField;
class MVPbkFieldType;
class MPbk2DialogEliminator;
class MPbk2ContactUiControl;
class CPsu2UIExtensionPlugin;

// CLASS DECLARATION

/**
 * Phonebook 2 copy from Phonebook to SIM command object.
 * Responsible for displaying a fetch and address select so that
 * the user selects one contact field from Phonebook. Responsible
 * copying that contact field to SIM memory.
 */
class CPsu2CopyFromPbkCmd :
        public CActive,
        public MPbk2Command,
        public MPbk2FetchDlgObserver,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactObserver,
        private MPbk2ProcessDecoratorObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aTargetStore      The target store for copied contacts.
         * @param aUiControl        Contact UI control.
         * @return  A new instance of this class.
         */
        static CPsu2CopyFromPbkCmd* NewL(
                MVPbkContactStore& aTargetStore,
                MPbk2ContactUiControl& aUiControl,
                CPsu2UIExtensionPlugin* aPsu2UIExensionPlugin );

        /**
         * Destructor.
         */
        ~CPsu2CopyFromPbkCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);


    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MPbk2FetchDlgObserver
        TPbk2FetchAcceptSelection AcceptFetchSelectionL(
                TInt aNumMarkedEntries,
                MVPbkContactLink& aLastSelection );
        void FetchCompletedL(
                MVPbkContactLinkArray* aMarkedEntries );
        void FetchCanceled();
        void FetchAborted();
        TBool FetchOkToExit();

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed( TInt aCancelCode );

    private: // Implementation
        CPsu2CopyFromPbkCmd(
                MVPbkContactStore& aTargetStore,
                MPbk2ContactUiControl& aUiControl,
                CPsu2UIExtensionPlugin* aPsu2UIExensionPlugin );
        void ConstructL();
        void QueryContactL();
        void QueryAddressL();
        void CopyContactL();
        void IssueRequest();
        TBool AddNameL(
                MVPbkBaseContact& aSource,
                MVPbkStoreContact& aTarget);
        TBool AppendSupportedFieldL(
                MVPbkStoreContactField& aSource,
                MVPbkStoreContact& aTarget);
        TBool CopyFieldL(
                const MVPbkBaseContactField& aFieldToCopy,
                MVPbkStoreContact& aTarget,
                const MVPbkFieldType& aSimType );
        TBool QueryNameL(
                TDes& aName,
                TInt aMaxDataLength );
        TBool QueryNumberL(
                TInt aPromptResourceId,
                TDes& aData,
                TInt aMaxDataLength,
                TInt aMaxEditorLength );
        void ShowWaitNoteL();
        void SetTitlePaneTextL( const TDesC& aText );
        HBufC* GetTitlePaneTextL();

    private: // Data
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Ref: Target store
        MVPbkContactStore& iTargetStore;
        /// Own: Copy contact operation
        MVPbkContactOperationBase* iCopyOperation;
        /// Own: Next task to be handled
        TInt iNextTask;
        /// Own: Store contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Source contact
        MVPbkStoreContact* iSourceStoreContact;
        /// Own: Target contact
        MVPbkStoreContact* iTargetStoreContact;
        /// Own: Chosen contact field
        MVPbkStoreContactField* iStoreContactField;
        /// Own: SIM field infos
        CPsu2CopyToSimFieldInfoArray* iSimFieldInfos;
        /// Ref: Fetch dialog eliminator
        MPbk2DialogEliminator* iFetchDialogEliminator;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iUiControl;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Help variable for querying name length
        TBool iLengthQuery;
        /// Own: Last error code
        TInt iLastError;       
        /// Own: Original title pane text
        HBufC* iTitlePaneText; 
        /// Own: Psu2 UI extension plugin
        CPsu2UIExtensionPlugin* iPsu2UIExensionPlugin;
    };

#endif // CPSU2COPYFROMPBKCMD_H

// End of File
