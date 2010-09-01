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
*     Adapts data from Shared data GPRS counters to listbox
*
*/


#ifndef __Logs_App_GprsCtAdapter_H__
#define __Logs_App_GprsCtAdapter_H__

//  INCLUDES
#include <bamdesca.h>  // MDesCArray
#include <e32base.h>

#include "LogsConstants.hrh"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsSharedData;

// CLASS DECLARATION

/**
*  Adapts data from Shared data GPRS counters to listbox
*/
class CLogsGprsCtAdapter : public CBase, public MDesCArray
    {
    enum TGprsDirectionType
        {
        EGprsDirectionSent = 0,
        EGprsDirectionReceived
        };

    public:  // Constructors and destructor

        /**
         *  Two-phased constructor.
         *
         *  @param aSharedData shared data
         */
        static CLogsGprsCtAdapter* NewL( MLogsSharedData* aSharedData );

        /**
         *  Destructor.
         */
        virtual ~CLogsGprsCtAdapter();

    private:        
        /**
         *  C++ default constructor.
         *
         *  @param aSharedData shared data
         */
        CLogsGprsCtAdapter( MLogsSharedData* aSharedData );

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
        
    private:

        /**
		 * Construct GPRS connection amount descriptor
         *
         * @param aDirection in / out direction 
		 */
        void ConstructAmountDescriptor( TGprsDirectionType aDirection ) const;

        /**
		 * Construct GPRS connection amount second phase
         *
         * @param aDes descriptor to put result into
         * @param aDividend dividend
         * @param aDivisor divisor
		 */
        void CalcPercentage( TInt64 aDividend,
                                        TInt64 aDivisor ) const;
	
	private: //data

        /// Ref: shared data
        MLogsSharedData* iSharedData;

		/// Own: Conversion buffer 
		TBuf<KLogsBuff128> iBuffer;

        /// Own: Sent title
        HBufC* iSentTitle;

        /// Own: Received title
        HBufC* iReceivedTitle;

        /// Own: bytes text
        HBufC* iBytes;

        /// Own: kilobytes text
        HBufC* iKBytes;

        /// Own: megabytes text
        HBufC* iMBytes;

        /// Own: gigabytes text
        HBufC* iGBytes;

    };

#endif  // __Logs_App_GprsCtAdapter_H__
           
// End of File
