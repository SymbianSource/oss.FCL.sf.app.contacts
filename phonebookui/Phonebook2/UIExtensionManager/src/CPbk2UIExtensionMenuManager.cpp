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


#include "CPbk2UIExtensionMenuManager.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionMenu.h"
#include "CPbk2ThinUIExtensionLoader.h"
#include <CPbk2UIExtensionThinPlugin.h>
#include <CPbk2UIExtensionPlugin.h>

// System includes
#include <eikmenup.h>

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::CPbk2UIExtensionMenuManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenuManager::CPbk2UIExtensionMenuManager(
        CPbk2UIExtensionLoader& aExtensionLoader,
        CPbk2ThinUIExtensionLoader& aThinExtensionLoader ) :
        iExtensionLoader( aExtensionLoader ),
        iThinExtensionLoader( aThinExtensionLoader )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::~CPbk2UIExtensionMenuManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenuManager::~CPbk2UIExtensionMenuManager()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::NewL
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenuManager* CPbk2UIExtensionMenuManager::NewL(
        CPbk2UIExtensionLoader& aExtensionLoader,
        CPbk2ThinUIExtensionLoader& aThinExtensionLoader )
    {
    CPbk2UIExtensionMenuManager* self =
        new ( ELeave ) CPbk2UIExtensionMenuManager
            ( aExtensionLoader, aThinExtensionLoader );
    return self;
	}

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionMenuManager::DynInitMenuPaneL(
        TUid aViewId, TUint32 aFilteringFlags, TInt aResourceId,
        CEikMenuPane* aMenuPane, MPbk2ContactUiControl& aUiControl )
    {
    const TInt informationCount =
        iExtensionLoader.PluginInformation().Count();

    for (TInt i = 0; i < informationCount; ++i)
        {
        CPbk2UIExtensionInformation* pluginInformation =
            iExtensionLoader.PluginInformation()[i];

        // Call for UI extensions
        DoDynInitMenuPaneL( *pluginInformation, aViewId, aFilteringFlags,
            aResourceId, aMenuPane, aUiControl );
        }
        
    // Call dynamic filtering for thin extension that are always loaded
    const TInt thinExtCount = iThinExtensionLoader.PluginCount();
    for ( TInt i = 0; i < thinExtCount; ++i )
        {
        iThinExtensionLoader.PluginAt(i).DynInitMenuPaneL( aResourceId,
            aMenuPane, aUiControl );
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::PluginForCommandIdL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionMenuManager::PluginForCommandIdL(
        TInt aCommandId, RArray<TUid>& aPluginUids ) const
    {
    const TInt informationCount =
        iExtensionLoader.PluginInformation().Count();
    for ( TInt i = 0; i < informationCount; ++i )
        {
        CPbk2UIExtensionInformation* pluginInformation =
            iExtensionLoader.PluginInformation()[i];
        if ( pluginInformation->ImplementsMenuCommand( aCommandId ) )
            {
            aPluginUids.AppendL( pluginInformation->ImplementationUid() );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenuManager::DoDynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionMenuManager::DoDynInitMenuPaneL(
        CPbk2UIExtensionInformation& aPluginInformation,
        TUid aViewId, TUint32 aFilteringFlags, TInt aResourceId,
        CEikMenuPane* aMenuPane, MPbk2ContactUiControl& aUiControl )
    {
    // Check has the extension been loaded
    TBool isLoaded =
        iExtensionLoader.IsLoaded( aPluginInformation.ImplementationUid() );

    // 1) Do filtering according to resource definitions
    TInt count = aPluginInformation.MenuCount();
    for ( TInt j = 0; j < count; ++j )
        {
        CPbk2UIExtensionMenu& menu = aPluginInformation.MenuAt(j);

        // Check is menu visible
        TBool isVisible =
            menu.IsVisible( aViewId, aResourceId, aFilteringFlags );

        if ( isVisible )
            {
            TInt position = KErrNotFound;

            // Menu is  visible, now check that the previous item exists
            TBool prevItemExists = aMenuPane->MenuItemExists
                ( menu.PreviousCommand(), position );
            // Check does the menu item already exist, or does the cascading
            // menu item exist
            TBool menuItemExists = ETrue;
            TInt cascadeMenuCmd = menu.CascadingMenuCmd();
            if ( cascadeMenuCmd == 0 ||
                    !aMenuPane->MenuItemExists( cascadeMenuCmd, position ) )
                {
                menuItemExists = EFalse;
                }

            if ( prevItemExists && !menuItemExists )
                {
                // The menu pane is visible with current flags and
                // the cascading menu has not been inserted by any
                // other extension yet
                aMenuPane->AddMenuItemsL
                    ( menu.MenuResId(), menu.PreviousCommand() );
                }
            }
        }

    // 2) If extension is loaded -> use dynamic filtering
    if ( isLoaded )
        {
        CPbk2UIExtensionPlugin* plugin = iExtensionLoader.LoadedPlugin
            ( aPluginInformation.ImplementationUid() );
        plugin->DynInitMenuPaneL( aResourceId, aMenuPane, aUiControl );
        }
    }

// End of File
