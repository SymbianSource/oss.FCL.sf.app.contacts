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
#include <hbabstractviewitem.h>
#include <hbmenu.h>
#include <xqservicerequest.h>
#include <mobhistorymodel.h>
#include "cnthistoryviewitem.h" 

const char *CNT_HISTORYVIEW_XML = ":/xml/contacts_history.docml";

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
    CntViewParameters args;//( CntViewParameters::commLauncherView );
    args.setSelectedContact( *mContact );
    viewManager()->back( args );
        /*
    viewManager()->previousViewParameters().setSelectedContact(*mContact);
    viewManager()->onActivatePreviousView();
    */
}

void CntHistoryView::activateView(const CntViewParameters &viewParameters)
{ 
    QContact contact = viewParameters.selectedContact();
    mContact = new QContact(contact);
    
    //group box
    HbGroupBox* groupBox = static_cast<HbGroupBox *>(findWidget(QString("cnt_groupbox_history")));
    QString text = hbTrId("History with %1").arg(contactManager()->synthesizedDisplayLabel(contact));
    groupBox->setHeading(text);
    
    //construct listview
    mHistoryListView = static_cast<HbListView*>(findWidget(QString("cnt_listview_history")));
    
    //bubble graphics - create our custom list view item to have different bubbles
    //for incoming and outgoing messages 
    CntHistoryViewItem *item = new CntHistoryViewItem;
    mHistoryListView->setItemPrototype(item); //ownership is taken
    connect(mHistoryListView, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
                      this,  SLOT(longPressed(HbAbstractViewItem *, const QPointF &)));
    connect(mHistoryListView, SIGNAL(activated(const QModelIndex &)),
                      this,  SLOT(pressed(const QModelIndex &)));
    
    mHistoryModel = new MobHistoryModel(contact.localId(), contactManager());
    mHistoryListView->setModel(mHistoryModel); //ownership is not taken
    
    //start listening to the events amount changing in the model
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
}

/*!
Called after new items are added to or removed from comm history view
*/
void CntHistoryView::updateScrollingPosition()
{
    int rowCnt = mHistoryModel->rowCount();
    mHistoryListView->scrollTo(mHistoryModel->index(rowCnt - 1, 0), 
        HbAbstractItemView::PositionAtBottom);
}

/*!
Add actions to menu
*/
void CntHistoryView::addMenuItems()
{
    CntActions* acts = actions();
    acts->clearActionList();
    acts->actionList() << acts->baseAction("cnt:clearhistory");
    acts->addActionsToMenu(menu());
        
    connect(acts->baseAction("cnt:clearhistory"), SIGNAL(triggered()),
            this, SLOT (clearHistory()));
}

/*
Clear communications history
*/
void CntHistoryView::clearHistory()
{
    mHistoryModel->clearHistory();
}

/*!
Called when a list item is longpressed
*/
void CntHistoryView::longPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    Q_UNUSED(item);
    
    HbMenu* menu = new HbMenu();
    HbAction* clearAction = menu->addAction(hbTrId("txt_phob_menu_clear_history"));
    
    HbAction* selectedAction = menu->exec(coords);
    
    if (selectedAction && selectedAction == clearAction) {
        // Clear comm history
        mHistoryModel->clearHistory();
    }
    
    menu->deleteLater();
}

void CntHistoryView::pressed(const QModelIndex &index)
{
    QVariant itemType = mHistoryModel->data(index, MobHistoryModel::ItemTypeRole);
    
    if (itemType.toInt() == MobHistoryModel::CallLog) {
        // Make a call
        QVariant v = mHistoryModel->data(index, MobHistoryModel::PhoneNumberRole);
        QString service("com.nokia.services.telephony");
        QString type("dial(QString)");
        XQServiceRequest snd(service, type, false);
        snd << v.toString();
        snd.send();
    }
}

// end of file
