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
* Description:  Phonebook 2 thin UI extension ECom plug-in interface.
*
*/


#ifndef CPBK2UIEXTENSIONTHINPLUGIN_H
#define CPBK2UIEXTENSIONTHINPLUGIN_H

// INCLUDE FILES
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class MPbk2ContactUiControl;
class MPbk2StartupMonitor;

/**
 * Phonebook 2 thin UI extension ECom plug-in interface.
 * Extensions that implement this interface must be as small
 * as possible because all Phonebook 2 thin UI extension are loaded in
 * Phonebook 2 start-up.
 * 
 * This class is responsible for defining the thin extension plug-in interface
 * and creating thin extension plug-in implementations.
 */
class CPbk2UIExtensionThinPlugin : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUid      The implementation UID of the plugin.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionThinPlugin* NewL(
                TUid aUid );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionThinPlugin();

    public: // Interface

        /**
         * Called before a menu is launched.
         *
         * Use CEikMenuPane::MenuItemExists( TInt aCommandId,
         * TInt& aPosition ) before setting an item dimmed.
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
         * Called in application start-up after the application
         * services have been created.
         *
         * @param aStartupMonitor   The start-up monitor that can be
         *                          used to get UI completion event.
         *                          The extension must notify the
         *                          monitor after it's own start-up.
         */
        virtual void ExtensionStartupL(
                MPbk2StartupMonitor& aStartupMonitor ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ThinUIPluginExtension(
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
        /// Own: Destructor ID key
        TUid iDtorIDKey;
        /// Own: Implementation UID
        TUid iImplementationUid;
    };


// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2UIExtensionThinPlugin::~CPbk2UIExtensionThinPlugin
// --------------------------------------------------------------------------
//
inline CPbk2UIExtensionThinPlugin::~CPbk2UIExtensionThinPlugin()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionThinPlugin::NewL
// --------------------------------------------------------------------------
//
inline CPbk2UIExtensionThinPlugin* CPbk2UIExtensionThinPlugin::NewL
        ( TUid aUid )
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL
        ( aUid, _FOFF( CPbk2UIExtensionThinPlugin, iDtorIDKey ) );

    CPbk2UIExtensionThinPlugin* self =
        reinterpret_cast<CPbk2UIExtensionThinPlugin*>( ptr );
    if ( self )
        {
        self->iImplementationUid = aUid;
        }
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionThinPlugin::ImplementationUid
// --------------------------------------------------------------------------
//
inline TUid CPbk2UIExtensionThinPlugin::ImplementationUid() const
    {
    return iImplementationUid;
    }

#endif // CPBK2UIEXTENSIONTHINPLUGIN_H

// End of File
