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

#ifndef CNTCOMMLAUNCHERVIEW_H
#define CNTCOMMLAUNCHERVIEW_H

#include "cntcontactcardview.h"

#include <QObject>
#include <QKeyEvent>
#include <QGraphicsSceneResizeEvent>
#include "cntglobal.h"

class HbView;
class HbScrollArea;
class CntDocumentLoader;
class HbAction;
class HbIcon;
class QGraphicsWidget;
class QGraphicsLinearLayout;
class ThumbnailManager;
class CntContactCardDataContainer;
class CntContactCardHeadingItem;
class CntContactCardDetailItem;
class CntImageLabel;
class CntActionLauncher;
class XQServiceRequest;
class ShareUi;
class QStandardItemModel;
class QModelIndex;
class HbSelectionDialog;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactManager;
class QContactDetail;
class QContactAction;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntContactCardViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntContactCardView)

public:
    CntContactCardViewPrivate();
    virtual ~CntContactCardViewPrivate();    

public slots:
    void onItemActivated();
    void editContact();
	void viewHistory();
    void onLongPressed(const QPointF &aCoords);
    void setPreferredAction(const QString &aAction, const QContactDetail &aDetail);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void drawMenu(const QPointF &aCoords);
    void sendToHs();
    
    void keyPressed(QKeyEvent *event);

private slots:
    void sendBusinessCard();
    void addToGroup();
    void deleteContact();
    void handleDeleteContact(HbAction *action);
    void setAsFavorite();
    void removeFromFavorite();
    void actionExecuted(CntActionLauncher* aAction);
    void setOrientation(Qt::Orientation orientation);
    void showPreviousView();
    void doChangeImage();
    void doRemoveImage();
    
    void handleMenuAction(HbAction* aAction);
    void handleSendBusinessCard( HbAction* aAction );
    void sendKeyDialogSlot(HbAction* action);
    
    void launchSendKeyAction(const QModelIndex &index);
    
public:
    CntContactCardView* q_ptr;    
    void activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs);
    void deactivate();
    CntDocumentLoader* document();
    QContactManager* contactManager();
    
signals:
    void preferredUpdated();
    void backPressed();
    void viewActivated(CntAbstractViewManager* aMgr, const CntViewParameters aArgs);

    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void executeAction(QContact& aContact, QContactDetail aDetail, QString aAction);
    void executeDynamicAction(QContact& aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor);
    
    void sendKeyPressed();
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    CntAbstractViewManager*     mViewManager;
    HbView*                     mView;
    HbScrollArea                *mScrollArea;
    QGraphicsWidget             *mContainerWidget;
    QGraphicsLinearLayout       *mContainerLayout;
    QContact                    *mContact;
    QGraphicsWidget             *mDetailsWidget;
    CntContactCardDataContainer *mDataContainer;
    CntContactCardHeadingItem   *mHeadingItem;
    ThumbnailManager            *mThumbnailManager;
    QContactAvatar              *mAvatar;
    bool                        mIsHandlingMenu;
    QMap<QString, CntContactCardDetailItem*> mPreferredItems;
    int                         mFavoriteGroupId;
    CntDocumentLoader           *mLoader;
    QContactAction              *mContactAction;
    HbAction                    *mBackKey;
    CntImageLabel               *mImageLabel;
	XQServiceRequest            *mHighwayService;
    HbIcon                      *mVCardIcon;
    CntViewParameters           mArgs;
    ShareUi                     *mShareUi;
    bool                        mAcceptSendKey;
    QStandardItemModel*         mSendKeyListModel;
    HbSelectionDialog*          mSendKeyPopup;
    
};

#endif // CNTCOMMLAUNCHERVIEW_H

// EOF
