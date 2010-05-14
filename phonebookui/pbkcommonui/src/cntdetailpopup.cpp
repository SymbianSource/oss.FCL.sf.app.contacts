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

CntDetailPopup::CntDetailPopup(QGraphicsItem *parent,  CntViewIdList aList ):
    HbDialog(parent),
    mListModel(NULL),
    mListView(NULL),
    mSelectedDetail(0),
    mViewIdList( aList )
{
    mListView = new HbListView(this);
    mListModel = new QStandardItemModel(this);

    addListItem("qtg_small_mobile", hbTrId("txt_phob_list_number"), phoneNumberEditorView );
    addListItem("qtg_small_email", hbTrId("txt_phob_list_email"), emailEditorView );
    addListItem("qtg_small_url_address", hbTrId("txt_phob_list_url"), urlEditorView);
    addListItem("qtg_small_location", hbTrId("txt_phob_list_address"), addressEditorView );
    addListItem("qtg_small_note", hbTrId("txt_phob_list_note"), noteEditorView);
    addListItem("qtg_small_sound", hbTrId("txt_phob_list_personal_ringing_tone"), noView );
    addListItem("qtg_small_calendar", hbTrId("txt_phob_list_date"), dateEditorView);
    addListItem("qtg_small_company_details", hbTrId("txt_phob_list_company_details"), companyEditorView);
    addListItem("qtg_small_family", hbTrId("txt_phob_list_family"), familyDetailEditorView);
    
    mListView->setModel(mListModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    // ownership of prototype is not transferred
    HbListViewItem* prototype = mListView->listItemPrototype();
    prototype->setGraphicsSize( HbListViewItem::SmallIcon );
    
    HbLabel *label = new HbLabel(this);    
    label->setPlainText(hbTrId("txt_phob_title_add_field"));

    setHeadingWidget(label);
    setContentWidget(mListView);

    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(cancelAction);
    setTimeout(HbDialog::NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setModal(true);

    connect(mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(listItemSelected(QModelIndex)));
}

CntDetailPopup::~CntDetailPopup()
{
}

void CntDetailPopup::listItemSelected(QModelIndex index)
{
    mSelectedDetail = mListModel->item(index.row())->data(Qt::UserRole).toInt();
    close();
}

int CntDetailPopup::selectedDetail()
{
    return mSelectedDetail;
}

void CntDetailPopup::selectDetail( CntViewIdList aList, QObject *receiver, const char *member  )
{
    CntDetailPopup *popup = new CntDetailPopup( NULL, aList );
    
    popup->open(receiver, member);
}

void CntDetailPopup::addListItem(QString aIcon, QString label, int aId )
{
    if ( !mViewIdList.contains(aId) )
    {
        QStandardItem *labelItem = new QStandardItem(label);
        labelItem->setData(HbIcon(aIcon), Qt::DecorationRole);
        labelItem->setData(aId, Qt::UserRole);

        mListModel->appendRow(labelItem);
    }
}
