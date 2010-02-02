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
* Description:  Phonebook 2 go to URL command object.
*
*/


#ifndef CPBK2GOTOURLCMD_H
#define CPBK2GOTOURLCMD_H

// INCLUDES
#include <e32base.h>
#include "MPbk2Command.h"
#include "MPbk2ProcessDecorator.h"
#include <MVPbkSingleContactOperationObserver.h>
#include <AknServerApp.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactLink;
class MVPbkStoreContact;
class CBrowserLauncher;
class MVPbkStoreContactField;
class CPbk2FieldPropertyArray;
class MPbk2ContactUiControl;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 go to URL command object.
 */
NONSHARABLE_CLASS(CPbk2GoToURLCmd) :
        public CActive,
        public MPbk2Command,
        public MPbk2ProcessDecoratorObserver,
        public MVPbkSingleContactOperationObserver,
        public MAknServerAppExitObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPbk2GoToURLCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2GoToURLCmd();

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(
                TInt aError );

    public:  // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MAknServerAppExitObserver
        void HandleServerAppExit(
                TInt aReason );

    private: // Implementation
        CPbk2GoToURLCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void RetrieveContactL(
                const MVPbkContactLink& aContactLink );
        void DoLaunchBrowserL();
        void ResolveUrlL();
        void SetURLFromFieldL(
                MVPbkStoreContactField& aField );
        void IssueRequest();
        void IssueStopRequest();

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EResolveURL,
            EStarting,
            ELaunching,
            EStopping
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Contact link
        MVPbkContactLink* iContactLink;
        /// Own: Browser launcher
        CBrowserLauncher* iBrowserLauncher;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: URL
        HBufC* iURL;
        /// Ref: Phonebook 2 field properties
        CPbk2FieldPropertyArray* iFieldPropertyArray;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2GOTOURLCMD_H

// End of File
