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

#include "cntservicehandler.h"

/* NOTE! THIS SERVICE WILL BE RENAMED TO com.nokia.services.phonebook.Services */
CntServiceHandler::CntServiceHandler(QObject *parent):
    XQServiceProvider(QLatin1String("com.nokia.services.phonebookservices.Fetch"), parent),
    mCurrentRequestIndex(0)
{
    publishAll();
}

CntServiceHandler::~CntServiceHandler()
{
}

/*
Complete the fetch and return the list selected contacts
*/
void CntServiceHandler::completeFetch( const CntServicesContactList &contactList )
{
    QVariant retValue;
    retValue.setValue(contactList);
    completeRequest(mCurrentRequestIndex, retValue);
    mCurrentRequestIndex = 0;
}

/*
Complete edit and return if contact wasn't modified (-2), was deleted (-1), saved (1) or if saving failed (0)
*/
void CntServiceHandler::completeEdit(int result)
{
    QVariant retValue(result);
    completeRequest(mCurrentRequestIndex, retValue);
    mCurrentRequestIndex = 0;
}

/*
Launch fetch service with given parameters
*/
void CntServiceHandler::fetch(const QString &title, const QString &action, const QString &filter)
{
    emit launchFetch(title, action, filter);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
Launch new contact editor with a given detail
*/
void CntServiceHandler::editCreateNew(const QString &definitionName, const QString &value)
{
    QContact contact;
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phoneNumber);
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        contact.saveDetail(&email);
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        account.setAccountUri(value);
        account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
        contact.saveDetail(&account);
    }
    
    emit launchEditor(contact);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
Launch contact selection view with a given detail (selecting detail there opens the editor and adds the new detail)
*/
void CntServiceHandler::editUpdateExisting(const QString &definitionName, const QString &value)
{
    QContactDetail detail;
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        detail = phoneNumber;
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        detail = email;
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        account.setAccountUri(value);
        account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
        detail = account;
    }
    
    emit launchContactSelection(detail);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
Launch communication launcher view for the contact with the given ID (QContactLocalId -> int)
*/
void CntServiceHandler::open(int contactId)
{
    QContactManager manager("symbian");
    QContact contact = manager.contact(contactId);

    emit launchContactCard(contact);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
Launch temporary communication launcher view with the given detail
*/
void CntServiceHandler::open(const QString &definitionName, const QString &value)
{
    QContact contact;
    QContactDetail detail;
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phoneNumber);
        detail = phoneNumber;
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        contact.saveDetail(&email);
        detail = email;
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        account.setAccountUri(value);
        account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
        contact.saveDetail(&account);
        detail = account;
    }
    
    emit launchAssignContactCard(contact, detail);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
