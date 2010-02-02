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
*     Includes AoC's and CT's commonly needed functions. Multimode ETEL version.
*
*/


#ifndef __CLogsAocUtil_H
#define __CLogsAocUtil_H

//  INCLUDE FILES  
#include <etel.h>
#include <etelmm.h>
#include <e32base.h>
#include "CLogsEtelBase.h"
#include "MLogsAocUtil.h"

//  FORWARD DECLARATIONS 

//  CLASS DECLARATIONS 

/**
 *  Includes AoC's and CT's commonly needed functions
 */
class CLogsAocUtil: public CLogsEtelBase, public MLogsAocUtil
    {
    public:
        /**
         *  Standard creation function. Creates and returns a new object of
         *  this class.
         *
         *  @return The pointer of the CLogsAocUtil
         */
        static CLogsAocUtil* NewL( );

        /**
         *  Destructor
         */
        ~CLogsAocUtil();

	private:
		/**
         *  Symbian OS Constructor
         *
         */
        void ConstructL( );

        /**
         *  Constructor
         *
         */
        CLogsAocUtil( );

    public:
        /**
         *  Asks security code
         *
         *  @return error code
         */
        TInt AskSecCodeL();

        /**
         * Get active call duration
         * @param aTime duration is set here.
         *
         * @return KErrNone or ETEL error code in error.
         */
        TInt GetAirTimeDuration( TTimeIntervalSeconds& aTime );
    
    private: // CActive
        void RunL();
        void DoCancel();
        
    private:
        // connection is done
        TBool                   iEtelConnectOK;

        /// Telephone servers pointer
        RTelServer              iServer;

         /// Own: sync. helper in async. requests
        CActiveSchedulerWait    iWait;    

        // Own: async cancel
        TInt                    iCancelStatus;
    };

#endif  // __CLogsAocUtil_H

// End of file
