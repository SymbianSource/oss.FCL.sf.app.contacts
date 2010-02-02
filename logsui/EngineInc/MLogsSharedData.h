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
*     Defines abstract interface for accessing Logs SD
*
*/


#ifndef __MLogsSharedData_H_
#define __MLogsSharedData_H_

// INCLUDES
#include "LogsEng.hrh"
#include "LogsAlsEnum.hrh"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsObserver;

// CLASS DECLARATION

/**
 *	Defines abstract interface for accessing Logs SD
 */
class MLogsSharedData
    {
    public:
    
        virtual ~MLogsSharedData() {};

    public:
        
        /**
         *  Last Call Timer's value getter. 
         *
         *  @param  aLine   Line to fetch, default EAllLines
         *  @param  aVoip   Whether or voip line is to be fetched
         *  @return Last Call Timer value
         */
        virtual TInt LastCallTimer( TAlsEnum aLine, TVoipEnum aVoip ) = 0; 

        /**
         *  Dialled Call Timer's value getter
         *
         *  @param  aLine   Line, to fetch
         *  @param  aVoip   Whether or voip line is to be included 
         *  @return Dialled Call Timer value
         */
        virtual TInt DialledCallsTimer( TAlsEnum aLine, TVoipEnum aVoip ) = 0;

        /**
         *  Received Call Timer's value getter
         *
         *  @param  aLine   Line to fetch
         *  @param  aVoip   Whether or voip line is to be included          
         *  @return Received Call Timer value
         */
        virtual TInt ReceivedCallsTimer( TAlsEnum aLine, TVoipEnum aVoip )  = 0;

        /**
         *  All Calls Timer's value getter, 
         *
         *  @param  aLine   Line to fetch
         *  @param  aVoip   Whether or voip line is to be included          
         *  @return All Calls Timer value
         */
        virtual TInt AllCallsTimer( TAlsEnum aLine, TVoipEnum aVoip ) = 0;

        /**
         *  Clears Call Timers
         */
        virtual void ClearCallTimers() = 0;
        
        /**
         *  Returns whether Logging is enabled 
         *  (Log length is != "no log")
         *
         *  @return LoggingEnabled true/false
         */        
        virtual TBool IsLoggingEnabled() = 0;

        /**
         *  Logging ON/OFF switch
         *
         *  @param  aSwitch    ETrue - ON, EFalse - OFF
         */
        virtual void SetLoggingEnabled( TBool aSwitch ) = 0;

        /**
         *  GPRS sent value getter
         *
         *  @return The value of the GPRS sent counter
         */
        virtual TInt64 GprsSentCounter() = 0;

        /**
         *  GPRS received value getter
         *
         *  @return The value of the GPRS received counter
         */
        virtual TInt64 GprsReceivedCounter() = 0;

        /**
         *  Clears GPRS counters
         */
        virtual void ClearGprsCounters() = 0;

        /**
         *  Flag getter for Show call duration in Logs application.
         *
         *  @return ETrue if call duration is to be shown,
         *          otherwise EFalse
         */
        virtual TBool ShowCallDurationLogsL() = 0;    

        //For ring duation feature
        /**
         *  Show Ring duration for missed calls
         *  @return ETrue if value must be shown,
         *          EFalse otherwise.
         */        
        virtual TBool ShowRingDuration() const = 0;
        
        /*******************************************************************************
        FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.*/
  
        /**
         *  Show Toolbar status getter
         *  @return 0   Don't show toolbar.
         *          !0  Show toolbar.
         */        
        //virtual TInt ToolbarVisibility() const = 0;

        /**
         *  Show Toolbar status setter
         *  @param aShow !0 (use 1) Show toolbar,
         *               0  Don't show toolbar.
         */        
         //virtual void SetToolbarVisibility( TInt aToolbarVisibility ) = 0; 
          
        /*******************************************************************************/
     
        
        /**
         *  New Missed Calls getter
         *
         *  @param aClear clear counter if true, default EFalse
         *  @return Amount of new missed calls
         */
        virtual TInt NewMissedCalls( const TBool aClear = EFalse ) = 0;

        /**
         *   Set UI observer for notifications
         * 
         *   @param aObserver pointer to observer.
         */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0;
        
        /**
         *  Returns whether MSK is enabled in platform
         *
         *  @return MSKEnabled true/false
         */            
        virtual TBool IsMSKEnabledInPlatform() = 0;
         
         /**
         *  Returns whether VoIP is enabled in platform
         *
         *  @return VoIPEnabled true/false
         */            
        virtual TBool IsVoIPEnabledInPlatform() = 0;  
    };

            
#endif

// End of File  __MLogsSharedData_H_
