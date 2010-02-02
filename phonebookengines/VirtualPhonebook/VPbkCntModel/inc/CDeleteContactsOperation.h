/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation object to delete contacts from contacts database
*
*/


#ifndef CDELETECONTACTSOPERATION_H
#define CDELETECONTACTSOPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <shareddataclient.h>

// FORWARD DECLARAIONS
class MVPbkBatchOperationObserver;
class CContactIdArray;
class MVPbkContactLinkArray;

namespace VPbkEngUtils { 
class CVPbkDiskSpaceCheck;
} /// namespace

namespace VPbkCntModel {

// FORWARD DECLARAIONS
class CContactStore;
class CContactLink;
class CCompressDbOperation;

NONSHARABLE_CLASS( CDeleteContactsOperation ): 
        public CActive,
        public MVPbkContactOperation
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aContactStore     Contact store.
         * @param aLinks            Contacts to delete.
         * @param aObserver         Observer for the operation.
         * @param aSharedDataClient SharedDataClient for freeing disk space
         * @param aDiskSpaceChecker Disk space checker 
         * @return A new instance of this class.
         */
        static CDeleteContactsOperation* NewL(
                CContactStore& aContactStore, 
                const MVPbkContactLinkArray& aLinks, 
                MVPbkBatchOperationObserver& aObserver,
                RSharedDataClient* aSharedDataClient,
                VPbkEngUtils::CVPbkDiskSpaceCheck& aDiskSpaceChecker );

        /**
         * Destructor.
         */
        ~CDeleteContactsOperation();

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // Implementation
        CDeleteContactsOperation(
                CContactStore& aContactStore, 
                MVPbkBatchOperationObserver& aObserver,
                RSharedDataClient* aSharedDataClient,
                VPbkEngUtils::CVPbkDiskSpaceCheck& aDiskSpaceChecker );
        void ConstructL(
                const MVPbkContactLinkArray& aLinks);
        void IssueRequest();
        void RunDeleteL();
        void RunCompressL(TBool aForce);
        void StartTransactionLC();
        void CommitTransactionLP();
        void DoRequestFreeDiskSpace();
        void DoRequestFreeDiskSpaceLC();
        void DoCancelFreeDiskSpaceRequest();
        void PrepareNextDelete();        
        void UpdateTimeStampOfAllContactsInGroupL();
        TBool CurrentItemIsGroupL();
        
    private: // Data structures
        /// Operation state
        enum TState
            {
            EDelete,
            EComplete
            };

    private: // Data
        /// Ref: Contact store
        CContactStore& iContactStore;
        /// Ref: Observer
        MVPbkBatchOperationObserver& iObserver;
        /// Own: Array of remaining contact IDs that are being deleted
        CContactIdArray* iRemainingContactIds;        
        /// Own: Array of contact IDs that are currently being deleted
        CContactIdArray* iCurrentContactIds;
        /// Own: Operation state
        TState iState;
        /// Ref: Shared data client for freeing disk space
        RSharedDataClient* iSharedDataClient;
        /// Ref: Reference to diskspace checker
        VPbkEngUtils::CVPbkDiskSpaceCheck& iDiskSpaceChecker;
        /// Own: Amount of free disk space required
        TBool iFreeDiskSpaceRequired;
        /// Own: Index of item to delete from iCurrentContactIds
        TInt iCurrentContactIndex;
        /// Own: number of groups in contact store
        TInt iGroupCount;
    };

} // namespace VPbkCntModel

#endif // CDELETECONTACTSOPERATION_H

// End of File
