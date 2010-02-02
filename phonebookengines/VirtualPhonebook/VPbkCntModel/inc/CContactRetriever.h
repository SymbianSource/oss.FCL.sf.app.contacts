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
* Description:  Virtual Phonebook asynchronous single contact retriever 
                 operation.
*
*/


#ifndef CCONTACTRETRIEVER_H
#define CCONTACTRETRIEVER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>


// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkSingleContactOperationObserver;
namespace VPbkEngUtils { class CVPbkAsyncOperation; }
class CContactDatabase;

namespace VPbkCntModel {

class CContactLink;
class CContactStore;
class CContact;

// CLASS DECLARATIONS

/**
 * Virtual Phonebook asynchronous single contact retriever operation.
 */
NONSHARABLE_CLASS( CContactRetriever ): 
        public CBase,
        public MVPbkContactOperation
    {
    public:  // Constructor, destructor
        /**
         * Creates a asynchronous contact retriever.
         */
        static CContactRetriever* NewLC(
                const MVPbkContactLink& aLink,
                CContactStore& aContactStore,
                MVPbkSingleContactOperationObserver& aObserver);

        /**
         * Creates a asynchronous contact retriever.
         * Takes ownership of the aLink.
         */
        static CContactRetriever* NewLC(
                MVPbkContactLink* aLink,
                CContactStore& aContactStore,
                MVPbkSingleContactOperationObserver& aObserver);

        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CContactRetriever();

    public: // from MVPbkContactOperation
        void StartL();
        void Cancel();
        
    private: // implementation
        CContactRetriever(CContactStore& aContactStore,
                         MVPbkSingleContactOperationObserver& aObserver);
        void BaseConstructL();
        void ConstructL(const MVPbkContactLink& aLink);
        void ConstructL(MVPbkContactLink* aLink);
        void DoReadL(MVPbkSingleContactOperationObserver& aObserver);
        void RetrieveError(MVPbkSingleContactOperationObserver& aObserver, TInt aError);

    private: // data members
        ///Ref: Contact store
        CContactStore& iContactStore;
        ///Ref: contact operation observer
        MVPbkSingleContactOperationObserver& iObserver;
        ///Own: contact link
        CContactLink* iLink;
        ///Own: Asynchronous operation
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOperation;

    };

} // namespace VPbkCntModel

#endif // CCONTACTRETRIEVER_H
//End of file

