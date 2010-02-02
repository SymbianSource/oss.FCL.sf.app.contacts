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
* Description:  Contact easy manager
*
*/


#ifndef CVPBKCONTACTEASYMANAGER_H_
#define CVPBKCONTACTEASYMANAGER_H_

// INCLUDES

#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
// FORWARD DECLARATIONS
class MVPbkOperationObserver;
class MVPbkOperationErrorObserver;
class CVPbkContactManager;
class CVPbkContactLinkArray;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;

// CLASS DECLARATION

/**
 * An API for easy locking and retrieving of several contacts.
 */
NONSHARABLE_CLASS( CVPbkContactEasyManager ) : public CBase,
    public MVPbkSingleContactOperationObserver,
    public MVPbkContactObserver,
    public MVPbkContactOperationBase
    {
    public:

        /**
         * @param aContactManager Reference to a contact manager which already
         *        has stores opened.
         */
        static CVPbkContactEasyManager* NewL(
            CVPbkContactManager& aContactManager );

	   ~CVPbkContactEasyManager();

    public: // new methods
        
        /**
         * Retrieves and locks contacts asynchronously.
         * The observer is informed when all the contacts have been retrieved.
         * 
         * @param aContactLinks Links for contacts that are to be retrieved.
         * 
         * @param aRetrievedContacts A reference to an array where the
         *        retrieved contacts are placed into. Ownership of the
         *        contacts is given to the owner of the array.
         * 
         * @param aObserver The observer.
         */
        void RetrieveAndLockContactsL(
            const MVPbkContactLinkArray& aContactLinks,            
            RPointerArray<MVPbkStoreContact>& aRetrievedContacts,
            MVPbkOperationObserver& aObserver,
            MVPbkOperationErrorObserver& aErrorObserver );

        /**
         * Cancel everything.
         * Cancelling will not clear or delete the retrieved contacts from the
         * array that was given as reference to RetrieveAndLockContactsL.
         * The client will still have ownership of those fetched contacts.
         *
         * The observer will not be called. 
         */
        void Cancel();

    private: // from MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError );
    private: // from MVPbkContactObserver
        void ContactOperationCompleted(
            MVPbkContactObserver::TContactOpResult aResult );
        void ContactOperationFailed(
            MVPbkContactObserver::TContactOp aOpCode,
            TInt aErrorCode, TBool aErrorNotified );

    private: // new methods
        void DoSingleContactOperationCompleteL(
            MVPbkStoreContact* aContact );
        void DoContactOperationCompletedL();
        static CVPbkContactLinkArray* CloneLinkArrayL(
            const MVPbkContactLinkArray& aArray );
    private:
        CVPbkContactEasyManager(CVPbkContactManager& aContactManager);
        CVPbkContactEasyManager();
    private: // data
        MVPbkOperationObserver* iObserver; // not owned
        RPointerArray<MVPbkStoreContact>* iContacts; // not owned
        CVPbkContactLinkArray* iContactLinks; // owned
        CVPbkContactManager& iContactManager;
        MVPbkContactOperationBase* iOperation; // owned
        TInt iNextContactToLock;
        TBool iCancelled;
        
        MVPbkOperationErrorObserver* iErrorObserver;
    };

#endif /*CVPBKCONTACTEASYMANAGER_H_*/
