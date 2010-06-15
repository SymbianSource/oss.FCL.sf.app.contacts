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

#ifndef CNTGROUPMEMBERVIEW_H
#define CNTGROUPMEMBERVIEW_H

#include <QObject>
#include <QList>
#include <hbdocumentloader.h>

#include <cntabstractview.h>

class CntListModel;
class CntAbstractViewManager;
class CntContactCardHeadingItem;
class CntFetchContacts;
class HbView;
class HbAction;
class HbListView;
class HbAbstractViewItem;
class ThumbnailManager;
class QModelIndex;
class CntImageLabel;
class HbDocumentLoader;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupMemberView : public QObject, public CntAbstractView
{
    friend class TestCntGroupMemberView;
    Q_OBJECT

public:
    CntGroupMemberView();
    ~CntGroupMemberView();
    
public: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return groupMemberView; }

public slots:
    void setOrientation(Qt::Orientation orientation);


private slots:
    void showPreviousView();
    void openGroupActions();
    void manageMembers();
    void handleManageMembers();
    void editGroup();
    void deleteGroup();
    void handleDeleteGroup(HbAction *action);
    
    void showContextMenu(HbAbstractViewItem *item, const QPointF &coords);
    void handleMenu(HbAction* action);

    void showContactView(const QModelIndex &index);
    void removeFromGroup(const QModelIndex &index);
    void editContact(const QModelIndex &index);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void openImageEditor();
    
    void drawImageMenu(const QPointF &aCoords);
    void createModel();
  
    void removeImage();
    
private:
    QContactManager* getContactManager();
    
private:
    QContact*                   mGroupContact; // own
    CntAbstractViewManager*     mViewManager;
    HbDocumentLoader            mDocumentLoader;
    HbView*                     mView; // own
    HbAction*                   mSoftkey; // owned by view
    CntContactCardHeadingItem*  mHeadingItem; // owned by layout
    ThumbnailManager*           mThumbnailManager; // own
    HbAction*                   mManageAction; // owned by view
    HbAction*                   mDeleteAction; // owned by view
    HbAction*                   mShowActionsAction; // owned by view
    HbAction*                   mEditGroupAction; // owned by view
    CntListModel*               mModel; // own
    CntImageLabel*              mImageLabel;
    HbListView*                 mListView; // owned by layout
    HbDocumentLoader*           mDocument;
    CntFetchContacts*           mFetchView;
    QList<QContactLocalId>      mOriginalGroupMembers;
    QContactAvatar*             mAvatar;
    CntViewParameters           mArgs;
};

#endif // CNTGROUPMEMBERVIEW_H

// EOF
