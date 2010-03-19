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
#include <hbtoolbar.h>
#include <hbpushbutton.h>
#include <hblistview.h>
#include <qtcontacts.h>

#include "cntEditView.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "cntbaseview.h"

#include "t_cnteditview.h"


void TestCntEditView::resetView()
{
    if (mEditView)
        {
        delete mEditView;
        mEditView=0;
        }
    mEditView= new CntEditView(mViewManager,0);
}


void TestCntEditView::initTestCase()
{
    mWindow=0;
    mViewManager=0;
    mEditView=0;

    mWindow=new CntMainWindow();
    mViewManager=new CntViewManager(mWindow);
}

void TestCntEditView::createView()
{
    resetView();
    QVERIFY(mWindow!=0);
    QVERIFY(mViewManager!=0);
    QVERIFY(mEditView!=0);
    
    
}


    
void TestCntEditView::addSoftkeyAction()
{
    resetView();
    
    mEditView->addSoftkeyAction();
    QVERIFY(mEditView->mSoftKeyQuitAction == mWindow->softKeyAction(Hb::SecondarySoftKey));
}

void TestCntEditView::signalMapper()
{

    resetView();
    mEditView->signalMapper();
    QVERIFY(mEditView->signalMapper() != 0);
}

void TestCntEditView::addActionsToToolBar()
{

    resetView();
    mEditView->addActionsToToolBar();
    QVERIFY(mEditView->actions()->actionList().count()==1);
}

void TestCntEditView::addMenuItems()
{

    resetView();
    mEditView->addMenuItems();
    QVERIFY(mEditView->actions()->actionList().count() == 1);
}

void TestCntEditView::activateView()
{

    resetView();


     
    //check that labels are empty
    HbPushButton* nameButton = static_cast<HbPushButton*>(mEditView->findWidget("nameButton"));
    QVERIFY(nameButton->text()==tr("name"));
     
    //set view parameters and create contact
    QContact* contact = new QContact();
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
     
    mEditView->contactManager()->saveContact(contact);
    
    CntViewParameters viewParameters(CntViewParameters::noView);
     
    viewParameters.setSelectedContact(*contact);
    mEditView->activateView(viewParameters);
     
    QVERIFY(nameButton->text()==tr("first"));
     
    delete name;
    mEditView->contactManager()->removeContact(contact->localId());



}


void TestCntEditView::viewId()
    {
    resetView();
    QVERIFY(mEditView->viewId()== CntViewParameters::editView);    
    }
void TestCntEditView::prepareToEditContact()
    {
    resetView();   
    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters=mEditView->prepareToEditContact(QContactName::DefinitionName,tr("Test"));

    
    CntViewParameters viewParameters1(CntViewParameters::noView);
    viewParameters1=mEditView->prepareToEditContact(QContactPhoneNumber::DefinitionName,tr("Test"));
    
    CntViewParameters viewParameters2(CntViewParameters::noView);
    viewParameters2=mEditView->prepareToEditContact(QContactEmailAddress::DefinitionName,tr("Test"));

    CntViewParameters viewParameters3(CntViewParameters::noView);
    viewParameters3=mEditView->prepareToEditContact(QContactAddress::DefinitionName,tr("Test"));

    CntViewParameters viewParameters4(CntViewParameters::noView);
    viewParameters4=mEditView->prepareToEditContact(QContactOnlineAccount::DefinitionName,tr("Test"));

    CntViewParameters viewParameters5(CntViewParameters::noView);
    viewParameters5=mEditView->prepareToEditContact(QContactUrl::DefinitionName,tr("Test"));

    CntViewParameters viewParameters6(CntViewParameters::noView);
    viewParameters6=mEditView->prepareToEditContact(QContactNote::DefinitionName,tr("Test"));
    
    CntViewParameters viewParameters7(CntViewParameters::noView);
    viewParameters7=mEditView->prepareToEditContact(QContactFamily::DefinitionName,tr("Test"));

    CntViewParameters viewParameters8(CntViewParameters::noView);
    viewParameters8=mEditView->prepareToEditContact(QContactOrganization::DefinitionName,tr("Test"));

    CntViewParameters viewParameters9(CntViewParameters::noView);
    viewParameters9=mEditView->prepareToEditContact(QContactBirthday::DefinitionName,tr("Test"));

    CntViewParameters viewParameters10(CntViewParameters::noView);
    viewParameters10=mEditView->prepareToEditContact(QContactAnniversary::DefinitionName,tr("Test"));
    
    CntViewParameters viewParameters11(CntViewParameters::editView);
    viewParameters11=mEditView->prepareToEditContact(tr("INVALID"),tr("Test"));
    
    QVERIFY(viewParameters.nextViewId()== CntViewParameters::namesEditorView);    
    QVERIFY(viewParameters1.nextViewId()== CntViewParameters::phoneNumberEditorView);    
    QVERIFY(viewParameters2.nextViewId()== CntViewParameters::emailEditorView);    
    QVERIFY(viewParameters3.nextViewId()== CntViewParameters::addressEditorView);    
    QVERIFY(viewParameters3.selectedAction()== tr("Test"));    
    QVERIFY(viewParameters4.nextViewId()== CntViewParameters::onlineAccountEditorView);    
    QVERIFY(viewParameters5.nextViewId()== CntViewParameters::urlEditorView);    
    QVERIFY(viewParameters6.nextViewId()== CntViewParameters::noteEditorView);    
    QVERIFY(viewParameters7.nextViewId()== CntViewParameters::familyDetailEditorView);    
    QVERIFY(viewParameters8.nextViewId()== CntViewParameters::companyEditorView);    
    QVERIFY(viewParameters9.nextViewId()== CntViewParameters::noView);    
    QVERIFY(viewParameters10.nextViewId()== CntViewParameters::noView);    
    QVERIFY(viewParameters11.nextViewId()== CntViewParameters::noView);    
    
    
    }



void TestCntEditView::aboutToCloseView()
    {
    resetView();
    //TODO: How to test?
    }
void TestCntEditView::onListViewActivated()
    {
    resetView();
    //TODO: How to test?    
    }
void TestCntEditView::deleteDetail()
    {
    resetView();    
    // create listView
    mEditView->listView();
    mEditView->listView()->setModel(mEditView->contactModel());

    int oldCount=mEditView->listView()->indexCount();

    QContact *contact=new QContact();
    QContactName *name=new QContactName(); 
    name->setFirst("first");
    contact->saveDetail(name);
    mEditView->contactManager()->saveContact(contact);
    mEditView->setContact(contact);
    
    int newCount=mEditView->listView()->indexCount();
    QVERIFY(newCount==oldCount+1);
    mEditView->contactManager()->removeContact(contact->localId());
    
    delete name;
    
    }

void TestCntEditView::onLongPressed()
    {
    resetView();    
    }

void TestCntEditView::addField()
    {
    resetView();
    //TODO: How to test?    
    }

void TestCntEditView::openNameEditor()
    {
    resetView();  
    CntViewParameters viewParameters(CntViewParameters::namesEditorView);
    QContact contact;
    QContactName *name=new QContactName(); 
    name->setFirst("first");
    contact.saveDetail(name);
  
    viewParameters.setSelectedContact(contact);
    mViewManager->onActivateView(viewParameters);
    delete name;
    
    
    
    }

void TestCntEditView::handleExecutedCommand()
    {
    resetView();    

    QContact *contact=new QContact();
    QContactName *name=new QContactName(); 
    name->setFirst("first");
    contact->saveDetail(name);
    
    mEditView->setContact(contact);
    int result=mEditView->handleExecutedCommand(tr("delete"), *contact);
    QVERIFY(result==0);    
    result=mEditView->handleExecutedCommand(tr("INVALID"), *contact);
    QVERIFY(result==-1);    

    delete name;
    delete contact;
    
    }


void TestCntEditView::cleanupTestCase()
{
    if (mWindow)
        {
        mWindow->deleteLater();
        mWindow=0;
        }
    if (mViewManager)
        {
        mViewManager->deleteLater();
        mViewManager=0;
        }
    if (mEditView)
        {
        mEditView->setMenu(0);
        mEditView->deleteLater();
        mEditView=0;
        }

}

QTEST_MAIN(TestCntEditView)


