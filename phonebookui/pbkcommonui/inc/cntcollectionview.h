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

#ifndef CNTCOLLECTIONVIEW_H
#define CNTCOLLECTIONVIEW_H

#include <QObject>
#include <hbdocumentloader.h>

#include <cntabstractview.h>

class HbAction;
class HbView;
class HbListView;
class CntCollectionListModel;
class QModelIndex;
class HbAbstractViewItem;
class CntExtensionManager;

class CntCollectionView : public QObject, public CntAbstractView
{
    Q_OBJECT
    friend class TestCntCollectionView;
    
public:
    CntCollectionView(CntExtensionManager &extensionManager);
    ~CntCollectionView();
    
public: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return collectionView; }

private slots:
    void showPreviousView();
    void openGroup(const QModelIndex &index);
    void showContextMenu(HbAbstractViewItem *item, const QPointF &coords);
    void newGroup();
    void refreshDataModel();
    void deleteGroup(QContact group);
    void deleteGroups();
    

private:
    CntExtensionManager&    mExtensionManager;
    HbView*                 mView; // own
    HbAction*               mSoftkey; // owned by view
    CntAbstractViewManager* mViewManager;
    HbDocumentLoader        mDocumentLoader;
    CntCollectionListModel* mModel; // own
    HbListView*             mListView; // owned by layout
    HbAction*               mNamesAction; // owned by view
    HbAction*               mFindAction; // owned by view
    HbAction*               mExtensionAction; // owned by view
    HbAction*               mNewGroupAction; // owned by view
    HbAction*               mDeleteGroupsAction; // owned by view
};

#endif // CNTCOLLECTIONVIEW_H

// EOF
