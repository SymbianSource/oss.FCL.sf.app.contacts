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

#ifndef MCONTACTPRESENCEOBS_H
#define MCONTACTPRESENCEOBS_H

#include <e32std.h>
// #include <badesca.h>

class CFbsBitmap;




/**
 * MContactPresenceObs
 *
 * This version is based on tel.conference discussion on 28.11.2007.
 *
 */
class MContactPresenceObs
    {
public:

    virtual void ReceiveIconInfoL( const TDesC8& aPackedLink, const TDesC8& aBrandId, const TDesC8& aElementId ) = 0;

    /**
     * Receives an icon that was requested by MContactPresence::GetIconL()
     */
    virtual void ReceiveIconFileL( const TDesC8& aBrandId, const TDesC8& aElementId, CFbsBitmap* aBrandedBitmap, CFbsBitmap* aMask ) = 0;

    };

#endif // MCONTACTPRESENCEOBS_H
