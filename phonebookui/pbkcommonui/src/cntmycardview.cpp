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
#include <hbpushbutton.h>

const char *CNT_MYCARD_UI_XML = ":/xml/contacts_mc.docml";

CntMyCardView::CntMyCardView(CntViewManager *viewManager, QGraphicsItem *parent) : CntBaseView(viewManager, parent), mContact(0)
{   
    bool ok = false;
    ok=loadDocument(CNT_MYCARD_UI_XML);

    if (ok)
    {
    
        QGraphicsWidget *w = findWidget(QString("view"));
        setWidget(w);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_mc.docml");
    }
}

CntMyCardView::~CntMyCardView()
{
    delete mContact;
}

/*!
Activates a previous view
*/
void CntMyCardView::aboutToCloseView()
{
    viewManager()->onActivateView(CntViewParameters::namesView);
}

/*
Activates a default view
*/
void CntMyCardView::activateView(const CntViewParameters &aViewParameters)
{
    mContact = new QContact(aViewParameters.selectedContact());
    
    HbPushButton *newButton = static_cast<HbPushButton*>(findWidget(QString("cnt_button_new")));
    connect(newButton, SIGNAL(clicked()), this, SLOT(openNameEditor()));

    HbPushButton *chooseButton = static_cast<HbPushButton*>(findWidget(QString("cnt_button_choose")));
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(openMyCardSelectionView()));

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QLatin1String(QContactType::TypeContact));
    if (contactManager()->contacts(filter).isEmpty())
    {
        chooseButton->setEnabled(false);
    }
}

void CntMyCardView::setOrientation(Qt::Orientation orientation)
{
    if( orientation == Qt::Vertical ) 
    {
        // reading "portrait" section
        loadDocument(CNT_MYCARD_UI_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        loadDocument(CNT_MYCARD_UI_XML, "landscape");
    }
}

/*!
Opens the name detail editor view
*/
void CntMyCardView::openNameEditor()
{
    //create a new my card contact
    contactManager()->saveContact(mContact);
    contactManager()->setSelfContactId(mContact->localId());
    //open the contact editor
    CntViewParameters viewParameters(CntViewParameters::editView);
    viewParameters.setSelectedContact(*mContact);
    viewManager()->onActivateView(viewParameters);
}

/*!
Opens the my card selection view
*/
void CntMyCardView::openMyCardSelectionView()
{
    CntViewParameters viewParameters(CntViewParameters::myCardSelectionView);
    viewManager()->onActivateView(viewParameters);
}

// EOF
