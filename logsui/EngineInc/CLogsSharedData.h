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
*     Implements interface MLogsSharedData
*
*/


#ifndef __Logs_Engine_CLogsSharedData_H__
#define __Logs_Engine_CLogsSharedData_H__

//  INCLUDES


#include <centralrepository.h>      
#include <cenrepnotifyhandler.h>    



#include <e32std.h>
#include "MLogsSharedData.h"
#include "MLogsStateHolder.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsSharedDataListener;

// CLASS DECLARATION

/**
*  Implements interface MLogsSharedData
*/
class CLogsSharedData :	public CBase,
                        public MLogsSharedData,
                        public MLogsStateHolder
    {
    public:  // Constructors and destructor
       /**
        * Two-phased constructor.
        *
        * @param aHandler pointer to handler
        * @return pointer to new object
        */
        static CLogsSharedData* NewL();

       /**
        * Destructor.
        */
        virtual ~CLogsSharedData();

    private:
        /**
         * Second phase constructor.
         */
        void ConstructL();

        /**
         * C++ constructor.
         *
         * @param aHandler pointer to handler
         */
        CLogsSharedData();

    public: // from MLogsSharedData
        /**
         *  Returns whether MSK is enabled in platform
         *
         *  @return MSKEnabled true/false
         */            
        TBool IsMSKEnabledInPlatform();
         /**
         *  Returns whether VoIP is enabled in platform
         *
         *  @return VoIPEnabled true/false
         */            
        
        /**
         *  Returns whether Logging is enabled 
         *  (Log length is != "no log")
         *
         *  @return LoggingEnabled true/false
         */        
        TBool IsLoggingEnabled();
         
        TBool IsVoIPEnabledInPlatform();   
        TInt LastCallTimer( TAlsEnum aLine, TVoipEnum aVoip ); 
        TInt DialledCallsTimer( TAlsEnum,   TVoipEnum aVoip );
        TInt ReceivedCallsTimer( TAlsEnum,  TVoipEnum aVoip );
        TInt AllCallsTimer( TAlsEnum,       TVoipEnum aVoip );
        void ClearCallTimers();
        void SetLoggingEnabled( TBool aSwitch );
        TInt64 GprsSentCounter();
        TInt64 GprsReceivedCounter();
        void ClearGprsCounters();
        TBool ShowCallDurationLogsL();    
        TBool ShowRingDuration() const;     //For ring duation feature
        
        /*******************************************************************************
        FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.
 
        TInt ToolbarVisibility() const;
        void SetToolbarVisibility( TInt aToolbarVisibility );   
        *******************************************************************************/     
        
        TInt NewMissedCalls( const TBool aClear = EFalse );
        void SetObserver( MLogsObserver* aObserver );

        //Callbacks needed by CLogsSharedDataListener listerner objects
        void HandleNotifyGeneric( TUid aCategory, TUint32 aId );
        void HandleNotifyInt( TUid aCategory, TUint32 aKey, TInt aNewValue);
        
    public: // from MLogsStateHolder

        TLogsState State() const;
		
    private:
    
        /**
         *  Informs observer when contents of a central repository key has changed
         *
         *  @param aCategory Central key category id 
         *  @param aKey Central repository key 
         */
        void InformObserver( TUid aCategory, TUint32 aKey );

        /**
         *  Checks from central repository whether data call numbers should be shown to
         *  users
         */
        void CheckShowCsdNumberStatusL();                

//For ring duation feature
        /**
         *  Checks from central repository whether ring duration for missed calls
         *  should be shown to users
         */
        void CheckShowRingDurationL();  
         
         /**
         *  Checks from central repository whether MSK is enabled and set
         *  iMSKEnabledInPlatform
         */             
         void CheckMSKEnabledInPlatformL();
         
         /**
         *  Checks from central repository whether VoIP is enabled and set
         *  iVoIPEnabledInPlatform
         */             
         void CheckVoIPEnabledInPlatform();  
         
    private:    // Data
    
    
        /// Own: Central repository
        CRepository* iRepository_Logs;
        CRepository* iRepository_Dcl;        
        CRepository* iRepository_Voip;                
        
        /// Own: Central repository notification handlers        
        CLogsSharedDataListener* iNotifyHandler_Logs;                        
        CLogsSharedDataListener* iNotifyHandler_Dcl;                                
        CLogsSharedDataListener* iNotifyHandler_Voip;                                        
       
        //For ring duation feature
        TBool iShowRingDuration;
       
        // For checking wether the MSK is supported in platform or not
        TBool iMSKEnabledInPlatform;   

        // For checking if the VoIP is supported in platform or not
        TBool iVoIPEnabledInPlatform;   
            
        /// Own: observer
        MLogsObserver*      iObserver;
		
        /// own: state for observer
        TLogsState      iState;  
   
    };

#endif  // __Logs_Engine_CLogsSharedData_H__

// End of File
