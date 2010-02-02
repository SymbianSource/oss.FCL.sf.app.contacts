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
* Description:  Phonebook 2 speed dial attribute unassigner.
*
*/


#ifndef CPBK2CONTACTSPEEDDIALATTRIBUTEUNASSIGNER_H
#define CPBK2CONTACTSPEEDDIALATTRIBUTEUNASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"
#include <Pbk2ServerAppIPC.h>
#include <MVPbkContactAttributeManager.h>

// FORWARD DECLARATIONS
class MPbk2ContactAssignerObserver;
class CVPbkContactManager;
class MVPbkContactAttribute;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 speed dial attribute unassigner.
 */
class CPbk2ContactSpeedDialAttributeUnassigner : public CActive,
                                                 public MPbk2ContactAssigner,
                                                 public MVPbkSetAttributeObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aContactManager       Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ContactSpeedDialAttributeUnassigner* NewL(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ContactSpeedDialAttributeUnassigner();

    public: // From MPbk2ContactAssigner
        void AssignDataL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                const MVPbkFieldType* aFieldType,
                const HBufC* aDataBuffer );
        void AssignAttributeL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                TPbk2AttributeAssignData aAttributeAssignData );

    private: // From MVPbkSetAttributeObserver
        void AttributeOperationComplete(
                MVPbkContactOperationBase& aOperation);
        void AttributeOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Implementation
        CPbk2ContactSpeedDialAttributeUnassigner(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager  );
        void UnassignAttributeL();
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Attribute data to assign
        TPbk2AttributeAssignData iAttributeAssignData;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Attribute unassign operation
        MVPbkContactOperationBase* iUnassignOperation;
        /// Own: The attribute to operate with
        MVPbkContactAttribute* iAttribute;
        /// Own: Store contact field
        MVPbkStoreContactField* iStoreContactField;
    };

#endif // CPBK2CONTACTSPEEDDIALATTRIBUTEUNASSIGNER_H

// End of File
