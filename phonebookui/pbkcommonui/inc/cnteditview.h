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

#ifndef CNTEDITVIEW__H
#define CNTEDITVIEW__H

#include <QObject>
#include "cntbaseview.h"
#include "cntviewparameters.h"
#include "qtpbkglobal.h"

class HbScrollArea;
class QGraphicsLinearLayout;
class HbGroupBox;
class CntEditViewHeadingItem;
class ThumbnailManager;

class QTPBK_EXPORT CntEditView : public CntBaseView
{ 
    Q_OBJECT

public slots:

    virtual void aboutToCloseView();
    virtual void discardAllChanges();
    virtual int handleExecutedCommand(QString aCommand, const QContact &aContact);
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

    void addField();
    void deleteContact();

    void openNameEditor();
    void openImageEditor();

    void onLongPressed(const QPointF &point);
    void onItemActivated();
    void addDetail();
    void deleteDetail();

public:

    CntEditView(CntViewManager *aViewManager, QGraphicsItem *aParent = 0);
    ~CntEditView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::editView; }
    void activateView(const CntViewParameters &aViewParameters);
    void addActionsToToolBar();
    void addMenuItems();
    CntViewParameters prepareToEditContact(const QString &aViewType, const QString &aAction);

    void prepareItems();
    
#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    QContact *contact();
    void setContact(QContact* aContact);
    void resizeEvent(QGraphicsSceneResizeEvent *event);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void createItem(const QContactDetail &detail, const QString &type = QString(), int position = -1);
        
protected:

    HbScrollArea            *mScrollArea;
    QGraphicsWidget         *mContainerWidget;
    QGraphicsLinearLayout   *mContainerLayout;
    QContact                *mContact;
    HbGroupBox              *mDetailItem;
    ThumbnailManager        *mThumbnailManager;
    CntEditViewHeadingItem  *mHeadingItem;
    int                      mDetailItemIndex;
    int                      mAddressItemIndex;
    QStringList              mExcludeList;

};
#endif //CNTEDITVIEW__H
// EOF
