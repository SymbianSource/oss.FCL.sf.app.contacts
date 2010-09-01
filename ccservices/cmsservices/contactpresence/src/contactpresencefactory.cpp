/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory class for ececontactpresence.dll
*
*/


// INCLUDE FILES

#include <e32std.h>

#include <contactpresencefactory.h>

#include "contactpresence.h"

#include <mcontactpresence.h>


// ================= MEMBER FUNCTIONS =======================
//

EXPORT_C MContactPresence* TContactPresenceFactory::NewContactPresenceL(
    MContactPresenceObs& aObs )
    {
    return CContactPresence::NewL( aObs );
    }

EXPORT_C MContactPresence* TContactPresenceFactory::NewContactPresenceL(
    CVPbkContactManager& aContactManager,
    MContactPresenceObs& aObs )
    {
    return CContactPresence::NewL( aContactManager, aObs );
    }



