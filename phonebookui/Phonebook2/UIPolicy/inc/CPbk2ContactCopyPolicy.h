/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact copy policy.
*
*/


#ifndef CPBK2CONTACTCOPYPOLICY_H
#define CPBK2CONTACTCOPYPOLICY_H

//  INCLUDES
#include <CVPbkContactCopyPolicy.h>

// FORWARD DECLARATIONS
namespace {
class CPbk2AddFieldOperation;
}
class CPbk2FieldPropertyArray;
class MVPbkContactStore;
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class CPbk2ContactMerge;

// CLASS DECLARATION

/**
 * Phonebook 2 contact copy policy.
 * Responsible for implementing Virtual Phonebook defined contact copy
 * policy so that contact copy process is executed as specified in
 * UI level specifications.
 */
class CPbk2ContactCopyPolicy : public CVPbkContactCopyPolicy
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aParam    Virtual Phonebook copy policy parameter.
         * @return  A new instance of this class.
         */
        static CPbk2ContactCopyPolicy* NewL(
                TParam* aParam );

        /**
         * Destructor.
         */
        ~CPbk2ContactCopyPolicy();

    public: // Interface

        /**
         * Returns field properties.
         *
         * @return  Field properties.
         */
        CPbk2FieldPropertyArray& FieldProperties() const;

    private: // From CVPbkContactCopyPolicy
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDateTime& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC8& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );
        MVPbkContactOperationBase* CopyTitleFieldsL(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MVPbkContactFieldsCopyObserver& aCopyObserver );
        MVPbkContactOperationBase* CommitContactL(
                MVPbkStoreContact* aContact,
                MVPbkContactCopyObserver& aCopyObserver );
        MVPbkContactOperationBase* CopyContactL(
                MVPbkStoreContact& aContact,
                MVPbkContactStore& aTargetStore,
                MVPbkContactCopyObserver& aCopyObserver );
        TBool SupportsContactMerge() const;
        MVPbkContactOperationBase* MergeAndSaveContactsL(
                RPointerArray<MVPbkStoreContact>& aSourceContacts,
                MVPbkStoreContact& aTarget,
                MVPbkContactCopyObserver& aCopyObserver );

    private: // Implementation
        CPbk2ContactCopyPolicy(
                CVPbkContactManager& aContactManager );
        void ConstructL();
        MPbk2ContactNameFormatter& NameFormatterL();
        CPbk2ContactMerge& ContactMergeL();

    private: // Data
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Field properties
        CPbk2FieldPropertyArray* iFieldProperties;
        /// Own: Combines data of two contacts
        CPbk2ContactMerge* iContactMerge;
        /// Own: Sort order
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: Name formatter for contact merge
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: Copy name to first name field
        TBool iCopyNameToFirstName;
    };

#endif // CPBK2CONTACTCOPYPOLICY_H

// End of File
