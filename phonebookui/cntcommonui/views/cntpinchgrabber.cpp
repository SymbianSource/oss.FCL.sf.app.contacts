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

#include "cntpinchgrabber.h"
#include "cntnamesviewitem.h"

#include <cntuids.h>
#include <hbpinchgesture.h>
#include <hblistview.h>
#include <xqsettingskey.h>
#include <QGestureEvent>
#include <QTimer>

CntPinchGrabber::CntPinchGrabber(HbListView *parent) :
    QObject(parent),
    mSettingsKey(NULL),
    mListView(parent),
    mCurrentSetting(-1),
    mUpdated(false)
{
    mSettingsKey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
                            KCRCntSettings.iUid,
                            KCntNameListRowSetting);
}

CntPinchGrabber::~CntPinchGrabber()
{
    delete mSettingsKey;
}

bool CntPinchGrabber::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    
    if (event->type() == QEvent::TouchBegin)
    {
        event->accept();
        return true;
    }

    if (event->type() == QEvent::Gesture)
    {
        QGestureEvent *ge = static_cast<QGestureEvent*>(event);
        
        if (HbPinchGesture *pinch = qobject_cast<HbPinchGesture *>(ge->gesture(Qt::PinchGesture)))
        {
            QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
            
            switch (pinch->state())
            {
            case Qt::GestureStarted:
                ge->accept(pinch);
                mCurrentSetting = mSettings.readItemValue(*mSettingsKey, XQSettingsManager::TypeInt).toInt();
                mUpdated = false;
                break;
            case Qt::GestureUpdated:
                if (changeFlags & QPinchGesture::ScaleFactorChanged && !mUpdated)
                {
                    if (pinch->scaleFactor() < CNT_NAME_LIST_LAYOUT_CHANGE_PINCH_IN_TRESHOLD && mCurrentSetting != CntOneRowNameOnly)
                    {
                        mSettings.writeItemValue(*mSettingsKey, QVariant(CntOneRowNameOnly));
                        mUpdated = true;
                        mListView->listItemPrototype()->setProperty("animationState", CNT_NAME_LIST_LAYOUT_PROPERTY_ANIMATE);
                    }
                    else if (pinch->scaleFactor() > CNT_NAME_LIST_LAYOUT_CHANGE_PINCH_OUT_TRESHOLD && mCurrentSetting != CntTwoRowsNameAndPhoneNumber)
                    {
                        mSettings.writeItemValue(*mSettingsKey, QVariant(CntTwoRowsNameAndPhoneNumber));
                        mUpdated = true;
                        mListView->listItemPrototype()->setProperty("animationState", CNT_NAME_LIST_LAYOUT_PROPERTY_ANIMATE);
                    }
                }
                break;
            case Qt::GestureFinished:
            case Qt::GestureCanceled:
            default:
                mCurrentSetting = -1;
                if (mUpdated)
                {
                    mListView->ungrabGesture(Qt::PinchGesture);
                    QTimer::singleShot(CNT_NAME_LIST_LAYOUT_CHANGED_TIMEOUT, this, SLOT(timerTimeout()));
                }
                break;
            }
            
            return true;
        }
    }
    
    return false;
}

void CntPinchGrabber::timerTimeout()
{
    mListView->listItemPrototype()->setProperty("animationState", CNT_NAME_LIST_LAYOUT_PROPERTY_NORMAL);
    mListView->grabGesture(Qt::PinchGesture);
}

