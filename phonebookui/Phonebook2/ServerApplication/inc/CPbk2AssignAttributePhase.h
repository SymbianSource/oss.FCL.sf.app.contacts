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
* Description:  Phonebook 2 server app contact attribute assign phase.
*
*/


#ifndef CPBK2ASSIGNATTRIBUTEPHASE_H
#define CPBK2ASSIGNATTRIBUTEPHASE_H

// INCLUDES
#include <e32base.h>
#include <Pbk2ServerAppIPC.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include "MPbk2ServicePhase.h"
#include "MPbk2ContactAssignerObserver.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MPbk2ContactAssigner;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app contact attribute assign phase.
 * Responsible for assigning given attribute to given contact.
 */
class CPbk2AssignAttributePhase : public CBase,
                                  public MPbk2ServicePhase,
                                  private MPbk2ContactAssignerObserver,
                                  private MVPbkContactObserver,
                                  private MVPbkSingleContactOperationObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aStoreContact         Contact to operate with,
         *                              ownership is taken.
         * @param aAttributeData        Attribute data to assign.
         * @param aRemoveAttribute      Remove attribute indicator.
         * @return  A new instance of this class.
         */
        static CPbk2AssignAttributePhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRemoveAttribute );

        /**
         * Destructor.
         */
        ~CPbk2AssignAttributePhase();

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

    private: // From MPbk2ContactAssignerObserver
        void AssignComplete(
                MPbk2ContactAssigner& aAssigner,
                TInt aIndex );
        void AssignFailed(
                MPbk2ContactAssigner& aAssigner,
                TInt aErrorCode );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Implementation
        CPbk2AssignAttributePhase(
                MPbk2ServicePhaseObserver& aObserver,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRemoveAttribute );
        void ConstructL(
                const MVPbkContactLinkArray* aContactLinks );
        void RetrieveContactL();
        void RetrieveContactFieldL();
        void AppendResultL();
        void FinalizeL();
        void AssignAttributeL();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact attribute assigner
        MPbk2ContactAssigner* iContactAttributeAssigner;
        /// Own: Contact links to operate with
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Results
        CVPbkContactLinkArray* iResults;
        /// Own: Attribute data to assign
        TPbk2AttributeAssignData iAttributeData;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Remove attribute indicator
        TBool iRemoveAttribute;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Store contact field
        MVPbkStoreContactField* iStoreContactField;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2ASSIGNATTRIBUTEPHASE_H

// End of File
