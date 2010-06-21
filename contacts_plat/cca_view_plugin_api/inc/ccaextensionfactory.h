/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef CCAEXTENSIONFACTORY_H
#define CCAEXTENSIONFACTORY_H

#include <e32base.h>

class MCCAViewLauncher;
class MCCAStatusProvider;

/**
 *  ECom interface for CCA Extension factory.
 *  At the moment extension factory interface is used only for extend CCA 
 *  Status button implementation:
 *   - CCA Status button implementation uses this interface to launch external 
 *   views (MCCAViewLauncher) when user tap the Status button.
 *   - CCA Status button implementation uses MCCAStatusProvider interface to
 *   read status information from plug-in if status infromation doesn't found in 
 *   Presence Cache.
 *  @lib n/a
 *  @since 9.2
 */
class CCCAExtensionFactory : public CBase
    {
public: // constructor and destructor
    inline static CCCAExtensionFactory* NewL(
                        const TUid& aImplementationUid );

    inline virtual ~CCCAExtensionFactory();

    /**
     * Creates a view launcher.
     * Ownership is given.
     * @return View Launcher instance @see MCCAViewLauncher
     */
    virtual MCCAViewLauncher* CreateViewLauncherL()
        {
        return NULL;
        }
    
    /**
     * Returns Service ID of the plug-in.
     */
    virtual const TDesC& ServiceID() const = 0;

    /**
     * Extension point for the factory
     */
    virtual TAny* FactoryExtension(
            TUid /*aExtensionUid*/ )
        {
        return NULL;
        }

private: // data

    // Own: ID key
    TUid iDtor_ID_Key;

    };

// Use this UID to access CCCAExtensionFactory's status provider creator.
// Used as a parameter to CCCAExtensionFactory::FactoryExtension method.
const TUid KCCAExtensionFactoryStatusProviderCreatorUid = { 2 };

/**
 * This class is an extension to CCCAExtensionFactory.
 *
 * @see CCCAExtensionFactory
 * @see KCCAExtensionFactoryStatusProviderCreatorUid
 */
class MCCAExtensionFactoryStatusProviderCreator
    {
    protected:  // Destructor
        virtual ~MCCAExtensionFactoryStatusProviderCreator() { }

    public:

        /**
         * Creates a status provider.
         * Ownership is given.
         * @return Status provider instance @see MCCAStatusProvider
         */
        virtual MCCAStatusProvider* CreateStatusProviderL() = 0;

    };

/**
 *  View launcher extension for the CCA.
 */
class MCCAViewLauncher
    {
public:
    /// virtual destructor
    virtual ~MCCAViewLauncher() {};

    enum TCCAView
        {
        /// External view for MyCard
        EMyCardView =   0x0,
        /// External view for contact
        ECCAView =      0x1
        };

    /**
     * Returns a true if view launcher supports current view.
     */
    virtual TBool IsViewSupported( MCCAViewLauncher::TCCAView aView ) = 0;

    /**
     * CCA and MyCard uses this function to launch external views.
     * @param aView @see TCCAView
     * @param aPackedContactLink packed link to the
     *        contact @see MVPbkContactLink
     */
    virtual void LaunchViewL( MCCAViewLauncher::TCCAView aView,
            const TDesC8* aPackedContactLink = NULL ) = 0;

    /**
     *  Extension point for the View Launcher
     */
    virtual TAny* ViewLauncherExtension(
            TUid /*aExtensionUid*/ )
        {
        return NULL;
        }

protected:
    MCCAViewLauncher() {};

    };

class MCCAStatusProviderObserver;
class MVPbkContactLink;
class CGulIcon;

/**
 * Status provider extension for the CCA
 */
class MCCAStatusProvider
    {
public:
    /// virtual destructor
    virtual ~MCCAStatusProvider() {};

    /**
     * Add observer
     * @param aObserver instance @see MCCAStatusProviderObserver
     */
    virtual void AddObserverL(
            MCCAStatusProviderObserver& aObserver ) = 0;

    /**
     * Removes observer
     * @param aObserver instance @see MCCAStatusProviderObserver
     */
    virtual void RemoveObserver(
            MCCAStatusProviderObserver& aObserver ) = 0;

    /**
     * CCA fetch status information for the Status button with this method 
     * if status information is not found from Presence Cache
     * @param aLink
     * @param aText Status Text, ownership is transfered 
     * @param aIcon Status Icon, ownership is transfered
     */
    virtual void GetStatusInformationL(
            const MVPbkContactLink& aLink,
            HBufC*& aText,
            CGulIcon*& aIcon ) = 0;

    /**
     * CCA fetch status information for the Status button with this method 
     * if status information is not found from Presence Cache
     * This function is used only for fetching My card status information
     * @param aLink
     * @param aText Status Text, ownership is transfered 
     * @param aIcon Status Icon, ownership is transfered
     */
     virtual void GetMyCardStatusInformationL(
             const MVPbkContactLink& aLink,
             HBufC*& aText,
             CGulIcon*& aIcon ) = 0;

    /**
     *  Extension point for the status provider
     */
    virtual TAny* StatusProviderExtension(
            TUid /*aExtensionUid*/ )
        {
        return NULL;
        }

protected:
    MCCAStatusProvider() {};
	
    };

/**
 * Interface to observe content changes
 */
class MCCAStatusProviderObserver
    {
public:
    /**
     * Event types
     */
    enum TCCAStatusProviderObserverEvent
        {
        /// Status is changed and CCA should fetch status information again
        EStatusChanged = 0x0
        };

    /**
     * Plug-in can notify CCA about status events 
     * @param aEvent Type of the event
     * @param aLink Contact which the event is about
     */
    virtual void StatusEvent(
        MCCAStatusProviderObserver::TCCAStatusProviderObserverEvent aEvent,
        const MVPbkContactLink* aLink = NULL ) = 0;

    /**
     *  Extension point for the MCCAStatusProviderObserver
     */
    virtual TAny* StatusProviderObserverExtension(
            TUid /*aExtensionUid*/ )
        {
        return NULL;
        }
   
protected:
    MCCAStatusProviderObserver(){};
    virtual ~MCCAStatusProviderObserver() {};
	
    };


#include "ccaextensionfactory.inl"

#endif // CCAEXTENSIONFACTORY_H
