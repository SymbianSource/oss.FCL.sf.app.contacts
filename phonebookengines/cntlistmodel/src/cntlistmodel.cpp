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
#include "cntlistmodel_p.h"
#include "cntlistmodel.h"
#include "cntcache.h"
#include "cntdebug.h"

#include <qtcontacts.h>
#include <QSet>
#include <QTimerEvent>

#include <hbindexfeedback.h>
#include <hbframebackground.h>
#include <hbframedrawer.h>
#include <xqsettingsmanager.h> // for reading cenrep keys
#include <xqsettingskey.h>

const uint dummyMyCardId = 0;

/*!
 * Constuct a new contact model instance. The model will own its handle
 * to the default persisted store.
 */
CntListModel::CntListModel(const QContactFilter& contactFilter,
                         bool showMyCard,
                         QObject *parent)
    : QAbstractListModel(parent)
{
    CNT_ENTRY
    m_defaultIcon = HbIcon("qtg_large_avatar");
    m_defaultMyCardIcon = HbIcon("qtg_large_mycard");

    d = new CntListModelData(contactFilter, showMyCard);
    d->m_contactManager = new QContactManager;
    d->m_ownedContactManager = true;
    d->m_cache = CntCache::instance();
    connect(d->m_cache, SIGNAL(contactInfoUpdated(QContactLocalId)), this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    d->m_myCardId = d->m_contactManager->selfContactId();
    if (doConstruct() != QContactManager::NoError) {
        throw("exception");
    }
    CNT_EXIT
}

/*!
 * Construct a new CntListModel object using manager as the QContactManager
 * instance to communicate with the contacts database.
 *
 * \param manager A QContactManager instance to be used for
 *  communications with the contacts persistant store.
 */
CntListModel::CntListModel(QContactManager* manager,
                         const QContactFilter& contactFilter,
                         bool showMyCard,
                         QObject *parent)
    : QAbstractListModel(parent)
{
    CNT_ENTRY
    m_defaultIcon = HbIcon("qtg_large_avatar");
    m_defaultMyCardIcon = HbIcon("qtg_large_mycard");
    m_defaultMyCardIcon = HbIcon("qtg_large_mycard");

    d = new CntListModelData(contactFilter, showMyCard);
    d->m_contactManager = manager;
    d->m_cache = CntCache::instance();
    connect(d->m_cache, SIGNAL(contactInfoUpdated(QContactLocalId)), this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    d->m_myCardId = d->m_contactManager->selfContactId();     
    if (doConstruct() != QContactManager::NoError) {
        throw("exception");
    }
    CNT_EXIT
}

CntListModel::~CntListModel()
{
}

/*!
 * Return the data to be used by the view or delegates for a particular
 * item and role.
 *
 * \param index The index of the item to return data about.
 * \param role The data should be relevant for this particular purpose.
 * \return QVariant The data for the specified index and role.
 */
QVariant CntListModel::data(const QModelIndex &index, int role) const
{
    CNT_ENTRY
    int row = index.row();

    // check that row is ok
    if (!validRowId(row)) {
        // invalid row
        return QVariant();
    }
    
    // update current contact if needed
    if (row != d->m_currentRow ) {
        if (d->m_contactIds[row] == dummyMyCardId) {
            // row contains dummy MyCard, so create dummy CntContactInfo
            d->m_currentContact = CntContactInfo();
        }
        else {
            d->m_currentContact = d->m_cache->fetchContactInfo(row, d->m_contactIds);
        }
        d->m_currentRow = row;
    }
    
    if (role == Qt::DisplayRole) {
        return dataForDisplayRole(row);
    }
    else if (role == Hb::IndexFeedbackRole) {
        if (row == 0 && (d->m_myCardId == d->m_contactIds[0] || dummyMyCardId == d->m_contactIds[0])) {
            // do not include MyCard in index feedback
            return QVariant();
        }
        else {
            return dataForDisplayRole(row).toStringList().at(0).toUpper();
        }
    }
    else if (role == Qt::BackgroundRole) {
        if (d->m_myCardId == d->m_contactIds[row] || dummyMyCardId == d->m_contactIds[row]) {
            return HbFrameBackground("qtg_fr_list_parent_normal", HbFrameDrawer::NinePieces);
        }
    }
    else if (role == Qt::DecorationRole) {
        QList<QVariant> icons;
        HbIcon avatar = d->m_currentContact.icon1();
        HbIcon statusIcon = d->m_currentContact.icon2();
    
        if (!avatar.isNull()) {
            icons.append(avatar);
        }
        else if (d->m_myCardId == d->m_contactIds[row] || dummyMyCardId == d->m_contactIds[row]) {
            icons.append(m_defaultMyCardIcon);
        }
        else {
            icons.append(m_defaultIcon);
        }

        if (!statusIcon.isNull()) {
            icons.append(statusIcon);
        }

        return icons;
    }
    CNT_EXIT
    return QVariant();
}

/*!
 * Get the number of rows (contacts) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of contacts in this model.
 */
int CntListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->m_contactIds.count();
}

/*!
 * Read a full contact entry from the database for the given model
 * index value. Only the row part of the index information will be
 * read. This is just an overload of CntListModel::contact() that
 * supports old behaviour and calls:
 *      CntListModel::contact(int row);
 *
 * The entry at the requested row will have its full contact information
 * (all fields) read from the database and returned as a QContact instance.
 *
 * \param index Index for the sought contact entry in this model.
 * \return A newly constructed QContact instance for this entry - ownership
 *  is transferred to the caller.
 *
 */
QContact CntListModel::contact(const QModelIndex &index) const
{
    return contact(index.row());
}

/*!
 * Return an index that points to the row relating to the supplied contact.
 *  E.g. if the contact is at row 7, the index with the following properties
 *  is returned:
 *      index.row() == 7
 
 * \param contact The contact for whose row an index is required
 * \return a QModelIndex with the row set to match that of the contact.
 */
QModelIndex CntListModel::indexOfContact(const QContact &contact) const
{
    return createIndex(rowId(contact.localId()), 0);
}

/*!
 * Return the contact manager used by this model.
 *
 * \return Reference to contact manager.
 */
QContactManager& CntListModel::contactManager() const
{
    return *d->m_contactManager;
}

/*!
 * Set new filter and sort order for the model.
 *
 * \param contactFilter New contact filter.
 * \param contactSortOrders New sort order.
 */
void CntListModel::setFilter(const QContactFilter& contactFilter)
{
    CNT_ENTRY
    
    d->m_filter = contactFilter;
    d->m_currentRow = -1;
    
    //refresh model
    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

void CntListModel::setSortOrder()
{
    XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
                         KCRUiContacts.iUid,
                         KCntNameOrdering);
    XQSettingsManager settingsMng;
    int order = settingsMng.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();

    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> sortOrders;
    if (order == CntOrderLastFirst || order == CntOrderLastCommaFirst) {
        sortOrders.append(sortOrderLastName);
        sortOrders.append(sortOrderFirstName);
    } else {
        sortOrders.append(sortOrderFirstName);
        sortOrders.append(sortOrderLastName);
    }
    
    QContactSortOrder sortOrderOrg;
    sortOrderOrg.setDetailDefinitionName(QContactOrganization::DefinitionName,
        QContactOrganization::FieldName);
    sortOrders.append(sortOrderOrg);
    
    d->m_sortOrders = sortOrders;
}

void CntListModel::refreshModel()
{
    CNT_ENTRY
    
    setSortOrder();
    d->m_cache->clearCache();
    d->m_currentRow = -1;
    
    //refresh model
    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
 * Enable/disable MyCard appearance in the model.
 *
 * \param enabled Status of MyCard appearance in the model.
 */
void CntListModel::showMyCard(bool enabled)
{
    CNT_ENTRY
    if (d->m_showMyCard == enabled) {
        return;
    }
    
    QContactLocalId myCardId = d->m_myCardId;
    if (enabled) {
        //add MyCard to the list
        if (myCardId <= 0) {
            // create a placeholder for MyCard
            d->m_contactIds.insert(0, dummyMyCardId);
        }
        else {
            d->m_contactIds.insert(0, myCardId);
        }
    }
    else {
        // remove MyCard from the list
        if (myCardId <= 0) {
            d->m_contactIds.removeOne(dummyMyCardId);
        }
        else {
            d->m_contactIds.removeOne(myCardId);
        }
    }
    d->m_showMyCard = enabled;
    d->m_currentRow = -1;

    beginResetModel();
    reset();
    endResetModel();
    CNT_EXIT
}

/*!
 * Returns MyCard status: shown or not.
 *
 * \return true if MyCard is shown, false otherwise.
 */
bool CntListModel::myCardStatus() const
{
    return d->m_showMyCard;
}

/*!
 * Returns MyCard id.
 *
 * \return MyCard id.
 */
QContactLocalId CntListModel::myCardId() const
{
    return d->m_myCardId;
}

/*!
 * Initializes model data and attaches database notifications to handlers.
 * 
 * \return Error status
 */
int CntListModel::doConstruct()
{
    CNT_ENTRY
    
    int error = initializeData();
    
    // Attach database notifications to handlers.
    connect(d->m_contactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(handleAdded(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(handleChanged(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(handleRemoved(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)), this, SLOT(handleMyCardChanged(const QContactLocalId&, const QContactLocalId&)));
    connect(d->m_contactManager, SIGNAL(dataChanged()), this, SLOT(refreshModel()));
    
    CNT_EXIT
    return error;
}

/*!
 * Initializes d->m_contactIds to contain contact Ids from the database.
 * 
 * \return Error status
 */
int CntListModel::initializeData()
{
    CNT_ENTRY
    int error(QContactManager::NoError);
    
    setSortOrder();

    // Get a list of all contact IDs in the database.
    updateContactIdsArray();

    CNT_EXIT
    return error;
}

/*!
 * Gets the filtered list of the contact Ids in a sorted order  
 * 
 * \return Error status
 */
void CntListModel::updateContactIdsArray()
{
    CNT_ENTRY
    d->m_contactIds = d->m_contactManager->contactIds(d->m_filter,
            d->m_sortOrders);
                      
    //find MyCard contact and move it to the first position
    QContactLocalId myCardId = d->m_myCardId;
    if (myCardId > 0) {
        // MyCard exists
        d->m_contactIds.removeOne(myCardId);
        if (d->m_showMyCard) {
            d->m_contactIds.insert(0, myCardId);
        }
    } 
    else if (d->m_showMyCard) {
        // create a placeholder for MyCard
        d->m_contactIds.insert(0, dummyMyCardId);
    }
    CNT_EXIT
}

/*!
 * Read a full contact entry from the database for the row number.
 *
 * The entry at the requested row will have its full contact information
 * (all fields) read from the database and returned as a QContact instance.
 *
 * \param row Row at which the sought contact entry is in this model.
 * \return A newly constructed QContact instance for this entry - ownership
 *  is transferred to the caller.
 *
 */
QContact CntListModel::contact(int row) const
{
    CNT_ENTRY
    if (!validRowId(row) || d->m_contactIds[row] == dummyMyCardId) {
        return QContact();
    }
    CNT_EXIT
    return d->m_contactManager->contact(d->m_contactIds[row]);
}

/*!
 * Verify specified row id is valid.
 *
 * \param row A row number
 * \return bool indicating validity of row id
 */
bool CntListModel::validRowId(int row) const
{
	return (row >= 0 && row < rowCount());
}

/*!
 * Fetch the id of the row containing the contact of the specified id.
 *
 * \param contactId The id of the contact
 * \return the row id of the contact or -1 if no item matched.
 */
int CntListModel::rowId(const QContactLocalId &contactId) const
{
    return d->m_contactIds.indexOf(contactId);
}

/*!
 * Return the data to be used by the view for a display role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 * \return QVariant The data for the specified index.
 */
QVariant CntListModel::dataForDisplayRole(int row) const
{
    CNT_ENTRY
    QStringList list;
    QString name;
    QString infoText;
    bool isSelfContact = false;
    bool isNonEmptySelfContact = false;
    
    QContactLocalId id = d->m_contactIds[row];
    if (d->m_myCardId == id || dummyMyCardId == id) {
        isSelfContact = true;
        if (d->m_currentContact.id() == -1) {
            // empty card
            name = hbTrId("txt_phob_dblist_mycard");
            infoText = hbTrId("txt_phob_dblist_mycard_val_create_my_identity");
        }
        else {
            isNonEmptySelfContact = true;
        }
    }

    if (!isSelfContact || isNonEmptySelfContact) {
        name = d->m_currentContact.name();
        if (name.isEmpty()) {
            name = hbTrId("txt_phob_list_unnamed");
        }
        infoText = d->m_currentContact.text();
    }
    
    list << name;
    
    if (!isNonEmptySelfContact) {
        list << infoText;
    }
    CNT_EXIT
    return list;
}

/*!
 * Handle adding of contacts. 
 *
 * \param contactIds Ids of contacts added.
 */
void CntListModel::handleAdded(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    //if contacts are added already, no need to do anything
    bool newContacts = false;
    for (int k = 0; k < contactIds.count() && !newContacts; k++) { 
        if(!d->m_contactIds.contains(contactIds.at(k))) {
            newContacts = true;
        }
    }
    if (!newContacts) {
        return;
    }

    //invalidate cached contact
    d->m_currentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->m_contactIds;
    updateContactIdsArray();
    
    //Find all new contacts (=rows)
    QList<int> newRows;
    for(int i = 0; i < d->m_contactIds.count(); i++) {
        if (!oldIdList.contains(d->m_contactIds.at(i))) {
            newRows.append(i);
        }
    }
    
    // only 1 (or no) contact is added, it can be handled by beginInsertRows()
    // and endInsertRows()
    if (newRows.count() <= 1) {
        for(int j = 0; j < newRows.count(); j++) {
            beginInsertRows(QModelIndex(), newRows.at(j), newRows.at(j));
            endInsertRows();   
        }
    }
    else {
        beginResetModel();
        reset();
        endResetModel();
    }
    CNT_EXIT
}

/*!
 * Handle changes in contacts. 
 *
 * \param contactIds Ids of contacts changed.
 */
void CntListModel::handleChanged(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    if (contactIds.count() == 0) {
        return;
    }
    
    //invalidate cached contact
    d->m_currentRow = -1; 

    int firstChangedContactPosBefore = rowId(contactIds.at(0));  
    updateContactIdsArray();
    int firstChangedContactPosAfter = rowId(contactIds.at(0)); 

    // if only one contact was updated and its position didn't change,
    // refresh the corresponding row
    if (contactIds.count() == 1 &&
        firstChangedContactPosBefore == firstChangedContactPosAfter &&
        firstChangedContactPosBefore >= 0) {
        QModelIndex top = index(firstChangedContactPosBefore);
        QModelIndex bottom = index(firstChangedContactPosBefore);
        emit dataChanged(top, bottom);
    }
    else {
        beginResetModel();
        reset();
        endResetModel();
    }
    
    CNT_EXIT
}

/*!
 * Handle removing of contacts. 
 *
 * \param contactIds Ids of contacts removed.
 */
void CntListModel::handleRemoved(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    bool removeContacts = false;
    QList<QContactLocalId> idList = d->m_contactIds;
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
            removeRows.append(rowId(contactIds.at(i)));
        }
    }
    
    // invalidate cached contact
    d->m_currentRow = -1;
        
    bool removeMyCard(false);
    int myCardRow = -1;
    if (contactIds.contains(d->m_myCardId)) {
        removeMyCard = true;
        myCardRow = rowId(d->m_myCardId);    
        d->m_myCardId = 0;
    }
    
    int removeRowsCount=removeRows.count();
    // no of rows
    for(int j = 0; j < removeRows.count(); j++) {
        if (removeMyCard 
             && removeRows.at(j) == myCardRow 
             && d->m_showMyCard) {
            QModelIndex index = createIndex(0, 0);
            emit dataChanged(index, index);
            removeRowsCount--;
        }
        else {
            beginRemoveRows(QModelIndex(), removeRows.at(j), removeRows.at(j));
            endRemoveRows();
        }
    }
    updateContactIdsArray();
    
    // check row count in new list
    // if there is a mismatch, probable late events. reset model
    QList<QContactLocalId> updatedIdList = d->m_contactIds;
    int rowsRemoved = idList.count() - updatedIdList.count();
    if (rowsRemoved != removeRowsCount) {
        beginResetModel();
        reset();
        endResetModel();
    }
    
    CNT_EXIT
}

/*!
 * Handle my card change. 
 *
 * \param oldId Id of the old MyCard.
 * \param newId Id of the new MyCard.
 */
void CntListModel::handleMyCardChanged(const QContactLocalId& /*oldId*/, const QContactLocalId& newId)
{
    CNT_ENTRY
    
    //invalidate cached contact
    d->m_currentRow = -1;
    d->m_myCardId = newId;

    updateContactIdsArray();
    
    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
* Notify views that info for a contact has become
* available or has changed.
*
* \param contactId the id of the contact
*/
void CntListModel::handleContactInfoUpdated(QContactLocalId contactId)
{
    CNT_ENTRY
    
    QModelIndex index = createIndex(rowId(contactId), 0);
    if (index.row() == d->m_currentRow) {
        d->m_currentRow = -1;
    }
    emit dataChanged(index, index);
    
    CNT_EXIT
}
