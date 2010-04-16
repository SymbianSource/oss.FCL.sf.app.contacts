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

#include <qcontactmanager.h>
#include <qcontactdetailfilter.h>
#include <qcontactphonenumber.h>
#include <QSharedDataPointer>
#include <qcontactname.h>
#include <qcontactdetail.h>
#include <QSharedData>
#include <qcontactavatar.h>
#include "qtcontacts_stubs_helper.h"

QTM_USE_NAMESPACE

QString logsTestNumber = "";
QContactLocalId logsTestContactId = 0;
QContactLocalId logsTestContactLocalId = 0;
QString logsTestAvatar = "";
QString logsFirstName = "";
QString logsLastName = "";
Q_DECLARE_METATYPE(QContactAvatar *)

void QtContactsStubsHelper::reset()
{
    logsTestNumber = "";
    logsTestContactId = 0;
    logsTestContactLocalId = 0;
    logsFirstName = "";
    logsLastName = "";
}
        
void QtContactsStubsHelper::setContactId(int id)
{
    logsTestContactId = id;
}

void QtContactsStubsHelper::setContactNames(const QString& first, const QString& last)
{
    logsFirstName = first;
    logsLastName = last;
}

// ----------------------------------------------------------------------------
// QContactData
// ----------------------------------------------------------------------------
//
      
class QtMobility::QContactData : public QSharedData
{
public:
    QContactData(): QSharedData()
    {
    }

    ~QContactData() {}
};

class QtMobility::QContactDetailPrivate : public QSharedData
{
public:
    QContactDetailPrivate(): QSharedData()
    {
    }

    ~QContactDetailPrivate() {}
};

// ----------------------------------------------------------------------------
// QContactManager
// ----------------------------------------------------------------------------
//
QContactManager::QContactManager(
        const QString& managerName, const QMap<QString, QString>& parameters, QObject* parent)
{
    Q_UNUSED(managerName)
    Q_UNUSED(parameters)
    Q_UNUSED(parent)
}

QContactManager::~QContactManager()
{

}

QList<QContactLocalId> QContactManager::contacts(
        const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrders)
    QList<QContactLocalId> list;
    if ( QString("11112222").endsWith(logsTestNumber) ){
        list.append( logsTestContactId );
    }
    return list;
}

QList<QContactLocalId> QContactManager::contactIds(
        const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrders)
    QList<QContactLocalId> list;
    if ( QString("11112222").endsWith(logsTestNumber) ){
        list.append( logsTestContactId );
    }
    return list;
}

QContact QContactManager::contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions) const
{
		Q_UNUSED(definitionRestrictions)
    QContact contact;
    if ( contactId == logsTestContactId ) {
        logsTestContactLocalId = logsTestContactId;
    }
    return contact;
}


// ----------------------------------------------------------------------------
// QContactDetailFilter
// ----------------------------------------------------------------------------
//
QContactDetailFilter::QContactDetailFilter()
{

}
  
void QContactDetailFilter::setDetailDefinitionName(
        const QString& definition, const QString& fieldName)
{

}
void QContactDetailFilter::setMatchFlags(QContactFilter::MatchFlags flags)
{
    Q_UNUSED(flags)
}
void QContactDetailFilter::setValue(const QVariant& value)
{
    logsTestNumber = value.toString();
}

// ----------------------------------------------------------------------------
// QContact
// ----------------------------------------------------------------------------
//
QContact::QContact() : d(new QContactData)
{

}

QContact::~QContact()
{

}


QContact& QContact::operator=(const QContact& other)
{
    Q_UNUSED(other)
}

QContactDetail QContact::detail(const QString& definitionId) const
{
    if ( definitionId == QContactName::DefinitionName ){
        QContactName name;
        return name;
    } else if ( definitionId == QContactPhoneNumber::DefinitionName ){
        QContactPhoneNumber number;
        return number;
    }
    else if ( definitionId == QContactAvatar::DefinitionName){
        QContactAvatar avatar;
        avatar.setSubType(QContactAvatar::SubTypeImage);
        avatar.setAvatar("c:\\data\\images\\logstest1.jpg");  
        return avatar;
    }
    QContactDetail detail;
    return detail;
}

bool QContact::saveDetail(QContactDetail* detail)
    {
      return true;
    }
 
bool QContact::removeDetail(QContactDetail* detail)
    {
     return true;
    }


QContactLocalId QContact::localId() const
{
    return logsTestContactLocalId;
}


// ----------------------------------------------------------------------------
// QContactDetail
// ----------------------------------------------------------------------------
//
QContactDetail::QContactDetail()
{

}
QContactDetail::QContactDetail(const QString& definitionName) : d(new QContactDetailPrivate)
{
    Q_UNUSED(definitionName)
}
QContactDetail::~QContactDetail()
{

}


QContactDetail& QContactDetail::operator=(const QContactDetail& other)
{
    Q_UNUSED(other)
}

QString QContactDetail::definitionName() const
{
    return QString("");
}

bool QContactDetail::isEmpty() const
{
    return false;
}


QVariant QContactDetail::variantValue(const QString& key) const
{
    QString val = value(key);
    return val;
}

QString QContactDetail::value(const QString& key) const
{
    if ( key == QContactName::FieldFirst ){
        return logsFirstName;
    } else if ( key == QContactName::FieldLast ) {
        return logsLastName;
    } else if ( key == QContactPhoneNumber::FieldNumber ) {
        return QString( "12345" );
    }
    else if ( key == QContactAvatar::FieldAvatar){
        return QString( "Avatar" );
    }
    return QString("");
}
