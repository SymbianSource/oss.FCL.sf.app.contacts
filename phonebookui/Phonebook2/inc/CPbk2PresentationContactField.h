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
* Description:  Phonebook 2 presentation level contact field.
*
*/


#ifndef CPBK2PRESENTATIONCONTACTFIELD_H
#define CPBK2PRESENTATIONCONTACTFIELD_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkStoreContactField.h>

// FORWARD DECLARATIONS
class MPbk2FieldProperty;
class MVPbkStoreContact;
class MVPbkContactFieldData;

// CLASS DECLARATION

/**
 * Phonebook 2 presentation level contact field.
 */
class CPbk2PresentationContactField : public CBase,
                                      public MVPbkStoreContactField
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperty    Phonebook 2 field property.
         * @param aStoreField       The field from the contact store.
         * @param aParentContact    The parent contact of the field.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PresentationContactField* NewL(
                const MPbk2FieldProperty& aFieldProperty,
                const MVPbkStoreContactField& aStoreField,
                MVPbkStoreContact& aParentContact );

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperty    Phonebook 2 field property.
         * @param aStoreField       The field from the contact store.
         * @param aParentContact    The parent contact of the field.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PresentationContactField* NewLC(
                const MPbk2FieldProperty& aFieldProperty,
                const MVPbkStoreContactField& aStoreField,
                MVPbkStoreContact& aParentContact );

        /**
         * Destructor.
         */
        ~CPbk2PresentationContactField();

    public: // Interface

        /**
         * Returns the maximum length of the field data.
         *
         * @return  Maximum length of field data.
         */
        IMPORT_C TInt MaxDataLength() const;

        /**
         * Checks if the field contains data.
         *
         * @return  ETrue if the field doesn't contain data.
         */
        IMPORT_C TBool IsEmpty() const;

        /**
         * Checks is the field editable.
         *
         * @return  ETrue if the field can be edited.
         */
        IMPORT_C TBool IsEditable() const;

        /**
         * Checks is the field removable.
         *
         * @return  ETrue if the field can not be removed from the
         *          contact. This is a static property of the field.
         */
        IMPORT_C TBool IsRemovable() const;

        /**
         * Checks is the visible in contact details view.
         *
         * @return  ETrue if the contact is visible in contact details view.
         */
        IMPORT_C TBool IsVisibleInDetailsView() const;
        
        /**
         * Sets field's dynamic visibility.
         *
         * @param aVisibility Dynamic Visibility.
         */
        IMPORT_C void SetDynamicVisibility( TBool aVisibility );

        /**
         * Gets field's dynamic visibility.
         *
         * @return Dynamic Visibility.
         */
        IMPORT_C TBool DynamicVisibility();

        /**
         * Returns the field's property.
         *
         * @return  The field's property.
         */
        inline const MPbk2FieldProperty& FieldProperty() const;

        /**
         * Returns corresponding store contact field.
         *
         * @return T    he contact store field.
         */
        inline MVPbkStoreContactField& StoreField() const;

    public: // From MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    public: // From MVPbkBaseContactField
        MVPbkBaseContact& ParentContact() const;
        const MVPbkFieldType* MatchFieldType(
                TInt aMatchPriority ) const;
        const MVPbkFieldType* BestMatchingFieldType() const;
        const MVPbkContactFieldData& FieldData() const;
        TBool IsSame(
                const MVPbkBaseContactField& aOther ) const;

    public: // From MVPbkStoreContactField
        TBool SupportsLabel() const;
        TPtrC FieldLabel() const;
        void SetFieldLabelL(
                const TDesC& aText );
        TInt MaxLabelLength() const;
        MVPbkContactFieldData& FieldData();
        MVPbkStoreContactField* CloneLC() const;
        MVPbkContactLink* CreateLinkLC() const;

    private: // Construction
        CPbk2PresentationContactField(
                const MPbk2FieldProperty& aFieldProperty,
                MVPbkStoreContact& aParentContact );
        void ConstructL(
                const MVPbkStoreContactField& aStoreField );

    private: // Data
        /// Ref: Phonebook 2 field property
        const MPbk2FieldProperty& iFieldProperty;
        /// Own: The field that is used to access actual field data
        MVPbkStoreContactField* iStoreField;
        /// Ref: The parent contact of the field
        MVPbkStoreContact& iParentContact;
        /// Ref: Presentation contact field text data
        MVPbkContactFieldData* iData;
        /// Own: Field's dynamic visibility.
        TBool iDynamicVisibility;
    };


// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::FieldProperty
// --------------------------------------------------------------------------
//
inline const MPbk2FieldProperty&
        CPbk2PresentationContactField::FieldProperty() const
    {
    return iFieldProperty;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactField::StoreField
// --------------------------------------------------------------------------
//
inline MVPbkStoreContactField&
        CPbk2PresentationContactField::StoreField() const
    {
    return *iStoreField;
    }

#endif // CPBK2PRESENTATIONCONTACTFIELD_H

// End of File
