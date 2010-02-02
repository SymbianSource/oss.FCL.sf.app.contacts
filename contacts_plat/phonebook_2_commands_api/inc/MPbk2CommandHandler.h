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
* Description:  Phonebook 2 command handler interface.
*
*/


#ifndef MPBK2COMMANDHANDLER_H
#define MPBK2COMMANDHANDLER_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CPbk2AppViewBase;
class CEikMenuPane;
class MPbk2Command;
class MPbk2MenuCommandObserver;
class CAknToolbar;

// CLASS DECLARATION

/**
 * Phonebook 2 command handler interface.
 */
class MPbk2CommandHandler
    {
    public: // Interface

        /**
         * Handles Phonebook 2 commands.
         *
         * @param aCommandId    The id of the command to handle.
         * @param aControl      The UI control of the calling view.
         * @param aAppView      Application view from where
         *                      the command is launched.
         * @return  ETrue if the command was handled, EFalse otherwise.
         */
        virtual TBool HandleCommandL(
                const TInt aCommandId,
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView ) = 0;

        /**
         * Performs Phonebook 2 menu filtering.
         *
         * @param aResourceId   Resource id of the filtered menu pane.
         * @param aMenuPane     Menu pane to be filtered.
         * @param aViewBase     Application view where the filtering
         *                      takes place.
         * @param aControl      The UI control of the calling view.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                CPbk2AppViewBase& aViewBase,
                MPbk2ContactUiControl& aControl ) = 0;

        /**
         * Registers (attaches) to an AIW service.
         *
         * @param aInterestId           Id of the AIW interest.
         * @param aMenuResourceId       Resource id of the menu to
         *                              attach to the AIW interest.
         * @param aInterestResourceId   Resource id of the AIW interest.
         * @param aAttachBaseService    If ETrue, attaches the AIW
         *                              resource id to the AIW base
         *                              service also. This enables command
         *                              handling via AIW without menus.
         */
        virtual void RegisterAiwInterestL(
                const TInt aInterestId,
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseService ) = 0;

        /**
         * Returns the AIW service command id associated to
         * given menu command. If found, it means that there is a
         * provider which can handle the menu command.
         *
         * @see CAiwServiceHandler::ServiceCmdByMenuCmd
         * @param aMenuCmdId    Menu command id to inspect.
         * @return  Service command id or KAiwCmdNone.
         */
        virtual TInt ServiceCmdByMenuCmd(
                TInt aMenuCmdId ) const = 0;

        /**
         * Adds the command into the Phonebook 2 command store and executes it.
         * Takes the ownership of the command.
         *
         * @param aCommand  The command to be executed.
         */
        virtual void AddAndExecuteCommandL(
                MPbk2Command* aCommand ) = 0;

        /**
         * Adds a menu command observer.
         *
         * @param aObserver     Menu command observer to add.
         */
        virtual void AddMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver ) = 0;

        /**
         * Removes a menu command observer.
         *
         * @param aObserver     Menu command observer to remove.
         */
        virtual void RemoveMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver ) = 0;

        /**
         * Initialises toolbar before it is drawn.
         *
         * @param aResourceId   The resource id of the toolbar.
         * @param aToolbar      The toolbar object.
         * @param aAppView      Application view from where
         *                      the command is launched.
         * @param aControl      The UI control of the calling view.
         */
        virtual void DynInitToolbarL(
                TInt aResourceId,
                CAknToolbar* aToolbar,
                const CPbk2AppViewBase& aAppView,
                MPbk2ContactUiControl& aControl ) = 0;

        /**
         * Handles toolbar events for a certain toolbar item.
         *
         * @param aCommand      The command id of the toolbar item.
         * @param aControl      The UI control of the calling view.
         * @param aAppView      Application view from where
         *                      the command is launched.
         */
        virtual void OfferToolbarEventL(
                TInt aCommand,
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* CommandHandlerExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected:  // Disabled functions
        virtual ~MPbk2CommandHandler()
                {}
    };

#endif // MPBK2COMMANDHANDLER_H

// End of File
