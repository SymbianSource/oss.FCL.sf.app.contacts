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

#include "cnthistoryviewitemwidget.h"

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>

#define INCOMING_EVENT_FRAME "qtg_fr_convlist_received_normal"
#define OUTGOING_EVENT_FRAME "qtg_fr_convlist_sent_normal"

CntHistoryViewItemWidget::CntHistoryViewItemWidget(QGraphicsItem *parent) :
    HbWidget(parent),
    mIconLabel(0),
    mTitleLabel(0),
    mBodyTextLabel(0),
    mTimeStampLabel(0),
    mFrameLabel(0)
{
}

CntHistoryViewItemWidget::~CntHistoryViewItemWidget()
{
}

void CntHistoryViewItemWidget::createPrimitives()
{
    //create frame first so it's painted below text labels
    if (!mFrameLabel) {
        HbFrameDrawer* frameDrawer = 0;
        if (mIncoming) {
            frameDrawer = new HbFrameDrawer(INCOMING_EVENT_FRAME, HbFrameDrawer::NinePieces);
            }
        else {
            frameDrawer = new HbFrameDrawer(OUTGOING_EVENT_FRAME, HbFrameDrawer::NinePieces);
        }
        mFrameLabel = new HbFrameItem(frameDrawer, this);
        style()->setItemName(mFrameLabel, "frame");
    }

    //create icon
    if (!mIcon.isNull()) {
        if (!mIconLabel) {
            mIconLabel = new HbIconItem(this);
            mIconLabel->setIcon(mIcon);
            style()->setItemName(mIconLabel, "icon");
        }
    }
    else {
        if (mIconLabel) {
            delete mIconLabel;
        }
        mIconLabel = 0;
    }

    //create title
    if (!mTitle.isNull()) {
        if (!mTitleLabel) {
            mTitleLabel = new HbTextItem(this);
            mTitleLabel->setText(mTitle);
            mTitleLabel->setTextWrapping(Hb::TextWordWrap);
            style()->setItemName(mTitleLabel, "title");
        }
    }
    else {
        if (mTitleLabel) {
            delete mTitleLabel;
        }
        mTitleLabel = 0;
    }

    //create body text
    if (!mBodyText.isNull()) {
        if (!mBodyTextLabel) {
            mBodyTextLabel = new HbTextItem(this);
            mBodyTextLabel->setText(mBodyText);
            style()->setItemName(mBodyTextLabel, "bodyText");
        }
    }
    else {
        if (mBodyTextLabel) {
            delete mBodyTextLabel;
        }
        mBodyTextLabel = 0;
    }

    //create timestamp
    if (!mTimeStamp.isNull()) {
        if (!mTimeStampLabel) {
            mTimeStampLabel = new HbTextItem(this);
            mTimeStampLabel->setText(mTimeStamp);
            style()->setItemName(mTimeStampLabel, "timeStamp");
        }
    }
    else {
        if (mTimeStampLabel) {
            delete mTimeStampLabel;
        }
        mTimeStampLabel = 0;
    }
}

void CntHistoryViewItemWidget::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIconLabel;
    mIconLabel = 0;

    delete mTitleLabel;
    mTitleLabel = 0;

    delete mBodyTextLabel;
    mBodyTextLabel = 0;

    delete mTimeStampLabel;
    mTimeStampLabel = 0;
    
    createPrimitives();
}


void CntHistoryViewItemWidget::updatePrimitives()
{
    HbWidget::updatePrimitives();
}

void CntHistoryViewItemWidget::setDetails(QString title, QString bodyText,
        QString timeStamp, QString iconName, bool incoming)
{
    mIcon.clear();    
    mTitle.clear();
    mBodyText.clear();
    mTimeStamp.clear();

    mIcon.setIconName(iconName);
    mTitle = title;
    mBodyText = bodyText; 
    mTimeStamp = timeStamp;
    mIncoming = incoming;

    recreatePrimitives();
}
