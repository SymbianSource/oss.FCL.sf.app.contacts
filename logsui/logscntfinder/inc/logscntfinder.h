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

#ifndef LOGSCNTFINDER_H
#define LOGSCNTFINDER_H

#include <logscntfinderexport.h>
#include <QObject>
#include <QList>
#include <QString>
#include <qmobilityglobal.h>


class LogsCntEntry;
typedef QObject LogsCntEntryHandle;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class LogsCntText
{
public:

    inline LogsCntText() : mHighlights(0){}
    inline const QString& text() const {return mText;}
    inline int highlights() const {return mHighlights;}
    LOGSCNTFINDER_EXPORT QString richText( QString startTag = QString("<b><u>"), 
                                           QString endTag = QString("</u></b>")) const;
    
    
private:
    
    QString mText;
    QString mTranslatedText;
    int mHighlights;
    
    friend class LogsCntEntry;
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
};

typedef QList<LogsCntText> LogsCntTextList;

class LogsCntEntry
{
public:

    enum EntryType {
        EntryTypeHistory,
        EntryTypeContact
    };
    
    LOGSCNTFINDER_EXPORT LogsCntEntry( LogsCntEntryHandle& handle, 
                                       quint32 cid );
    LogsCntEntry( quint32 cid );
    LogsCntEntry( const LogsCntEntry& entry );
    ~LogsCntEntry();
    LOGSCNTFINDER_EXPORT EntryType type() const;
    LOGSCNTFINDER_EXPORT const LogsCntTextList& firstName() const;
    LOGSCNTFINDER_EXPORT const LogsCntTextList& lastName() const;
    LOGSCNTFINDER_EXPORT quint32 contactId() const;
    LOGSCNTFINDER_EXPORT LogsCntEntryHandle* handle() const;
    LOGSCNTFINDER_EXPORT void setFirstName( const QString& name );
    LOGSCNTFINDER_EXPORT void setLastName( const QString& name );
    LOGSCNTFINDER_EXPORT const LogsCntText& phoneNumber() const;
    LOGSCNTFINDER_EXPORT const QString& avatarPath() const;
    LOGSCNTFINDER_EXPORT void setPhoneNumber( const QString& number );
    LOGSCNTFINDER_EXPORT void setAvatarPath( const QString& avatarpath );
    LOGSCNTFINDER_EXPORT const QString& speedDial() const;
    
    bool isCached() const;
    void setHighlights( const QString& pattern );
    void setSpeedDial( const QString& number );
    bool match( const QString& pattern ) const;

private:
    
    void doSetText( const QString& text, LogsCntTextList& textlist ); 
    QStringList tokens( const QString& source, const QChar& separ = ' ' ) const;
    void doSetHighlights( const QString& pattern, LogsCntTextList& nameArray );
    void resetHighlights( LogsCntTextList& nameArray );
    bool doSimpleMatch( const QString& pattern ) const;
    bool doComplexMatch( QStringList patternArray ) const;

private:
    
    EntryType mType;
    quint32 mCid;
    LogsCntTextList mFirstName;
    LogsCntTextList mLastName;
    bool mCached;
    LogsCntEntryHandle* mHandle;
    LogsCntText mPhoneNumber;
    QString mAvatarPath;
    QString mSpeedDial;
    
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
};

typedef QList<LogsCntEntry*> LogsCntEntryList;
    
/**
 * Log events and contacts finder
 *
 */
class LogsCntFinder : public QObject 
{

    Q_OBJECT
    
public: // The exported API

    LOGSCNTFINDER_EXPORT LogsCntFinder();
    LOGSCNTFINDER_EXPORT LogsCntFinder(QContactManager& contactManager);
    LOGSCNTFINDER_EXPORT ~LogsCntFinder();

    /**
    * Starts/continues predictive query based on pattern. If
    * there is a previously executed query with same pattern, 
    * call is treated as continue query.
    * @param pattern the predictive pattern, containing digit(s)
    */
    LOGSCNTFINDER_EXPORT 
        void predictiveSearchQuery( const QString& pattern );
                                      
    /**
    * returns number of results
    * @return number of results
    */
    LOGSCNTFINDER_EXPORT 
      int resultsCount() const;
    
    /**
    * returns result at index
    * @param index the index
    */
    LOGSCNTFINDER_EXPORT 
      const LogsCntEntry& resultAt( int index );
    
    /**
    * Used for adding entiries to be part of a query
    * Ownership is transfered
    * @param entry the entry
    */
    LOGSCNTFINDER_EXPORT 
      void insertEntry( int index, LogsCntEntry* entry ); 
    
    /**
    * Used for updating entiries
    * @param handle the handle
    */
    LOGSCNTFINDER_EXPORT 
        LogsCntEntry* getEntry( const LogsCntEntryHandle& handle ) const; 
    
    /**
    * Used for updating entiries
    * @param entry the entry
    */
    LOGSCNTFINDER_EXPORT 
        void deleteEntry( const LogsCntEntryHandle& handle );
    
signals:

    /**
    * emitted when query is ready
    */
    void queryReady();
    
private:

    void doPredictiveHistoryQuery();
    void doPredictiveContactQuery();
    
    LogsCntEntry* doGetEntry( const LogsCntEntryList& list, 
                              const LogsCntEntryHandle& handle ) const;
    
    
private:
    
    QString mCurrentPredictivePattern;
    LogsCntEntryList mResults;
    QContactManager* mContactManager;
    LogsCntEntryList mHistoryEvents;
    
    friend class UT_LogsCntFinder;
    
};

#endif //LOGSCNTFINDER_H
