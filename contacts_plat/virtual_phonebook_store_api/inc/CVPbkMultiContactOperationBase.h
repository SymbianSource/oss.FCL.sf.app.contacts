/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An base class for multioperation implementations
*
*/



#ifndef CVPBKMULTICONTACTOPERATIONBASE_H
#define CVPBKMULTICONTACTOPERATIONBASE_H

// INCLUDES
#include <e32base.h>
#include <mvpbkcontactoperation.h>

// CLASS DECLARATION
/**
 * Base class for multi contact operations. Multi operations
 * contain other operations that are executed as one.
 *
 * @lib VPbkEng.lib
 */
class CVPbkMultiContactOperationBase : 
        public CBase,
        public MVPbkContactOperation
    {
    public: // Interface
        /**
         * Adds a new sub operation to this multi operation. Ownership
         * is taken by this object if this function does not leave.
         *
         * @param aOperation A Sub operation to add.
         */
        IMPORT_C void AddSubOperationL( MVPbkContactOperation* aOperation );

        /**
         * Returns number of sub operations in this multi operation.
         *
         * @return Number of sub operations in this multi operation.
         */
        IMPORT_C TInt SubOperationCount() const;
        
    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // interface for implementation opearation
        /**
         * If StartL is called and there are no sub operations
         * then this is called asynchronously to allow implementation
         * class to notify its observers.
         */
        virtual void HandleZeroSuboperations() = 0;
        static TInt AsyncCallback( TAny* aThis );
        
    protected: // Implementation
        CVPbkMultiContactOperationBase();
        ~CVPbkMultiContactOperationBase();

    private: // Data
        /// Own: Array of sub operations.
        RPointerArray<MVPbkContactOperation> iSubOperations;
        /// Own: an idle for zero suboperation case
        CIdle* iIdle;
    };

#endif // CVPBKMULTICONTACTOPERATIONBASE_H

// End of File
