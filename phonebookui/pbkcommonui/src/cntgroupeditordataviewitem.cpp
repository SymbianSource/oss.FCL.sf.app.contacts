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

#include "cntgroupeditordataviewitem.h"
#include "cnteditordatamodelitem.h"
#include "cntgroupeditorview.h"
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hbdatetimeedit.h>
#include <hblineedit.h>
#include <hbcombobox.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QStandardItemModel>

CntGroupEditorDataViewItem::CntGroupEditorDataViewItem(CntGroupEditorView *view, QGraphicsItem *parent):
    HbDataFormViewItem(parent),
    mView(view),
    mIconLabel(0),
    mLineEdit(0)
{
}

CntGroupEditorDataViewItem::~CntGroupEditorDataViewItem()
{
}

HbAbstractViewItem* CntGroupEditorDataViewItem::createItem()
{
    return new CntGroupEditorDataViewItem(*this);
}

HbWidget* CntGroupEditorDataViewItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    
    switch(itemType)
    {
    // DataFormItem with label and lineedit
    case HbDataFormModelItem::CustomItemBase:
    {
        HbWidget *widget = new HbWidget();
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        widget->setLayout(layout);
        
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
        
        HbLabel *label = new HbLabel();
        label->setPlainText(item->fieldLabel());
        label->setAlignment(Qt::AlignLeft);
        layout->addItem(label);
        
        mLineEdit = new HbLineEdit();
        mLineEdit->setText(item->contactDetail().value(item->key()));
        mLineEdit->setMaxLength(item->maxLength());
        setTextFilter(mLineEdit, item);
        connect(mLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
        layout->addItem(mLineEdit);       
        
        return widget;
    }
    // DataFormItem with push button
    case HbDataFormModelItem::CustomItemBase + 1:
    {
        HbWidget *widget = new HbWidget();
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        widget->setLayout(layout);
        
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
        
        HbPushButton *button = new HbPushButton();
        button->setText(item->buttonLabel());
        connect(button, SIGNAL(clicked()), this, SLOT(buttonPushed()));
        layout->addItem(button);
        
        return widget;
    }
    
    default:
        return 0;
    }
}

void CntGroupEditorDataViewItem::setTextFilter(QObject *aEditor, CntEditorDataModelItem *item)
{
    HbEditorInterface editorInterface(aEditor);
    editorInterface.setFilter(item->inputFilter());

    if (editorInterface.filter() == HbEmailAddressFilter::instance() ||
            editorInterface.filter() == HbInputLowerCaseFilter::instance())
    {
        editorInterface.setTextCase(HbTextCaseLower);
    }
    else if (editorInterface.filter() == HbPhoneNumberFilter::instance())
    {
        editorInterface.setUpAsPhoneNumberEditor();
    }
}


void CntGroupEditorDataViewItem::textChanged(QString text)
{
    HbDataForm* form = static_cast<HbDataForm*>(itemView());
    HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
    CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
    item->contactDetail().setValue(item->key(), text);
}

void CntGroupEditorDataViewItem::buttonPushed()
{
mView->itemPressed();
}

