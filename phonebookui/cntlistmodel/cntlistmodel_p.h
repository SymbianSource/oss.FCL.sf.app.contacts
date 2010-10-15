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
* Description: Private data for CntListModel.
*
*/

#ifndef CNTLISTMODEL_P_H
#define CNTLISTMODEL_P_H

#include <QSharedData>
#include <HbIcon>
#include <qcontactmanager.h>
#include <qcontactfilter.h>
#include <qcontactid.h>

QTM_USE_NAMESPACE

class CntCache;
class CntContactInfo;
class XQSettingsManager;
class XQSettingsKey;
class CntDisplayTextFormatter;

class CntListModelData : public QSharedData
{
public:
    CntListModelData(QContactManager *manager, const QContactFilter &filter, bool showMyCard);
    ~CntListModelData();
    void setFilter(const QContactFilter &contactFilter);

public:
    QContactManager *mContactManager;           // contact manager, not owned
    CntCache *mCache;                           // cache instance, not owned

    QContactFilter mFilter;                     // filter for selecting contacts to display in the list
    QList<QContactLocalId> mContactIds;         // the IDs of the selected contacts
    QContactLocalId mGroupId;                   // the ID of the group if the filter is a group filter, otherwise 0

    mutable CntContactInfo *mCurrentContact;    // info about last requested contact, owned
    mutable int mCurrentRow;                    // row of last requested contact
    bool mShowMyCard;
	QContactLocalId mMyCardId;                  // id of the my card contact

    XQSettingsManager *mSettings;               // settings manager, owned
    XQSettingsKey *mNameListRowSettingkey;      // settings key, owned
    int mCurrentRowSetting;                     // current setting for how a list item should be displayed; name only or name + info

    CntDisplayTextFormatter *mFormat;           // text formatter used for highlighting search strings in list items, owned

    HbIcon mDefaultIcon;                        // default icon for contacts
    HbIcon mDefaultMyCardIcon;                  // default icon for my card
};

#endif
