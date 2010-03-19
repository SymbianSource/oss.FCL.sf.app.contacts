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

#include "cntcontactcardheadingitem.h"

#include <qtcontacts.h>

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbmarqueeitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbstyleloader.h>



CntContactCardHeadingItem::CntContactCardHeadingItem(QGraphicsItem *parent) :
    HbWidget(parent),
    mIcon(0),
    mFirstLineText(0),
    mPrimaryText(0),
    mSecondLineText(0),
    mSecondaryText(0),
    mMarqueeItem(0),
    mFrameItem(0)
{

}

CntContactCardHeadingItem::~CntContactCardHeadingItem()
{

}

void CntContactCardHeadingItem::createPrimitives()
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

    if (!firstLineText.isNull())
    {
        if (!mFirstLineText)
        {
            mFirstLineText = new HbTextItem(this);
            mFirstLineText->setText(firstLineText);
            mFirstLineText->setMaximumLines(1);
            style()->setItemName(mFirstLineText, "first_line_text");
        }
    }
    else
    {
        if (mFirstLineText)
        {
            delete mFirstLineText;
        }
        mFirstLineText = 0;
    }

    if (!primaryText.isNull())
    {
        if (!mPrimaryText)
        {
            mPrimaryText = new HbTextItem(this);
            mPrimaryText->setText(primaryText);
            mPrimaryText->setMaximumLines(2);
            mPrimaryText->setTextWrapping(Hb::TextWordWrap);
            style()->setItemName(mPrimaryText, "primary_text");
        }
    }
    else
    {
        if (mPrimaryText)
        {
            delete mPrimaryText;
        }
        mPrimaryText = 0;
    }


    if (!secondLineText.isNull())
    {
        if (!mSecondLineText)
        {
            mSecondLineText = new HbTextItem(this);
            mSecondLineText->setText(secondLineText);
            mSecondLineText->setMaximumLines(1);
            style()->setItemName(mSecondLineText, "second_line_text");
        }
    }
    else
    {
        if (mSecondLineText)
        {
            delete mSecondLineText;
        }
        mSecondLineText = 0;
    }

    if (!secondaryText.isNull())
    {
        if (!mSecondaryText)
        {
            mSecondaryText = new HbTextItem(this);
            mSecondaryText->setText(secondaryText);
            mSecondaryText->setMaximumLines(1);
            style()->setItemName(mSecondaryText, "secondary_text");
        }
    }
    else
    {
        if (mSecondaryText)
        {
            delete mSecondaryText;
        }
        mSecondaryText = 0;
    }

    if (!tinyMarqueeText.isNull())
    {
        if (!mMarqueeItem)
        {
            mMarqueeItem = new HbMarqueeItem(this);
            mMarqueeItem->setLoopCount(1);
            mMarqueeItem->setText(tinyMarqueeText);
            mMarqueeItem->startAnimation();
            mMarqueeItem->setFontSpec(HbFontSpec(HbFontSpec::PrimarySmall));
            style()->setItemName(mMarqueeItem, "marquee_item");
        }
    }
    else
    {
        if (mMarqueeItem)
        {
            delete mMarqueeItem;
        }
        mMarqueeItem = 0;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }
}

void CntContactCardHeadingItem::setIcon(const HbIcon newIcon)
{
    if (newIcon != icon)
    {
        icon = newIcon;
        createPrimitives();
        mIcon->setIcon(icon);
        repolish();
    }
}

void CntContactCardHeadingItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = 0;

    delete mFirstLineText;
    mFirstLineText = 0;

    delete mPrimaryText;
    mPrimaryText = 0;

    delete mSecondLineText;
    mSecondLineText = 0;

    delete mSecondaryText;
    mSecondaryText = 0;

    delete mMarqueeItem;
    mMarqueeItem = 0;

    delete mFrameItem;
    mFrameItem = 0;

    createPrimitives();
}


void CntContactCardHeadingItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
}

bool CntContactCardHeadingItem::isNickName(const QContact* contact)
{
    return !(contact->detail<QContactNickname>().nickname().isNull());
}

bool CntContactCardHeadingItem::isCompanyName(const QContact* contact)
{
    return (!(contact->detail<QContactOrganization>().title().isNull()) || !(contact->detail<QContactOrganization>().name().isNull()));
}

void CntContactCardHeadingItem::setDetails(const QContact* contact)
{
    primaryText.clear();
    firstLineText.clear();
    secondaryText.clear();
    icon.clear();
    tinyMarqueeText.clear();

    // icon label
    icon = HbIcon(":/icons/qtg_large_avatar.svg");

    QContactName name = contact->detail<QContactName>();

    // name label
    if (isNickName(contact) || isCompanyName(contact))
    {
        // prefix, first, middle, last and suffix
        QStringList nameList;
        nameList << name.prefix() << name.first() << name.middle() << name.last() << name.suffix();
        firstLineText = nameList.join(" ").trimmed();
        if (firstLineText.isEmpty())
        {
            firstLineText = hbTrId("Unnamed");
        }
    }
    else
    {
        // prefix, first, middle, last and suffix
        QStringList nameList;
        nameList << name.prefix() << name.first() << name.middle() << name.last() << name.suffix();
        primaryText = nameList.join(" ").trimmed();
        if (primaryText.isEmpty())
        {
            primaryText = hbTrId("Unnamed");
        }
        
        /*
        // prefix, first and middle
        QStringList firstNameList;
        firstNameList << name.prefix() << name.first() << name.middle();
        firstLineText = firstNameList.join(" ").trimmed();

        // last and suffix
        QStringList lastNameList;
        lastNameList << name.last() << name.suffix();
        mSecondLineText = lastNameList.join(" ").trimmed();
        */
    }

    // nick label
    if (isNickName(contact))
    {
        secondaryText = contact->detail<QContactNickname>().nickname();
    }

    // company label
    if (isCompanyName(contact))
    {
        QStringList companyList;
        companyList << contact->detail<QContactOrganization>().title() << contact->detail<QContactOrganization>().name();
        tinyMarqueeText = companyList.join(" ").trimmed();
    }

    recreatePrimitives();
}

void CntContactCardHeadingItem::setGroupDetails(const QContact* contact)
{
    firstLineText.clear();
    secondaryText.clear();
    icon.clear();

    // icon label
    icon = HbIcon(":/icons/qtg_large_custom.svg");

    QContactName contactName = contact->detail( QContactName::DefinitionName );
    QString groupName = contactName.value( QContactName::FieldCustomLabel );

    primaryText = groupName;
    
    if (primaryText.isEmpty())
    {
        primaryText = hbTrId("Unnamed");
    }
       
    recreatePrimitives();
}
