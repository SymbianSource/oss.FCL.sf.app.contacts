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

#include "cntservices.h"
#include "cntglobal.h"
#include "cntdebug.h"
#include <cntabstractviewmanager.h>
#include "cntimageutility.h"
#include "cntserviceviewparams.h"

#include <hbview.h>
#include <hblistview.h>
#include <qversitreader.h>
#include <qversitcontactimporter.h>
#include <qversitdocument.h>
#include <xqserviceprovider.h>

#include <QApplication>
#include <QPixmap>
#include <QFile>
#include <QUrl>

CntServices::CntServices() :
mViewManager(NULL),
mCurrentProvider(NULL),
mIsQuitable(true)
{
    CNT_ENTRY

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
    
    CNT_EXIT
}


CntServices::~CntServices()
{
    CNT_LOG
}


void CntServices::setViewManager( CntAbstractViewManager& aViewManager )
{
    CNT_LOG
    mViewManager = &aViewManager;
}


void CntServices::singleFetch(
    const QString &title, const QString &action,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntViewParameters params;
    params.insert(EViewId, serviceContactFetchView);
    params.insert(ESelectedAction, action);
    params.insert(KCntServiceViewParamTitle, title);
    // SINGLE fetch
    params.insert(ESelectionMode, HbAbstractItemView::SingleSelection);

    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::multiFetch(
    const QString &title, const QString &action,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntViewParameters params;
    params.insert(EViewId, serviceContactFetchView);
    params.insert(ESelectedAction, action);
    params.insert(KCntServiceViewParamTitle, title);
    // MULTI fetch
    params.insert(ESelectionMode, HbAbstractItemView::MultiSelection);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::editCreateNew(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider)
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

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

    // Launch editor now
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::editCreateNewFromVCard(const QString &vCardFile,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntImageUtility imageUtility;
    QContact contact;
    QVersitReader reader;
    QFile file(vCardFile);
    if (!file.open(QIODevice::ReadOnly))
        return;
    reader.setDevice(&file);
    
    reader.startReading();
    reader.waitForFinished();
    // Use the resulting document(s)...
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();
    if(versitDocuments.count() > 0 
       && importer.importDocuments(versitDocuments))
        {
        QList<QContact> contacts = importer.contacts();
        if(contacts.count() > 0)
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
            if(imageUtility.createImage(pixmap,imagepath))
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

    // Launch editor now
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::editUpdateExisting(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

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

    CntViewParameters params;
    params.insert(EViewId, serviceContactSelectionView);
    QVariant var;
    var.setValue(detail);
    params.insert(ESelectedDetail, var);
    mViewManager->changeView(params);
    CNT_EXIT
}

void CntServices::editExisting(int contactId,
CntAbstractServiceProvider& aServiceProvider)
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    // Give parameters for launching editor.
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);

    QContact contact = mViewManager->contactManager(SYMBIAN_BACKEND)->contact(contactId);
    QVariant varContact;
    varContact.setValue(contact);
    params.insert(ESelectedContact, varContact);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::launchContactCard(int aContactId,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContactManager manager("symbian");
    QContact contact = manager.contact(aContactId);

    // Launch Contact Card view
    CntViewParameters params;
    params.insert(EViewId, serviceContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView( params );
    CNT_EXIT
}


void CntServices::launchTemporaryContactCard(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

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

    // Launch the view
    CntViewParameters params;
    params.insert(EViewId, serviceAssignContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    QVariant varDetail;
    varDetail.setValue(detail);
    params.insert(ESelectedDetail, varDetail);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::removeNotSupportedDetails(QContact& contact)
{
    CNT_ENTRY
    // Check definition map, remove details not supported
    for (int i=0; i < contact.details().count(); i++)
    {
        if (!m_definitionNames.contains(contact.details().at(i).definitionName()))
        {
            QContactDetail detail= contact.details().at(i);
            contact.removeDetail(&detail);
        }
    }
    CNT_EXIT
}



void CntServices::removeNotSupportedFields(QContact& contact)
{
    CNT_ENTRY
    QContactManager cm("symbian");
    QList<QContactDetail>   removeList;
    
    // Not all fields are supported in symbian back-end
    // Remove not supported fields
    for (int i=0; i < contact.details().count(); i++)
    {
        QContactDetail detail = contact.details().at(i);
        if(detail.definitionName() == QContactAddress::DefinitionName)
        {
            QContactAddress addrDetail = static_cast<QContactAddress>(detail);
            
            // Sub-types not supported
            if(detail.removeValue(QContactAddress::FieldSubTypes))
                contact.saveDetail(&detail);
        }
        else if(detail.definitionName() == QContactAnniversary::DefinitionName)
        {
            QContactAnniversary annivDetail = static_cast<QContactAnniversary>(detail);
            
            // Sub-types not supported
            if(detail.removeValue(QContactAnniversary::FieldSubType))
                contact.saveDetail(&detail);
            
            // Context not supported
            if(detail.removeValue(QContactDetail::FieldContext))
                contact.saveDetail(&detail);
        }
        else if(detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber phoneDetail = static_cast<QContactPhoneNumber>(detail);
            
            if(phoneDetail.subTypes().size() > 0 )
            {
                // Sub-types not supported
                if(phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeVoice)
                   || phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeMessagingCapable))
                {
                    detail.removeValue(QContactPhoneNumber::FieldSubTypes);
                    contact.saveDetail(&detail);
                }
            }
        }
        else if(detail.definitionName() == QContactUrl::DefinitionName)
        {
            QContactUrl urlDetail = static_cast<QContactUrl>(detail);
            
            QString stype=QContactUrl::SubTypeHomePage;
            if(urlDetail.subType().compare(stype) != 0)
            {
                detail.removeValue(QContactUrl::FieldSubType);
                contact.saveDetail(&detail);
            }
        }
    }
    CNT_EXIT
}

// This method is inherited from CntAbstractServiceProvider
void CntServices::CompleteServiceAndCloseApp(const QVariant& retValue)
{
    CNT_ENTRY
    if (  mCurrentProvider )
        {
        mCurrentProvider->CompleteServiceAndCloseApp( retValue );
        }
    CNT_EXIT
}

void CntServices::setQuitable(bool quitable)
{
    mIsQuitable = quitable;
}

void CntServices::quitApp()
{
    CNT_ENTRY
   
    // Only embedded applications should be exited once a client
    // disconnects. At the moments QtHighWay has unresolved issues
    // when closing non-embedded applications. Error ou1cimx1#472852
    // has more info
    if ( mIsQuitable ) 
    {
       qApp->quit();
    }
   
    CNT_EXIT
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
