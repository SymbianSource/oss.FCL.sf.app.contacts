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

#include "cntemailaction.h"

#include <qcontactemailaddress.h>
#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
CntEmailAction::CntEmailAction() : 
    CntAction("email")
{
}

CntEmailAction::~CntEmailAction()
{
}

QContactFilter CntEmailAction::contactFilter(const QVariant& value) const
{
     Q_UNUSED(value);
     
    QContactDetailFilter emailFilter;
    emailFilter.setDetailDefinitionName(QContactEmailAddress::DefinitionName); 
    
    return emailFilter;
}

bool CntEmailAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactEmailAddress::DefinitionName);
}

QList<QContactDetail> CntEmailAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactEmailAddress::DefinitionName);
}

CntEmailAction* CntEmailAction::clone() const
{
	 return new CntEmailAction();
}

void CntEmailAction::performAction()
{
    QVariant retValue;
    emitResult(GeneralError, retValue);
    
    /*
    QString service("com.nokia.services.telephony");
    QString type("dial(QString,int)");
    
    performNumberAction(service, type);
    */
}


