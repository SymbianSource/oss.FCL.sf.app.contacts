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
#ifndef MOBCNTMODEL_H
#define MOBCNTMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <HbIcon>

#include "mobcntmodelglobal.h"
#include <qcontactmanager.h>
#include <qcontactfilter.h>
#include <qcontactsortorder.h>

QTM_USE_NAMESPACE

class MobCntModelData;
class MobCntModel;
class MobCntIconManager;

/*!
 * MobCntModel is a list model view for contacts database
 * content. It will cache contacts database entries to be
 * displayed on the screen.
 *
 * Note that that this is a prototype implementation and does
 * not yet support more advanced features, such as automatic
 * update and lazy fetching from contacts database.
 */
class MOBCNTMODEL_EXPORT MobCntModel : public QAbstractListModel
{
    Q_OBJECT
    friend class TestMobCntModel;

public:
	MobCntModel(const QContactFilter& contactFilter = QContactFilter(),
                const QList<QContactSortOrder>& contactSortOrders = QList<QContactSortOrder>(),
                bool showMyCard = true,
                QObject *parent = 0);
	MobCntModel(QContactManager* manager,
                const QContactFilter& contactFilter = QContactFilter(),
	            const QList<QContactSortOrder>& contactSortOrders = QList<QContactSortOrder>(),
	            bool showMyCard = true,
	            QObject *parent = 0);
	~MobCntModel();

public: // from QAbstractTableModel/QAbstractItemModel
	QVariant data(const QModelIndex &index, int role) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

public:
	QContact contact(const QModelIndex &index) const;
	QModelIndex indexOfContact(const QContact &contact) const;
	QContactManager& contactManager() const;
	void setFilterAndSortOrder(const QContactFilter& contactFilter = QContactFilter(),
                               const QList<QContactSortOrder>& contactSortOrders = QList<QContactSortOrder>());
	void showMyCard(bool enabled);
	bool myCardStatus() const;
	QContactLocalId myCardId() const;
	
private:
	// Construction helpers
	int doConstruct();
	int initializeData();
	void updateContactIdsArray();

	// Data manipulation
	QContact contact(int row) const;

	// Utility
	bool validRowId(int row) const;
	int rowId(const QContactLocalId &contactId) const;
	QVariant dataForDisplayRole(int row) const;
	QList< QList<int> > findIndexes(const QList<QContactLocalId>& contactIds);

public slots:
    void updateContactIcon(int index);

protected slots:
	void handleAdded(const QList<QContactLocalId>& contactIds);
	void handleChanged(const QList<QContactLocalId>& contactIds);
	void handleRemoved(const QList<QContactLocalId>& contactIds);
	void handleMyCardChanged(const QContactLocalId& oldId, const QContactLocalId& newId);

private:
    QSharedDataPointer<MobCntModelData>  d;
    MobCntIconManager                   *mIconManager;
    HbIcon                               mDefaultIcon;
    HbIcon                               mDefaultMyCardIcon;
};

#endif
