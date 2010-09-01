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
* Description:  Phonebook 2 MMC UI extension copy from MMC command.
*
*/


#ifndef CPMUCOPYFROMMMCCMD_H
#define CPMUCOPYFROMMMCCMD_H

// INCLUDES
#include <e32base.h>
#include <s32file.h>
#include <MPbk2Command.h>
#include <MVPbkContactCopyObserver.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactLinkArray;
class MVPbkContactStore;
class CVPbkVCardEng;
class MVPbkContactOperationBase;
class CPbk2ApplicationServices;

/**
 * Phonebook 2 MMC UI extension copy from MMC command object.
 * Responsible for copying contacts from the MMC.
 */
class CPmuCopyFromMmcCmd : public CActive,
                           public MPbk2Command,
                           private MVPbkContactCopyObserver,
                           private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPmuCopyFromMmcCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPmuCopyFromMmcCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public:  // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    private: // From MVPbkContactCopyObserver
        void FieldAddedToContact(
                MVPbkContactOperationBase& aOperation );
        void FieldAddingFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );
        void ContactsSaved(
                MVPbkContactOperationBase& aOperation,
                MVPbkContactLinkArray* aResults );
        void ContactsSavingFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // Implementation
        CPmuCopyFromMmcCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void CopyNextL();
        void IssueRequest();
        void ShowResultsL();
        TBool IsDefaultStorePhoneMemoryL() const;
        void CommandCompletedL();
        TBool ShowMemorySelectionDialogL();

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Ref: Target store
        MVPbkContactStore* iTargetStore;
        /// Own: Decorator for copying process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: VCard engine
        CVPbkVCardEng* iVCardEngine;
        /// Own: Current asynchronous import operation
        MVPbkContactOperationBase* iImportOperation;
        /// Own: Currently copied contact
        TInt iCurrentContactIndex;
        /// Own: Count of copied contacts
        TInt iCountOfContacts;
        /// Own: Directory to import from
        CDir* iDir;
        /// Own: Stream to read from
        RFileReadStream iReadStream;
        /// Own: Command's state
        TInt iState;
        /// Own: Path to contacts in memory card
        TFileName iContactsPath;
        /// Own: A handle to app services singleton
        CPbk2ApplicationServices* iAppServices;
        /// Own: ETrue if import must check duplicates.
        TBool iCheckDuplicates;

    };

#endif // CPMUCOPYFROMMMCCMD_H

// End of File
