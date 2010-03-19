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

#ifndef LOGSPREDICTIVETRANSLATOR_H
#define LOGSPREDICTIVETRANSLATOR_H

#include <QObject>
#include <QChar>

class HbKeymap;

/**
 * predictive translator. Singelton
 *
 */
class LogsPredictiveTranslator : public QObject 
{

    Q_OBJECT

public: 

    static LogsPredictiveTranslator* instance();
    static void deleteInstance();
    
    ~LogsPredictiveTranslator();
    
    const QString translate( const QString& name, int count = -1 ) const;
    int startsWith( const QString& text, const QString& pattern, bool optimize = true ) const;
    
    
private:

    explicit LogsPredictiveTranslator();
    const QChar translate( const QChar character ) const;
    

private:
    
    static LogsPredictiveTranslator* mInstance;
    const HbKeymap* mKeyMap;
    
    friend class UT_LogsPredictiveTranslator;
    
};

#endif //LOGSPREDICTIVETRANSLATOR_H
