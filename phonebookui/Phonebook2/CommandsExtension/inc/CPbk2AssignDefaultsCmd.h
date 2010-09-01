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
* Description:  Phonebook 2 assign defaults command.
*
*/


#ifndef CPBK2ASSIGNDEFAULTSCMD_H
#define CPBK2ASSIGNDEFAULTSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MPbk2ContactRelocatorObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CAknProgressDialog;
class CEikProgressInfo;
class MPbk2ContactUiControl;
class MVPbkContactLink;
class MVPbkStoreContact;
class MPbk2ContactNameFormatter;
class MVPbkContactLinkArray;
class CPbk2ContactRelocator;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 assign defaults command.
 */
NONSHARABLE_CLASS(CPbk2AssignDefaultsCmd) :
        public CBase,
        public MPbk2Command,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactObserver,
        public MPbk2ContactRelocatorObserver
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2AssignDefaultsCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2AssignDefaultsCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

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

    private: // Implementation
        CPbk2AssignDefaultsCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void LockContactL();
        void LaunchAssignDialogL();
        TBool RelocateContactL();
        void ProcessFinished(
                TInt aError );

    private:  // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Own: Array of contacts
        MVPbkContactLinkArray* iContactLinkArray;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: An operation to get a contact from the link
        MVPbkContactOperationBase* iContactRetriever;
        /// Own: Retrieved contact
        MVPbkStoreContact* iContact;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2ASSIGNDEFAULTSCMD_H

// End of File
