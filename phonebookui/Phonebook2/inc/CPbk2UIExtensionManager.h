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
* Description:  Phonebook 2 UI extension manager.
*
*/


#ifndef CPBK2UIEXTENSIONMANAGER_H
#define CPBK2UIEXTENSIONMANAGER_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CPbk2UIExtensionMenuManager;
class MPbk2UIExtensionFactory;
class CPbk2ViewGraph;
class MPbk2UIExtensionIconSupport;
class CEikMenuPane;
class CPbk2AppViewBase;
class MPbk2ContactUiControl;
class CPbk2ThinUIExtensionLoader;
class CPbk2UIExtensionConfigurationObserver;
class TCoeHelpContext;
class CPbk2ApplicationServices;

/**
 * Phonebook 2 UI extension manager.
 * Singleton class.
 * Responsible for:
 * - creating extension loaders, extension menu manager and extension factory
 * - applying extension view graph changes
 * - managing menu pane initialisation provided by extension menu manager
 * - retrieving help context provided by extension plugins
 */
class CPbk2UIExtensionManager : public CBase
    {
    public: // Interface
        /**
         * Returns the instance of this class or instantiates it
         * if it has not already been created.
         *
         * @return  The singleton instance of this class.
         */
        IMPORT_C static CPbk2UIExtensionManager* InstanceL();

        /**
         * Returns the extension factory. If no instance exists yet
         * it is created.
         *
         * @return  The extension factory.
         */
        IMPORT_C MPbk2UIExtensionFactory* FactoryL();

        /**
         * Applies the extension view graph changes to the Phonebook 2
         * view graph. The extension changes are loaded from extension
         * resources.
         *
         * @param aViewGraph    The view graph to modify.
         */
        IMPORT_C void ApplyExtensionViewGraphChangesL(
                CPbk2ViewGraph& aViewGraph );

        /**
         * Returns the extension icon support API. UI extensions can add
         * icons to icon container and to different icon arrays defining
         * iconInformation in extension's PHONEBOOK2_EXTENSION_INFORMATION
         * resource structure.
         *
         * @return  Extension manager's icon support interface.
         */
        IMPORT_C MPbk2UIExtensionIconSupport& IconSupportL();

        /**
         * Lets extensions to filter the menu pane. Menu filtering can
         * be done in two different ways.
         *
         * 1) Static filtering.
         *    It's not needed to load the extension dll to do this.
         *    Menus are filtered according to menu definitions in
         *    PHONEBOOK2_EXTENSION_INFORMATION of the extension's resource.
         *    Use menuFilteringFlags field of the
         *    PHONEBOOK2_EXTENSION_MENU_RESOURCE to set visibility flags.
         *
         * 2) Dynamic filtering.
         *    If flags are not enough it's possible to do an ECom plugin
         *    that is always loaded by the extension framework.
         *    The interface UID is of the plugin is
         *    KPbk2ThinUiExtensionInterfaceUID and the base class is
         *    CPbk2UIExtensionThinPlugin.
         *    Use KPbk2ThinUiExtensionDefaultDataString in default_data
         *    field.
         *
         * @param aResourceId   Resource id of the menu pane.
         * @param aMenuPane     Reference to the menu pane.
         * @param aViewBase     The view where the filtering takes place.
         * @param aControl      Contact UI control.
         */
        IMPORT_C void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                CPbk2AppViewBase& aViewBase,
                MPbk2ContactUiControl& aControl );

        /**
         * Gets help context from the extensions.
         *
         * @param aContext      The extension help context.
         * @param aView         Application view from where the help
         *                      is displayed.
         * @param aUiControl    Contact UI control.
         * @return  ETrue if help context was found.
         */
        IMPORT_C TBool GetHelpContextL(
                TCoeHelpContext& aContext,
                const CPbk2AppViewBase& aView,
                MPbk2ContactUiControl& aUiControl );

        /**
         * Returns the menu manager.
         *
         * @return  Menu manager.
         */
        CPbk2UIExtensionMenuManager& MenuManager() const;

        /**
         * Pushes this object to the cleanup stack for releasing.
         */
        inline void PushL();

    private: // Release interface
        friend void Release(
                CPbk2UIExtensionManager* aObj );
        virtual void DoRelease();
        static void CleanupRelease(
                TAny* aObj );

    private: // Implementation
        CPbk2UIExtensionManager();
        ~CPbk2UIExtensionManager();
        void ConstructL();
        void IncRef();
        TInt DecRef();

    private: // Data
        /// Own: Reference count
        TInt iRefCount;
        /// Own: UI extension menu manager
        CPbk2UIExtensionMenuManager* iMenuManager;
        /// Own: UI extension loader
        CPbk2UIExtensionLoader* iExtensionLoader;
        /// Own: UI extension factory
        MPbk2UIExtensionFactory* iExtensionFactory;
        /// Own: UI extension icon support
        MPbk2UIExtensionIconSupport* iIconSupport;
        /// Own: Thin UI extension loader
        CPbk2ThinUIExtensionLoader* iThinExtensionLoader;
        /// Own: Handles the updating of the store property array
        CPbk2UIExtensionConfigurationObserver* iStoreConfigurationObserver;
        /// Own: Application services
        CPbk2ApplicationServices* iAppServices;

    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// Release
// --------------------------------------------------------------------------
//
inline void Release( CPbk2UIExtensionManager* aObj )
    {
    if ( aObj )
        {
        aObj->DoRelease();
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::PushL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionManager::PushL()
    {
    CleanupStack::PushL( TCleanupItem( CleanupRelease, this ) );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::CleanupRelease
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionManager::CleanupRelease( TAny* aObj )
    {
    Release( static_cast<CPbk2UIExtensionManager*>( aObj ) );
    }

#endif // CPBK2UIEXTENSIONMANAGER_H

// End of File
