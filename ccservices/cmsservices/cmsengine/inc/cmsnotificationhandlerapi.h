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
* Description:  Presence in pbonebook
*
*/


#ifndef __CMSNOTIFICATIONHANDLERAPI_H__
#define __CMSNOTIFICATIONHANDLERAPI_H__

#include <e32std.h>
#include "cmscontactfielditem.h"

enum TCmsPhonebookEvent
    {
    ECmsContactModified = 0,
    ECmsContactDeleted        
    };

class CCmsContactField;


/**
 * MCmsNotificationHandlerAPI
 *
 * Presence in phonebook
 *
 * @lib cmsclient
 * @since s60 v5.0
 */
class MCmsNotificationHandlerAPI
    {
public:
    
    /**
    * Get the subscribed data.
    * @param aField contact field, ownership is transferred to callee.
    */
    virtual void HandlePresenceNotificationL( CCmsContactField* aField ) = 0;
    
    /**
    * Get a notification of a change in the status of a contact
    * @param TPhonebookEvent The type of the notification
    */
    virtual void HandlePhonebookNotificationL( TCmsPhonebookEvent aNotificationType ) = 0;
    
    /**
    * Subscription is terminated.
    * @param aStatus error code
    * @param aNotificationType type of subscritpion    
    */    
    virtual void CmsNotificationTerminatedL( TInt aStatus, 
        CCmsContactFieldItem::TCmsContactNotification aNotificationType ) = 0; 

    };

#endif // __CMSNOTIFICATIONHANDLERAPI_H__


