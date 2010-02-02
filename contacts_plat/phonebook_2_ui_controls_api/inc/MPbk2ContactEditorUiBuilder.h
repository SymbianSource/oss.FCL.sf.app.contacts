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
* Description:  Phonebook 2 contact editor UI builder interface.
*
*/


#ifndef MPBK2CONTACTEDITORUIBUILDER_H
#define MPBK2CONTACTEDITORUIBUILDER_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CCoeControl;
class CEikCaptionedControl;
class MPbk2FieldProperty;
class CPbk2IconInfoContainer;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI builder interface.
 */
class MPbk2ContactEditorUiBuilder
    {
    public:  // Interface
    
		/**
		 * Creates an user interface control for a line.
         *
		 * @param aCaption	        The caption descriptor to the control.
		 * @param aControlId	    Id of the control.
		 * @param aControlType	    Type of the control.
		 */
        virtual CCoeControl* CreateLineL(
                const TDesC& aCaption,
                TInt aControlId,
                TInt aControlType ) = 0;

		/**
		 * Return the control corresponding to given id.
         *
         * @param aControlId    Id of the control.
		 * @return  The control or NULL if the control does not exist.
		 */
		virtual CCoeControl* Control(
                TInt aControlId ) const = 0;

        /**
         * Returns the control line corresponding to given id.
         *
         * @param aControlId    Id of the control.
         * @return  Line control.
         */
        virtual CEikCaptionedControl* LineControl(
                TInt aControlId ) const = 0;

        /**
         * Removes control identified by given id from the UI.
         *
         * @param aControlId    Id of the control.
         */
        virtual void DeleteControl(
                TInt aControlId ) = 0;

		/**
		 * Try to change the focus to given control.
         *
         * @param aControlId    Id of the control.
		 * @return	    ETrue if the focus was changed.
		 */
		virtual void TryChangeFocusL(
                TInt aControlId ) = 0;

		/**
		 * Change the line controls caption in the UI.
         *
         * @param aText     The caption to set.
		 */
		virtual void SetCurrentLineCaptionL(
                const TDesC& aText ) = 0;

		/**
		 * Sets the field to editable state.
         *
		 * @param aState    ETrue to set editable, EFalse otherwise.
		 */
		virtual void SetEditableL(
                TBool aState ) = 0;

        /**
         * Loads bitmap to field.
         *
         * @param aFieldProperty        Field property.
         * @param aIconInfoContainer    Icon info container.
         * @param aControl Id.          Id of the control.
         */
        virtual void LoadBitmapToFieldL(
                const MPbk2FieldProperty& aFieldProperty,
                const CPbk2IconInfoContainer& aIconInfoContainer,
                TInt aControlId ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorUiBuilderExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2ContactEditorUiBuilder()
                {};
    };

#endif // MPBK2CONTACTEDITORUIBUILDER_H

// End of File
