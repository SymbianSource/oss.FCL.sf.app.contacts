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

#ifndef MOBCNTMODELPRIVATE_H
#define MOBCNTMODELPRIVATE_H

#include <QSharedData>
#include <QMap>

#include <qcontactmanager.h>
#include <qcontactfilter.h>
#include <qcontactsortorder.h>
#include "cntcache.h"

QTM_USE_NAMESPACE

class CntListModelData : public QSharedData
{
public:
    CntListModelData( const QContactFilter& contactFilter = QContactFilter(),
                     const QList<QContactSortOrder>& contactSortOrders = QList<QContactSortOrder>(),
                     bool showMyCard = true) :
                         m_contactManager(0),
                         ownedContactManager(false),
                         currentRow(-1),
                         filter(contactFilter),
                         sortOrders(contactSortOrders),
                         showMyCard(showMyCard)
                      { }
    ~CntListModelData() { if (ownedContactManager) {delete m_contactManager;}}

public:
    QContactManager* m_contactManager;
    CntCache* m_cache;
    bool ownedContactManager;
    mutable CntContactInfo currentContact;
    mutable int currentRow;
	
    QList<QContactLocalId> contactIds;
    QContactFilter filter;
    QList<QContactSortOrder> sortOrders;
    bool showMyCard;
	QContactLocalId mMyCardId;
};

#endif // QCONTACTMODELPRIVATE_H

