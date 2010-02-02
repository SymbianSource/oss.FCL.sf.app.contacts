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
* Description:  Phonebook 2 contact editor field array.
*
*/


#ifndef CPBK2CONTACTEDITORFIELDARRAY_H
#define CPBK2CONTACTEDITORFIELDARRAY_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactEditorFieldArray.h"
#include "TPbk2ContactEditorParams.h"
#include "CPbk2ContactEditorArrayItem.h"
#include "Pbk2UIFieldProperty.hrh"

// FORWARD DECLARATIONS
class MPbk2ContactEditorUiBuilder;
class CPbk2PresentationContact;
class CPbk2PresentationContactField;
class CPbk2IconInfoContainer;
class MPbk2ContactEditorField;
class MVPbkFieldType;
class CVPbkContactManager;
class CPbk2ReadingFieldEditorVisitor;
class MPbk2ContactEditorFieldVisitor;
class MPbk2ContactEditorExtension;
class CPbk2ContactEditorFieldFactory;
class CPbkIconInfoContainer;
class CPbk2UIFieldArray;
class MPbk2ApplicationServices;
// CLASS DECLARATION

/**
 *  An array of editor fields.
 *
 */
NONSHARABLE_CLASS(CPbk2ContactEditorFieldArray) : 
        public CBase,
        public MPbk2ContactEditorFieldArray
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aManager          Virtual Phonebook contact manager.
         * @param aContact          The presentation level contact.
         * @param aUiBuilder        A reference to the dialog builder.
         * @param aEditorExtension  Contact editor extension. 
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorFieldArray* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2PresentationContact& aContact,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2ContactEditorFieldFactory& aFieldFactory);
        
        /**
		 * Creates a new instance of this class.
		 *
		 * @param aManager          Virtual Phonebook contact manager.
		 * @param aContact          The presentation level contact.
		 * @param aUiBuilder        A reference to the dialog builder.
		 * @param aEditorExtension  Contact editor extension. 
		 * @param aAppServices	    Pointer to application services
		 * @return  A new instance of this class.
		 */
		static CPbk2ContactEditorFieldArray* NewL(
				CVPbkContactManager& aContactManager,
				CPbk2PresentationContact& aContact,
				MPbk2ContactEditorUiBuilder& aUiBuilder,
				CPbk2ContactEditorFieldFactory& aFieldFactory,
				MPbk2ApplicationServices* aAppServices );
        
        /**
         * Destructor.
         */
        ~CPbk2ContactEditorFieldArray();

    public: // Interface

        /**
         * Creates editor fields from the store contact.
         */
        void CreateFieldsFromContactL(TPbk2ContactEditorParams& aParams);

    public: // From MPbk2ContactEditorFieldArray
        inline TInt Count() const;
        inline CPbk2ContactEditorArrayItem& At(
                TInt aIndex );
        void SaveFieldsL();
        TBool FieldsChanged() const;
        TBool AreAllUiFieldsEmpty() const;
        void SetFocus(
                TInt aFieldIndex );
        CPbk2ContactEditorArrayItem* Find(
                TInt aControlId );
        TInt FindIndex(
                TInt aControlId );
        TInt AddNewFieldL(
                const MVPbkFieldType& aFieldType );
        void AddNewFieldL(
                const TPbk2FieldGroupId aAddressGroup );
        void RemoveField(
        		CPbk2ContactEditorArrayItem& aField );
        void RemoveFieldFromUI( 
        		TInt aIndex );
        TInt AddNewFieldL(
                const MVPbkFieldType& aFieldType, const TDesC& aName );
        void UpdateControlsL();
        
    public: // Implementation
        TBool IsEditorFieldControlEmpty( TInt aIndex ) const;
        TBool IsEditorUiFieldControlEmpty( TInt aIndex ) const;
  
    private: // Implementation
        CPbk2ContactEditorFieldArray(
                CPbk2PresentationContact& aContact,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2ContactEditorFieldFactory& aFieldFactory,
                MPbk2ApplicationServices* aAppServices = NULL );
        void ConstructL(
                CVPbkContactManager& aContactManager );
        TInt GetUiFieldIndex(
                TInt aContactFieldIndex );
        TInt GetUiFieldIndex(
        		TPbk2FieldGroupId aContactAddressGroupID );
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
        void __DbgTestInvariant() const;
        TBool IsFieldPartOfAddress(CPbk2PresentationContactField& aField);
        void CreateUIFieldL(
        		CPbk2UIFieldArray& aFieldsArr, 
        		TPbk2FieldGroupId aAddressGroup,
				TPbk2FieldOrder aOrder, 
        		TInt& aNextFreePos );
        
    private: // Data
        /// Ref: The contact that is edited
        CPbk2PresentationContact& iContact;
        /// Ref: Editor UI creator
        MPbk2ContactEditorUiBuilder& iUiBuilder;
		/// Own: An array of dialog fields
		RPointerArray<CPbk2ContactEditorArrayItem> iFieldArray;
        /// Own: Reading field visitor
        CPbk2ReadingFieldEditorVisitor* iReadingFieldBinder;
        /// Ref:Field factory
        CPbk2ContactEditorFieldFactory& iFieldFactory;
        /// Own:Contact editor params
        TPbk2ContactEditorParams iParams;
		/// Own: icon info container
        CPbk2IconInfoContainer* iIconInfoContainer;
        // Ref: Pointer to Pbk2 application services
        MPbk2ApplicationServices* iAppServices;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::Count
// --------------------------------------------------------------------------
//
inline TInt CPbk2ContactEditorFieldArray::Count() const
    {
    return iFieldArray.Count();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::At
// --------------------------------------------------------------------------
//    
inline CPbk2ContactEditorArrayItem& CPbk2ContactEditorFieldArray::At
        ( TInt aIndex )
    {
    return *iFieldArray[aIndex];
    }

#endif // CPBK2CONTACTEDITORFIELDARRAY_H
            
// End of File
