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
#include <hbframebackground.h>

class HbIconItem;
class HbTextItem;
class HbFrameItem;

class CntHistoryViewItemWidget : public HbWidget
{
    friend class TestCntHistoryView;
    
    Q_OBJECT
    Q_PROPERTY( QString title READ getTitle )
    Q_PROPERTY( QString bodyText READ getBodyText )
    Q_PROPERTY( QString timeStamp READ getTimeStamp )
    Q_PROPERTY( HbIcon icon READ getIcon )
    Q_PROPERTY( bool incoming READ getIncoming )
    Q_PROPERTY( bool newmessage READ isNewMessage )

public:
    CntHistoryViewItemWidget(QGraphicsItem *parent = 0);
    ~CntHistoryViewItemWidget();

public:
    void createPrimitives();
    void recreatePrimitives();
    
    void setModelIndex(const QModelIndex& index);
    
    void pressStateChanged(bool pressed);

    QString getTitle() const { return title; }
    QString getBodyText() const { return bodyText; }
    QString getTimeStamp() const { return timeStamp; }
    HbIcon getIcon() const { return icon; }
    bool getIncoming() const { return incoming; }
    bool isNewMessage() const { return newmessage; }

private:
    HbIconItem              *mIcon;
    HbTextItem              *mTitle;
    HbTextItem              *mBodyText;
    HbTextItem              *mTimeStamp;
    HbFrameItem             *mFrameItem;
    HbFrameItem             *mFocusItem;
    HbFrameItem             *mNewItem;

    HbFrameBackground       backGround;
    QString                 title;
    QString                 bodyText;
    QString                 timeStamp;
    HbIcon                  icon;
    bool                    incoming;
    bool                    newmessage;
};

#endif /* CNTHISTORYVIEWITEMWIDGET_H */
