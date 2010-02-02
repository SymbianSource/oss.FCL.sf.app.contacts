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
* Description:  Phonebook 2 server app prepare single assign phase.
*
*/


#ifndef CPBK2PREPAREMULTIPLEASSIGNPHASE_H
#define CPBK2PREPAREMULTIPLEASSIGNPHASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <MPbk2KeyEventHandler.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <TVPbkContactStoreUriPtr.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MPbk2SelectFieldProperty;
class MPbk2ContactDataAssigner;
class MVPbkStoreContact;
class CPbk2KeyEventDealer;
class CPbk2ContactRelocator;

// CLASS DECLARATION

/**
 * Phonebook 2 server app prepare single assign phase.
 * Responsible for preparing contact for assign.
 */
class CPbk2PrepareMultipleAssignPhase :
                public CActive,
                public MPbk2ServicePhase,
                private MPbk2ContactRelocatorObserver,
                private MVPbkSingleContactOperationObserver,
                private MPbk2KeyEventHandler
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contact links to operate with.
         * @param aSelectFieldProperty  Select field property.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2PrepareMultipleAssignPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                const MVPbkContactLinkArray* aContactLinks,
                MPbk2SelectFieldProperty* aSelectFieldProperty,
                TUint& aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2PrepareMultipleAssignPhase();

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

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts );
        void RelocationProcessComplete();

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2PrepareMultipleAssignPhase(
                MPbk2ServicePhaseObserver& aObserver,
                MPbk2SelectFieldProperty* aSelectFieldProperty,
                TUint& aNoteFlags );
        void ConstructL(
                const MVPbkContactLinkArray* aContactLinks );
        void RetrieveContactL();
        void RelocateContactL(
                MVPbkStoreContact* aStoreContact );
        void RelocateContactsL();
        void PrepareForAssignL();
        void AppendResultL(
                MVPbkContactLink* aContactLink );
        void SendToRelocationL(
                MVPbkContactLink* aContactLink );
        TBool SupportsFieldL(
                TVPbkContactStoreUriPtr aStoreUri );
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact data assigner
        MPbk2ContactDataAssigner* iContactDataAssigner;
        /// Ref: Select field property
        MPbk2SelectFieldProperty* iSelectFieldProperty;
        /// Ref: Information note flags
        TUint& iNoteFlags;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Indicates is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Results
        CVPbkContactLinkArray* iResults;
        /// Own: Contact links to operate with
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Contacts to be relocated
        CVPbkContactLinkArray* iEntriesToRelocate;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Own: Retrieve contact operation
        MVPbkContactOperationBase* iRetrieveOperation;
    };

#endif // CPBK2PREPAREMULTIPLEASSIGNPHASE_H

// End of File
