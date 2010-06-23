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

#include <hblistview.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbmenu.h>
#include <hbview.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <xqservicerequest.h>
#include <cnthistorymodel.h>
#include <hbparameterlengthlimiter.h>
#include <hbmainwindow.h>
#include <hbframebackground.h>

#include "cnthistoryviewitem.h"
#include "cntglobal.h"

const char *CNT_HISTORYVIEW_XML = ":/xml/contacts_history.docml";

/*!
Constructor, initialize member variables.
*/
CntHistoryView::CntHistoryView() : 
    mHistoryListView(NULL),
    mHistoryModel(NULL),
    mDocumentLoader(NULL),
    mViewMgr(NULL),
    mContact(NULL)
{
    bool ok = false;
    
    docLoader()->load(CNT_HISTORYVIEW_XML, &ok);
    
    if (ok)
    {
        mView = static_cast<HbView*>(docLoader()->findWidget(QString("view")));
        
        // Create a back key action an set it as the default navigation
        // action once the back key is pressed
        mBackKey = new HbAction(Hb::BackNaviAction, mView);
        mView->setNavigationAction(mBackKey);        
        connect(mBackKey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
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
    mView->deleteLater();
    
    if (mDocumentLoader) {
        delete mDocumentLoader;
        mDocumentLoader = NULL;
    }
    if (mHistoryModel) {
        delete mHistoryModel;
        mHistoryModel = NULL;
    }
    if (mContact) {
        delete mContact;
        mContact = NULL;
    }
}

/*!
 * Deactivate the view
 */
void CntHistoryView::deactivate()
{
}

/**
 * Activate the view
 */
void CntHistoryView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewMgr = aMgr;
    mArgs = aArgs;
    mContact = new QContact(mArgs.value(ESelectedContact).value<QContact>());
    
    // Set history view heading
    HbGroupBox* groupBox = static_cast<HbGroupBox*>(docLoader()->findWidget(QString("groupBox")));
    groupBox->setHeading(hbTrId("txt_phob_subtitle_history_with_1").arg(mContact->displayLabel()));
    
    //construct listview
    mHistoryListView = static_cast<HbListView*>(docLoader()->findWidget(QString("listView")));
    mHistoryListView->setLayoutName("history");
    CntHistoryViewItem *item = new CntHistoryViewItem;
    item->setSecondaryTextRowCount(1, 3);
    item->setGraphicsSize(HbListViewItem::SmallIcon);
    mHistoryListView->setItemPrototype(item); //ownership is taken
    
    // Connect listview items to respective slots
    connect(mHistoryListView, SIGNAL(activated(const QModelIndex &)),
                      this,  SLOT(itemActivated(const QModelIndex &)));
    mHistoryModel = new CntHistoryModel(mContact->localId(),
                                        mViewMgr->contactManager(SYMBIAN_BACKEND));
    mHistoryListView->setModel(mHistoryModel); //ownership is not taken
    
    //start listening to the events amount changing in the model
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(showClearHistoryMenu()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(showClearHistoryMenu()));
    connect(mHistoryModel, SIGNAL(layoutChanged()),
        this, SLOT(updateScrollingPosition()));
    
    // Connect the menu options to respective slots
    mClearHistory = static_cast<HbAction*>(docLoader()->findObject("cnt:clearhistory"));
    connect(mClearHistory, SIGNAL(triggered()), this, SLOT(clearHistory()));
    showClearHistoryMenu();
    
    HbMainWindow* mainWindow = mView->mainWindow();
    connect(mainWindow, SIGNAL(orientationChanged(Qt::Orientation)), 
            this, SLOT(updateScrollingPosition()));
}

/*!
Called after new items are added to or removed from comm history view
*/
void CntHistoryView::updateScrollingPosition()
{
    int rowCnt = mHistoryModel->rowCount();
    
    // Scroll to the last item in the list
    mHistoryListView->scrollTo(mHistoryModel->index(rowCnt - 1, 0), 
        HbAbstractItemView::PositionAtBottom);
}

/*
Query the user for clearing communications history
*/
void CntHistoryView::clearHistory()
{
    // Ask the use if they want to clear the history
    QString name = mContact->displayLabel();
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_clear_communications_history_with_1")).arg(name), this, 
            SLOT(handleClearHistory(HbAction*)), hbTrId("txt_common_button_delete"), hbTrId("txt_common_button_cancel"));
}

/*
Handle the selected action for clearing history
*/
void CntHistoryView::handleClearHistory(HbAction *action)
{
    HbMessageBox *note = static_cast<HbMessageBox*>(sender());
    
    if (note && action == note->actions().first())
    {
        mHistoryModel->clearHistory();
    }
}

/*!
Once list item is pressed on the list view this slot handles the 
emitted signal
 */
void CntHistoryView::itemActivated(const QModelIndex &index)
{
    int flags = index.data(CntHistoryModel::FlagsRole).toInt();
    
    // If the list item is a call log a call is made to that item
    if (flags & CntHistoryModel::CallLog) {
        // Make a call
        QVariant v = index.data(CntHistoryModel::PhoneNumberRole);
        if (!v.isValid())
            return;
        
        QString service("com.nokia.symbian.ICallDial");
        QString type("dial(QString)");
        XQServiceRequest snd(service, type, false);
        snd << v.toString();
        snd.send();
    }
    else if (flags & CntHistoryModel::Message) {
        // Open conversation view
        QVariant v = index.data(CntHistoryModel::PhoneNumberRole);
        if (!v.isValid())
            return;
        
        QString service("com.nokia.services.hbserviceprovider.conversationview");
        QString type("send(QString,qint32,QString)");
        XQServiceRequest snd(service, type, false);       
        snd << v.toString() << mContact->localId() << mContact->displayLabel();
        snd.send();
    }
}

/*!
Go back to previous view
*/
void CntHistoryView::showPreviousView()
{
    QVariant var;
    var.setValue(*mContact);
    mArgs.insert(ESelectedContact, var);
    mViewMgr->back( mArgs );
}

/*!
Show or hide the clear history menu
*/
void CntHistoryView::showClearHistoryMenu()
{
    if (mHistoryModel->rowCount() > 0) {
        mClearHistory->setEnabled(true);
    } else {
        mClearHistory->setEnabled(false);
    }
}

/*!
 * Document loader
 */
HbDocumentLoader* CntHistoryView::docLoader()
{
    if (!mDocumentLoader) {
        mDocumentLoader = new HbDocumentLoader();
    }
    return mDocumentLoader;
}

// end of file
