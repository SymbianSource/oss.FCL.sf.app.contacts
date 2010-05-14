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

#include "mobcntmessageaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
MobCntMessageAction::MobCntMessageAction() : 
    MobCntAction("message")
{
}

MobCntMessageAction::~MobCntMessageAction()
{
}

MobCntMessageAction* MobCntMessageAction::clone() const
{
	 return new MobCntMessageAction();
}

bool MobCntMessageAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
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

QList<QContactDetail> MobCntMessageAction::supportedDetails(const QContact& contact) const
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

void MobCntMessageAction::performAction()
{
    QString service("com.nokia.services.hbserviceprovider.conversationview");
    QString type("send(QString,qint32,QString)");
    
    performNumberAction(service, type);
}



