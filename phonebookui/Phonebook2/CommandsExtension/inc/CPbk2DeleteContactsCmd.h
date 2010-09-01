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
* Description:  Phonebook 2 delete contacts command.
*
*/


#ifndef CPBK2DELETECONTACTSCMD_H
#define CPBK2DELETECONTACTSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkContactFindObserver.h>
#include "MPbk2ProcessDecorator.h"

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CAknProgressDialog;
class CEikProgressInfo;
class MPbk2ContactUiControl;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkStoreContact;
class MPbk2ContactNameFormatter;
class CVPbkContactLinkArray;
class MPbk2ContactLinkIterator;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 delete contacts command.
 */
NONSHARABLE_CLASS(CPbk2DeleteContactsCmd) :
        public CActive,
        public MPbk2Command,
        public MVPbkSingleContactOperationObserver,
        public MVPbkBatchOperationObserver,
        public MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2DeleteContactsCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2DeleteContactsCmd();

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
                MVPbkContactOperationBase& aOperation, TInt aError );

    private: // From MVPbkBatchOperationObserver
        void StepComplete( MVPbkContactOperationBase& aOperation,
                TInt aStepSize );
        TBool StepFailed( MVPbkContactOperationBase& aOperation,
                TInt aStepSize, TInt aError );
        void OperationComplete( MVPbkContactOperationBase& aOperation );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed( TInt aCancelCode );

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
        CPbk2DeleteContactsCmd( MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void RetrieveContactL( const MVPbkContactLink& aContactLink );
        void DoDeleteContactsL();
        TInt FilterErrors( TInt aErrorCode );
        void IssueRequest();
        void IssueStopRequest( TProcessState aState );
        void ConfirmDeletionL();
        TInt GetContactCountL();
        TBool IsFromReadOnlyStore(
                const MVPbkContactLink& aContactLink ) const;

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Contacts to delete
        MVPbkContactLinkArray* iContactLinkArray;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Delete operation
        MVPbkContactOperationBase* iDeleteOperation;
        /// Own: List contacts operation
        MVPbkContactOperationBase* iListContactsOperation;
        /// Own: The store contact to delete
        MVPbkStoreContact* iStoreContact;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Contact to be deleted in next cycle
        CVPbkContactLinkArray* iContactsToDelete;
        /// Own: Count of contacts to be deleted
        TInt iContactCount;
        /// Own: Contact link iterator
        MPbk2ContactLinkIterator* iContactIterator;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2DELETECONTACTSCMD_H

// End of File
