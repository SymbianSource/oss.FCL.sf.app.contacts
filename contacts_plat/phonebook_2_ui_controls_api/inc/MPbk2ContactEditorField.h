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
* Description:  Phonebook 2 contact editor field interface.
*
*/


#ifndef MPBK2CONTACTEDITORFIELD_H
#define MPBK2CONTACTEDITORFIELD_H

// INCLUDES
#include <w32std.h>

// FORWARD DECLARATIONS
class CEikEdwin;
class MPbk2FieldProperty;
class MVPbkStoreContactField;
class MPbk2ContactEditorFieldVisitor;

//Use this UID to access contact store extension 2. 
//Used as a parameter to ContactEditorFieldExtension() method.
const TUid KMPbk2ContactEditorFieldExtension2Uid = { 2 };
// CLASS DECLARATION

/**
 * Phonebook 2 contact editor field interface.
 */
class MPbk2ContactEditorField
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactEditorField()
                {}
    
        /**
         * Return the dialog control id associated to this field.
         *
         * @return  Dialog control id.
         */
        virtual TInt ControlId() const = 0;

        /**
         * Returns the editor control.
         *
         * @return  Editor control.
         */
        virtual CEikEdwin* Control() const = 0;

        /**
         * Saves editor control data to contact item field.
         */
        virtual void SaveFieldL() = 0;


        /**
         * Returns ETrue if the data in the field has changed and 
         * the contact needs to be saved.
		 * @return  ETrue if success, EFalse otherwise
         */
        virtual TBool FieldDataChanged() const = 0;

        /**
         * Returns the contact item fields label.
         *
         * @return Field label.
         */
        virtual TPtrC FieldLabel() const = 0;

        /**
         * Sets given label as the contact item field's label
         * Takes ownership of the label.
         *
         * @param aLabel    The label to set.
         */
        virtual void SetFieldLabelL(
                const TDesC& aLabel ) = 0;

		/**
		 * Returns the text in the fields control.
         *
         * @return The text in the control.
		 */
		virtual HBufC* ControlTextL() const = 0;

        /**
         * Sets the editor focus to this field
         */
        virtual void SetFocus() = 0;
        
        /**
         * Activates the control.
         */
        virtual void ActivateL() = 0;
        
        /**
         * Returns the contact field.
         *
         * @return  Contact field.
         */
        virtual MVPbkStoreContactField& ContactField() const = 0;

        /**
         * Returns field property.
         *
         * @return  Field property.
         */
        virtual const MPbk2FieldProperty&  FieldProperty() const = 0;

        /**
         * Accepts contact editor field visitor.
         *
         * @param aVisitor  Visitor to accept.
         */
        virtual void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor ) = 0;

        /**
         * Returns ETrue of the editor consumers this key.
         *
         * @param aKeyEvent     The key event.
         * @param aType         Key even type.
         * @return  ETrue if editor consumes the event.
         */
        virtual TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorFieldExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTEDITORFIELD_H

// End of File
