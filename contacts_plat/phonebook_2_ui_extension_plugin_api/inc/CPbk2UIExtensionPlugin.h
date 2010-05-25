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
* Description:  Phonebook 2 UI extension ECom plug-in interface.
*
*/


#ifndef CPBK2UIEXTENSIONPLUGIN_H
#define CPBK2UIEXTENSIONPLUGIN_H

// INCLUDE FILES
#include <e32base.h>
#include <ecom/ecom.h>
#include <MPbk2UiExtensionFactory.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionView;
class MPbk2UIExtensionView;
class CEikMenuPane;
class MPbk2ContactUiControl;
class CPbk2StorePropertyArray;
class TCoeHelpContext;
class CPbk2AppViewBase;
class CPbk2ViewGraph;
class CPbk2UIExtensionInformation;

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension ECom plug-in interface.
 * 
 * This class is responsible for defining the extension plug-in interface
 * and creating extension plug-in implementations.
 */
class CPbk2UIExtensionPlugin : public CBase,
                               public MPbk2UIExtensionFactory
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUid      Extension implementation UID.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionPlugin* NewL(
                TUid aUid );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionPlugin();

    public: // Interface

        /**
         * Called for creating a view from the extension.
         *
         * @param aViewId       The id of the view to be created. Check the
         *                      id and return a view if the extension
         *                      implements it.
         * @param aView         The view that owns the extension view.
         *                      Derived from CAknView.
         * @return  Created extension view or NULL.
         */
        virtual MPbk2UIExtensionView* CreateExtensionViewL(
                TUid aViewId,
                CPbk2UIExtensionView& aView ) = 0;

        /**
         * Called before a menu is launched. If the
         * extension is loaded on demand this won't be called
         * until the first command has caused the loading of the
         * extension.
         *
         * Use CEikMenuPane::MenuItemExists(TInt aCommandId,
         * TInt& aPosition) before setting an item dimmed.
         *
         * @param aResourceId       The id of the menu pane.
         * @param aMenuPane         A reference to the menu pane.
         * @param aControl          The current UI control.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl ) = 0;

        /**
         * Called in an application start-up and when the store
         * configuration changes. The extension can update the store
         * property array by adding or removing properties.
         *
         * @param aPropertyArray    Phonebook 2 store property array.
        */
        virtual void UpdateStorePropertiesL(
                CPbk2StorePropertyArray& aPropertyArray ) = 0;

        /**
         * Gets the help context from the extension.
         *
         * @param aContext          Help context, if found.
         * @param aView             Reference to the current view.
         * @param aUiControl        Reference to the current control.
         * @return  ETrue if help context was found.
         */
        virtual TBool GetHelpContextL(
                TCoeHelpContext& aContext,
                const CPbk2AppViewBase& aView,
                MPbk2ContactUiControl& aUiControl ) = 0;

        /**
         * Allows extensions to modify the view graph dynamically during
         * the application initialisation.
         *
         * The UI extension's start-up policy must be KPbk2LoadInStartup.
         *
         * @param aViewGraph    Phonebook 2 view graph.
         */
        virtual void ApplyDynamicViewGraphChangesL(
                CPbk2ViewGraph& aViewGraph ) = 0;

        /**
         * Allows extensions to add plugin information dynamically
         * during the application initialisation.
         *
         * The UI extension's start-up policy must be KPbk2LoadInStartup.
         */
        virtual void ApplyDynamicPluginInformationDataL(
                CPbk2UIExtensionInformation& aUiExtensionInformation ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* UIExtensionPluginExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    public: // Internal interface for the extension manager

        /**
         * Returns the implementation UID.
         *
         * @return  Implementation UID.
         */
        TUid ImplementationUid() const;

    private: // Data
        /// Own: Destructor key
        TUid iDtorIDKey;
        /// Own: Implementation UID
        TUid iImplementationUid;
    };

// --------------------------------------------------------------------------
// CPbk2UIExtensionPlugin::~CPbk2UIExtensionPlugin
// --------------------------------------------------------------------------
//
inline CPbk2UIExtensionPlugin::~CPbk2UIExtensionPlugin()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
inline CPbk2UIExtensionPlugin* CPbk2UIExtensionPlugin::NewL( TUid aUid )
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL( aUid,
        _FOFF( CPbk2UIExtensionPlugin, iDtorIDKey ) );

    CPbk2UIExtensionPlugin* self =
        reinterpret_cast<CPbk2UIExtensionPlugin*>( ptr );
    if ( self )
        {
        self->iImplementationUid = aUid;
        }
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionPlugin::ImplementationUid
// --------------------------------------------------------------------------
//
inline TUid CPbk2UIExtensionPlugin::ImplementationUid() const
    {
    return iImplementationUid;
    }

#endif // CPBK2UIEXTENSIONPLUGIN_H

// End of File
