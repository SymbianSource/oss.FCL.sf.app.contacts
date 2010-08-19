/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server app assign update
*              : empty property phase.
*
*/


#ifndef CPBK2SELECTUPDATEEMPTYPROPERTYPHASE_H
#define CPBK2SELECTUPDATEEMPTYPROPERTYPHASE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ServicePhase.h"
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MPbk2ServicePhaseObserver;
class MPbk2SelectFieldProperty;
class MVPbkStoreContact;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class CVPbkContactLinkArray;


// CLASS DECLARATION

/**
 * Phonebook 2 server app assign update empty property phase.
 * Responsible for implementing the select property service
 * phase in no data / empty data cases for entry update service.
 */
class CPbk2SelectUpdateEmptyPropertyPhase : public CBase,
                                   public MPbk2ServicePhase,
                                   private MVPbkSingleContactOperationObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactLinks         Contact links.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2SelectUpdateEmptyPropertyPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray& aContactLinks,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2SelectUpdateEmptyPropertyPhase();

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
        MVPbkStoreContact* TakeStoreContact();
        TInt ExtraResultData() const; 
        HBufC* FieldContent() const;

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2SelectUpdateEmptyPropertyPhase(
                MPbk2ServicePhaseObserver& aObserver,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );
        void ConstructL(
        		MVPbkContactLinkArray& aContactLinks );
        void RetrieveContactL();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref: Select field property
        MPbk2SelectFieldProperty*& iSelectFieldProperty;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Selected field index
        TInt& iResult;
    };

#endif // CPBK2SELECTUPDATEEMPTYPROPERTYPHASE_H

// End of File
