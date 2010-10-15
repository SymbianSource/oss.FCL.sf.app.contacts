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
    delete m_request;
    m_request = NULL;
    m_request = m_AppManager.create(XQI_EMAIL_MESSAGE_SEND, "send(QVariant)", true);
    
    if (m_request) {
        QVariantMap dataMap;
        QVariantMap emailMap;
        QStringList recipients;
        QList<QVariant> data;

        //QContactType == TypeGroup
        if (QContactType::TypeGroup == m_contact.type()) {
            QStringList emails;
            QVariant value = m_data.value("email");
            if (value.canConvert<QVariantMap>()) {
                emailMap = value.toMap();
            }
            
            if (!emailMap.isEmpty()) {
                QVariantMap::const_iterator i = emailMap.constBegin();
                while (i != emailMap.constEnd()) {
                    QString formatString;
                    QString emailAddress;
                    QString displayLabel;
                    
                    emailAddress = i.key();
                    if (i.value().canConvert<QString>())
                        displayLabel = i.value().toString();
                    
                    // Email addresses format string:
                    // Firstname Lastname <email@address.com>
                    if (!displayLabel.isEmpty())
                        formatString += displayLabel + " ";
                    formatString += "<" + emailAddress + ">";
                    recipients.append(formatString);
                 
                    ++i;
                }
                dataMap.insert(EMAIL_SEND_TO_KEY, recipients);
                data.append(dataMap);

                m_request->setArguments(data);
                m_request->send(retValue);
                emitResult(m_request->lastError(), retValue);
            }
            else {
                emitResult(GeneralError, retValue);
            }
        }
        //QContactType == TypeContact
        // If detail exists use it.
        else if (m_detail.definitionName() == QContactEmailAddress::DefinitionName) {
            const QContactEmailAddress &email = static_cast<const QContactEmailAddress &>(m_detail);

            // Email addresses format string:
            // Firstname Lastname <email@address.com>
            QString formatString;
            QString displayLabel = m_contact.displayLabel();
            if (!displayLabel.isEmpty())
                formatString += displayLabel + " ";
            formatString += "<" + email.emailAddress() + ">";
            
            recipients.append(formatString);
            dataMap.insert(EMAIL_SEND_TO_KEY, recipients);
            data.append(dataMap);

            m_request->setArguments(data);
            m_request->send(retValue);
            emitResult(m_request->lastError(), retValue);
        }
        else {
            emitResult(GeneralError, retValue);
        }
    }

    // Service not found.
    else {
        emitResult(GeneralError, retValue);
    }
}



