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
* Description:  Phonebook 2 presentation level contact field collection.
*
*/



#ifndef CPBK2PRESENTATIONCONTACTFIELDCOLLECTION_H
#define CPBK2PRESENTATIONCONTACTFIELDCOLLECTION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkStoreContactFieldCollection.h>

// FORWARD DECLARATIONS
class CPbk2PresentationContactField;
class MPbk2FieldPropertyArray;
class MPbk2FieldProperty;
class CPbk2FieldPropertyGroup;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 presentation level contact field collection.
 */
class CPbk2PresentationContactFieldCollection : public CBase,
                                                public MVPbkStoreContactFieldCollection
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperties  An array of field properties.
         * @param aFieldCollection  A collection of store fields.
         * @param aParentContact    The contact that owns this collection.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PresentationContactFieldCollection* NewL(
                const MPbk2FieldPropertyArray& aFieldProperties,
                MVPbkStoreContactFieldCollection& aFieldCollection,
                MVPbkStoreContact& aParentContact );

        /**
         * Destructor.
         */
        virtual ~CPbk2PresentationContactFieldCollection();

    public: // Interface

        /**
         * Returns the presentation level field.
         *
         * @param aIndex    The index of the field in the array.
         * @return  Presentation level field.
         */
        IMPORT_C CPbk2PresentationContactField& At(
                TInt aIndex ) const;

        /**
         * Checks if there is a field with given property in the collection.
         *
         * @param aProperty     The property to search for.
         * @return  The index of the field or KErrNotFound.
         */
        IMPORT_C TInt ContainsField(
                const MPbk2FieldProperty& aProperty ) const;

        /**
         * Checks if the collection contains a field from the group of
         * properties.
         *
         * @param aPropertyGroup    A group of field properties.
         * @return  ETrue if the collection has at least one field
         *          with the property in the group.
         */
        IMPORT_C TBool ContainsFieldFromGroup(
                const CPbk2FieldPropertyGroup& aPropertyGroup ) const;

        /**
         * Returns the index of the given field or
         * KErrNotFound if not found.
         *
         * @param aField    The field that is searched for.
         * @return  The index of the field in this collection.
         */
        IMPORT_C TInt FindFieldIndex(
                const MVPbkStoreContactField& aField ) const;

        /**
         * Gets the index of the field in the store contact.
         *
         * @param aFieldIndex   The index of the field in this collection.
         * @return  The index of the field in the
         *          store contact field collection.
         */
        IMPORT_C TInt StoreIndexOfField(
                TInt aFieldIndex ) const;

        /**
         * Returns the field properties.
         *
         * @return  Field properties.
         */
        inline const MPbk2FieldPropertyArray& FieldProperties() const;

        /**
         * Adds a new field mapping. The field must belong to
         * the same contact as this field collection.
         *
         * @param aField    A new store contact field for the mapping.
         * @param aName     A new store contact field property name for the mapping.
         */
        void AddFieldMappingL(const MVPbkStoreContactField& aField,
                const TDesC& aName);

        /**
         * Deletes the field mapping from the given index.
         *
         * @param aIndex    The index from where to remove mapping.
         */
        void RemoveFieldMapping(
                TInt aIndex );

        /**
         * Destroys all the field mappings.
         */
        void ResetFieldMappings();

    public: // From MVPbkStoreContactFieldCollection
        MVPbkBaseContact& ParentContact() const;
        TInt FieldCount() const;
        const MVPbkStoreContactField& FieldAt(
                TInt aIndex ) const;
        MVPbkStoreContactField& FieldAt(
                TInt aIndex );
        MVPbkStoreContactField* FieldAtLC(
                TInt aIndex ) const;
        MVPbkStoreContact& ParentStoreContact() const;
        MVPbkStoreContactField* RetrieveField(
                const MVPbkContactLink& aContactLink ) const;

    private: // Implementation
        CPbk2PresentationContactFieldCollection(
                const MPbk2FieldPropertyArray& aFieldProperties,
                MVPbkStoreContactFieldCollection& aFieldCollection,
                MVPbkStoreContact& aParentContact );
        void ConstructL();

    private: // Data
        /// Ref: An array of field properties
        const MPbk2FieldPropertyArray& iFieldProperties;
        /// Ref: A collection of store fields
        MVPbkStoreContactFieldCollection& iFieldCollection;
        /// Ref: The parent contact of the collection
        MVPbkStoreContact& iParentContact;
        /// Own: Phonebook 2 presentation contact fields
        RPointerArray<CPbk2PresentationContactField> iFields;
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldCollection::FieldProperties
// --------------------------------------------------------------------------
//
inline const MPbk2FieldPropertyArray&
        CPbk2PresentationContactFieldCollection::FieldProperties() const
    {
    return iFieldProperties;
    }

#endif // CPBK2PRESENTATIONCONTACTFIELDCOLLECTION_H

// End of File
