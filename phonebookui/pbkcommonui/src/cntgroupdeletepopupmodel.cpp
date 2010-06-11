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

#include "cntgroupdeletepopupmodel.h"

#include <qtcontacts.h>
#include <hbglobal.h>

/*!
Constructor
*/
CntGroupDeletePopupModel::CntGroupDeletePopupModel(QContactManager *manager, QObject *parent)
    : QAbstractListModel(parent),
    mContactManager(manager),
    mFavoriteGroupId(-1)
{
    mDataPointer = new CntGroupPopupListData();
    isFavoriteGroupCreated();
}

/*!
Destructor
*/
CntGroupDeletePopupModel::~CntGroupDeletePopupModel()
{
    
}

/*!
Initialize groups
*/
void CntGroupDeletePopupModel::initializeGroupsList()
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    
    QContactSortOrder sortOrderGroupName;
    sortOrderGroupName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldCustomLabel);
    sortOrderGroupName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> groupsOrder;
    groupsOrder.append(sortOrderGroupName);

    QList<QContactLocalId> groupContactIds = mContactManager->contactIds(groupFilter, groupsOrder);

    if (!groupContactIds.isEmpty())
    {
        for(int i = 0;i < groupContactIds.count();i++)
        {
            QVariantList dataList;
            
            // group name
            QStringList displayList;
            
            QContact contact = mContactManager->contact(groupContactIds.at(i));
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            if(groupContactIds.at(i) != mFavoriteGroupId )
            {
                if (groupName.isNull())
                    {
                    QString unnamed(hbTrId("txt_phob_dblist_unnamed"));
                    displayList.append(unnamed);
                    }
                else
                    {
                    displayList.append(groupName);
                    }    
                
                dataList.append(displayList);
                
                // contact Id for identification
                dataList.append(groupContactIds.at(i));
                
                mDataPointer->mDataList.append(dataList);
            }
        }
    }
}

bool CntGroupDeletePopupModel::isFavoriteGroupCreated()
{
    bool favoriteGroupCreated = false;
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mContactManager->contactIds(groupFilter);
    
    if (!groupContactIds.isEmpty())
    {
        for(int i = 0;i < groupContactIds.count();i++)
        {
            QContact contact = mContactManager->contact(groupContactIds.at(i));
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            if(groupName.compare("Favorites") == 0)
            {
                favoriteGroupCreated = true;
                mFavoriteGroupId = groupContactIds.at(i);
                break;
            }
        }
    }
    return favoriteGroupCreated;
}

int CntGroupDeletePopupModel::favoriteGroupId()
{
    return mFavoriteGroupId;
}

/*!
Returns the number of rows in the model
*/
int CntGroupDeletePopupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mDataPointer->mDataList.count();
}
/*!
Returns data for given index with a given role
*/
QVariant CntGroupDeletePopupModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    
    if (row < 0)
    {
        return QVariant();
    }
    
    QVariantList values = mDataPointer->mDataList.at(row);
    
    if (role == Qt::DisplayRole)
    {
        QStringList list = values[0].toStringList();        
        return QVariant(list);
    }
    
    else if (role == Qt::UserRole)
       {
           int contactId = values[1].toInt();
           return QVariant(contactId);
       }
    return QVariant();
}


QContact CntGroupDeletePopupModel::contact(QModelIndex &index) const
{
    int row = index.row();
    QVariantList values = mDataPointer->mDataList.at(row);
    int groupId = values[1].toInt();
    return mContactManager->contact( groupId );
}
