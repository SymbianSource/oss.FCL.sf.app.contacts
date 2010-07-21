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

#include "cntimagelabel.h"

#include <hbinstantfeedback.h>
#include <hbtapgesture.h>
#include <hbeffect.h>
#include <QGraphicsSceneMouseEvent>

CntImageLabel::CntImageLabel(QGraphicsItem *parent) :
    HbLabel(parent)
{
    grabGesture(Qt::TapGesture);
    HbEffect::add(this, "groupbox_icon_click", "iconclick");
}

CntImageLabel::~CntImageLabel()
{

}

void CntImageLabel::gestureEvent(QGestureEvent* event)
{ 
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) 
    {    
        switch (tap->state()) 
        {
            case Qt::GestureStarted:
                HbEffect::start(this, QString("iconclick"));
                break;
            case Qt::GestureFinished:
                if (tap->tapStyleHint() == HbTapGesture::Tap)
                {
                    HbInstantFeedback::play(HbFeedback::Basic);
                    emit iconClicked();
                }
                break;
            case Qt::GestureUpdated:
                if (tap->tapStyleHint() == HbTapGesture::TapAndHold) 
                {
                    HbEffect::cancel(this, QString("iconclick"));
                    emit iconLongPressed(tap->scenePosition());
                }      
                break;
            default:
                break;
        }
        event->accept();
    } 
    else 
    {
        event->ignore();
    }
}

