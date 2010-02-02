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
* Description:  Phonebook 2 Group UI Extension send message command.
*
*/


#ifndef CPGUSENDMESSAGEGROUPCMD_H
#define CPGUSENDMESSAGEGROUPCMD_H

// INCLUDES
#include <e32base.h>
#include <TSendingCapabilities.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <RVPbkContactFieldDefaultPriorities.h>

// FORWARD DECLARATIONS
class CPbk2FieldPropertyArray;
class MPbk2ContactUiControl;
class MPbk2CommandObserver;
class MVPbkStoreContact;
class MVPbkContactOperationBase;
class MVPbkStoreContactField;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class CMessageData;
class CPguUIExtensionPlugin;

// CLASS DECLARATION

/**
 * Phonebook 2 Group UI Extension send message command.
 * Responsible for:
 * - displaying select message type query
 * - performing address select to the contacts of the selected group
 * - utilizing SendUI to send the message
 */
class CPguSendMessageGroupCmd : 
    public CActive,
    public MPbk2Command,
    private MVPbkSingleContactOperationObserver
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control
         * @return  A new instance of this class.
         */
        static CPguSendMessageGroupCmd* NewL (
                   MPbk2ContactUiControl& aUiControl, 
                   CPguUIExtensionPlugin* iPguUIExtensionPlugin );

        /**
         * Destructor.
         */
        ~CPguSendMessageGroupCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPguSendMessageGroupCmd (
            MPbk2ContactUiControl& aUiControl,
            CPguUIExtensionPlugin* iPguUIExtensionPlugin );
        void ConstructL();
        TUid ShowWriteQueryL();
        void RetrieveContactL(
                const MVPbkContactLink& aContactLink );
        void DoSendMessageL();
        void ResolveGroupL();
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
        void IssueStopRequest();

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EResolveGroup,
            EResolveURL,
            EShowWriteQuery,
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
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: Phonebook 2 field property array
        CPbk2FieldPropertyArray* iFieldPropertyArray;
        /// Ref: Focused field
        const MVPbkStoreContactField* iFocusedField;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Message data
        CMessageData* iMessageData;
        /// Own: Current index of contact in array
        TInt iCurrentContactLinkIndex;
        /// Own: Current state of process
        TProcessState iState;
        /// Ref: Group UI extension
        CPguUIExtensionPlugin* iPguUIExtensionPlugin;
    };

#endif // CPGUSENDMESSAGEGROUPCMD_H

// End of File
