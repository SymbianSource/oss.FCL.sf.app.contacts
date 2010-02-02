/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 belongs to groups command.
*
*/


#ifndef CPGUBELONGSTOGROUPSCMD_H
#define CPGUBELONGSTOGROUPSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactSelector.h>
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactGroup;
class MVPbkContactStore;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Contacts belong to group command.
 */
class CPguBelongsToGroupsCmd :
        public CActive,
        public MPbk2Command,
        private MVPbkSingleContactOperationObserver,
        private MVPbkContactSelector
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPguBelongsToGroupsCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguBelongsToGroupsCmd();

    public:  // From MPbk2Command
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

    private: // From MVPbkContactSelector
        TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );

    private: // Data types

        /// Process states
        enum TProcessState
            {
            ELaunchPopup,
            EFinishCommand
            };

    private: // Implementation
        CPguBelongsToGroupsCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void LaunchGroupPopupL();
        void IssueRequest(
                TProcessState aState );

    private:  // Data
        /// Ref: Ui control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Groups that the iStoreContact belongs to
        MVPbkContactLinkArray* iGroupsJoined;
        /// Own: current state of process
        TProcessState iState;
    };

#endif // CPGUBELONGSTOGROUPSCMD_H

// End of File
