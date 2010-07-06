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

#include "cntmessageaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
CntMessageAction::CntMessageAction() : 
    CntAction("message")
{
}

CntMessageAction::~CntMessageAction()
{
}

CntMessageAction* CntMessageAction::clone() const
{
	 return new CntMessageAction();
}

bool CntMessageAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    if (detail.definitionName() == QContactPhoneNumber::DefinitionName 
        && !static_cast<QContactPhoneNumber>(detail).subTypes().isEmpty())
    {
        return (static_cast<QContactPhoneNumber>(detail).subTypes().first() == QContactPhoneNumber::SubTypeMobile);
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> CntMessageAction::supportedDetails(const QContact& contact) const
{
    QList<QContactDetail> details = contact.details(QContactPhoneNumber::DefinitionName);
    QList<QContactDetail> supportedDetails;
    for (int i = 0; i < details.count(); i++)
    {
        if (!static_cast<QContactPhoneNumber>(details[i]).subTypes().isEmpty() 
            && static_cast<QContactPhoneNumber>(details[i]).subTypes().first() == QContactPhoneNumber::SubTypeMobile)
        {
            supportedDetails.append(details[i]);
        }
    } 
    return supportedDetails;
}

void CntMessageAction::performAction()
{
    QString service("com.nokia.symbian.IMessageSend");
    QString type("send(QString,qint32,QString)");
    
    performNumberAction(service, type);
}



