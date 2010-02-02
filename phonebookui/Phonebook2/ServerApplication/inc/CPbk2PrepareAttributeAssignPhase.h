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
* Description:  Phonebook 2 server app prepare attribute assign phase.
*
*/


#ifndef CPBK2PREPAREATTRIBUTEASSIGNPHASE_H
#define CPBK2PREPAREATTRIBUTEASSIGNPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2KeyEventHandler.h>
#include <Pbk2ServerAppIPC.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class CPbk2KeyEventDealer;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app prepare attribute assign phase.
 * Responsible for preparing contact for assign.
 */
class CPbk2PrepareAttributeAssignPhase :
                public CBase,
                public MPbk2ServicePhase,
                private MVPbkSingleContactOperationObserver,
                private MPbk2ContactRelocatorObserver,
                private MPbk2KeyEventHandler
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contacts to operate with.
         * @param aAttributeData        Attribute data.
         * @return  A new instance of this class.
         */
        static CPbk2PrepareAttributeAssignPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRemoveAttribute );

        /**
         * Destructor.
         */
        ~CPbk2PrepareAttributeAssignPhase();

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

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
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
        CPbk2PrepareAttributeAssignPhase(
                MPbk2ServicePhaseObserver& aObserver,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRemoveAttribute );
        void ConstructL(
                MVPbkContactLinkArray& aContactLinks );
        void RetrieveContactL();
        void PrepareAssignL();
        TBool ValidateContactL();
        TBool ContactMeetsPreconditionsL();
        TBool ContactNeedsRelocation();
        void RelocateContactL();
        void AppendResultL(
                const MVPbkStoreContact* aStoreContact );
        void AppendResultL(
                const MVPbkContactLink& aContactLink );

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Own: Attribute data
        TPbk2AttributeAssignData iAttributeData;
        /// Own: Results array
        CVPbkContactLinkArray* iResults;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Indicates is this a attribute removal operation
        TBool iRemoveAttribute;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikenv;
    };

#endif // CPBK2PREPAREATTRIBUTEASSIGNPHASE_H

// End of File
