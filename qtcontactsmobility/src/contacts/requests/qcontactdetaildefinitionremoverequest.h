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

#ifndef QCONTACTDETAILDEFINITIONREMOVEREQUEST_H
#define QCONTACTDETAILDEFINITIONREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

class QContactDetailDefinitionRemoveRequestPrivate;
class Q_CONTACTS_EXPORT QContactDetailDefinitionRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactDetailDefinitionRemoveRequest();
    ~QContactDetailDefinitionRemoveRequest();

    /* Selection */
    void Q_DECL_DEPRECATED setNames(const QStringList& names);  // deprecated
    QStringList Q_DECL_DEPRECATED names() const;                // deprecated
    void Q_DECL_DEPRECATED setContactType(const QString& type); // deprecated
    void setDefinitionNames(const QString& contactType, const QStringList& names);
    QStringList definitionNames() const;               // replaces ^^
    QString contactType() const;

    /* Results */
    QMap<int, QContactManager::Error> errorMap() const;

signals:
    void progress(QContactDetailDefinitionRemoveRequest* self);  // deprecated

private:
    Q_DISABLE_COPY(QContactDetailDefinitionRemoveRequest)
    friend class QContactManagerEngine;
    Q_DECLARE_PRIVATE_D(d_ptr, QContactDetailDefinitionRemoveRequest)
};

QTM_END_NAMESPACE

#endif