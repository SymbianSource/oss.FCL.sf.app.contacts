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
* Description:  Phonebook 2 create new contact command object.
*
*/


#ifndef CPBK2CREATENEWCONTACTCMD_H
#define CPBK2CREATENEWCONTACTCMD_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2EditedContactObserver.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactStore;
class MVPbkStoreContact;
class MVPbkBaseContactField;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 create new contact command object.
 * Responsible for:
 * - verifying saving store availability and fullness
 * - displaying information notes  if store is not available or it is full
 * - creating new contact to the store
 * - adding possible find text to the created contact's last name field
 * - verifying synchronization field existence
 * - launching contact editor for the new contact
 * - updating UI control after the editor is exited
 * - notifying command observer when the command is finished
 */
NONSHARABLE_CLASS(CPbk2CreateNewContactCmd) :
            public CActive,
            public MPbk2Command,
            public MPbk2EditedContactObserver,
            public MVPbkContactStoreObserver

    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPbk2CreateNewContactCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        virtual ~CPbk2CreateNewContactCmd();
        
    private: // From CActive
        void RunL();        
        void DoCancel();
        TInt RunError( TInt aError );
        void HandleError( TInt aError );
        
    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingDeletedContact(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingAborted();
        
    private: // From MVPbkContactStoreObserver
        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( 
                MVPbkContactStore& aContactStore, 
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation
        CPbk2CreateNewContactCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();        
        TInt AddFindTextL(
                MVPbkStoreContact& aContact );
        void ShowStoreFullNoteL(
                const MVPbkContactStore& aTargetStore );
        void HandleContactEditingCompleteL(
                MVPbkStoreContact* aEditedContact );
        TInt ShowCreateNewToPhoneQueryL() const;
        void LoadContactStoreL();
        void OpenContactStoreL();
        void ValidateStoreL();
        void CreateNewContactL();
        void IssueRequest();
        
    private: // Data structures
        /// Process states
        enum TProcessState
            {            
            ELoadContactStore,
            EOpenContactStore,
            EValidateStore,
            ECreateNewContact,
            EFinishCommand
            };        

    private: // Data
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Target store
        MVPbkContactStore* iTargetStore;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2CREATENEWCONTACTCMD_H

// End of File
