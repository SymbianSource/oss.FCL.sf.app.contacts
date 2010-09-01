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
* Description:  Phonebook 2 contact editor dialog UI field.
*
*/

#ifndef MPBK2CONTACTEDITORUIFIELD_H_
#define MPBK2CONTACTEDITORUIFIELD_H_

// INCLUDES
#include <w32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikEdwin;
class MPbk2ContactEditorUiBuilder;
class CPbk2IconInfoContainer;
class MPbk2UIField;
class TPbk2IconId;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI base field.
 */
class MPbk2ContactEditorUIField
    {
    public:
        /**
         * Destructor.
         */
    	virtual ~MPbk2ContactEditorUIField(){};

    
        /**
         * Return the dialog control id associated to this field.
         *
         * @return  Dialog control id.
         */
    	virtual TInt ControlId() = 0;

        /**
         * Returns the editor control.
         *
         * @return  Editor control.
         */
    	virtual CEikEdwin* Control() = 0;
        
        /**
         * Returns the UI field.
         *
         * @return  UI field.
         */
    	virtual MPbk2UIField* UIField() const = 0;

        /**
         * Handles custom field command.
         *
         * @param aCommand Custom command id.
         * @return  ETrue if command was executed.
         */
    	virtual TBool HandleCustomFieldCommandL(TInt aCommand) = 0;

        /**
         * Returns the contact item fields label.
         *
         * @return Field label.
         */
        virtual const TDesC& FieldLabel() const = 0;

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
		virtual const TDesC& ControlText() const = 0;
		
		/**
         * Sets given control text as the contact item control text
         * Takes ownership of the label.
         *
         * @param aText    The text to set.
         */
		virtual void SetControlTextL(
                const TDesC& aText ) = 0;

        /**
         * Sets the editor focus to this field
         */
		virtual void SetFocus() = 0;
        
        /**
         * Activates the control.
         */
		virtual void ActivateL() = 0;
        
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
         * Loads icon to field.
         *
         * @param aIconId     	Icon ID to load.
         */
		virtual void LoadBitmapToFieldL( const TPbk2IconId& aIconId ) = 0;
    };


#endif /*MPBK2CONTACTEDITORUIFIELDBASE_H_*/

// End of File