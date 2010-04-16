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

#include <hbdialog.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbwidget.h>
#include <QGraphicsLinearLayout>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceAssignContactCardView::CntServiceAssignContactCardView(CntServiceHandler *aServiceHandler, CntViewManager *viewManager, QGraphicsItem *parent) : 
    CntContactCardView(viewManager, parent),
    mServiceHandler(aServiceHandler)
{

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
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:addtocontacts"); 
    actions()->addActionsToToolBar(toolBar());

    connect(actions()->baseAction("cnt:addtocontacts"), SIGNAL(triggered()),
            this, SLOT(addToContacts()));
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
    CntViewParameters viewParameters(CntViewParameters::serviceEditView);
    viewParameters.setSelectedContact(*mContact);
    viewManager()->changeView(viewParameters);
}

/*!
Update an existing contact with the detail
*/
void CntServiceAssignContactCardView::updateExisting()
{
    CntViewParameters viewParameters(CntViewParameters::serviceContactSelectionView);
    viewParameters.setSelectedDetail(mDetail);
    viewManager()->changeView(viewParameters);
}

void CntServiceAssignContactCardView::activateView(const CntViewParameters &viewParameters)
{
    CntContactCardView::activateView(viewParameters);
    mDetail = viewParameters.selectedDetail();
}

/*!
Close the view (quits the service as well)
*/
void CntServiceAssignContactCardView::aboutToCloseView()
{
    qApp->quit();
}

// end of file