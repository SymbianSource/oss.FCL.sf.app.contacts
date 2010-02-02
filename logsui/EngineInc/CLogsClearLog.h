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
*     Class for log clearing.
*
*/


#ifndef __CLogsClearLog_H_
#define __CLogsClearLog_H_

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <logviewchangeobserver.h>



#include "MLogsClearLog.h"
#include "MLogsStateHolder.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogClient;
class MLogsObserver;
class CLogViewRecent;

// CLASS DECLARATION

/**
 *	Clearing class.
 */
class CLogsClearLog : public CActive, 
                      public MLogsClearLog,
                      public MLogsStateHolder
    {
    public:
        /**
         * Symbian OS constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer. Default NULL
         * @return New instance of this class
         */
        static CLogsClearLog* NewL
                            (   RFs& aFsSession
                            ,   MLogsObserver* aObserver = NULL\
                            );

        /**
         *  Destructor
         */
        ~CLogsClearLog();

    private:
        /**
         * Default constructor. 
         */
        CLogsClearLog();

        /**
         * Second phase constructor. 
         */
        void ConstructL();

        /**
         * C++ constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @param aObserver pointer to observer
         */
        CLogsClearLog( RFs& aFsSession, MLogsObserver* aObserver );

    protected: // from CActive
        void DoCancel();
        void RunL();
		TInt RunError(TInt aError);

    public: // from MLogsClearLog
        void ClearModelL( const TLogsModel aModel );       
        void DeleteEventL( TLogId aId );               
        void SetObserver( MLogsObserver* aObserver );

    public: // from MLogsStateHolder
        TLogsState State() const;

    private:
        // internal states of object
        enum TClearPhase
            {
            EClearLog = 0,
            EClearEvent,
            EClearDuplicates
            };

    private: // data
        /// Ref: File server session
        RFs&                iFsSession;

        /// Own: Log client
        CLogClient*         iLogClient;

        /// Own:
        CLogViewRecent*     iViewRecent;

        /// Ref: Observer
        MLogsObserver*      iObserver;        






        /// Own: state
        TLogsState          iState;

        /// Own: phase
        TClearPhase         iPhase;
    };

#endif

// End of File  __CLogsClearLog_H_
