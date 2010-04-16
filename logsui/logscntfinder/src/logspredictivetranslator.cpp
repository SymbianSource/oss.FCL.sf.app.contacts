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
#include <QObject>
#include <QLocale>
#include <hbinputkeymapfactory.h>
#include <hbinputkeymap.h>
#include <hbinputsettingproxy.h>

#include "logspredictivetranslator.h"
#include "logslogger.h"

LogsPredictiveTranslator* LogsPredictiveTranslator::mInstance = 0;


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::instance()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator* LogsPredictiveTranslator::instance()
{
    if ( !mInstance ) {
        mInstance = new LogsPredictiveTranslator();
    }
    return mInstance;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::deleteInstance()
// -----------------------------------------------------------------------------
//
void LogsPredictiveTranslator::deleteInstance()
{
    delete mInstance;
    mInstance = 0;
}


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::LogsPredictiveTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator::LogsPredictiveTranslator()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::\
LogsPredictiveTranslator()" )
    HbInputLanguage lang = 
            HbInputSettingProxy::instance()->globalInputLanguage();
    mKeyMap = HbKeymapFactory::instance()->keymap( lang.language(), 
                                                   lang.variant() );
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
LogsPredictiveTranslator()" )
}


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::~LogsPredictiveTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator::~LogsPredictiveTranslator()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::\
~LogsPredictiveTranslator()" )
    mInstance = 0;
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
~LogsPredictiveTranslator()" )
    
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::translate()
// -----------------------------------------------------------------------------
//
const QChar LogsPredictiveTranslator::translate( const QChar character ) const
{
    const HbMappedKey* mappedKey = mKeyMap->keyForCharacter( HbKeyboardVirtual12Key, 
                                     character );
    if (!mappedKey) {
        QString decomposed = character.decomposition();
        if (decomposed.isEmpty()) {
            return character;
        }
        return translate (decomposed.at(0));
    }
    return mappedKey->keycode;
}


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::translate()
// -----------------------------------------------------------------------------
//
const QString LogsPredictiveTranslator::translate( const QString& name, 
                                                   int count ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::translate()" )
    LOGS_QDEBUG_2( "logs [FINDER] name ", name );
    
    count = count == -1 ? name.length() : count;
    QString result;
    const QChar* content = name.data();
    int index = 0;
    while( index < name.length() && index < count ) {
        result.insert( index++, translate( *content++ ) );
    }
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::translate()" )
    return result;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::startsWith()
// -----------------------------------------------------------------------------
//
int LogsPredictiveTranslator::startsWith( const QString& text, 
                                          const QString& pattern,
                                          bool optimize ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::startsWith()" )
    //assumed that text has found based on pattern, thus only checking with
    //first char is enough, if mightContainZeroes eq false
    int matchCount = pattern.length();
    if ( text.isEmpty() || matchCount > text.length() ) {
        matchCount = 0;
    } else {
        if ( !optimize ) {
            QString translatedText = translate( text, pattern.length() );
            matchCount = translatedText == pattern ? matchCount : 0; 
        } else {
            matchCount = translate( *text.data() ) == *pattern.data() ? 
                         matchCount : 0;
        }
    }
        
    LOGS_QDEBUG_2( "logs [FINDER] matchCount=", matchCount )
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::startsWith()" )
    return matchCount;
}

