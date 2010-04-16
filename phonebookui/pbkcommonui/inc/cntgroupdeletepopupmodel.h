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

#ifndef CNTGROUPDELETEPOPUPMODEL_H
#define CNTGROUPDELETEPOPUPMODEL_H

#include <QSharedData>
#include <QAbstractListModel>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupPopupListData : public QSharedData
{
public:
    CntGroupPopupListData() { }
    ~CntGroupPopupListData() { }

public:
    QList<QVariantList> mDataList;
};

class CntGroupDeletePopupModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    CntGroupDeletePopupModel(QContactManager *manager, QObject *parent = 0);
    ~CntGroupDeletePopupModel();
    
public:
    void initializeGroupsList();
    bool isFavoriteGroupCreated();
    int favoriteGroupId();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QContact contact(QModelIndex &index) const ;
    
private:
    QContactManager                           *mContactManager;
    QSharedDataPointer<CntGroupPopupListData>  mDataPointer;
    int                                        mFavoriteGroupId;
};

#endif // CNTGROUPDELETEPOPUPMODEL_H

// EOF
