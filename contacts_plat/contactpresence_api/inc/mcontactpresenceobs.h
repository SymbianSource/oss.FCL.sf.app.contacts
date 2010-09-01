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
* Description:  Presence in pbonebook
*
*/


#ifndef MCONTACTPRESENCEOBS_H
#define MCONTACTPRESENCEOBS_H

#include <e32std.h>
#include <mcontactpresenceinfo.h>

class CFbsBitmap;

/**
 * MContactPresenceObs is an observer interface to receive asynchronous 
 * callbacks from MContactPresence methods.
 *
 * @lib contactpresence.lib
 * @since s60 v5.0
 */
class MContactPresenceObs
    {
public:


    /**
     * Receive contact's presence status icon info.
     * The request is launched by MContactPresence::SubscribePresenceInfoL.
     * @param aPackedLink a packed contact link for phonebook contact
     * @param aBrandId branding id to identify branding package. 
     *        Zero length data means that icon is not available anymore.
     * @param aElementId element id in branding package
     *        Zero length data means that icon is not available anymore.    
     */
    virtual void ReceiveIconInfoL(
        const TDesC8& aPackedLink,
        const TDesC8& aBrandId,
        const TDesC8& aElementId ) = 0;

    /**
     * Receive Icon File.
     * The request is launched by MContactPresence::GetPresenceIconFileL.     
     * @param aBrandId branding id to identify branding package
     * @param aElementId element id in branding package
     * @param aBrandedBitmap Bitmap. Ownership is transferred.
     * @param aMask Mask bitmap. Ownership is transferred.
     */
    virtual void ReceiveIconFileL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId,
        CFbsBitmap* aBrandedBitmap,
        CFbsBitmap* aMask ) = 0;
              
    /**
     * Error occurred in MContactPresence::SubscribePresenceInfoL request.
     * @param aPackedLink a packed contact link for phonebook contact
     * @param aStatus Error status
     */
    virtual void PresenceSubscribeError(
        const TDesC8& aPackedLink,
        TInt aStatus ) = 0;

    /**
     * Error occurred in MContactPresence::GetPresenceIconFileL or in GetPresenceInfoL request.
     * @param aOpId Operation ID to identify the operation
     * @param aStatus Error status
     */
    virtual void ErrorOccured(
        TInt aOpId,
        TInt aStatus ) = 0;
        
    /**
     * Receive contact's presence status icon infos for all services.
     * The request is launched by MContactPresence::GetPresenceInfoL.
     * @param aPackedLink a packed contact link for phonebook contact
     * @param aInfoArray array of icon infos, ownership is not transferred
     * @param aOpId Operation ID to identify the operation      
     */
    virtual void ReceiveIconInfosL(
        const TDesC8& aPackedLink,
        RPointerArray <MContactPresenceInfo>& aInfoArray,
        TInt aOpId ) = 0;           

    };

#endif // MCONTACTPRESENCEOBS_H
