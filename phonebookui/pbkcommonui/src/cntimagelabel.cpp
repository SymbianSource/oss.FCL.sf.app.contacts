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
#include <QGraphicsSceneMouseEvent>

CntImageLabel::CntImageLabel(QGraphicsItem *parent) :
    HbLabel(parent)
{
    grabGesture(Qt::TapGesture);
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
                    emit iconLongPressed(tap->position());
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

