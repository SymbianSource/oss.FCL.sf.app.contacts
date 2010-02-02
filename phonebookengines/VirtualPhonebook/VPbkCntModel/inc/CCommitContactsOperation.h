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
* Description:  The virtual phonebook commit contacts operation
*
*/



#ifndef CCOMMITCONTACTSOPERATION_H
#define CCOMMITCONTACTSOPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARAIONS
class MVPbkBatchOperationObserver;

namespace VPbkCntModel {

// FORWARD DECLARAIONS
class CContactStore;
class CContact;

NONSHARABLE_CLASS( CCommitContactsOperation ): 
        public CActive,
        public MVPbkContactOperation,
        public MVPbkContactObserver
    {
    public:
        /**
        * Two-phased constructor.
        * @param aContactStore the store that is used by the operation
        * @param aContacts virtual phonebook contacts
        * @param aObserver an observer to notify while commiting
        *
        * @return a new instance of this class
        */
        static CCommitContactsOperation* NewL(
                CContactStore& aContactStore, 
                const TArray<CContact*> aContacts, 
                MVPbkBatchOperationObserver& aObserver );
        ~CCommitContactsOperation();

    private: // From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();

    private: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // MVPbkContactObserver
        void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode, 
                                    TBool aErrorNotified );

    private: // Implementation
        CCommitContactsOperation(
                CContactStore& aContactStore, 
                MVPbkBatchOperationObserver& aObserver );
        void ConstructL( const TArray<CContact*>& aContacts );
        void NextStepL();
        void IssueRequest();

    private: // Data
        /// Ref: The store that is used by the operation
        CContactStore& iContactStore;
        /// Ref: an observer to notify while commiting
        MVPbkBatchOperationObserver& iObserver;
        /// Own: Contacts
        RPointerArray<CContact> iContacts;
        /// Operation state
        enum TState
            {
            ECommit,
            EComplete
            };
        TState iState;
    };

} // namespace VPbkCntModel

#endif // CCOMMITCONTACTSOPERATION_H

//End of file
