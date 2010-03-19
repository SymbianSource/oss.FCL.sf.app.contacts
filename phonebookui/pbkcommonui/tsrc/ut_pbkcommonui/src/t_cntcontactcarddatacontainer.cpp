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

#include <QtTest/QtTest>
#include <QObject>
#include <QDebug>

#include "cntcontactcarddatacontainer.h"
#include "t_cntcontactcarddatacontainer.h"

#include <hbnamespace.h>

void TestCntContactCardDataContainer::initTestCase()
{
    mDataContainer = 0;
}

void TestCntContactCardDataContainer::createClass(QContact* contact)
{
    mDataContainer = new CntContactCardDataContainer(contact, this);
    QVERIFY(mDataContainer != 0);
}

void TestCntContactCardDataContainer::data()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setNumber("1234567890");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact->saveDetail(number);
    QContactNote* note = new QContactNote();
    note->setNote("note");
    contact->saveDetail(note);
    
    createClass(contact);
    
    //phone number
    QVERIFY(mDataContainer->data(-1, Qt::DisplayRole).isNull() == true);
    QVERIFY(mDataContainer->data(0, Qt::DisplayRole).isNull() == false);
    QVERIFY(mDataContainer->data(0, Qt::DecorationRole).isNull() == false);
    QVERIFY(mDataContainer->data(0, Qt::UserRole+1).isNull() == false);
    QVERIFY(mDataContainer->data(0, Qt::UserRole+2).isNull() == true);
    //separator
    QVERIFY(mDataContainer->data(mDataContainer->mSeparatorIndex, Qt::DisplayRole).isNull() == false);
    QVERIFY(mDataContainer->data(mDataContainer->mSeparatorIndex, Qt::DecorationRole).isNull() == true);
    QVERIFY(mDataContainer->data(mDataContainer->mSeparatorIndex, Qt::BackgroundRole).isNull() == true);
    //note
    QVERIFY(mDataContainer->data(3, Qt::DecorationRole).isNull() == true);
    QVERIFY(mDataContainer->data(3, Qt::DisplayRole).isNull() == false);
    
    delete contact;
    delete number;
    delete note;
}

void TestCntContactCardDataContainer::rowCount()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    createClass(contact);
    
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == mDataContainer->rowCount());
}

void TestCntContactCardDataContainer::initializeData()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    //Test #1 contact without details
    QContact* contact = new QContact();
    
    //calls also mDataContainer->initializeData()
    createClass(contact);
    
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 0);
    delete contact;
    
    delete mDataContainer;
    mDataContainer = 0;
    
    //Test #2 contact witht number + email
    contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setNumber("1234567890");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact->saveDetail(number);
    QContactEmailAddress* email = new QContactEmailAddress();
    email->setEmailAddress("email@email.email");
    contact->saveDetail(email);
        
    //calls also mDataContainer->initializeData()
    createClass(contact);
    
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 3);
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == mDataContainer->rowCount() );
    
    QVariantList values = mDataContainer->mDataPointer->mDataList.value(0);
    QVERIFY(values[0].toString().compare("call") == 0);    
    QVERIFY(values[1].toString().compare("call mobile", Qt::CaseInsensitive) == 0);
    QVERIFY(values[2].toString().compare("1234567890") == 0);
    
    QVariantList values1 = mDataContainer->mDataPointer->mDataList.value(1);
    QVERIFY(values1[0].toString().compare("message") == 0);
    QVERIFY(values1[1].toString().compare("send message", Qt::CaseInsensitive) == 0);
    QVERIFY(values1[2].toString().compare("1234567890") == 0);
    
    QVariantList values2 = mDataContainer->mDataPointer->mDataList.value(2);
    QVERIFY(values2[0].toString().compare("email") == 0);
    QVERIFY(values2[1].toString().compare("mail email", Qt::CaseInsensitive) == 0);
    QVERIFY(values2[2].toString().compare("email@email.email") == 0);
     
    delete contact;
    delete number;
    delete email;
}

void TestCntContactCardDataContainer::actionDetails()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    QContactEmailAddress* email = new QContactEmailAddress();
    email->setEmailAddress("email@email.email");
    contact->saveDetail(email);
    
    createClass(contact);
    
    QList<QContactDetail> details = mDataContainer->actionDetails("email", *contact);
    QVERIFY(details.count() == 1);
    
    details = mDataContainer->actionDetails("call", *contact);
    QVERIFY(details.count() == 0);
    
    delete contact;
    delete email;
}

void TestCntContactCardDataContainer::supportsDetail()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    QContactEmailAddress* email = new QContactEmailAddress();
    email->setEmailAddress("email@email.email");
    contact->saveDetail(email);
    
    createClass(contact);
    
    QVERIFY(mDataContainer->supportsDetail("email", *email) == true);
    QVERIFY(mDataContainer->supportsDetail("call", *email) == false);
    
    delete contact;
    delete email;   
}

void TestCntContactCardDataContainer::addSeparator()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    createClass(contact);
    
    mDataContainer->mSeparatorIndex = -1;
    mDataContainer->addSeparator(1);
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 1);
    QVERIFY(mDataContainer->mSeparatorIndex == 1);
    
    delete mDataContainer;
    mDataContainer = 0;
    
    createClass(contact);
    
    mDataContainer->mSeparatorIndex = 1;
    mDataContainer->addSeparator(-1);
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 0); 
    QVERIFY(mDataContainer->mSeparatorIndex == 1);
    
    delete contact;
}

void TestCntContactCardDataContainer::initializeDetailsData()
{
    delete mDataContainer;
    mDataContainer = 0;
    
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    QContactNote* note = new QContactNote();
    note->setNote("note");
    contact->saveDetail(note);
    QContactBirthday* day = new QContactBirthday();
    day->setDate(QDate(1,1,1));
    contact->saveDetail(day);
    QContactAddress* address = new QContactAddress();
    address->setContexts("h");
    address->setPostOfficeBox("1");
    address->setStreet("s");
    address->setPostcode("12345");
    address->setLocality("l");
    address->setRegion("r");
    address->setCountry("c");
    contact->saveDetail(address);
        
    //calls also mDataContainer->initializeDetailsData()
    createClass(contact);
    
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 4);
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == mDataContainer->rowCount() );
    
    //separator
    QVariantList values = mDataContainer->mDataPointer->mDataList.value(0);
    QVERIFY(values[0].toString().compare("Details") == 0);

    QVariantList values1 = mDataContainer->mDataPointer->mDataList.value(1);
    QVERIFY(values1[0].toString().compare("") == 0);
    QVERIFY(values1[1].toString().compare("Address (h):") == 0);
    QVERIFY(values1[2].toString().compare("1 s 12345 l r c") == 0);
    
    QVariantList values2 = mDataContainer->mDataPointer->mDataList.value(2);
    QVERIFY(values2[0].toString().compare("") == 0);
    QVERIFY(values2[1].toString().compare("Birthday:") == 0);
    QVERIFY(values2[2].toString().compare(day->date().toString("dd MMMM yyyy")) == 0);
    
    QVariantList values3 = mDataContainer->mDataPointer->mDataList.value(3);
    QVERIFY(values3[0].toString().compare("") == 0);
    QVERIFY(values3[1].toString().compare("Note:") == 0);
    QVERIFY(values3[2].toString().compare(note->note()) == 0);
         
    delete contact;
    delete note;
    delete day;
    delete address;
    
    delete mDataContainer;
    mDataContainer = 0;
    
    contact = new QContact();
    address = new QContactAddress();
    address->setCountry("sweden");
    contact->saveDetail(address);
    
    //calls also mDataContainer->initializeDetailsData()
    createClass(contact);
    
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == 2);
    QVERIFY(mDataContainer->mDataPointer->mDataList.count() == mDataContainer->rowCount());
    
    values = mDataContainer->mDataPointer->mDataList.value(1);
    QVERIFY(values[0].toString().compare("") == 0);
    QVERIFY(values[1].toString().compare("Address:") == 0);
    QVERIFY(values[2].toString().compare("sweden") == 0);
    
    delete contact;
    delete address;   
}

void TestCntContactCardDataContainer::cleanupTestCase()
{
    delete mDataContainer;
}

// EOF

