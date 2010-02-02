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
*     Call active notifier: either for data or voice line
*
*/


#ifndef __CLogsCallStatus_H
#define __CLogsCallStatus_H

//  INCLUDE FILES  
#include <etel.h>
#include <etelmm.h>

#include "CLogsEtelBase.h"
#include "MLogsCallStatus.h"
// CONSTANTS

// FORWARD DECLARATIONS
class MLineStatusObserver;

//  CLASS DECLARATIONS 



class CLogsCallStatus: public CLogsEtelBase, public MLogsCallStatus
    {
    public:

        enum TLineStatus
            {
            EUnknownLine,
            ECallAndDataLines,
            ECallLine,
            ESecondaryLine, // ALS secondary line
            EDataLine
            };

        /**
         *  Standard creation function. Creates and returns a new object of
         *  this class.
         *  Parameter: aLine, ETel line
         *
         *  Return value: The pointer of the CLogsCallStatus
         */
        static CLogsCallStatus* NewLC( const TLineStatus aLine );

        /**
         *  Destructor
         */
        ~CLogsCallStatus();

    private: // CActive
        void RunL();
        void DoCancel();

    private:

		/**
         *  Symbian OS Constructor
         */
        void ConstructL();

        /**
         *  C++ Constructor
         *
         */
        CLogsCallStatus( const TLineStatus aLine );

    public: // from MLogsCallStatus
        
        void SetObserver( MLineStatusObserver* aObserver );
        TInt CallIsActive( TBool& aCallIsActive );
        TInt ActiveCallDuration(TTimeIntervalSeconds& aCallDuration);
    private: // data
        /// Ref: observer
        MLineStatusObserver* iObserver;
        
        /// Own: Telephone line
        RMobileLine iLine;

        /// Own: status
        RMobileCall::TMobileCallStatus iLineStatus;

        /// Own: composite object if both the data and cellular lines are initialised.
        CLogsCallStatus* iDataLine;
        
        /// Own: composite object if also the secondary (ALS) line is initialised.
        CLogsCallStatus* iSecondaryLine;

        /// Own: voice or data call
        TLineStatus iSelectedLine;
    };

#endif  // __AOCCT_CALL_STATUS_H

// End of file
