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
#include "cntfetchcontactsview.h"
#include "cntglobal.h"
#include <qtcontacts.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>

const char *CNT_MYCARD_UI_XML = ":/xml/contacts_mc.docml";

CntMyCardView::CntMyCardView() :
    mContact(NULL),
    mViewManager(NULL),
    mFetchView(NULL)
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
    
    delete mFetchView;
    mFetchView = NULL;
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
    mViewManager = aMgr;
    
    if (mView->navigationAction() != mSoftkey) {
        mView->setNavigationAction(mSoftkey);
    }
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());
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
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, editView);
    viewParameters.insert(EMyCard, "myCard" );
    
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
    // Display a list of contacts to choose a mycard from.
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(*mViewManager->contactManager( SYMBIAN_BACKEND ));
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleMultiCardSelection()));
    }
    mFetchView->setDetails(hbTrId("txt_phob_title_select_contact"), "");
    QSet<QContactLocalId> emptyContactsSet;
    mFetchView->displayContacts(HbAbstractItemView::SingleSelection, emptyContactsSet);
}

void CntMyCardView::handleMultiCardSelection()
{
    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );

    QSet<QContactLocalId> selectedContacts = mFetchView->getSelectedContacts();

    if ( !mFetchView->wasCanceled() && !selectedContacts.isEmpty() ) {
        QList<QContactLocalId> selectedContactsList = selectedContacts.values();
        QContact contact = manager->contact(selectedContactsList.front());
        removeFromGroup(&contact);
        
        manager->setSelfContactId( contact.localId() );
        showPreviousView();
    }
    else {
        delete mFetchView;
        mFetchView = NULL;
    }
}

void CntMyCardView::removeFromGroup(const QContact* aContact)
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));

    QContactManager* mgr = mViewManager->contactManager(SYMBIAN_BACKEND);
    QList<QContactLocalId> groupContactIds = mgr->contactIds(groupFilter);
    if (!groupContactIds.isEmpty()) {
        for(int i = 0;i < groupContactIds.count();i++) {
            QContact groupContact = mgr->contact(groupContactIds.at(i));
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(groupContact.id());
            relationship.setSecond(aContact->id());
            mgr->removeRelationship(relationship);  
        }
    }
}

// EOF
