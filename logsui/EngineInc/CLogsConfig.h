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
*     Log configuration setter / getter
*
*/


#ifndef __Logs_Engine_CLogsConfig_H__
#define __Logs_Engine_CLogsConfig_H__

// INCLUDES
#include <e32base.h> 






#include "MLogsConfig.h"
#include "MLogsStateHolder.h"

// FORWARD DECLARATIONS
class CLogClient;
class MLogsObserver;

// CLASS DECLARATION

/**
 * Log configuration setter / getter
 */
class CLogsConfig : public CActive, public MLogsConfig,
                    public MLogsStateHolder                   
    {
    public:
        /**
         * Standard creation function.
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer. Default NULL
         * @return New instance of this class
         */
        static CLogsConfig* NewL( RFs& aFsSession, MLogsObserver* aObserver = NULL );

        /**
         * Destructor.
         */
        ~CLogsConfig();
        
    private:
        /**
         * Default constructor.
         */
        CLogsConfig();

        /**
         * Constructor, second phase 
         */
        void ConstructL();

        /**
         * Constructor
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer
         */
        CLogsConfig( RFs& aFsSession, MLogsObserver* aObserver );
       
    
    protected: // from CActive
       
        void RunL();
		TInt RunError(TInt aError);
        void DoCancel();

    public: // from MLogsConfig
        
        void SetConfig( const TLogConfig& aConfig );        
        void GetConfig( TLogConfig& aConfig );        
        void SetObserver( MLogsObserver* aObserver ); 

    public: // from MLogsStateHolder

        TLogsState State() const;

    private: // data

        /// Ref: File server session
        RFs& iFsSession;
                
        /// Own: Log client
        CLogClient* iLogClient;
        
        /// Ref: Observer
        MLogsObserver* iObserver;        

        /// Own: state
        TLogsState iState;
    };

#endif // __Logs_Engine_CLogsConfig_H__

// End of File
