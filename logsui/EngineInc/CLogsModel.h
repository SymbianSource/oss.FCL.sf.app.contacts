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
*     Abstract base class for logs model.
*
*/


#ifndef __CLogsModel_H_
#define __CLogsModel_H_

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "MLogsModel.h"
#include "MLogsObserver.h"
#include "MLogsStateHolder.h"
#include "TLogsEventStrings.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsStateHolder;
class MLogsEvent;
class MLogsReader;
class MLogsEventWrapper;
class MLogsEventArray;
class CLogsEngine;

// CLASS DECLARATION

/**
 *	Defines abstract interface for Logs model
 */
class CLogsModel :  public CBase,
                    public MLogsModel,
                    public MLogsObserver,
                    public MLogsStateHolder
    {
    public:
        /**
         * EPOC constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aModel model type
         * @param aStrings     log client constants
         * @return model object
         */
        static CLogsModel* NewL( RFs& aFsSession, TLogsModel aModel,
                            TLogsEventStrings& aStrings, CLogsEngine* aLogsEngine );
        /**
         *  Destructor
         */
        ~CLogsModel();

    private:
        /**
         * EPOC constructor. 
         */
        void ConstructL();
        
        /**
         * C++ constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aModel model type
         * @param aStrings     log client constants
         */
        CLogsModel( RFs& aFsSession, 
                    TLogsModel aModel, 
                    TLogsEventStrings& aStrings, 
                    CLogsEngine* aLogsEngineRef );

    private:

    public: // from MLogsModel
		void Delete( TInt aIndex );
        TInt Count();
        const MLogsEventGetter* At( TInt aIndex );
        void SetObserver( MLogsObserver* aObserver );
        void DoActivateL( TDoActivate aDoActivate );  
        void DoDeactivate( TClearMissedCalls aClearMissedCalls,
                           TDoDeactivate aDisconnectDb  );  
        void Reset();
        void ConfigureL( const MLogsReaderConfig* aConfig );
        void KickStartL( TBool aReset );

    public: // from MLogsStateHolder
        TLogsState State() const;

    public: // from MLogsObserver
        void StateChangedL( MLogsStateHolder* aHolder );

    private: // data

        /// Ref: File server session
        RFs& iFsSession;

        /// Own: model type
        TLogsModel iModel;

        /// Ref: log strings
        TLogsEventStrings& iStrings;

        /// Own: Event array
        MLogsEventArray* iEventArray;

        /// Own: MLogsReader
        MLogsReader* iReader;

        /// Own: wrapper for CLogsGetEvent
        MLogsEventWrapper* iWrapper;

        /// Ref: Observer
        MLogsObserver* iObserver;

        /// own: state for observer
        TLogsState      iState;
        
        /// Ref: Log db engine        
        CLogsEngine* iLogsEngineRef;         
    };

            
#endif

// End of File  __CLogsModel_H_
