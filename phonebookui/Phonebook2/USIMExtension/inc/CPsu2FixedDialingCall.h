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
* Description:  Phonebook 2 USIM UI Extension FDN call implementation.
*
*/


#ifndef CPSU2FIXEDDIALINGCALL_H
#define CPSU2FIXEDDIALINGCALL_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class CPbk2AppViewBase;
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkFieldType;
class MVPbkContactLink;
class MVPbkFieldTypeList;
class CPbk2ContactUiControlSubstitute;
class MPbk2CommandHandler;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension FDN call implementation.
 */
class CPsu2FixedDialingCall : 
        public CActive,
        private MVPbkSingleContactOperationObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aCommandHandler   Command handler.
         * @param aAppViewBase      Application view.
         * @return  A new instance of this class.
         */
        static CPsu2FixedDialingCall* NewL(
            CVPbkContactManager& aContactManager,
            MPbk2CommandHandler& aCommandHandler,
            CPbk2AppViewBase& aAppViewBase );

        /**
         * Destructor.
         */
        ~CPsu2FixedDialingCall();

    public: // Interface
        /**
         * Creates a call to given contact
         *
         * @param aContactLink  Link to a contact that is called to,
         *                      ownership is taken
         * @param aCommand    Command id.
         */
        void CreateCallL(
                MVPbkContactLink* aContactLink,
                TInt aCommand );

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
        CPsu2FixedDialingCall(
                CVPbkContactManager& aContactManager,
                MPbk2CommandHandler& aCommandHandler,
                CPbk2AppViewBase& aAppViewBase );
        void ConstructL();
        const TPtrC ContactTextDataL(
                MVPbkStoreContact& aContact,
                TInt aSelectorResourceId );
        const MVPbkFieldType* ContactDataTypeL(
                const MVPbkFieldTypeList& aFieldTypeList,
                TInt aSelectorResourceId );
        void IssueRequest();
        void NumberQueryL();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Command handler
        MPbk2CommandHandler& iCommandHandler;
        /// Ref: Extension view
        CPbk2AppViewBase& iAppViewBase;
        /// Own: UI control substitute
        CPbk2ContactUiControlSubstitute* iUiControlSubstitute;
        /// Own: Temporary contact for calling
        MVPbkStoreContact* iTempContact;
        /// Own: Retrieve contact operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Currently focused contact
        MVPbkContactLink* iContactLink;
        /// Own: Command Id
        TInt iCommand;
    };

#endif // CPSU2FIXEDDIALINGCALL_H

// End of File
