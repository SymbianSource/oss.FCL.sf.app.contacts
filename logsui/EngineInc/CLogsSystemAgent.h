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
*     Implements the functionality of the System Agent observer
*
*/


#ifndef __Logs_Engine_Clogs_System_Agent_H__
#define __Logs_Engine_Clogs_System_Agent_H__

//  INCLUDE FILES  
#include <e32property.h>
#include <RSSSettings.h>        //Supplementary Services Settings
#include "MLogsSystemAgent.h"
#include "MLogsStateHolder.h"
#include "LogsEng.hrh"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsCallObserver;
// CLASS DECLARATION

/**
 * Implements the functionality of the System Agent observer
 */
class CLogsSystemAgent :    public CActive,
                            public MLogsSystemAgent 
    {
    public:  // Constructor and destructors
        /**
         *  Two-phased constructor
         *
         *  @param aObserver interface for notifications
         *  @return pointer to new object
         */
        static CLogsSystemAgent* NewL( MLogsObserver* aObserver,
                                       MLogsCallObserver* aCallObserver );

        /**
         *  Destructor
         */
        virtual ~CLogsSystemAgent();

    protected: /// from CActive
        void RunL();
        void DoCancel();

    private:    // Constructors.

        /**
         *  Second phase constructor
         */
        void ConstructL();

        /**
         *  Symbian OS Default constructor
         *  @param aObserver interface for notifications
         */
        CLogsSystemAgent( MLogsObserver* aObserver,
                          MLogsCallObserver* aCallObserver );
        
        /**
         *  Is network in use Gprs or Wcdma
         *  @return true / false
         */
        TBool IsGprs();
   
    public: // from MLogsSystemAgent
       
        TBool AlsSupport();        
        TBool GprsConnectionActive();
        // TInt  CallActive( TBool &aValue );  
        void SetObserver( MLogsObserver* aObserver );
        void ResetNewMissedPocCalls(); //Reset missed Poc call notification
        // void SetCallObserver( MLogsCallObserver* aCallObserver );
      
    private:

    private:    // Data
    
        //Handles to Publish and subscribe
        RProperty iPropertyGprs;
        RProperty iPropertyWcdma;        
        // RProperty iPropertyCallHandling;        
        // RProperty iPropertyCallStatus;        

        /// Ref: observer
        MLogsObserver* iObserver ;
        MLogsCallObserver* iCallObserver ;        

        //Handle to Supplementary Services Settings (Als support etc)
        RSSSettings iSSSettings;
    };

#endif // __Logs_Engine_Clogs_System_Agent_H__
          

// End of File
