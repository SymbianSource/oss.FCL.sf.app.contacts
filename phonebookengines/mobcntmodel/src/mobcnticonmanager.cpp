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

#include "mobcnticonmanager.h"
#include <thumbnailmanager_qt.h>
#include <QDebug>

const int KMaxQueueSize = 25;
const int KContactFetchTimer1 = 400;
const int KContactFetchTimer2 = 20;
/*!
 Constructor
 */
MobCntIconManager::MobCntIconManager(QObject *parent)
    : QObject(parent),
      mQueueCount(0)
{
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);

    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)));
             
    mTimer = new QTimer();
    connect( mTimer, SIGNAL(timeout()), this, SLOT(timerTimeout()) );
                      
}

/*!
 Destructor
 */
MobCntIconManager::~MobCntIconManager()
{
    cancel();
    mImageCache.clear();
    delete mTimer;
}

/*!
Get the icon for the requested avatarPath or send a new request to the thumbnailmanager if it doesn't exist yet
*/
QIcon MobCntIconManager::contactIcon(const QString &avatarPath, int index)
{
    QIcon icon;
    if ( mImageCache.contains(avatarPath) )
    {
        icon = mImageCache.value(avatarPath);
		qDebug() << "MobCntIconManager::contactIcon:: image was cached";
    }
    else
    {      
        mTimer->stop();
        mTimer->start(KContactFetchTimer1); 
        mQueueCount++;
        mRequestQueue.enqueue(qMakePair(avatarPath, index));
        if( mQueueCount > KMaxQueueSize )
        {
           mRequestQueue.dequeue();
           mQueueCount--;
        }        
    }
    return icon;
}

/*!
Cancel all requests
*/
void MobCntIconManager::cancel()
{
    if (!mTnmReqMap.empty())
    {
        QMapIterator<int, QString> iter(mTnmReqMap);
        while (iter.hasNext())
        {
            iter.next();
            bool result = mThumbnailManager->cancelRequest(iter.key());
        }
    }
    mTnmReqMap.clear();
    mRequestQueue.clear();
    mQueueCount = 0;
}

/*!
Called when thumbnailmanager finishes creating a thumbnail, emits a signal for MobCntModel to update icon for a contact
*/
void MobCntIconManager::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
	qDebug() << "MobCntIconManager::thumbnailReady:: error is " << error;
	
    // Find the index
    if (mTnmReqMap.contains(id))
    {
        QString avatarPath = mTnmReqMap[id];
        mTnmReqMap.remove(id);
        if ( error == 0 )
        {
            int *clientData = (int *)data;
            int index = *clientData;
			qDebug() << "MobCntIconManager::thumbnailReady:: data is " << index;
            QIcon icon(pixmap);
            mImageCache.insert(avatarPath, icon);
            emit contactIconReady(index);
			qDebug() << "MobCntIconManager::thumbnailReady - signal emitted";
            if (!mRequestQueue.isEmpty())
            {
                mTimer->start(KContactFetchTimer2);                
            }
    
            delete clientData;
            
        } else {
        	  thumbnailLoad();
        }
    }
}

void MobCntIconManager::thumbnailLoad()
{
	mTimer->stop();
    if (!mRequestQueue.isEmpty())
    { 
		mQueueCount--;
        QPair<QString, int> req = mRequestQueue.dequeue();
        QString avatarPath = req.first;
        int index = req.second;
        int *clientData = new int(index);
        int reqId = mThumbnailManager->getThumbnail(avatarPath, clientData, 0);
        mTnmReqMap.insert(reqId, avatarPath);
    }
 
}

void MobCntIconManager::timerTimeout()
{
    thumbnailLoad();	
}


