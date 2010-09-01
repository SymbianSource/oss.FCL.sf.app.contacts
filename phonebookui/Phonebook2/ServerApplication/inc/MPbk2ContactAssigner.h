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
* Description:  Phonebook 2 contact assigner interface.
*
*/


#ifndef MPBK2CONTACTASSIGNER_H
#define MPBK2CONTACTASSIGNER_H

// INCLUDES
#include <e32std.h>
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class MVPbkFieldType;
class MVPbkStoreContact;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 contact assigner interface.
 */
class MPbk2ContactAssigner
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactAssigner()
            {}

        /**
         * Assigns given data to given contact.
         *
         * @param aStoreContact     The contact to assign data to.
         * @param aContactField     The contact field to assign data to.
         *                          If NULL, a new field is created.
         * @param aFieldType        Contact field type.
         * @param aDataBuffer       The data to assign.
         */
        virtual void AssignDataL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                const MVPbkFieldType* aFieldType,
                const HBufC* aDataBuffer ) = 0;

        /**
         * Assigns given attribute to given contact.
         *
         * @param aStoreContact         The contact to assign attribute to.
         * @param aContactField         The contact field to assign
         *                              attribute to.
         * @param aAttributeAssignData  The attribute data.
         */
        virtual void AssignAttributeL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                TPbk2AttributeAssignData aAttributeAssignData ) = 0;
    };

#endif // MPBK2CONTACTASSIGNER_H

// End of File
