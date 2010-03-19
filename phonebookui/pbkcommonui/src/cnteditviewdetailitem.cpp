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

#include "cnteditviewdetailitem.h"

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbeffect.h>
#include <hbgesturefilter.h>
#include <hbgesture.h>
#include <hbinstantfeedback.h>

#include <QGraphicsSceneMouseEvent>

CntEditViewDetailItem::CntEditViewDetailItem(QGraphicsItem *parent):
        HbWidget(parent),
        mIcon(0),
        mLabel(0),
        mContent(0),
        mFrameItem(0),
        mFocusItem(0),
        mHasFocus(false),
        mGestureFilter(0),
        mGestureLongpressed(0)
{
    HbEffect::add(QString("frameitem"), QString(":/xml/edit_button_pressed.fxml"), QString("pressed"));
    HbEffect::add(QString("frameitem"), QString(":/xml/edit_button_released.fxml"), QString("released"));

    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFocusPolicy(Qt::StrongFocus);
    initGesture();
}

CntEditViewDetailItem::~CntEditViewDetailItem()
{

}

void CntEditViewDetailItem::createPrimitives()
{
    if (!icon.isNull())
    {
        if (!mIcon)
        {
            mIcon = new HbIconItem(this);
            mIcon->setIcon(icon);
            style()->setItemName(mIcon, "icon");
        }
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = 0;
    }

    if (!text.isNull())
    {
        if (!mLabel)
        {
            mLabel = new HbTextItem(this);
            mLabel->setText(text);
            style()->setItemName(mLabel, "text");
        }
    }
    else
    {
        if (mLabel)
        {
            delete mLabel;
        }
        mLabel = 0;
    }

    if (!valueText.isNull())
    {
        if (!mContent)
        {
            mContent = new HbTextItem(this);
            mContent->setText(valueText);
            mContent->setTextWrapping(Hb::TextNoWrap);
            style()->setItemName(mContent, "valueText");
            
            // make sure phone number truncation characters are placed
            // on left side of the phone number.
            if ( mDetail.definitionName() == QContactPhoneNumber::DefinitionName )
                {
                mContent->setElideMode( Qt::ElideLeft );
                }
        }
    }
    else
    {
        if (mContent)
        {
            delete mContent;
        }
        mContent = 0;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_normal");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }

    if (!mFocusItem)
    {
        mFocusItem = new HbFrameItem(this);
        mFocusItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_pressed");
        mFocusItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFocusItem->setZValue(-1);
        mFocusItem->setVisible(false);
        style()->setItemName(mFocusItem, "highlight");
    }
}

void CntEditViewDetailItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = 0;

    delete mLabel;
    mLabel = 0;

    delete mContent;
    mContent = 0;

    delete mFrameItem;
    mFrameItem = 0;

    delete mFocusItem;
    mFocusItem = 0;

    createPrimitives();
}

void CntEditViewDetailItem::initGesture()
{
    mGestureFilter = new HbGestureSceneFilter(Qt::LeftButton, this);
    mGestureLongpressed = new HbGesture(HbGesture::longpress, 5);
    mGestureFilter->addGesture(mGestureLongpressed);
    mGestureFilter->setLongpressAnimation(true);
    installSceneEventFilter(mGestureFilter);

    connect(mGestureLongpressed, SIGNAL(longPress(QPointF)), this, SLOT(onLongPress(QPointF)));
}

void CntEditViewDetailItem::onLongPress(const QPointF &point)
{
    mHasFocus = false;
    updatePrimitives();
    emit longPressed(point);
}

void CntEditViewDetailItem::setDetail(const QContactDetail &detail, const QString &type)
{
    bool doPolish = false;
    if (!mDetail.isEmpty() && detail != mDetail)
    {
        text.clear();
        valueText.clear();
        icon.clear();
        doPolish = true;
    }
    mDetail = detail;
    mType = type;

    QString context;
    if (mDetail.contexts().count())
    {
        context = mStringMapper.getMappedDetail(detail.contexts().at(0));
    }

    // phone number
    if (mDetail.definitionName() == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber number = mDetail;
        if (number.isEmpty())
        {
            text = hbTrId("txt_phob_list_add_phone_number");
            icon = HbIcon(mStringMapper.getMappedIcon(number.definitionName()));
        }
        else
        {
            QStringList list;
            list << mStringMapper.getMappedDetail(number.subTypes().at(0)) << context;
            text = list.join(" ");
            valueText = number.number();
            icon = HbIcon(mStringMapper.getMappedIcon(number.subTypes().at(0)));
        }
    }

    // email address
    if (mDetail.definitionName() == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email = mDetail;
        if (email.isEmpty())
        {
            text = hbTrId("txt_phob_list_add_email_address");
            icon = HbIcon(mStringMapper.getMappedIcon(email.definitionName()));
        }
        else
        {
            QStringList list;
            list << mStringMapper.getMappedDetail(email.definitionName()) << context;
            text = list.join(" ");
            valueText = email.emailAddress();
            icon = HbIcon(mStringMapper.getMappedIcon(email.definitionName()));
        }
    }

    // postal address
    if (mDetail.definitionName() == QContactAddress::DefinitionName)
    {
        QContactAddress address = mDetail;
        if (address.isEmpty())
        {
            text = qtTrId("Add postal address");
            icon = HbIcon(mStringMapper.getMappedIcon(address.definitionName()));
        }
        else
        {
            QStringList nameList;
            nameList << mStringMapper.getMappedDetail(address.definitionName()) << context;
            text = nameList.join(" ");
            QStringList list;
            if (!address.street().isEmpty())
                list << address.street();
            if (!address.postcode().isEmpty())
                list << address.postcode();
            if (!address.postOfficeBox().isEmpty())
                list << address.postOfficeBox();
            if (!address.locality().isEmpty())
                list << address.locality();
            if (!address.region().isEmpty())
                list << address.region();
            if (!address.country().isEmpty())
                list << address.country();

            QString data = list.join(", ");
            valueText = data;
        }
    }

    // online account
    if (mDetail.definitionName() == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account = mDetail;
        if (account.isEmpty())
        {
            text = hbTrId("Add IM address");
            icon = HbIcon(mStringMapper.getMappedIcon(account.definitionName()));
        }
        else
        {
            QStringList list;
            list << mStringMapper.getMappedDetail(account.subTypes().at(0)) << context;
            text = list.join(" ");
            valueText = account.accountUri();
            icon = HbIcon(mStringMapper.getMappedIcon(account.subTypes().at(0)));
        }
    }

    // url address
    if (mDetail.definitionName() == QContactUrl::DefinitionName)
    {
        QContactUrl url = mDetail;
        if (url.isEmpty())
        {
            text = hbTrId("txt_phob_list_add_url_address");
            icon = HbIcon(mStringMapper.getMappedIcon(url.definitionName()));
        }
        else
        {
            QStringList list;
            list << mStringMapper.getMappedDetail(url.definitionName()) << context;
            text = list.join(" ");
            valueText = url.url();
            icon = HbIcon(mStringMapper.getMappedIcon(url.definitionName()));
        }
    }

    // company details
    if (mDetail.definitionName() == QContactOrganization::DefinitionName)
    {
        QContactOrganization org = mDetail;

        // general details
        if (mType == QContactOrganization::DefinitionName)
        {
            text = mStringMapper.getMappedDetail(org.definitionName());
            QStringList list;
            if(!org.title().isEmpty())
                list << org.title();
            if(!org.name().isEmpty())
                list << org.name();
            if(!org.department().isEmpty())
                list << org.department().join(" ");
            valueText = list.join(", ");

        }
        // assistant name
        else if (mType == QContactOrganization::FieldAssistantName)
        {
            text = mStringMapper.getMappedDetail(QContactOrganization::FieldAssistantName);
            valueText = org.assistantName();
        }
    }

    // birthday
    if (mDetail.definitionName() == QContactBirthday::DefinitionName)
    {
        QContactBirthday bDay = mDetail;

        text = mStringMapper.getMappedDetail(bDay.definitionName());
        valueText = bDay.date().toString("dd MMMM yyyy");
    }

    // anniversary
    if (mDetail.definitionName() == QContactAnniversary::DefinitionName)
    {
        QContactAnniversary anniversary = mDetail;

        text = mStringMapper.getMappedDetail(anniversary.definitionName());
        valueText = anniversary.originalDate().toString("dd MMMM yyyy");
    }

    // ringing tone
    if (mDetail.definitionName() == QContactAvatar::DefinitionName)
    {
        QContactAvatar avatar = mDetail;

        text = mStringMapper.getMappedDetail(QContactAvatar::SubTypeAudioRingtone);
        valueText = avatar.avatar();
    }

    // note
    if (mDetail.definitionName() == QContactNote::DefinitionName)
    {
        QContactNote note = mDetail;

        text = mStringMapper.getMappedDetail(note.definitionName());
        valueText = note.note();
    }

    // family details
    if (mDetail.definitionName() == QContactFamily::DefinitionName)
    {
        QContactFamily family = mDetail;

        // spouse
        if (mType == QContactFamily::FieldSpouse)
        {
            text = mStringMapper.getMappedDetail(QContactFamily::FieldSpouse);
            valueText = family.spouse();

        }
        // children
        else if (mType == QContactFamily::FieldChildren)
        {
            text = mStringMapper.getMappedDetail(QContactFamily::FieldChildren);
            valueText = family.children().join(", ");
        }
    }

    recreatePrimitives();
    if (doPolish)
    {
        repolish();
    }
}

QContactDetail CntEditViewDetailItem::detail()
{
    return mDetail;
}

QString CntEditViewDetailItem::fieldType()
{
    return mType;
}

void CntEditViewDetailItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
    if (mHasFocus)
    {
        mFocusItem->setVisible(true);
        HbEffect::start(mFocusItem, QString("frameitem"), QString("pressed"));
    }
    else
    {
        HbEffect::start(mFocusItem, QString("frameitem"), QString("released"));
    }
}

void CntEditViewDetailItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbInstantFeedback::play(HbFeedback::Basic);
    event->accept();
    mHasFocus = true;
    updatePrimitives();
}

void CntEditViewDetailItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!rect().contains(event->pos()) && mHasFocus)
    {
        mHasFocus = false;
        updatePrimitives();
    }
    event->accept();
}


void CntEditViewDetailItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    bool hadFocus = mHasFocus;
    mHasFocus = false;
    if (rect().contains(event->pos()) && hadFocus)
    {
        updatePrimitives();
        emit clicked();
    }
    event->accept();
}
