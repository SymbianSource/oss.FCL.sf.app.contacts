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
* Description:  The virtual phonebook asynchronous contact operation
*
*/


#ifndef VPBKCNTMODEL_CASYNCCONTACTOPERATION_H
#define VPBKCNTMODEL_CASYNCCONTACTOPERATION_H


// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MVPbkContactObserver.h>


// FORWARD DECLARATIONS
class CContactDatabase;
class CContactItem;

namespace VPbkEngUtils { 
class CVPbkDiskSpaceCheck;
} /// namespace

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;
class CContact;

// CLASS DECLARATIONS

/**
 * Asynchronous contact operation.
 */
NONSHARABLE_CLASS( CAsyncContactOperation ): public CAsyncOneShot
    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
		 * @param aContactStore contact store.
         */
        static CAsyncContactOperation* NewL
            ( CContactStore& aContactStore );

        ~CAsyncContactOperation();

    public:  // New functions
        /**
         * Prepares an asynchronous contact operation.
         *
         * @param aOpCode       the operation to perform.
         * @param aContact      target contact of the operation.
         * @param aObserver     observer of the operation.
         * @exception KErrInUse if an operation is currently executing.
         * @precond aOpCode is EContactSave or EContactCommit. 
         */
        void PrepareL
            ( MVPbkContactObserver::TContactOp aOpCode, 
            const CContact& aContact, 
            MVPbkContactObserver& aObserver );
            
        /**
         * Prepares an asynchronous contact operation.
         *
         * @param aOpCode       the operation to perform.
         * @param aContactId    id of the target contact of the operation.
         * @param aObserver     observer of the operation.
         * @exception KErrInUse if an operation is currently executing.
         * @precond aOpCode is EContactSave or EContactCommit. 
         */
        void PrepareL
            ( MVPbkContactObserver::TContactOp aOpCode, 
            TContactItemId aContactId, 
            MVPbkContactObserver& aObserver );            

        /**
         * Executes the prepared operation. Calls the observer passed to 
         * PrepareL when the operation completes.
         */
        void Execute() { CAsyncOneShot::Call(); }

        /**
         * Overloaded Cancel. Determines if an async operation has finished.
         *
         * @param aContact    contact item for comparison against
         *					  iClientContact.
         */
        void Cancel( CContactItem* aContact );
        
    private:  // from CAsyncOneShot
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private:  // Implementation
		CAsyncContactOperation( CContactStore& aContactStore );
		void ConstructL();

    private:  // Data
        ///Ref: contact store
		CContactStore& iContactStore;
        ///Own: contact operation code
        MVPbkContactObserver::TContactOp iOpCode;
        ///Ref: client contact
        const CContact* iClientContact;
        ///Own: contact item id
        TContactItemId iTargetContactId;
        ///Ref: contact observer
        MVPbkContactObserver* iObserver;
        ///Own: file system session
        RFs iFs;
        ///Own: Disk Space Checker
        VPbkEngUtils::CVPbkDiskSpaceCheck* iDiskSpaceChecker;
    };


} // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_CASYNCCONTACTOPERATION_H
//End of file


