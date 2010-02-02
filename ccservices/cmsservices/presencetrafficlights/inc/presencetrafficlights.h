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
* Description:  
*
*/


#ifndef PRESENCETRAFFICLIGHTS_H_
#define PRESENCETRAFFICLIGHTS_H_

#include <e32base.h>
#include <bamdesca.h>
#include <badesca.h>

#include "cmsonecontactstatus.h"
#include "mpresencetrafficlights.h"

class MPresenceIconNotifier;
class MVPbkContactLink;
class CCmsOneContactStatus;

/**
 *  Branded presence icons handling class.
 *
 *  @lib cmsserver.exe
 *  @since S60 v5.0
 */
class CPresenceTrafficLights: public CBase, public MPresenceTrafficLights
    {

public: 

    /**
     * Symbian constructors.
     *
     * @param aCallback callback method for notifications
     * @return CCmsBrandedIcons
     */
    static CPresenceTrafficLights* NewL( );

    /**
     * Public destructor. 
     */
    ~CPresenceTrafficLights();
    
// new methods

#ifdef _DEBUG    
    static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );
#endif  
    
// from base class MPresenceTrafficLights
    
    /**
     * Defined in a base class
     */
    void SubscribeBrandingForContactL( 
        MVPbkContactLink* aContact , 
        MDesC16Array* aIdentities, 
        MPresenceIconNotifier* aCallback, 
        TInt aId );
    
    /**
     * Defined in a base class
     */
    void UnsubscribeBrandingForContact( 
        MVPbkContactLink* aContact, MPresenceIconNotifier* aCallback );
    
    /**
     * Defined in a base class
     */    
    virtual void GetAllBrandingsForContactL( 
        MVPbkContactLink* aContact, 
        MDesC16Array* aIdentities, 
        MPresenceIconNotifier* aCallback,
        TInt aId );     
                
private:
 
    CPresenceTrafficLights( );
    
    void ConstructL(
        const TDesC8& aDefaultTrafficLightBrandId, 
        const TDesC8& aDefaultOnlineElementId, 
        const TDesC8& aDefaultOfflineElementId );
    
// new methods
    
   /**
    * Search one contact status
    * @param aContact contact
    * @param aCallback callback observer
    * @return contact status, NULL if not found
    */ 
   CCmsOneContactStatus* FindOne( MVPbkContactLink* aContact, MPresenceIconNotifier* aCallback );
      
    
    
private:

    /**
     * NOT OWN
     */
    HBufC8* iDefaultTrafficLightBrandId;
    HBufC8* iDefaultOnlineElementId;
    HBufC8* iDefaultOfflineElementId;
    
    RPointerArray<CCmsOneContactStatus> iStatuses;
    };


#endif /* PRESENCETRAFFICLIGHTS_H_*/
