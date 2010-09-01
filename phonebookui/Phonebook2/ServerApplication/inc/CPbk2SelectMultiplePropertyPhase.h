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
* Description:  Phonebook 2 server app assign select multiple
*              : property phase.
*
*/


#ifndef CPBK2SELECTMULTIPLEPROPERTYPHASE_H
#define CPBK2SELECTMULTIPLEPROPERTYPHASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ExitCallback.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MPbk2SelectFieldProperty;
class MPbk2DialogEliminator;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 server app assign select multiple property phase.
 * Responsible for selecting multi assign property.
 */
class CPbk2SelectMultiplePropertyPhase :
            public CActive,
            public MPbk2ServicePhase,
            private MPbk2ExitCallback
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contacts to operate with.
         * @param aFilterBuffer         Address select filter buffer.
         * @param aSelectFieldProperty  Select field property.
         * @return  A new instance of this class.
         */
        static CPbk2SelectMultiplePropertyPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                HBufC8& aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty );

        /**
         * Destructor.
         */
        ~CPbk2SelectMultiplePropertyPhase();

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

    private: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // Implementation
        CPbk2SelectMultiplePropertyPhase(
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8& aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty );
        void ConstructL(
                MVPbkContactLinkArray* aContactLinks );
        void SelectFieldL();
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Ref: Select field dialog eliminator
        MPbk2DialogEliminator* iSelectFieldDialogEliminator;
        /// Ref: Address select filter buffer
        HBufC8& iFilterBuffer;
        /// Ref: Select field property
        MPbk2SelectFieldProperty*& iSelectFieldProperty;
    };

#endif // CPBK2SELECTMULTIPLEPROPERTYPHASE_H

// End of File
