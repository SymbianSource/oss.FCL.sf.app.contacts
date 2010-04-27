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
* Description:  Phonebook 2 start-up monitor interface.
*
*/


#ifndef MPBK2STARTUPMONITOR_H
#define MPBK2STARTUPMONITOR_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbk2StartupObserver;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 start-up monitor interface.
 * Offers a possibility to get a start-up complete event.
 */
class MPbk2StartupMonitor
    {
    public: // Interface

        /**
         * Extensions that are loaded in start-up must call this
         * after their own start-up routines have been done.
         */
        virtual void HandleStartupComplete() = 0;

        /**
         * Called if there is an error during the start-up.
         *
         * @param aError    Error code.
         */
        virtual void HandleStartupFailed(
                TInt aError ) = 0;

        /**
         * Adds a new observer for the start-up process.
         * Duplicate observers are not added.
         *
         * @param aObserver     New observer for the start-up process.
         */
        virtual void RegisterEventsL(
                MPbk2StartupObserver& aObserver ) = 0;

        /**
         * Removes an observer from the monitor.
         * Does nothing the if observer does not exist.
         *
         * @param aObserver     An observer to be removed.
         */
        virtual void DeregisterEvents(
                MPbk2StartupObserver& aObserver ) = 0;

        /**
         * Notify the startup monitor of an application view launch.
         * The monitor will be activated only for the first application
         * view but the client doesn't need to worry about that.
         *
         * Must be called from the application view's DoActivateL if
         * the view can be directly launched from other process
         * (e.g. using CPbkViewState or CPbk2ViewState).
         *
         * Uses AllContactsViewL for waiting. If you have some
         * other view then use the specialised version.
         *
         * @param aViewId       Id of the launched view.
         */
        virtual void NotifyViewActivationL(
                TUid aViewId ) = 0;

        /**
         * Notify the startup monitor of an application view launch.
         * The monitor will be activated only for the first application
         * view but the client doesn't need to worry about that.
         *
         * Must be called from the application view's DoActivateL if
         * the view can be directly launched from other process
         * (e.g. using CPbkViewState or CPbk2ViewState).
         *
         * Uses given contact view for waiting.
         *
         * @param aViewId       Id of the launched view.
         * @param aContactView  The view that will be waited by the monitor.
         *                      The monitor saves the reference of
         *                      aContactView until it gets it's job done.
         *                      The application view must call
         *                      NotifyViewDeActivation in DoDeactivate
         *                      to reset the aContactView reference.
         */
        virtual void NotifyViewActivationL(
                TUid aViewId,
                MVPbkContactViewBase& aContactView ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* StartupMonitorExtension( TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2StartupMonitor()
                {}
    };

/// Use this UID to access startup monitor extension 2.
//  Used as a parameter to StartupMonitorExtension() method.
const TUid KPbk2StartupMonitorExtensionUid = { 1 };

/**
 * This class is an extension to MPbk2StartupMonitor.
 *
 * @see MPbk2StartupMonitor
 *
 */
class MPbk2StartupMonitorExtension
    {
    protected:  // Destructor
        virtual ~MPbk2StartupMonitorExtension() { }

    public:

        /**
         * Disable monitoring.
         */
        virtual void DisableMonitoring() = 0;
    };

#endif // MPBK2STARTUPMONITOR_H

// End of File
