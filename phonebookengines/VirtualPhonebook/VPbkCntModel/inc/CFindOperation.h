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
* Description:  Virtual Phonebook Contact model find operation.
*
*/


#ifndef CFINDOPERATION_H
#define CFINDOPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <cntdb.h>
#include <MVPbkContactOperation.h>

// FORWARD DECLARATIONS
class MVPbkContactFindObserver;
class MVPbkContactLink;
class MVPbkFieldTypeList;
class CContactItemFieldDef;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

/**
 * Virtual Phonebook Contact model find operation.
 */
NONSHARABLE_CLASS( CFindOperation ): 
        public CActive,
        public MVPbkContactOperation,
        private MIdleFindObserver
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aContactStore A store to search
         * @param aSearchString A string to be searched
         * @param aFieldTypes Field types where to search
         * @param aObserver Observer where to notify when done
         * @return A new instance of this class
         */
        static CFindOperation* NewL(
                CContactStore& aContactStore, 
                const TDesC& aSearchString, 
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactFindObserver& aObserver);
        ~CFindOperation();

    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private: // From MIdleFindObserver
        void IdleFindCallback();

    private: // Implementation
        CFindOperation(CContactStore& aContactStore, 
                MVPbkContactFindObserver& aObserver);
        void ConstructL(const TDesC& aSearchString, 
                const MVPbkFieldTypeList& aFieldTypes);
        void IssueRequest();

    private: // Data
        ///Ref: contact store
        CContactStore& iContactStore;
        ///Ref: find observer
        MVPbkContactFindObserver& iObserver;
        ///Own: search string
        HBufC* iSearchString;
        ///Own: contact item field definition
        CContactItemFieldDef* iFieldDef;
        ///Own: contact models asynchronous finder
        CIdleFinder* iAsyncFinder;
        ///Own: matching contact ids
        CContactIdArray* iContactIds;
        ///Own: find results
        CVPbkContactLinkArray* iResults;
        enum TState
            {
            EBuildLinks,
            EComplete,
            ECancelled
            };
        ///Own: find operation state
        TState iState;
    };

} // namespace VPbkCntModel 

#endif // CFINDOPERATION_H

//End of file
