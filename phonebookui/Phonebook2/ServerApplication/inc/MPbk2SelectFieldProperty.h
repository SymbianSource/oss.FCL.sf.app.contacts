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
* Description:  Phonebook 2 field property selector interface.
*
*/


#ifndef MPBK2SELECTFIELDPROPERTY_H
#define MPBK2SELECTFIELDPROPERTY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkFieldType;

// CLASS DECLARATION

/**
 * Phonebook 2 field property selector interface.
 */
class MPbk2SelectFieldProperty
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2SelectFieldProperty()
            {};

        /**
         * Prepares the selector.
         */
        virtual void PrepareL() = 0;

        /**
         * Executes the selector.
         *
         * @return  Error code given by field property selector.
         *          These error codes are custom coded:
         *          - KErrNone if no errors.
         *          - KErrAlreadyExists if the contact contains one
         *            field of the given field type and the maximum
         *            number of contact fields of that type is one.
         *          - KErrNotSupported if the contact does not support
         *            given field type.
         */
        virtual TInt ExecuteL() = 0;

        /**
         * Cancels the selection service.
         *
         * @param aCommandId    Cancel command id.
         */
        virtual void Cancel(
                TInt aCommandId ) = 0;

        /**
         * Returns selected field type.
         *
         * @return  User selected field type.
         */
        virtual const MVPbkFieldType* SelectedFieldType() const = 0;

        /**
         * Returns selected field index.
         * This is usable only when updating existing contact with a
         * detail that has a multiplicity of one and that detail already
         * exists in the contact. In that case this function returns
         * the store index of that field. In other cases
         * returns KErrNotSupported.
         *
         * @return  Store index of selected field.
         */
        virtual TInt SelectedFieldIndex() const = 0;
    };

#endif // MPBK2SELECTFIELDPROPERTY_H

// End of File
