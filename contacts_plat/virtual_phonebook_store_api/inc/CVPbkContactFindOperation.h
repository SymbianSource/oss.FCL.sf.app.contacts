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
* Description:  An implementation for find operation. Can be used e.g
*                in store domain implementation to handle Find -operations
*                from stores.
*
*/


#ifndef CVPBKCONTACTFINDOPERATION_H
#define CVPBKCONTACTFINDOPERATION_H

// INCLUDE FILES
#include <cvpbkmulticontactoperationbase.h>
#include <mvpbkcontactfindobserver.h>

class CVPbkContactLinkArray;

/**
 * An implementation for find operation
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactFindOperation :
        public CVPbkMultiContactOperationBase,
        public MVPbkContactFindObserver
    {
    public:
        /**
         * @param aObserver the observer that is called after 
         *                  all suboperations have completed or 
         *                  there are zero suboperations.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkContactFindOperation* NewLC( 
            MVPbkContactFindObserver& aObserver );

        /**
         * Destructor.
         */
        ~CVPbkContactFindOperation();

    private: // from CVPbkMultiContactOperationBase
        void HandleZeroSuboperations();

    private: // from MVPbkContactFindObserver
        void FindCompleteL( MVPbkContactLinkArray* aResults );
        void FindFailed( TInt aError );

    private: // Implementation
        CVPbkContactFindOperation( MVPbkContactFindObserver& aObserver );
        void ConstructL();
        void HandleZeroSuboperationsL();

    private: // Data
        ///Own: completed operation count
        TInt iCompleteCount;
        ///Own: find operation observer
        MVPbkContactFindObserver& iObserver;
        ///Own: find results
        CVPbkContactLinkArray* iResults;
    };

#endif // CVPBKCONTACTFINDOPERATION_H

// End of file
