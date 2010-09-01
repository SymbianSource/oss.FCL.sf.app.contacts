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
* Description:  Virtual Phonebook Contact model phone number match operation.
*
*/
 

#ifndef CMATCHPHONENUMBEROPERATION_H
#define CMATCHPHONENUMBEROPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperation.h>

// FORWARD DECLARATIONS
class MVPbkContactFindObserver;
class MVPbkContactLink;
class CContactIdArray;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

/**
 * Virtual Phonebook Contact model phone number match operation.
 */
NONSHARABLE_CLASS( CMatchPhoneNumberOperation ): 
        public CActive,
        public MVPbkContactOperation
    {
    public:
        static CMatchPhoneNumberOperation* NewL(
                CContactStore& aContactStore, 
                const TDesC& aPhoneNumber, 
                TInt aMaxMatchDigits,
                MVPbkContactFindObserver& aObserver);
        ~CMatchPhoneNumberOperation();

    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private: // Implementation
        CMatchPhoneNumberOperation(
                CContactStore& aContactStore, 
                TInt aMaxMatchDigits, 
                MVPbkContactFindObserver& aObserver);
        void ConstructL(const TDesC& aPhoneNumber);
        void IssueRequest();

    private: // Data
        ///Ref: contact store
        CContactStore& iContactStore;
        ///Ref: find observer
        MVPbkContactFindObserver& iObserver;
        ///Own: max digits to match
        TInt iMaxMatchDigits;
        ///Own: phone number
        HBufC* iPhoneNumber;
        ///Own: matching contact ids
        CContactIdArray* iContactIds;
        ///Own: find results
        CVPbkContactLinkArray* iResults;
        enum TState
            {
            EMatch,
            EBuildLinks,
            EComplete,
            ECancelled
            };
        ///Own: find operation state
        TState iState;

    };

} // namespace VPbkCntModel 

#endif // CMATCHPHONENUMBEROPERATION_H

//End of file
