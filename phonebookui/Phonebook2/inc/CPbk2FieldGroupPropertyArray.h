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
* Description:  Phonebook 2 field group properties.
*
*/


#ifndef CPBK2FIELDGROUPPROPERTYARRAY_H
#define CPBK2FIELDGROUPPROPERTYARRAY_H

#include <e32base.h>

class MPbk2FieldPropertyArray;
class CPbk2FieldPropertyGroup;
class MPbk2FieldProperty;
class TResourceReader;

/**
 * Phonebook 2 field group properties.
 */
class CPbk2FieldGroupPropertyArray : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperties  Field properties used for populating
         *                          the group.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldGroupPropertyArray* NewL(
                const MPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperties  Field properties used for populating
         *                          the group.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldGroupPropertyArray* NewLC(
            const MPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Destructor.
         */
        virtual ~CPbk2FieldGroupPropertyArray();

    public: // Interface

        /**
         * Returns the number of group properties in the array
         *
         * @return  Number of group properties in the array.
         */
        IMPORT_C TInt GroupCount() const;

        /**
         * Returns the field group property at given index.
         *
         * @param aIndex    The index of the property.
         * @return  Group property.
         */
        IMPORT_C const CPbk2FieldPropertyGroup& GroupAt(
                TInt aIndex ) const;

        /**
         * Finds the group into which the given property belongs.
         * Returns NULL if the property does not belong to the group.
         *
         * @param aProperty     The property to search for.
         * @return  Group of the property or NULL.
         */
        IMPORT_C const CPbk2FieldPropertyGroup* FindGroupForProperty(
                const MPbk2FieldProperty& aProperty ) const;

    private: // Implementation
        CPbk2FieldGroupPropertyArray();
        void ConstructL(
                const MPbk2FieldPropertyArray& aFieldProperties );
        void ReadFieldGroupsL(
                TResourceReader& aReader );
        void PopulateGroupsL(
                const MPbk2FieldPropertyArray& aFieldProperties );

    private: // Data
        /// Own: Field groups
        RPointerArray<CPbk2FieldPropertyGroup> iFieldGroups;
    };

#endif // CPBK2FIELDGROUPPROPERTYARRAY_H

// End of File
