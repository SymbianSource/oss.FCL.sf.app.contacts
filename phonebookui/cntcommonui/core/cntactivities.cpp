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

#include "cntactivities.h"
#include "cntglobal.h"
#include <cntdebug.h>
#include <afactivation.h>
#include <afactivitystorage.h>
#include <QPixmap>


/*!
Constructor, initialize member variables.
*/
CntActivities::CntActivities()
{
    mActivation = new AfActivation();
    mActivityStorage = new AfActivityStorage();
}

/*!
Destructor
*/
CntActivities::~CntActivities()
{
    delete mActivation;
    delete mActivityStorage;
}

bool CntActivities::loadActivity(QByteArray &serializedModel)
{   
    bool isActityStartup = false;
    if (mActivation->reason() == Af::ActivationReasonActivity)
    {
        QVariant data = mActivityStorage->activityData(mActivation->name());       
        // restore state from activity data           
        serializedModel = data.toByteArray();
        isActityStartup = true;
    }
    
    // clean up all activities from the activity manager.
    removeActivities();
    
    return isActityStartup;
}

void CntActivities::saveActivity(const QString &name, const QByteArray &serializedActivity, const QPixmap &screenshot)
{ 
    // clean up all activities from the activity manager.
    removeActivities();
    
    // do not save empty activity
    if (name.isEmpty())
        return;
    
    // screen shot
    QVariantHash metadata;
    metadata.insert("screenshot", screenshot);
    
    // add the activity to the activity manager
    bool ok = mActivityStorage->saveActivity(name, serializedActivity, metadata);  
    if ( !ok )
    {
        qFatal("Add failed");
    }
}

void CntActivities::removeActivities()
{ 
    QStringList activityList = mActivityStorage->allActivities();
    foreach (QString activity, activityList)
    {            
        mActivityStorage->removeActivity(activity);
    }
}

