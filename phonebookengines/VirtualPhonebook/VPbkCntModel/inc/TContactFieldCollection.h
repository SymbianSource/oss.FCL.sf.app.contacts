/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Maps a Contact Model field collection to a virtual Phonebook 
                 field collection.
*
*/


#ifndef VPBKCNTMODEL_TCONTACTFIELDCOLLECTION_H
#define VPBKCNTMODEL_TCONTACTFIELDCOLLECTION_H


// INCLUDES
#include <MVPbkStoreContactFieldCollection.h>
#include "TContactField.h"

// FORWARD DECLARATIONS
class CContactItemFieldSet;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContact;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model field collection to a virtual Phonebook field collection.
 */
NONSHARABLE_CLASS( TContactFieldCollection ): 
        public MVPbkStoreContactFieldCollection
    {
    public:  // Constructor and destructor
        /**
         * Constructor.
         *
         * @param aParentContact    the contact whose field collection this is.
         * @parma aFieldSet         the Contact Model field set to map.
         */
        TContactFieldCollection
            (CContact& aParentContact, CContactItemFieldSet& aFieldSet);

        /**
         * Destructor.
         */
        ~TContactFieldCollection();

    public:  // New functions
        /**
         * Resets a new parent contact for this field collection.
         *
         * @param aParentContact    the contact whose field collection this is.
         * @parma aFieldSet         the Contact Model field set to map.
         */
        void SetContact
            (CContact& aParentContact, CContactItemFieldSet& aFieldSet);

        /**
         * Returns a pointer to the field that is used as a return value of 
         * FieldAt functions.
         *
         * @see CContact::AddFieldL
         */
        const MVPbkStoreContactField* FieldPointer() const;

    public:  // from MVPbkStoreContactFieldCollection
        MVPbkBaseContact& ParentContact() const;
        TInt FieldCount() const;
        const MVPbkStoreContactField& FieldAt(TInt aIndex) const;
        MVPbkStoreContactField& FieldAt(TInt aIndex);
        MVPbkStoreContactField* FieldAtLC(TInt aIndex) const;
        MVPbkStoreContact& ParentStoreContact() const;
        MVPbkStoreContactField* RetrieveField(
            const MVPbkContactLink& aContactLink) const;

    private:  // Data
        CContactItemFieldSet* iFieldSet;
        mutable TContactField iCurrentField;
        CContact* iParentContact;
    };


// INLINE FUNCTIONS

inline const MVPbkStoreContactField* TContactFieldCollection::FieldPointer() const
    {
    return &iCurrentField;
    }


} // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_TCONTACTFIELDCOLLECTION_H
//End of file


