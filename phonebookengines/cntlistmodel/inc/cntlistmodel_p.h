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
#include <cntuids.h>
#include "cntcache.h"

QTM_USE_NAMESPACE

class CntListModelData : public QSharedData
{
public:
    CntListModelData( const QContactFilter& contactFilter = QContactFilter(),
                     bool showMyCard = true) :
                         m_contactManager(NULL),
                         m_ownedContactManager(false),
                         m_currentRow(-1),
                         m_filter(contactFilter),
                         m_showMyCard(showMyCard)
                      { 
                      }
    ~CntListModelData() {if (m_ownedContactManager) delete m_contactManager;}

public:
    QContactManager* m_contactManager;
    CntCache* m_cache;
    bool m_ownedContactManager;
    mutable CntContactInfo m_currentContact;
    mutable int m_currentRow;
	
    QList<QContactLocalId> m_contactIds;
    QContactFilter m_filter;
    QList<QContactSortOrder> m_sortOrders;
    bool m_showMyCard;
	QContactLocalId m_myCardId;
	int nameOrder;
};

#endif // QCONTACTMODELPRIVATE_H

