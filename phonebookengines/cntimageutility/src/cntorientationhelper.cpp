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

#include "cntorientationhelper.h"
#include <restricted/hbcorepskeys_r.h>

CntOrientationHelper::CntOrientationHelper( QObject* parent ) : 
QObject( parent ),
mSettings( NULL ),
mOrientationKey( NULL )
{
    mSettings = new XQSettingsManager();
    
    mOrientationKey = new XQSettingsKey( XQSettingsKey::TargetPublishAndSubscribe, 
        KHbPsForegroundAppOrientationCategoryUid.iUid, 
        KHbPsForegroundAppOrientationKey);
    
    connect( mSettings, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)),
             this, SLOT(emitOrientationChanged(const XQSettingsKey&, const QVariant&)) );

    int orientation = mSettings->readItemValue(*mOrientationKey, XQSettingsManager::TypeInt).toInt(); 
    mOrientation = Qt::Orientation(orientation & 0x7F);

    mSettings->startMonitoring( *mOrientationKey, XQSettingsManager::TypeInt );
}

CntOrientationHelper::~CntOrientationHelper()
{
    mSettings->stopMonitoring( *mOrientationKey );
    
    delete mSettings;
    delete mOrientationKey;
}

Qt::Orientation& CntOrientationHelper::orientation()
{
    return mOrientation;
}

void CntOrientationHelper::emitOrientationChanged( const XQSettingsKey& key, const QVariant& value )
{
    if ( key.uid() == mOrientationKey->uid() && key.key() == mOrientationKey->key() ) 
    {
        bool ok;
        int orientation = value.toInt( &ok );
        if ( ok ) {
            // Bits 0-7 contain the Qt::Orientation value.
            // If bit 8 is set then the orientation is a fixed (forced) one.
            // If bit 8 is not set then the orientation is managed automatically by the framework.
            mOrientation = Qt::Orientation( orientation & 0x7F ); 
            emit orientationChanged( mOrientation );   
        }
    }
}
