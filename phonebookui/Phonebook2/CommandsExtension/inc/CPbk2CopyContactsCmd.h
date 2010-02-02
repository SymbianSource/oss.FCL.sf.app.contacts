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
* Description:  Phonebook 2 copy contacts command.
*
*/


#ifndef CPBK2COPYCONTACTSCMD_H
#define CPBK2COPYCONTACTSCMD_H

//  INCLUDES
#include <MPbk2Command.h>
#include <MPbk2ProcessDecorator.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbk2DriveSpaceCheck;
class MPbk2ProcessDecorator;
class MPbk2ContactUiControl;
class CVPbkContactStoreUriArray;
class CVPbkContactLinkArray;
class CVPbkContactCopier;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class MVPbkContactStore;
class MVPbkContactBookmark;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 copy contacts command.
 */
NONSHARABLE_CLASS(CPbk2CopyContactsCmd) :
        public CActive,
        public MPbk2Command,
        private MVPbkBatchOperationObserver,
        private MPbk2ProcessDecoratorObserver,
        private MVPbkSingleContactOperationObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2CopyContactsCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        virtual ~CPbk2CopyContactsCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From MVPbkBatchOperationObserver
        void StepComplete(
                MVPbkContactOperationBase& aOperation,
                TInt aStepSize );
        TBool StepFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aStepSize,
                TInt aError );
        void OperationComplete(
                MVPbkContactOperationBase& aOperation );

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

    private: // Implementation
        CPbk2CopyContactsCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void IssueRequest();
        void InitL();
        void RetrieveL();
        void StartCopyL();
        void ShowResultsL();
        void FinishCommandL();
        void CreateLinkArrayL(
                MVPbkContactLinkArray& aSelection );
        void ShowSelectedContactsInfoNoteL();
        void HandleError( TInt aError );

    private: // Data
        /// Ref: Store control
        MPbk2ContactUiControl* iStoreUiControl;
        /// Ref: UI control
        MPbk2ContactUiControl* iNameListUiControl;
        /// Ref: Observer for the completion of the command
        MPbk2CommandObserver* iCommandObserver;
        /// Ref: The target store for the contacts
        MVPbkContactStore* iTargetStore;
        /// Own: Selected contacts
        CVPbkContactLinkArray* iContactLinks;
        /// Own: An operation for retrieving the contact
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Contact when copying one contact
        MVPbkStoreContact* iContact;
        /// Own: Virtual Phonebook copy operation
        MVPbkContactOperationBase* iCopyOperation;
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Indicator for succesfully copied contacts
        TInt iCopiedSuccessfully;
        /// Own: The internal state of the command
        TInt iState;
        /// Own: The virtual phonebook contact copier
        CVPbkContactCopier* iCopier;
        /// Own: An array for copy results
        CVPbkContactLinkArray* iCopiedContacts;
        /// Own: Flash File System space checker
        CPbk2DriveSpaceCheck* iDriveSpaceCheck;
        /// Own: The focus must kept during the copy
        MVPbkContactBookmark* iFocusedContactBookmark;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif  // CPBK2COPYCONTACTSCMD_H

// End of File
