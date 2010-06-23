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

#ifndef CNTCOLLECTIONLISTMODEL_H
#define CNTCOLLECTIONLISTMODEL_H

#include <QSharedData>
#include <QAbstractListModel>
#include <qmobilityglobal.h>
#include <cntuigroupsupplier.h>
#include <xqsettingsmanager.h>

class CntExtensionManager;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntCollectionListData : public QSharedData
{
public:
    CntCollectionListData() { }
    ~CntCollectionListData() { }

public:
    QList<QVariantList> mDataList;
    QMap<int, CntUiGroupSupplier*> mExtensions; // row, plugin
};

class CntCollectionListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    CntCollectionListModel(QContactManager *manager, CntExtensionManager &extensionManager, QObject *parent = 0);
    ~CntCollectionListModel();
    
public:
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void removeGroup(int localId);
    bool isExtensionGroup(const QModelIndex &index);
    CntViewParameters extensionGroupActivated(int row);
    void extensionGroupLongPressed(int row, const QPointF& coords, CntExtensionGroupCallback* interface);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void doConstruct();
    void initializeStaticGroups();
    void initializeExtensions();
    void initializeUserGroups();
    
private:
    CntExtensionManager&                       mExtensionManager;
    QSharedDataPointer<CntCollectionListData>  mDataPointer;
    QContactManager                           *mContactManager;
    XQSettingsManager                          mSettings;
    int                                        mFavoriteGroupId;
    
};

#endif // CNTCOLLECTIONLISTMODEL_H

// EOF
