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
* Description:  Phonebook 2 contact field type selector for
*              : single contact assign.
*
*/


#ifndef CPBK2ASSIGNSINGLEPROPERTY_H
#define CPBK2ASSIGNSINGLEPROPERTY_H

// INCLUDES
#include "CPbk2SelectFieldPropertyBase.h"

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 contact field type selector for single contact assign.
 * Responsible for:
 * - validating created add item wrappers
 * - returning selected field index
 */
class CPbk2AssignSingleProperty : public CPbk2SelectFieldPropertyBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aSelector         Externalized field type selector buffer.
         * @param aStoreContact     Store contact.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2AssignSingleProperty* NewL(
                HBufC8& aSelector,
                MVPbkStoreContact& aStoreContact,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2AssignSingleProperty();

    public: // From MPbk2AssignSelectFieldProperty
        void PrepareL();
        TInt ExecuteL();
        TInt SelectedFieldIndex() const;

    private: // Implementation
        CPbk2AssignSingleProperty(
                HBufC8& aSelector,
                TInt aResourceId,
                MVPbkStoreContact& aStoreContact,
                CVPbkContactManager& aContactManager );
        void ConstructL();

    private: // Data
        /// Own: Index of the desired field if it exists in the contact
        TInt iFieldIndex;
        /// Ref: Store contact
        MVPbkStoreContact& iStoreContact;
        /// Own: Error code returned to client after ExecuteL is finished
        TInt iResult;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

#endif // CPBK2ASSIGNSINGLEPROPERTY_H

// End of File
