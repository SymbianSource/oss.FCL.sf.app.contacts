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
#include <QStandardItemModel>

QTM_USE_NAMESPACE

CntDetailPopup::CntDetailPopup(QStringList excludeList, QGraphicsItem *parent):
    HbDialog(parent),
    mListModel(0),
    mListView(0),
    mExcludeList(excludeList)
{
    mListView = new HbListView(this);
    mListModel = new QStandardItemModel();

    addListItem(hbTrId("txt_phob_formlabel_note"), QContactNote::DefinitionName);
    addListItem(hbTrId("txt_phob_formlabel_personal_ringing_tone"), QContactAvatar::SubTypeAudioRingtone);
    addListItem(hbTrId("txt_phob_formlabel_date"), QContactAnniversary::DefinitionName, QContactBirthday::DefinitionName);
    addListItem(hbTrId("txt_phob_formlabel_company_details"), QContactOrganization::DefinitionName, QContactOrganization::FieldAssistantName);
    addListItem(hbTrId("txt_phob_formlabel_family"), QContactFamily::FieldSpouse, QContactFamily::FieldChildren);
    addListItem("Synchronization", "some-synch-id");        // TODO: change to real synch id when backend done

    mListView->setModel(mListModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);

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

QString CntDetailPopup::selectDetail(QStringList excludeList)
{
    CntDetailPopup *popup = new CntDetailPopup(excludeList);
    QString result;

    HbAction *action = popup->exec();

    if (action != popup->secondaryAction() && !popup->selectedDetail().isEmpty())
   	{
        result = popup->selectedDetail();
    }
    delete popup;

    return result;
}

void CntDetailPopup::addListItem(QString label, QString id1, QString id2)
{
    if (mExcludeList.contains(id1) || mExcludeList.contains(id2))
        return;

    QList<QStandardItem*> items;
    QStandardItem *labelItem = new QStandardItem(label);
    QStandardItem *idItem = new QStandardItem(id1);

    items << labelItem << idItem;
    mListModel->appendRow(items);
}
