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
* Description:  Phonebook 2 UI extension factory interface.
*
*/


#ifndef MPBK2UIEXTENSIONFACTORY_H
#define MPBK2UIEXTENSIONFACTORY_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CAknView;
class CVPbkContactManager;
class MVPbkStoreContact;
class MPbk2ContactEditorControl;
class MPbk2ContactUiControlExtension;
class MPbk2AppUiExtension;
class MPbk2ContactEditorExtension;
class MPbk2SettingsViewExtension;
class MPbk2Command;
class MPbk2ContactUiControl;
class CAiwServiceHandler;
class MPbk2AiwInterestItem;

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension factory interface.
 *
 * Abstract interface for creating Phonebook 2 UI extensions. Instance of
 * this factory should be kept as long as objects created with it are in use.
 */
class MPbk2UIExtensionFactory
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UIExtensionFactory()
                {}

        /**
         * Creates a Phonebook 2 contact editor extension.
         *
         * @param aContactManager   Shared Virtual Phonebook contact manager
         *                          instance owned by the Phonebook 2
         *                          application.
         * @param aContact          The edited contact.
         * @param aEditorControl    Contact editor control.
         * @return  Phonebook 2 contact editor extension.
         */
        virtual MPbk2ContactEditorExtension* CreatePbk2ContactEditorExtensionL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl ) = 0;

        /**
         * Creates a Phonebook 2 UI control extension.
         *
         * @param aContactManager   Shared Virtual Phonebook contact manager
         *                          instance owned by the Phonebook 2
         *                          application.
         * @return  Phonebook 2 UI control extension.
         */
        virtual MPbk2ContactUiControlExtension*  CreatePbk2UiControlExtensionL(
                CVPbkContactManager& aContactManager ) = 0;

        /**
         * Creates a Phonebook 2 settings extension for the settings view.
         *
         * @param aContactManager   Shared Virtual Phonebook contact manager
         *                          instance owned by the Phonebook 2
         *                          application.
         * @return  Phonebook 2 settings extension.
         */
        virtual MPbk2SettingsViewExtension* CreatePbk2SettingsViewExtensionL(
                CVPbkContactManager& aContactManager ) = 0;

        /**
         * Creates a Phonebook 2 application UI extension.
         *
         * @param aContactManager   Shared Virtual Phonebook contact manager
         *                          instance owned by the Phonebook 2
         *                          application.
         * @return  Phonebook 2 UI application UI extension.
         */
        virtual MPbk2AppUiExtension* CreatePbk2AppUiExtensionL(
                CVPbkContactManager& aContactManager ) = 0;

        /**
         * Creates a Phonebook 2 command object for a given command id.
         *
         * @param aCommandId        The command id to create
         *                          the command object for.
         * @param aUiControl        UI control from where the command
         *                          is launched from.
         * @return  Created Phonebook 2 command object.
         */
        virtual MPbk2Command* CreatePbk2CommandForIdL(
                TInt aCommandId,
                MPbk2ContactUiControl& aUiControl ) const = 0;

        /**
         * Creates a Phonebook 2 AIW interest item for a given service id.
         *
         * @param aInterestId       AIW service command id to create
         *                          the interest for.
         * @param aServiceHandler   AIW service handler.
         * @return  Created Phonebook 2 AIW interest item.
         */
        virtual MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler ) const = 0;

        /**
         * Creates a new extension-implemented Phonebook 2 view.
         *
         * Phonebook 2 extension manager creates CPbk2UIExtensionView
         * instance and returns it. CPbk2UIExtensionView then creates
         * a extension implemented MPbk2UIExtensionView instance and
         * delegates most of the calls to it. So, none of the UI extension
         * gets called by CreatePhonebook2ViewL.
         *
         * @param aId   Id of the extension view to create.
         * @return  The extension view instance.
         */
        virtual CAknView* CreatePhonebook2ViewL(
                TUid /*aId*/ )
            {
            // Default implementation
            return NULL;
            }

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* UIExtensionFactoryExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2UIEXTENSIONFACTORY_H

// End of File
