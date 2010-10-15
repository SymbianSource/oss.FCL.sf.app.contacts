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

#ifndef CNTPINCHGRABBER_H
#define CNTPINCHGRABBER_H

#include <QObject>

#include <xqsettingsmanager.h>

class XQSettingsKey;
class HbListView;

class CntPinchGrabber : public QObject
{
    friend class TestCntPinchGrabber;
    Q_OBJECT

public:
    CntPinchGrabber(HbListView *parent = 0);
    ~CntPinchGrabber();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    
private slots:
    void timerTimeout();
    
private:
    XQSettingsManager   mSettings;
    XQSettingsKey*      mSettingsKey;
    HbListView*         mListView;
    int                 mCurrentSetting;
    bool                mUpdated;
};

#endif /* CNTPINCHGRABBER_H */
