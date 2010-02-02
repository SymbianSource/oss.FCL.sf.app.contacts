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


#ifndef CPBK2SINGLEASSIGNDATAPHASE_H
#define CPBK2SINGLEASSIGNDATAPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include <MPbk2EditedContactObserver.h>
#include <MPbk2ExitCallback.h>
#include "MPbk2ServicePhase.h"
#include "MPbk2ContactAssignerObserver.h"
#include <coehelp.h>

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MPbk2SelectFieldProperty;
class MPbk2ContactAssigner;
class MPbk2DialogEliminator;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app contact data assign phase.
 * Responsible for assigning given data to given contacts.
 */
class CPbk2SingleAssignDataPhase : public CBase,
                                   public MPbk2ServicePhase,
                                   private MPbk2ContactAssignerObserver,
                                   private MVPbkContactObserver,
                                   private MPbk2EditedContactObserver,
                                   private MPbk2ExitCallback
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aStoreContact         Contact to operate with,
         *                              ownership is taken.
         * @param aSelectFieldProperty  Selected field property.
         * @param aSelectedField        Selected field index.
         * @param aDataBuffer           Textual data to assign.
         * @param aMimeType             Mime type of the data to assign.
         * @param aHelpContext          Editor help context.
         * @param aAssignFlags          Assign flags.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2SingleAssignDataPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkStoreContact* aStoreContact,
                MPbk2SelectFieldProperty* aSelectedFieldProperty,
                TInt aSelectedFieldIndex,
                HBufC* aDataBuffer,
                TInt aMimeType,
                TCoeHelpContext aHelpContext,
                TUint aAssignFlags,
                TUint aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2SingleAssignDataPhase();

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

    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingDeletedContact(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingAborted();

    public: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // Implementation
        CPbk2SingleAssignDataPhase(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkStoreContact* aStoreContact,
                MPbk2SelectFieldProperty* aSelectedFieldProperty,
                TInt aSelectedFieldIndex,
                HBufC* aDataBuffer,
                TInt aMimeType,
                TCoeHelpContext aHelpContext,
                TUint aAssignFlags,
                TUint aNoteFlags );
        void RetrieveContactL();
        void DoAssignDataL();
        void HandleContactDataAssignedL(
                TInt aIndex );
        void AppendResultL(
                const MVPbkStoreContact* aStoreContact );
        void DisplayNotesL();
        TBool IsContactInSelectedMemoryL(
                MVPbkStoreContact& aContact );
        void FinalizeL(
                const MVPbkStoreContact* aStoreContact );

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Results
        CVPbkContactLinkArray* iResults;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Contact data assigner
        MPbk2ContactAssigner* iContactDataAssigner;
        /// Own: Selected field index
        TInt iSelectedFieldIndex;
        /// Ref: Select field property
        MPbk2SelectFieldProperty* iSelectedFieldProperty;
        /// Own: Textual data to assign
        HBufC* iDataBuffer;
        /// Own: Mime type
        TInt iMimeType;
        /// Ref: Contact editor eliminator
        MPbk2DialogEliminator* iContactEditorEliminator;
        /// Own: Editor help context
        TCoeHelpContext iHelpContext;
        /// Own: Assign flags set by the consumer
        TUint iAssignFlags;
        /// Own: Note flags
        TUint iNoteFlags;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2SINGLEASSIGNDATAPHASE_H

// End of File
