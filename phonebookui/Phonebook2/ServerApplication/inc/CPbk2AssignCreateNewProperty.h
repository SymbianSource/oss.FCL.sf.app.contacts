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
* Description:  Phonebook 2 contact field property selector
*              : for new contact creation.
*
*/


#ifndef CPBK2ASSIGNCREATENEWPROPERTY_H
#define CPBK2ASSIGNCREATENEWPROPERTY_H

// INCLUDES
#include "CPbk2SelectFieldPropertyBase.h"

// FORWARD DECLARATIONS
class MVPbkContactStore;
class CPbk2StorePropertyArray;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 contact field property selector for new contact creation.
 * Responsible for:
 * - validating created add item wrappers
 * - changing target store if selected store does not handle the field type
 */
class CPbk2AssignCreateNewProperty : public CPbk2SelectFieldPropertyBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aSelector         Externalized field type selector buffer.
         * @param aContactStore     Selected saving store.
         * @param aStoreProperties  Store properties
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2AssignCreateNewProperty* NewL(
                HBufC8& aSelector,
                MVPbkContactStore*& aContactStore,
                const CPbk2StorePropertyArray& aStoreProperties,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2AssignCreateNewProperty();

    public: // From MPbk2AssignSelectFieldProperty
        void PrepareL();
        TInt ExecuteL();
        TInt SelectedFieldIndex() const;

    private: // Implementation
        CPbk2AssignCreateNewProperty(
                HBufC8& aSelector,
                TInt aResourceId,
                MVPbkContactStore*& aContactStore,
                const CPbk2StorePropertyArray& aStoreProperties,
                CVPbkContactManager& aContactManager );
        void ConstructL();

    private: // Data
        /// Ref: Saving store
        MVPbkContactStore*& iContactStore;
        /// Ref: Phonebook store properties
        const CPbk2StorePropertyArray& iStoreProperties;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

#endif // CPBK2ASSIGNCREATENEWPROPERTY_H

// End of File
