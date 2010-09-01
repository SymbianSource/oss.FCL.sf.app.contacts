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
* Description:  Virtual Phonebook Empty batch operation.
*
*/


#ifndef CVPBKEMPTYBATCHOPERATION_H
#define CVPBKEMPTYBATCHOPERATION_H

// INCLUDE FILES
#include <CVPbkMultiContactOperationBase.h>
#include <MVPbkBatchOperationObserver.h>

// CLASS DECLARATION
/**
 * Virtual Phonebook Empty batch operation.
 */
class CVPbkEmptyBatchOperation : 
        public CActive,
		public MVPbkContactOperation
    {
    public: // Construction & destruction
        IMPORT_C static CVPbkEmptyBatchOperation* NewLC(MVPbkBatchOperationObserver& aObserver);
        ~CVPbkEmptyBatchOperation();

    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // From CActive
        void RunL();
        void DoCancel();

    private: // Implementation
        CVPbkEmptyBatchOperation(MVPbkBatchOperationObserver& aObserver);
        void IssueRequest();

    private: // Data
        // Ref: Operation observer
        MVPbkBatchOperationObserver& iObserver;
    };

#endif // CVPBKEMPTYBATCHOPERATION_H
//End of file

