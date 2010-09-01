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
*     Implements interface for Logs reader. Common functionality.
*
*/


#ifndef __Logs_Engine_CLogsBaseReader_H__
#define __Logs_Engine_CLogsBaseReader_H__

//  INCLUDES
#include <e32base.h>
#include <logwrap.h>
#include <logviewchangeobserver.h>
#include "MLogsReader.h"
#include "MLogsStateHolder.h"
#include "TLogsEventStrings.h"

// FORWARD DECLARATION
class CLogClient;
class MLogsEvent;
class MLogsEventArray;
class CLogsEngine;

// CLASS DECLARATION

/**
 *  Implements interface for Logs reader. Common functionality.
 */
class CLogsBaseReader : public CActive, 
                        public MLogsReader,
                        public MLogsStateHolder,
                        public MLogViewChangeObserver
    {
    public:
       /**
        *   Destructor.
        */
        virtual ~CLogsBaseReader();

    private:
       /**
        *  Default constructor
        */
        CLogsBaseReader();
      
    protected:
        /**
         *  Constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aEventArray  array of events
         *  @param aStrings     log client constants
         *  @param aModel       model id
         *  @param aObserver    observer
         */
        CLogsBaseReader(    RFs& aFsSession
                       ,    MLogsEventArray& aEventArray
                       ,    TLogsEventStrings& aStrings
                       ,    TLogsModel aModel
                       ,    MLogsObserver* aObserver
                       ,    CLogsEngine* aLogsEngineRef
                       );

        /**
         *  Second phase constructor
         */
        void BaseConstructL();

        /**
         *  Pure virtual. Next reading operation.
         *
         *  @return ETrue to set the object active
         */
        virtual TBool DoNextL() = 0;

        /**
         *  Wrapper for DoNextL
         */
        void NextL();

        /**
         *  Base event construction
         *
         *  @param aDest    ref. to MLogsEvent implemetation object
         *  @param aSource  pointer to CLogEvent object
         */
        void BaseConstructEventL( MLogsEvent& aDest, const CLogEvent& aSource );

        /**
         *  Event construction
         *
         *  @param aDest    ref. to MLogsEvent implemetation object
         *  @param aSource  pointer to CLogEvent object
         */
        virtual void ConstructEventL
                        (   MLogsEvent& aDest
                        ,   const CLogEvent& aSource
                        ) = 0;

        /**
         *  Checks array's length against the view length and deletes
         *  extra events from the array
         */
        void CheckArrayLengthL();

        /**
         *  Event count getter in a view
         *
         *  @return number of events in a view
         */
        virtual TInt ViewCountL() const = 0;

        /**
         *  Current event getter from the view
         *
         *  @return ref. to current event
         */
        virtual CLogEvent& Event() const = 0;

        /**
         *  Duplicate count getter
         *
         *  @return number of duplicates of the current event
         */
        virtual TInt DuplicateCountL() const;

        /**
         * Operations to be done once reader has finished reading
         * If function is implemented in derived base class implementation 
         * must be called.
         */
        virtual void ReadingFinishedL();

        /**
         * Db change handler
         *
	     * @param aTotalChangeCount		This is the total count of all 
         *                              changes which will be processed in this batch
         *                              default 1
	     */
        void HandleViewChangeL( TInt aTotalChangeCount = 1 );

    protected: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    public: // from MLogsReader
        void Stop() = 0;
        void Interrupt();
        TBool IsInterrupted() const;
        void SetObserver( MLogsObserver* aObserver );
        void StartL() = 0;
        void ContinueL();
        TBool IsDirty() const;
        void SetDirty();
        void ActivateL();
        void DeActivate();

    public: // from MLogsStateHolder
        TLogsState State() const;

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

    protected:
        // internal states of object
        enum TReaderPhase
            {
            EInitial = 0,
            EFilter,
            ERead,
            EDuplicate,
            EDone
            };

    protected:    // data
        /// ref: file session
        RFs&                    iFsSession;        

        /// ref: to event array
        MLogsEventArray&		iEventArray;

        /// own: string constants
        TLogsEventStrings&      iStrings;

        /// ref: log client
        CLogClient*             iLogClientRef;

        /// ref: observer
        MLogsObserver*          iObserver;

        /// Ref: Log db engine        
        CLogsEngine*            iLogsEngineRef;         

        /// own: phase
        TReaderPhase            iPhase;

        /// own: model id
        TLogsModel              iModelId;

        /// own: array index
        TInt                    iIndex; 
        
        /// own: flag for interruption
        TBool                   iInterrupted;        

        /// own: state for observer
        TLogsState              iState;

        /// own: dirty flag
        TBool                   iDirty;

        // own: activated flag
        TBool                   iActivated;  
        
        /// own:
        TInt                    iDeleteChangeCount; 

        /// own: const emergency nbr supported
        TBool                   iStaticEmerg;
        
        TBool                   iReadMissedDuplicateCounts;
        
    };

#endif      // __Logs_Engine_CLogsBaseReader_H__

// End of File
