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
* Description:  Phonebook 2 group rename command.
*
*/


#ifndef CPGURENAMEGROUPCMD_H
#define CPGURENAMEGROUPCMD_H

// INCLUDES
#include "MPbk2Command.h"
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactGroup;
class MPbk2ContactUiControl;
class MVPbkContactStore;
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Phonebook 2 group rename command.
 */
class CPguRenameGroupCmd : public CActive,
                           public MPbk2Command,
                           private MVPbkContactObserver,
                           private MVPbkSingleContactOperationObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control
         * @return  A new instance of this class.
         */
        static CPguRenameGroupCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguRenameGroupCmd();

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

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Data structures

        /// Process states
        enum TProcessState
            {
            ELauchQuery,
            EFinishCommand
            };

    private: // Implementation
        CPguRenameGroupCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void QueryGroupNameL();
        void CleanupCommand();
        void IssueRequest(
                TProcessState aState );

    private:  // Data
        /// Ref: Ui control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Target store
        MVPbkContactStore* iTargetStore;
        /// Ref: command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Link to contact to edit
        MVPbkContactLink* iGroupLink;
        /// Own: Contact operation
        MVPbkContactOperationBase* iContactOperation;
        /// Own: Contact group
        MVPbkContactGroup* iContactGroup;
        /// Own: current state of process
        TProcessState iState;
    };

#endif // CPGURENAMEGROUPCMD_H

// End of File
