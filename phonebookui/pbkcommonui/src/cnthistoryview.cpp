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

#include "cnthistoryview.h"

#include <QSqlDatabase>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hblistviewitem.h>
#include <mobhistorymodel.h>
#include "cnthistoryviewitem.h" 

const char *CNT_HISTORYVIEW_XML = ":/xml/contacts_history.docml";
//const QString db_path("c:\\conversations.sqlite");

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntHistoryView::CntHistoryView(CntViewManager *viewManager, QGraphicsItem *parent) : 
    CntBaseView(viewManager, parent),
    mHistoryListView(0),
    mHistoryModel(0),
    mContact(0),
    mIsMyCard(false)
{
    bool ok = false;
    ok = loadDocument(CNT_HISTORYVIEW_XML);

    if (ok)
    {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_history.docml");
    }
}

/*!
Destructor
*/
CntHistoryView::~CntHistoryView()
{
    delete mContact;
    delete mHistoryModel;
}

/*!
Activates a previous view
*/
void CntHistoryView::aboutToCloseView()
{
    viewManager()->previousViewParameters().setSelectedContact(*mContact);
    viewManager()->onActivatePreviousView();
}

void CntHistoryView::activateView(const CntViewParameters &viewParameters)
{ 
    QContact contact = viewParameters.selectedContact();
    mContact = new QContact(contact);
    
    //group box
    HbGroupBox* groupBox = static_cast<HbGroupBox *>(findWidget(QString("cnt_groupbox_history")));
    QString text;
    if (mContact->localId() == contactManager()->selfContactId()) {
        //My card
        text.append(hbTrId("All history"));
    }
    else {
        text.append(hbTrId("History: "));
        text.append(contactManager()->synthesizeDisplayLabel(contact));
    }
    groupBox->setHeading(text);
    
    //construct listview
    mHistoryListView = static_cast<HbListView*>(findWidget(QString("cnt_listview_history")));
    
    //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName(db_path);
    //db.open();
    
    //bubble graphics - create our custom list view item to have different bubbles
    //for incoming and outgoing messages 
    CntHistoryViewItem *item = new CntHistoryViewItem;
    mHistoryListView->setItemPrototype(item); //ownership is taken
    
    //create and set model for the list
    mHistoryModel = new MobHistoryModel(contact.localId(), contactManager());
    mHistoryListView->setModel(mHistoryModel); //ownership is not taken
    
    //start listening to the events amount changing in the model
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
}

/*!
Called after new items were added to comm history view
*/
void CntHistoryView::updateScrollingPosition()
{
    int rowCnt = mHistoryModel->rowCount();
    mHistoryListView->scrollTo(mHistoryModel->index(rowCnt - 1, 0), 
        HbAbstractItemView::PositionAtBottom);
}
// end of file
