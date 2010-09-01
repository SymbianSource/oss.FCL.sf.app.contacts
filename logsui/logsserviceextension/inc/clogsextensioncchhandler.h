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
* Description:  Class handling CCH connection
*
*/


#ifndef     C_LOGSEXTENSIONCCHHANDLER_H
#define     C_LOGSEXTENSIONCCHHANDLER_H

#include <e32base.h>
#include <cchclientserver.h>

#include "mlogsextensioncchhandlerobserver.h"

class RCCHServer;
class RCCHClient;

// CLASS DECLARATION

NONSHARABLE_CLASS( CLogsExtensionCchHandler ) : public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aObserver Observer for CCH events
     * @param aServiceId, service id to monitor
     */
    static CLogsExtensionCchHandler* NewL( 
        MLogsExtensionCchHandlerObserver* aObserver,
        TUint aServiceId );

    /**
     * Two-phased constructor
     *
     * @param aObserver Observer for CCH events
     * @param aServiceId, service id to monitor
     */
    static CLogsExtensionCchHandler* NewLC( 
        MLogsExtensionCchHandlerObserver* aObserver,
        TUint aServiceId );
        
    /**
     * Standard C++ destructor
     */    
    ~CLogsExtensionCchHandler();
        
    /**
     * Resolves sub service state for given service id
     *
     * @since S60 3.2
     * @param aServiceId, service id
     * @param aCchErr, service error is stored here
     */                                    
    void ResolveSubServiceState( TUint aServiceId, TInt& aCchErr );
        
private: // From CActive

    /**
     * From CActive
     * Called when asynchronous event occures
     *
     * @since S60 3.2
     */
    void RunL();

    /**
     * From CActive
     * Called when this active object is cancelled.
     *
     * @since S60 3.2
     */
    void DoCancel();

    /**
     * From CActive
     * Asynchronous error
     *
     * @since S60 3.2
     * @param aError, error code
     * @return TInt, error 
     */
    TInt RunError( TInt aError );
    
private: // Implementation

    /**
     * Standard C++ constructor
     */    
    CLogsExtensionCchHandler();
        
    /**
     * Performs the 2nd phase of construction.
     *
     * @param aObserver, Observer
     * @param aServiceId, serviceId
     */        
    void ConstructL( 
        MLogsExtensionCchHandlerObserver* aObserver,
        TUint aServiceId );

    /**
     * Gets service state
     *
     * @since S60 3.2
     * @param aServiceSelection service selection for query
     * @param aServiceState service state (result)
     * @return TInt Standard Symbian error code
     */
    TInt GetServiceState( 
        TServiceSelection aServiceSelection, 
        TCCHSubserviceState& aServiceState );

private: // Data
    
    /** 
     * NOT OWNED
     */
    MLogsExtensionCchHandlerObserver* iObserver;

    /**
     * Own: CCH Server
     */
    RCCHServer* iCchServer;
        
    /**
     * Own: CCH Client
     */
    RCCHClient* iCchClient;
        
    /**
     * Stores service status information
     */
    TPckgBuf<TServiceStatus> iServiceStatus;
        
    /**
     * Stores service selection (ie. sub services we are
     * interested in
     */
    TServiceSelection iServiceSelection;

    };

#endif // C_LOGSEXTENSIONCCHHANDLER_H

// End of file
