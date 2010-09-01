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
* Description:  Top Contact opeartion.
*
*/


#ifndef VPBKTOPCONTACTOPERATION_H
#define VPBKTOPCONTACTOPERATION_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkContactSelector.h>
#include <MVPbkOperationObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkStoreContact.h>


// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactLinkArray;
class CVPbkContactLinkArray;
class CVPbkContactManager;
class MVPbkContactViewBase;
class CVPbkSortOrder;
class MVPbkContactStore;
class CVPbkContactEasyManager;
class MVPbkBaseContact;
class MVPbkContactFieldTextData;
class TVPbkContactStoreUriPtr;
class CVPbkSortOrderAcquirer;

// CLASS DECLARATION

/**
 * Top contact manager operation. One class used for all operations, a new instance 
 * is created for each client request.
 */
NONSHARABLE_CLASS( CVPbkTopContactOperation ) : public CActive,
    public MVPbkContactOperationBase,
    public MVPbkContactViewObserver,
    public MVPbkContactStoreObserver,
    public MVPbkBatchOperationObserver,
    public MVPbkOperationObserver,
    public MVPbkOperationErrorObserver,
    public MVPbkContactSelector
    {
    public:
        /**
         * Top contacts operation
         */
        enum TTopOperation
            {
            EGetTopContacts,
            EGetTopContactLinks,
            EGetNonTopContacts,
            EGetNonTopContactLinks, 
            EAddToTop,
            ERemoveFromTop,
            EReorderTop
            };
        
    public:
        /**
         * Creates a top contact manager operation that returns a view. Used to 
         * create top and non-top contact views.
         * 
         * @param aContactManager Contact manager which already has its stores opened.
         * @param aObserver Receives view when it's ready
         * @param aErrorObserver Error observer is notified in case of an error
         * @param aOperation Valid values EGetTopContacts and EGetNonTopContacts
         * 
         * @return Contact operation handle 
         */
        static MVPbkContactOperationBase* NewGetViewOperationL(
                CVPbkContactManager& aContactManager,
                MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver,
                TTopOperation aOperation );

        /**
         * Creates a top contact manager operation that returns a contact link array. Used to 
         * receive top and non-top contact links.
         * 
         * @param aContactManager Contact manager which already has its stores opened.
         * @param aObserver Receives contact link array when it's ready
         * @param aErrorObserver Error observer is notified in case of an error
         * @param aOperation Valid values EGetTopContactLinks and EGetNonTopContactLinks 
         *
         * @return Contact operation handle  
         */
        static MVPbkContactOperationBase* NewGetViewLinksOperationL(
                CVPbkContactManager& aContactManager,
                MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver,
                TTopOperation aOperation );

        /**
         * Creates a top contact manager operation that manipulates top contacts:
         * add to top contacts, remove from top contacts, reorder top contacts.
         * 
         * @param aContactManager Contact manager which already has its stores opened.
         * @param aObserver Receives notification when operation is complete
         * @param aErrorObserver Error observer is notified in case of an error
         * @param aOperation valid values EAddToTop,
         *                                ERemoveFromTop,
         *                                EReorderTop                                
         * @param aTopView Top contact view which already has been constructed. 
         *                 If a NULL value is passed, own top contact view is 
         *                 constructed.
         *                
         * @return Contact operation handle 
         */
        static MVPbkContactOperationBase* NewTopOperationL(
                CVPbkContactManager& aContactManager,
                const MVPbkContactLinkArray& aContactLinks,
                MVPbkOperationObserver& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver,
                TTopOperation aOperation,
                MVPbkContactViewBase* aViewRef = NULL
                );
          
        
        /**
         * Destructor. Cancels any ongoing requests.
         */ 
        ~CVPbkTopContactOperation();
        
        /**
         * Return contact's top index or KErrNotFound
         * 
         * @param aContact contact item 
         * 
         * @return top index or KErrNotFound
         */
        static TInt TopOrder( const MVPbkBaseContact& aContact );

    private: //From CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );
        
    private: // from MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView, 
                TInt aError, 
                TBool aErrorNotified );
    
    private: // from MVPbkContactStoreObserver
        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable(MVPbkContactStore& aContactStore, 
            TInt aReason);
        void HandleStoreEventL(
            MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent);

    private: // from MVPbkBatchOperationObserver
        void StepComplete( 
            MVPbkContactOperationBase& aOperation,
            TInt aStepSize );
        TBool StepFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aStepSize, TInt aError );
        void OperationComplete( 
            MVPbkContactOperationBase& aOperation );

    private: // from MVPbkOperationObserver
        void VPbkOperationCompleted( MVPbkContactOperationBase* aOperation );
        void VPbkOperationFailed( MVPbkContactOperationBase* aOperation,
                TInt aError );

    private: // From MVPbkContactSelector
        TBool IsContactIncluded( const MVPbkBaseContact& aContact );    

    private: // construction        
        static CVPbkTopContactOperation* NewLC(
                CVPbkContactManager& aContactManager,
                MVPbkOperationErrorObserver& aErrorObserver,
                TTopOperation aOperation);
        void ConstructL();
        CVPbkTopContactOperation(
                CVPbkContactManager& aContactManager,
                MVPbkOperationErrorObserver& aErrorObserver,
                TTopOperation aOperation);
        
    private: // new methods
        static void SetTopOrderL(
            MVPbkContactFieldTextData& aTextData, TInt aTopOrderIndex );
        void SetTopOrderL(
            MVPbkStoreContact& aContact, TInt aTopOrderIndex );
        static const MVPbkContactFieldTextData* FindTopFieldTextData(
            const MVPbkBaseContact& aContact );
        static MVPbkContactFieldTextData* FindTopFieldTextData(
            MVPbkStoreContact& aContact );  
        void OpenStoreL( const TVPbkContactStoreUriPtr& aUri );
        void OpenDefaultStoreL();
        
        void RequestTopContactsViewL();
        void RequestNonTopContactsViewL();      
        
        void Top_RunL();
        void GetView_RunL();
        void DoTopOperationL();
        
        TBool SaveOneContactL();

        static TInt NextTopOrderIndexL(
            const MVPbkContactViewBase& aView );

        void CompleteOurself();

        static CVPbkContactLinkArray* CloneArrayL(
            const MVPbkContactLinkArray& aArray );

        static MVPbkContactLinkArray* ViewToLinkArrayL(
            const MVPbkContactViewBase& aView );
        
        void SetTopOrderToContactsL(
            RPointerArray<MVPbkStoreContact>& aContacts,
            TInt aHighestOrderIndex );

        void RemoveTopnessFromContacts(
            RPointerArray<MVPbkStoreContact>& aContacts );

        void RemoveTopnessFromContact( MVPbkStoreContact& aContact );
        
        void ReorderContactsL(
            RPointerArray<MVPbkStoreContact>& aContacts );
        
        const MVPbkFieldType& TopContactFieldTypeL();
        
        void NotifyResultL();
        void NotifyError(TInt aErr);
        void AbortWithError(TInt aErr);
        
        CVPbkSortOrderAcquirer* AllContactsSortOrderL() const;
        
    private:        
        enum TTopManagementState
            {
            EStateNone,
            EStateOpenStore,
            EStateCreateView,
            EStateRetrieveLock,
            EStateModifyCommit,
            EStateAbortWithError
            };
        
    private: // data
        const TTopOperation iCurrentOperation;
        TTopManagementState iNextState;

        CVPbkContactManager& iContactManager; // not owned
        MVPbkContactViewBase* iView; // owned
        //  A handle to externally provided top contact view
        MVPbkContactViewBase* iViewRef; // Not owned.
        
        MVPbkContactStore* iContactStore; // not owned
        CVPbkContactLinkArray* iInputLinks; // owned
        RPointerArray<MVPbkStoreContact> iContacts; // owns the contacts
        MVPbkContactOperationBase* iContactOperation; // own
        CVPbkContactEasyManager* iContactEasyManager; //own
        
        // observers, not owned
        MVPbkOperationObserver* iObserver;
        MVPbkOperationResultObserver<MVPbkContactViewBase*>* iViewObserver;
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>* iLinksObserver;
        MVPbkOperationErrorObserver* iErrorObserver;
        TInt iAbortError;
    };

#endif //VPBKTOPCONTACTOPERATION_H
//End of file
