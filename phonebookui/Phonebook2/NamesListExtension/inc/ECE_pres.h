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

#ifndef MCONTACTPRESENCE_H
#define MCONTACTPRESENCE_H

#include <e32std.h>
#include <badesca.h>

class MDesC8Array;

/**
 * MContactPresence
 *
 * This version is based on tel.conference discussion on 28.11.2007.
 */
class MContactPresence
    {
public:

    /**
     * Destructor
     */
    virtual void Close() = 0;
    
    /**
     * Start to get contacts's presence info.
     * Response is received in MContactPresenceObs.
     * @param aLinkArray an array of packed contact link
     */
    virtual void GetPresenceStatusL( const MDesC8Array& aLinkArray ) = 0;    
    
    /**
     * Stop to get contacts' presence info.
     * @param aLinkArray an array of packed contact link
     */
    virtual void CancelGetPresenceStatusL( const MDesC8Array& aLinkArray ) = 0; 



    virtual void GetIconL( const TDesC8& aBrandId, const TDesC8& aElementId ) = 0;

    /**
     * Cancel all pending operations.
     * MContactPresenceObs is NOT called but the operations are cancelled silently.
     */
    virtual void CancelAll() = 0;    

    };


#endif // MCONTACTPRESENCE_H
