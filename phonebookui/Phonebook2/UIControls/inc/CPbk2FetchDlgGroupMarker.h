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
* Description:  Phonebook 2 fetch dialog group page selection marker.
*
*/


#ifndef CPBK2FETCHDLGGROUPMARKER_H
#define CPBK2FETCHDLGGROUPMARKER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactLinkArray;
class CPbk2NamesListControl;
class MVPbkContactViewBase;
class CVPbkContactManager;
class MVPbkContactOperationBase;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog group page selection marker.
 * Responsible for checking which contact groups are selected.
 */
NONSHARABLE_CLASS(CPbk2FetchDlgGroupMarker) : public CActive,
                                 private MVPbkSingleContactOperationObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager       Virtual Phonebook contact manager.
         * @param aSelectedContacts     Currently selected contacts.
         * @param aGroupsView           Groups view.
         * @param aControl              Control presenting the groups.
         * @return  A new instance of this class.
         */
        static CPbk2FetchDlgGroupMarker* NewL(
                CVPbkContactManager& aContactManager,
                MVPbkContactLinkArray& aSelectedContacts,
                MVPbkContactViewBase& aGroupsView,
                CPbk2NamesListControl& aControl );

        /**
         * Destructor
         */
        ~CPbk2FetchDlgGroupMarker();

    public: // Interface

        /**
         * Marks selected groups.
         */
        void MarkSelectedGroupsL();

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

    private: // Implementation
        CPbk2FetchDlgGroupMarker(
                CVPbkContactManager& aContactManager,
                MVPbkContactLinkArray& aSelectedContacts,
                MVPbkContactViewBase& aGroupsView,
                CPbk2NamesListControl& aControl );
        void ConstructL();
        void RetrieveNextGroupL();
        void SelectGroupL();
        TBool SelectionContainsAllMembersL(
                MVPbkContactLinkArray& aMembers );
        void IssueRequest();

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EInspecting,
            EStopping
            };

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Currently selected contacts
        MVPbkContactLinkArray& iSelectedContacts;
        /// Ref: Groups view
        MVPbkContactViewBase& iGroupsView;
        /// Ref: Control presenting the groups
        CPbk2NamesListControl& iControl;
        /// Own: Cursor to current group in groups view
        TInt iCursor;
        /// Own: Process state
        TProcessState iState;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
    };

#endif // CPBK2FETCHDLGGROUPMARKER_H

// End of File
