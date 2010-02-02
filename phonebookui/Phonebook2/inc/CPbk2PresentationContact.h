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
* Description:  Phonebook 2 presentation level contact.
*
*/


#ifndef CPBK2PRESENTATIONCONTACT_H
#define CPBK2PRESENTATIONCONTACT_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContact2.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MPbk2FieldPropertyArray;
class CPbk2PresentationContactFieldCollection;
class MVPbkContactGroup;
class MPbk2FieldProperty;

// CLASS DECLARATION

/**
 *  Phonebook 2 presentation level contact.
 */
class CPbk2PresentationContact : public CBase,
                                 public MVPbkStoreContact,
                                 public MVPbkStoreContact2                               
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreContact     The actual store contact that
         *                          has the data.
         * @param aFieldProperties  An array of field properties.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PresentationContact* NewL(
                MVPbkStoreContact& aStoreContact,
                const MPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Destructor.
         */
        virtual ~CPbk2PresentationContact();

    public: // Interface

        /**
         * Checks the limits of the fields in the store and
         * in the UI field property.
         *
         * @param aType     The type of the field to be added to the contact.
         * @return  ETrue if the field can be added to the contact.
         */
        IMPORT_C TBool IsFieldAdditionPossibleL(
                const MVPbkFieldType& aType ) const;

        /**
         * Adds store supported template fields to the contact.
         * Does not add duplicates.
         */
        IMPORT_C void AddSupportedTemplateFieldsL();

        /**
         * Returns an array of field properties that can be used to
         * add fields to the contact. The properties have field types,
         * and those field types are then possible to add to the contact.
         * If NULL is returned then it is  not possible to add fields
         * to the contact.
         *
         * @return  An array of field properties or NULL, caller
         *          owns the array but not the MPbk2FieldProperty instances.
         */
        IMPORT_C CArrayPtr<const MPbk2FieldProperty>*
            AvailableFieldsToAddL() const;

        /**
         * Returns Phonebook 2 specific field collection.
         *
         * @return  Phonebook 2 specific field collection.
         */
        inline CPbk2PresentationContactFieldCollection&
            PresentationFields() const;

        /**
         * Returns the actual store contact this
         * presentation contact presents.
         *
         * @return  Store contact.
         */
        inline MVPbkStoreContact& StoreContact() const;

    public: // From MVPbkStoreContact
        MVPbkObjectHierarchy& ParentObject() const;
        const MVPbkStoreContactFieldCollection& Fields() const;
        TBool IsSame(
                const MVPbkStoreContact& aOtherContact ) const;
        MVPbkContactLink* CreateLinkLC() const;
        void DeleteL(
                MVPbkContactObserver& aObserver ) const;
        void Close() const;
        MVPbkContactStore& ParentStore() const;
        MVPbkStoreContactFieldCollection& Fields();
        MVPbkStoreContactField* CreateFieldLC(
                const MVPbkFieldType& aFieldType ) const;
        TInt AddFieldL(
                MVPbkStoreContactField* aField );
        void RemoveField(
                TInt aIndex );
        void RemoveAllFields();
        void LockL(
                MVPbkContactObserver& aObserver ) const;
        void CommitL(
                MVPbkContactObserver& aObserver ) const;
        MVPbkContactLinkArray* GroupsJoinedLC() const;
        MVPbkContactGroup* Group();
        TInt MaxNumberOfFieldL(
                const MVPbkFieldType& aType ) const;
        TBool MatchContactStore(
                const TDesC& aContactStoreUri ) const;
        TBool MatchContactStoreDomain(
                const TDesC& aContactStoreDomain ) const;
        MVPbkContactBookmark* CreateBookmarkLC() const;
        TAny* StoreContactExtension(TUid aExtensionUid);
        
      public: // From MVPbkStoreContact2  
        MVPbkStoreContactProperties* PropertiesL() const;
        void SetAsOwnL(MVPbkContactObserver& aObserver) const;

    public:
        IMPORT_C TInt AddFieldL(
                MVPbkStoreContactField* aField, const TDesC& aName );
    private: // Implementation
        CPbk2PresentationContact(MVPbkStoreContact& aStoreContact,
                const MPbk2FieldPropertyArray& aFieldProperties );
        void ConstructL();
        TInt CurrentAmountOfFieldTypeInContact(
                const MVPbkFieldType& aType, const TDesC& aName ) const;

    private: // Data
        /// Ref: The actual store contact this contact maps to
        MVPbkStoreContact& iStoreContact;
        /// Ref: Static field properties
        const MPbk2FieldPropertyArray& iFieldProperties;
        /// Own: A collection of field mappers
        CPbk2PresentationContactFieldCollection* iCollection;
    };


// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2PresentationContact::PresentationFields
// --------------------------------------------------------------------------
//
inline CPbk2PresentationContactFieldCollection&
        CPbk2PresentationContact::PresentationFields() const
    {
    return *iCollection;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::StoreContact
// --------------------------------------------------------------------------
//
inline MVPbkStoreContact& CPbk2PresentationContact::StoreContact() const
    {
    return iStoreContact;
    }

#endif // CPBK2PRESENTATIONCONTACT_H

// End of File
