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
* Description:  xSP Contact operation.
*
*/


#ifndef VPBKXSPCONTACTSOPERATION_H
#define VPBKXSPCONTACTSOPERATION_H

// INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <badesca.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <MVPbkOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactLinkArray;
class CVPbkContactLinkArray;
class CVPbkContactManager;
class MVPbkBaseContact;
class MVPbkStoreContact;
class MVPbkContactStore;
class CVPbkFieldTypeRefsList;

// CLASS DECLARATION

/**
 * Top contact manager operation. One class used for all operations, a new instance 
 * is created for each client request.
 */
NONSHARABLE_CLASS( CVPbkxSPContactsOperation ) : public CActive,
    public MVPbkContactOperationBase,
    public MVPbkContactFindFromStoresObserver
    {
    public:
        /**
         * xSP contacts operation
         */
        enum TxSPOperation
            {
            EGetxSPContactLinks
            };
        
    public:
        /**
         * Creates a xSP contact manager operation that returns a array of xsp contact links. 
         * Used to fetch xSP contact links for contact.
         * 
         * @param aContactManager Contact manager which already has its stores opened.
         * @param aObserver Receives array when it's ready
         * @param aErrorObserver Error observer is notified in case of an error
         * @param aOperation Valid values EGetxSPContactLinks
         * 
         * @return Contact operation handle 
         */
        static MVPbkContactOperationBase* NewGetxSPLinksOperationL(
                const MVPbkStoreContact& aContact,
                CVPbkContactManager& aContactManager,
                MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver,
                TxSPOperation aOperation );
        
        /**
         * Destructor. Cancels any ongoing requests.
         */ 
        ~CVPbkxSPContactsOperation();
        
    private: //From CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );
        
    
    private: //From MVPbkContactFindFromStoresObserver
        void FindFromStoreSucceededL( MVPbkContactStore& aStore, 
            MVPbkContactLinkArray* aResultsFromStore );
        void FindFromStoreFailed( 
                MVPbkContactStore& aStore, TInt aError );
        void FindFromStoresOperationComplete();
                
    private: // construction        
        static CVPbkxSPContactsOperation* NewLC(
                const MVPbkStoreContact& aContact,
                CVPbkContactManager& aContactManager,
                MVPbkOperationErrorObserver& aErrorObserver,
                TxSPOperation aOperation);
        void ConstructL();
        CVPbkxSPContactsOperation(
                const MVPbkStoreContact& aContact,
                CVPbkContactManager& aContactManager,
                MVPbkOperationErrorObserver& aErrorObserver,
                TxSPOperation aOperation);
        
    private: // new methods
        void GetIMPPDataL( );
        TBool FindUsingIdL();        
        static TInt WordParserL( TAny* aWordParserParam );        
        void ProcessContactLinksL( MVPbkContactLinkArray* aResultsFromStore );        
        void CompleteOurself();
        void NotifyResult();        
        void SubsearchCompleteL();
        void NotifyError(TInt aErr);
        void AbortWithError(TInt aErr);
                
    private:        
        enum TxSPManagementState
            {
            EStateNone,
            EStateFind,
            EStateAbortWithError
            };
        
    private: // data
        TxSPManagementState iNextState;
        const MVPbkStoreContact& iContact; // not owned
        MVPbkContactLink* iContactLink;  //own
        CVPbkContactManager& iContactManager; // not owned
        CVPbkFieldTypeRefsList*  iFieldTypeRefList; // owned
        CVPbkContactLinkArray* iContactLinks; // owned
        MVPbkContactOperationBase* iContactOperation; // own
        CDesCArrayFlat* iXspIdArray; // own        
        
        // observers, not owned
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>* iLinksObserver;
        MVPbkOperationErrorObserver* iErrorObserver;
        TInt iAbortError;
        TInt iNextXspId;
    };

#endif //VPBKXSPCONTACTSOPERATION_H

//End of file
