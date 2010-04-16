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


#include "cntcommands.h"
#include "cntviewmanager.h"

#include <xqservicerequest.h>

#include <hbmessagebox.h>
#include <hbaction.h>

CntCommands::CntCommands(CntViewManager &viewManager,
                         QContactManager* contactManager,
                         QContactManager* contactSimManager,
                         QObject *aParent):
    mViewManager(viewManager),
    mContactAction(0),
    mContactManager(contactManager),
    mContactSimManager(contactSimManager)
{
    setParent(aParent);
}

CntCommands::~CntCommands()
{
}

void CntCommands::launchAction(QContact contact, QContactDetail detail, QString action)
{
    // detail might be empty -> in that case engine uses the preferred detail for the selected action
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors(action, "symbian");
    mContactAction = QContactAction::action(callActionDescriptors.at(0));
    connect(mContactAction, SIGNAL(progress(QContactAction::Status, const QVariantMap&)),
            this, SLOT(progress(QContactAction::Status, const QVariantMap&)));
    mContactAction->invokeAction(contact, detail);
}

void CntCommands::progress(QContactAction::Status status, const QVariantMap& result)
{
    Q_UNUSED(result);
    switch(status)
    {
    case QContactAction::Finished:
    case QContactAction::FinishedWithError:
        mContactAction->deleteLater();
        mContactAction = 0;
        break;
    default:
        break;
    }
}

/*!
* Create a new contact
*/
void CntCommands::newContact()
{
    QContact contact;
    editContact(contact);
}

/*!
* Import contacts from SIM card
*/
void CntCommands::importFromSim()
{
    int copied(0);
    int failed(0);
    QList<QContactLocalId> contactIds = mContactSimManager->contactIds();
    if (contactIds.count() == 0) {
        HbMessageBox::information("Nothing to copy: SIM card is empty or not accessible.");
        return;
    }
    
    foreach(QContactLocalId id, contactIds) {
        QContact contact = mContactSimManager->contact(id);
        if (contact.localId() > 0) {
            //delete local id before saving to different storage
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(0);
            contactId->setManagerUri(QString());
            contact.setId(*contactId);

            // custom label contains name information, save it to the first name 
            QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
            if (names.count() > 0) {
                QContactName name = static_cast<QContactName>(names.at(0));
                name.setFirstName(name.customLabel());
                name.setCustomLabel(QString());
                contact.saveDetail(&name);
            }
            
            if (mContactManager->saveContact(&contact)) {
                copied++;
            }
            else {
                failed++;
            }
        }
        else {
            failed++;
        }
    }
    QString resultMessage;
    resultMessage.setNum(copied);
    resultMessage.append(" contact copied, ");
    resultMessage.append(QString().setNum(failed));
    resultMessage.append(" failed.");
    
    HbMessageBox::information(resultMessage);
}

/*!
* Edit a contact
*/
void CntCommands::editContact(QContact contact)
{
    CntViewParameters viewParameters(CntViewParameters::editView);
    //viewParameters.setPreviousViewId(mViewManager.previousViewParameters().previousViewId());
    viewParameters.setSelectedContact(contact);
    mViewManager.changeView(viewParameters);
}

/*!
* Delete a contact (confirmation query)
*/
void CntCommands::deleteContact(QContact contact)
{
    QString name = mContactManager->synthesizedDisplayLabel(contact);

    HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_delete_1").arg(name), HbMessageBox::MessageTypeQuestion);
    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        mContactManager->removeContact(contact.localId());
        emit commandExecuted("delete", contact);
    }
    delete note;
}

/*!
* Open a contact to Communication Launcher view
*/
void CntCommands::openContact(QContact contact)
{
    CntViewParameters viewParameters(CntViewParameters::commLauncherView);
    viewParameters.setSelectedContact(contact);

    mViewManager.changeView(viewParameters);
}

/*!
* open history view
*/
void CntCommands::viewHistory(QContact contact)
{
    CntViewParameters viewParameters(CntViewParameters::historyView);
    viewParameters.setSelectedContact(contact);
    mViewManager.changeView(viewParameters);
}

/*!
* Launch dialer
*/
void CntCommands::launchDialer()
{
    XQServiceRequest snd("com.nokia.services.logsservices.starter", "start(int,bool)", false);
    snd << 0; // all calls
    snd << true; // show dialpad
    snd.send();
}

