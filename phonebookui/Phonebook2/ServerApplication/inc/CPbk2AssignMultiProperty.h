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
*              : for multi contact assign.
*
*/


#ifndef CPBK2ASSIGNMULTIPROPERTY_H
#define CPBK2ASSIGNMULTIPROPERTY_H

// INCLUDES
#include "CPbk2SelectFieldPropertyBase.h"

// FORWARD DECLARATIONS
class MVPbkContactLinkArray;
class MVPbkFieldTypeList;

// CLASS DECLARATION

/**
 * Phonebook 2 contact field property selector for multi contact assign.
 * Responsible for validating created add item wrappers.
 */
class CPbk2AssignMultiProperty : public CPbk2SelectFieldPropertyBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aSelector         Externalized field type selector buffer.
         * @param aMarkedEntries    Marked contacts.
         * @return  A new instance of this class.
         */
        static CPbk2AssignMultiProperty* NewL(
                HBufC8& aSelector,
                MVPbkContactLinkArray& aMarkedEntries );

        /**
         * Destructor.
         */
        ~CPbk2AssignMultiProperty();

    public: // From MPbk2AssignSelectFieldProperty
        void PrepareL();
        TInt ExecuteL();
        TInt SelectedFieldIndex() const;

    private: // Implementation
        CPbk2AssignMultiProperty(
                HBufC8& aSelector,
                TInt aResourceId,
                MVPbkContactLinkArray& aMarkedEntries );
        void ConstructL();
        TBool AreAllContactsFromSameStore(
                MVPbkContactLinkArray& aMarkedEntries );
        void CreateWrappersFromCntDbL();

    private: // Data
        /// Ref: Marked contacts
        MVPbkContactLinkArray& iMarkedEntries;
    };

#endif // CPBK2ASSIGNMULTIPROPERTY_H

// End of File
