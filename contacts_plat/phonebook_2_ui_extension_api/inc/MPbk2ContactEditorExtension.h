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
* Description:  Phonebook 2 contact editor UI extension interface.
*
*/


#ifndef MPBK2CONTACTEDITOREXTENSION_H
#define MPBK2CONTACTEDITOREXTENSION_H

// INCLUDES
#include <MPbk2UiReleasable.h>
#include <MPbk2ContactEditorEventObserver.h>

//  FORWARD DECLARATIONS
class MVPbkStoreContactField;
class MPbk2ContactEditorField;
class MPbk2ContactEditorUiBuilder;
class CEikMenuPane;
class MPbk2FieldProperty;
class CEikButtonGroupContainer;
class CPbk2IconInfoContainer;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI extension interface.
 */
class MPbk2ContactEditorExtension : public MPbk2UiReleasable,
                                    public MPbk2ContactEditorEventObserver
    {
    public: // Interface

        /**
         * Creates a new contact editor field.
         *
         * @param aField                The field to create editor field for.
         * @param aFieldProperty        Field property.
         * @param aUiBuilder            Builder for adding the field into dialog.
         * @param aIconInfoContainer    Icon container for setting the icon.
         * @return  A new contact editor field or NULL.
         */
         virtual MPbk2ContactEditorField* CreateEditorFieldL(
                MVPbkStoreContactField& aContactField,
                const MPbk2FieldProperty& aFieldProperty,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer ) = 0;

        /**
         * Filters the menu pane of the Phonebook 2 contact editor.
         *
         * @param aResourceId   Menu resource id.
         * @param aMenuPane     Menu pane which will be filtered.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) = 0;

        /**
         * Processes extension commands.
         *
         * @param aCommandId    Command id.
         * @return  ETrue if command was handled, EFalse otherwise.
         */
        virtual TBool ProcessCommandL(
                TInt aCommandId ) = 0;

        /**
         * For adding additional functionality before deleting
         * the contact and exiting editor.
         *
         * @param aParams   Event parameters.
         * @return  ETrue if the delete and exit is ok, EFalse otherwise.
         */
        virtual TBool OkToDeleteContactL(
                MPbk2ContactEditorEventObserver::TParams& aParams ) = 0;

        /**
         * For adding additional functionality before saving
         * the contact and exiting editor.
         *
         * @param aParams   Event parameters.
         * @return  ETrue if the save and exit is ok, EFalse otherwise.
         */
        virtual TBool OkToSaveContactL(
                MPbk2ContactEditorEventObserver::TParams& aParams ) = 0;

        /**
         * For modifying a dialog's button group. Called before dialog
         * have been activated.
         *
         * @param aButtonGroupContainer     Dialog's button group container.
         */
        virtual void ModifyButtonGroupContainerL(
                CEikButtonGroupContainer& aButtonGroupContainer ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTEDITOREXTENSION_H

// End of File
