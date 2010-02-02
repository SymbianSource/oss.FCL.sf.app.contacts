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
* Description:  An implementation for operations that needs to do a big task
*                and report steps to observers. Can be used e.g
*                in store domain implementation to handle multiple store
*                operations
*
*/


#ifndef CVPBKBATCHOPERATION_H
#define CVPBKBATCHOPERATION_H

// INCLUDE FILES
#include <cvpbkmulticontactoperationbase.h>
#include <mvpbkbatchoperationobserver.h>

// CLASS DECLARATION

/**
 * An implementation for batch operations
 *
 * @lib VPbkEng.lib
 */
class CVPbkBatchOperation :   
        public CVPbkMultiContactOperationBase,
        public MVPbkBatchOperationObserver
    {
    public:
        /**
         * @param aObserver the observer that is called after 
         *                  all suboperations have completed or 
         *                  there are zero suboperations.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkBatchOperation* NewLC( 
            MVPbkBatchOperationObserver& aObserver );
        ~CVPbkBatchOperation();

    // from base class CVPbkMultiContactOperationBase
    private:
        void HandleZeroSuboperations();

    // from base class MVPbkBatchOperationObserver
        void StepComplete( MVPbkContactOperationBase& aOperation, TInt aStepSize );
        TBool StepFailed( MVPbkContactOperationBase& aOperation, TInt aStepSize, 
            TInt aError );
        void OperationComplete( MVPbkContactOperationBase& aOperation );

    private: // Implementation
        CVPbkBatchOperation( MVPbkBatchOperationObserver& aObserver );

    private: // Data
        /// Ref: Operation observer
        MVPbkBatchOperationObserver& iObserver;
        /// Own: A counter for completed suboperations
        TInt iCompleteCount;
    };

#endif // CVPBKBATCHOPERATION_H

// End of file
