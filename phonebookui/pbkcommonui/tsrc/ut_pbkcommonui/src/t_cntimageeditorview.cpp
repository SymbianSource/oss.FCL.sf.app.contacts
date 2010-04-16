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

#include "t_cntimageeditorview.h"

#include <QtTest/QtTest>
#include <QObject>

#include "cntimageeditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "qthighway_stub_helper.h"

void TestCntImageEditorView::initTestCase()
{
    /*mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mImageEditorView = new CntImageEditorView(mViewManager);
    mWindow->addView(mImageEditorView);
    mWindow->setCurrentView(mImageEditorView);
    
    QVERIFY(mImageEditorView != 0);*/
}

void TestCntImageEditorView::aboutToCloseView()
{
    /*QContact *contact= new QContact();
    QContactAvatar *avatar = new QContactAvatar();
    mImageEditorView->mAvatar = avatar;
    mImageEditorView->mContact = contact;
    
    // empty image isn't saved
    mImageEditorView->aboutToCloseView();
    QVERIFY(mImageEditorView->mContact->details<QContactAvatar>().count() == 0);
    // Spouse
    avatar->setAvatar("avatar");
    mImageEditorView->aboutToCloseView();
    QVERIFY(mImageEditorView->mContact->details<QContactAvatar>().count() == 1);
    
    delete mImageEditorView;
    mImageEditorView = 0;
	
    mWindow->removeView(mImageEditorView);
    mImageEditorView = new CntImageEditorView(mViewManager);
    QVERIFY(mImageEditorView != 0);*/
}

void TestCntImageEditorView::activateView()
{
    // no avatar set
    /*CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView); // these don't matter..
    QContact contact;
    params.setSelectedContact(contact);

    mImageEditorView->activateView(params);

    QVERIFY(mImageEditorView->mAvatar != 0);
    QVERIFY(mImageEditorView->mAvatar->avatar().isEmpty());

    delete mImageEditorView;
    mImageEditorView = 0;
    mImageEditorView = new CntImageEditorView(mViewManager);
    QVERIFY(mImageEditorView != 0);

    CntViewParameters params2(CntViewParameters::namesView, CntViewParameters::collectionView); // these don't matter..
    QContact contact2;
    QContactAvatar ringtone;
    ringtone.setSubType(QContactAvatar::SubTypeAudioRingtone);
	contact2.saveDetail(&ringtone);
    QContactAvatar image;
    image.setSubType(QContactAvatar::SubTypeImage);
	image.setAvatar("dummypath");
	contact2.saveDetail(&image);
    params2.setSelectedContact(contact2);

    mImageEditorView->activateView(params2);
	
    QVERIFY(mImageEditorView->mAvatar != 0);
    QVERIFY(mImageEditorView->mAvatar->avatar() == "dummypath");*/
}

void TestCntImageEditorView::openGallery()
{
    /*QtHighwayStubHelper::reset();
    mImageEditorView->openGallery();
    QVERIFY(QtHighwayStubHelper::service() == "com.nokia.services.media");
    QVERIFY(QtHighwayStubHelper::message() == "image");
    QVERIFY(QtHighwayStubHelper::operation() == "fetch(QVariantMap,QVariant)");
    
    delete mImageEditorView;
    mImageEditorView = 0;*/
}

void TestCntImageEditorView::handleImageChange()
{
    /*mImageEditorView = new CntImageEditorView(mViewManager);
    QVERIFY(mImageEditorView != 0);
    
    QContact *contact= new QContact();
    QContactAvatar *avatar = new QContactAvatar();
    avatar->setAvatar("path");
    mImageEditorView->mAvatar = avatar;
    mImageEditorView->mContact = contact;
    
    QVariant var;
    QIcon icon;
    var.setValue(icon);
    
    mImageEditorView->handleImageChange(var);
    QVERIFY(mImageEditorView->mAvatar->avatar() == "path");
    
    QVariant var2;
    QString temp("test");
    var2.setValue(temp);
    
    mImageEditorView->handleImageChange(var2);
    QVERIFY(mImageEditorView->mAvatar->avatar() == "test");*/
}

void TestCntImageEditorView::cleanupTestCase()
{
    /*mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mImageEditorView;
    mImageEditorView = 0;*/
}

// EOF
