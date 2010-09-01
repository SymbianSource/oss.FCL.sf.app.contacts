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
* Description:  Phonebook 2 MMC UI extension copy to MMC command.
*
*/


#ifndef CPMUCOPYTOMMCCMD_H
#define CPMUCOPYTOMMCCMD_H

// INCLUDES
#include <e32base.h>
#include <s32file.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class CFileMan;
class MPbk2ContactUiControl;
class MVPbkContactLinkArray;
class CVPbkVCardEng;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class CPbk2DriveSpaceCheck;

/**
 * Phonebook 2 MMC UI extension copy to MMC command object.
 * Responsible for copying selected contacts into the MMC.
 */
class CPmuCopyToMmcCmd : public CActive,
                         public MPbk2Command,
                         private MPbk2ProcessDecoratorObserver,
                         private MVPbkSingleContactOperationObserver,
                         private MFileManObserver

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPmuCopyToMmcCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPmuCopyToMmcCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public: // From MPbk2Command
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
                
    private: // From MFileManObserver
        TControl NotifyFileManStarted();
	    TControl NotifyFileManOperation();

    private: // Implementation
        CPmuCopyToMmcCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void CopyNextL();
        void IssueRequest();
        HBufC* CreateFileNameLC(
                const MVPbkStoreContact& aContact ) const;
        void DeleteOldEntriesL();
        void ShowConfirmationQueryL();
        TBool ShowMemorySelectionDialogL();
        void ShowResultsL();
        TInt HandleError( TInt aError );

    private:  // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command's state
        TInt iState;
        /// Ref: Delete old entries
        TBool iDeleteOldEntries;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: File manager for deleting the old vCards
        CFileMan* iFileMan;
        /// Own: Decorator for copying process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Copied contacts
        MVPbkContactLinkArray* iContactLinks;
        /// Own: VCard engine
        CVPbkVCardEng* iVCardEngine;
        /// Own: Current asynchronous retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Current asynchronous export operation
        MVPbkContactOperationBase* iExportOperation;
        /// Own: Retrieved store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Previously copied contact
        MVPbkStoreContact* iPreviousContact;
        /// Own: Currently copied contact
        TInt iCurrentIndex;
        /// Own: Count of copied contacts
        TInt iCountOfContacts;
        /// Own: Write stream
        RFileWriteStream iWriteStream;
        /// Own: Path to contacts on memory card
        TFileName iContactsPath;
        /// Own: Memory checker
        CPbk2DriveSpaceCheck* iDriveSpaceCheck;
        
    };

#endif // CPMUCOPYTOMMCCMD_H

// End of File
