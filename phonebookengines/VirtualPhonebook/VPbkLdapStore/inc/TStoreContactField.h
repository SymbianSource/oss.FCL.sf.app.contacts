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
* Description:  TStoreContactField - Contact field class.
*
*/


#ifndef __TSTORECONTACTFIELD_H__
#define __TSTORECONTACTFIELD_H__

// INCLUDES
#include <mvpbkstorecontactfield.h>
#include "tcontactfielddata.h"

// FORWARD DECLARATIONS
class CLDAPContactField;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContact;

// CLASS DECLARATIONS

/**
*  TStoreContactField - contact field class
*  
*/
class TStoreContactField : public MVPbkStoreContactField
    {
    public:     // TStoreContactField public constructor

        /**
        @function   TStoreContactField
        @discussion TStoreContactField default contructor
        */
        TStoreContactField();

    public:     // TStoreContactField public methods
        
        /**
        @function   SetContact
        @discussion Sets this field contact.
        @param      aContact Contact
        */
        void SetContact(CContact& aContact);

        /**
        @function   SetContact
        @discussion Sets this field.
        @param      aField Contact field
        */
        void SetField(CLDAPContactField& aField);

        /**
        @function   Field
        @discussion Returns this field's ldap field.
        @return     Contact field
        */
        inline CLDAPContactField* Field();
        
    public:     // Methods from MVPbkObjectHierarchy

        /**
        @function   ParentObject
        @discussion Returns this field parent object - contact.
        @return     Parent object.
        */
        MVPbkObjectHierarchy& ParentObject() const;

    public:     // Methods from MVPbkBaseContactField

        /**
        @function   ParentContact
        @discussion Returns this field parent contact.
        @return     Parent contact.
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        @function   MatchFieldType
        @discussion Matches this field type.
        @param      aMatchPriority Priority
        @return     Field type.
        */
        const MVPbkFieldType* MatchFieldType(TInt aMatchPriority) const;
        
        /**
        @function   BestMatchingFieldType
        @discussion Returns the best matching type of the field.
        @return     Field type or null
        */
        const MVPbkFieldType* BestMatchingFieldType() const;

        /**
        @function   FieldData
        @discussion Returns this field data.
        @return     Field data.
        */
        const MVPbkContactFieldData& FieldData() const;

        /**
        @function   IsSame
        @discussion Returns true if this is the field as passed.
        @param      aOther Contact field to compare.
        @return     True, this is the same, false when not.
        */
        TBool IsSame(const MVPbkBaseContactField& aOther) const;
        
    public:     // Methods from MVPbkStoreContactField

        /**
        @function   SupportsLabel
        @discussion Returns true if this field supports label.
        @return     True, supports label, false when not.
        */
        TBool SupportsLabel() const;
        
        /**
        @function   FieldLabel
        @discussion Returns this field label.
        @return     Field label.
        */
        TPtrC FieldLabel() const;

        /**
        @function   SetFieldLabelL
        @discussion Sets this field label.
        @param      Field label.
        */
        void SetFieldLabelL(const TDesC& aText);
        
        /**
        @function   MaxLabelLength
        @discussion Returns this field label maximum length.
        @return     Field label maximum length.
        */
        TInt MaxLabelLength() const;
        
        /**
        @function   FieldData
        @discussion Returns this field data.
        @return     Field data.
        */
        MVPbkContactFieldData& FieldData();

        /**
        @function   CloneLC
        @discussion Clones this field.
        @return     Cloned field.
        */
        MVPbkStoreContactField* CloneLC() const;
        
        /**
        @function   CreateLinkLC
        @discussion Creates link to this field.
        @return     Link to this field.
        */
        MVPbkContactLink* CreateLinkLC() const;
        
    private:    // TStoreContactField private member variables

        /// Contact reference
        CContact* iContact;

        /// Contact field reference
        CLDAPContactField* iField;

        /// Contact field data
        TContactFieldData iFieldData;
    };

} // End of namespace LDAPStore
#endif  // __TSTORECONTACTFIELD_H__

