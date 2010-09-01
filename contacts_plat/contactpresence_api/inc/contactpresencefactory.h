/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory class for contactpresence.dll
*
*/


#ifndef CONTACTPRESENCEFACTORY_H
#define CONTACTPRESENCEFACTORY_H

#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATION
class MContactPresence;
class MContactPresenceObs;
class CVPbkContactManager;

/**
 * Factory class of contactpresence.dll
 *
 * @lib contactpresence.lib
 * @since S60 5.0
 */
class TContactPresenceFactory
    {

public:

    /**
     * Create new MContactPresence object.
     * @since S60 5.0
     * @param aObs callback observer implementation for receiving result data.
     * @return MContactPresence, ownership is transferred.
     */
    IMPORT_C static MContactPresence* NewContactPresenceL(
        MContactPresenceObs& aObs );

    /**
     * Create new MContactPresence object.
     * @since S60 5.0
     * @param aContactManager contact manager with open store.
     * @param aObs callback observer implementation for receiving result data.
     * @return MContactPresence, ownership is transferred.
     */
    IMPORT_C static MContactPresence* NewContactPresenceL(
        CVPbkContactManager& aContactManager,
        MContactPresenceObs& aObs );
    };

#endif  // CONTACTPRESENCEFACTORY_H

// End of File
