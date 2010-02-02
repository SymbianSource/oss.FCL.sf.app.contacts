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
* Description:  Phonebook 2 Group UI Extension delete group command.
*
*/


#ifndef CPGUDELETEGROUPCMD_H
#define CPGUDELETEGROUPCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2ProcessDecorator.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkBatchOperationObserver.h>

// FORWARD DECLARATIONS
class CAknProgressDialog;
class CEikProgressInfo;
class MPbk2ContactNameFormatter;
class MPbk2ContactUiControl;
class CVPbkContactManager;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkStoreContact;
class MVPbkContactGroup;

// CLASS DECLARATION

/**
 * Phonebook 2 Group UI Extension delete group command.
 */
class CPguDeleteGroupCmd : 
        public CActive,
        public MPbk2Command,
        private MVPbkSingleContactOperationObserver,
        private MVPbkBatchOperationObserver,
        private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPguDeleteGroupCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguDeleteGroupCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

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

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EConfirming,
            EStarting,
            EDeleting,
            EStopping,
            ECanceling
            };

    private: // Implementation
        CPguDeleteGroupCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void RetrieveContactL(
                const MVPbkContactLink& aContactLink );
        TInt FilterErrors(
                TInt aErrorCode );
        void IssueRequest();
        void IssueStopRequest(
                TProcessState aState );
        void ConfirmDeletionL();
        void DoDeleteContactsL();

    private:  // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Links to the contacts to delete
        MVPbkContactLinkArray* iContactLinkArray;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Delete operation
        MVPbkContactOperationBase* iDeleteOperation;
        /// Own: The group to delete
        MVPbkContactGroup* iContactGroup;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Current state of process
        TProcessState iState;
    };

#endif // CPGUDELETEGROUPCMD_H

// End of File
