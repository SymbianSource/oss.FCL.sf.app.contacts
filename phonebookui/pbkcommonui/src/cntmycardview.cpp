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

#include "cntmycardview.h"
#include "qtpbkglobal.h"
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>

const char *CNT_MYCARD_UI_XML = ":/xml/contacts_mc.docml";

CntMyCardView::CntMyCardView() :
    mContact(NULL),
    mViewManager(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_MYCARD_UI_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_mc.docml");
    }

    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
}

CntMyCardView::~CntMyCardView()
{
    mView->deleteLater();
    
    delete mContact;
    mContact = 0;
}

/*!
Activates a previous view
*/
void CntMyCardView::showPreviousView()
{
	CntViewParameters args;
    mViewManager->back(args);
}

/*
Activates a default view
*/
void CntMyCardView::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());
    mViewManager = aMgr;
    
    HbPushButton *newButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_new")));
    connect(newButton, SIGNAL(clicked()), this, SLOT(openNameEditor()));

    HbPushButton *chooseButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_choose")));
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(openMyCardSelectionView()));

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QLatin1String(QContactType::TypeContact));

    if (mViewManager->contactManager( SYMBIAN_BACKEND )->contactIds(filter).isEmpty())
    {
        chooseButton->setEnabled(false);
    }
}

void CntMyCardView::deactivate()
{

}

void CntMyCardView::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocumentLoader.load(CNT_MYCARD_UI_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        mDocumentLoader.load(CNT_MYCARD_UI_XML, "landscape");
    }
}

/*!
Opens the name detail editor view
*/
void CntMyCardView::openNameEditor()
{
    //create a new my card contact
    QContactManager* mgr = mViewManager->contactManager( SYMBIAN_BACKEND );
    mgr->saveContact(mContact);
    mgr->setSelfContactId(mContact->localId());
    //open the contact editor
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, editView);
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

/*!
Opens the my card selection view
*/
void CntMyCardView::openMyCardSelectionView()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, myCardSelectionView);
    mViewManager->changeView(viewParameters);
}

// EOF
