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

#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "t_cntfavoritesmemberview.h"
#include "cntfavoritesmemberview.h"
#include "hbstubs_helper.h"

void TestCntFavoritesMemberView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mFavoritesMemberView = 0;
}

void TestCntFavoritesMemberView::init()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mFavoritesMemberView = new CntFavoritesMemberView(mViewManager, 0);
    mWindow->addView(mFavoritesMemberView);
    mWindow->setCurrentView(mFavoritesMemberView);
    
    QList<QContactLocalId> ids = mFavoritesMemberView->contactManager()->contactIds();
    mFavoritesMemberView->contactManager()->removeContacts(&ids);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mFavoritesMemberView != 0);
    QVERIFY(mFavoritesMemberView->findWidget("content") != 0);
}

void TestCntFavoritesMemberView::cleanup()
{
    QList<QContactLocalId> ids = mFavoritesMemberView->contactManager()->contactIds();
    mFavoritesMemberView->contactManager()->removeContacts(&ids);
    delete mViewManager;
    mViewManager = 0;
    mWindow->deleteLater();
}

void TestCntFavoritesMemberView::cleanupTestCase()
{    
}


void TestCntFavoritesMemberView::activateView()
{
    CntViewParameters params(CntViewParameters::FavoritesMemberView);
        
    //Create Fav grp
    QContact favoriteGroup;
    favoriteGroup.setType(QContactType::TypeGroup);
    QContactName favoriteGroupName;
    favoriteGroupName.setCustomLabel("Favorites");
    favoriteGroup.saveDetail(&favoriteGroupName);
    mFavoritesMemberView->contactManager()->saveContact(&favoriteGroup);
        
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mFavoritesMemberView->contactManager()->saveContact(&firstContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(favoriteGroup.id());
    relationship.setSecond(firstContact.id());
    
    mFavoritesMemberView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(favoriteGroup);
    
    // call activate view 
    mFavoritesMemberView->activateView(params);
    
    QVERIFY(mFavoritesMemberView->findWidget("content")->layout()->count() == 2);
}

void TestCntFavoritesMemberView::addMenuItems()
{
    HbStubHelper::reset();
    mFavoritesMemberView->addMenuItems();
    
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);
}

void TestCntFavoritesMemberView::aboutToCloseView()
{
    mFavoritesMemberView->aboutToCloseView();
    //QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}

void TestCntFavoritesMemberView::manageFavorites()
{
//    mWindow->addView(mFavoritesMemberView);
//    mWindow->setCurrentView(mFavoritesMemberView);
//    
//    // create a group
//    QContact firstGroup;
//    firstGroup.setType(QContactType::TypeGroup);
//    QContactName firstGroupName;
//    firstGroupName.setCustomLabel("Favorites");
//    firstGroup.saveDetail(&firstGroupName);
//    mFavoritesMemberView->contactManager()->saveContact(&firstGroup);
//    
//    QContact firstContact;
//    QContactName firstContactName;
//    firstContactName.setFirst("firstname");
//    firstContact.saveDetail(&firstContactName);
//    mFavoritesMemberView->contactManager()->saveContact(&firstContact);
//    
//    mFavoritesMemberView->manageFavorites();
    
    // what to verify ?
}

void TestCntFavoritesMemberView::openContact()
{
    CntViewParameters params(CntViewParameters::FavoritesMemberView);
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mFavoritesMemberView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mFavoritesMemberView->contactManager()->saveContact(&firstContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mFavoritesMemberView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(firstGroup);
    
    // call activate view 
    mFavoritesMemberView->activateView(params);
    
    QModelIndex favIndex = mFavoritesMemberView->contactModel()->index(0);
    
    mFavoritesMemberView->openContact(favIndex);
    
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
    
}

void TestCntFavoritesMemberView::editContact()
{
    CntViewParameters params(CntViewParameters::FavoritesMemberView);
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mFavoritesMemberView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mFavoritesMemberView->contactManager()->saveContact(&firstContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mFavoritesMemberView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(firstGroup);
    
    // call activate view 
    mFavoritesMemberView->activateView(params);
    
    QModelIndex favIndex = mFavoritesMemberView->contactModel()->index(0);
    
    mFavoritesMemberView->editContact(favIndex);
    
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::editView);
    
}

void TestCntFavoritesMemberView::removeFromFavorites()
{
    CntViewParameters params(CntViewParameters::FavoritesMemberView);
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mFavoritesMemberView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mFavoritesMemberView->contactManager()->saveContact(&firstContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mFavoritesMemberView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(firstGroup);
    
    // call activate view 
    mFavoritesMemberView->activateView(params);
    
    QModelIndex favIndex = mFavoritesMemberView->contactModel()->index(0);
    
    mFavoritesMemberView->removeFromFavorites(favIndex);
    
    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter filter2;
    filter2.setRelationshipType(QContactRelationship::HasMember);
    filter2.setRelatedContactRole(QContactRelationshipFilter::First); 
    filter2.setRelatedContactId(firstGroup.id());
    
    QList<QContactLocalId> contactsList = mFavoritesMemberView->contactManager()->contactIds(filter2);
    int count = contactsList.count();
    
    QVERIFY(count == 0);
}

// EOF
