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
* Description:  Presence of contact in phonebook.
*
*/


#ifndef MCONTACTPRESENCE_H
#define MCONTACTPRESENCE_H

#include <e32std.h>
#include <badesca.h>

class MDesC8Array;
class TSize;

/**
 * MContactPresence is interface of Contact Presence API. Objects of this
 * interface are created using TContactPresenceFactory in contactpresencefactory.h.
 * This API is used to get presence status icons of phonebook contacts.
 * Asynchronous methods of this interface are completed in MContactPresenceObs
 * which shall be implemented by the users of this interface.
 *
 * @lib contactpresence.lib
 * @since s60 v5.0
 */
class MContactPresence
    {
public:

    /**
     * Destructor that must be called by client to destroy this object.
     */
    virtual void Close() = 0;

    /**
     * Get contacts's presence icon info and to subscribe for future changes in
     * icon info. Response(s) are received in MContactPresenceObs::ReceiveIconInfoL()
     * for each individual link if presence status for that link is available.     
     * If subscribing to certain contact presences fails then the observer
     * will be informed by a call to MContactPresenceObs::PresenceSubscribeError()
     * for every failed contact subscription. 
     *
     * @param aLinkArray an array of packed contact links, specified in phonebook API
     */
    virtual void SubscribePresenceInfoL(const MDesC8Array& aLinkArray ) = 0;
    
    /**
     * Get icon file.
     * Response is received in MContactPresenceObs::ReceiveIconFileL().
     * If operation fails then MContactPresenceObs::ErrorOccured() is called.
     *
     * @param aBrandId branding id
     * @param aElementId element id
     * @return Operation ID
     */
    virtual TInt GetPresenceIconFileL( const TDesC8& aBrandId,
        const TDesC8& aElementId ) = 0;

    /**
     * Set icon size
     * If not set then a default size is used in response of GetPresenceIconFileL.
     * @param aSize new size used from now on.
     */
    virtual void SetPresenceIconSize( const TSize aSize  ) = 0;

    /**
     * Cancels subscriptions to presence info.
     * Cancelling of non-subscribed contacts will not produce error callbacks.
     * @param aLinkArray The contact links.
     */
    virtual void CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray ) = 0;

    /**
     * Cancel the specific operation. Either GetPresenceIconFileL or GetPresenceInfoL.
     * MContactPresenceObs is NOT called but the operations are cancelled silently.
     * @param aOpId Operation id to identify the operation [IN]
     */
    virtual void CancelOperation( TInt aOpId ) = 0;

    /**
     * Cancel all pending operations. 
     * MContactPresenceObs is NOT called but the operations are cancelled silently.
     */
    virtual void CancelAll() = 0;
    
    /**
     * Get contacts's presence icon info but do not subscribe for future changes.
     * Response is received in MContactPresenceObs::ReceiveIconInfosL().
     * If operation fails then the observer 
     * will be informed by a call to MContactPresenceObs::ErrorOccured().
     *
     * @param aPackedLink a packed contact link for phonebook contact
     * @return Operation ID     
     */
    virtual TInt GetPresenceInfoL( const TDesC8& aPackedLink ) = 0;        

    };


#endif // MCONTACTPRESENCE_H
