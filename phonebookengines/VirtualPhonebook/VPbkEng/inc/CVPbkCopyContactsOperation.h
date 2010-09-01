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
* Description:  Copy contacts operation
*
*/


#ifndef CVPBKCOPYCONTACTSOPERATION_H
#define CVPBKCOPYCONTACTSOPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactCopyObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactFindObserver.h>

// FORWARD DECLARAIONS
class CVPbkContactManager;
class MVPbkBatchOperationObserver;
class MVPbkContactStore;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class CVPbkContactCopyPolicyManager;
class MVPbkContactCopyPolicy;
class CVPbkContactDuplicatePolicy;
class CVPbkContactLinkArray;


/**
 * Virtual phonebook copy contacts operation
 */
NONSHARABLE_CLASS(CVPbkCopyContactsOperation) 
    :   public CActive,
        public MVPbkContactOperation,
        private MVPbkSingleContactOperationObserver,
        private MVPbkContactCopyObserver,
        private MVPbkContactObserver,
        private MVPbkContactFindObserver
    {
    public:
        /**
         * Creates a new instance of this class
         * @param aCopyContactFlags Copy contact flags
         * @param aContactManager Contact manager
         * @param aLinks Contact link array
         * @param aTargetStore Target store
         * @param aCopiedContactLinks Copied contact links
         * @param aObserver Observer who is notified when completed
         * @return A new instance of this class
         */
        static CVPbkCopyContactsOperation* NewLC(
                TUint32 aCopyContactFlags,
                CVPbkContactManager& aContactManager,
                const MVPbkContactLinkArray& aLinks, 
				MVPbkContactStore* aTargetStore,
				CVPbkContactLinkArray* aCopiedContactLinks,
                MVPbkBatchOperationObserver& aObserver);
        ~CVPbkCopyContactsOperation();

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);
    private: // From MVPbkContactCopyObserver

        void ContactsSaved(MVPbkContactOperationBase& aOperation,
                MVPbkContactLinkArray* aResults);
        void ContactsSavingFailed(MVPbkContactOperationBase& aOperation,
                TInt aError);

    private: // From MVPbkContactObserver
        void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode, 
            TBool aErrorNotified );
    
    private: // From MVPbkContactFindObserver
        void FindCompleteL( MVPbkContactLinkArray* aResults );
        void FindFailed( TInt aError );
        
    private: // Implementation
        CVPbkCopyContactsOperation(
                CVPbkContactManager& aContactManager,
                const MVPbkContactLinkArray& aLinks,
                MVPbkContactStore* aTargetStore,
                CVPbkContactLinkArray* aCopiedContactLinks,
                MVPbkBatchOperationObserver& aObserver);
        void ConstructL( TUint32 aCopyContactFlags );
        void IssueRequest();
        void SetTargetStoreL();
        void RetrieveNextContactL();
        void DefaultCopyL();
        void CopyUsingPolicyL();
        void FindDuplicatesL();
        void ContinueAfterFailure( TInt aError );
        void AppendResultsL( MVPbkContactLinkArray& aLinks );
        
    private: // Data
        CVPbkContactManager& iContactManager;
        const MVPbkContactLinkArray& iLinks;
        /// Ref: the target of the copy
		MVPbkContactStore* iTargetStore;
		/// Ref: the array from the client for copied contacts
		CVPbkContactLinkArray* iCopiedContactLinks;
		/// Ref: ETrue if contacts are duplicated.
		TBool iDuplicateContacts;
        MVPbkBatchOperationObserver& iObserver;
        TInt iCurrentLinkIndex;
        /// Own: the operation instance for async operation
        MVPbkContactOperationBase* iOperation;
        /// Own: Currently copied contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Copy contacts manager
        CVPbkContactCopyPolicyManager* iCopyPolicyManager;
        /// Ref: The copy policy for the target store
        MVPbkContactCopyPolicy* iCopyPolicy;
        /// Own: a duplicate policy for duplicate checking
        CVPbkContactDuplicatePolicy* iDuplicatePolicy;
        enum TState
            {
            ERetrieveNextContact,
            EDefaultCopy,
            ECopyUsingPolicy,
            EFindDuplicates,
            ECommit,
            EComplete
            };
        TState iState;
        /// Own: The duplicate contacts that has been found
        RPointerArray<MVPbkStoreContact> iDuplicates;
        /// Ref: An array for merge and save. Doesn't own contact.
        RPointerArray<MVPbkStoreContact> iSourceContactsForMerge;
    };

#endif // CVPBKCOPYCONTACTSOPERATION_H
//End of file
