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
#include "cntcollectionlistmodel_p.h"
#include "cntcollectionlistmodelworker.h"
#include "cntextensionmanager.h"
#include "cntfavourite.h"
#include "cntdebug.h"

#include <cntuiextensionfactory.h>
#include <cntuigroupsupplier.h>
#include <cntuids.h>
#include <xqsettingskey.h>

#include <qtcontacts.h>
#include <hbglobal.h>
#include <hbicon.h>

#include <thumbnailmanager_qt.h>

/*!
    Constructor
*/
CntCollectionListModel::CntCollectionListModel(QContactManager *manager, CntExtensionManager &extensionManager, QObject *parent)
    : QAbstractListModel(parent)
{
    CNT_ENTRY
    
    d = new CntCollectionListModelData(extensionManager);
    d->mContactManager = manager;
    
    XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
            KCRCntSettings.iUid,
            KCntNameOrdering);
    int order = mSettings.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();
    QString unnamed = hbTrId("txt_phob_dblist_unnamed");
    QString noFavs = hbTrId("txt_phob_dblist_favorites_val_no_favorites_selecte");
    QString noMembers = hbTrId("txt_phob_dblist_val_no_members_selected");
    mThread = new CntCollectionListModelWorker(unnamed, noFavs, noMembers, order);
    connect(mThread, SIGNAL(fetchDone(int, const QString&, int)), this, SLOT(informationUpdated(int, const QString&, int)));
    
    d->mThumbnailManager = new ThumbnailManager(this);
    d->mThumbnailManager->setMode(ThumbnailManager::Default);
    d->mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    d->mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    connect(d->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(onIconReady(QPixmap, void *, int, int)));
    
    doConstruct();
    
    CNT_EXIT
}

/*!
    Destructor
    
    Cancels any pending icon requests for the Thumbnailmanager
*/
CntCollectionListModel::~CntCollectionListModel()
{
    CNT_ENTRY
    
    foreach (int id, d->mIconRequests.keys())
    {
        d->mThumbnailManager->cancelRequest(id);
    }

    d->mIconRequests.clear();
    
    delete mThread;
    
    CNT_EXIT
}

/*!
    Returns requested data for a given index and role. Schedules fetching 2nd row text and
    member count when called for the first time.

    \param index the QModelIndex of the list item
    \param role the requested role
    \return QVariant with requested data
*/
QVariant CntCollectionListModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    
    if ( !validateRowIndex(row) )
        return QVariant();
    
    CollectionItemPointer p = d->mList.at(row);
    if ( p.isNull() )
        return QVariant();
    
    if (!p->fetched)
    {
        mThread->scheduleJob(p->id);
        p->fetched = true;
    }
    
    switch( role )
    {       
        case Qt::DisplayRole:
            return displayRoleData(*p);
        case Qt::DecorationRole:
            return decorationRoleData(*p);
        case Qt::UserRole:
            return p->id;
        default:
            return QVariant();
    }
}

/*!
    Returns the item count of the model

    \param parent not used as all items share the same parent
    \return the current row count
*/
int CntCollectionListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return d->mList.count();
}

/*!
    Removes given row from the model, only supports removing one row at the time

    \param row the first row which is removed
    \param count amount of rows to be removed (not supported)
    \param parent not used as all items share the same parent
    \return success of the operation
*/
bool CntCollectionListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    Q_UNUSED(parent);
    
    if ( !validateRowIndex(row) )
    {
        return false;
    }
    
    beginRemoveRows(parent, row, row);
    d->mList.removeAt(row);
    endRemoveRows();

    return true;
}

/*!
    Adds a row to the model. This is called from addGroup(int localId), so no
    real logic in here.

    \param row the first row which is added
    \param count amount of rows to be added (not supported)
    \param parent not used as all items share the same parent
    \return success of the operation
*/
bool CntCollectionListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    Q_UNUSED(parent);
    
    if ( !validateRowIndex(row) )
    {
        return false;
    }
    
    beginInsertRows(parent, row, row);

    endInsertRows();

    return true;
}

/*!
    Remove given group from the model. Ignore plugin groups.

    \param localId QContactLocalId of the group that should be removed
*/
void CntCollectionListModel::removeGroup(int localId)
{
    CNT_ENTRY
    
    for (int i = 0;i < rowCount();i++)
    {
        if (!d->mList.at(i)->isPlugin && d->mList.at(i)->id == localId)
        {
            removeRow(i);
            break;
        }
    }
    
    CNT_EXIT
}

/*!
    Add a new group to the model. This also takes care of putting it in the
    correct row.

    \param localId QContactLocalId of the group that should be added
*/
void CntCollectionListModel::addGroup(int localId)
{
    CNT_ENTRY
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QContactSortOrder sortOrderGroupName;
    sortOrderGroupName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldCustomLabel);
    sortOrderGroupName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> groupsOrder;
    groupsOrder.append(sortOrderGroupName);

    QList<QContactLocalId> groupContactIds = d->mContactManager->contactIds(groupFilter, groupsOrder);
    
    int row = 1;
    
    for (int i = 0;i < groupContactIds.count();i++)
    {
        if (groupContactIds.at(i) == localId)
        {
            row += d->mExtensions.count();
            
            CollectionItemPointer item(new CntCollectionItem());
            
            QContactFetchHint noRelationshipsFetchHint;
            noRelationshipsFetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
            
            QContact contact = d->mContactManager->contact(groupContactIds.at(i), noRelationshipsFetchHint);
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            
            if (groupName.isNull())
            {
                item->groupName = hbTrId("txt_phob_dblist_unnamed");
            }
            else
            {
                item->groupName = groupName;
            }

            item->icon = HbIcon("qtg_large_custom");
            
            QList<QContactAvatar> details = contact.details<QContactAvatar>();
            for (int k = 0;k < details.count();k++)
            {
                if (details.at(k).imageUrl().isValid())
                {
                    int id = d->mThumbnailManager->getThumbnail(details.at(k).imageUrl().toString());
                    d->mIconRequests.insert(id, groupContactIds.at(i));
                    break;
                }
            }
            
            // contact Id for identification
            item->id = groupContactIds.at(i);
            item->secondLineText = " ";
            item->memberCount = -1;

            d->mList.insert(row, item);
            insertRow(row);
            break;
        }
        else if (groupContactIds.at(i) != d->mFavoriteGroupId)
        {
            row++;
        }
    }
    
    CNT_EXIT
}

/*!
    Check if list item at given index is coming from an extension plugin or not.

    \param index the QModelIndex of the list item
    \return success of the operation
*/
bool CntCollectionListModel::isExtensionGroup(const QModelIndex &index)
{
    int row = index.row();
    if ( !validateRowIndex(row) )
    {
        return false;
    }
    
    return d->mList.at(row)->isPlugin;
}

/*!
    Called when a list item from a plugin is activated. Used for view switching.

    \param row the row of the activated plugin item
    \return view parameters used for view switching
*/
CntViewParameters CntCollectionListModel::extensionGroupActivated(int row)
{
    CntViewParameters params;
    for(int i = 0;i < d->mExtensions.value(row)->groupCount();i++)
    {
        CntUiExtensionGroup& group = d->mExtensions.value(row)->groupAt(i);
        if (group.serviceId() == d->mList.at(row)->id)
        {
            group.activated(params);
            break;
        }
    }
    return params;
}

/*!
    Called when a list item from a plugin is long pressed. Callback is done asynch, as HbMenu usage
    is assumed.

    \param row the row of the long pressed plugin item
    \param coords the coordinates of the long tap gesture
    \param interface callback interface for asynch handling
*/
void CntCollectionListModel::extensionGroupLongPressed(int row, const QPointF& coords, CntExtensionGroupCallback* interface)
{
    CNT_ENTRY
    
    for(int i = 0;i < d->mExtensions.value(row)->groupCount();i++)
    {
        CntUiExtensionGroup& group = d->mExtensions.value(row)->groupAt(i);
        if (group.serviceId() == d->mList.at(row)->id)
        {
            group.longPressed(coords, interface);
            break;
        }
    }
    
    CNT_EXIT
}

/*!
    Get the index for a given group id.

    \param localId the id of the group
    \return QModelIndex of the group
*/
QModelIndex CntCollectionListModel::indexOfGroup(int localId)
{
    for (int i = 0;i < d->mList.count();i++)
    {
        if (d->mList.at(i)->id == localId && !d->mList.at(i)->isPlugin)
        {
            return index(i);
        }
    }
    
    return QModelIndex();
}

/*!
    Initialize different groups.
*/
void CntCollectionListModel::doConstruct()
{
    CNT_ENTRY
    
    initializeStaticGroups();
    initializeExtensions();
    initializeUserGroups();
    
    CNT_EXIT
}

/*!
    Initialize static group(s), only favorites for now.
*/
void CntCollectionListModel::initializeStaticGroups()
{
    CNT_ENTRY
    
    CollectionItemPointer item(new CntCollectionItem());
    
    item->groupName = hbTrId("txt_phob_dblist_favorites");
    
    d->mFavoriteGroupId = CntFavourite::createFavouriteGroup( d->mContactManager );
    
    item->icon = HbIcon("qtg_large_favourites");
    item->id = d->mFavoriteGroupId;
    item->secondLineText = " ";
    item->memberCount = -1;
    
    d->mList.append(item);
    
    CNT_EXIT
}

/*!
    Initialize extension groups from plugins.
*/
void CntCollectionListModel::initializeExtensions()
{
    CNT_ENTRY
    
    for(int i = 0;i < d->mExtensionManager.pluginCount();i++)
    {
        CntUiGroupSupplier* groupSupplier = d->mExtensionManager.pluginAt(i)->groupSupplier();
        if (groupSupplier)
        {
            if (groupSupplier->isAsynch())
            {
                connect(groupSupplier, SIGNAL(groupsReady()), this, SLOT(extensionGroupsReady()));
            }
            else
            {
                for (int j = 0;j < groupSupplier->groupCount();j++)
                {
                    const CntUiExtensionGroup& group = groupSupplier->groupAt(j);
                    
                    CollectionItemPointer item(new CntCollectionItem());
                    
                    item->groupName = group.name();
                    item->secondLineText = group.extraText();
                    item->memberCount = group.memberCount();
                    item->icon = HbIcon(group.groupIcon());
                    item->secondaryIcon = HbIcon(group.extraIcon());
                    
                    item->id = group.serviceId();
                    item->isPlugin = true;
                    item->fetched = true;
                    
                    d->mExtensions.insert(rowCount(), groupSupplier);
                    d->mList.append(item);
                }
            }
        }
    }
    
    CNT_EXIT
}

/*!
    Initialize user defined custom groups.
*/
void CntCollectionListModel::initializeUserGroups()
{
    CNT_ENTRY
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QContactSortOrder sortOrderGroupName;
    sortOrderGroupName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldCustomLabel);
    sortOrderGroupName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> groupsOrder;
    groupsOrder.append(sortOrderGroupName);

    QList<QContactLocalId> groupContactIds = d->mContactManager->contactIds(groupFilter, groupsOrder);

    for(int i = 0;i < groupContactIds.count();i++)
    {
        if(groupContactIds.at(i) != d->mFavoriteGroupId )
        {
            CollectionItemPointer item(new CntCollectionItem());
            
            QContactFetchHint noRelationshipsFetchHint;
            noRelationshipsFetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
            
            QContact contact = d->mContactManager->contact(groupContactIds.at(i), noRelationshipsFetchHint);
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            
            if (groupName.isNull())
            {
                item->groupName = hbTrId("txt_phob_dblist_unnamed");
            }
            else
            {
                item->groupName = groupName;
            }

            item->icon = HbIcon("qtg_large_custom");
            
            QList<QContactAvatar> details = contact.details<QContactAvatar>();
            for (int k = 0;k < details.count();k++)
            {
                if (details.at(k).imageUrl().isValid())
                {
                    int id = d->mThumbnailManager->getThumbnail(details.at(k).imageUrl().toString());
                    d->mIconRequests.insert(id, groupContactIds.at(i));
                    break;
                }
            }
            
            // contact Id for identification
            item->id = groupContactIds.at(i);
            item->secondLineText = " ";
            item->memberCount = -1;

            d->mList.append(item);
        }
    }
    
    CNT_EXIT
}

/*!
    Get Qt::DisplayRole data from a CntCollectionItem.

    \param item the collection item from where data is read
    \return QVariant containing the requested data
*/
QVariant CntCollectionListModel::displayRoleData(const CntCollectionItem& item) const
{
    QStringList list;

    list << item.groupName << item.secondLineText;
    
    if (item.memberCount > 0)
    {
        list << QString("(%1)").arg(item.memberCount);
    }
    else if (item.memberCount < 0)
    {
        list << " ";
    }
    
    return QVariant(list);
}

/*!
    Get Qt::DecorationRole data from a CntCollectionItem.

    \param item the collection item from where data is read
    \return QVariant containing the requested data
*/
QVariant CntCollectionListModel::decorationRoleData(const CntCollectionItem& item) const
{
    QList<QVariant> icons;
    icons << item.icon;
    
    if (!item.secondaryIcon.isNull())
    {
        icons << item.secondaryIcon;
    }
    
    return QVariant(icons);
}

/*!
    Check if the given index is valid or not.

    \param index the row to be checked
    \return index validity
*/
bool CntCollectionListModel::validateRowIndex(const int index) const
{
    return (index < rowCount() && index >= 0);
}

/*!
    Update CollectionItem at the given row with the new information and notify the list
    about the change.

    \param row the row receiving the updated information
    \param secondRowText text to be shown in the second row
    \param memberCount the amount of members the group has (shown in text-3 in HbListViewItem)
*/
void CntCollectionListModel::informationUpdated(int id, const QString& secondRowText, int memberCount)
{
    CNT_ENTRY
    
    for (int i = 0;i < d->mList.count();i++)
    {
        if (d->mList.at(i)->id == id && !d->mList.at(i)->isPlugin)
        {
            CollectionItemPointer item = d->mList.at(i);
            
            item->secondLineText = secondRowText;
            item->memberCount = memberCount;
            
            int idx = d->mList.indexOf(item);
            emit dataChanged(index(idx, 0), index(idx, 0));
            
            break;
        }
    }
    
    CNT_EXIT
}

/*!
    Update CollectionItem mapped to this request (id) with the new icon and notify the list
    about the change.

    \param pixmap requested icon
    \param data not used
    \param id the id of the request
    \param error 0 if no error happened
*/
void CntCollectionListModel::onIconReady(const QPixmap& pixmap, void *data, int id, int error)
{
    CNT_ENTRY
    
    Q_UNUSED(data);
    
    if (error == 0)
    {
        int localId = d->mIconRequests.take(id);
        
        for (int i = 0;i < d->mList.count();i++)
        {
            if (d->mList.at(i)->id == localId && !d->mList.at(i)->isPlugin)
            {
                CollectionItemPointer item = d->mList.at(i);
                
                item->icon = HbIcon(pixmap);
                
                int idx = d->mList.indexOf(item);
                emit dataChanged(index(idx, 0), index(idx, 0));
                
                break;
            }
        }
    }

    CNT_EXIT
}

/*!
    Slot to handle asynchronously initiated extension groups. As
    these are added right after the static favorites group, this takes
    also care of updating the mapping between row and CntUiGroupSuppliers
    in case there was already some synchronously loaded extension groups.
*/
void CntCollectionListModel::extensionGroupsReady()
{
    CntUiGroupSupplier* groupSupplier = static_cast<CntUiGroupSupplier*>(sender());
    
    int addedCount = groupSupplier->groupCount();
    
    if (addedCount > 0)
    {
        QList<int> rowList = d->mExtensions.keys();
        QList<CntUiGroupSupplier*> supplierList = d->mExtensions.values();
        
        d->mExtensions.clear();
        
        for (int i = 0;i < rowList.count();i++)
        {
            int row = rowList.at(i) + addedCount;
            CntUiGroupSupplier* supplier = supplierList.at(i);
            
            d->mExtensions.insert(row, supplier);
        }
        
        
        beginInsertRows(QModelIndex(), 1, groupSupplier->groupCount());
        
        for (int j = 0;j < groupSupplier->groupCount();j++)
        {
            const CntUiExtensionGroup& group = groupSupplier->groupAt(j);
            
            CollectionItemPointer item(new CntCollectionItem());
            
            item->groupName = group.name();
            item->secondLineText = group.extraText();
            item->memberCount = group.memberCount();
            item->icon = HbIcon(group.groupIcon());
            item->secondaryIcon = HbIcon(group.extraIcon());
            
            item->id = group.serviceId();
            item->isPlugin = true;
            item->fetched = true;
            
            d->mExtensions.insert(1 + j, groupSupplier);
            d->mList.insert(1 + j, item);
        }
        
        endInsertRows();
    }
}
