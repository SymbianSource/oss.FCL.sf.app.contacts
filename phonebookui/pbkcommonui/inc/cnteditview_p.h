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

#ifndef CNTEDITVIEW_P_H_
#define CNTEDITVIEW_P_H_

#include <QObject>

#include "cnteditview.h"
#include "cntglobal.h"
#include <qtcontacts.h>
#include <cnteditviewitemcallback.h>

class CntAbstractViewManager;
class CntEditViewListModel;
class CntEditViewHeadingItem;
class CntEditViewItem;
class CntImageLabel;

class ThumbnailManager;
class QModelIndex;
class HbDocumentLoader;
class HbAbstractViewItem;
class HbListView;
class HbView;
class HbAction;
class QAction;
class HbMenu;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
class QContactId;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class CntEditViewPrivate : public QObject, public CntEditViewItemCallback
{
    Q_OBJECT    
    
public:
    CntEditViewPrivate();
    ~CntEditViewPrivate();
    
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    
public: // From CntEditViewItemCallback
    void openView(CntViewParameters& viewParams);
    void requestRefresh();
    
signals:
    void contactUpdated(bool aSuccess);
    void contactRemoved(bool aSuccess);
    void changesDiscarded();
    
public:
    CntEditView* q_ptr;
    
private slots:
    void activated( const QModelIndex& aIndex );
    void longPressed( HbAbstractViewItem *item, const QPointF &coords );
    
    void addDetailItem();
    void handleAddDetailItem(HbAction *aAction);
    void deleteContact();
    void handleDeleteContact(HbAction *action);
    void discardChanges();
    void saveChanges();
    
    void openNameEditor();
    void openImageEditor();
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    void setOrientation(Qt::Orientation aOrientation);
    void handleMenuAction( HbAction* aAction );
    void setScrollPosition();
    
private:
    void loadAvatar();
    HbMenu* createPopup( const QModelIndex aIndex, CntEditViewItem* aDetail );
    void addDetail( CntEditViewItem* aDetail );
    void editDetail( CntEditViewItem* aDetail );
    void removeDetail( CntEditViewItem* aDetail, const QModelIndex& aIndex );
    void setSelectedContact( QContact aContact );
    
public:
    HbView* mView;
    HbListView* mListView;
    CntEditViewListModel* mModel;
    CntEditViewHeadingItem* mHeading;
    CntImageLabel *mImageLabel;
    CntViewParameters mArgs;
    HbDocumentLoader* mDocument;
    ThumbnailManager* mThumbnailManager;
    QContact* mContact; // own
    CntAbstractViewManager* mMgr; // not owned
    bool mIsMyCard;
    
    HbAction* mSoftkey;
    HbAction* mDiscard;
    HbAction* mDelete;
    HbAction* mSave;
    
private:
    Q_DECLARE_PUBLIC(CntEditView)
    friend class TestCntEditView;
};
#endif /* CNTEDITVIEW_P_H_ */
