/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for custom icon change events.
*/

#ifndef MCUSTOMICONCHANGEOBSERVER_H
#define MCUSTOMICONCHANGEOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <ContactCustomIconPluginDefs.h>

// FORWARD DECLARATIONS
class CContactCustomIconPluginBase;

// CLASS DECLARATION

/**
 * Observer interface for custom icon change events.
 */
class MCustomIconChangeObserver
    {
    public: // Notifications from plug-in to client

        /**
         * Icon event type definitions
         */
        enum TEventType
            {
            /**
             * General icon update notification indicates that some
             * or all of the icons have changed in some manner (update,
             * removed, added).
             * Client app should update all custom icons.
             */
            EIconsMajorChange,

            /**
             * Icons have been updated.
             */
            EIconsUpdated,

            /**
             * Some of the icons have been removed.
             */
            EIconsRemoved,

            /**
             * There are new icons available.
             */
            EIconsAdded
            };

        /**
         * Icon change notification event from plug-in to the client.
         * Indicates that one or more of the icons have become invalid. Client
         * needs to refresh its view by loading and drawing new icons.
         *
         * @param aPlugin       Plug-in, which initiated this notification. Plug-in
         *                      should provide 'this' reference.
         * @param aEventType    Event type, @see TEventType
         * @param aIconIds      Icon id array indicating the icons, which have changed.
         *                      While have to be given-in by plug-in, semantics of
         *                      the parameter are loose. Specifically empty array
         *                      means the TEventType concerns all possible icons.
         */
        virtual void CustomIconEvent(
                CContactCustomIconPluginBase& aPlugin,
                TEventType aEventType,
                const TArray<TCustomIconId>& aIconIds ) = 0;

        /**
         * Factory function for getting access to extended notifications. Uid
         * defines the extension interface being used.
         *
         * @param aNotificationUid  Identifier for an extension API class.
         * @return  Extension class.
         */
        virtual TAny* IconChangeNotificationExtension(
                    TUid /*aNotificationUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        ~MCustomIconChangeObserver() {}
    };

#endif // MCUSTOMICONCHANGEOBSERVER_H

// End of File
