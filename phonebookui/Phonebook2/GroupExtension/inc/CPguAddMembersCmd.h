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
* Description:  Phonebook 2 add to group command object.
*
*/


#ifndef CPGUADDMEMBERSCMD_H
#define CPGUADDMEMBERSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2FetchDlgObserver.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MPbk2Command.h>
#include <MVPbkContactSelector.h>
#include <MPbk2ProcessDecorator.h>
#include <mpgugroupcmd.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactGroup;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactViewBase;
class CPbk2ContactRelocator;

// CLASS DECLARATION

/**
 * Phonebook 2 add to group command object.
 */
class CPguAddMembersCmd : public CActive,
                          public MPbk2Command,
                          private MVPbkSingleContactOperationObserver,
                          private MVPbkContactObserver,
                          private MPbk2FetchDlgObserver,
                          private MVPbkContactViewObserver,
                          private MVPbkContactSelector,
                          private MPbk2ContactRelocatorObserver,
                          private MPbk2ProcessDecoratorObserver,
                          public MPguGroupCmd
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aGroup        Group link.
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPguAddMembersCmd* NewLC(
                MVPbkContactLink& aGroup,
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguAddMembersCmd();
        
    public: //MPguGroupCmd
        void Abort();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // From MVPbkContactViewObserver
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

    private: // From MVPbkContactSelector
        TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );

    private: // From MPbk2FetchDlgObserver
        TPbk2FetchAcceptSelection AcceptFetchSelectionL(
                TInt aNumMarkedEntries,
                MVPbkContactLink& aLastSelection );
        void FetchCompletedL(
                MVPbkContactLinkArray* aMarkedEntries );
        void FetchCanceled();
        void FetchAborted();
        TBool FetchOkToExit();

    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts );
        void RelocationProcessComplete();

    public: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // Implementation
        CPguAddMembersCmd(
                MVPbkContactLink& aContactGroup,
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void HandleContactLockedEventL();
        void DoLaunchFetchDialogL();
        void PrepareFetchResultsL(
                MVPbkContactLinkArray* aMarkedEntries );
        void CheckContactsL();
        void BeginRelocationL();
        void RelocateContactsL();
        void RelocateContactL(
                MVPbkStoreContact* aStoreContact );
        void AddContactsToGroupL();
        void CommitTransactionL();
        void FinishCommand(
                TInt aResult );
        void ShowRelocationNoteL(
                TInt aCount );
        void ShowProgressNoteL();
        void IssueRequest();
        void IssueStopRequest();
        void SetFocusToLastAddedL();
        
    private:  // Data structures
        /// Process states
        enum TState
            {
            ERetrievingContact,
            EHandleContactLockedEvent,
            ECheckContacts,
            ERelocateContacts,
            EShowingProgressNote,
            EAddingContactsToGroup,
            ECommitingTransaction,
            EStopping
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Retrieve group operation
        MVPbkContactOperationBase* iRetrieveGroupOperation;
        /// Own: Retrieve contact operation
        MVPbkContactOperationBase* iRetrieveContactOperation;
        /// Ref: A group where to add members
        MVPbkContactLink& iGroupLink;
        /// Own: Contact group
        MVPbkContactGroup* iContactGroup;
        /// Own: A filtered view for dialog
        MVPbkContactViewBase* iFilteredView;
        /// Own: Entries to add to group
        CVPbkContactLinkArray* iEntriesToAdd;
        /// Own: Entries to relocate
        CVPbkContactLinkArray* iEntriesToRelocate;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Contacts that already belongs to the group
        MVPbkContactLinkArray* iContactsInGroup;
        /// Own: State of the command
        TState iState;
        /// Own: count of contacts already added to group 
        TInt iAddedContactsCount;
        /// Own: counter of contacts 
        TInt iContactsCounter;
        /// Own: Contact 
        MVPbkStoreContact* iContact;
        TBool iAbort;
    };

#endif // CPGUADDMEMBERSCMD_H

// End of File
