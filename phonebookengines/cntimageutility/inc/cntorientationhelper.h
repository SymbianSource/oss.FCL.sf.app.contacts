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

#ifndef CNTORIENTATIONHELPER_H_
#define CNTORIENTATIONHELPER_H_

#include "cntimageutilityglobal.h"

#include <QObject>
#include <xqsettingskey.h>
#include <xqsettingsmanager.h>

class CNTIMAGEUTILITYLIB_EXPORT CntOrientationHelper : public QObject
{
    Q_OBJECT
public:
    CntOrientationHelper( QObject* parent = NULL );
    ~CntOrientationHelper();
    
    Qt::Orientation& orientation();
    
signals:
    void orientationChanged( Qt::Orientation orientation );
    
private slots:
    void emitOrientationChanged( const XQSettingsKey& key, const QVariant& value );
    
private:
    XQSettingsManager* mSettings; // own
    XQSettingsKey* mOrientationKey; // own
    Qt::Orientation mOrientation;
    
    friend class TestCntOrientationHelper;
};
#endif /* CNTORIENTATIONHELPER_H_ */
