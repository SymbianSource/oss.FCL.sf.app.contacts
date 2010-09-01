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
* Description:  Phonebook 2 send message command.
*
*/


#ifndef CPBK2SENDMESSAGECMD_H
#define CPBK2SENDMESSAGECMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <TSendingCapabilities.h>
#include <VPbkFieldType.hrh>
#include <RVPbkContactFieldDefaultPriorities.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactOperationBase;
class MPbk2CommandObserver;
class MVPbkStoreContactField;
class MVPbkContactLink;
class CPbk2FieldPropertyArray;
class MPbk2ContactUiControl;
class MVPbkContactLinkArray;
class CMessageData;
class CPbk2AttachmentFile;
class CPbk2ApplicationServices;
class CSendUi;

// CLASS DECLARATION

/**
 * Phonebook 2 send message command.
 * Responsible for:
 * - displaying select message type query
 * - performing address select to the contact
 * - utilizing SendUI to send the message
 * - sending all messages ( sms, mms, email... )
 * - sending post card by creating, filling and finally destroying
 *   the post card file read by post card application
 */
NONSHARABLE_CLASS(CPbk2SendMessageCmd) :
        public CActive,
        public MPbk2Command,
        public MVPbkSingleContactOperationObserver
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2SendMessageCmd* NewL (
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2SendMessageCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);
        
    public:
    	void SetMtmUid( TUid aUid );
    
    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2SendMessageCmd (
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        TUid ShowWriteQueryL();
        void RetrieveContactL(
                const MVPbkContactLink& aContactLink );
        void DoSendMessageL();
        void ResolveUrlL();
        TInt PrepareAddressSelectL(
                RVPbkContactFieldDefaultPriorities& aPriorities,
                TUid aTechType );
        void SetAddressFromFieldL(
                MVPbkStoreContactField& aField );
        void ProcessDismissed(
                TInt aCancelCode );
        TInt FilterErrors(
                TInt aErrorCode );
        void IssueRequest();
        void IssueStopRequest(
                TInt aErrorCode );
        TInt ToStoreFieldIndexL(
                TInt aPresIndex );
        MVPbkStoreContactField* SelectFieldL(
                TUint aResourceId,
                RVPbkContactFieldDefaultPriorities& aPriorities );
        void SendPostCardL();

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EShowWriteQuery,
            EResolveURL,
            EStarting,
            ELaunching,
            EStopping
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Own: Selected contacts
        MVPbkContactLinkArray* iSelectedContacts;
        /// Own: MTM filter
        CArrayFixFlat<TUid>* iMtmFilter;
        /// Own: Send MTM uid
        TUid iMtmUid;
        /// Own: Sending capabilities
        TSendingCapabilities iSendingCapabilities;
        /// Own: The contact to send message to
        MVPbkStoreContact* iStoreContact;
        /// Own: The retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: The field property array
        CPbk2FieldPropertyArray* iFieldPropertyArray;
        /// Ref: Focused field
        const MVPbkStoreContactField* iFocusedField;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Message data object
        CMessageData* iMessageData;
        /// Own: Current index of a contact in array
        TInt iCurrentContactLinkIndex;
        /// Own: The count of valid recipients
        TInt iRecipientCount;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Postcard temporary attachment file
        CPbk2AttachmentFile* iAttachmentFile;
        /// Own: Initially focused field index
        TInt iFocusedFieldIndex;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
        /// Own: Send UI
        CSendUi* iSendUi;
    };

#endif // CPBK2SENDMESSAGECMD_H

// End of File
