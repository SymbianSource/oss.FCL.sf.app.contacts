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
*     Implements interface for Logs event
*
*/


#ifndef __Logs_Engine_CLogsEngine_H__
#define __Logs_Engine_CLogsEngine_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <logcli.h>

#include "MLogsObserver.h"
#include "MLogsCallObserver.h"
#include "TLogsEventStrings.h"
#include "LogsEng.hrh"
#include "CLogsClearNewMissed.h"
#include "MLogsClearNewMissed.h"

// FORWARD DECLARATION

// CLASS DECLARATION

class MLogsModel;
class MLogsClearLog;
class MLogsStateHolder;
class MLogsReader;
//class TPreferredPbkStore;
class MLogsConfig;
class MLogsGetEvent;
class MLogsSharedData;
class MLogsSystemAgent;
class CCnvCharacterSetConverter;
class CLogsCntLinkChecker;

/**
 *  Implements entry point class to LogsEngine.dll
 */
class CLogsEngine : public CBase,
                    public MLogsObserver,
                    public MLogsCallObserver
    {

    public:

       /**
        * Constructor.
        *
        * @return CLogsEngine object
        */
        IMPORT_C static CLogsEngine* NewL();
        
       /**
        * Destructor.
        */
        IMPORT_C virtual ~CLogsEngine();

    private:

       /**
        * C++ default constructor.
        */
        CLogsEngine();

       
       /**
        * Second phase of construction
        */
        void ConstructL();
        
        void DeleteClearNewMissedL();


    public: 

       /**
        * Model getter
        *
        * @param aModel model identificator
        * @return pointer to model
        */
        IMPORT_C MLogsModel* Model( TLogsModel aModel ) const;

       /**
        * DB clearer getter
        *
        * @return pointer to db clearer object
        */
        IMPORT_C MLogsClearLog* ClearLogsL();
        
        /**
        * Event writer getter
        *
        * @return pointer to event writer object
        */
        IMPORT_C MLogsClearNewMissed* ClearNewMissedL();

       /**
        * DB event updater 
        * @param aStore preferred phonebook storage for searching contacts        
        *
        * @return pointer to db event updater
        */
        IMPORT_C MLogsReader* EventUpdaterL();

       /**
        * CntLinkChecker getter
        *
        * @return pointer to CntLink checker object
        */
        IMPORT_C CLogsCntLinkChecker* CntLinkCheckerL();

       /**
        * DB configurator getter
        *
        * @return pointer to db configurator
        */
        IMPORT_C MLogsConfig* ConfigL();

       /**
        * Getter for log event getter
        *
        * @return pointer to log event getter
        */
        IMPORT_C MLogsGetEvent* GetEventL();

       /**
        * Shared data getter
        *
        * @return pointer to shared data object
        */
        IMPORT_C MLogsSharedData* SharedDataL();        

       /**
        * System agent getter
        *
        * @return pointer to system agent object
        */
        IMPORT_C MLogsSystemAgent* SystemAgentL();

       /**
        * Deletes log event getter
        */
        IMPORT_C void DeleteGetEvent();

       /**
        * Deletes log event updater
        */
        IMPORT_C void DeleteEventUpdater();

       /**
        * Deletes CntLink checker
        */
        IMPORT_C void DeleteCntLinkChecker();

       /**
        * Deletes sms event updater
        */
        IMPORT_C void DeleteSMSEventUpdater();

       /**
        * Deletes system agent object
        */
        IMPORT_C void DeleteSystemAgent();

       /**
        * Deletes log db configurator
        */
        IMPORT_C void DeleteConfig();

       /**
        * Deletes log db clearer
        */
        IMPORT_C void DeleteClearLog();

       /**
        * Model creator
        *
        * @param aModel model identificator
        */
        IMPORT_C void CreateModelL( TLogsModel aModel );

       /**
        * Starts matching SMS events with the phonebook and updating
        * @param aStore preferred phonebook storage for searching contacts                
        */
        IMPORT_C void StartSMSEventUpdaterL();

		/**
		 * Shared Data creator
		 */
		// IMPORT_C void CreateSharedDataL();

		/**
		 * Deletes all not-needed resources when switching the logs to
		 * background mode.
		 */
		IMPORT_C void FreeResourcesForBGModeL();

		/**
		 * Converts 8-bit descriptor data to 16-bit format.
         * @param aForeignText Text to be converted
         * @param aConvertedText Text converted. model identificator
         *
         * @return KErrNone, if converted ok. Otherwise error code.
		 */
        IMPORT_C TInt ConvertToUnicode(const TDesC8& aForeignText,
                                             TDes16& aConvertedText);
                                             
		/**
		 * Returns identification strings used by Log database
         *
         * @return strings used by Log database
		 */
        IMPORT_C TLogsEventStrings LogDbStrings();
        
		/**
		 * Returns reference to CLogClient. 
         *
         * @return pointer to CLogClient
		 */
        IMPORT_C CLogClient* CLogClientRef();

    public: // from MLogsObserver

        void StateChangedL( MLogsStateHolder* aHolder );
    
    public: // from MLogsCallObserver    
    
        void CallStateChangedL( TInt aCallState );
    
    private:    // data

        /// Ref:
        CLogClient* iLogClient;

        /// Own:
        TLogsEventStrings iStrings;

        /// Own: file server session
        RFs iFsSession;

        /// Own: main ( log ) model
        MLogsModel* iMainModel;

        /// Own: recent received model
        MLogsModel* iReceivedModel;

        /// Own: recent dialled model
        MLogsModel* iDialledModel;

        /// Own: recent missed model
        MLogsModel* iMissedModel;

        /// Own: clearing log db object
        MLogsClearLog* iClearLogs;
        
        CLogsClearNewMissed* iClearNewMissed;

        /// Own: sms event updater
        MLogsReader* iSMSUpdater;

        /// Own: event updater
        MLogsReader* iEventUpdater;

        /// Own: CntLink checker
        CLogsCntLinkChecker* iCntLinkChecker;
        
        /// Own: log db configuration
        MLogsConfig* iConfig;

        /// Own: log get event
        MLogsGetEvent* iGetEvent;

        /// Own: shared data
        MLogsSharedData* iSharedData;

        /// Own: system agent
        MLogsSystemAgent* iSystemAgent;

        /// Own: Unicode converter
        CCnvCharacterSetConverter* iConverter;
    };

#endif      // __Logs_Engine_CLogsEngine_H__



// End of File
