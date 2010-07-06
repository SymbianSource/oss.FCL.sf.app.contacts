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
#include <xqaiwdeclplat.h>
#include <xqaiwrequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

static const QString EMAIL_SEND_TO_KEY = "to";

//Action class
CntEmailAction::CntEmailAction() : 
    CntAction("email")
{
    mRequest = m_AppManager.create(XQI_EMAIL_MESSAGE_SEND, "send(QVariant)", true);
}

CntEmailAction::~CntEmailAction()
{
    delete mRequest;
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

    if (mRequest)
    {
        QMap<QString, QVariant> map;
        QStringList recipients;
        QList<QVariant> data;

        // If detail exists use it.
        if (m_detail.definitionName() == QContactEmailAddress::DefinitionName)
        {
            const QContactEmailAddress &email = static_cast<const QContactEmailAddress &>(m_detail);

            recipients.append(email.emailAddress());
            map.insert(EMAIL_SEND_TO_KEY, recipients);
            data.append(map);

            mRequest->setArguments(data);
            mRequest->send(retValue);
            emitResult(mRequest->lastError(), retValue);
        }

        // If no detail, pick preferred.
        // Todo : Temporary fix. Remove setting preferred when selection popup feature available.
        else if (m_detail.isEmpty())
        {
            QContactDetail detail = m_contact.preferredDetail(m_actionName);
            QContactEmailAddress email;

            // If preferred is empty pick first email.
            if(detail.isEmpty())
            {
                email = m_contact.detail<QContactEmailAddress>();
            }
            else
            {
                email = static_cast<QContactEmailAddress>(detail);
            }

            recipients.append(email.emailAddress());
            map.insert(EMAIL_SEND_TO_KEY, recipients);
            data.append(map);			

            mRequest->setArguments(data);
            mRequest->send(retValue);
            emitResult(mRequest->lastError(), retValue);
        }
        else
        {
            emitResult(GeneralError, retValue);
        }
    }

    // Service not found.
    else
    {
        emitResult(GeneralError, retValue);
    }
}



