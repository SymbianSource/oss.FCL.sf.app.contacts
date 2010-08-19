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
* Description:  Phonebook 2 fetch results.
*
*/


#ifndef CPBK2FETCHRESULTS_H
#define CPBK2FETCHRESULTS_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkStreamable;
class CVPbkContactLinkArray;
class CVPbkContactManager;
class MVPbkContactStoreList;
class MPbk2FetchDlg;
class MPbk2FetchDlgPages;
class MPbk2FetchDlgObserver;
class MPbk2FetchResultsObserver;
class CAknInputBlock;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch results.
 * Responsible for managing fetch results
 */
NONSHARABLE_CLASS(CPbk2FetchResults) : public CBase,
                                       public MVPbkContactLinkArray,
                                       private MVPbkSingleContactOperationObserver,
                                       private MVPbkContactStoreObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aFetchDlg         Fetch dialog.
         * @param aPages            Fetch dialog pages.
         * @param aObserver         Fetch dialog observer.
         * @param aResultsObserver  Results observer.
         * @return  A new instance of this class.
         */
        static CPbk2FetchResults* NewL(
                CVPbkContactManager& aContactManager,
                MPbk2FetchDlg& aFetchDlg,
                MPbk2FetchDlgPages& aPages,
                MPbk2FetchDlgObserver& aObserver,
                MPbk2FetchResultsObserver& aResultsObserver );

        /**
         * Destructor
         */
        ~CPbk2FetchResults();

    public: // Interface

        /**
         * Appends a contact link to selection.
         *
         * @param aLink     The link to append.
         */
        void AppendL(
                const MVPbkContactLink& aLink );

        /**
         * Appends a delayed contact link to selection.
         *
         * @param aLink     The link to append.
         */
        void AppendDelayedL(
                const MVPbkContactLink& aLink );

        /**
         * Deny to Append a delayed contact link to selection.
         *
         * @param aLink     The link to deny append.
         */
        void DenyAppendDelayedL(
                const MVPbkContactLink& aLink );

        /**
         * Appends a contact link to fetch results.
         *
         * @param aLink     The link to append.
         */
        void AppendToResultsL(
                const MVPbkContactLink& aLink );

        /**
         * Removes a contact link from selection.
         *
         * @param aLink     The link to remove.
         */
        void RemoveL(
                const MVPbkContactLink& aLink );

        /**
         * Resets the array and destroys the links.
         */
        void ResetAndDestroy();

        /**
         * Wait operations to complete
         */
        void WaitOperationsCompleteL();
    public: // From MVPbkContactLinkArray
        TInt Count() const;
        const MVPbkContactLink& At(
                TInt aIndex ) const;
        TInt Find(
                const MVPbkContactLink& aLink ) const;
        HBufC8* PackLC() const;
        const MVPbkStreamable* Streamable() const;
      
    private:   
        /**
         * Fetch Results Append- and Remove-Contact operation
         * representation 
         */
        class CFRConatactOperation : public CBase
            {
            public:
                /**
                 * Contact Operation types
                 */
                enum EOperationType
                    {
                    EAppendContact = 0x0,
                    EAppendContactDelayed,
                    ERemoveContact
                    };
                
                /**
                 * Creates a new instance of this class.
                 *
                 * @param aContactLink   MVPbkContactLink.
                 * @param aType          Operation type.
                 * @return  A new instance of this class.
                 */
                static CFRConatactOperation* NewL(
                                  const MVPbkContactLink& aContactLink,
                                  const EOperationType aType );
                
                /**
                 * Destructor
                 */
                ~CFRConatactOperation();
                
                // Get Contact Link
                MVPbkContactLink* GetContactLink(){ return iContactLink; };
                
                // Get Operation Type
                EOperationType GetOperationType(){ return iType; };
                  
            private:
                CFRConatactOperation( const EOperationType aType );
                void ConstructL( const MVPbkContactLink& aContactLink );
                
            private:
                MVPbkContactLink* iContactLink;
                const EOperationType iType; 
            };
        
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation
        CPbk2FetchResults(
                CVPbkContactManager& aContactManager,
                MPbk2FetchDlg& aFetchDlg,
                MPbk2FetchDlgPages& aPages,
                MPbk2FetchDlgObserver& aObserver,
                MPbk2FetchResultsObserver& aResultsObserver );
        void ConstructL(
                CVPbkContactManager& aContactManager );
        void DoAppendContactL(
                MVPbkStoreContact& aContact,
                TBool aDelayed );
        void DoRemoveContactL(
                MVPbkStoreContact& aContact );
        void RemoveFromResultsL(
                const MVPbkContactLink& aLink );
        TInt CountContactsL(
                MVPbkStoreContact& aContact,
                MVPbkContactLink& aLink );
        void HandleContactOperationCompleteL(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void ProcessNextContactOperationL();
        void AppendContactOperationL( CFRConatactOperation* aOperation );
        CFRConatactOperation* GetCurrentContactOperation( );
        void RemoveCurrentContactOperation( );


    private: // Data
        /// Own: Fetch results
        CVPbkContactLinkArray* iSelectedContacts;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Contact stores used
        MVPbkContactStoreList* iStoreList;
        /// Ref: Fetch dialog
        MPbk2FetchDlg& iFetchDlg;
        /// Ref: Fetch dialog pages
        MPbk2FetchDlgPages& iPages;
        /// Ref: Observer
        MPbk2FetchDlgObserver& iObserver;
        /// Ref: Results observer
        MPbk2FetchResultsObserver& iResultsObserver;
        /// Own: Contact-Operations queue
        CArrayFixFlat<CFRConatactOperation*>* iOperationQueue;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Is it waiting the acceptance, initialized to EFalse
        TBool iWaitingForDelayedAppend;
        /// Own: Wait until finish contact store operation
        CActiveSchedulerWait* iWaitForAllOperationComplete;
        /// Own: User input blocker
        CAknInputBlock* iInputBlock;
    };

#endif // CPBK2FETCHRESULTS_H

// End of File
