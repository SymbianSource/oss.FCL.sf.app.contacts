/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Abstract class for Call status notifier
*
*/


#ifndef __MLogsCallStatus_H
#define __MLogsCallStatus_H

//  INCLUDE FILES  

// CONSTANTS

// FORWARD DECLARATIONS
class MLineStatusObserver;

//  CLASS DECLARATIONS 



class MLogsCallStatus
    {
    public:
        
        /**
         * Destructor.
         */
        virtual ~MLogsCallStatus() {};        
        
        /**
         * Call/line status observer
         *
         * Parameter: pointer to observer, if NULL then connection to ETel is closed
         *
         */
        virtual void SetObserver( MLineStatusObserver* aObserver ) = 0;

        /**
         * Call status, initiate observer first with LineStatusObserver!
         *
         * Parameter: aCallIsActive, set to ETrue if call is active, else EFalse
         *
         * Return value: KErrNone if call is succesfull, or ETEL error code
         */
        virtual TInt CallIsActive(TBool& aCallIsActive) = 0;
        
        /**
         * Active call duration.
         *
         * @param aCallDuration, call duration if active, otherwise will return error code.
         * Return value: KErrNone if call is active otherwise error code.
         */
        virtual TInt ActiveCallDuration( TTimeIntervalSeconds& aCallDuration ) = 0;

    };

#endif  // __MLogsCallStatus_H

// End of file
