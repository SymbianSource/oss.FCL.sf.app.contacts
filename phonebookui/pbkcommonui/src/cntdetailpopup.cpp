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
#include <QStringListModel>

QTM_USE_NAMESPACE

CntDetailPopup::CntDetailPopup(QStringList excludeList, QGraphicsItem *parent):
    HbDialog(parent),
    mListModel(0),
    mListView(0)
{
    mListView = new HbListView(this);

    mListModel = new QStringListModel();
    
    QStringList list;
    
    if (!excludeList.contains(QContactNote::DefinitionName))
    {
        list << hbTrId("txt_phob_formlabel_note");
    }
    
    if (!excludeList.contains(QContactAvatar::SubTypeAudioRingtone))
    {
        list << hbTrId("txt_phob_formlabel_personal_ringing_tone");
    }
    
    if (!excludeList.contains(QContactBirthday::DefinitionName) && 
            !excludeList.contains(QContactAnniversary::DefinitionName))
    {
        list << hbTrId("txt_phob_formlabel_date");
    }

    if (!excludeList.contains(QContactOrganization::DefinitionName) && 
            !excludeList.contains(QContactOrganization::FieldAssistantName))
    {
        list << hbTrId("txt_phob_formlabel_company_details");
    }
    
    if (!excludeList.contains(QContactFamily::FieldSpouse) && 
            !excludeList.contains(QContactFamily::FieldChildren))
    {
        list << hbTrId("txt_phob_formlabel_family");
    }
    list << hbTrId("Synchronization");


    mListModel->setStringList(list);
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
    mSelectedDetail = mListModel->data(index, Qt::DisplayRole).toString();
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

    if (action == popup->secondaryAction())
    {

    }
    else
    {
        if (!popup->selectedDetail().isEmpty())
        {
            result = popup->selectedDetail();
        }
    }
    delete popup;

    return result;
}
