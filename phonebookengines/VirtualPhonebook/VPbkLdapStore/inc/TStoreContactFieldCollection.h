/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact field collection class.
*
*/


#ifndef __TSTORECONTACTFIELDCOLLECTION_H__
#define __TSTORECONTACTFIELDCOLLECTION_H__

// INCLUDES
#include <mvpbkstorecontactfieldcollection.h>
#include "tstorecontactfield.h"

// FORWARD DECLARATIONS
class CLDAPContact;
class CLDAPContactField;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContact;

/**
*  TStoreContactFieldCollection - contact field collection class
*  
*/
class TStoreContactFieldCollection : public MVPbkStoreContactFieldCollection
    {
    public:     // TStoreContactFieldCollection public constructor
        
        /**
        @function   TStoreContactFieldCollection
        @discussion TStoreContactFieldCollection default contructor
        */
        TStoreContactFieldCollection();

    public:     // TStoreContactFieldCollection public methods

        /**
        @function   SetContact
        @discussion Set this field collection.
        @param      aContact Contact
        @param      aLDAPContact LDAP Contact
        @return     Pointer to instance of CStoreBookmark
        */
        void SetContact(CContact& aContact,CLDAPContact& aLDAPContact);

        /**
        @function   FieldPointer
        @discussion Returns field pointer
        @return     Field pointer
        */
        const MVPbkStoreContactField* FieldPointer() const;

    public:     // Methods from MVPbkBaseContactFieldCollection

        /**
        @function   ParentContact
        @discussion Returns contact
        @return     Contact
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        @function   FieldCount
        @discussion Returns number of fields in this collection.
        @return     Number of fields.
        */
        TInt FieldCount() const;

        /**
        @function   FieldAt
        @discussion Returns field at position.
        @param      aIndex Index to get field.
        @return     Field at position.
        */
        const MVPbkStoreContactField& FieldAt(TInt aIndex) const;

    public:     // Methods from MVPbkStoreContactFieldCollection

        /**
        @function   FieldAt
        @discussion Returns field at position.
        @param      aIndex Index to get field.
        @return     Field at position.
        */
        MVPbkStoreContactField& FieldAt(TInt aIndex);

        /**
        @function   FieldAt
        @discussion Creates new field at position.
        @param      aIndex Index to get field.
        @return     Field at position.
        */
        MVPbkStoreContactField* FieldAtLC(TInt aIndex) const;

        /**
        @function   ParentStoreContact
        @discussion Returns store contact.
        @param      aIndex Index to get field.
        @return     Field at position.
        */
        MVPbkStoreContact& ParentStoreContact() const;
      
        /**
        @function   RetrieveField
        @discussion Creates linked field.
        @param      aContactLink Link to create field.
        @return     Field at position.
        */
        MVPbkStoreContactField* RetrieveField(
            const MVPbkContactLink& aContactLink) const;
        
    private:    // TStoreContactFieldCollection private member variables

        /// Current field
        mutable TStoreContactField iCurrentField;

        /// Contact reference
        CContact* iContact;

        /// Contact fields referense
        RPointerArray<CLDAPContactField>* iFields;
    };

} // End of namespace LDAPStore
#endif // __TSTORECONTACTFIELDCOLLECTION_H__

