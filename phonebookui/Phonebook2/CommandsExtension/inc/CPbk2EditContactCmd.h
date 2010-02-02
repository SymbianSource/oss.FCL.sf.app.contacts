/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command for editing a contact
*
*/


#ifndef CPBK2EDITCONTACTCMD_H
#define CPBK2EDITCONTACTCMD_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MPbk2EditedContactObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactOperationBase;
class MVPbkBaseContact;
class MPbk2ContactUiControl;
class MVPbkContactLink;
class MVPbkStoreContactField;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 *  A command for editing a contact.
 */
NONSHARABLE_CLASS(CPbk2EditContactCmd) : 
        public CBase,
        public MPbk2Command,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactObserver,
        public MPbk2EditedContactObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *
         * @param aParams The command parameters, if link array is null
         *                then the control is used the get contacts.
         *
         * @return A new instance of this class.
         */
        static CPbk2EditContactCmd* NewL(MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        virtual ~CPbk2EditContactCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);
        
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError);

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
            TContactOpResult aResult);
        void ContactOperationFailed(
            TContactOp aOpCode,
            TInt aErrorCode,
            TBool aErrorNotified);

    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
            MVPbkStoreContact* aEditedContact);
        void ContactEditingDeletedContact(
            MVPbkStoreContact* aEditedContact);
        void ContactEditingAborted();
            
    private: // Implementation
        CPbk2EditContactCmd(
            MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void ExecuteEditorL();
        MVPbkStoreContactField* FocusedFieldLC
                ( MVPbkStoreContact& aStoreContact );
        
    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Link to contact to edit
        MVPbkContactLink* iContactLink;
        /// Own: An operation to get a contact from the link
        MVPbkContactOperationBase* iContactRetriever;
        /// Own: The store contact to edit
        MVPbkStoreContact* iContact;
        /// Own: The flag indicating if the editing has been aborted
        TBool iIsEditingAborted;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif  // CPBK2EDITCONTACTCMD_H
            
// End of File
