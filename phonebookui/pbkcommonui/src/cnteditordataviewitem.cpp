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

#include "cnteditordataviewitem.h"
#include "cnteditordatamodelitem.h"
#include "cntbasedetaileditorview.h"

#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hbdatetimepicker.h>
#include <hblineedit.h>
#include <hbcombobox.h>
#include <hbpushbutton.h>
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbdialog.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

CntEditorDataViewItem::CntEditorDataViewItem(CntBaseDetailEditorView *view, QGraphicsItem *parent):
    HbDataFormViewItem(parent),
    mView(view),
    mLineEdit(0)
{
}

CntEditorDataViewItem::~CntEditorDataViewItem()
{
}

HbAbstractViewItem* CntEditorDataViewItem::createItem()
{
    return new CntEditorDataViewItem(*this);
}

HbWidget* CntEditorDataViewItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    
    switch(itemType)
    {
    // DataFormItem with combobox and lineedit
    case HbDataFormModelItem::CustomItemBase:
    {
        HbWidget *widget = new HbWidget();
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        widget->setLayout(layout);

        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
        
        HbComboBox *box = new HbComboBox();
        box->setModel(item->fieldModel());
        box->setCurrentIndex(modelPosition(item->fieldModel(), item->contactDetail()));
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
        layout->addItem(box);
        
        mLineEdit = new HbLineEdit();
        mLineEdit->setText(item->contactDetail().value(item->key()));
        mLineEdit->setMaxLength(item->fieldModel()->item(modelPosition(item->fieldModel(), 
                item->contactDetail()))->data(Qt::UserRole+3).toInt());
        setTextFilter(mLineEdit, item);
        connect(mLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
        layout->addItem(mLineEdit);

        return widget;
    }
    // DataFormItem with lineedit
    case HbDataFormModelItem::CustomItemBase + 1:
    {
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
        
        mLineEdit = new HbLineEdit();
        mLineEdit->setText(item->contactDetail().value(item->key()));
        mLineEdit->setMaxLength(item->maxLength());
        setTextFilter(mLineEdit, item);
        connect(mLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

        if (item->contactDetail().definitionName() == QContactNote::DefinitionName)
        {
            mLineEdit->setMinRows(5);
            mLineEdit->setMaxRows(5);
        }
        else
        {
            mLineEdit->setMinRows(1);
            mLineEdit->setMaxRows(1);
        }
        
        return mLineEdit;
    }
    // DataFormItem with pushbutton
    case HbDataFormModelItem::CustomItemBase + 2:
    {
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
        
        HbPushButton *button = new HbPushButton();
        button->setBackground(HbIcon(" "));
        button->setText(item->buttonLabel());
        connect(button, SIGNAL(clicked()), this, SLOT(addDetail()));
        
        return button;
    }
    // DataFormItem with pushbutton for date editors
    case HbDataFormModelItem::CustomItemBase + 3:
    {
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));

        QString date;
        QDate tempDate = item->contactDetail().value<QDate>(item->key());

        if (tempDate.isNull())
        {
            date = hbTrId("No date set");
        }
        else
        {
            date = tempDate.toString("dd MMMM yyyy");
        }

        HbPushButton *button = new HbPushButton();
        button->setBackground(HbIcon(" "));
        button->setText(date);
        connect(button, SIGNAL(clicked()), this, SLOT(editDate()));

        return button;
    }
    default:
        return 0;
    }
}

/*!
Set focus to the current item's lineedit
*/
void CntEditorDataViewItem::focusLineEdit()
{
    mLineEdit->setFocus();
}

int CntEditorDataViewItem::modelPosition(QStandardItemModel *model, QContactDetail &detail)
{
    int position = 0;
    
    QString subType;
    QString context;
    
    if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber number = detail;
        subType = number.subTypes().at(0);
    }
    else if (detail.definitionName() == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account = detail;
        subType = account.subTypes().at(0);
    }
    else if (detail.definitionName() == QContactEmailAddress::DefinitionName)
    {
        QString temp = QContactEmailAddress::FieldEmailAddress;
        subType = temp;
    }
    else if (detail.definitionName() == QContactUrl::DefinitionName)
    {
        QString temp = QContactUrl::FieldUrl;
        subType = temp;
    }
    
    if (detail.contexts().count() != 0)
    {
        context = detail.contexts().at(0);
    }
    
    for (int i = 0;i < model->rowCount();i++)
    {
        if (model->item(i)->data(Qt::UserRole).toString() == subType &&
                model->item(i)->data(Qt::UserRole+1).toString() == context)
        {
            return position;
        }
        else
        {
            position++;
        }
    }
    
    return position;
}

/*!
    Set text filter to the widget.
*/
void CntEditorDataViewItem::setTextFilter(QObject *aEditor, CntEditorDataModelItem *item)
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

void CntEditorDataViewItem::textChanged(QString text)
{
    HbDataForm* form = static_cast<HbDataForm*>(itemView());
    HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
    CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
    
    if (item->key() == QContactOrganization::FieldDepartment || item->key() == QContactFamily::FieldChildren)
    {
        item->contactDetail().setValue(item->key(), QStringList(text.trimmed()));
    }
    else
    {
        item->contactDetail().setValue(item->key(), text.trimmed());
    }
}

void CntEditorDataViewItem::indexChanged(int index)
{
    HbDataForm* form = static_cast<HbDataForm*>(itemView());
    HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
    CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));
    QStandardItem *sItem = item->fieldModel()->item(index);
    
    // Update context
    if (!sItem->data(Qt::UserRole+1).toString().isEmpty())
    {
        item->contactDetail().setContexts(sItem->data(Qt::UserRole+1).toString());
    }
    else
    {
        item->contactDetail().setContexts(QStringList());
    }

    // Update subtype if applicable
    if (item->contactDetail().definitionName() == QContactPhoneNumber::DefinitionName)
    {
        item->contactDetail().setValue(QContactPhoneNumber::FieldSubTypes, QStringList(sItem->data(Qt::UserRole).toString()));
    }
    else if (item->contactDetail().definitionName() == QContactOnlineAccount::DefinitionName)
    {
        item->contactDetail().setValue(QContactOnlineAccount::FieldSubTypes, QStringList(sItem->data(Qt::UserRole).toString()));
    }
    
    // Update maxlength of the lineedit
    mLineEdit->setMaxLength(sItem->data(Qt::UserRole+3).toInt());
}

void CntEditorDataViewItem::addDetail()
{
    mView->addDetail();
}

void CntEditorDataViewItem::editDate()
{
    HbDataForm* form = static_cast<HbDataForm*>(itemView());
    HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
    CntEditorDataModelItem* item = static_cast<CntEditorDataModelItem*>(model->itemFromIndex(modelIndex()));

    HbDialog popup;

    popup.setDismissPolicy(HbDialog::NoDismiss);
    popup.setTimeout(HbPopup::NoTimeout);

    HbTextItem *headingText = new HbTextItem(&popup);
    headingText->setFontSpec(HbFontSpec(HbFontSpec::Title));
    if (item->contactDetail().definitionName() == QContactBirthday::DefinitionName)
    {
        headingText->setText(hbTrId("txt_phob_dblist_birthday"));
    }
    else
    {
        headingText->setText(hbTrId("txt_phob_dblist_anniversary"));
    }

    popup.setHeadingWidget(headingText);

    HbDateTimePicker *picker = new HbDateTimePicker(&popup);
    picker->setDisplayFormat("dd.MMM.yyyy");
    picker->setDateRange(QDate(1900, 0, 0), QDate(2050, 0, 0));
    picker->setDate(item->contactDetail().value<QDate>(item->key()));
    popup.setContentWidget(picker);

    popup.setPrimaryAction(new HbAction(hbTrId("txt_common_button_ok"), &popup));
    popup.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), &popup));

    HbAction *selected = popup.exec();
    if (selected == popup.primaryAction())
    {
        item->contactDetail().setValue(item->key(), picker->date());
        qobject_cast<HbPushButton*>(sender())->setText(picker->date().toString("dd MMMM yyyy"));
    }
}
