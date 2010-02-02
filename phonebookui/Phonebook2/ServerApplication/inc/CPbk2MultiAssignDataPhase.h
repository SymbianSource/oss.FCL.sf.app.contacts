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
* Description:  Phonebook 2 server app contact data assign phase.
*
*/


#ifndef CPBK2MULTIASSIGNDATAPHASE_H
#define CPBK2MULTIASSIGNDATAPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MPbk2KeyEventHandler.h>
#include "MPbk2ServicePhase.h"
#include "MPbk2ContactAssignerObserver.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MPbk2SelectFieldProperty;
class MPbk2ContactAssigner;
class MVPbkStoreContactField;
class CPbk2KeyEventDealer;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app contact data assign phase.
 * Responsible for assigning given data to given contacts.
 */
class CPbk2MultiAssignDataPhase : public CBase,
                                  public MPbk2ServicePhase,
                                  private MPbk2ContactAssignerObserver,
                                  private MVPbkContactObserver,
                                  private MVPbkSingleContactOperationObserver,
                                  private MVPbkBatchOperationObserver,
                                  private MPbk2KeyEventHandler
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contacts to operate with.
         * @param aSelectFieldProperty  Selected field property.
         * @param aDataBuffer           Textual data to assign.
         * @param aMimeType             Mime type of the data to assign.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2MultiAssignDataPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                MPbk2SelectFieldProperty* aSelectedFieldProperty,
                HBufC* aDataBuffer,
                TInt aMimeType,
                TUint aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2MultiAssignDataPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        TInt ExtraResultData() const;
        MVPbkStoreContact* TakeStoreContact();
        HBufC* FieldContent() const;

    private: // From MPbk2ContactAssignerObserver
        void AssignComplete(
                MPbk2ContactAssigner& aAssigner,
                TInt aIndex );
        void AssignFailed(
                MPbk2ContactAssigner& aAssigner,
                TInt aErrorCode );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

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

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2MultiAssignDataPhase(
                MPbk2ServicePhaseObserver& aObserver,
                MPbk2SelectFieldProperty* aSelectedFieldProperty,
                HBufC* aDataBuffer,
                TInt aMimeType,
                TUint aNoteFlags );
        void ConstructL(
                MVPbkContactLinkArray* aContactLinks );
        void AssignDataL();
        TBool SelectContactFieldL(
                MVPbkStoreContactField*& aField );
        void AppendResultL(
                MVPbkStoreContact* aStoreContact );
        void DisplayNotesL();
        void AppendResultAndContinueL(
                MVPbkStoreContact* aContact );
        void RetrieveContactL();
        void CommitContactsL();
        void ContinueL();
        void Finalize();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contacts to operate with
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Results
        CVPbkContactLinkArray* iResults;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Array of store contacts
        CArrayPtrFlat<MVPbkStoreContact>* iStoreContacts;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Commit operation
        MVPbkContactOperationBase* iCommitOperation;
        /// Own: Contact data assigner
        MPbk2ContactAssigner* iContactDataAssigner;
        /// Ref: Select field property
        MPbk2SelectFieldProperty* iSelectedFieldProperty;
        /// Own: Textual data to assign
        HBufC* iDataBuffer;
        /// Own: Mime type
        TInt iMimeType;
        /// Own: Note flags
        TUint iNoteFlags;
        /// Own: Contacts processed
        TInt iContactsProcessed;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Own: Indicates cancellation
        TBool iOperationCancelled;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikenv;
    };

#endif // CPBK2MULTIASSIGNDATAPHASE_H

// End of File
