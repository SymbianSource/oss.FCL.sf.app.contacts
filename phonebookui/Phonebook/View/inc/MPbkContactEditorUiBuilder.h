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
*     Phonebook Contact editor field abstract callback class.
*
*/


#ifndef __MPbkContactEditorUiBuilder_H__
#define __MPbkContactEditorUiBuilder_H__

//  INCLUDES
#include <e32def.h>
#include "PbkFields.hrh"    // TPbkFieldId

// FORWARD DECLARATIONS
class CCoeControl;
class CEikCaptionedControl;

// CLASS DECLARATION

/**
 * Phonebook Contact editor field abstract class. 
 */
class MPbkContactEditorUiBuilder
    {
    public:  // Interface
		/**
		 * Creates a User interface control for a line.
		 * @param aCaption	The caption descriptor to the control.
		 * @param aControlId	The created controls ID.
		 * @param aControlType	The created controls type.
		 */
        virtual CCoeControl* CreateLineL(const TDesC& aCaption,
                TInt aControlId, TInt aControlType) = 0;
        
        /**
         * Returns the control line corresponding to aControlId.
         */
        virtual CEikCaptionedControl* LineControl(TInt aControlId) const = 0;

        /**
         * Removed the aControlId control from the UI.
         */
        virtual void DeleteControl(TInt aControlId) = 0;

		/**
		 * Try to change the focus to aControlId.
		 * @return	ETrue if focus changes to aControlId
		 */
		virtual void TryChangeFocusL(TInt aControlId) = 0;

		/**
		 * Change the line controls caption aText in the UI.
		 */
		virtual void SetCurrentLineCaptionL(const TDesC& aText) = 0;

		/**
		 * Return the control object corresponding to aControlId.
		 * @return NULL if the control does not exist.
		 */
		virtual CCoeControl* Control(TInt aControlId) const = 0;

		/**
		 * Sets the field editable state.
		 * @param aState ETrue to set editable, EFalse otherwise
		 */
		virtual void SetEditableL(TBool aState) = 0;
    };

#endif // __MPbkContactEditorUiBuilder_H__
            
// End of File
