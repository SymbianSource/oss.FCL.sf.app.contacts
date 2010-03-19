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

#include "cntbasedetaileditorview.h"
#include "cnteditordataviewitem.h"

#include <hbgroupbox.h>

const char *CNT_EDITOR_XML = ":/xml/contacts_editor.docml";

CntBaseDetailEditorView::CntBaseDetailEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseView(viewManager, parent),
    mContact(0),
    mDataForm(0),
    mDataFormModel(0),
    mHeader(0)
{
    bool ok = false;
    ok = loadDocument(CNT_EDITOR_XML);

    if (ok)
    {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_editor.docml");
    }
}

CntBaseDetailEditorView::~CntBaseDetailEditorView()
{
    for(int i = 0;i < mDetailList.count();i++)
    {
        delete mDetailList.at(i);
    }
    delete mContact;
    delete mDataFormModel;
    delete mDataForm;
}

void CntBaseDetailEditorView::aboutToCloseView()
{
    viewManager()->previousViewParameters().setSelectedContact(*mContact);
    viewManager()->onActivatePreviousView();
}

/*!
Add actions to options menu
*/
void CntBaseDetailEditorView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:discardchanges");
    actions()->addActionsToMenu(menu());
    
    connect(actions()->baseAction("cnt:discardchanges"), SIGNAL(triggered()),
            this, SLOT(cancelChanges()));
}

/*!
Cancel all changes made to the details and return to main edit view
*/
void CntBaseDetailEditorView::cancelChanges()
{
    viewManager()->previousViewParameters().setSelectedContact(*mContact);
    viewManager()->onActivatePreviousView();
}

HbDataFormModel *CntBaseDetailEditorView::formModel()
{
    if (!mDataFormModel)
    {
        mDataFormModel = new HbDataFormModel();
    }
    return mDataFormModel;
}

HbDataForm *CntBaseDetailEditorView::dataForm()
{
    if (!mDataForm)
    {
        mDataForm = static_cast<HbDataForm*>(findWidget(QString("dataForm")));
        mDataForm->setItemRecycling(true);
        CntEditorDataViewItem *viewItem = new CntEditorDataViewItem(this);
        mDataForm->setModel(formModel(), viewItem);
    }
    return mDataForm;
}

HbGroupBox *CntBaseDetailEditorView::header()
{
    if (!mHeader)
    {
        mHeader = static_cast<HbGroupBox*>(findWidget(QString("headerBox")));
    }
    return mHeader;
}

void CntBaseDetailEditorView::setHeader(QString headerText)
{
    header()->setHeading(headerText);
}

void CntBaseDetailEditorView::activateView(const CntViewParameters &viewParameters)
{
    mContact = new QContact(viewParameters.selectedContact());
    mParamString = viewParameters.selectedAction();  
    dataForm();
    initializeForm();
    if (mParamString == "add")
    {
        addDetail();
    }
}

