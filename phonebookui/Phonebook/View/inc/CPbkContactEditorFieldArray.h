/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Phonebook contact editor field array.
*
*/


#ifndef __CPbkContactEditorFieldArray_H__
#define __CPbkContactEditorFieldArray_H__

//  INCLUDES
#include <e32base.h>
#include "MPbkContactEditorCreateField.h"

// FORWARD DECLARATIONS
class MPbkContactEditorField;
class CPbkContactItem;
class MPbkContactEditorUiBuilder;
class TPbkContactItemField;
class CPbkIconInfoContainer;
class CPbkReadingEditorBinderVisitor;
class MPbkFieldEditorVisitor;

// CLASS DECLARATION

/**
 * Phonebook Contact editor field array. 
 */
NONSHARABLE_CLASS(CPbkContactEditorFieldArray) : 
        public CBase,
		public MPbkContactEditorCreateField
    {
    public:  // Constructors and destructor
        /**
         * Creates an editor for aContactItem.
         * @param aContactItem  edited contact item
         * @param aUiBuilder    reference to the UI creator
         */
		static CPbkContactEditorFieldArray* NewL
                (CPbkContactItem& aContactItem, 
                MPbkContactEditorUiBuilder& aUiBuilder);

        /**
         * Destructor.
         */
        ~CPbkContactEditorFieldArray();
		
    public:  // Interface
        /**
		 * 2nd part constructor.
         * Create dialog lines from iContactItem.
         */
		void CreateFieldsFromContactL();

        /**
         * Returns ETrue if all the fields of this contact are empty 
         * or contain only whitespace characters.
         */
        TBool AreAllFieldsEmpty() const;

        /**
         * Adds aField to the editor.
         * NOTE: it does not add them to the contact item.
         * this done by the add item manager.
         * @see TPbkAddItemManager
         */
        void AddFieldL(TPbkContactItemField& aField);

        /**
         * Removes aField from the contact item and editor.
         */
        void RemoveField(MPbkContactEditorField& aField);

        /**
         * Returns the amount of fields in the contact item.
         */
        TInt ContactItemFieldCount() const;

        /**
         * Returns the amount of editors.
         */
        TInt EditorCount() const;

		/**
		 * Returns the aFieldIndex field.
		 * @precond	aFieldIndex >= 0 && aFieldIndex < EditorCount()
		 */
		const MPbkContactEditorField& FieldAt(TInt aFieldIndex);

        /**
         * Returns the editor field for aField.
		 * @return NULL if not found.
         */
        MPbkContactEditorField* Find(const TPbkContactItemField& aField) const;

        /**
         * Returns the contact editor field corresponding to aControlId.
		 * @return NULL if not found.
         */
        MPbkContactEditorField* Find(TInt aControlId) const;

		/**
		 * Returns the contact items fields index corresponding to aField.
		 */
		TInt FindContactItemFieldIndex(const TPbkContactItemField& aField) const;

        /**
         * Saves all fields in the editor to the contact item.
         */
        void SaveFieldsL();

        /**
         * Returns ETrue if any of the fields have changed.
         * EFalse otherwise.
         */
        TBool FieldsChanged() const;

		/**
		 * Set the focus onto the aFocusIndex control.
		 */
		void SetFocusToIndex(TInt aFocusIndex);

        /**
         * Accepts the given visitor, ie. calls accept for each field
         * of the array.
         * @param aVisitor Visitor to accept.
         */
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);

	private: // from MPbkContactEditorCreateField
		MPbkContactEditorField& CreateFieldL(CPbkFieldInfo& aFieldInfo);

    private:  // Implementation
        CPbkContactEditorFieldArray(
				CPbkContactItem& aContactItem, 
				MPbkContactEditorUiBuilder& aUiBuilder);
        void ConstructL();
		MPbkContactEditorField& DoAddFieldL(
				TPbkContactItemField& aField, TInt aFieldInsertIndex);

		void __DbgTestInvariant() const;

    private:  // Data
        /// Ref: edited contact item
        CPbkContactItem& iContactItem;
        /// Ref: editor UI creator
        MPbkContactEditorUiBuilder& iUiBuilder;
        /// Own: Contact editor fields
        CArrayPtrFlat<MPbkContactEditorField> iFieldArray;
		/// Own: icon info container
		CPbkIconInfoContainer* iIconInfoContainer;
        /// Own: contact field editor binder for japanese reading fields
        CPbkReadingEditorBinderVisitor* iReadingFieldBinder;
    };

#endif // __CPbkContactEditorFieldArray_H__
            
// End of File
