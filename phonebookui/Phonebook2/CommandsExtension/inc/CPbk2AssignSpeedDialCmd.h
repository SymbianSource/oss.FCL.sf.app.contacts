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
* Description:  Phonebook 2 assign speed dial command object.
*
*/


#ifndef CPBK2ASSIGNSPEEDDIALCMD_H
#define CPBK2ASSIGNSPEEDDIALCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ContactRelocatorObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MPbk2CommandObserver;
class CVPbkContactIdConverter;
class MVPbkStoreContact;
class CPbk2ContactRelocator;
class MVPbkStoreContactField;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 assign speed dial command object.
 * Responsible for assigning a speed dial to a contact.
 */
NONSHARABLE_CLASS(CPbk2AssignSpeedDialCmd) :
        public CBase,
        public MPbk2Command,
        public MVPbkContactObserver,
        public MVPbkSingleContactOperationObserver,
        public MPbk2ContactRelocatorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2AssignSpeedDialCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2AssignSpeedDialCmd();

    public:  // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    public: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    public: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

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
        CPbk2AssignSpeedDialCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void FinishCommand(
                TInt aError );
        TBool RelocateContactL();
        void AssignSpeedDialL();
        MVPbkStoreContactField* FocusedFieldLC(
                MVPbkStoreContact& aStoreContact );

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Contact id converter
        CVPbkContactIdConverter* iIdConverter;
        /// Own: Store contact to assign a speed dial
        MVPbkStoreContact* iStoreContact;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iOperation;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2ASSIGNSPEEDDIALCMD_H

// End of File
