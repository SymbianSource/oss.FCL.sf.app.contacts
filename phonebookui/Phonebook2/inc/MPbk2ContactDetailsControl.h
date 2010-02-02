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
* Description:  Phonebook 2 contact details control interface.
*
*/


#ifndef MPBK2CONTACTDETAILSCONTROL_H
#define MPBK2CONTACTDETAILSCONTROL_H

// INCLUDES
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MPbk2ContactDetailsControlExt;

// CLASS DECLARATION

/**
 * Phonebook 2 contact details control interface.
 * Presents the fields of a contact as a list.
 */
class MPbk2ContactDetailsControl : public MPbk2ContactUiControl,
                                   public MPbk2ContactUiControl2
    {
    public: // Interface

        /**
         * Updates this control to display the details of the given contact.
         * NOTE: This function does not redraw the control on the screen.
         * NOTE: This function is leave-safe; it quarantees that this
         * control's state does not change if this function leaves.
         *
         * @param aContact  The contact to display. This object takes
         *                  the ownership of the contact.
         */
        virtual void UpdateL(
                MVPbkStoreContact* aContact ) = 0;

        /**
         * Returns the field count.
         *
         * @return  Field count.
         */
        virtual TInt FieldCount() const = 0;

        /**
         * Returns given field's position in the list.
         *
         * @param aField    The field whose position to inspect.
         * @return  Given field's position in the list.
         */
        virtual TInt FieldPos(
                const MVPbkStoreContactField& aField ) const = 0;

        /**
         * Extension point.
         *
         * @return  Contact details control extension
         */
        virtual MPbk2ContactDetailsControlExt* ContactDetailsControlExt()
                { return NULL; }
    };

#endif // MPBK2CONTACTDETAILSCONTROL_H

// End of File
