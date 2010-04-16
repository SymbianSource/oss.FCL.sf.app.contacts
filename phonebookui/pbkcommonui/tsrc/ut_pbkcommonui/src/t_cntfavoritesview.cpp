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

#include "cntfavoritesview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntfavoritesview.h"
#include "cntgroupselectionpopup.h"

#include "hbstubs_helper.h"

void TestCntFavoritesView::initTestCase()
{
    /*mWindow = 0;
    mViewManager = 0;
    mFavoritesView = 0;*/
}

void TestCntFavoritesView::init()
{
    /*mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mFavoritesView = new CntFavoritesView(mViewManager, 0);
    mWindow->addView(mFavoritesView);
    mWindow->setCurrentView(mFavoritesView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mFavoritesView != 0);*/
}

void TestCntFavoritesView::activateView()
{
    /*CntViewParameters params(CntViewParameters::collectionFavoritesView);
        
    //Create Fav grp
    QContact favoriteGroup;
    favoriteGroup.setType(QContactType::TypeGroup);
    QContactName favoriteGroupName;
    favoriteGroupName.setCustomLabel("Favorites");
    favoriteGroup.saveDetail(&favoriteGroupName);
    mFavoritesView->contactManager()->saveContact(&favoriteGroup);

    // set contact as the group id
    params.setSelectedContact(favoriteGroup);
    
    mFavoritesView->activateView(params);
    
    
    QVERIFY(mFavoritesView->findWidget("cnt_button_new")->isEnabled());*/
}

void TestCntFavoritesView::openNamesList()
{
//    mWindow->addView(mFavoritesView);
//    mWindow->setCurrentView(mFavoritesView);
//    
//    // create a group
//    QContact firstGroup;
//    firstGroup.setType(QContactType::TypeGroup);
//    QContactName firstGroupName;
//    firstGroupName.setCustomLabel("Favorites");
//    firstGroup.saveDetail(&firstGroupName);
//    mFavoritesView->contactManager()->saveContact(&firstGroup);
//    
//    QContact firstContact;
//    QContactName firstContactName;
//    firstContactName.setFirst("firstname");
//    firstContact.saveDetail(&firstContactName);
//    mFavoritesView->contactManager()->saveContact(&firstContact);
//    
//    mFavoritesView->openNamesList();
//    
//    // what to verify ?
    
}

void TestCntFavoritesView::aboutToCloseView()
{
    /*mWindow->addView(mFavoritesView);
    mWindow->setCurrentView(mFavoritesView);

    mFavoritesView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() ==
            CntViewParameters::collectionView);*/
}


void TestCntFavoritesView::cleanup()
{
    /*delete mFavoritesView;
    mFavoritesView = 0;
    delete mViewManager;
    mViewManager = 0;
    mWindow->deleteLater();*/
}
