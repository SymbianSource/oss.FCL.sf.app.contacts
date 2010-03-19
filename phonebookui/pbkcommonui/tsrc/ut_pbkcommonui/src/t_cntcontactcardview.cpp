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
#include <QGraphicsLinearLayout>
#include <qtcontacts.h>

#include "cntcontactcardview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "t_cntcontactcardview.h"


void TestCntContactCardView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mContactCardView = 0;
}

void TestCntContactCardView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mContactCardView = new CntContactCardView(mViewManager,0);
    
    //check that docml
    QVERIFY(mContactCardView->findWidget("content") != 0);
    
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mContactCardView != 0);
    QVERIFY(mContactCardView->mThumbnailManager != 0);
}

void TestCntContactCardView::activateView()
{
    delete mContactCardView;
    mContactCardView = 0;
    
    mContactCardView = new CntContactCardView(mViewManager, 0);
    
    //set view parameters and create contact
    CntViewParameters viewParameters(CntViewParameters::noView);
    QContact* contact = new QContact();
  
    viewParameters.setSelectedContact(*contact);
    mContactCardView->activateView(viewParameters);
    
    QVERIFY(mContactCardView->mContact != 0);
    QVERIFY(mContactCardView->mHeadingItem != 0);
    QVERIFY(mContactCardView->mDataContainer != 0);
    QVERIFY(mContactCardView->mScrollArea != 0);
    QVERIFY(mContactCardView->mContainerWidget != 0);
    QVERIFY(mContactCardView->mContainerLayout != 0);
     
    //heading + scroll area
    QVERIFY(mContactCardView->findWidget("content")->layout()->count() == 2);
    //no items
    QVERIFY(mContactCardView->mContainerLayout->count() == 0);
    
    
    delete contact;
    
    //----------------------------------------------------------------------------------
    
    //test #2 only name with number
    delete mContactCardView;
    mContactCardView = 0;
    
    mContactCardView = new CntContactCardView(mViewManager,0);
        
    //set view parameters and create contact 
    CntViewParameters viewParameters2(CntViewParameters::noView);
    
    QContact* contact2 = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact2->saveDetail(name);
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setNumber("123456789");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact2->saveDetail(number);
    QContactNote* note = new QContactNote();
    note->setNote("note");
    contact2->saveDetail(note);
    QContactAvatar* avatar = new QContactAvatar();
    avatar->setAvatar("no_icon.svg");
    contact2->saveDetail(avatar);
     
    viewParameters2.setSelectedContact(*contact2);
    mContactCardView->activateView(viewParameters2);
    
    QVERIFY(mContactCardView->mContact != 0);
    QVERIFY(mContactCardView->mHeadingItem != 0);
    QVERIFY(mContactCardView->mDataContainer != 0);
    QVERIFY(mContactCardView->mScrollArea != 0);
    QVERIFY(mContactCardView->mContainerWidget != 0);
    QVERIFY(mContactCardView->mContainerLayout != 0);

    //call + messaging + separator + company
    QVERIFY(mContactCardView->mContainerLayout->count() == 4);
}

void TestCntContactCardView::addActionsToToolBar()
{
    delete mContactCardView;
    mContactCardView=0;
    
    mContactCardView= new CntContactCardView(mViewManager,0);

    mContactCardView->addActionsToToolBar();
    QVERIFY(mContactCardView->actions()->actionList().count() == 3);
}


void TestCntContactCardView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mContactCardView;
    mContactCardView = 0;
}

