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
* Description:  Presence of contact in phonebook.
*
*/


#ifndef MCONTACTPRESENCEINFO_H
#define MCONTACTPRESENCEINFO_H

#include <e32std.h>


/**
 * MContactPresenceInfo contains presence information for one service.  
 *
 * @lib contactpresence.lib
 * @since s60 v5.0
 */
class MContactPresenceInfo
{
public:
    
    /**
     * Brand id accessor
     * @return   Brand id in branding server
     */      
    virtual TPtrC8 BrandId() = 0;
    
    /**
     * Element id accessor
     * @return Element id in branding server
     */     
    virtual TPtrC8 ElementId() = 0;
    
    /**
     * Service Name accessor
     * @return  Service Name
     */     
    virtual TPtrC ServiceName() = 0;
};



#endif // MCONTACTPRESENCEINFO_H
