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
#include <qtcontacts.h>

#include "cntaddresseditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntaddresseditorview.h"

void TestCntAddressEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mAddressEditorView = 0;
}

void TestCntAddressEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    mWindow->addView(mAddressEditorView);
    mWindow->setCurrentView(mAddressEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mAddressEditorView != 0);
}

void TestCntAddressEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactAddress *address = new QContactAddress();
    mAddressEditorView->mDetailList.append(address);
    mAddressEditorView->mContact = contact;
    
    // empty address isn't saved
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 0);
    
    // P.O.Box
    address->setPostOfficeBox("123");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    address->setPostOfficeBox("");
    // Street
    address->setStreet("Street");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    address->setStreet("");
    // Postcode
    address->setPostcode("123");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    address->setPostcode("");
    // Locality
    address->setLocality("Locality");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    address->setLocality("");
    // Region
    address->setRegion("Region");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    address->setRegion("");
    // Country
    address->setCountry("Country");
    mAddressEditorView->aboutToCloseView();
    QVERIFY(mAddressEditorView->mContact->details<QContactAddress>().count() == 1);
    
    mWindow->removeView(mAddressEditorView);
    delete mAddressEditorView;
    mAddressEditorView = 0;
}

void TestCntAddressEditorView::initializeForm()
{
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    QContact *contact = new QContact();
   
    mAddressEditorView->mContact = contact;
    mAddressEditorView->mParamString = "";
    mAddressEditorView->initializeForm();
    
    //Check count and that the first address is expanded
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    QVERIFY(mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(0, 0, QModelIndex())));
    delete mAddressEditorView;
    mAddressEditorView = 0;
    
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    contact = new QContact();
    mAddressEditorView->mContact = contact;
    QString home = QContactDetail::ContextHome;
    mAddressEditorView->mParamString = home;
    mAddressEditorView->initializeForm();
    
    //Check count and that the second address is expanded
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    QVERIFY(mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(1, 0, QModelIndex())));
    delete mAddressEditorView;
    mAddressEditorView = 0;
    
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    contact = new QContact();
    mAddressEditorView->mContact = contact;
    QString work = QContactDetail::ContextWork;
    mAddressEditorView->mParamString = work;
    mAddressEditorView->initializeForm();
    
    //Check count and that the third address is expanded
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    QVERIFY(mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(2, 0, QModelIndex())));
    delete mAddressEditorView;
    mAddressEditorView = 0;
    
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    contact = new QContact();
    mAddressEditorView->mContact = contact;
    mAddressEditorView->mParamString = "dummy";
    mAddressEditorView->initializeForm();
    
    //Check count and none of the addresses is expanded
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    QVERIFY(!mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(0, 0, QModelIndex())));
    QVERIFY(!mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(1, 0, QModelIndex())));
    QVERIFY(!mAddressEditorView->dataForm()->isExpanded(mAddressEditorView->formModel()->index(2, 0, QModelIndex())));
    delete mAddressEditorView;
    mAddressEditorView = 0;
}

void TestCntAddressEditorView::initializeFormOneData()   

{   
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    
    QContactAddress *address = new QContactAddress();
    address->setStreet("Street");
    address->setPostcode("2090");
    address->setLocality("Locality");
    address->setRegion("Region");
    address->setCountry("Country");
    
    QContact *contact= new QContact();
    contact->saveDetail(address);
    
    mAddressEditorView->mContact = contact;
    
    mAddressEditorView->initializeForm();
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mAddressEditorView;
    mAddressEditorView = 0;
    
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    
    address = new QContactAddress();
    address->setStreet("Street");
    address->setPostcode("2090");
    address->setLocality("Locality");
    address->setRegion("Region");
    address->setCountry("Country");
    address->setContexts(QContactDetail::ContextHome);
    
    contact= new QContact();
    contact->saveDetail(address);
    
    mAddressEditorView->mContact = contact;
    
    mAddressEditorView->initializeForm();
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mAddressEditorView;
    mAddressEditorView = 0;
        
}
 
void TestCntAddressEditorView::initializeFormAllData()   

{   
    mAddressEditorView = new CntAddressEditorView(mViewManager, 0);
    
    QContactAddress *address = new QContactAddress();
    address->setStreet("Street");
    address->setPostcode("2090");
    address->setLocality("Locality");
    address->setRegion("Region");
    address->setCountry("Country");
    
    QContactAddress *otherAddress = new QContactAddress();
    otherAddress->setStreet("Street");
    otherAddress->setPostcode("2090");
    otherAddress->setLocality("Locality");
    otherAddress->setRegion("Region");
    otherAddress->setCountry("Country");
    
    QContactAddress *addressHome = new QContactAddress();
    addressHome->setStreet("Street");
    addressHome->setPostcode("2090");
    addressHome->setLocality("Locality");
    addressHome->setRegion("Region");
    addressHome->setCountry("Country");
    addressHome->setContexts(QContactDetail::ContextHome);
    
    QContactAddress *otherAddressHome = new QContactAddress();
    otherAddressHome->setStreet("Street");
    otherAddressHome->setPostcode("2090");
    otherAddressHome->setLocality("Locality");
    otherAddressHome->setRegion("Region");
    otherAddressHome->setCountry("Country");
    otherAddressHome->setContexts(QContactDetail::ContextHome);
    
    QContactAddress *addressWork = new QContactAddress();
    addressWork->setStreet("Street");
    addressWork->setPostcode("2090");
    addressWork->setLocality("Locality");
    addressWork->setRegion("Region");
    addressWork->setCountry("Country");
    addressWork->setContexts(QContactDetail::ContextWork);
    
    QContactAddress *otherAddressWork = new QContactAddress();
    otherAddressWork->setStreet("Street");
    otherAddressWork->setPostcode("2090");
    otherAddressWork->setLocality("Locality");
    otherAddressWork->setRegion("Region");
    otherAddressWork->setCountry("Country");
    otherAddressWork->setContexts(QContactDetail::ContextWork);
    
    QContact *contact= new QContact();
    
    contact->saveDetail(address);
    contact->saveDetail(otherAddress);
    contact->saveDetail(addressHome);
    contact->saveDetail(otherAddressHome);
    contact->saveDetail(addressWork);
    contact->saveDetail(otherAddressWork);
    
    mAddressEditorView->mContact = contact;
    
    mAddressEditorView->initializeForm();
    // duplicate address types (like 2x home or 2x work) are ignored and only first one of them is really used
    QVERIFY(mAddressEditorView->formModel()->rowCount(QModelIndex()) == 3);
        
}

void TestCntAddressEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mAddressEditorView;
    mAddressEditorView = 0;
}

// EOF
