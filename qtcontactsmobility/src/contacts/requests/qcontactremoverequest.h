/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTREMOVEREQUEST_H
#define QCONTACTREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactfilter.h"

#include <QList>

QTM_BEGIN_NAMESPACE

class QContactRemoveRequestPrivate;
class Q_CONTACTS_EXPORT QContactRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactRemoveRequest();
    ~QContactRemoveRequest();

    /* Selection */
    void Q_DECL_DEPRECATED setFilter(const QContactFilter& filter); // deprecated, replaced by explicit list of contacts to remove
    QContactFilter Q_DECL_DEPRECATED filter() const;                // deprecated, replaced by explicit list of contacts to remove

    void setContactIds(const QList<QContactLocalId>& contactIds);     // replaces the above
    QList<QContactLocalId> contactIds() const;

    /* Results */
    QMap<int, QContactManager::Error> errorMap() const;

signals:
    void progress(QContactRemoveRequest* self); // deprecated in week 2, removed after transition period has elapsed.

private:
    Q_DISABLE_COPY(QContactRemoveRequest)
    friend class QContactManagerEngine;
    Q_DECLARE_PRIVATE_D(d_ptr, QContactRemoveRequest)
};

QTM_END_NAMESPACE

#endif
