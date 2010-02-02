/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CCACONNECTIONMONITOR_H
#define CCACONNECTIONMONITOR_H

#include <e32std.h>
#include <e32base.h>
#include <aknserverapp.h>
#include <apparc.h>
#include "mccaconnection.h"
#include "ccaclientuiservice.h"

// FORWARD DECLARATION


/**
 *  CCCAConnectionMonitor
 *
 *  CCA client connection observer
 *
 *  @lib ccaclient
 *  @since S60 3.2
 */
class CCCAConnectionMonitor : CBase

    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 3.2
     * @return CCCAConnectionMonitor
     */
    static CCCAConnectionMonitor* NewL(RCCAClientUIService& aClient,
            MApaServerAppExitObserver& aObserver, TInt aPriority);

    /**
     * Destructor.
     */
    virtual ~CCCAConnectionMonitor();

private:
    // From MAknServerAppExitObserver
    void HandleServerAppExit(TInt aReason);

private:
    // Own implement

    /**
     * Two-phase constructor
     */
    void ConstructL(RApaAppServiceBase& aClient,
            MApaServerAppExitObserver& aObserver, TInt aPriority);

    /**
     * constructor
     */
    CCCAConnectionMonitor();

private:
    // data
    /// Own: Exit application server monitor
    CApaServerAppExitMonitor* iExitAppServerMonitor;

    };

#endif // CCACONNECTIONMONITOR_H
// End of File
