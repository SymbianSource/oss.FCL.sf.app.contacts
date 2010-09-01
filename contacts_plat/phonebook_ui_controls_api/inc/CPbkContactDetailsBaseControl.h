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
*     UI control base class for Phonebook's "Contact Info View".
*
*/


#ifndef __CPbkContactDetailsBaseControl_H__
#define __CPbkContactDetailsBaseControl_H__

//  INCLUDES
#include <coecntrl.h>       // CCoeControl
#include "MPbkContactUiControl.h"

//  FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;
class TPbkContactItemField;
class CPbkContactDetailsBaseControlExt;


//  CLASS DECLARATION

/**
 * Phonebook "Contact Info View" UI control base class. 
 * Presents the fields of a contact as a list.
 */
class CPbkContactDetailsBaseControl :
        public CCoeControl,
		public MPbkContactUiControl
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aResID        Id of a PBK_CONTACTINFO_CONTROL resource.
         * @param aParent       parent of this control.
         * @param aContact      Contact to display. This object does NOT take
         *                      ownership of aContact.
         * @param aEngine       Contact engine needed for MPbkFieldAnalyzer
         *                      functions. This object does NOT take
         *                      ownership of aContact.
         */
        IMPORT_C static CPbkContactDetailsBaseControl* NewL(
                TInt aResId,
                const CCoeControl& aParent, 
                CPbkContactItem* aContact,
                CPbkContactEngine* aEngine );

        /**
         * Destructor.
         */
        ~CPbkContactDetailsBaseControl();

	protected:
        /**
         * Constructor.
         */
		CPbkContactDetailsBaseControl();

    public:  // interface
        /** 
         * Updates this control to display aContact's details.
         * NOTE: This function does not redraw the control on screen.
         * NOTE: This function is leave-safe; it quarantees that this 
         * control's state does not change if this function leaves.
         *
         * @param aContact  Contact to display.  This object does NOT take
         *                  ownership of aContact.
         */
        virtual void UpdateL(CPbkContactItem* aContact) = 0;

        /**
         * Returns the contact item. 
         */
        virtual CPbkContactItem& ContactItem() = 0;

        /**
         * Returns the currently selected field. 
         *
         * @return  currently selected field or NULL if no selection.
         */
        virtual TPbkContactItemField* CurrentField() const = 0;

        /**
         * Hides the thumbnail window control is showing. 
         * Call ShowThumbnailL() or UpdateL() to restore the thumbnail.
         */
        virtual void HideThumbnail() = 0;

        /**
         * Shows the thumbnail image.
         */
        virtual void ShowThumbnailL() = 0;

		/**
		 * Extension point.
		 */
		virtual CPbkContactDetailsBaseControlExt*
			ContactDetailsBaseControlExt() { return NULL; }
    };

#endif  // __CPbkContactDetailsBaseControl_H__

// End of File
