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
* Description:  Phonebook 2 contact duplicate policy.
*
*/



#ifndef CPBK2CONTACTDUPLICATEPOLICY_H
#define CPBK2CONTACTDUPLICATEPOLICY_H

// INCLUDE FILES
#include <CVPbkContactDuplicatePolicy.h>

// FORWARD DECLARATIONS
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class CPbk2DuplicateContactFinder;

// CLASS DECLARATION

/**
 * Phonebook 2 contact duplicate policy.
 * Responsible for implementing Virtual Phonebook defined contact
 * duplication policy by following rules specied for contact duplication
 * in UI level specifications.
 */
NONSHARABLE_CLASS(CPbk2ContactDuplicatePolicy)
    :   public CVPbkContactDuplicatePolicy
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aParam    Virtual Phonebook duplicate policy.
         * @return  A new instance of this class.
         */
        static CPbk2ContactDuplicatePolicy* NewL(
                TParam* aParam );

        /**
         * Destructor.
         */
        ~CPbk2ContactDuplicatePolicy();

    public: // From CVPbkContactDuplicatePolicy
        MVPbkContactOperationBase* FindDuplicatesL(
                const MVPbkBaseContact& aContact,
                MVPbkContactStore& aTargetStore,
                RPointerArray<MVPbkStoreContact>& aDuplicates,
                MVPbkContactFindObserver& aObserver,
                TInt aMaxDuplicatesToFind );

    private: // Implementation
        CPbk2ContactDuplicatePolicy(
                CVPbkContactManager& aContactManager );
        void ConstructL(
                const MVPbkFieldTypeList* aFieldTypeForFind );

    private: // Data
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Name formatter
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: Sort order
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: Duplicate manager
        CPbk2DuplicateContactFinder* iDuplicateFinder;
        /// Own: Duplicate contacts
        RPointerArray<MVPbkStoreContact> iDuplicates;
    };

#endif // CPBK2CONTACTDUPLICATEPOLICY_H

// End of File
