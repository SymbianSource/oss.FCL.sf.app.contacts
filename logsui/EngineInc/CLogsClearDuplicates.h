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
*     Recent list duplicate cleaner
*
*/


// INCLUDE FILES

#include <logview.h>
#include "CLogsRecentReader.h"
#include "MLogsObserver.h"
#include "MLogsEvent.h"
#include "MLogsEventArray.h"
#include "LogsEngConsts.h"

// MODULE DATA STRUCTURES
class CLogsClearDuplicates : public CActive, 
                             public MLogViewChangeObserver
    {
    public:
        /**
         * Symbian OS constructor. 
         *
         * @param aFsSession   ref. to file server session
         * @return New instance of this class
         */
        static CLogsClearDuplicates* NewL( RFs& aFsSession );

        /**
         *  Destructor
         */
        ~CLogsClearDuplicates();

    private:
        /**
         * Default constructor. 
         */
        CLogsClearDuplicates();

        /**
         * Second phase constructor. 
         */
        void ConstructL();

        /**
         * C++ constructor. 
         *
         * @param aFsSession   ref. to file server session
         */
        CLogsClearDuplicates( RFs& aFsSession );

    public:
        /**
         * Clear duplicates and set KLogEventRead flag
         *
         * @param aDuplicates       clear duplicates if ETrue
         * @param aView             view to clear duplicates from
         */
        void ClearDuplicatesL( TBool aDuplicates, CLogViewRecent* aView );

    private:
        /**
         * Initialize database view for log events having "read" flag unset 
         */
        void SetFlagClearViewL();
        /**
         * Initialize the database view for log events having "ALS" flag set 
         */
        void SetFlagClearViewALSL();

    public: // from MLogViewChangeObserver
        void HandleLogViewChangeEventAddedL
                    (   TLogId aId
                    ,   TInt aViewIndex
                    ,   TInt aChangeIndex
                    ,   TInt aTotalChangeCount
                    );

        void HandleLogViewChangeEventChangedL
                    (   TLogId aId
                    ,   TInt aViewIndex
                    ,   TInt aChangeIndex
                    ,   TInt aTotalChangeCount
                    );

        void HandleLogViewChangeEventDeletedL
                    (   TLogId aId
                    ,   TInt aViewIndex
                    ,   TInt aChangeIndex
                    ,   TInt aTotalChangeCount
                    );

    protected: /// from CActive
        void RunL();
        void DoCancel();

    private: // data
        /// Ref: file server session
        RFs&            iFsSession;

        /// Own: log client
        CLogClient*     iLogClient;

        /// Own: log view
        // database view for log events having "read" flag unset 
        CLogViewRecent* iFlagClearView;
        // database view for log events having "ALS" flag set 
        CLogViewRecent* iFlagClearViewALS;
        TBool iClearViewALSIsSet; 
        

        /// Own: log clear flags filter
        CLogFilter*     iFilterFlagClear;
        CLogFilter*     iFilterFlagClearALS;
    };

