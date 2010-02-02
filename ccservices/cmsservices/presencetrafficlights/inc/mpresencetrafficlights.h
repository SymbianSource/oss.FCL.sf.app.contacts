/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presence traffic lights subscription API
*
*/

#ifndef MPRESENCETRAFFICLIGHTS_H
#define MPRESENCETRAFFICLIGHTS_H

#include <e32std.h>
#include <badesca.h>

class MVPbkContactLink;
class MPresenceIconNotifier;

/**
 * Interface for Presence traffic lights subscription API.
 *
 * @since S60 v3.2
 */
class MPresenceTrafficLights
    {
public:


    /**
     * Factory method to instantiate MPresenceTrafficLights
     *
     * @return The new MPresenceTrafficLights object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceTrafficLights* NewL( );

    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */
    virtual ~MPresenceTrafficLights() {};
    
public:
    /**
     * Subscribe common presence icon for a contact
     * 
     * @param aContact contact to be subscribed.
     * @param aIdentites fields of the contact to be scubscribed
     * @param aCallback callback observer 
     * @param aId is of the request used in callback method
     */
    virtual void SubscribeBrandingForContactL( 
        MVPbkContactLink* aContact, 
        MDesC16Array* aIdentities, 
        MPresenceIconNotifier* aCallback,
        TInt aId ) = 0;
        
    /**
     * Unsubscribe 
     * 
     * @param aContact contact to be unsubscribed
     */
    virtual void UnsubscribeBrandingForContact( 
        MVPbkContactLink* aContact, 
        MPresenceIconNotifier* aCallback ) = 0;   
 
    /**
     * Get all services branded presence icons for a contact 
     * 
     * @param aContact contact to be subscribed.
     * @param aIdentites fields of the contact to be scubscribed
     * @param aCallback callback observer 
     * @param aId is of the request used in callback method
     */
    virtual void GetAllBrandingsForContactL( 
        MVPbkContactLink* aContact, 
        MDesC16Array* aIdentities, 
        MPresenceIconNotifier* aCallback,
        TInt aId ) = 0;    
    };
    
#endif // MPRESENCETRAFFICLIGHTS_H



