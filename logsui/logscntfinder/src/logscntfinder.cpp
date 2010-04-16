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

#include <QListIterator>

#include <qcontactdetailfilter.h>
#include <qcontactphonenumber.h>
#include <qcontactname.h>
#include <qcontactmanager.h>
#include <qcontactavatar.h>
#include <QVector>

#include "logscntfinder.h"
#include "logspredictivetranslator.h"
#include "logslogger.h"

const int MaxPredSearchPatternLen = 15;


// -----------------------------------------------------------------------------
// LogsCntEntry::richText()
// -----------------------------------------------------------------------------
//
QString LogsCntText::richText( QString startTag, 
                               QString endTag ) const
{
    QString str = text();
    if ( str.length() > 0 && highlights() > 0 ) {
        str.insert( highlights() , endTag );
        str.insert( 0, startTag );
    }

    return str;
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( LogsCntEntryHandle& handle, 
                            quint32 cid )
    : mType( EntryTypeHistory ), mCid( cid ), 
      mCached( true ),mHandle(&handle)
{
    LogsCntText empty;
    mFirstName.append( empty );
    mLastName.append( empty );
    mAvatarPath = "";
}

// -----------------------------------------------------------------------------
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( quint32 cid )
    : mType( EntryTypeContact ), mCid( cid ), 
      mCached( false ),mHandle(0)
{
    LogsCntText empty;
    mFirstName.append( empty );
    mLastName.append( empty );
    mAvatarPath = "";
}

// -----------------------------------------------------------------------------
// copy LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( const LogsCntEntry& entry )
    : mType(entry.mType),
      mCid(entry.mCid),
      mFirstName(entry.mFirstName),
      mLastName(entry.mLastName),
      mCached(entry.mCached),
      mHandle(entry.mHandle),
      mPhoneNumber(entry.mPhoneNumber),
      mAvatarPath(entry.mAvatarPath)
{
}

// -----------------------------------------------------------------------------
// LogsCntEntry::~LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::~LogsCntEntry()
{
}

// -----------------------------------------------------------------------------
// LogsCntEntry::firstName()
// -----------------------------------------------------------------------------
//
const LogsCntTextList& LogsCntEntry::firstName() const 
{
    return mFirstName;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::lastName()
// -----------------------------------------------------------------------------
//
const LogsCntTextList& LogsCntEntry::lastName() const
{
    return mLastName;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::avatarPath()
// -----------------------------------------------------------------------------
//
const QString& LogsCntEntry::avatarPath() const
{
    return mAvatarPath;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::phoneNumber()
// -----------------------------------------------------------------------------
//
const LogsCntText& LogsCntEntry::phoneNumber() const
{
    return mPhoneNumber;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::speedDial()
// -----------------------------------------------------------------------------
//
const QString& LogsCntEntry::speedDial() const
{
    return mSpeedDial;
}



// -----------------------------------------------------------------------------
// LogsCntEntry::contactId()
// -----------------------------------------------------------------------------
//
quint32 LogsCntEntry::contactId() const
{
    return mCid;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::handle()
// -----------------------------------------------------------------------------
//
LogsCntEntryHandle* LogsCntEntry::handle() const
{
    return mHandle;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setFirstName()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setFirstName( const QString& name ) 
{
    mCached=true;
    mFirstName.clear();
    doSetText( name, mFirstName );
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setLastName()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setLastName( const QString& name ) 
{
    mCached=true;
    mLastName.clear();
    doSetText( name, mLastName );
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setAvatarPath()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setAvatarPath( const QString& avatarpath ) 
{
    mCached=true;
    mAvatarPath.clear();
    mAvatarPath = avatarpath;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::tokens()
// -----------------------------------------------------------------------------
//
QStringList LogsCntEntry::tokens( const QString& source, 
                                  const QChar& separ ) const
{
    QStringList target = source.split( separ, QString::SkipEmptyParts );
    return target;
    
}



// -----------------------------------------------------------------------------
// LogsCntEntry::doSetText()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetText( const QString& text, LogsCntTextList& textlist ) 
{
    QListIterator<QString> iter( tokens( text ) );

    while( iter.hasNext() ) {
        LogsCntText txt;
        txt.mText = iter.next();
        txt.mTranslatedText = 
            LogsPredictiveTranslator::instance()->translate( txt.mText );
        textlist.append( txt );
    }
    if ( textlist.count() == 0 ) {
        textlist.append( LogsCntText() );
    }
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setPhoneNumber()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setPhoneNumber( const QString& number )
{
    mCached=true;
    mPhoneNumber.mText = number;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::resetHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::resetHighlights( LogsCntTextList& nameArray )
{
    QMutableListIterator<LogsCntText> names( nameArray ); 
    while( names.hasNext() ) {
        names.next().mHighlights = 0;
    }
    
}


// -----------------------------------------------------------------------------
// LogsCntEntry::setHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setHighlights( const QString& pattern )
{
    resetHighlights( mFirstName );
    resetHighlights( mLastName );
    
    mPhoneNumber.mHighlights = 
            mPhoneNumber.text().startsWith( pattern ) &&
            mPhoneNumber.text().length() >= pattern.length() ?
            pattern.length(): 0;
    
    doSetHighlights( pattern, mFirstName );
    doSetHighlights( pattern, mLastName );
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSetHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetHighlights( const QString& pattern, 
                                    LogsCntTextList& nameArray )
{
    const QChar zeroSepar('0');
    
    LogsPredictiveTranslator* translator = 
            LogsPredictiveTranslator::instance();
    QMutableListIterator<LogsCntText> names( nameArray ); 

    //simple
    while( names.hasNext() ) {
        LogsCntText& nameItem = names.next();
        nameItem.mHighlights = translator->startsWith( nameItem.mText, pattern, false );
    }
    
    //complex
    QStringList tokenArray = tokens( pattern, zeroSepar );
    QListIterator<QString> patternArray( tokenArray  );
    while( tokenArray.count() > 1 && patternArray.hasNext() ) {
        QString patternItem = patternArray.next();
        names.toFront();
        while( names.hasNext() ) {
            LogsCntText& nameItem = names.next();
            int matchSize = translator->startsWith( nameItem.mText, patternItem );
            nameItem.mHighlights = matchSize > nameItem.mHighlights ?
                                   matchSize : nameItem.mHighlights; 
        }
    }
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setSpeedDial()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setSpeedDial( const QString& number )
{
    mSpeedDial = number;
}



// -----------------------------------------------------------------------------
// LogsCntEntry::match()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::match( const QString& pattern ) const
{
    const QChar zeroSepar('0');
    bool match = false;
    
    //direct match with phone number is enough
    match = mPhoneNumber.text().startsWith( pattern ) ||
            doSimpleMatch( pattern );
    
    if (!match ) {
        QStringList patternArray = tokens( pattern, zeroSepar );
        match = patternArray.count() > 1 && 
                doComplexMatch( patternArray );
    }
    
    return match;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSimpleMatch()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::doSimpleMatch( const QString& pattern ) const
{
    LogsCntTextList nameArray = mFirstName + mLastName; //with empties
    QListIterator<LogsCntText> names( nameArray ); 
    int matchCount = 0;

    while( names.hasNext() && !matchCount ) {
        matchCount = (int)names.next().mTranslatedText.startsWith( pattern );
    }

    return matchCount > 0;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::doComplexMatch()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::doComplexMatch( QStringList patternArray ) const
{
    const QChar zeroSepar('0');
    const bool zero = false;

    LogsCntTextList nameArray = mFirstName + mLastName; //with empties

    int targetMatchCount = patternArray.count();
    int namesCount = nameArray.count();

    //if pattern has more tokens than name(s), it is a missmatch
    if ( namesCount < targetMatchCount ) {
        return false;
    }

    QListIterator<LogsCntText> names( nameArray ); 
    QListIterator<QString> patterns( patternArray );
    QVector<bool> matchVector(targetMatchCount, zero );
    int currentPattern = 0;
    int matchCount = 0;
    bool match = false;
    
    while( names.hasNext() && matchCount < targetMatchCount ) {
        LogsCntText name = names.next();
        currentPattern = 0;
        patterns.toFront();
        match = false;
        while ( !name.mText.isEmpty() && 
                 patterns.hasNext() && !match ) {
            QString pattern = patterns.next();
            //unique match check
            if ( !matchVector.at( currentPattern ) ) {
                match = matchVector[ currentPattern ] 
                      = name.mTranslatedText.startsWith( pattern );
                matchCount = match ? matchCount+1 : matchCount;
            }
            currentPattern++;
        }
    }
    return matchCount >= targetMatchCount;

    }
    
    

// -----------------------------------------------------------------------------
// LogsCntEntry::isCached()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::isCached() const
{
    return mCached;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::type()
// -----------------------------------------------------------------------------
//
LogsCntEntry::EntryType LogsCntEntry::type() const
{
    return mType;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::LogsCntFinder()
    : mCachedCounter(0)
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::LogsCntFinder()" )
    
    // Create manager ourselves, object takes care of deletion when registering
    // as parent.
    QMap<QString, QString> dummyParams;
    mContactManager = new QContactManager("symbian", dummyParams, this);
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::LogsCntFinder()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::LogsCntFinder(QContactManager& contactManager)
    : mCachedCounter(0)
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::LogsCntFinder(), cntmgr from client" )
    
    mContactManager = &contactManager;
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::LogsCntFinder()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::~LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::~LogsCntFinder()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::~LogsCntFinder()" )

    qDeleteAll( mResults );
    qDeleteAll( mHistoryEvents );
    LogsPredictiveTranslator::deleteInstance();
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::~LogsCntFinder()" )
}


// -----------------------------------------------------------------------------
// LogsCntFinder::predictiveSearchQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::predictiveSearchQuery( const QString& pattern )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::predictiveSearchQuery()" )
    LOGS_QDEBUG_2( "logs [FINDER] pattern= ", pattern )
    
    bool canUseCacheQuery = 
            pattern.length() > 1 && 
            pattern.length() - mCurrentPredictivePattern.length() > 0;
    bool areAllResultsCached = resultsCount() > 0 && 
                               mCachedCounter == resultsCount();

    LOGS_QDEBUG_2( "logs [FINDER] canUseCacheQuery= ", canUseCacheQuery )
    LOGS_QDEBUG_2( "logs [FINDER] areAllResultsCached= ", areAllResultsCached )
    LOGS_QDEBUG_2( "logs [FINDER] cachedCounter= ", mCachedCounter )
    
    mCurrentPredictivePattern = pattern.left(MaxPredSearchPatternLen);
    
    if ( mCurrentPredictivePattern.isEmpty() ) {
        qDeleteAll( mResults );
        mResults.clear();
        mCachedCounter = 0;
    } else if ( areAllResultsCached &&
                canUseCacheQuery ) { 
        doPredictiveCacheQuery();
    } else {        
        mCachedCounter = 0;
        LogsCntEntryList recentResults = mResults;
        mResults.clear();
        doPredictiveHistoryQuery();
        doPredictiveContactQuery( recentResults );
        qDeleteAll( recentResults );
    }
    emit queryReady();
 
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::predictiveSearchQuery()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveHistoryQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveHistoryQuery()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveHistoryQuery()" )
    
    QListIterator<LogsCntEntry*> iter(mHistoryEvents);
    
    while( iter.hasNext() ) {
        LogsCntEntry* e = iter.next();
        if ( e->match( mCurrentPredictivePattern ) ) {
            LogsCntEntry* entry = new LogsCntEntry( *e );
            addResult( entry );
        }
    }
        
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveHistoryQuery()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveContactQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveContactQuery( LogsCntEntryList& recentResults )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveContactQuery()" )
    QContactDetailFilter df;
    df.setDetailDefinitionName( QContactName::DefinitionName );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( mCurrentPredictivePattern );
    QList<QContactLocalId> cntIds;
    LOGS_QDEBUG( "logs [FINDER] about to call contacts manager" )
    
    cntIds = mContactManager->contactIds( df );
    LOGS_QDEBUG_2( "logs [FINDER] number of matched contacts =", cntIds.count() )
    int index = 0;
    while( index < cntIds.count() ) {
        addResult( cntIds.at( index++ ), recentResults );
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveContactQuery()" )
    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveCacheQuery()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveCacheQuery()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveCacheQuery()" )
    QMutableListIterator<LogsCntEntry*> iter(mResults);
    while( iter.hasNext() ) {
        LogsCntEntry* entry = iter.next();
        if ( !entry->match( mCurrentPredictivePattern ) ) {
            mCachedCounter = 
                    entry->isCached() ? mCachedCounter-1 : mCachedCounter;
            iter.remove();
            delete entry;
        } else {
            entry->setHighlights( mCurrentPredictivePattern );
        }
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveCacheQuery()" )
    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::addResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::addResult( quint32 cntId, LogsCntEntryList& recentResults )
{
    QMutableListIterator<LogsCntEntry*> iter(recentResults);
    bool reused = false;
    while( iter.hasNext() && !reused ) {
        LogsCntEntry* entry = iter.next();
        if ( entry->contactId() == cntId ) {
            LOGS_QDEBUG_4( "logs [FINDER] LogsCntFinder::addResult() - \
re-using entry. contact id ", cntId, "cached=", entry->isCached() );
            iter.remove();
            addResult( entry );
            reused = true;
        }
    }
    
    if ( !reused ) {
        LogsCntEntry* entry = new LogsCntEntry( cntId );
        addResult( entry );
    }
}


// -----------------------------------------------------------------------------
// LogsCntFinder::addResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::addResult( LogsCntEntry* entry )
{
    updateResult( entry );
    mResults.append( entry );
}

// -----------------------------------------------------------------------------
// LogsCntFinder::updateResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::updateResult( LogsCntEntry* entry )
{
    if ( entry->isCached() ) {
        entry->setHighlights( mCurrentPredictivePattern );
        mCachedCounter++;
    }
}


// -----------------------------------------------------------------------------
// LogsCntFinder::resultsCount
// -----------------------------------------------------------------------------
//
int LogsCntFinder::resultsCount() const
{
    return mResults.count();
}

// -----------------------------------------------------------------------------
// LogsCntFinder::resultAt
// -----------------------------------------------------------------------------
//
const LogsCntEntry& LogsCntFinder::resultAt( int index )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::resultAt()" )
    LOGS_QDEBUG_2( "logs [FINDER] index=", index )
    
    LogsCntEntry* entry = mResults.at( index );
    if ( !entry->isCached() ) {
        LOGS_QDEBUG_2( "logs [FINDER] caching from DB cid=", entry->contactId() )
        QContact contact = mContactManager->contact( entry->contactId() );
        QContactName contactName = contact.detail( QContactName::DefinitionName );
        entry->setFirstName( contactName.value( QContactName::FieldFirst ) );
        entry->setLastName( contactName.value( QContactName::FieldLast ) );
        QContactPhoneNumber contactPhoneNumber = 
              contact.detail( QContactPhoneNumber::DefinitionName );
        entry->setPhoneNumber( 
              contactPhoneNumber.value( QContactPhoneNumber::FieldNumber ) );
        QContactAvatar contactAvatar = contact.detail<QContactAvatar>();  
        if (contactAvatar.subType().compare(
        QLatin1String(QContactAvatar::SubTypeImage)) == 0 && 
               !contactAvatar.avatar().isEmpty()) {
                  entry->setAvatarPath(contactAvatar.avatar());
              } 
        
        updateResult( entry );      
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::resultAt()" )
    return *entry;
}


// -----------------------------------------------------------------------------
// LogsCntFinder::insertEntry
// -----------------------------------------------------------------------------
//
void LogsCntFinder::insertEntry( int index, LogsCntEntry* entry )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::insertEntry()" )
    LOGS_QDEBUG_4( "logs [FINDER] handle=", entry->handle()," to index ", index )
    
    mHistoryEvents.insert( index, entry );
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::insertEntry()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::getEntry
// -----------------------------------------------------------------------------
//
LogsCntEntry* LogsCntFinder::getEntry( const LogsCntEntryHandle& handle ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::getEntry()" )
    return doGetEntry( mHistoryEvents, handle );      
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doGetEntry
// -----------------------------------------------------------------------------
//
LogsCntEntry* LogsCntFinder::doGetEntry( const LogsCntEntryList& list, 
                                         const LogsCntEntryHandle& handle ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doGetEntry()" )
    LOGS_QDEBUG_2( "logs [FINDER] handle= ", &handle )
    
    LogsCntEntry* entry = 0;
    QListIterator<LogsCntEntry*> iter(list);
    
    while( iter.hasNext() && !entry ) {
        LogsCntEntry* e = iter.next();
        entry = e->handle() == &handle ? e : 0;
    }
    
    LOGS_QDEBUG_2( "logs [FINDER] found=", (entry!=0) )
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doGetEntry()" )
    return entry;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::deleteEntry
// -----------------------------------------------------------------------------
//
void LogsCntFinder::deleteEntry( const LogsCntEntryHandle& handle )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::deleteEntry()" )
    LOGS_QDEBUG_2( "logs [FINDER] handle= ", &handle )
    
    LogsCntEntry* toRemoveHistoryEv = doGetEntry( mHistoryEvents, handle );
    mHistoryEvents.removeOne( toRemoveHistoryEv );
    delete toRemoveHistoryEv;
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::deleteEntry()" )
    
}




