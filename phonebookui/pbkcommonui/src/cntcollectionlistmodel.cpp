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
#include "cntextensionmanager.h"
#include "cntfavourite.h"

#include <cntuiextensionfactory.h>
#include <cntuigroupsupplier.h>
#include <cntuids.h>
#include <xqsettingskey.h>

#include <QIcon>
#include <qtcontacts.h>
#include <hbglobal.h>
#include <hbiconitem.h>

// CONTSTANTS
const int CntNamesLengthLimit = 30;

/*!
Constructor
*/
CntCollectionListModel::CntCollectionListModel(QContactManager *manager, CntExtensionManager &extensionManager, QObject *parent)
    : QAbstractListModel(parent),
    mExtensionManager(extensionManager),
    mContactManager(manager),
    mFavoriteGroupId(-1)
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
        for (int i = 0;i < values[1].toStringList().count();i++)
        {
            HbIcon icon(values[1].toStringList().at(i));
            icons.append(icon);
        }
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
        // extension items have 4 items in the list, we don't allow those to be deleted from here
        if (mDataPointer->mDataList.at(i)[2] == localId && mDataPointer->mDataList.at(i).count() < 4)
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
    initializeExtensions();
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
    
    mFavoriteGroupId = CntFavourite::createFavouriteGroup( mContactManager );
    
    if(mFavoriteGroupId != -1)
    {
        QContact favoriteGroup =  mContactManager->contact(mFavoriteGroupId);
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType(QContactRelationship::HasMember);
        rFilter.setRelatedContactRole(QContactRelationship::First);
        rFilter.setRelatedContactId(favoriteGroup.id());
       
        QContactSortOrder sortOrderFirstName;
        sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldFirstName);
        sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

        QContactSortOrder sortOrderLastName;
        sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldLastName);
        sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

        QList<QContactSortOrder> sortOrders;
        XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
            KCRUiContacts.iUid,
            KCntNameOrdering);
        int order = mSettings.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();
        if (order == CntOrderFirstLast)
        {
            sortOrders.append(sortOrderFirstName);
            sortOrders.append(sortOrderLastName);
        } 
        else
        {
            sortOrders.append(sortOrderLastName);
            sortOrders.append(sortOrderFirstName);
        }
        // group members and their count
        QList<QContactLocalId> groupMemberIds = mContactManager->contactIds(rFilter, sortOrders);

        if (!groupMemberIds.isEmpty())
        {
            QStringList nameList;
            for(int i = 0;i < groupMemberIds.count();i++)
            {
                QContact contact = mContactManager->contact(groupMemberIds.at(i));
                QString memberName = contact.displayLabel();
                if (memberName.isEmpty())
                {
                    memberName = hbTrId("txt_phob_dblist_unnamed");
                }
                
                // Incase the format is LastnameCommaFirstname, remove the commas
                memberName = memberName.replace(", ", " ");
                nameList << memberName;
                if (nameList.join(", ").length() > CntNamesLengthLimit)
                {
                    break;
                }
            }
            QString names = nameList.join(", ").trimmed();
            displayList.append(names);
            displayList.append(hbTrId("(%1)").arg(groupMemberIds.count()));
        }
        else
        {
            displayList.append(hbTrId("txt_phob_dblist_favorites_val_no_favorites_selecte"));
        }
    }
    else
    {
       displayList.append(hbTrId("txt_phob_dblist_favorites_val_no_favorites_selecte"));
    }
    
    dataList.append(displayList);
    dataList.append(QStringList("qtg_large_favourites"));
    dataList.append(mFavoriteGroupId);
    mDataPointer->mDataList.append(dataList);
}

/*!
Initialize extension groups
*/
void CntCollectionListModel::initializeExtensions()
{
    for(int i = 0;i < mExtensionManager.pluginCount();i++)
    {
        CntUiGroupSupplier* groupSupplier = mExtensionManager.pluginAt(i)->groupSupplier();
        if (groupSupplier)
        {
            for(int j = 0;j < groupSupplier->groupCount();j++)
            {
                const CntUiExtensionGroup& group = groupSupplier->groupAt(j);

                QVariantList dataList;
                
                QStringList displayList;
                displayList.append(group.name());
                displayList.append(group.extraText());
                if (group.memberCount() > 0)
                {
                    displayList.append(hbTrId("(%1)").arg(group.memberCount()));
                }
                dataList.append(displayList);
                
                QStringList decorationList;
                decorationList.append(group.groupIcon());
                decorationList.append(group.extraIcon());
                dataList.append(decorationList);
                
                dataList.append(group.serviceId());
                dataList.append(true); // for checking if this is from an extension
                
                mDataPointer->mExtensions.insert(rowCount(), groupSupplier);
                mDataPointer->mDataList.append(dataList);
            }
        }
    }
}

/*!
Initialize user defined groups
*/
void CntCollectionListModel::initializeUserGroups()
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

            QContactRelationshipFilter rFilter;
            rFilter.setRelationshipType(QContactRelationship::HasMember);
            rFilter.setRelatedContactRole(QContactRelationship::First);
            rFilter.setRelatedContactId(contact.id());

            QContactSortOrder sortOrderFirstName;
            sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
                    QContactName::FieldFirstName);
            sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

            QContactSortOrder sortOrderLastName;
            sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
                    QContactName::FieldLastName);
            sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

            QList<QContactSortOrder> sortOrders;
            XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
                KCRUiContacts.iUid,
                KCntNameOrdering);
            int order = mSettings.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();
            if (order == CntOrderFirstLast)
            {
                sortOrders.append(sortOrderFirstName);
                sortOrders.append(sortOrderLastName);
            } 
            else
            {
                sortOrders.append(sortOrderLastName);
                sortOrders.append(sortOrderFirstName);
            }

            // group members and their count
            QList<QContactLocalId> groupMemberIds = mContactManager->contactIds(rFilter, sortOrders);

            if (!groupMemberIds.isEmpty())
            {
                QStringList nameList;
                for(int i = 0;i < groupMemberIds.count();i++)
                {
                    QContact contact = mContactManager->contact(groupMemberIds.at(i));
                    QString memberName = contact.displayLabel();
                    if (memberName.isEmpty())
                    {
                        memberName = hbTrId("txt_phob_dblist_unnamed");
                    }
                        
                    // Incase the format is LastnameCommaFirstname, remove the commas
                    memberName = memberName.replace(", ", " ");
                    nameList << memberName;
                    if (nameList.join(", ").length() > CntNamesLengthLimit)
                    {
                        break;
                    }
                }
                QString names = nameList.join(", ").trimmed();
                displayList.append(names);
                displayList.append(hbTrId("(%1)").arg(groupMemberIds.count()));
            }
            else
            {
                displayList.append(hbTrId("txt_phob_dblist_val_no_members_selected"));
            }
            dataList.append(displayList);

            // Default if no image for group 
            bool icon = false;
            QList<QContactAvatar> details = contact.details<QContactAvatar>();
            for (int k = 0;k < details.count();k++)
            {
                if (details.at(k).imageUrl().isValid())
                {
                    dataList.append(QStringList(details.at(k).imageUrl().toString()));
                    icon = true;
                    break;
                }
            }
            if(!icon)
            {
                dataList.append(QStringList("qtg_large_custom"));
            }

            // contact Id for identification
            dataList.append(groupContactIds.at(i));

            mDataPointer->mDataList.append(dataList);
        }
    }
}


bool CntCollectionListModel::isExtensionGroup(const QModelIndex &index)
{
    int row = index.row();
    if (row < 0)
    {
        return false;
    }
    
    QVariantList values = mDataPointer->mDataList.at(row);
    
    if(values.count() > 3)
    {
        return true;
    }
    
    return false;
}

CntViewParameters CntCollectionListModel::extensionGroupActivated(int row)
{
    CntViewParameters params;
    for(int i = 0;i < mDataPointer->mExtensions.value(row)->groupCount();i++)
    {
        CntUiExtensionGroup& group = mDataPointer->mExtensions.value(row)->groupAt(i);
        if (group.serviceId() == mDataPointer->mDataList.at(row)[2].toInt())
        {
            group.activated(params);
            break;
        }
    }
    return params;
}

void CntCollectionListModel::extensionGroupLongPressed(int row, const QPointF& coords, CntExtensionGroupCallback* interface)
{
    for(int i = 0;i < mDataPointer->mExtensions.value(row)->groupCount();i++)
    {
        CntUiExtensionGroup& group = mDataPointer->mExtensions.value(row)->groupAt(i);
        if (group.serviceId() == mDataPointer->mDataList.at(row)[2].toInt())
        {
            group.longPressed(coords, interface);
            break;
        }
    }
}

