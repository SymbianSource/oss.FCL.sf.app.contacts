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
* Description:  Presence traffic light observer API
*
*/

#ifndef MPRESENCETRAFFICLIGHTSOBS_H
#define MPRESENCETRAFFICLIGHTSOBS_H

#include <e32std.h>
#include <badesca.h>

class MVPbkContactLink;

class MPresenceServiceIconInfo
{
public:
        
    
    virtual TPtrC8 BrandId() = 0;
    
    virtual TPtrC8 ElementId() = 0;
    
    virtual TPtrC ServiceName() = 0;
    
    virtual TInt BrandLanguage() = 0;
};



/**
 * MPresenceIconNotifier
 *
 * Presence icons observer
 *
 * @lib cmsserver
 * @since s60 v5.0
 */

class MPresenceIconNotifier
{
public:

    /**
     * icon has changed for a contact, common traffic light logic here.
     * 
     * @param aLink contactlink
     * @param aBrandId brand id
     * @param aElementId element id
     * @param aId id given in SubscribeBrandingForContactL
     * @param aBrandLanguage - Brand Language ID
     */ 
    virtual void NewIconForContact( 
            MVPbkContactLink* aLink, 
            const TDesC8& aBrandId, 
            const TDesC8& aElementId,
            TInt aId,
            TInt aBrandLanguage ) = 0;
    
    /**
     * Get all icons at once, all service specific icons, no common traffic light logic here.
     * 
     * @param aLink contactlink
     * @param aInfoArray array of incons infos, ownership is not transferred
     * @param aId id given in SubscribeBrandingForContactL
     */ 
    virtual void NewIconsForContact( 
            MVPbkContactLink* aLink, 
            RPointerArray <MPresenceServiceIconInfo>& aInfoArray,
            TInt aId ) = 0;    
    
};
    
#endif // MPRESENCETRAFFICLIGHTSOBS_H



