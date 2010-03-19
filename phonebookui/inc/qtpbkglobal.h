/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HbGlobal contains global LibHb declarations
*
*/


#ifndef QTPBKGLOBAL_H
#define QTPBKGLOBAL_H

#include <QtGlobal>
#include <QString>


#include <QFile>
#include <QDir>
#include <qdatetime.h>
#include <QTextStream>
#include <QDebug>
#include <QMap>
#include <QMutex>

#ifdef PBK_UNIT_TEST
#define QTPBK_EXPORT
#else
#ifdef BUILD_QTPBK
#define QTPBK_EXPORT Q_DECL_EXPORT
#else
#define QTPBK_EXPORT Q_DECL_IMPORT
#endif
#endif

#if defined(EXPORT_PRIVATE)
#define QTPBK_PRIVATE_EXPORT HB_EXPORT
#else
#define QTPBK_PRIVATE_EXPORT
#endif

#define __QTPBK_WRITE_TO_PERF_LOG_START(p) WriteToPerfLog(p,-1);
#define __QTPBK_WRITE_TO_PERF_LOG_POINT(p,v) WriteToPerfLog(p,v);
#define __QTPBK_WRITE_TO_PERF_LOG_END(p) WriteToPerfLog(p,100);
#define __QTPBK_WRITE_TO_PERF_FILE() WriteToFile();

//
// undef this to remove performance logging
//
#define QTPBK_PERF_LOGGING
#ifdef QTPBK_PERF_LOGGING

static QMap<QTime, QString> mLogMap;
static QMutex mMutex;

inline void WriteToPerfLog(QString p, int v)
    {
    mMutex.lock();
    QString function;
    if (v==-1)
        {
        function = "Function: " + p + "(START)";
        }
    else if(v==100)
        {
        function = "Function: " + p + "(END)";
        }
    else
        {
        QString value;
        value.setNum(v,10);
        function = "Function: " + p + "(POINT): " + value;
        }

    QTime time = QTime::currentTime();
    mLogMap.insertMulti(time, function);
    mMutex.unlock();
    qDebug() << "QtPhonebook(perf): " << function << "Time: " << time.toString() << "." << time.msec();
    }

inline void WriteToFile()
    {
    qDebug() << "Writing to perflog " << QDir::rootPath() + "QT_PBK_perflog.txt";
    QFile file("c:/QT_PBK_perflog.txt");
    if ( file.exists() )
        file.remove();
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append ))
        return;
    QTextStream out(&file);
    QMapIterator<QTime, QString> i(mLogMap);
    while (i.hasNext())
        {
        i.next();
        out << i.key().toString() << "." << i.key().msec() << ": " << i.value() << endl;
        }

    file.close();
    }

#else

inline void WriteToPerfLog(QString p, int v)
    {
    Q_UNUSED(s);
    }

#endif // QTPBK_PERF_LOGGING

#endif // QTPBKGLOBAL_H
