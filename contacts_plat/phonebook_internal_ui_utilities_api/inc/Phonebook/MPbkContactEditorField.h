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
*     Phonebook contact editor field abstract class.
*
*/


#ifndef __MPbkContactEditorField_H__
#define __MPbkContactEditorField_H__

//  INCLUDES
#include <e32def.h>
#include <coedef.h>
#include <w32std.h>
#include <PbkFields.hrh>    // TPbkFieldId

// FORWARD DECLARATIONS
class TPbkContactItemField;
class CPbkContactItem;
class MPbkFieldEditorVisitor;

// CLASS DECLARATION

/**
 * Phonebook Contact editor field abstract class. 
 */
class MPbkContactEditorField
    {
    public:  // destructor
        /**
         * Destructor.
         */
        virtual ~MPbkContactEditorField() = 0;
		
    public:  // Interface
        /**
         * Return the dialog control id associated to this field.
         */
        virtual TInt ControlId() const = 0;

        /**
         * Returns the id of this field.
         */
        virtual TPbkFieldId FieldId() const = 0;

        /**
         * Save editor control data to contact item field.
         */
        virtual void SaveFieldL() = 0;

        /**
         * Returns ETrue if the data in the field changed and 
         * the contact now needs to be saved.
		 * @return ETrue if success, EFalse otherwise
         */
        virtual TBool FieldDataChanged() const = 0;

        /**
         * Adds editor field value to aContact.
         * @param aContact field value will be added to this contact
         */
        virtual void AddFieldL(CPbkContactItem& aContact) =0;

        /**
         * Returns the contact item fields label.
         */
        virtual TPtrC FieldLabel() const = 0;

        /**
         * Sets aLabel as the contact item fields label
         * Takes ownership of the aLabel.
         */
        virtual void SetFieldLabelL(HBufC* aLabel) = 0;
        
        /**
         * Returns the contact item field.
         */
        virtual const TPbkContactItemField& ContactItemField() const = 0;

		/**
		 * Returns the text in the fields control.
		 */
		virtual HBufC* ControlTextL() const = 0;

        /**
         * Sets text for control. Default implementation is empty.
         */
        virtual void SetControlTextL(const TDesC& /*aDes*/) { }

        /**
         * Activates the control.
         */
        virtual void ActivateL() = 0;

        /**
         * Accepts editor binder (visitor pattern). 
         */
        virtual void AcceptL(MPbkFieldEditorVisitor& aVisitor) = 0;

        /**
         * Returns ETrue of the editor consumers this key
         */
        virtual TBool ConsumesKeyEvent(const TKeyEvent& /*aKeyEvent*/, 
                TEventCode /*aType*/) = 0;
        

    };

inline MPbkContactEditorField::~MPbkContactEditorField()
	{ 
	}

#endif // __MPbkContactEditorField_H__
            
// End of File
