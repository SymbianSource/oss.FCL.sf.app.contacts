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
* Description:  Maps a Contact Model field to a Virtual Phonebook field.
*
*/

#ifndef VPBKCNTMODEL_TCONTACTFIELD_H
#define VPBKCNTMODEL_TCONTACTFIELD_H


// INCLUDES
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactField2.h>
#include "TContactFieldData.h"

// FORWARD DECLARATIONS
class CContactItemField;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CFieldTypeMap;
class CContact;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model field to a Virtual Phonebook field.
 */
NONSHARABLE_CLASS( TContactField ) :
    public MVPbkStoreContactField,
    public MVPbkStoreContactField2
    {
    public:  // Constructors
        /**
         * Constructor. Sets the parent contact and resets other data to null
         * values.
         */
        TContactField(CContact& aParentContact);

        /**
         * Destructor.
         */
        ~TContactField();

    public:  // New functions
        /**
         * Sets the Contact Model field to map.
         *
         * @param aCntModelField    Contact Model field to map to
         *                          MVPbkContactField.
         */
        void SetField
            (CContactItemField& aCntModelField);

        /**
         * Sets a new parent contact for this field. Resets the Contact Model
         * field mapping to null.
         */
        void SetParentContact(CContact& aParentContact);

        /**
         * Creates a new field instance that points to the same native
         * contact item
         * @return a new field
         */
        MVPbkStoreContactField* CloneLC() const;

        CContactItemField* NativeField() const;
        TInt NativeFieldIndex() const;

    public:  // from MVPbkStoreContactField
        virtual MVPbkBaseContact& ParentContact() const;
        const MVPbkFieldType* MatchFieldType(TInt aMatchPriority) const;
        const MVPbkFieldType* BestMatchingFieldType() const;
        TBool SupportsLabel() const;
        /// @precond SetField has been called
        TPtrC FieldLabel() const;
        /// @precond SetField has been called
        void SetFieldLabelL(const TDesC& aText);
        TInt MaxLabelLength() const;
        /// @precond SetField has been called
        const MVPbkContactFieldData& FieldData() const;
        /// @precond SetField has been called
        MVPbkContactFieldData& FieldData();
        /// @precond SetField has been called
        TBool IsSame(const MVPbkBaseContactField& aOther) const;
        MVPbkContactLink* CreateLinkLC() const;
        TAny* StoreContactFieldExtension(TUid aExtensionUid);

    public:  // from MVPbkStoreContactField2
        CArrayFix<TInt>* SpeedDialIndexesL();

    public:  // from MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    protected:  // Access for TNewContactField
        /// Used by TNewContactField
        void ResetNativeField();

    private:  // Data
        /// Ref: Parent contact
        CContact* iParentContact;
        /// Ref: Contact model field
        CContactItemField* iCntModelField;
        /// Ref: field data
        TContactFieldData iFieldData;
    };


// INLINE FUNCTIONS

inline CContactItemField* TContactField::NativeField() const
    {
    return iCntModelField;
    }

}  // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_TCONTACTFIELD_H

//End of file
