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
* Description:  Phonebook 2 compress policy manager.
*
*/


#ifndef CPBK2COMPRESSPOLICYMANAGER_H
#define CPBK2COMPRESSPOLICYMANAGER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2CompressPolicyManager.h"
#include <MVPbkBatchOperationObserver.h>

// FORWARD DECLARATIONS
class MPbk2StoreObservationRegister;
class MPbk2CompressPolicy;
class MVPbkContactOperationBase;
class CVPbkContactManager;

// CLASS DECLARATIONS

/**
 * Phonebook 2 compress policy manager.
 * Responsible for owning and managing compress policies.
 */
NONSHARABLE_CLASS(CPbk2CompressPolicyManager) :
        public CBase,
        public MPbk2CompressPolicyManager,
        public MVPbkBatchOperationObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreObservationRegister     Store manager.
         * @param aContactManager               Virtual Phonebook
         *                                       contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2CompressPolicyManager* NewL(
                MPbk2StoreObservationRegister& aStoreObservationRegister,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2CompressPolicyManager();

    public: // From MPbk2CompressPolicyManager
        void CompressStores();
        void CancelCompress();
        void StartAllPolicies();
        void StopAllPolicies();

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

    private: // Implementation
        CPbk2CompressPolicyManager(
                MPbk2StoreObservationRegister& aStoreObservationRegister,
                CVPbkContactManager& aContactManager );
        void ConstructL();
        void ConstructPoliciesL();

    private: // Data
        /// Ref: Store observation register
        MPbk2StoreObservationRegister& iStoreObservationRegister;
        /// Own: Array of compress policies
        RPointerArray<MPbk2CompressPolicy> iPolicyArray;
        /// Own: Compress operation
        MVPbkContactOperationBase* iCompressOperation;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

#endif // CPBK2COMPRESSPOLICYMANAGER_H

// End of File
