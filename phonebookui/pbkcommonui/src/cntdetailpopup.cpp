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

#include "cntdetailpopup.h"

#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <qtcontacts.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <QStandardItemModel>

QTM_USE_NAMESPACE

CntDetailPopup::CntDetailPopup(QGraphicsItem *parent):
    HbDialog(parent),
    mListModel(NULL),
    mListView(NULL)
{
    mListView = new HbListView(this);
    mListModel = new QStandardItemModel(this);

    addListItem("qtg_small_mobile", hbTrId("txt_phob_list_number"), phoneNumberEditorView );
    addListItem("qtg_small_email", hbTrId("txt_phob_list_email"), emailEditorView );
    addListItem("qtg_small_url_address", hbTrId("txt_phob_list_url"), urlEditorView);
    addListItem("qtg_small_location", hbTrId("txt_phob_list_address"), addressEditorView );
    addListItem("qtg_small_note", hbTrId("txt_phob_formlabel_note"), noteEditorView);
    addListItem("qtg_small_sound", hbTrId("txt_phob_formlabel_personal_ringing_tone"), noView );
    addListItem("qtg_small_calendar", hbTrId("txt_phob_formlabel_date"), dateEditorView);
    addListItem("qtg_small_company_details", hbTrId("txt_phob_formlabel_company_details"), companyEditorView);
    addListItem("qtg_small_family", hbTrId("txt_phob_formlabel_family"), familyDetailEditorView);
    
    mListView->setModel(mListModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    // ownership of prototype is not transferred
    HbListViewItem* prototype = mListView->listItemPrototype();
    prototype->setGraphicsSize( HbListViewItem::SmallIcon );
    
    HbGroupBox *headingLabel = new HbGroupBox();
    HbLabel *label = new HbLabel(hbTrId("txt_phob_title_add_field"));    
    headingLabel->setContentWidget(label);

    setHeadingWidget(headingLabel);
    setContentWidget(mListView);

    HbAction *mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    setSecondaryAction(mSecondaryAction);
    setTimeout(0);
    setModal(true);

    connect(mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(listItemSelected(QModelIndex)));
}

CntDetailPopup::~CntDetailPopup()
{
}

void CntDetailPopup::listItemSelected(QModelIndex index)
{
    mSelectedDetail = mListModel->item(index.row(), 1)->text();
    close();
}

QString CntDetailPopup::selectedDetail()
{
    return mSelectedDetail;
}

int CntDetailPopup::selectDetail()
{
    CntDetailPopup *popup = new CntDetailPopup();
    QString result;

    HbAction *action = popup->exec();

    if (action != popup->secondaryAction() && !popup->selectedDetail().isEmpty())
   	{
        result = popup->selectedDetail();
    }
    delete popup;

    return result.toInt();
}

void CntDetailPopup::addListItem(QString aIcon, QString label, int aId )
{
    QList<QStandardItem*> items;
    QStandardItem *labelItem = new QStandardItem(HbIcon(aIcon).qicon(), label);
    QString id;
    QStandardItem *idItem = new QStandardItem( id.number(aId) );

    items << labelItem << idItem;
    mListModel->appendRow(items);
}
