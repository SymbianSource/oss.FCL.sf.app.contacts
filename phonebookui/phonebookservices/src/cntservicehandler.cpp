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
#include "cntimageutility.h"

#include <qversitreader.h>
#include <qversitcontactimporter.h>
#include <qversitdocument.h>

#include <QPixmap>
#include <QFile>
#include <QUrl>

/* NOTE! THIS SERVICE WILL BE RENAMED TO com.nokia.services.phonebook.Services */
CntServiceHandler::CntServiceHandler(QObject *parent):
    XQServiceProvider(QLatin1String("com.nokia.services.phonebookservices.Fetch"), parent),
    mCurrentRequestIndex(0)
{
    publishAll();
    
    // Definition names supported in symbian back-end
    m_definitionNames << QContactName::DefinitionName;
    m_definitionNames << QContactNickname::DefinitionName;
    m_definitionNames << QContactPhoneNumber::DefinitionName;
    m_definitionNames << QContactEmailAddress::DefinitionName;
    m_definitionNames << QContactAddress::DefinitionName;
    m_definitionNames << QContactUrl::DefinitionName;
    m_definitionNames << QContactBirthday::DefinitionName;
    m_definitionNames << QContactOrganization::DefinitionName;
    m_definitionNames << QContactSyncTarget::DefinitionName;
    m_definitionNames << QContactNote::DefinitionName;
    m_definitionNames << QContactFamily::DefinitionName;
    m_definitionNames << QContactAvatar::DefinitionName;
    m_definitionNames << QContactAnniversary::DefinitionName;
    m_definitionNames << QContactGeoLocation::DefinitionName;
    m_definitionNames << QContactGender::DefinitionName;
    m_definitionNames << QContactOnlineAccount::DefinitionName;
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
Launch fetch service with given parameters. Fetching mode is No-select by default.
*/
void CntServiceHandler::Dofetch(const QString &title, const QString &action, const QString &filter, const QString &mode)
{
    emit launchFetchVerified(title, action, filter, mode);
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
Launch new contact editor with a given vCard filename
*/
void CntServiceHandler::editCreateNew(const QString &vCardFile)
{
    CntImageUtility imageUtility;
    QContact contact;
    QVersitReader reader;
    QFile file(vCardFile);
    file.open(QIODevice::ReadOnly);
    reader.setDevice(&file);
    
    reader.startReading();
    reader.waitForFinished();
    // Use the resulting document(s)...
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();
    if (versitDocuments.count() > 0 
        && importer.importDocuments(versitDocuments))
    {
        QList<QContact> contacts = importer.contacts();
        if (contacts.count() > 0)
            contact = contacts.first();
    }
    file.close();
    
    // Save thumbnail images
    QList<QContactThumbnail> details = contact.details<QContactThumbnail>();
    for (int i = 0;i < details.count();i++)
    {
        if (!details.at(i).thumbnail().isNull())
        {
            QString imagepath;
            QPixmap pixmap = QPixmap::fromImage(details.at(i).thumbnail());
            if (imageUtility.createImage(pixmap,imagepath))
            {
                // Update contact detail
                QContactAvatar detail=details.at(i);
                detail.setImageUrl(imagepath);
                contact.saveDetail(&detail);
            }
        }
    }
    
    removeNotSupportedDetails(contact);
    removeNotSupportedFields(contact);
    emit launchEditor(contact);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
* Remove details not supported in symbian back-end.
*/
void CntServiceHandler::removeNotSupportedDetails(QContact& contact)
{
    // Check definition map, remove details not supported
    for (int i=0; i < contact.details().count(); i++)
    {
        if (!m_definitionNames.contains(contact.details().at(i).definitionName()))
        {
            QContactDetail detail= contact.details().at(i);
            contact.removeDetail(&detail);
        }
    }
}
/*
* Remove fields not supported in symbian back-end.
*/
void CntServiceHandler::removeNotSupportedFields(QContact& contact)
{
    QContactManager cm("symbian");
    
    // Not all fields are supported in symbian back-end
    // Remove not supported fields
    for (int i=0; i < contact.details().count(); i++)
    {
        QContactDetail detail = contact.details().at(i);
        if (detail.definitionName() == QContactAddress::DefinitionName)
        {
            QContactAddress addrDetail = static_cast<QContactAddress>(detail);
            
            // Sub-types not supported
            if (detail.removeValue(QContactAddress::FieldSubTypes))
                contact.saveDetail(&detail);
        }
        else if (detail.definitionName() == QContactAnniversary::DefinitionName)
        {
            QContactAnniversary annivDetail = static_cast<QContactAnniversary>(detail);
            
            // Sub-types not supported
            if (detail.removeValue(QContactAnniversary::FieldSubType))
                contact.saveDetail(&detail);
            
            // Context not supported
            if (detail.removeValue(QContactDetail::FieldContext))
                contact.saveDetail(&detail);
        }
        else if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber phoneDetail = static_cast<QContactPhoneNumber>(detail);
            
            if (phoneDetail.subTypes().size() == 0)
            {
                // No Sub-type found, set as SubTypeMobile
                phoneDetail.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                contact.saveDetail(&phoneDetail);
            }
            else if (phoneDetail.subTypes().size() > 0 )
            {
                // Sub-types not supported
                if (phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeVoice)
                    || phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeMessagingCapable)
                    || phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeVideo))
                {
                    // Replace as SubTypeMobile
                    phoneDetail.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                    contact.saveDetail(&phoneDetail);
                }
            }
        }
        else if (detail.definitionName() == QContactUrl::DefinitionName)
        {
            QContactUrl urlDetail = static_cast<QContactUrl>(detail);
            
            QString stype=QContactUrl::SubTypeHomePage;
            if (urlDetail.subType().compare(stype) != 0)
            {
                detail.removeValue(QContactUrl::FieldSubType);
                contact.saveDetail(&detail);
            }
        }
    }
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
