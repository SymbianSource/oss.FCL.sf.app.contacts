/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RCCACLIENTUISERVICE_H
#define RCCACLIENTUISERVICE_H

// INCLUDES
#include <aknserverapp.h>
#include "ccaclientserverappipc.h"
//#include "ccaclientconnectionservice.h"

// CLASS DECLARATION
class MCCAParameter;

/**
 *  IPC client class for handling IPC traffic with CCA application server. 
 */
class RCCAClientUIService //: public RAknAppServiceBase
    {
public:
    // Constructor

    /**
     * Constructor.
     */
    RCCAClientUIService();

public:
    //new methods


    /**
     * Launches application service.
     *
     * @param aStatus                   Request status.
     * @param aMessagePtr               Connection parameters.
     */
    void DoLaunch(TRequestStatus& aStatus, TPtr8& aMessagePtr);
    
    /**
     * Closes application service.
     */        	
    void DoCloseApplication();    

    /**
     * Connects to application server.
     */
    void DoConnectL();

private: 
    

    /**
     * From RAknAppServiceBase.
     * (see baseclass for details)
     *
     * @since S60 5.0
     */
    TUid ServiceUid() const;
    
    
    };

#endif // RCCACLIENTUISERVICE_H
// End of File
