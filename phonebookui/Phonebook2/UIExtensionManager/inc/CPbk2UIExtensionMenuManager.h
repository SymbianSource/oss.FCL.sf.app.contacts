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
* Description:  Phonebook 2 UI extension menu manager.
*
*/


#ifndef CPBK2UIEXTENSIONMENUMANAGER_H
#define CPBK2UIEXTENSIONMENUMANAGER_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CPbk2ThinUIExtensionLoader;
class CEikMenuPane;
class CPbk2UIExtensionInformation;
class MPbk2ContactUiControl;

/**
 * Phonebook 2 UI extension menu manager.
 *
 * Menu manager that initializes menu pane from resources or loads the
 * related extension if needed.
 */
NONSHARABLE_CLASS(CPbk2UIExtensionMenuManager) : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionLoader      Extension loader.
         * @param aThinExtensionLoader  Thin extension loader.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionMenuManager* NewL(
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionMenuManager();

    public: // Interface

        /**
         * Initializes the menu pane.
         *
         * @param aViewId           Id of the view whose menu
         *                          pane is being initialized.
         * @param aFilteringFlags   Menu filtering flags that are
         *                          on in the current state of the UI.
         * @param aResourceId       Resource id of the menu that
         *                          is being initialized.
         * @param aMenuPane         Menu pane that is being initialized.
         * @param aUiControl        Current UI control.
         */
        void DynInitMenuPaneL(
                TUid aViewId,
                TUint32 aFilteringFlags,
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aUiControl );

        /**
         * Returns UIDs of the plugins that informed to
         * implement the command.
         *
         * @param aCommandId    The command id that is checked for.
         * @param aPluginUids   An array for plugin UIDs.
         */
        void PluginForCommandIdL(
                TInt aCommandId,
                RArray<TUid>& aPluginUids ) const;

    private: // Implementation
        CPbk2UIExtensionMenuManager(
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader );
        void DoDynInitMenuPaneL(
                CPbk2UIExtensionInformation& aPluginInformation,
                TUid aViewId,
                TUint32 aFilteringFlags,
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aUiControl );

    private: // Data
        /// Ref: Extension loader for loading extensions when needed
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Ref: Yhin extension loader
        CPbk2ThinUIExtensionLoader& iThinExtensionLoader;
    };

#endif // CPBK2UIEXTENSIONMENUMANAGER_H

// End of File
