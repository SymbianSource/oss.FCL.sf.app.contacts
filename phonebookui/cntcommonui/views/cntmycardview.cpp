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
#include "cntfetchcontactpopup.h"
#include "cntglobal.h"

#include <qtcontacts.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>

const char *CNT_MYCARD_UI_XML = ":/xml/contacts_mc.docml";

/*!
Constructor
*/
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

/*!
Destructor
*/
CntMyCardView::~CntMyCardView()
{
    mView->deleteLater();
    
    delete mContact;
}

/*
Activates the view
*/
void CntMyCardView::activate(const CntViewParameters aArgs)
{
    mViewManager = &mEngine->viewManager();
    
    if (mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());

    mNewButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_new")));
    connect(mNewButton, SIGNAL(released()), this, SLOT(openEditor()));

    mChooseButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_choose")));
    connect(mChooseButton, SIGNAL(released()), this, SLOT(openMyCardSelectionDialog()));
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QLatin1String(QContactType::TypeContact));

    if (mEngine->contactManager( SYMBIAN_BACKEND ).contactIds(filter).isEmpty())
    {
        mChooseButton->setEnabled(false);
    }
}

void CntMyCardView::deactivate()
{
}

/*!
Activates a previous view
*/
void CntMyCardView::showPreviousView()
{
    CntViewParameters args;
    mViewManager->back(args);
}

/*!
Opens the editor view
*/
void CntMyCardView::openEditor()
{
    if (mNewButton->isUnderMouse())
    {
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, editView);
        viewParameters.insert(EMyCard, "myCard" );
        
        QVariant var;
        var.setValue(*mContact);
        viewParameters.insert(ESelectedContact, var);
        viewParameters.insert(EExtraAction, CNT_ROOT_ACTION);
        mViewManager->changeView(viewParameters);
    }

}

/*!
Opens the my card selection dialog
*/
void CntMyCardView::openMyCardSelectionDialog()
{
    if (mChooseButton->isUnderMouse())
    {
        CntFetchContactPopup* popup = CntFetchContactPopup::createSingleSelectionPopup(
                   hbTrId("txt_phob_title_select_contact"),
                   mEngine->contactManager(SYMBIAN_BACKEND));
        connect( popup, SIGNAL(fetchReady(QSet<QContactLocalId>)), this, SLOT(handleMyCardSelection(QSet<QContactLocalId>)));
        QSet<QContactLocalId> ids;
        popup->setSelectedContacts(ids);
        popup->showPopup();
    }
}

/*!
Handle the contact selection
*/
void CntMyCardView::handleMyCardSelection( QSet<QContactLocalId> aIds )
{
    QContactManager& manager = mEngine->contactManager( SYMBIAN_BACKEND );

    if ( !aIds.isEmpty() ) {
        QList<QContactLocalId> selectedContactsList = aIds.values();
        QContact contact = manager.contact(selectedContactsList.front());
        removeFromGroup(&contact);
        
        manager.setSelfContactId( contact.localId() );
        showPreviousView();
    }
}

/*!
Sets the layout according to the orientation
*/
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
Removes the contact from all possible groups
*/
void CntMyCardView::removeFromGroup(const QContact* aContact)
{
    // Fetch all groups the contact is member and remove the relationships
    QContactManager& mgr = mEngine->contactManager(SYMBIAN_BACKEND);
    QContactRelationshipFilter relationshipFilter;                   
    relationshipFilter.setRelationshipType(QContactRelationship::HasMember);
    relationshipFilter.setRelatedContactId(aContact->id());           
    relationshipFilter.setRelatedContactRole(QContactRelationship::Second);
    QList<QContactLocalId> groupIds = mgr.contactIds(relationshipFilter);
    
    QList<QContactRelationship> relationships;
    for(int i = 0;i < groupIds.count();i++)
    {
        QContact groupContact = mgr.contact(groupIds.at(i));
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(groupContact.id());
        relationship.setSecond(aContact->id());
        relationships.append(relationship);
    }
    
    QMap<int, QContactManager::Error> errorMap;
    mgr.removeRelationships(relationships,&errorMap);
}

// EOF
