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

#include "cntserviceassigncontactcardview.h"
#include "cntservicehandler.h"

#include <hbaction.h>
#include <hbdialog.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbwidget.h>
#include <hbtoolbar.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <QCoreApplication>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceAssignContactCardView::CntServiceAssignContactCardView(CntServiceHandler *aServiceHandler) : 
    CntContactCardView(),
    mServiceHandler(aServiceHandler)
{
    connect(this, SIGNAL(backPressed()), this, SLOT(doCloseView()));
    connect(this, SIGNAL(viewActivated(QContact, QContactDetail)), this, SLOT(doViewActivated(QContact,QContactDetail)));
    
    addActionsToToolBar();
}

/*!
Destructor
*/
CntServiceAssignContactCardView::~CntServiceAssignContactCardView()
{

}

/*!
Add actions to the toolbar
*/
void CntServiceAssignContactCardView::addActionsToToolBar()
{
    view()->toolBar()->clearActions();  
    HbAction* addToContact = view()->toolBar()->addAction("txt_phob_button_add_to_contacts");
    connect(addToContact, SIGNAL(triggered()), this, SLOT(addToContacts()));
}

/*!
Opens the Add to Contacts popup
*/
void CntServiceAssignContactCardView::addToContacts()
{  
    HbDialog *popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setHeadingWidget(new HbLabel(hbTrId("txt_phob_title_add_to_contacts"), popup));
    popup->setAttribute(Qt::WA_DeleteOnClose);

    HbWidget *buttonWidget = new HbWidget(popup);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    HbPushButton *addButton = new HbPushButton(hbTrId("txt_missing_list_save_as_a_new_contact"), buttonWidget);
    connect(addButton, SIGNAL(clicked()), this, SLOT(saveNew()));
    HbPushButton *updateButton = new HbPushButton(hbTrId("txt_missing_list_update_existing_contact"), buttonWidget);
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateExisting()));
    layout->addItem(addButton);
    layout->addItem(updateButton);
    buttonWidget->setLayout(layout);
    popup->setContentWidget(buttonWidget);
    
    popup->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), popup));
}

/*!
Create a new contact with the detail
*/
void CntServiceAssignContactCardView::saveNew()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(mContact);
    viewParameters.insert(ESelectedContact, var);
    //viewManager()->changeView(viewParameters);
}

/*!
Update an existing contact with the detail
*/
void CntServiceAssignContactCardView::updateExisting()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, serviceContactSelectionView);
    QVariant var;
    var.setValue(mDetail);
    viewParameters.insert(ESelectedDetail, var);
    //viewManager()->changeView(viewParameters);
}

/*!
Called after the view has been activated
*/
void CntServiceAssignContactCardView::doViewActivated(QContact contact, QContactDetail detail)
{
    mContact = contact;
    mDetail = detail;
}

/*!
Close the view (quits the service as well)
*/
void CntServiceAssignContactCardView::doCloseView()
{
    qApp->quit();
}

// end of file
