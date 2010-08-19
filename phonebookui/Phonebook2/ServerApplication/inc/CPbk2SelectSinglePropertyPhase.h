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
* Description:  Phonebook 2 server app assign select single property phase.
*
*/


#ifndef CPBK2SELECTSINGLEPROPERTYPHASE_H
#define CPBK2SELECTSINGLEPROPERTYPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ExitCallback.h>
#include "MPbk2ServicePhase.h"
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MPbk2SelectFieldProperty;
class MPbk2DialogEliminator;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 server app assign select single property phase.
 * Responsible for selecting single assign property.
 */
class CPbk2SelectSinglePropertyPhase :
            public CBase,
            public MPbk2ServicePhase,
            private MVPbkSingleContactOperationObserver,
            private MPbk2ExitCallback,
            private MVPbkContactObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contacts to operate with.
         * @param aFilterBuffer         Address select filter buffer.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @return  A new instance of this class.
         */
        static CPbk2SelectSinglePropertyPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                HBufC8* aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult );

        /**
         * Destructor.
         */
        ~CPbk2SelectSinglePropertyPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        void DenyDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        TInt ExtraResultData() const;
        MVPbkStoreContact* TakeStoreContact();
        HBufC* FieldContent() const;

    public: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    public: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // Implementation
        CPbk2SelectSinglePropertyPhase(
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult );
        void ConstructL(
                MVPbkContactLinkArray* aContactLinks );
        void RetrieveContactL();
        void SelectFieldL();

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: Select field dialog eliminator
        MPbk2DialogEliminator* iSelectFieldDialogEliminator;
        /// Ref: Address select filter buffer
        HBufC8* iFilterBuffer;
        /// Ref: Select field property
        MPbk2SelectFieldProperty*& iSelectFieldProperty;
        /// Ref: Selected field index
        TInt& iResult;
    };

#endif // CPBK2SELECTSINGLEPROPERTYPHASE_H

// End of File
