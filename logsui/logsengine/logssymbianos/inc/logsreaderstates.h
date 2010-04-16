/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef LOGSREADERSTATES_H
#define LOGSREADERSTATES_H

//  INCLUDES
#include <e32std.h>
#include <logclientchangeobserver.h>
#include <logviewchangeobserver.h>

// FORWARD DECLARATION
class LogsReaderStateContext;
class LogsEvent;
class CLogViewEvent;
class CLogFilterList;
class CLogFilter;
class CLogEvent;

// CLASS DECLARATION

/**
 * Reader state base
 */
class LogsReaderStateBase {

    friend class UT_LogsReaderStates;
    friend class UT_LogsReader;
    
    public:
        
        /**
         * Destructor
         */
        virtual ~LogsReaderStateBase();
        
        /**
         * Set next state to be used once this state has completed.
         * @param nextState
         */
        void setNextState(LogsReaderStateBase& nextState);

        /**
         * Enter to the state, may proceed immediately to next state.
         * @return true if entering started async operation,  false if not
         */
        virtual bool enterL();
        
        /**
         * Continue running in the state, may proceed to next state
         * @return true if continue started async operation,  false if not
         */
        virtual bool continueL();
    
    protected:
        
        /**
         * Constructor
         */
        LogsReaderStateBase(LogsReaderStateContext& context);
        
        /**
         * Proceed to next state if such exists.
         * @return true if entering started async operation,  false if not 
         */
        virtual bool enterNextStateL();
        
        /**
         * Get number of events in view
         * @return view count
         */
        int viewCountL() const;
        
        /**
         * Get current event
         * @return event
         */
        CLogEvent& event() const;
        
        /**
         * Fill dest event with source event data and insert to specified
         * place in events list.
         * @param source event 
         * @param dest event, ownership is transferred
         * @param index, increased if event was added
         * @param events, event is possibly added to this list
         * @return true if dest event was inserted, false if discarded and deleted
         */
        bool constructAndInsertEventL(
                const CLogEvent& source, LogsEvent* dest, int &eventIndex, QList<LogsEvent*>& events );
        
        /**
         * Fill already used dest event with source event data and insert
         * to specified place in events list.
         * @param source event 
         * @param dest event, ownership is transferred
         * @param index, increased if event was added
         * @return true if dest event was inserted, false if discarded and deleted
         */
        bool updateAndInsertEventL(
                const CLogEvent& source, LogsEvent* dest, int &eventIndex );
        
        /**
         * Reset current events
         */
        void resetEvents();
        
        /**
         * Try to find matching event and remove it from
         * events list.
         * @param event, symbian log event
         * @return LogsEvent or 0 if no matching found, ownership is transferred
         */
        LogsEvent* takeMatchingEvent(const CLogEvent& event);
        
        /**
         * Finds specified event from the event list. Event not removed from the list.
         * @param eventId, id of the event to be found
         * @return pointer to event or 0 if not found
         */
        LogsEvent* eventById(int eventId);
        
        /**
         * Try to search duplicates for current event in the view
         * @param aFilter, filter to be used for searching duplicates
         * @return true, if duplicates are searched
         */
        bool duplicatesL(const CLogFilter* aFilter = 0);

    protected:   
        LogsReaderStateContext& mContext;
        LogsReaderStateBase* mNextState;
        int mStateIndex;
};

/**
 * Filtering recent state
 */
class LogsReaderStateFiltering : public LogsReaderStateBase {

     friend class UT_LogsReaderStates;
     
    public:
        LogsReaderStateFiltering(LogsReaderStateContext& context);
        virtual ~LogsReaderStateFiltering();

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    
    protected:
        /**
         * Setting filter for view, derived class may add special filtering
         * at this stage.
         */
        virtual bool setFilterL(CLogFilterList& filterList);
        
    protected:
        CLogFilterList* mFilterList;
 };

/**
 * Filtering all state
 */
class LogsReaderStateFilteringAll : public LogsReaderStateFiltering {

     friend class UT_LogsReaderStates;
     
    public:
        LogsReaderStateFilteringAll(LogsReaderStateContext& context);
        virtual ~LogsReaderStateFilteringAll();

    protected: // From LogsReaderStateFiltering
        virtual bool setFilterL(CLogFilterList& filterList);
 };

/**
 * Reading state
 */
class LogsReaderStateReading : public LogsReaderStateBase {

    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReading(LogsReaderStateContext& context);
        virtual ~LogsReaderStateReading();

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    
    protected:
        bool handleMissedL(LogsEvent& parsedEvent);
        void updateReadSizeCounter(LogsEvent& event);
        bool canContinueReadingL(int index) const;     
        
    protected:
        CLogFilter* mDuplicateMissedFilter;
        bool mCheckingMissed;
        int mEventIndex;
        int mReadSizeCounter;
};

/**
 * Fill missing information state
 */
class LogsReaderStateFillDetails : public LogsReaderStateBase {
    public:
        LogsReaderStateFillDetails(LogsReaderStateContext& context);
        virtual ~LogsReaderStateFillDetails();
        
        /**
         * Synchronously fills details from phonebook
         */
        void fillDetails(); 
        
    public: // From LogsReaderStateBase
        virtual bool enterL();
};

/**
 * Completed state
 */
class LogsReaderStateDone : public LogsReaderStateBase {
    public:
        LogsReaderStateDone(LogsReaderStateContext& context);
        virtual ~LogsReaderStateDone();
        
    public: // From LogsReaderStateBase
        virtual bool enterL();
};


/**
 * Searching event state
 */
class LogsReaderStateSearchingEvent : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateSearchingEvent(LogsReaderStateContext& context);
        virtual ~LogsReaderStateSearchingEvent(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
};

/**
 * Finding duplicate events state
 */
class LogsReaderStateFindingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateFindingDuplicates(LogsReaderStateContext& context);
        virtual ~LogsReaderStateFindingDuplicates();

    public: // From LogsReaderStateBase
        virtual bool enterL();  
        virtual bool continueL();
        
    protected:
        CLogFilter* mDuplicateFilter;
};

/**
 * Marking duplicate events state
 */
class LogsReaderStateMarkingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateMarkingDuplicates(LogsReaderStateContext& context);
        virtual ~LogsReaderStateMarkingDuplicates(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL(); 
        
    protected:
        bool mGettingDuplicates;
};

/**
 * Marking duplicate events state
 */
class LogsReaderStateReadingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReadingDuplicates(LogsReaderStateContext& context);
        virtual ~LogsReaderStateReadingDuplicates(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();   
        virtual bool continueL();
};

/**
 * Modifying done state
 */
class LogsReaderStateModifyingDone : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateModifyingDone(LogsReaderStateContext& context);
        virtual ~LogsReaderStateModifyingDone(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
};

/**
 * Reading duplicates done state
 */
class LogsReaderStateReadingDuplicatesDone : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReadingDuplicatesDone(LogsReaderStateContext& context);
        virtual ~LogsReaderStateReadingDuplicatesDone(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
};



#endif      // LOGSREADERSTATES_H


// End of File
      

        
       
