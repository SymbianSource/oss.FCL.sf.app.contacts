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
* Description:  Phonebook 2 contact editor control interface.
*
*/


#ifndef MPBK2CONTACTEDITORCONTROL_H
#define MPBK2CONTACTEDITORCONTROL_H

// INCLUDES
#include <Pbk2FieldProperty.hrh>    // TPbk2FieldCtrlType

// FORWARD DECLATIONS
class MPbk2ContactEditorField;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor control interface.
 */
class MPbk2ContactEditorControl
    {
    public: // Interface

        /**
         * Sets the focus to the control in given index.
         *
         * @param aIndex The index of the control to set focused.
         */
        virtual void SetFocus(
                TInt aIndex ) = 0;

        /**
         * Returns the number of controls.
         *
         * @return  The number of controls in the editor.
         */
         virtual TInt NumberOfControls() const = 0;

        /**
         * Returns index of given control type in contact editor.
         *
         * @param aType         The control type to search for.
         * @param aIndex        Indicates from what index to
         *                      start matching controls.
         * @return  The index of the control, KErrNotFound if not found.
         */
        virtual TInt IndexOfCtrlType(
                TPbk2FieldCtrlType aType,
                TInt& aIndex ) const = 0;

        /**
         * Returns ETrue if all UI controls are empty.
         *
         * @return  ETrue if all controls are empty.
         */
        virtual TBool AreAllControlsEmpty() const = 0;

        /**
         * Returns ETrue if the control at given index is empty.
         *
         * @param aIndex    Index of control to query.
         * @return  ETrue if the control is empty.
         */
        virtual TBool IsControlEmptyL(
                TInt aIndex ) const = 0;

        /**
         * Returns editor field at given index.
         *
         * @param aIndex    Index of editor field to query.
         * @return  ETrue if the control is empty.
         */
        virtual MPbk2ContactEditorField& EditorField(
                TInt aIndex ) const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorControlExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2ContactEditorControl()
                {}
    };

#endif // MPBK2CONTACTEDITORCONTROL_H

// End of File
