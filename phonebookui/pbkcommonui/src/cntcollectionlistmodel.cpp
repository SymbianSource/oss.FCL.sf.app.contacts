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

#include "cntcollectionlistmodel.h"

#include <QIcon>
#include <qtcontacts.h>
#include <hbglobal.h>

/*!
Constructor
*/
CntCollectionListModel::CntCollectionListModel(QContactManager *manager, QObject *parent)
    : QAbstractListModel(parent),
    mContactManager(manager)
{
    mDataPointer = new CntCollectionListData();
    doConstruct();
}

/*!
Destructor
*/
CntCollectionListModel::~CntCollectionListModel()
{
    
}

/*!
Returns data for given index with a given role
*/
QVariant CntCollectionListModel::data(const QModelIndex &index, int role) const
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
     
    else if (role == Qt::DecorationRole)
    {
        QVariantList icons;
        QIcon itemIcon(values[1].toString());
        icons.append(itemIcon);
        return QVariant(icons);
    }
    
    else if (role == Qt::UserRole)
    {
        int contactId = values[2].toInt();
        return QVariant(contactId);
    }

    return QVariant();
}

/*!
Returns the number of rows in the model
*/
int CntCollectionListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mDataPointer->mDataList.count();
}

/*!
Removes given rows from the model
*/
bool CntCollectionListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    Q_UNUSED(parent);
    if (row < 0 || row > rowCount())
    {
        return false;
    }
    
    beginRemoveRows(parent, row, row);
    mDataPointer->mDataList.removeAt(row);
    endRemoveRows();

    return true;
}

/*!
Remove given group from the model
*/
void CntCollectionListModel::removeGroup(int localId)
{
    for (int i = 0;i < rowCount();i++)
    {
        if (mDataPointer->mDataList.at(i)[2] == localId)
        {
            removeRow(i);
            break;
        }
    }
}

/*!
Initialize the data, both static (favorites, online) and user defined groups
*/
void CntCollectionListModel::doConstruct()
{
    initializeStaticGroups();
    initializeUserGroups();
}

/*!
Initialize static groups
*/
void CntCollectionListModel::initializeStaticGroups()
{
    QVariantList dataList;
    QStringList displayList;
    displayList.append(hbTrId("txt_phob_dblist_favorites"));
    displayList.append(hbTrId("txt_phob_dblist_favorites_val_no_favorites_selecte")); // as this isn't supported yet
    dataList.append(displayList);
    
    dataList.append(":/icons/qtg_large_favourites.svg");
    
    dataList.append(-1); // as favorites doesn't really have a contact Id, -1 is used
    
    mDataPointer->mDataList.append(dataList);
}

/*!
Initialize user defined groups
*/
void CntCollectionListModel::initializeUserGroups()
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mContactManager->contacts(groupFilter);
    
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
            
            if (groupName.isNull())
            {
                QString unnamed(hbTrId("Unnamed"));
                displayList.append(unnamed);
            }
            else
            {
                displayList.append(groupName);
            }
            
            QContactRelationshipFilter rFilter;
            rFilter.setRelationshipType(QContactRelationship::HasMember);
            rFilter.setRole(QContactRelationshipFilter::Second);
            rFilter.setOtherParticipantId(contact.id());
            
            // group members and their count
            QList<QContactLocalId> groupMemberIds = mContactManager->contacts(rFilter);
            
            if (!groupMemberIds.isEmpty())
            {
                QStringList nameList;
                for(int i = 0;i < groupMemberIds.count();i++)
                {
                    QContact contact = mContactManager->contact(groupMemberIds.at(i));
                    QString memberName = contact.displayLabel();
                    nameList << memberName;
                    if (nameList.join(", ").length() > 30)
                    {
                        break;
                    }
                }
                QString names = nameList.join(", ");
                displayList.append(names);
                displayList.append(hbTrId("(%1)").arg(groupMemberIds.count()));
            }
            else
            {
                displayList.append(hbTrId("No members selected"));
            }
            dataList.append(displayList);
            
            // icon, default for now always
            dataList.append(":/icons/qtg_large_custom.svg");
            
            // contact Id for identification
            dataList.append(groupContactIds.at(i));
            
            mDataPointer->mDataList.append(dataList);
        }
    }
}
