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
* Description:  An array of editor fields.
*
*/


#ifndef MPBK2CONTACTEDITORFIELDARRAY_H
#define MPBK2CONTACTEDITORFIELDARRAY_H

// FORWARD DECLARATIONS
class TPbk2ContactEditorParams;
class MPbk2ContactEditorUiBuilder;
class CPbk2PresentationContact;
class CPbk2PresentationContactField;
class CPbk2IconInfoContainer;
class MPbk2ContactEditorField;
class MVPbkFieldType;
class CPbk2ContactEditorArrayItem;

// CLASS DECLARATION


class MPbk2ContactEditorFieldArray
    {
    public: // Destruction    
        /**
         * Destructor.
         */
        virtual ~MPbk2ContactEditorFieldArray() {}
    
    public:  // Interface
        
        /**
         * Returns the amount fields in the array.
         *
         * @return The amount fields in the array.
         */
        virtual TInt Count() const = 0;
        
        /**
         * Returns the field in given index.
         *
         * @param aIndex    The index of the field.
         * @return The field in given index.
         */
        virtual CPbk2ContactEditorArrayItem& At(TInt aIndex) = 0;
        
        /**
         * Saves all fields in the editor to the contact item.
         *		 
         */
        virtual void SaveFieldsL() = 0;
        
        /**
         * Check are fields changed. SaveFieldsL method 
         * should call before this.
         *
         * @return ETrue if at least one field was changed.
         */
        virtual TBool FieldsChanged() const = 0;
        
        /**
         * Returns ETrue if all the fields of this contact are empty
         * or contain only whitespace characters.
         *
         * @return ETrue if all fields are empty.
         */
        virtual TBool AreAllUiFieldsEmpty() const = 0;
        
        /**
         * Sets the focus to the field in given index.
         *
         * @param aFieldIndex The index of the field to set.
         */
        virtual void SetFocus(TInt aFieldIndex) = 0;
        
        /**
         * Finds the field that has the given control id.
         *
         * @param aControlId    The control id of the field.
         * @return The field or NULL if not found.
         */
        virtual CPbk2ContactEditorArrayItem* Find(TInt aControlId) = 0;
        
        /**
         * Adds a new field to the contact and to the UI.
         *
         * @param aFieldType    The field type of the new field.
         * @return The control id of the new UI field.
         */
        virtual TInt AddNewFieldL(const MVPbkFieldType& aFieldType) = 0;
        
        /**
         * Removes the field from the contact and the editor.
         *
         * @param The field to remove.
         */
        virtual void RemoveField(CPbk2ContactEditorArrayItem& aField) = 0;

        /**
         * Adds a new field to the contact and to the UI.
         *
         * @param aFieldType    The field type of the new field.
         * @param aName         The name for field type of the new field.
         * @return The control id of the new UI field.
         */
        virtual TInt AddNewFieldL(const MVPbkFieldType& aFieldType,
                const TDesC& aName) = 0;
        
    };

#endif // MPBK2CONTACTEDITORFIELDARRAY_H
            
// End of File
