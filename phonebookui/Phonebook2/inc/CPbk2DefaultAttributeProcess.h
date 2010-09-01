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
* Description:  Phonebook 2 default attribute modification processor.
*
*/


#ifndef CPBK2DEFAULTATTRIBUTEPROCESS_H
#define CPBK2DEFAULTATTRIBUTEPROCESS_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactAttributeManager.h>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkContactOperationBase;
class MPbk2DefaultAttributeProcessObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 default attribute modification processor.
 * Responsible for:
 * - wrapping Virtual Phonebook attribute manager for
 *   default attribute set and removal
 * - removing the previously set default attribute from the contact,
 *   when updating existing default attribute to another field
 */
NONSHARABLE_CLASS(CPbk2DefaultAttributeProcess) :
            public CBase,
            private MVPbkSetAttributeObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aManager      Virtual Phonebook contact manager.
         * @param aContact      Contact to operate with.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2DefaultAttributeProcess* NewL(
                CVPbkContactManager& aManager,
                MVPbkStoreContact& aContact,
                MPbk2DefaultAttributeProcessObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2DefaultAttributeProcess();

    public: // Interface

        /**
         * Sets given default attribute to given contact field.
         * Also removes the given default attribute from the
         * contact fields they were previously assigned to.
         *
         * @param aDefaultType          Attribute to set.
         * @param aField                Contact field to set
         *                              the attribute to.
         */
        IMPORT_C void SetDefaultL(
                TVPbkDefaultType aDefaultType,
                MVPbkStoreContactField& aField );

        /**
         * Removes the given default attribute from given contact field.
         *
         * @param aDefaultType          Attribute to remove.
         */
        IMPORT_C void RemoveDefaultL(
                TVPbkDefaultType aDefaultType );

        /**
         * Sets given default attributes to given contact field.
         * Also removes given default attributes from the
         * contact fields they were previously assigned to.
         *
         * @param aDefaultProperties    Attributes to set.
         *                              Ownership of the array is taken.
         * @param aField                Contact field to set
         *                              the attributes to.
         */
        IMPORT_C void SetDefaultsL(
                CArrayFixFlat<TVPbkDefaultType>* aDefaultProperties,
                MVPbkStoreContactField& aField );

        /**
         * Removes given default attributes from given contact field.
         *
         * @param aDefaultProperties    Attributes to remove.
         *                              Ownership of the array is taken.
         */
        IMPORT_C void RemoveDefaultsL(
                CArrayFixFlat<TVPbkDefaultType>* aDefaultProperties );

    private: // From MVPbkSetAttributeObserver
        void AttributeOperationComplete(
                MVPbkContactOperationBase& aOperation );
        void AttributeOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2DefaultAttributeProcess(
                CVPbkContactManager& aManager,
                MVPbkStoreContact& aContact,
                MPbk2DefaultAttributeProcessObserver& aObserver );
        MVPbkStoreContactField* FindContactFieldWithAttributeL(
                TVPbkDefaultType aDefaultType );
        void DoSetL(
                TVPbkDefaultType aDefaultType );
        TBool DoRemoveL(
                TVPbkDefaultType aDefaultType );
        TBool DoRemovePreviousL(
                TVPbkDefaultType aDefaultType );
        void SetNextL();
        void RemoveNextL();
        TVPbkDefaultType NextAttribute();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iManager;
        /// Ref: Contact
        MVPbkStoreContact& iContact;
        /// Ref: Attribute observer
        MPbk2DefaultAttributeProcessObserver& iObserver;
        /// Own: Set attribute operation
        MVPbkContactOperationBase* iSetAttributeOperation;
        /// Own: Remove attribute operation
        MVPbkContactOperationBase* iRemoveAttributeOperation;
        /// Own: Remove previous attribute operation
        MVPbkContactOperationBase* iRemovePreviousAttributeOperation;
        /// Ref: Field the attribute is set to
        MVPbkStoreContactField* iSetAttributeField;
        /// Own: Field containing the default attribute to be removed
        MVPbkStoreContactField* iRemoveAttributeField;
        /// Own: Array of default attributes to operate with
        CArrayFixFlat<TVPbkDefaultType>* iDefaultAttributes;
        /// Own: Attribute type
        TVPbkDefaultType iAttributeType;
    };

#endif // CPBK2DEFAULTATTRIBUTEPROCESS_H

// End of File
