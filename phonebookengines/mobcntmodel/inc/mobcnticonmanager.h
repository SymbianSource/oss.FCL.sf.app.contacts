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
* Description: Music Player collection album art manager.
*
*/


#ifndef MOBCNTICONMANAGER_H
#define MOBCNTICONMANAGER_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QIcon>
#include <QTimer>

class ThumbnailManager;

class MobCntIconManager : public QObject
{
    Q_OBJECT

public:

    explicit MobCntIconManager(QObject *parent=0);
    virtual ~MobCntIconManager();

    QIcon contactIcon(const QString &avatarPath, int index);
    void cancel();

signals:
    void contactIconReady(int index);

public slots:
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void thumbnailLoad();
    void timerTimeout();

private:
    ThumbnailManager                *mThumbnailManager;
    QHash<QString, QIcon>           mImageCache;
    QMap<int, QString>              mTnmReqMap;

    QQueue< QPair<QString, int> >   mRequestQueue;
    int                             mQueueCount;
    QTimer*                         mTimer;

};

#endif // MOBCNTICONMANAGER_H

