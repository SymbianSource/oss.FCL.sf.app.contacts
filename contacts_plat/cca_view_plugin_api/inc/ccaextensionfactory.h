/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 *  ECom interface for CCA Extension factory.
 *
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

#include "ccaextensionfactory.inl"

#endif // CCAEXTENSIONFACTORY_H
