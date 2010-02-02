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
* Description:  Phonebook 2 contact name construction policy.
*
*/


#ifndef CPBK2CONTACTNAMECONSTRUCTIONPOLICY_H
#define CPBK2CONTACTNAMECONSTRUCTIONPOLICY_H

// INCLUDES
#include <e32base.h>
#include <CVPbkContactNameConstructionPolicy.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
class CVPbkFieldTypeRefsList;

// CLASS DECLARATION

/**
 * Phonebook 2 contact name construction policy.
 * Responsible for implementing Virtual Phonebook defined name
 * formatting policy so that a name gets formatted as specified
 * in UI level specifications.
 */
class CPbk2ContactNameConstructionPolicy :
            public CVPbkContactNameConstructionPolicy
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aParam    Virtual Phonebook name construction policy.
         * @return  A new instance of this class.
         */
        static CPbk2ContactNameConstructionPolicy* NewL(
                TParam* aParam );

        /**
         * Destructor.
         */
        ~CPbk2ContactNameConstructionPolicy();

    private: // From CVPbkContactNameConstructionPolicy
        MVPbkBaseContactFieldIterator* NameConstructionFieldsLC(
                const MVPbkBaseContactFieldCollection& aFieldCollection,
                CVPbkFieldTypeRefsList& aFieldTypeRefsList );

    private: // Implementation
        CPbk2ContactNameConstructionPolicy(
                const MVPbkFieldTypeList& aMasterFieldTypeList );
        void ConstructL();

    private: // Data
        /// Ref: Master field type list
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Own: Sort order
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: Name formatter
        MPbk2ContactNameFormatter* iNameFormatter;
    };

#endif // CPBK2CONTACTNAMECONSTRUCTIONPOLICY_H

// End of File
