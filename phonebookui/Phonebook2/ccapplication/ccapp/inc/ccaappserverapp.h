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
* Description:  Implementation of server app server.
*
*/

#ifndef CCAAPPPSERVERAPP_H
#define CCAAPPPSERVERAPP_H

#include <AknServerApp.h>

/**
 *  Implementation of server app server.
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */ 
class CCAAppServerApp : public CAknAppServer
    {
    
public:

    static CCAAppServerApp* NewL();
        
private:

// from base class CAknAppServer

    /**
     * From CAknAppServer
     * (see details from header)
     *
     * @since S60 v5.0
     */        
    CApaAppServiceBase* CreateServiceL(
        TUid aServiceType ) const;

    };

#endif // CCAAPPPSERVERAPP_H
            
// End of File
