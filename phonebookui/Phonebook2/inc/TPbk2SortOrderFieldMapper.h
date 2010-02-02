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
* Description:  Phonebook 2 sort order field mapper.
*
*/


#ifndef TPBK2SORTORDERFIELDMAPPER_H
#define TPBK2SORTORDERFIELDMAPPER_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkBaseContactFieldCollection;
class MVPbkBaseContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 sort order field mapper.
 * Maps fields in a contact to the same order than in the sort order.
 */
class TPbk2SortOrderFieldMapper
    {
    public: // Construction

        /**
         * Constructor.
         */
        IMPORT_C TPbk2SortOrderFieldMapper();

    public: // Interface

        /**
         * Sets the current sort order that the fields are mapped against.
         *
         * @param aSortOrder    New sort order.
         */
        IMPORT_C void SetSortOrder(
                const MVPbkFieldTypeList& aSortOrder );

        /**
         * Sets the contact fields that are mapped.
         *
         * @param aContactFields    Fields to be mapped to sort order.
         */
        IMPORT_C void SetContactFields(
                const MVPbkBaseContactFieldCollection& aContactFields );

        /**
         * Returns the number of mapped fields.
         *
         * @return  Number of mapped fields.
         */
        IMPORT_C TInt FieldCount() const;

        /**
         * Returns a field in a contact that is of the same type than
         * the field at given index in the sort order.
         *
         * @param aIndex    Index of the field type in the sort order.
         * @return  Field that is of the same type than the field type
         *          at given index in the sort order.
         */
        IMPORT_C const MVPbkBaseContactField* FieldAt(
                TInt aIndex ) const;

    private: // Data
        /// Ref: Current sort order
        const MVPbkFieldTypeList* iSortOrder;
        /// Ref: Fields that are mapped to sort order
        const MVPbkBaseContactFieldCollection* iContactFields;
    };

#endif // TPBK2SORTORDERFIELDMAPPER_H

// End of File
