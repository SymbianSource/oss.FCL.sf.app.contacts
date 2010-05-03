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

#ifndef CNTCONTACTCARDDATAITEM_H
#define CNTCONTACTCARDDATAITEM_H

#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>
#include <hbicon.h>

QTM_USE_NAMESPACE

class CntContactCardDataItem: public QObject
{
    Q_OBJECT    

public:
    CntContactCardDataItem(const QString& aTitle, bool aIsFocusable);
    virtual ~CntContactCardDataItem();

public:
    void setTitleText(const QString& aTitle);
    void setValueText(const QString& aValueText, Qt::TextElideMode aValueTextElideMode = Qt::ElideRight);
    void setIcon(HbIcon aIcon);
    void setSecondaryIcon(HbIcon aSecondaryIcon);
    void setAction(const QString& aAction);
    void setContactDetail(QContactDetail aDetail);

public:
    QString titleText() { return mTitle; }
    QString valueText() { return mValueText; }
    HbIcon  icon() { return mIcon; }
    HbIcon  secondaryIcon() { return mSecondaryIcon; }
    QString action() { return mAction; }
    QContactDetail detail() { return mDetail; }
    Qt::TextElideMode elideMode() { return mValueTextElideMode; }
    bool isFocusable() { return mIsFocusable; }
    
private:
    HbIcon              mIcon;
    HbIcon              mSecondaryIcon;
    QString             mTitle;
    QString             mValueText;
    QString             mAction;
    QContactDetail      mDetail;
    Qt::TextElideMode   mValueTextElideMode;
    bool                mIsFocusable;
};

#endif /* CNTCONTACTCARDDATAITEM_H */
