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
* Description:  Phonebook 2 field property group.
*
*/


#ifndef CPBK2FIELDPROPERTYGROUP_H
#define CPBK2FIELDPROPERTYGROUP_H

// INCLUDES
#include <e32base.h>
#include <TPbk2IconId.h>
#include <Pbk2FieldProperty.hrh>

// FORWARD DECLARATIONS
class TResourceReader;
class MPbk2FieldProperty;

// CLASS DECLARATION

/**
 * Phonebook 2 field property group.
 * Field property group for Phonebook 2 field types.
 */
class CPbk2FieldPropertyGroup : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   Resource reader pointed to a
         *                  PHONEBOOK2_FIELD_GROUP resource.
         * @return  A new instance of this class.
         */
        static CPbk2FieldPropertyGroup* NewLC(
                TResourceReader& aReader );

        /**
         * Destructor.
         */
        virtual ~CPbk2FieldPropertyGroup();

    public: // Interface

        /**
         * Returns the number of field properties in the group.
         *
         * @return  Number of field properties in the group.
         */
        IMPORT_C TInt Count() const;

        /**
         * Returns the field property at given index.
         *
         * @param aIndex    The index of the property.
         * @return  Field property in given index.
         */
        IMPORT_C const MPbk2FieldProperty& At(
                TInt aIndex ) const;

        /**
         * Returns the id of this group.
         *
         * @return  Id of the group.
         */
        inline TPbk2FieldGroupId GroupId() const;

        /**
         * Returns the label of this group.
         *
         * @return  The label of this group.
         */
        inline const TDesC& Label() const;

        /**
         * Returns the icon id of this group.
         *
         * @return  Icon id of this group.
         */
        inline const TPbk2IconId& IconId() const;

        /**
         * Returns the add item ordering of this group.
         *
         * @return  Add item ordering of this group.
         */
        inline TInt AddItemOrdering() const;

        /**
         * A private API function to add properties to the group.
         *
         * @param aProperty     A new property to add.
         */
        void AddFieldPropertyL(
                const MPbk2FieldProperty& aProperty );

    private: // Implementation
        CPbk2FieldPropertyGroup();
        void ConstructL(
                TResourceReader& aReader );

    private: // Data
        /// Own: The id of the group
        TPbk2FieldGroupId iGroupId;
        /// Own: Label of this group
        HBufC* iLabel;
        /// Own: Icon Id of this group
        TPbk2IconId iIconId;
        /// Own: Add item ordering of this group
        TInt iAddItemOrdering;
        /// Own: Does not own the instances
        RPointerArray<const MPbk2FieldProperty> iProperties;
    };


// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::GroupId
// --------------------------------------------------------------------------
//
inline TPbk2FieldGroupId CPbk2FieldPropertyGroup::GroupId() const
    {
    return iGroupId;
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::Label
// --------------------------------------------------------------------------
//
inline const TDesC& CPbk2FieldPropertyGroup::Label() const
    {
    return *iLabel;
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::IconId
// --------------------------------------------------------------------------
//
inline const TPbk2IconId& CPbk2FieldPropertyGroup::IconId() const
    {
    return iIconId;
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::AddItemOrdering
// --------------------------------------------------------------------------
//
inline TInt CPbk2FieldPropertyGroup::AddItemOrdering() const
    {
    return iAddItemOrdering;
    }


#endif // CPBK2FIELDPROPERTYGROUP_H

// End of File
