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

// INCLUDE FILES
#include <QList>
#include <logview.h>
#include <logwraplimits.h>
#include "logsreaderstates.h"
#include "logsreaderstatecontext.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "logslogger.h"
#include "logsreaderobserver.h"
#include "logscommondata.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// LogsReaderStateBase::LogsReaderStateBase
// ----------------------------------------------------------------------------
//
LogsReaderStateBase::LogsReaderStateBase(
    LogsReaderStateContext& context) 
 : mContext(context),
   mNextState(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::~LogsReaderStateBase
// ----------------------------------------------------------------------------
//
LogsReaderStateBase::~LogsReaderStateBase()
{

}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::setNextState
// ----------------------------------------------------------------------------
//
void LogsReaderStateBase::setNextState(LogsReaderStateBase& nextState)
{
    mNextState = &nextState;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::enterNextStateL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::enterNextStateL()
{
    if ( mNextState ){
        mContext.setCurrentState(*mNextState);
        return mNextState->enterL();
    }
    return false;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::enterL()
{
    return false;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::continueL()
{
    return false;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::viewCount
// ----------------------------------------------------------------------------
//
int LogsReaderStateBase::viewCountL() const
    {
    return mContext.logView().CountL();
    }

// ----------------------------------------------------------------------------
// LogsReaderStateBase::event
// ----------------------------------------------------------------------------
//
CLogEvent& LogsReaderStateBase::event() const
    {
    //The RVCT compiler provides warnings "type qualifier on return type is meaningless"
    //for functions that return const values. In order to avoid these numerous warnings and 
    //const cascading, the CLogEvent is const_casted here.
    return const_cast<CLogEvent&>( mContext.logView().Event() );
    }

// ----------------------------------------------------------------------------
// LogsReaderStateBase::updateAndInsertEventL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::updateAndInsertEventL(
    const CLogEvent& source, LogsEvent* dest, int& eventIndex)
{
    Q_ASSERT( dest );
    dest->initializeEventL( source, mContext.strings() );
    dest->setIndex(eventIndex);
    mContext.events().insert(eventIndex, dest);
    eventIndex++;
    return true;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::constructAndInsertEventL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::constructAndInsertEventL(
    const CLogEvent& source, LogsEvent* dest, int& eventIndex)
{
    Q_ASSERT( dest );
    dest->initializeEventL( source, mContext.strings() );
    if ( !dest->validate() ){
        LOGS_QDEBUG( "LogsReaderStateBase::constructAndInsertEventL, event discarded" )
        delete dest;
        return false;
    } 
    dest->setIndex(eventIndex);
    mContext.events().insert(eventIndex, dest);
    eventIndex++;
    return true;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::resetEvents
// ----------------------------------------------------------------------------
//
void LogsReaderStateBase::resetEvents()
{
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count(); i++ ){
        events.at(i)->setIsInView(false);
    }
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::takeMatchingEvent
// ----------------------------------------------------------------------------
//
LogsEvent* LogsReaderStateBase::takeMatchingEvent(const CLogEvent& event)
{
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count(); i++ ){
        if ( events.at(i)->logId() == event.Id() ){
            return events.takeAt(i);
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::eventById
// ----------------------------------------------------------------------------
//
LogsEvent* LogsReaderStateBase::eventById(int eventId)
{
    LogsEvent* event = 0;
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count(); i++ ){
        if ( events.at(i)->logId() == eventId ){
            event = events.at(i);
            break;
        }
    }
    return event;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::duplicatesL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::duplicatesL(const CLogFilter* aFilter){
    bool gettingDuplicates( false );
    if ( mContext.isRecentView() ){
        if ( aFilter ){
            gettingDuplicates = 
                static_cast<CLogViewRecent&>( mContext.logView() ).DuplicatesL( 
                        mContext.duplicatesView(), *aFilter, mContext.reqStatus() ); 
        } else {
            gettingDuplicates = 
                static_cast<CLogViewRecent&>( mContext.logView() ).DuplicatesL( 
                        mContext.duplicatesView(), mContext.reqStatus() ); 
        }
    }
    return gettingDuplicates;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFiltering::LogsReaderStateFiltering(
    LogsReaderStateContext& context ) 
 : LogsReaderStateBase(context),
   mFilterList(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::~LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFiltering::~LogsReaderStateFiltering()
{
    if ( mFilterList ){
        mFilterList->ResetAndDestroy();
    }
    delete mFilterList;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::enterL()
{
    // Filtering all recent calls (max number of recent events is configurable,
    // see TLogConfig in logcli.h
    //
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFiltering::enterL" );
    resetEvents();
    
    CLogFilterList* filterList = new ( ELeave ) CLogFilterList;
    CleanupStack::PushL(filterList);
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( KLogCallEventTypeUid );
    filterList->AppendL( filter );
    CleanupStack::Pop( filter );
    
    if ( mFilterList ){
        mFilterList->ResetAndDestroy();
        delete mFilterList;
        mFilterList = 0;
    }
    mFilterList = filterList;
    CleanupStack::Pop(filterList);
    
    if ( setFilterL( *filterList ) ) {
        return true;
    }
    
    // Not possible to continue with filtering
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::continueL()
{
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::setFilterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::setFilterL(CLogFilterList& filterList){
    __ASSERT_ALWAYS( mContext.isRecentView(), User::Leave( KErrNotFound ) );
    return static_cast<CLogViewRecent&>( mContext.logView() ).SetRecentListL( 
            KLogNullRecentList, filterList, mContext.reqStatus() );
}
    
// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFilteringAll::LogsReaderStateFilteringAll(
    LogsReaderStateContext& context ) 
 : LogsReaderStateFiltering(context)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFilteringAll::~LogsReaderStateFilteringAll
// ----------------------------------------------------------------------------
//
LogsReaderStateFilteringAll::~LogsReaderStateFilteringAll()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFilteringAll::setFilterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFilteringAll::setFilterL(CLogFilterList& filterList){
    __ASSERT_ALWAYS( !mContext.isRecentView(), User::Leave( KErrNotFound ) );
    return static_cast<CLogViewEvent&>( mContext.logView() ).SetFilterL( 
            filterList, mContext.reqStatus() );
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::LogsReaderStateReading
// ----------------------------------------------------------------------------
//
LogsReaderStateReading::LogsReaderStateReading(
    LogsReaderStateContext& context ) 
 : LogsReaderStateBase(context),
   mDuplicateMissedFilter(0),
   mCheckingMissed(false),
   mEventIndex(0),
   mReadSizeCounter(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::~LogsReaderStateReading
// ----------------------------------------------------------------------------
//
LogsReaderStateReading::~LogsReaderStateReading()
{
    delete mDuplicateMissedFilter;
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::enterL()
{
    mCheckingMissed = false;
    mEventIndex = 0;
    mReadSizeCounter = 0;
    
    if ( !mDuplicateMissedFilter ){
        // Interested only about duplicates which are not marked as read
        mDuplicateMissedFilter = CLogFilter::NewL();
        mDuplicateMissedFilter->SetFlags(KLogEventRead);
        mDuplicateMissedFilter->SetNullFields(ELogFlagsField);
    }
    
    if ( viewCountL() > 0 && mContext.logView().FirstL( mContext.reqStatus() ) ){
        return true;
    }
    
    // Not possible to continue with reading
    return enterNextStateL();  
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::continueL()
{
    int& index = mContext.index();  
    QList<LogsEvent*> &events = mContext.events();
        
    if ( mCheckingMissed ) {
        events.at(mEventIndex-1)->setDuplicates( 
                mContext.duplicatesView().CountL() );
        mCheckingMissed = false;
    } 
    else {
        const CLogEvent& sourceEvent = event();
        LogsEvent* event = takeMatchingEvent(sourceEvent);
        bool inserted = false;
        if ( event ){
            // Matching event is updated and put to new position
            inserted = updateAndInsertEventL( sourceEvent, event, mEventIndex );
        }
        else {
            // Create new entry
            event = new LogsEvent;
            inserted = constructAndInsertEventL( sourceEvent, event, mEventIndex );
        }
        
        if ( inserted  ){
            updateReadSizeCounter(*event);
            if ( handleMissedL(*event) ){
                mCheckingMissed = true;
                return true;
            }
        }
    }

    index++;    
    if ( canContinueReadingL(index) ){
        return mContext.logView().NextL( mContext.reqStatus() );
    }           
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::handleMissedL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::handleMissedL(LogsEvent& parsedEvent)
{
    bool handled = false;
    if ( parsedEvent.direction() == LogsEvent::DirMissed ){
        handled = duplicatesL(mDuplicateMissedFilter); 
    }
    return handled;
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::updateReadSizeCounter
// ----------------------------------------------------------------------------
//
void LogsReaderStateReading::updateReadSizeCounter(LogsEvent& event)
{
    if ( LogsCommonData::getInstance().maxReadSize() >= 0 ){
        LogsEvent::LogsDirection dir = 
            LogsCommonData::getInstance().maxReadSizeDirection();
        if ( dir == LogsEvent::DirUndefined || dir == event.direction() ){
            mReadSizeCounter++;
        }
    }
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::canContinueReadingL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::canContinueReadingL(int index) const
{
    bool canContinue( index < viewCountL() );
    int maxReadSize = LogsCommonData::getInstance().maxReadSize();
    if ( canContinue && maxReadSize >= 0 ){
        canContinue = ( mReadSizeCounter < maxReadSize );
    }
    return canContinue;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::LogsReaderStateFillDetails
// ----------------------------------------------------------------------------
//
LogsReaderStateFillDetails::LogsReaderStateFillDetails(
    LogsReaderStateContext& context) 
 : LogsReaderStateBase(context)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::~LogsReaderStateFillDetails
// ----------------------------------------------------------------------------
//
LogsReaderStateFillDetails::~LogsReaderStateFillDetails()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFillDetails::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFillDetails::enterL()" )
    fillDetails();
    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateFillDetails::enterL()" )
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::fillDetails
// ----------------------------------------------------------------------------
//
void LogsReaderStateFillDetails::fillDetails()
{
    QHash<QString, ContactCacheEntry>& contactMappings = mContext.contactCache();
    QList<LogsEvent*> &events = mContext.events();
    foreach ( LogsEvent* event, events ){
        const QString& num = event->getNumberForCalling();
        if ( !event->isInView() ){
            // Not interested about to be removed event
        } else if ( contactMappings.contains(num) ) {
            // Matching cached contact found, use that
            LOGS_QDEBUG_2( "logs [ENG]    Use existing contact for num:", num )
            ContactCacheEntry entry = contactMappings.value(num);
            event->setRemoteParty( entry.mRemoteParty );
            event->setContactLocalId( entry.mContactLocalId );
        } else if ( event->remoteParty().length() == 0 ) {
            // No remote party name, search for match from phonebook
            QString contactNameStr = event->updateRemotePartyFromContacts(
                    LogsCommonData::getInstance().contactManager());
            if (contactNameStr.length() > 0){
                LOGS_QDEBUG_3( "LogsReaderStateFillDetails, (name, num):", 
                               contactNameStr, num );
                // Cache the new contact name
                ContactCacheEntry contactEntry(contactNameStr, event->contactLocalId());
                contactMappings.insert( num, contactEntry );
            }
        }
    }    
}
    
// ----------------------------------------------------------------------------
// LogsReaderStateDone::LogsReaderStateDone
// ----------------------------------------------------------------------------
//
LogsReaderStateDone::LogsReaderStateDone(
    LogsReaderStateContext& context ) 
 : LogsReaderStateBase(context)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateDone::~LogsReaderStateDone
// ----------------------------------------------------------------------------
//
LogsReaderStateDone::~LogsReaderStateDone()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateDone::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateDone::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateDone::enterL" );
    
    int numRead = qMin(mContext.index(),viewCountL());
    mContext.observer().readCompleted(numRead);

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateDone::enterL" );
    
    return false;
}


// ----------------------------------------------------------------------------
// LogsReaderStateSearchingEvent::LogsReaderStateSearchingEvent
// ----------------------------------------------------------------------------
//
LogsReaderStateSearchingEvent::LogsReaderStateSearchingEvent(
    LogsReaderStateContext& context ) 
  : LogsReaderStateBase(context)
{
}


// ----------------------------------------------------------------------------
// LogsReaderStateSearchingEvent::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateSearchingEvent::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateSearchingEvent::enterL" );
    mUpdatingEvent = false;
    if ( viewCountL() > 0 && mContext.logView().FirstL( mContext.reqStatus() ) ){
        return true;
    }    
    return enterNextStateL();    
}


// ----------------------------------------------------------------------------
// LogsReaderStateSearchingEvent::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateSearchingEvent::continueL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateSearchingEvent::continueL" );
    int& index = mContext.index();
    if ( !mUpdatingEvent ) {
        if ( event().Id() == mContext.modifyingEventId() ) {
            // Mark event read
            event().SetFlags( event().Flags() | KLogEventRead ); 
            mContext.logClient().ChangeEvent(event(), mContext.reqStatus());
            mUpdatingEvent = true;
            return true;
        } else {
            index++;            
            if ( index < viewCountL() ){
                return mContext.logView().NextL( mContext.reqStatus() );
            }
        }
    }
    
    return enterNextStateL();
}    

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::LogsReaderStateFindingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateFindingDuplicates::LogsReaderStateFindingDuplicates(
    LogsReaderStateContext& context ) 
 : LogsReaderStateBase(context)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFindingDuplicates::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFindingDuplicates::enterL" );
    if ( duplicatesL() ) {
        LOGS_QDEBUG( "logs [ENG] duplicates exist!");
        return true;
    }
    
    // Not possible to continue with deletion
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFindingDuplicates::continueL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFindingDuplicates::continueL" );

    // Mark duplicate events read
    mContext.duplicatesView().SetFlagsL(
            mContext.duplicatesView().Event().Flags() | KLogEventRead ); 

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateFindingDuplicates::continueL" );
    
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateModifyingDone::LogsReaderStateModifyingDone
// ----------------------------------------------------------------------------
//
LogsReaderStateModifyingDone::LogsReaderStateModifyingDone(
    LogsReaderStateContext& context) : LogsReaderStateBase(context)
{

}
    
// ----------------------------------------------------------------------------
// LogsReaderStateModifyingDone::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateModifyingDone::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateModifyingDone::enterL" );
    
    LogsEvent* modifiedEvent = eventById(mContext.modifyingEventId());
    if ( modifiedEvent ){
        // Update modified event to know that it has been marked. Real
        // update of the event happens soon when db notifies the change.
        modifiedEvent->markedAsSeenLocally(true);
    }
    mContext.observer().eventModifyingCompleted();

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateModifyingDone::enterL" );
    
    return false;
}
