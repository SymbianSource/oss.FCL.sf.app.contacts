/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mt_mobcntactions.h"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QContactAction::Status)


#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 1000; \
        QTest::qWait(10); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


void TestMobCntActions::initTestCase()
{
	//Non standard types needs to be registered before they can be used by QSignalSpy
	int error = qRegisterMetaType<QContactAction::Status>();
	
	//create manager
	m_manager = new QContactManager("symbian");
	    
}

void TestMobCntActions::cleanupTestCase()
{
    delete m_manager;
}

void TestMobCntActions::init()
{
    //delete all contacts from the database
    QList<QContactLocalId> contacts = m_manager->contacts();
    m_manager->removeContacts(&contacts);
}

void TestMobCntActions::cleanup()
{}

void TestMobCntActions::emptyContactNoActionSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QStringList actions = contact.availableActions();
    QVERIFY(actions.count() == 0);
}

void TestMobCntActions::phonenumberCallSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QStringList actions = contact.availableActions();
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("call", Qt::CaseInsensitive));
    
    //verify that the action works
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors("call", "symbian");
    QVERIFY(callActionDescriptors.count() == 1);
    QContactAction *callAction = QContactAction::action(callActionDescriptors.at(0));
    QVERIFY(callAction != 0);
    QVERIFY(callAction->supportsDetail(numberList.at(0)) == true);
    QVariantMap variantMap = callAction->metadata();
    QVERIFY(variantMap.count() == 0);
    QVERIFY(variantMap.count() == 0);
    variantMap = callAction->result();
    QSignalSpy spyCallAction(callAction, SIGNAL(progress(QContactAction::Status, const QVariantMap &)));
    callAction->invokeAction(contact, numberList.at(0));
    callAction->invokeAction(contact);
    QTRY_COMPARE(spyCallAction.count(), 2); // make sure the signal was emitted exactly one time
    //Verify that the data is correct
    //QList<QVariant> arguments = spy.takeFirst(); // take the first signal
    //QContactAction::Status status = arguments.at(0).value<QContactAction::Status>();
    //QVariantMap map = (arguments.at(1)).toMap();
    delete callAction;
}

void TestMobCntActions::phonenumberNoCallSupport()
{
    QContact contact;
    
    QContactPhoneNumber faxNumber;
    faxNumber.setNumber("555111222");
    faxNumber.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    contact.saveDetail(&faxNumber);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() == 1);
    
    QStringList subTypeList = numberList.at(0).subTypes();
    
    QVERIFY(subTypeList.count() == 1);
    QVERIFY(subTypeList.contains(QContactPhoneNumber::SubTypeFacsimile));
        
    //no actions expected
    QStringList actions = contact.availableActions();
    
    QVERIFY(actions.contains("call", Qt::CaseInsensitive)      == false);
}

void TestMobCntActions::phonenumberMessageSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QStringList actions = contact.availableActions();
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("message", Qt::CaseInsensitive));
    
    QList<QContactActionDescriptor> messageActionDescriptors = QContactAction::actionDescriptors("message", "symbian");
    QVERIFY(messageActionDescriptors.count() == 1);
    QContactAction *messageAction = QContactAction::action(messageActionDescriptors.at(0));
    QVERIFY(messageAction != 0);
    QVERIFY(messageAction->supportsDetail(numberList.at(0)) == true);
    QVariantMap variantMap = messageAction->metadata();
    QVERIFY(variantMap.count() == 0);
    variantMap = messageAction->result();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyMessageAction(messageAction, SIGNAL(progress(QContactAction::Status, const QVariantMap &)));
    messageAction->invokeAction(contact, numberList.at(0));
    messageAction->invokeAction(contact);
    QTRY_COMPARE(spyMessageAction.count(), 2); // make sure the signal was emitted exactly one time
    delete messageAction;
}

void TestMobCntActions::phonenumberNoMessageSupport()
{
    QContact contact;
    
    QContactPhoneNumber faxNumber;
    faxNumber.setNumber("555111222");
    faxNumber.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    contact.saveDetail(&faxNumber);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() == 1);
    
    QStringList subTypeList = numberList.at(0).subTypes();
    
    QVERIFY(subTypeList.count() == 1);
    QVERIFY(subTypeList.contains(QContactPhoneNumber::SubTypeFacsimile));
        
    //no actions expected
    QStringList actions = contact.availableActions();
    
    QVERIFY(actions.contains("message", Qt::CaseInsensitive)   == false);
}

void TestMobCntActions::phonenumberVideoCallSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QStringList actions = contact.availableActions();
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("videocall", Qt::CaseInsensitive));
    
    //Test Video Call action
    QList<QContactActionDescriptor> videoCallActionDescriptors = QContactAction::actionDescriptors("videocall", "symbian");
    QVERIFY(videoCallActionDescriptors.count() == 1);
    QContactAction *videoCallAction = QContactAction::action(videoCallActionDescriptors.at(0));
    QVERIFY(videoCallAction != 0);
    QVERIFY(videoCallAction->supportsDetail(numberList.at(0)) == true);
    QVariantMap variantMap = videoCallAction->metadata();
    QVERIFY(variantMap.count() == 0);
    variantMap = videoCallAction->result();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyVideoCallAction(videoCallAction, SIGNAL(progress(QContactAction::Status, const QVariantMap &)));
    videoCallAction->invokeAction(contact, numberList.at(0));
    videoCallAction->invokeAction(contact);
    QTRY_COMPARE(spyVideoCallAction.count(), 2); // make sure the signal was emitted exactly one time
    delete videoCallAction;
}

void TestMobCntActions::phonenumberNoVideoCallSupport()
{
    QContact contact;
    
    QContactPhoneNumber faxNumber;
    faxNumber.setNumber("555111222");
    faxNumber.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    contact.saveDetail(&faxNumber);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() == 1);
    
    QStringList subTypeList = numberList.at(0).subTypes();
    
    QVERIFY(subTypeList.count() == 1);
    QVERIFY(subTypeList.contains(QContactPhoneNumber::SubTypeFacsimile));
        
    //no actions expected
    QStringList actions = contact.availableActions();
    
    QVERIFY(actions.contains("videocall", Qt::CaseInsensitive) == false);
}

void TestMobCntActions::phonenumberEmailSupport()
{
    QContact contact;
    QContactEmailAddress email;
    email.setEmailAddress("test@test.com");
    contact.saveDetail(&email);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactEmailAddress> emailList = contact.details<QContactEmailAddress>();
    QVERIFY(emailList.count() == 1);
    
    //no actions expected
    QStringList actions = contact.availableActions();
    QVERIFY(actions.count() == 1);   
    QVERIFY(actions.contains("email", Qt::CaseInsensitive));
    
    //pick first number for the actions
    QContactEmailAddress emailAddress = contact.detail<QContactEmailAddress>();
        
    //Test Email action
    QList<QContactActionDescriptor> emailActionDescriptors = QContactAction::actionDescriptors("email", "symbian");
    QVERIFY(emailActionDescriptors.count() == 1);
    QContactAction *emailAction = QContactAction::action(emailActionDescriptors.at(0));
    QVERIFY(emailAction != 0);
    QVERIFY(emailAction->supportsDetail(emailList.at(0)) == true);
    QVariantMap variantMap = emailAction->metadata();
    QVERIFY(variantMap.count() == 0);
    variantMap = emailAction->result();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyEmailAction(emailAction, SIGNAL(progress(QContactAction::Status, const QVariantMap &)));
    emailAction->invokeAction(contact, emailAddress);
    emailAction->invokeAction(contact);
    QTRY_COMPARE(spyEmailAction.count(), 2); // make sure the signal was emitted exactly one time
    delete emailAction;
}

void TestMobCntActions::phonenumberNoEmailSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QStringList actions = contact.availableActions();
    QVERIFY(actions.contains("email", Qt::CaseInsensitive) == false);
}

QTEST_MAIN(TestMobCntActions);
