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

#include "cntgroupactionsview.h"

#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbgroupbox.h>
#include <mobcntmodel.h>
#include <hbframebackground.h>

#include <QStandardItemModel>

const char *CNT_GROUPACTIONSVIEW_XML = ":/xml/contacts_groupactions.docml";

CntGroupActionsView::CntGroupActionsView() :
mGroupContact(NULL),
mModel(NULL),
mViewManager(NULL),
mListView(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_GROUPACTIONSVIEW_XML, &ok);
  
    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_groupactions.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu actions
    mEditGrpDetailAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:editgroupdetail"));
    connect(mEditGrpDetailAction, SIGNAL(triggered()), this, SLOT(editGroup()));
}

/*!
Destructor
*/
CntGroupActionsView::~CntGroupActionsView()
{
    mView->deleteLater();
    
    delete mEditGrpDetailAction; 
    mEditGrpDetailAction = NULL;
    
    delete mGroupContact;
    mGroupContact = NULL;
    
    delete mModel;
    mModel = NULL;
}


void CntGroupActionsView::editGroup()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, groupEditorView);
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);

}


/*!
Activates a previous view
*/
void CntGroupActionsView::showPreviousView()
{
    CntViewParameters viewParameters;
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->back(viewParameters);
}

void CntGroupActionsView::deactivate()
{
    
}

/*
Activates a default view and setup name label texts
*/
void CntGroupActionsView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mGroupContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());
    mViewManager = aMgr;

    QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
    
    
    //group box
    HbGroupBox* groupBox = static_cast<HbGroupBox *>(mDocumentLoader.findWidget(QString("groupBox")));
    groupBox->setHeading(groupName);
    
    // create list & model
    mListView = static_cast<HbListView*> (mDocumentLoader.findWidget("listView"));
    mListView->setUniformItemSizes(true);
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_parent_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    
    
    mModel = new QStandardItemModel();
    
    QContactPhoneNumber confCallNumber = mGroupContact->detail<QContactPhoneNumber>();
    if(!confCallNumber.number().isEmpty())
        {
        populatelist(hbTrId("txt_phob_dblist_conference_call"), HbIcon("qtg_large_call_group"),confCallNumber.number());
        }
      
    populatelist(hbTrId("txt_phob_dblist_send_message"),HbIcon("qtg_large_message_group"),hbTrId("txt_phob_dblist_send_message_val_members"));
    populatelist(hbTrId("txt_phob_dblist_email"),HbIcon("qtg_large_email_group"),hbTrId("txt_phob_dblist_send_message_val_members"));
    
    mListView->setModel(mModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    
}

void CntGroupActionsView:: populatelist(QString primaryText,HbIcon icon,QString secondaryText)
{
    QList<QStandardItem*> items;
    QStandardItem *labelItem = new QStandardItem();
    
    QStringList textList;
   
    textList << primaryText << secondaryText;
    
    labelItem->setData(textList, Qt::DisplayRole);
    labelItem->setData(icon, Qt::DecorationRole);
    
    items << labelItem ;
    mModel->appendRow(items);
}
