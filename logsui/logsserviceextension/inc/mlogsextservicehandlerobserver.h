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
* Description:  Observer of the service handler.
*
*/



#ifndef M_MLOGSEXTSERVICEHANDLEROBSERVER_H
#define M_MLOGSEXTSERVICEHANDLEROBSERVER_H


/**
 * Interface to notify service handler observers.
 *
 * @lib logsserviceextension.dll
 * @since S60 v3.2
 */
class MLogsExtServiceHandlerObserver
    {
    
    public: 
        /**
         * Handles the service handler's callback.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         * @param aPresentityId presentity id
         * @param aPresenceStatus a presence status
         * @param aShowStatus presentity presense status display flag         
         */
        virtual void HandlePresentityPresenceStatusL( 
                            const TUint32 aServiceId, 
                            const TDesC& aPresentityId,
                            const TDesC& aPresenceStatus,
                            TBool aShowStatus ) = 0;

        /**
         * This function is used to notify the observer of a service handler
         * if the presence status of a certain presentity should be displayed
         * or not.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         * @param aPresentityId presentity id
         * @param aShowStatus presentity presense status display flag
         */
        virtual void SetDisplayStatusFlag( const TUint32 aServiceId, 
                                           const TDesC& aPresentityId,
                                           TBool aShowStatus ) = 0;
        
    };

#endif // M_MLOGSEXTSERVICEHANDLEROBSERVER_H
