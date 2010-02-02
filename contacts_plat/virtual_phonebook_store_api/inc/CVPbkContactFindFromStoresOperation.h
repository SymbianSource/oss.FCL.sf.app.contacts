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
* Description:  An implementation of find from stores multioperation
*
*/


#ifndef CVPBKCONTACTFINDFROMSTORESOPERATION_H
#define CVPBKCONTACTFINDFROMSTORESOPERATION_H

#include <cvpbkmulticontactoperationbase.h>
#include <mvpbkcontactfindfromstoresobserver.h>


/**
 * An implementation of find from stores multioperation
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactFindFromStoresOperation :   
        public CVPbkMultiContactOperationBase,
        public MVPbkContactFindFromStoresObserver
    {
    public:
        /**
         * @param aObserver the observer that is notified after all
         *                  suboperations have completed.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkContactFindFromStoresOperation* NewLC( 
            MVPbkContactFindFromStoresObserver& aObserver );

        virtual ~CVPbkContactFindFromStoresOperation();

    private: // from CVPbkMultiContactOperationBase
        void HandleZeroSuboperations();
    
    private: // from MVPbkContactFindFromStoresObserver
        void FindFromStoreSucceededL( MVPbkContactStore& aStore, 
                MVPbkContactLinkArray* aResultsFromStore );
        void FindFromStoreFailed( MVPbkContactStore& aStore, TInt aError );
        void FindFromStoresOperationComplete();

    private: // implemenation
        CVPbkContactFindFromStoresOperation( 
                MVPbkContactFindFromStoresObserver& aObserver );
        void ConstructL();
        void HandleFindFromStoreSucceedL( MVPbkContactStore& aStore, 
                const MVPbkContactLinkArray& aResultsFromStore );
        
    private: // data
        /// Ref: An observer for this find operation
        MVPbkContactFindFromStoresObserver& iObserver;
        /// Own: completed operation count
        TInt iNumberOfCompleted;
    };

#endif // CVPBKCONTACTFINDFROMSTORESOPERATION_H

// End of File
