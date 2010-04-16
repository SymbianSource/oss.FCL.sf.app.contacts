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
#include "cntdefaultviewmanager.h"
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
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
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

void TestCntContactCardView::addMenuItems()
{
    delete mContactCardView;
    mContactCardView=0;
    
    mContactCardView= new CntContactCardView(mViewManager,0);

    mContactCardView->addMenuItems();
    QVERIFY(mContactCardView->actions()->actionList().count() == 6);
}

void TestCntContactCardView::setAsFavorite()
{
int favoriteGroupId =-1;
    delete mContactCardView;
    mContactCardView=0;
    
    mContactCardView= new CntContactCardView(mViewManager,0);
    
    QList<QContactLocalId> ids = mContactCardView->contactManager()->contactIds();
    mContactCardView->contactManager()->removeContacts(&ids);
    
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    mContactCardView->contactManager()->saveContact(contact);
    int contactid = contact->localId();
    
    //set view parameters and create contact 
    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters.setSelectedContact(*contact);
    mContactCardView->activateView(viewParameters);
    mContactCardView->setAsFavorite();
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mContactCardView->contactManager()->contactIds(groupFilter);
    QContact favcontact;
    if (!groupContactIds.isEmpty())
        {
            for(int i = 0;i < groupContactIds.count();i++)
            {
                favcontact = mContactCardView->contactManager()->contact(groupContactIds.at(i));
                QContactName contactName = favcontact.detail<QContactName>();
                QString groupName = contactName.customLabel();
                if(groupName.compare("Favorites") == 0)
                {
                favoriteGroupId = groupContactIds.at(i);
                 break;
                }
            }
        }
    
    int favcontactid11 = favcontact.localId();
    
    QContactRelationshipFilter filter;                   
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRelatedContactId(favcontact.id());                
    filter.setRelatedContactRole(QContactRelationshipFilter::First);
    
    QList<QContactLocalId> mContactsList = mContactCardView->contactManager()->contacts(filter);
    int count = mContactsList.count();
    QVERIFY(count == 1);
}

void TestCntContactCardView::removeFromFavorite()
{
    int favoriteGroupId =-1;
    delete mContactCardView;
    mContactCardView=0;
    
    mContactCardView= new CntContactCardView(mViewManager,0);
    QList<QContactLocalId> ids = mContactCardView->contactManager()->contactIds();
    mContactCardView->contactManager()->removeContacts(&ids);
    
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mContactCardView->contactManager()->saveContact(&firstGroup);
    
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    mContactCardView->contactManager()->saveContact(contact);
    int contactid = contact->localId();
    
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(contact->id());
    
    // save relationship
    mContactCardView->contactManager()->saveRelationship(&relationship);
    
    QContactRelationshipFilter filter;                   
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRelatedContactId(firstGroup.id());                
    filter.setRelatedContactRole(QContactRelationshipFilter::First);
    
    QList<QContactLocalId> mContactsList = mContactCardView->contactManager()->contacts(filter);
    int count = mContactsList.count();
    QVERIFY(count == 1);
    
    //set view parameters and create contact 
    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters.setSelectedContact(*contact);
    mContactCardView->activateView(viewParameters);
    mContactCardView->removeFromFavorite();
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mContactCardView->contactManager()->contactIds(groupFilter);
    QContact favcontact;
    
    QContactRelationshipFilter filter1;                   
    filter1.setRelationshipType(QContactRelationship::HasMember);
    filter1.setRelatedContactId(firstGroup.id());                
    filter1.setRelatedContactRole(QContactRelationshipFilter::First);
    
    QList<QContactLocalId> mContactsList1 = mContactCardView->contactManager()->contacts(filter1);
    int count1 = mContactsList1.count();
    QVERIFY(count1 == 0);
}

void TestCntContactCardView::isFavoriteGroupCreated()
    {
    delete mContactCardView;
    mContactCardView=0;
    
    mContactCardView= new CntContactCardView(mViewManager,0);
    QList<QContactLocalId> ids = mContactCardView->contactManager()->contactIds();
    mContactCardView->contactManager()->removeContacts(&ids);
    
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    mContactCardView->contactManager()->saveContact(contact);
    int contactid = contact->localId();
   
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mContactCardView->contactManager()->saveContact(&firstGroup);
    
    //set view parameters and create contact 
    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters.setSelectedContact(*contact);
    mContactCardView->activateView(viewParameters);
    QVERIFY(mContactCardView->isFavoriteGroupCreated() == true);
    }



void TestCntContactCardView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mContactCardView;
    mContactCardView = 0;
}

