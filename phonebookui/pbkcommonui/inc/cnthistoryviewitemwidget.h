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

#ifndef CNTHISTORYVIEWITEMWIDGET_H
#define CNTHISTORYVIEWITEMWIDGET_H

#include <QObject>
#include <hbwidget.h>

class HbIconItem;
class HbTextItem;
class HbFrameItem;

class CntHistoryViewItemWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY( bool titleExist READ isTitleExist )
    Q_PROPERTY( bool bodyTextExist READ isBodyTextExist )
    Q_PROPERTY( bool incoming READ getIncoming )

public:
    CntHistoryViewItemWidget(QGraphicsItem *parent = 0);
    ~CntHistoryViewItemWidget();

public:
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetails(QString title, QString bodyText, QString timeStamp, QString iconName, bool incoming, bool newMessage);

    bool isTitleExist() const { return !mTitle.isNull(); }
    bool isBodyTextExist() const { return !mBodyText.isNull(); }
    bool getIncoming() const { return mIncoming; }

private:

    HbIconItem              *mIconLabel;
    HbTextItem              *mTitleLabel;
    HbTextItem              *mBodyTextLabel;
    HbTextItem              *mTimeStampLabel;
    HbFrameItem             *mFrameLabel;
    HbFrameItem             *mNewItemLabel;

    QString                 mTitle;
    QString                 mBodyText;
    QString                 mTimeStamp;
    HbIcon                  mIcon;
    bool                    mIncoming;
    bool                    mNewMessage;
};

#endif // CNTHISTORYVIEWITEMWIDGET_H
