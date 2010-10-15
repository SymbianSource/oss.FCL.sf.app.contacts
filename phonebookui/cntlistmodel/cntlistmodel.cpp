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

#include <QSet>
#include <qtcontacts.h>
#include <hbindexfeedback.h>
#include <hbframebackground.h>
#include <hbframedrawer.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <cntlistmodel.h>
#include <cntlistmodel_p.h>
#include <cntcache.h>
#include <cntdisplaytextformatter.h>
#include <cntdebug.h>

/*!
    \class CntListModel
    \brief List model for list with contacts.

    CntListModel is a list model view for contacts database content. It uses
    CntCache to fetch and cache entries displayed on the screen.
 */

const uint dummyMyCardId = 0;

/*!
    Construct a new CntListModel object using manager as the QContactManager
    instance to communicate with the contacts database.
 
    \param manager      a QContactManager instance to be used for communications
                        with the contacts persistant store
    \param filter       a filter that selects the contacts to show in the list
    \param showMyCard   true if my card entry should be shown at the top of
                        the list, otherwise false
    \param parent       parent of this QObject
 */
CntListModel::CntListModel(QContactManager* manager,
                           const QContactFilter& filter,
                           bool showMyCard,
                           QObject *parent)
    : QAbstractListModel(parent)
{
    CNT_ENTRY

    // set up data
    d = new CntListModelData(manager, filter, showMyCard);

    // fetch IDs
    updateContactIdsArray();

    // get current setting how to show an item in the name list and subscribe for changes
    d->mSettings = new XQSettingsManager;
    d->mNameListRowSettingkey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
        KCRCntSettings.iUid,
        KCntNameListRowSetting);
    d->mCurrentRowSetting = d->mSettings->readItemValue(*d->mNameListRowSettingkey,
        XQSettingsManager::TypeInt).toInt();
    d->mSettings->startMonitoring(*d->mNameListRowSettingkey, XQSettingsManager::TypeInt);
    connect(d->mSettings, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)),
            this, SLOT(handleRowSettingChanged(const XQSettingsKey&, const QVariant&)));

    // listen to cache for changes in contacts
    connect(d->mCache, SIGNAL(contactInfoUpdated(QContactLocalId)),
            this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    connect(d->mCache, SIGNAL(contactsAdded(const QList<QContactLocalId>&)),
            this, SLOT(handleAdded(const QList<QContactLocalId>&)));
    connect(d->mCache, SIGNAL(contactsChanged(const QList<QContactLocalId>&)),
            this, SLOT(handleChanged(const QList<QContactLocalId>&)));
    connect(d->mCache, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(handleRemoved(const QList<QContactLocalId>&)));
    connect(d->mCache, SIGNAL(dataChanged()),
            this, SLOT(refreshModel()));

    // listen to contactmanager for changes in relationships or mycard
    connect(d->mContactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)),
            this, SLOT(handleMyCardChanged(const QContactLocalId&, const QContactLocalId&)));
    connect(d->mContactManager, SIGNAL(relationshipsAdded(const QList<QContactLocalId>&)),
            this, SLOT(handleAddedRelationship(const QList<QContactLocalId>&)));
    connect(d->mContactManager, SIGNAL(relationshipsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(handleRemovedRelationship(const QList<QContactLocalId>&)));

    CNT_EXIT
}

CntListModel::~CntListModel()
{
}

/*!
    Return the data to be used by the view or delegates for a particular
    item and role.

    \param index The index of the item to return data about.
    \param role The data should be relevant for this particular purpose.
    \return QVariant The data for the specified index and role.
*/
QVariant CntListModel::data(const QModelIndex &index, int role) const
{
    CNT_ENTRY

    QVariant returnData;
    int row = index.row();

    // check that row is ok
    if (!isValidRow(row)) {
        // invalid row
        return QVariant();
    }
    
    // update current contact if needed
    if (row != d->mCurrentRow) {
        delete d->mCurrentContact;
        if (d->mContactIds[row] == dummyMyCardId) {
            // row contains dummy MyCard, so create dummy CntContactInfo
            d->mCurrentContact = NULL;
        } else {
            d->mCurrentContact = d->mCache->fetchContactInfo(row, d->mContactIds);
        }
        d->mCurrentRow = row;
    }

    if (role == Qt::DisplayRole) {
        returnData = dataForRole(row, role);
    } else if (role == Hb::IndexFeedbackRole) {
        if (row > 0 || (d->mMyCardId != d->mContactIds[0] && dummyMyCardId != d->mContactIds[0])) {
            returnData = dataForRole(row, role).toStringList().at(0).toUpper();
        }
    } else if (role == Qt::BackgroundRole) {
        if (d->mMyCardId == d->mContactIds[row] || dummyMyCardId == d->mContactIds[row]) {
            returnData = HbFrameBackground("qtg_fr_list_parent_normal", HbFrameDrawer::NinePieces);
        }
    } else if (role == Qt::DecorationRole) {
        if (d->mCurrentRowSetting == CntTwoRowsNameAndPhoneNumber) {
            //icon fits only if user selected 2 rows in each name list item
            QList<QVariant> icons;

            if (d->mCurrentContact != NULL && !d->mCurrentContact->icon1().isNull()) {
                icons.append(d->mCurrentContact->icon1());
            } else if (d->mMyCardId == d->mContactIds[row] || dummyMyCardId == d->mContactIds[row]) {
                icons.append(d->mDefaultMyCardIcon);
            } else {
                icons.append(d->mDefaultIcon);
            }

            if (d->mCurrentContact != NULL && !d->mCurrentContact->icon2().isNull()) {
                icons.append(d->mCurrentContact->icon2());
            }

            returnData = icons;
        }
    }
    
    CNT_EXIT
    return returnData;
}

/*!
    Get the number of rows (contacts) in this model.

    \param parent Optional parent index value.
    \return Number of contacts in this model.
 */
int CntListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->mContactIds.count();
}

/*!
    Read a full contact entry from the database for the given model
    index value. Only the row part of the index information will be
    read. This is just an overload of CntListModel::contact() that
    supports old behaviour and calls:
         CntListModel::contact(int row);

    The entry at the requested row will have its full contact information
    (all fields) read from the database and returned as a QContact instance.

    \param index Index for the sought contact entry in this model.
    \return A newly constructed QContact instance for this entry - ownership
     is transferred to the caller.
 */
QContact CntListModel::contact(const QModelIndex &index) const
{
    return contact(index.row());
}

/*!
    Returns the id for the contact at the requested row.

    \param index Index for the sought contact entry in this model.
    \return The id for the contact, 0 if invalid index.
 */
QContactLocalId CntListModel::contactId(const QModelIndex &index) const
{
    CNT_ENTRY

    if (!isValidRow(index.row())) {
        return 0;
    }

    CNT_EXIT
    return d->mContactIds[index.row()];
}

/*!
    Return an index that points to the row relating to the supplied contact.
     E.g. if the contact is at row 7, the index with the following properties
     is returned:
         index.row() == 7
 
    \param contact The contact for whose row an index is required
    \return a QModelIndex with the row set to match that of the contact.
 */
QModelIndex CntListModel::indexOfContact(const QContact &contact) const
{
    return createIndex(row(contact.localId()), 0);
}

/*!
    Return an index that points to the row relating to the supplied contact id.
     E.g. if the contact with this id is at row 7, the index with the following
     properties is returned:
         index.row() == 7
 
    \param contactId The id of the contact for whose row an index is required
    \return a QModelIndex with the row set to match that of the contact id.
 */
QModelIndex CntListModel::indexOfContactId(const QContactLocalId &contactId) const
{
    return createIndex(row(contactId), 0);
}

/*!
    Set new filter and sort order for the model.

    \param contactFilter New contact filter.
 */
void CntListModel::setFilter(const QContactFilter& contactFilter)
{
    CNT_ENTRY

    d->setFilter(contactFilter);

    //refresh model
    updateContactIdsArray();

    beginResetModel();
    endResetModel();

    CNT_EXIT
}

/*!
    Enable/disable MyCard appearance in the model.

    \param enabled Status of MyCard appearance in the model.
 */
void CntListModel::showMyCard(bool enabled)
{
    CNT_ENTRY

    if (d->mShowMyCard == enabled) {
        return;
    }
    
    QContactLocalId myCardId = d->mMyCardId;
    if (enabled) {
        //add MyCard to the list
        if (myCardId <= 0) {
            // create a placeholder for MyCard
            d->mContactIds.insert(0, dummyMyCardId);
        } else {
            d->mContactIds.insert(0, myCardId);
        }
    } else {
        // remove MyCard from the list
        if (myCardId <= 0) {
            d->mContactIds.removeOne(dummyMyCardId);
        } else {
            d->mContactIds.removeOne(myCardId);
        }
    }
    d->mShowMyCard = enabled;
    d->mCurrentRow = -1;

    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
    \return true if MyCard is shown, false otherwise.
 */
bool CntListModel::isMyCardShown() const
{
    return d->mShowMyCard;
}

/*!
    \return the id of the MyCard contact.
 */
QContactLocalId CntListModel::myCardId() const
{
    return d->mMyCardId;
}

/*!
    Gets the filtered list of the contact Ids in a sorted order  
    
    \return Error status
 */
void CntListModel::updateContactIdsArray()
{
    CNT_ENTRY

    QContactDetailFilter* detailFilter = NULL;

    if (d->mFilter.type() == QContactFilter::ContactDetailFilter) {
        detailFilter = static_cast<QContactDetailFilter*>(&d->mFilter);
    }

    // special handling for all-contacts filter
    if (detailFilter
        && detailFilter->detailDefinitionName() == QContactType::DefinitionName
        && detailFilter->detailFieldName() == QContactType::FieldType
        && detailFilter->value() == QContactType::TypeContact) {
        d->mContactIds = d->mCache->sortIdsByName(NULL);
    } else if (detailFilter
        && detailFilter->detailDefinitionName() == QContactDisplayLabel::DefinitionName
        && detailFilter->detailFieldName() == QContactDisplayLabel::FieldLabel
        && detailFilter->matchFlags() == Qt::MatchStartsWith) {
        QStringList searchList = detailFilter->value().toStringList();
        d->mContactIds = d->mCache->sortIdsByName(searchList);
    } else {
        QSet<QContactLocalId> filterIds = d->mContactManager->contactIds(d->mFilter).toSet();
        d->mContactIds = d->mCache->sortIdsByName(&filterIds);
    }
    
    //find MyCard contact and move it to the first position
    QContactLocalId myCardId = d->mMyCardId;
    if (myCardId > 0) {
        // MyCard exists
        d->mContactIds.removeOne(myCardId);
        if (d->mShowMyCard) {
            d->mContactIds.insert(0, myCardId);
        }
    } else if (d->mShowMyCard) {
        // create a placeholder for MyCard
        d->mContactIds.insert(0, dummyMyCardId);
    }
    
    CNT_EXIT
}

/*!
    Read a full contact entry from the database for the row number.

    The entry at the requested row will have its full contact information
    (all fields) read from the database and returned as a QContact instance.

    \param row Row at which the sought contact entry is in this model.
    \return A newly constructed QContact instance for this entry - ownership
     is transferred to the caller.
 */
QContact CntListModel::contact(int row) const
{
    CNT_ENTRY

    if (!isValidRow(row) || d->mContactIds[row] == dummyMyCardId) {
        return QContact();
    }

    CNT_EXIT
    return d->mContactManager->contact(d->mContactIds[row]);
}

/*!
    Verify specified row id is valid.

    \param row A row number
    \return bool indicating validity of row id
 */
bool CntListModel::isValidRow(int row) const
{
	return (row >= 0 && row < rowCount());
}

/*!
    Fetch the row containing the contact with the specified id.

    \param contactId The id of the contact
    \return the row of the contact or -1 if no item matched.
 */
int CntListModel::row(const QContactLocalId &contactId) const
{
    return d->mContactIds.indexOf(contactId);
}

/*!
    Return the data to be used by the view for a display role.

    \param row The row of the item to return data about.
    \param column The column of the item to return data about.
    \return QVariant The data for the specified index.
 */
QVariant CntListModel::dataForRole(int row, int role) const
{
    CNT_ENTRY

    QStringList list;
    QString name;
    QString infoText;
    bool isSelfContact = false;
    bool isNonEmptySelfContact = false;
    
    QContactLocalId id = d->mContactIds[row];
    if (d->mMyCardId == id || dummyMyCardId == id) {
        isSelfContact = true;
        if (d->mCurrentContact == NULL) {
            // empty card
            name = hbTrId("txt_phob_dblist_mycard");
            infoText = hbTrId("txt_phob_dblist_mycard_val_create_my_identity");
        } else {
            isNonEmptySelfContact = true;
        }
    }

    if (!isSelfContact || isNonEmptySelfContact) {
        name = d->mCurrentContact->name();
        if (name.isEmpty()) {
            name = hbTrId("txt_phob_list_unnamed");
        }
        infoText = d->mCurrentContact->text();
    }
    
    if (role == Qt::DisplayRole) {
        list << d->mFormat->formattedText(name, d->mFilter);
    } else {
        list << name;
    }
    
    if (!isNonEmptySelfContact) {
        if (d->mCurrentRowSetting == CntTwoRowsNameAndPhoneNumber) {
            //add additional text only if user wants 2 rows in each name list item
            list << infoText;
        }
    }

    CNT_EXIT
    return list;
}

/*!
    Handle adding of contacts. 

    \param contactIds Ids of contacts added.
 */
void CntListModel::handleAdded(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    // if contacts are added already, no need to do anything
    bool newContacts = false;
    for (int k = 0; k < contactIds.count() && !newContacts; k++) { 
        if (!d->mContactIds.contains(contactIds.at(k))) {
            newContacts = true;
        }
    }
    if (!newContacts) {
        return;
    }

    // invalidate cached contact
    d->mCurrentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->mContactIds;
    updateContactIdsArray();

    QList<int> newRows;
    for (int i = 0; i < d->mContactIds.count(); i++) {
        if (!oldIdList.contains(d->mContactIds.at(i))) {
            newRows.append(i);
        }
    }

    if (newRows.size() == 1) {
        beginInsertRows(QModelIndex(), newRows.at(0), newRows.at(0));
        endInsertRows();
    } else {
        beginResetModel();
        reset();
        endResetModel();
    }

    CNT_EXIT
}

/*!
    Handle changes in contacts. 

    \param contactIds Ids of contacts changed.
 */
void CntListModel::handleChanged(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    if (contactIds.count() == 0) {
        return;
    }

    //invalidate cached contact
    d->mCurrentRow = -1; 

    int firstChangedContactPosBefore = row(contactIds.at(0));  
    updateContactIdsArray();
    int firstChangedContactPosAfter = row(contactIds.at(0)); 

    // if only one contact was updated and its position didn't change,
    // refresh the corresponding row
    if (contactIds.count() == 1 &&
        firstChangedContactPosBefore == firstChangedContactPosAfter &&
        firstChangedContactPosBefore >= 0) {
        QModelIndex top = index(firstChangedContactPosBefore);
        QModelIndex bottom = index(firstChangedContactPosBefore);
        emit dataChanged(top, bottom);
    } else {
        beginResetModel();
        reset();
        endResetModel();
    }
    
    CNT_EXIT
}

/*!
    Handle removing of contacts. 

    \param contactIds Ids of contacts removed.
 */
void CntListModel::handleRemoved(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    bool removeContacts = false;
    QList<QContactLocalId> idList = d->mContactIds;
    for (int k = 0; k < contactIds.count() && !removeContacts; k++) {
        if(idList.contains(contactIds.at(k))) {
            removeContacts = true;
        }
    }
    if (!removeContacts) {
        return;
    }
    
    //Find contacts to remove (=rows)
    QList<int> removeRows;
    for(int i = 0; i < contactIds.count(); i++) {
        if (idList.contains(contactIds.at(i))) {
            removeRows.append(row(contactIds.at(i)));
        }
    }
    
    // invalidate cached contact
    d->mCurrentRow = -1;
    
    int myCardRow = -1;
    if (contactIds.contains(d->mMyCardId)) {
        myCardRow = row(d->mMyCardId);
        d->mMyCardId = 0;
    }

    // remove rows starting from the bottom
    qSort(removeRows.begin(), removeRows.end(), qGreater<int>());
    foreach (int row, removeRows) {
        if (row != myCardRow || !d->mShowMyCard) {
            beginRemoveRows(QModelIndex(), row, row);
            endRemoveRows();
        }
    }

    foreach (QContactLocalId id, contactIds) {
        d->mContactIds.removeOne(id);
    }

    if (myCardRow != -1 && d->mShowMyCard) {
        d->mContactIds.insert(0, dummyMyCardId);
        QModelIndex index = createIndex(0, 0);
        emit dataChanged(index, index);
    }

    CNT_EXIT
}

/*!
    Handle my card change. 

    \param oldId Id of the old MyCard.
    \param newId Id of the new MyCard.
 */
void CntListModel::handleMyCardChanged(const QContactLocalId& /*oldId*/, const QContactLocalId& newId)
{
    CNT_ENTRY
    
    //invalidate cached contact
    d->mCurrentRow = -1;
    d->mMyCardId = newId;

    updateContactIdsArray();
    
    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
    Handle added relationships. 

    \param contactIds Ids of contacts added (group id and contact ids).
 */
void CntListModel::handleAddedRelationship(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    if (contactIds.contains(d->mGroupId)) {
        foreach (QContactLocalId id, contactIds) {
            if (id != d->mGroupId && !d->mContactIds.contains(id)) {
                // at least one new contact id has been added to this group,
                // so update the model
                updateRelationships();
                break;
            }
        }
    }

    CNT_EXIT
}

/*!
    Handle removed relationships. 

    \param contactIds Ids of contacts removed from a relationship (group id and contact ids).
 */
void CntListModel::handleRemovedRelationship(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    if (contactIds.contains(d->mGroupId)) {
        foreach (QContactLocalId id, contactIds) {
            if (d->mContactIds.contains(id)) {
                // at least one new contact id has been removed from this group,
                // so update the model
                updateRelationships();
                break;
            }
        }
    }

    CNT_EXIT
}

/*!
    Updates the model to reflect changes in the relationships.
 */
void CntListModel::updateRelationships()
{
    CNT_ENTRY

    //invalidate cached contact
    d->mCurrentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->mContactIds;
    updateContactIdsArray();
    
    // find all changed rows
    QList<int> newRows, removedRows;
    for (int i = 0; i < d->mContactIds.count(); i++) {
        if (!oldIdList.contains(d->mContactIds.at(i))) {
            newRows.append(i);
        }
    }
    for (int i = 0; i < oldIdList.count(); i++) {
        if (!d->mContactIds.contains(oldIdList.at(i))) {
            removedRows.append(i);
        }
    }

    // currently only one-row-changes are handled with beginInsertRows/beginRemoveRows
    // if there are more than one change, the whole model is reset
    if (removedRows.count() == 1 && newRows.count() == 0) {
        beginRemoveRows(QModelIndex(), removedRows.at(0), removedRows.at(0));
        endRemoveRows();
    } else if (newRows.count() == 1 && removedRows.count() == 0) {
        beginInsertRows(QModelIndex(), newRows.at(0), newRows.at(0));
        endInsertRows();
    } else {
        beginResetModel();
        endResetModel();
    }

    CNT_EXIT
}

/*!
    Notify views that info for a contact has become
    available or has changed.

    \param contactId the id of the contact
 */
void CntListModel::handleContactInfoUpdated(QContactLocalId contactId)
{
    CNT_ENTRY

    QModelIndex index = createIndex(row(contactId), 0);
    if (index.row() == d->mCurrentRow) {
        d->mCurrentRow = -1;
    }

    if (isValidRow(index.row())) {
        emit dataChanged(index, index);
    }

    CNT_EXIT
}

/*!
    Handle a change in how name list item should be represented.

    \param key Central repository key
    \param value New value in the key
 */
void CntListModel::handleRowSettingChanged(const XQSettingsKey& /*key*/, const QVariant& value)
{
    bool ok = false;
    int newSetting = value.toInt(&ok);
    if (ok) {
        d->mCurrentRowSetting = newSetting;

        if (rowCount() > 0) {
            QModelIndex first = createIndex(0, 0);
            QModelIndex last = createIndex(rowCount() - 1, 0);
            emit dataChanged(first, last);
        }
    }
}

/*!
    Handle a change in data.
 */
void CntListModel::refreshModel()
{
    d->mCurrentRow = -1;

    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
}
