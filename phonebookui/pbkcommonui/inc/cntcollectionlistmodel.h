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
};

class CntCollectionListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    CntCollectionListModel(QContactManager *manager, QObject *parent = 0);
    ~CntCollectionListModel();
    
public:
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void removeGroup(int localId);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void doConstruct();
    void initializeStaticGroups();
    void initializeUserGroups();
    
private:
    QSharedDataPointer<CntCollectionListData>  mDataPointer;
    QContactManager                           *mContactManager;
    
};

#endif // CNTCOLLECTIONLISTMODEL_H

// EOF
