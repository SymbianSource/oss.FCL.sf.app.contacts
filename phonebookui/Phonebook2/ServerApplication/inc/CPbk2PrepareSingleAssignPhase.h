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


#ifndef CPBK2PREPARESINGLEASSIGNPHASE_H
#define CPBK2PREPARESINGLEASSIGNPHASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2KeyEventHandler.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MPbk2SelectFieldProperty;
class MVPbkStoreContact;
class CPbk2KeyEventDealer;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app prepare single assign phase.
 * Responsible for preparing contact for assign.
 */
class CPbk2PrepareSingleAssignPhase :
                public CActive,
                public MPbk2ServicePhase,
                private MPbk2ContactRelocatorObserver,
                private MPbk2KeyEventHandler
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aStoreContact         Contact to operate with,
         *                              ownership is taken.
         * @param aSelectFieldProperty  Select field property.
         * @param aSelectFieldResult    Result from the select field dialog.
         * @param aFieldIndex           Index of the field to assign data to.
         * @param aNoteFlags            Information note flags.
         * @param aRelocationFlags      Relocation flags.
         * @return  A new instance of this class.
         */
        static CPbk2PrepareSingleAssignPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkStoreContact* aStoreContact,
                MPbk2SelectFieldProperty* aSelectFieldProperty,
                TInt aSelectFieldResult,
                TInt& aFieldIndex,
                TUint& aNoteFlags,
                TUint aRelocationFlags );

        /**
         * Destructor.
         */
        ~CPbk2PrepareSingleAssignPhase();

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

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2PrepareSingleAssignPhase(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkStoreContact* aStoreContact,
                MPbk2SelectFieldProperty* aSelectFieldProperty,
                TInt aSelectFieldResult,
                TInt& aFieldIndex,
                TUint& aNoteFlags,
                TUint aRelocationFlags );
        void ConstructL();
        TBool RelocateContactL(
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aPolicy,
                TBool& aThisDestroyed );
        TInt QueryForDataReplaceL();
        TInt QueryForRelocationL();
        void PrepareForAssignL();
        TBool IsContactFromPhoneMemoryL(
                const MVPbkStoreContact& aStoreContact );
        void AppendResultL(
                const MVPbkStoreContact* aStoreContact );
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Select field dialog result
        TInt iSelectFieldResult;
        /// Ref: Select field property
        MPbk2SelectFieldProperty* iSelectFieldProperty;
        /// Ref: Index of the field to assign data to
        TInt& iFieldIndex;
        /// Ref: Information note flags
        TUint& iNoteFlags;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Indicates is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Relocation flags
        TUint iRelocationFlags;
        /// Own: Results
        CVPbkContactLinkArray* iResults;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2PREPARESINGLEASSIGNPHASE_H

// End of File
