/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cntsimcontactlocalidfetchrequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <qcontactfetchrequest.h>

CntSimContactLocalIdFetchRequest::CntSimContactLocalIdFetchRequest(CntSymbianSimEngine *engine, QContactLocalIdFetchRequest *req)
    :CntAbstractSimRequest(engine),
     m_req(req)
{
    connect( simStore(), SIGNAL(getInfoComplete(RMobilePhoneBookStore::TMobilePhoneBookInfoV5, QContactManager::Error)),
        this, SLOT(getInfoComplete(RMobilePhoneBookStore::TMobilePhoneBookInfoV5, QContactManager::Error)), Qt::QueuedConnection );
    
    connect( simStore(), SIGNAL(readComplete(QList<QContact>, QContactManager::Error)),
        this, SLOT(readComplete(QList<QContact>, QContactManager::Error)), Qt::QueuedConnection );
}

CntSimContactLocalIdFetchRequest::~CntSimContactLocalIdFetchRequest()
{
    cancel();
}

bool CntSimContactLocalIdFetchRequest::start()
{
    QContactManager::Error error = simStore()->getInfo();
    if (error) {
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);    
        QContactManagerEngine::updateContactLocalIdFetchRequest(m_req, QList<QContactLocalId>(), error);
        return false;
    }
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::ActiveState);
    return true;    
}

bool CntSimContactLocalIdFetchRequest::cancel()
{
    if (m_req->isActive()) {
        simStore()->cancel();
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::CanceledState);
        return true;
    }
    return false;
}

void CntSimContactLocalIdFetchRequest::getInfoComplete(RMobilePhoneBookStore::TMobilePhoneBookInfoV5 info, QContactManager::Error error)
{
    if (error == QContactManager::NoError) {
        //contacts are fetched starting from index 1, all slots should be checked
        //since slots may be not filled in a sequence.
        error = simStore()->read(1, info.iTotalEntries);
    }

    if (error) {
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);    
        QContactManagerEngine::updateContactLocalIdFetchRequest(m_req, QList<QContactLocalId>(), error);
    }
}

void CntSimContactLocalIdFetchRequest::readComplete(QList<QContact> contacts, QContactManager::Error error)    
{
    // Filter & sort results
    QList<QContact> filteredAndSorted;
    for (int i=0; i<contacts.count(); i++) {
        if (QContactManagerEngine::testFilter(m_req->filter(), contacts.at(i)))
            QContactManagerEngine::addSorted(&filteredAndSorted, contacts.at(i), m_req->sorting());
    }

    // Convert to QContactLocalId-list
    QList<QContactLocalId> filteredAndSortedIds;
    for (int i=0; i<filteredAndSorted.count(); i++) {
        filteredAndSortedIds << filteredAndSorted.at(i).localId();
    }
    
    // Complete the request
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);    
    QContactManagerEngine::updateContactLocalIdFetchRequest(m_req, filteredAndSortedIds, error);
}