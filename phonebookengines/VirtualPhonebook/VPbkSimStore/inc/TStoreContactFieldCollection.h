/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook store field collection implementation
*
*/


#ifndef VPBKSIMSTORE_TSTORECONTACTFIELDCOLLECTION_H
#define VPBKSIMSTORE_TSTORECONTACTFIELDCOLLECTION_H


// INCLUDES
#include <MVPbkStoreContactFieldCollection.h>
#include "TStoreContactField.h"

// FORWARD DECLARATIONS
class CVPbkSimContact;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContact;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model field collection to a virtual Phonebook field collection.
 */
NONSHARABLE_CLASS(  TStoreContactFieldCollection ): public MVPbkStoreContactFieldCollection
    {
    public:  // Constructor and destructor
        
        /**
        * C++ Constructor.
        */
        TStoreContactFieldCollection();

    public:  // New functions
        /**
         * Resets a new parent contact for this field collection.
         *
         * @param aParentContact    the contact whose field collection this is.
         * @param aSimContact the native sim contact that owns the data
         */
        void SetContact( CContact& aParentContact, 
            CVPbkSimContact& aSimContact );

        /**
         * Returns a pointer to the field that is used as a return value of 
         * FieldAt functions.
         *
         * @see CContact::AddFieldL
         */
        const MVPbkStoreContactField* FieldPointer() const;

    public:  // Functions from base classes

        /**
        * From MVPbkStoreContactFieldCollection
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        * From MVPbkStoreContactFieldCollection
        */
        TInt FieldCount() const;

        /**
        * From MVPbkStoreContactFieldCollection
        */
        const MVPbkStoreContactField& FieldAt( TInt aIndex ) const;

        /**
        * From MVPbkStoreContactFieldCollection
        */
        MVPbkStoreContactField& FieldAt( TInt aIndex );

        /**
        * From MVPbkStoreContactFieldCollection
        */
        MVPbkStoreContactField* FieldAtLC( TInt aIndex ) const;

        /**
        * From MVPbkStoreContactFieldCollection
        */
        MVPbkStoreContact& ParentStoreContact() const;
      
        /**
        * From MVPbkStoreContactFieldCollection
        */
        MVPbkStoreContactField* RetrieveField(
            const MVPbkContactLink& aContactLink ) const;
        
    private:  // Data
        /// Own: the current field that is used by FieldAt
        mutable TStoreContactField iCurrentField;
        /// Ref: the contact whose field collection this is.
        CContact* iParentContact;
        /// Ref: native sim field array
        RPointerArray<CVPbkSimCntField>* iSimFields;
    };


// INLINE FUNCTIONS

inline const MVPbkStoreContactField* TStoreContactFieldCollection::FieldPointer() const
    {
    return &iCurrentField;
    }


} // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_TSTORECONTACTFIELDCOLLECTION_H
//End of file


