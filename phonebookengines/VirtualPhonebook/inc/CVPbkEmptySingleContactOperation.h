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
* Description:  Virtual Phonebook Empty single contact operation.
*
*/


#ifndef CVPBKEMPTYSINGLECONTACTOPERATION_H
#define CVPBKEMPTYSINGLECONTACTOPERATION_H

// INCLUDE FILES
#include <MVPbkContactOperation.h>
#include <e32base.h>

class MVPbkSingleContactOperationObserver;

// CLASS DECLARATION
/**
 * Virtual Phonebook Empty operation.
 */
NONSHARABLE_CLASS( CVPbkEmptySingleContactOperation ) : 
        public CActive,
		public MVPbkContactOperation
    {
    public: // Construction & destruction
        static CVPbkEmptySingleContactOperation* 
            NewL( MVPbkSingleContactOperationObserver& aObserver, TInt aErrorCode );
        CVPbkEmptySingleContactOperation();

    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // From CActive
        void RunL();
        void DoCancel();

    private: // Implementation
        CVPbkEmptySingleContactOperation(
            MVPbkSingleContactOperationObserver& aObserver, 
            TInt aErrorCode );
        void IssueRequest();
        ~CVPbkEmptySingleContactOperation();

    private: // Data
        // Ref: Operation observer
        MVPbkSingleContactOperationObserver& iObserver;
        // Own: Error code to complete single contact operation
        TInt iErrorCode;
    };

#endif // CVPBKEMPTYSINGLECONTACTOPERATION_H
//End of file

