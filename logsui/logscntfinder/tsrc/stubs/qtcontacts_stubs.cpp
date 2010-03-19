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

#include <QContactManager.h>
#include <QContactDetailFilter.h>
#include <QContactPhoneNumber.h>
#include <QSharedDataPointer>
#include <QContactName.h>
#include <QContactDetail.h>

#include <QSharedData>
#include <QContactAvatar.h>

QTM_USE_NAMESPACE

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
        const QString& /*managerName*/, const QMap<QString, QString>& /*parameters*/, 
        QObject* /*parent*/)
{

}

QContactManager::~QContactManager()
{

}

QList<QContactLocalId> QContactManager::contacts(
        const QContactFilter& filter, 
        const QList<QContactSortOrder>& /*sortOrders*/) const
{
    const QContactDetailFilter& df = 
        static_cast<const QContactDetailFilter&>( filter );
        
    int digits = df.value().toString().length();        
    QList<QContactLocalId> list;
    for( int i=0;i<10-digits;i++) {
        list.append( 1000+i );
    }
    return list;
}

QContact QContactManager::contact(const QContactLocalId& /*contactId*/) const
{
    QContact contact;
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
        const QString& /*definition*/, const QString& /*fieldName*/)
{

}


void QContactDetailFilter::setMatchFlags(QContactFilter::MatchFlags /*flags*/)
{

}
void QContactDetailFilter::setValue(const QVariant& /*value*/)
{
}

QVariant QContactDetailFilter::value() const
{
    return QVariant();
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

}

QContactDetail QContact::detail(const QString& definitionId) const
{
    if ( definitionId == QContactName::DefinitionName ){
        QContactName name;
        return name;
    }
    if ( definitionId == QContactPhoneNumber::DefinitionName ){
        QContactPhoneNumber number;
        return number;
    }
if ( definitionId == QContactAvatar::DefinitionName){
        QContactAvatar avatar;
        avatar.setSubType(QContactAvatar::SubTypeImage);
        avatar.setAvatar("c:\\data\\images\\logstest1.jpg");  
        return avatar;
    }
    QContactDetail detail;
    return detail;
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

}
QContactDetail::~QContactDetail()
{

}


QContactDetail& QContactDetail::operator=(const QContactDetail& other)
{

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
        return QString( "first" );
    } else if ( key == QContactName::FieldLast ) {
        return QString( "last" );
    } else if ( key == QContactPhoneNumber::FieldNumber ) {
        return QString( "555789987" );
    }
    else if ( key == QContactAvatar::FieldAvatar){
        return QString( "Avatar" );
    }
    
    return QString("");
}


