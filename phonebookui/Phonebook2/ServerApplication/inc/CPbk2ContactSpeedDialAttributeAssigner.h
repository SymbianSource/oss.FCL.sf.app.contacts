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
* Description:  Phonebook 2 speed dial attribute assigner.
*
*/


#ifndef CPBK2CONTACTSPEEDDIALATTRIBUTEASSIGNER_H
#define CPBK2CONTACTSPEEDDIALATTRIBUTEASSIGNER_H

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
 * Phonebook 2 speed dial attribute assigner.
 */
class CPbk2ContactSpeedDialAttributeAssigner : public CActive,
                                               public MPbk2ContactAssigner,
                                               private MVPbkSetAttributeObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aAttributeAssignData  Attribute assing data.
         * @param aContactManager       Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ContactSpeedDialAttributeAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ContactSpeedDialAttributeAssigner();

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
        CPbk2ContactSpeedDialAttributeAssigner(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager );
        void AssignAttributeL();
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Attribute data to assign
        TPbk2AttributeAssignData iAttributeAssignData;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Attribute assign operation
        MVPbkContactOperationBase* iAssignOperation;
        /// Own: The attribute to operate with
        MVPbkContactAttribute* iAttribute;
        /// Own: Selected store contact field
        MVPbkStoreContactField* iStoreContactField;
    };

#endif // CPBK2CONTACTSPEEDDIALATTRIBUTEASSIGNER_H

// End of File
