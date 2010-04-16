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
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>

#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "cnteditordataviewitem.h"
#include "t_cnteditordataviewitem.h"

void TestCntEditorDataViewItem::initTestCase()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mTestView = new CntDetailEditorTestView(mViewManager, 0);
    mWindow->addView(mTestView);
    mWindow->setCurrentView(mTestView);

    mViewItem = new CntEditorDataViewItem(mTestView);
    QVERIFY(mViewItem != 0);
    QVERIFY(mViewItem->contentWidget() == 0);
}

void TestCntEditorDataViewItem::createItem()
{
    QVERIFY(mViewItem->createItem() != 0);
}

void TestCntEditorDataViewItem::addDetail()
{
    mViewItem->addDetail();
    QVERIFY(mTestView->addDetailSelected);
}

void TestCntEditorDataViewItem::createCustomWidget()
{
    HbDataForm *datas = new HbDataForm();
    datas->setItemPrototype(mViewItem);
	
    HbDataFormModel *model = new HbDataFormModel();

    //CustomItemBase
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    QStandardItemModel *fieldModel = new QStandardItemModel();
    QStandardItem *mobile = new QStandardItem;
    mobile->setText("Mobile");
    QString temp = QContactPhoneNumber::SubTypeMobile;
    mobile->setData(temp, Qt::UserRole);
    mobile->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobile);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*number, QContactPhoneNumber::FieldNumber, fieldModel, 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbPhoneNumberFilter::instance());
    model->appendDataFormItem(item, model->invisibleRootItem());
	
    //CustomItemBase +1 (note)
    QContactNote *note = new QContactNote();
    item = new CntEditorDataModelItem(*note, QContactNote::FieldNote, CNT_NOTE_EDITOR_MAXLENGTH, 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1));
    model->appendDataFormItem(item, model->invisibleRootItem());

    //CustomItemBase +1 (nickname)
    QContactNickname *nick = new QContactNickname();
    item = new CntEditorDataModelItem(*nick, QContactNickname::FieldNickname, CNT_NAMES_EDITOR_MAXLENGTH, 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1));
    model->appendDataFormItem(item, model->invisibleRootItem());

    //CustomItemBase +2
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*number, tr("Add number"), 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    model->appendDataFormItem(buttonItem, model->invisibleRootItem());
    
    //CustomItemBase +3 (empty date)
    QContactBirthday *birthday = new QContactBirthday();
    birthday->setDate(QDate());
    item = new CntEditorDataModelItem(*birthday, QContactBirthday::FieldBirthday, 0,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+3), hbTrId("txt_phob_formlabel_birthday"));
    model->appendDataFormItem(item, model->invisibleRootItem());
    
    //CustomItemBase +3 (valid date)
    QContactAnniversary *anniversary = new QContactAnniversary();
    anniversary->setOriginalDate(QDate(2000, 2, 2));
    item = new CntEditorDataModelItem(*anniversary, QContactBirthday::FieldBirthday, 0,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+3), hbTrId("txt_phob_formlabel_birthday"));
    model->appendDataFormItem(item, model->invisibleRootItem());

    //CustomItemBase DEFAULT
    buttonItem = new CntEditorDataModelItem(*number, tr("Add number"), 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+5));
    model->appendDataFormItem(buttonItem, model->invisibleRootItem());

    datas->setModel(model);
    
    delete number;
    delete note;
    delete nick;
    delete birthday;
    delete anniversary;
	
    delete model;
    delete datas;
}

void TestCntEditorDataViewItem::modelPosition()
{
    QStandardItemModel *fieldModel = new QStandardItemModel();
	
    QString mobileType = QContactPhoneNumber::SubTypeMobile;	
    QString contextHome = QContactDetail::ContextHome;

    QStandardItem *mobile = new QStandardItem;
    mobile->setText("Mobile");
    mobile->setData(mobileType, Qt::UserRole);
    mobile->setData(contextHome, Qt::UserRole+1);
    mobile->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobile);

    QContactPhoneNumber number;
	number.setSubTypes(mobileType);
    number.setContexts(contextHome);

    QVERIFY(mViewItem->modelPosition(fieldModel, number) == 0);
    
    QContactPhoneNumber number2;
    number2.setSubTypes(mobileType);

    QVERIFY(mViewItem->modelPosition(fieldModel, number2) == 1);

    QContactOnlineAccount account;
	account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);

    QVERIFY(mViewItem->modelPosition(fieldModel, account) == 1);

    QContactEmailAddress email;
    email.setContexts(contextHome);

    QVERIFY(mViewItem->modelPosition(fieldModel, email) == 1);

    QContactUrl url;

    QVERIFY(mViewItem->modelPosition(fieldModel, url) == 1);

    delete fieldModel;
}

void TestCntEditorDataViewItem::textChanged()
{
    mViewItem = 0;
    mViewItem = new CntEditorDataViewItem(mTestView);
    
    HbDataForm *datas = new HbDataForm();
    datas->setItemPrototype(mViewItem);
    
    HbDataFormModel *model = new HbDataFormModel();
    
    QContactNickname *nick = new QContactNickname();
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*nick, QContactNickname::FieldNickname, CNT_NAMES_EDITOR_MAXLENGTH, 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1));
    model->appendDataFormItem(item, model->invisibleRootItem());
    
    QContactOrganization *company = new QContactOrganization();
    CntEditorDataModelItem *itemOrg = new CntEditorDataModelItem(*company, QContactOrganization::FieldDepartment, CNT_NAMES_EDITOR_MAXLENGTH,
                static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_department"));
    model->appendDataFormItem(itemOrg, model->invisibleRootItem());
    
    QContactFamily *family = new QContactFamily();
    CntEditorDataModelItem *itemFamily = new CntEditorDataModelItem(*family, QContactFamily::FieldChildren, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_children"));
    model->appendDataFormItem(itemFamily, model->invisibleRootItem());
    
    datas->setModel(model);
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(item)))->textChanged("nickname");
    QVERIFY(static_cast<QContactNickname>(item->contactDetail()).nickname() == "nickname");
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(item)))->textChanged("     ");
    QVERIFY(static_cast<QContactNickname>(item->contactDetail()).nickname().isEmpty());
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(item)))->textChanged("  nik   ");
    QVERIFY(static_cast<QContactNickname>(item->contactDetail()).nickname() == "nik");
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(itemOrg)))->textChanged("department");
    QVERIFY(static_cast<QContactOrganization>(itemOrg->contactDetail()).department().join("") == "department");
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(itemFamily)))->textChanged("child");
    QVERIFY(static_cast<QContactFamily>(itemFamily->contactDetail()).children().join("") == "child");
    
    delete nick;
    delete company;
    delete family;
    
    delete model;
    delete datas;
}

void TestCntEditorDataViewItem::indexChanged()
{
    mViewItem = 0;
    mViewItem = new CntEditorDataViewItem(mTestView);
    
    HbDataForm *datas = new HbDataForm();
    datas->setItemPrototype(mViewItem);
    
    HbDataFormModel *model = new HbDataFormModel();
    
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    
    QStandardItemModel *fieldModel = new QStandardItemModel();
    QStandardItem *mobile = new QStandardItem;
    mobile->setText("Mobile");
    QString temp = QContactPhoneNumber::SubTypeMobile;
    mobile->setData(temp, Qt::UserRole);
    mobile->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobile);
    QStandardItem *landlinehome = new QStandardItem;
    landlinehome->setText("Landline (home)");
    QString land = QContactPhoneNumber::SubTypeLandline;
    landlinehome->setData(land, Qt::UserRole);
    QString home = QContactDetail::ContextHome;
    landlinehome->setData(home, Qt::UserRole+1);
    landlinehome->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    landlinehome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(landlinehome);
    
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*number, QContactPhoneNumber::FieldNumber, fieldModel, 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbPhoneNumberFilter::instance());
    model->appendDataFormItem(item, model->invisibleRootItem());
    
    QContactOnlineAccount *account = new QContactOnlineAccount();
    account->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    
    QStandardItemModel *onlineAccountFieldModel = new QStandardItemModel();
    QStandardItem *internet = new QStandardItem;
    internet->setText(hbTrId("Internet call"));
    QString internetSubType = QContactOnlineAccount::SubTypeSipVoip;
    internet->setData(internetSubType, Qt::UserRole);
    internet->setData(":/icons/qgn_prop_nrtyp_voip.svg", Qt::UserRole+2);
    internet->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    onlineAccountFieldModel->appendRow(internet);
    QStandardItem *sip = new QStandardItem;
    sip->setText(hbTrId("SIP"));
    QString sipSubType = QContactOnlineAccount::SubTypeSip;
    sip->setData(sipSubType, Qt::UserRole);
    sip->setData(":/icons/qgn_prop_nrtyp_sip.svg", Qt::UserRole+2);
    sip->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    onlineAccountFieldModel->appendRow(sip);
    
    CntEditorDataModelItem *accountItem = new CntEditorDataModelItem(*account, QContactOnlineAccount::FieldAccountUri, onlineAccountFieldModel, 
            HbDataFormModelItem::CustomItemBase);
    accountItem->setTextFilter(HbInputLowerCaseFilter::instance());
    model->appendDataFormItem(accountItem, model->invisibleRootItem());
    
    datas->setModel(model);
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(item)))->indexChanged(1);
    QVERIFY(static_cast<QContactPhoneNumber>(item->contactDetail()).subTypes().first() == QContactPhoneNumber::SubTypeLandline);
    QVERIFY(static_cast<QContactPhoneNumber>(item->contactDetail()).contexts().first() == QContactDetail::ContextHome);
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(item)))->indexChanged(0);
    QVERIFY(static_cast<QContactPhoneNumber>(item->contactDetail()).subTypes().first() == QContactPhoneNumber::SubTypeMobile);
    QVERIFY(!static_cast<QContactPhoneNumber>(item->contactDetail()).contexts().count());
    
    static_cast<CntEditorDataViewItem*>(datas->itemByIndex(model->indexFromItem(accountItem)))->indexChanged(1);
    QVERIFY(static_cast<QContactOnlineAccount>(accountItem->contactDetail()).subTypes().first() == QContactOnlineAccount::SubTypeSip);
    QVERIFY(!static_cast<QContactOnlineAccount>(accountItem->contactDetail()).contexts().count());
    
    delete number;
    delete account;
    
    delete model;
    delete datas;
}

void TestCntEditorDataViewItem::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
}

// EOF
