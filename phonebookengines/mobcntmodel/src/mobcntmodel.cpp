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
#include "mobcntmodel_p.h"
#include "mobcntmodel.h"
#include "cntcache.h"

#include <qtcontacts.h>
#include <QSet>
#include <QTimerEvent>

#include <hbindexfeedback.h>
#include <hbframebackground.h>
#include <hbframedrawer.h>

const uint dummyMyCardId = 0;

/*!
 * Constuct a new contact model instance. The model will own its handle
 * to the default persisted store.
 */
MobCntModel::MobCntModel(const QContactFilter& contactFilter,
                         const QList<QContactSortOrder>& contactSortOrders,
                         bool showMyCard,
                         QObject *parent)
    : QAbstractListModel(parent)
{
    mDefaultIcon = HbIcon("qtg_large_avatar");
    mDefaultMyCardIcon = HbIcon("qtg_large_mycard");

    d = new MobCntModelData(contactFilter, contactSortOrders, showMyCard);
    d->m_contactManager = new QContactManager;
    d->ownedContactManager = true;
    d->m_cache = CntCache::instance();
    connect(d->m_cache, SIGNAL(contactInfoUpdated(QContactLocalId)), this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    d->mMyCardId = d->m_contactManager->selfContactId();
    if (doConstruct() != QContactManager::NoError) {
        throw("exception");
    }
}

/*!
 * Construct a new MobCntModel object using manager as the QContactManager
 * instance to communicate with the contacts database.
 *
 * \param manager A QContactManager instance to be used for
 *  communications with the contacts persistant store.
 */
MobCntModel::MobCntModel(QContactManager* manager,
                         const QContactFilter& contactFilter,
                         const QList<QContactSortOrder>& contactSortOrders,
                         bool showMyCard,
                         QObject *parent)
    : QAbstractListModel(parent)
{
    mDefaultIcon = HbIcon("qtg_large_avatar");
    mDefaultMyCardIcon = HbIcon("qtg_large_mycard");

    d = new MobCntModelData(contactFilter, contactSortOrders, showMyCard);
    d->m_contactManager = manager;
    d->m_cache = CntCache::instance();
    connect(d->m_cache, SIGNAL(contactInfoUpdated(QContactLocalId)), this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    d->mMyCardId = d->m_contactManager->selfContactId();
    if (doConstruct() != QContactManager::NoError) {
        throw("exception");
    }
}

MobCntModel::~MobCntModel()
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
QVariant MobCntModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    // check that row is ok
    if (!validRowId(row)) {
        // invalid row
        return QVariant();
    }
    
    // update current contact if needed
    if (row != d->currentRow ) {
        if (d->contactIds[row] == dummyMyCardId) {
            // row contains dummy MyCard, so create dummy CntContactInfo
            d->currentContact = CntContactInfo();
        }
        else {
            d->currentContact = d->m_cache->fetchContactInfo(row, d->contactIds);
        }
        d->currentRow = row;
    }
    
    if (role == Qt::DisplayRole) {
        return dataForDisplayRole(row);
    }
    else if (role == Hb::IndexFeedbackRole) {
        if (row == 0 && (d->mMyCardId == d->contactIds[0] || dummyMyCardId == d->contactIds[0])) {
            // do not include MyCard in index feedback
            return QVariant();
        }
        else {
            return dataForDisplayRole(row).toStringList().at(0).toUpper();
        }
    }
    else if (role == Qt::BackgroundRole) {
        if (d->mMyCardId == d->contactIds[row] || dummyMyCardId == d->contactIds[row]) {
            return HbFrameBackground("qtg_fr_list_parent_normal", HbFrameDrawer::NinePieces);
        }
    }
    else if (role == Qt::DecorationRole) {
        QList<QVariant> icons;
        HbIcon avatar = d->currentContact.icon1();
        HbIcon statusIcon = d->currentContact.icon2();
    
        if (!avatar.isNull()) {
            icons.append(avatar);
        }
        else if (d->mMyCardId == d->contactIds[row] || dummyMyCardId == d->contactIds[row]) {
            icons.append(mDefaultMyCardIcon);
        }
        else {
            icons.append(mDefaultIcon);
        }

        if (!statusIcon.isNull()) {
            icons.append(statusIcon);
        }

        return icons;
    }

    return QVariant();
}

/*!
 * Get the number of rows (contacts) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of contacts in this model.
 */
int MobCntModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->contactIds.count();
}

/*!
 * Read a full contact entry from the database for the given model
 * index value. Only the row part of the index information will be
 * read. This is just an overload of MobCntModel::contact() that
 * supports old behaviour and calls:
 *      MobCntModel::contact(int row);
 *
 * The entry at the requested row will have its full contact information
 * (all fields) read from the database and returned as a QContact instance.
 *
 * \param index Index for the sought contact entry in this model.
 * \return A newly constructed QContact instance for this entry - ownership
 *  is transferred to the caller.
 *
 */
QContact MobCntModel::contact(const QModelIndex &index) const
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
QModelIndex MobCntModel::indexOfContact(const QContact &contact) const
{
    return createIndex(rowId(contact.localId()), 0);
}

/*!
 * Return the contact manager used by this model.
 *
 * \return Reference to contact manager.
 */
QContactManager& MobCntModel::contactManager() const
{
    return *d->m_contactManager;
}

/*!
 * Set new filter and sort order for the model.
 *
 * \param contactFilter New contact filter.
 * \param contactSortOrders New sort order.
 */
void MobCntModel::setFilterAndSortOrder(const QContactFilter& contactFilter,
                           const QList<QContactSortOrder>& contactSortOrders)
{
    d->filter = contactFilter;
    // keep current sort order if new one isn't given
    if (contactSortOrders.count()) {
        d->sortOrders = contactSortOrders;
    }
    d->currentRow = -1;
    
    //refresh model
    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
}

/*!
 * Enable/disable MyCard appearance in the model.
 *
 * \param enabled Status of MyCard appearance in the model.
 */
void MobCntModel::showMyCard(bool enabled)
{
    if (d->showMyCard == enabled) {
        return;
    }
    
    QContactLocalId myCardId = d->mMyCardId;
    if (enabled) {
        //add MyCard to the list
        if (myCardId <= 0) {
            // create a placeholder for MyCard
            d->contactIds.insert(0, dummyMyCardId);
        }
        else {
            d->contactIds.insert(0, myCardId);
        }
    }
    else {
        // remove MyCard from the list
        if (myCardId <= 0) {
            d->contactIds.removeOne(dummyMyCardId);
        }
        else {
            d->contactIds.removeOne(myCardId);
        }
    }
    d->showMyCard = enabled;
    d->currentRow = -1;

    beginResetModel();
    reset();
    endResetModel();
}

/*!
 * Returns MyCard status: shown or not.
 *
 * \return true if MyCard is shown, false otherwise.
 */
bool MobCntModel::myCardStatus() const
{
    return d->showMyCard;
}

/*!
 * Returns MyCard id.
 *
 * \return MyCard id.
 */
QContactLocalId MobCntModel::myCardId() const
{
    return d->mMyCardId;
}

/*!
 * Initializes model data and attaches database notifications to handlers.
 * 
 * \return Error status
 */
int MobCntModel::doConstruct()
{
    int error = initializeData();
    
    // Attach database notifications to handlers.
    connect(d->m_contactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(handleAdded(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(handleChanged(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(handleRemoved(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)), this, SLOT(handleMyCardChanged(const QContactLocalId&, const QContactLocalId&)));

    return error;
}

/*!
 * Initializes d->contactIds to contain contact Ids from the database.
 * 
 * \return Error status
 */
int MobCntModel::initializeData()
{
    int error(QContactManager::NoError);

    // Get a list of all contact IDs in the database.
    updateContactIdsArray();

    return error;
}

/*!
 * Gets the filtered list of the contact Ids in a sorted order  
 * 
 * \return Error status
 */
void MobCntModel::updateContactIdsArray()
{
    d->contactIds = d->m_contactManager->contactIds(d->filter,
            d->sortOrders);
                      
    //find MyCard contact and move it to the first position
    QContactLocalId myCardId = d->mMyCardId;
    if (myCardId > 0) {
        // MyCard exists
        d->contactIds.removeOne(myCardId);
        if (d->showMyCard) {
            d->contactIds.insert(0, myCardId);
        }
    } 
    else if (d->showMyCard) {
        // create a placeholder for MyCard
        d->contactIds.insert(0, dummyMyCardId);
    }
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
QContact MobCntModel::contact(int row) const
{
    if (!validRowId(row) || d->contactIds[row] == dummyMyCardId) {
        return QContact();
    }

    return d->m_contactManager->contact(d->contactIds[row]);
}

/*!
 * Verify specified row id is valid.
 *
 * \param row A row number
 * \return bool indicating validity of row id
 */
bool MobCntModel::validRowId(int row) const
{
	return (row >= 0 && row < rowCount());
}

/*!
 * Fetch the id of the row containing the contact of the specified id.
 *
 * \param contactId The id of the contact
 * \return the row id of the contact or -1 if no item matched.
 */
int MobCntModel::rowId(const QContactLocalId &contactId) const
{
    return d->contactIds.indexOf(contactId);
}

/*!
 * Return the data to be used by the view for a display role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 * \return QVariant The data for the specified index.
 */
QVariant MobCntModel::dataForDisplayRole(int row) const
{
    QStringList list;
    QString name;
    QString infoText;
    bool isSelfContact = false;
    bool isNonEmptySelfContact = false;
    
    QContactLocalId id = d->contactIds[row];
    if (d->mMyCardId == id || dummyMyCardId == id) {
        isSelfContact = true;
        if (d->currentContact.id() == -1) {
            // empty card
            name = hbTrId("txt_phob_dblist_mycard");
            infoText = hbTrId("txt_phob_dblist_mycard_val_create_my_identity");
        }
        else {
            isNonEmptySelfContact = true;
        }
    }

    if (!isSelfContact || isNonEmptySelfContact) {
        name = d->currentContact.name();
        if (name.isEmpty()) {
            name = hbTrId("txt_phob_list_unnamed");
        }
        infoText = d->currentContact.text();
    }
    
    list << name;
    
    if (!isNonEmptySelfContact) {
        list << infoText;
    }
    
    return list;
}

/*!
 * Handle adding of contacts. 
 *
 * \param contactIds Ids of contacts added.
 */
void MobCntModel::handleAdded(const QList<QContactLocalId>& contactIds)
{
    //if contacts are added already, no need to do anything
    bool newContacts = false;
    for (int k = 0; k < contactIds.count() && !newContacts; k++) {
        if(!d->contactIds.contains(contactIds.at(k))) {
            newContacts = true;
        }
    }
    if (!newContacts) {
        return;
    }

    //invalidate cached contact
    d->currentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->contactIds;
    updateContactIdsArray();
    
    //Find all new contacts (=rows)
    QList<int> newRows;
    for(int i = 0; i < d->contactIds.count(); i++) {
        if (!oldIdList.contains(d->contactIds.at(i))) {
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
}

/*!
 * Handle changes in contacts. 
 *
 * \param contactIds Ids of contacts changed.
 */
void MobCntModel::handleChanged(const QList<QContactLocalId>& contactIds)
{
    //invalidate cached contact
    d->currentRow = -1; 

    updateContactIdsArray();
    QList< QList<int> > indexSequences = findIndexes(contactIds);

    for (int i = (indexSequences.count() - 1);i >= 0; i--)
    {
        QModelIndex top = index(indexSequences.at(i).first());
        QModelIndex bottom = index(indexSequences.at(i).last());
        emit dataChanged(top, bottom);
    }
}

/*!
 * Handle removing of contacts. 
 *
 * \param contactIds Ids of contacts removed.
 */
void MobCntModel::handleRemoved(const QList<QContactLocalId>& contactIds)
{
    bool removeContacts = false;
    QList<QContactLocalId> idList = d->contactIds;
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
    d->currentRow = -1;
        
    bool removeMyCard(false);
    int myCardRow;
    if (contactIds.contains(d->mMyCardId)) {
        removeMyCard = true;
        myCardRow = rowId(d->mMyCardId);    
        d->mMyCardId = 0;
    }
    
    int removeRowsCount=removeRows.count();
    // no of rows
    for(int j = 0; j < removeRows.count(); j++) {
        if (removeMyCard 
             && removeRows.at(j) == myCardRow 
             && d->showMyCard) {
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
    QList<QContactLocalId> updatedIdList = d->contactIds;
    int rowsRemoved = idList.count() - updatedIdList.count();
    if (rowsRemoved != removeRowsCount) {
        beginResetModel();
        reset();
        endResetModel();
    }
}

/*!
 * Handle my card change. 
 *
 * \param oldId Id of the old MyCard.
 * \param newId Id of the new MyCard.
 */
void MobCntModel::handleMyCardChanged(const QContactLocalId& oldId, const QContactLocalId& newId)
{
    int row = rowId(newId);

    //invalidate cached contact
    d->currentRow = -1;
    d->mMyCardId = newId;

    // if the new mycard was taken from an existing contact,
    // notify views that that row was removed
    if (row > 0) {
        beginRemoveRows(QModelIndex(), row, row);
        endRemoveRows();
    }
    updateContactIdsArray();
    
    // if old mycard is not 0, it means it probably appears in the list
    if (oldId > 0 && rowId(oldId) > 0) {
        beginInsertRows(QModelIndex(), rowId(oldId), rowId(oldId));
        endInsertRows();    
    }
    
    // notify views that mycard was updated
    QModelIndex index = createIndex(rowId(newId), 0);
    emit dataChanged(index, index);
}

/*!
 * Find the sequences of indices for the given contactIds from the private list
 * 
 * \param contactIds List of ids
 * \return Sequence of indices
 */
QList< QList<int> > MobCntModel::findIndexes(const QList<QContactLocalId>& contactIds)
{
    QList< QList<int> > sequences;
    QList<int> currSequence;
    int prevIndex = rowId(contactIds.at(0)) - 1;
    
    for (int i = 0; i < contactIds.count(); i++)
    {
        int currIndex = rowId(contactIds.at(i));
        
        if (currIndex >= 0)
        {
            if (prevIndex+1 != currIndex && !currSequence.isEmpty())
            {
                sequences.append(currSequence);
                currSequence.clear();
            } 
            currSequence.append(currIndex);
            prevIndex = currIndex;
        }
    }
    
    if (!currSequence.isEmpty())
    {
        // Add last sequence if such exist
        sequences.append(currSequence);
    }
    
    return sequences;
}

/*!
* Notify views that info for a contact has become
* available or has changed.
*
* \param contactId the id of the contact
*/
void MobCntModel::handleContactInfoUpdated(QContactLocalId contactId)
{
    QModelIndex index = createIndex(rowId(contactId), 0);
    if (index.row() == d->currentRow) {
        d->currentRow = -1;
    }
    emit dataChanged(index, index);
}
