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

#ifndef CNTLISTMODEL_H
#define CNTLISTMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <cntlistmodelglobal.h>
#include <qcontactmanager.h>
#include <qcontact.h>
#include <qcontactid.h>
#include <qcontactfilter.h>

QTM_USE_NAMESPACE

class CntListModelData;
class XQSettingsKey;
class CntDisplayTextFormatter;

class CNTLISTMODEL_EXPORT CntListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CntListModel(QContactManager *manager,
                 const QContactFilter &contactFilter = QContactFilter(),
                 bool showMyCard = true,
                 QObject *parent = 0);
    ~CntListModel();

public: // from QAbstractTableModel/QAbstractItemModel
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

public:
	QContact contact(const QModelIndex &index) const;
    QContactLocalId contactId(const QModelIndex &index) const;
	QModelIndex indexOfContact(const QContact &contact) const;
	QModelIndex indexOfContactId(const QContactLocalId &contactId) const;
	void setFilter(const QContactFilter& contactFilter = QContactFilter());
	void showMyCard(bool enabled);
	bool isMyCardShown() const;
	QContactLocalId myCardId() const;

private:
    void updateContactIdsArray();
    QContact contact(int row) const;
    bool isValidRow(int row) const;
    int row(const QContactLocalId &contactId) const;
    QVariant dataForRole(int row, int role) const;
    void updateRelationships();

private slots:
    void handleAdded(const QList<QContactLocalId> &contactIds);
    void handleChanged(const QList<QContactLocalId> &contactIds);
    void handleRemoved(const QList<QContactLocalId> &contactIds);
    void handleMyCardChanged(const QContactLocalId &oldId, const QContactLocalId &newId);
    void handleContactInfoUpdated(QContactLocalId contactId);
    void handleAddedRelationship(const QList<QContactLocalId> &contactIds);
    void handleRemovedRelationship(const QList<QContactLocalId> &contactIds);
    void handleRowSettingChanged(const XQSettingsKey &key, const QVariant &value);
    void refreshModel();

private:
    QSharedDataPointer<CntListModelData> d;

    friend class TestCntListModel;
};

#endif
