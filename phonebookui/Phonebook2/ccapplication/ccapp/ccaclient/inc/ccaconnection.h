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
* Description:  CCA client's connecting class.
*
*/


#ifndef C_CCACONNECTION_H
#define C_CCACONNECTION_H

#include <e32base.h>
#include <mccaconnection.h>
#include <mccaconnectionext.h>

/**
 *  CCCAConnection
 *  CCA client's core connection class.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class CCCAConnection : 	public CActive,
                        public MCCAConnectionExt, 
                        private MAknServerAppExitObserver
    {
#ifdef __CCACLIENTUNITTESTMODE
    friend class ut_CCAConnection;
#endif// __CCACLIENTUNITTESTMODE

public:

    /**
     * Two-phased constructor.
     */
    static CCCAConnection* NewL( );

    /**
    * Destructor.
    */
    virtual ~CCCAConnection();


public:

// from base class CActive

    /**
     * From CActive.
     * (see baseclass for details)
     *
     * @since S60 v5.0
     */
    void RunL();

    /**
     * From CActive.
     * (see baseclass for details)
     *
     * @since S60 v5.0
     */
    void DoCancel();

// from base class MCCAConnection

    /**
     * From MCCAConnection
     * (see details from header)
     *
     * @since S60 v5.0
     */
    void Close();

    /**
     * From MCCAConnection
     * (see details from header)
     *
     * @since S60 v5.0
     */
    void LaunchApplicationL( MCCAParameter& aParameter );

    /**
     * From MCCAConnection
     * (see details from header)
     *
     * @since S60 v5.0
     */
    void LaunchAppL(
            MCCAParameter& aParameter,
            MCCAObserver* aObserver = NULL );

    /**
     * From MCCAConnectionExt
     * (see details from header)
     *
     * @since S60 v5.0
     */	
    void CloseAppL();

private:

// from base class MAknServerAppExitObserver

    /**
     * From MAknServerAppExitObserver.
     * (see baseclass for details)
     *
     * @since S60 v5.0
     */
    void HandleServerAppExit( TInt aReason );

private:

    CCCAConnection();
    void ConstructL();

    /**
     * Makes connection to service class.
     */
    void ConnectL();

    /**
     * Close down service connection.
     */
    void DisconnectServerApplication();

    /**
    * Do service connection
    */
    void ConnectServerApplicationL();

    /**
    * Do launch
    */
    void DoLaunchL(MCCAParameter& aParameter);

    /**
    * Clean iParameter
    */
    void CloseParameter();
	
private: // Data structures
    /// Application launching status
    enum TAppStatus
            {
            EAppClosed,
            EAppLaunching,
            EAppRunning
            };

private: // data

    /**
     * Application server service.
     * Own.
     */
    RCCAClientUIService iAppService;

    /**
     * Exit application server monitor.
     * Own.
     */
    CApaServerAppExitMonitor* iMonitor;

    /**
     * Launch parameter including contact link etc.
     * Own.
     */
    CCCAParameter* iParameter;

    /**
     * Observer to indicate happenings.
     * Not own.
     */
    MCCAObserver* iObserver;

    /**
     * The status of connection.
     * Own.
     */
    TAppStatus iAppStatus;

    /**
     * For making the asyncronic syncronic.
     * Own.
     */
    CActiveSchedulerWait* iSchedulerWait;

    /**
     * Stream buffer for launch.
     * Own.
     */
    HBufC8* iMsgStream;
    
    /**
     * Ptr to the Stream buffer iMsgStream     
     */
    TPtr8 iMsgStreamPtr;

    /**
     * Boolean to protect from attempts to close CCA twice from
	 * external client     
     */
	TBool iClosePending;
    };

#endif // C_CCACONNECTION_H

// End of File
