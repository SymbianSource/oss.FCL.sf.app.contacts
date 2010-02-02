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
*     Includes AoC's and CT's commonly needed functions
*
*/


#ifndef __MLogsAocUtil_H
#define __MLogsAocUtil_H

//  INCLUDE FILES  

//  CLASS DECLARATIONS 

/**
 *  Abstract clss for Aoc utility functions.
 */
class MLogsAocUtil 
    {
    public:

        /**
         * Destructor.
         */
        virtual ~MLogsAocUtil() {};

        /**
         *  Clears all the Costs
         *
         *  Return value: KErrNone if timers are cleared, Otherwice error 
         *          code from ETEL
         */
        //virtual TInt ClearAllCostsL() = 0;

        /**
         *  Asks security code
         *
         *  Return value: error code
         */
        virtual TInt AskSecCodeL() = 0;

        /**
         *  This function sets the value of the ACMM.
         *  This function depends of the ETel. Units version
         *
         *  Parameter:  aAcmMax The value of the ACMM
         *  Return value: KErrNone for succesfull call, otherwise 
         *          ETEL error code.
         */
        //virtual TInt SetAcmMaxUnitL( const TUint aAcmMax ) = 0;

        /**
         *  This function sets the value of the ACMM.
         *  This function depends of the ETel. Currency version
         *
         *  Parameter:  aAcmMax The value of the ACMM
         *  Return value: value, KErrNone for succesfull call,otherwise 
         *          ETEL error code.
         */
        //virtual TInt SetAcmMaxCurrencyL( const TReal aAcmMax ) = 0;

        /**
         *  With this function is set the currency name and the price per unit
         *  in to the SIM. This depends on the ETel.
         *  The currency name can only be 3 characters long.
         *  The price per unit is TReal.
         *
         *  Parameter:  aCurrencyName   The text to be set as a currency name
         *  Parameter:  aPricePerUnit   The price per one unit
         *  Return value: KErrNone for succesfull call,otherwise 
         *          ETEL error code.
         */
        //virtual TInt SetPuctL( TDesC& aCurrencyName, TReal aPricePerUnit ) = 0;

        /**
         *  Disable ACM max.
         *
         *  Return value: KErrNone for succesfull
         *          call, ETEL error code in error.
         */
        //virtual TInt DisableACMMaxL() = 0;

        /**
         *  Is aoc (charging or information supported)
         *
         *  Parameter: aValue, 0 no support
         *					 , 1 Information service
         *					 , 3 Charging service
         *  Return value: KErrNone if succesfull or ETEL error code in error.
         */
        //virtual TInt AocSupported( TInt& aValue ) = 0;

        /**
         *  Disable puct. Set aoc server to unit mode.
         *
         *  Return value: KErrNone or ETEL error code in error.
         */
        //virtual TInt DisablePuctL() = 0;

        /**
         * Get active call duration
         * Parameter: aTime duration is set here.
         *
         * Return value: KErrNone or ETEL error code in error.
         */
        virtual TInt GetAirTimeDuration( TTimeIntervalSeconds& aTime ) = 0;



    };

#endif  // __MLogsAocUtil_H

// End of file
