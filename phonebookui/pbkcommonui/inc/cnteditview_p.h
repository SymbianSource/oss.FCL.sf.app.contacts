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
#include "qtpbkglobal.h"
#include <qtcontacts.h>

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

QTM_BEGIN_NAMESPACE
class QContactDetail;
class QContactId;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class CntEditViewPrivate : public QObject
{
    Q_OBJECT    
    
public:
    CntEditViewPrivate();
    ~CntEditViewPrivate();
    
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    
signals:
    void contactUpdated();
    void contactRemoved();
    void changesDiscarded();
    
public:
    CntEditView* q_ptr;
    
private slots:
    void activated( const QModelIndex& aIndex );
    void longPressed( HbAbstractViewItem *item, const QPointF &coords );
    
    void addDetailItem();
    void deleteContact();
    void discardChanges();
    void saveChanges();
    
    void openNameEditor();
    void openImageEditor();
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    void setOrientation(Qt::Orientation aOrientation);
    
private:
    void loadAvatar();
    QList<QAction*> createPopup( CntEditViewItem* aDetail );
    void addDetail( CntEditViewItem* aDetail );
    void editDetail( CntEditViewItem* aDetail );
    void removeDetail( CntEditViewItem* aDetail, const QModelIndex& aIndex );
    
public:
    HbView* mView;
    HbListView* mListView;
    CntEditViewListModel* mModel;
    CntEditViewHeadingItem* mHeading;
    CntImageLabel *mImageLabel;
    HbDocumentLoader* mDocument;
    ThumbnailManager* mThumbnailManager;
    QContact* mContact; // own
    CntAbstractViewManager* mMgr; // not owned
    
    HbAction* mSoftkey;
    HbAction* mDiscard;
    HbAction* mDelete;
    HbAction* mSave;
    
private:
    Q_DECLARE_PUBLIC(CntEditView)
    friend class TestCntEditView;
};
#endif /* CNTEDITVIEW_P_H_ */
