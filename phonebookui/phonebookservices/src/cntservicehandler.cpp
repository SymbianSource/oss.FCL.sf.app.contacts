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

/* NOTE! THIS SERVICE WILL BE RENAMED TO com.nokia.services.phonebook.Services */
CntServiceHandler::CntServiceHandler(QObject *parent):
    XQServiceProvider(QLatin1String("com.nokia.services.phonebookservices.Fetch"), parent),
    mCurrentRequestIndex(0)
{
    publishAll();
    
    // Definition names supported in symbian back-end
    QString def("%1");
    m_definitionNames << def.arg(QContactName::DefinitionName);
    m_definitionNames << def.arg(QContactNickname::DefinitionName);
    m_definitionNames << def.arg(QContactPhoneNumber::DefinitionName);
    m_definitionNames << def.arg(QContactEmailAddress::DefinitionName);
    m_definitionNames << def.arg(QContactAddress::DefinitionName);
    m_definitionNames << def.arg(QContactUrl::DefinitionName);
    m_definitionNames << def.arg(QContactBirthday::DefinitionName);
    m_definitionNames << def.arg(QContactOrganization::DefinitionName);
    m_definitionNames << def.arg(QContactSyncTarget::DefinitionName);
    m_definitionNames << def.arg(QContactNote::DefinitionName);
    m_definitionNames << def.arg(QContactFamily::DefinitionName);
    m_definitionNames << def.arg(QContactAvatar::DefinitionName);
    m_definitionNames << def.arg(QContactAnniversary::DefinitionName);
    m_definitionNames << def.arg(QContactGeoLocation::DefinitionName);
    m_definitionNames << def.arg(QContactGender::DefinitionName);
    m_definitionNames << def.arg(QContactOnlineAccount::DefinitionName);
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
    if(versitDocuments.count() > 0)
        {
        contact = importer.importContacts(versitDocuments).first();
        }
    file.close();
    
    // Save photo to image file
    // image file used in phonebook to generate thumbnail
    QList<QContactAvatar> details = contact.details<QContactAvatar>();
    if (details.count() > 0)
        {
        for (int i = 0;i < details.count();i++)
            {
            if (details.at(i).subType() == QContactAvatar::SubTypeImage)
                {
                if(!details.at(i).pixmap().isNull())
                    {
                    int err;
                    QString imagepath;
                    QPixmap pixmap(details.at(i).pixmap());
                    if(imageUtility.createImage(pixmap,imagepath,err))
                        {
                        // Update contact detail
                        QContactAvatar detail=details.at(i);
                        detail.setAvatar(imagepath);
                        contact.saveDetail(&detail);
                        }
                    }
                }
            }
        }
    removeDetails(contact);
    removeFields(contact);
    emit launchEditor(contact);

    mCurrentRequestIndex = setCurrentRequestAsync();
}

/*
* Remove details not supported in symbian back-end.
*/
void CntServiceHandler::removeDetails(QContact& contact)
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
void CntServiceHandler::removeFields(QContact& contact)
{
    // Replace contact
    QContactManager cm("symbian");
    for (int i=0; i < contact.details().count(); i++)
        {
        QContactDetail detail = contact.details().at(i);
        if(detail.definitionName() == QContactBirthday::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactBirthday saveBod;
            QContactBirthday bodDetail = static_cast<QContactBirthday>(detail);
            
            if(!bodDetail.date().isNull())
                saveBod.setDate(bodDetail.date());
            
            contact.removeDetail(&bodDetail);
            contact.saveDetail(&saveBod);
            }
        else if(detail.definitionName() == QContactNickname::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactNickname saveName;
            QContactNickname nameDetail = static_cast<QContactNickname>(detail);
            
            if(!nameDetail.nickname().isEmpty())
                saveName.setNickname(nameDetail.nickname());
            
            contact.removeDetail(&nameDetail);
            contact.saveDetail(&saveName);
            }
        else if(detail.definitionName() == QContactNote::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactNote saveNote;
            QContactNote noteDetail = static_cast<QContactNote>(detail);
            
            if(!noteDetail.note().isEmpty())
                saveNote.setNote(noteDetail.note());
            
            contact.removeDetail(&noteDetail);
            contact.saveDetail(&saveNote);
            }
        else if(detail.definitionName() == QContactOrganization::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactOrganization saveOrg;
            QContactOrganization orgDetail = static_cast<QContactOrganization>(detail);
            
            if(!orgDetail.name().isEmpty())
                saveOrg.setName(orgDetail.name());
            if(!orgDetail.title().isEmpty())
                saveOrg.setTitle(orgDetail.title());
            if(!orgDetail.assistantName().isEmpty())
                saveOrg.setAssistantName(orgDetail.assistantName());
            if(!orgDetail.department().isEmpty())
                saveOrg.setDepartment(orgDetail.department());
            
            if(orgDetail.contexts().size() > 0
               && (orgDetail.contexts().contains(QContactDetail::ContextHome)
                   || orgDetail.contexts().contains(QContactDetail::ContextWork)))
	            {
	            saveOrg.setContexts(orgDetail.contexts());
	            }
            
            contact.removeDetail(&orgDetail);
            contact.saveDetail(&saveOrg);
            }
        else if(detail.definitionName() == QContactAddress::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactAddress saveAddr;
            QContactAddress addrDetail = static_cast<QContactAddress>(detail);
            
            if(!addrDetail.street().isEmpty())
               saveAddr.setStreet(addrDetail.street());
            if(!addrDetail.locality().isEmpty())
               saveAddr.setLocality(addrDetail.locality());
            if(!addrDetail.region().isEmpty())
               saveAddr.setRegion(addrDetail.region());
            if(!addrDetail.postcode().isEmpty())
               saveAddr.setPostcode(addrDetail.postcode());
            if(!addrDetail.country().isEmpty())
               saveAddr.setCountry(addrDetail.country());
            if(!addrDetail.postOfficeBox().isEmpty())
               saveAddr.setPostOfficeBox(addrDetail.postOfficeBox());
            
            if(addrDetail.contexts().size() > 0
               && (addrDetail.contexts().contains(QContactDetail::ContextHome)
                   || addrDetail.contexts().contains(QContactDetail::ContextWork)))
	            {
	            saveAddr.setContexts(addrDetail.contexts());
	            }
            
            contact.removeDetail(&addrDetail);
            contact.saveDetail(&saveAddr);
            }
        else if(detail.definitionName() == QContactAnniversary::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactAnniversary saveAnniv;
            QContactAnniversary annivDetail = static_cast<QContactAnniversary>(detail);
            
            if(!annivDetail.originalDate().isNull())
                saveAnniv.setOriginalDate(annivDetail.originalDate());
            if(!annivDetail.event().isEmpty())
                saveAnniv.setEvent(annivDetail.event());
            
            contact.removeDetail(&annivDetail);
            contact.saveDetail(&saveAnniv);
            }
        else if(detail.definitionName() == QContactGender::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactGender saveGender;
            QContactGender genderDetail = static_cast<QContactGender>(detail);
            
            if(!genderDetail.gender().isEmpty())
                saveGender.setGender(genderDetail.gender());
            
            contact.removeDetail(&genderDetail);
            contact.saveDetail(&saveGender);
            }
        else if(detail.definitionName() == QContactGeoLocation::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactGeoLocation saveGeo;
            QContactGeoLocation geoDetail = static_cast<QContactGeoLocation>(detail);

            QString latitude,longitude;
            latitude.setNum(geoDetail.latitude());
            longitude.setNum(geoDetail.longitude());
            
            if(!latitude.isEmpty())
                saveGeo.setLatitude(geoDetail.latitude());
            if(!longitude.isEmpty())
                saveGeo.setLongitude(geoDetail.longitude());
            
            if(geoDetail.contexts().size() > 0
               && (geoDetail.contexts().contains(QContactDetail::ContextHome)
                    || geoDetail.contexts().contains(QContactDetail::ContextWork)))
	            {
	            saveGeo.setContexts(geoDetail.contexts());  
	            }
            
            contact.removeDetail(&geoDetail);
            contact.saveDetail(&saveGeo);
            }
        else if(detail.definitionName() == QContactPhoneNumber::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactPhoneNumber savePhone;
            QContactPhoneNumber phoneDetail = static_cast<QContactPhoneNumber>(detail);
            
            if(!phoneDetail.number().isEmpty())
                savePhone.setNumber(phoneDetail.number());
            
            if(phoneDetail.subTypes().size() > 0)
                savePhone.setSubTypes(phoneDetail.subTypes());
            
            if(phoneDetail.contexts().size() > 0
               && (phoneDetail.contexts().contains(QContactDetail::ContextHome)
                   || phoneDetail.contexts().contains(QContactDetail::ContextWork)))
	            {
	            savePhone.setContexts(phoneDetail.contexts());
	            }
            
            contact.removeDetail(&phoneDetail);
            contact.saveDetail(&savePhone);
            }
        else if(detail.definitionName() == QContactOnlineAccount::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactOnlineAccount saveOnlineAcc;
            QContactOnlineAccount onlineAccDetail = static_cast<QContactOnlineAccount>(detail);

            if(!onlineAccDetail.accountUri().isEmpty())
                saveOnlineAcc.setAccountUri(onlineAccDetail.accountUri());
            if(!onlineAccDetail.serviceProvider().isEmpty())
                saveOnlineAcc.setServiceProvider(onlineAccDetail.serviceProvider());
            if(!onlineAccDetail.presence().isEmpty())
                saveOnlineAcc.setPresence(onlineAccDetail.presence());
            if(!onlineAccDetail.statusMessage().isEmpty())
                saveOnlineAcc.setStatusMessage(onlineAccDetail.statusMessage());
            
            if(onlineAccDetail.subTypes().size() > 0)
                saveOnlineAcc.setSubTypes(onlineAccDetail.subTypes());
            
            if(onlineAccDetail.contexts().size() > 0
               && (onlineAccDetail.contexts().contains(QContactDetail::ContextHome)
                   || onlineAccDetail.contexts().contains(QContactDetail::ContextWork)))
                {
                saveOnlineAcc.setContexts(onlineAccDetail.contexts());
                }
            
            contact.removeDetail(&onlineAccDetail);
            contact.saveDetail(&saveOnlineAcc);
            }
        else if(detail.definitionName() == QContactUrl::DefinitionName)
            {
            // Not all fields are supported in symbian back-end, replace
            QContactUrl saveUrl;
            QContactUrl urlDetail = static_cast<QContactUrl>(detail);
            
            if(!urlDetail.url().isEmpty())
                saveUrl.setUrl(urlDetail.url());
            
            QString stype=QContactUrl::SubTypeHomePage;
            if(urlDetail.subType().compare(stype) == 0)
                {
                saveUrl.setSubType(stype);
                }
            
            if(urlDetail.contexts().size() > 0
               && (urlDetail.contexts().contains(QContactDetail::ContextHome)
                   || urlDetail.contexts().contains(QContactDetail::ContextWork)))
                {
                saveUrl.setContexts(urlDetail.contexts());
                }
            
            contact.removeDetail(&urlDetail);
            contact.saveDetail(&saveUrl);
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
