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
*     Adaptor to EventList, copies data from model listbox
*
*/


#ifndef __CLogsCtAdapter_H__
#define __CLogsCtAdapter_H__

//  INCLUDES
#include <bamdesca.h>  // MDesCArray
#include <e32base.h>

#include "LogsAlsEnum.hrh"

// CONSTANTS
const TInt KLogsCtAdapterLen = 100;
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsSharedData;
class MLogsCallStatus;

// CLASS DECLARATION

/**
*  Adaptor to EventList, copies data from model listbox
*/
class CLogsCtAdapter : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         *  Two-phased constructor.
         *
         * @param aModel, pointer to logs model.
         * @param aSelectedAlsLine, selected als line.
         * @param aCallStatus, call status accessor class.
         */
        static CLogsCtAdapter* NewL( MLogsSharedData* aSharedData, 
                            const TAlsEnum& aSelectedAlsLine,
                            MLogsCallStatus& aCallStatus );

        /**
         *  Destructor.
         */
        virtual ~CLogsCtAdapter();

    public: 
        /**
         * Update data for adaptation
         */
        TBool UpdateDataL();

        /**
         * Get time strings in localised form for recent call counters. UpdateDataL() needs
         * to be called before to get up-to-date values
         */
        TInt LastCallTimeStringL( TDes& aDes, const TAlsEnum& aSelectedAlsLine, const TVoipEnum aVoip );
        TInt ReceivedCallsTimeStringL( TDes& aDes, const TAlsEnum& aSelectedAlsLine, const TVoipEnum aVoip);        
        TInt DialledCallsTimeStringL( TDes& aDes, const TAlsEnum& aSelectedAlsLine, const TVoipEnum aVoip );                
        TInt AllCallsTimeStringL( TDes& aDes, const TAlsEnum& aSelectedAlsLine, const TVoipEnum aVoip );                        
        

    private:
        /** 
         * Active call timer in seconds.
         */
        TInt ActiveTime();

        /**
         * Converts seconds to proper time format
         * @param aSeconds time in seconds
         * @param aText converted string
         */
        void ToTimeFormat( TInt aSeconds, HBufC* aText );

    private:        
        /**
         *  C++ default constructor.
         */
        CLogsCtAdapter( MLogsSharedData* aSharedData, 
                            const TAlsEnum& aSelectedAlsLine,
                            MLogsCallStatus& aCallStatus);

        /**
         *  Symbian OS constructor.
         */
        void ConstructL();

	private: // from MDesCArray

		/**
		 *  Returns number of entries
         *
		 *  @return TInt 
		 */
		TInt MdcaCount() const;
	
		/**
		 *  Returns a TPtrC related with the position given by aIndex
         *
		 *  @param  TInt    aIndex
		 *  @return TPtrC16
		 */
		TPtrC16 MdcaPoint( TInt aIndex ) const;        
	
	private: //data

        /// Ref: Reference to SD
        MLogsSharedData* iSharedData;

        /// Own: main buffer 
		TBuf<KLogsCtAdapterLen> iBuffer;

        /// Own: "Active call timer" text
        HBufC* iActiveCallText;

        /// Own: "Last call timer" text
        HBufC* iLastCallText;

        /// Own: "Dialled call timer" text
        HBufC* iDialledCallsText;

        /// Own: "Received call timer" text
        HBufC* iReceivedCallsText;

        /// Own: "All call timer" text
        HBufC* iAllCallsText;

        /// Own: active call timer conversion buffer
        HBufC* iActiveCallTime; 

        /// Own: last call timer conversion buffer
        HBufC* iLastCallTime; 

        /// Own: dialled call timer conversion buffer
        HBufC* iDialledCallsTime; 

        /// Own: received call timer conversion buffer
        HBufC* iReceivedCallsTime;

        /// Own: all call timer conversion buffer
        HBufC* iAllCallsTime; 

        /// Ref: Selected als line
        const TAlsEnum& iSelectedAlsLine;

        /// Ref: call status object reference.
        MLogsCallStatus& iCallStatus;

        //Members to track whether a time value has changed from previous call
        TInt iPreviousActiveTime;
        TInt iPreviousAllCallsTime;

        TBool iShowCallDurationLogs;
    };

#endif  // CLogsCtAdapter_H
           
// End of File
