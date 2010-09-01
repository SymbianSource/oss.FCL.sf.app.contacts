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
*        Interface for UI controls, which handles contacts.
*
*/


#ifndef __MPbkContactUiControl_H__
#define __MPbkContactUiControl_H__

//  INCLUDES
#include <cntdef.h>     // TContactItemId

// FORWARD DECLATIONS
class CContactIdArray;
class TPbkContactItemField;
class MPbkContactUiControlExt;
class MObjectProvider;

// CLASS DECLARATION

/**
 * Interface for ui controls, which handles contacts.
 */
class MPbkContactUiControl
    {
    protected: // Destructor
        virtual ~MPbkContactUiControl() {}

    public: // Interface
        /**
         * Returns the number of active entries in the UI control.
         */
        virtual TInt NumberOfItems() const =0;

        /**
         * Returns ETrue if there are marked contact items.
         */
        virtual TBool ItemsMarked() const =0;

        /**
         * Returns an array of currently marked items.
		 * If no items marked the array contains the focused item only.
         */
        virtual const CContactIdArray& MarkedItemsL() const =0;

        /**
         * Returns the currently focused contact's id.
         */
        virtual TContactItemId FocusedContactIdL() const =0;

        /**
         * Returns the currently focused contact field or NULL if this control
         * does not support field-level focus.
         */
        virtual const TPbkContactItemField* FocusedField() const =0;

		/**
		 * Extension point.
		 */
		virtual MPbkContactUiControlExt* ContactUiControlExt() { return NULL; }

        /**
         * Returns the controls object provider object.
         */
        virtual MObjectProvider* ObjectProvider() { return NULL; }

        /**
         * Returns the currently focused contact field index or -1 if this control
         * does not support field-level focus.
         */
        virtual TInt FocusedFieldIndex() const { return -1; }
            
    };

#endif // __MPbkContactUiControl_H__

// End of File
