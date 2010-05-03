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

#include "cnthistoryviewitem.h"
#include "qtpbkglobal.h"

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
    mContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());
    
    // Set history view heading
    HbGroupBox* groupBox = static_cast<HbGroupBox*>(docLoader()->findWidget(QString("groupBox")));
    groupBox->setHeading(hbTrId("txt_phob_subtitle_history_with_1").arg(mContact->displayLabel()));
    
    //construct listview
    mHistoryListView = static_cast<HbListView*>(docLoader()->findWidget(QString("listView")));
    CntHistoryViewItem *item = new CntHistoryViewItem;
    mHistoryListView->setItemPrototype(item); //ownership is taken
    mHistoryListView->setUniformItemSizes(true);
    
    // Connect listview items to respective slots
    connect(mHistoryListView, SIGNAL(activated(const QModelIndex &)),
                      this,  SLOT(itemActivated(const QModelIndex &)));
    mHistoryModel = new CntHistoryModel(mContact->localId(),
                                        mViewMgr->contactManager(SYMBIAN_BACKEND));
    mHistoryListView->setModel(mHistoryModel); //ownership is not taken
    
    //start listening to the events amount changing in the model
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(layoutChanged()),
        this, SLOT(updateScrollingPosition()));
    
    // Connect the menu options to respective slots
    HbAction* clearHistory = static_cast<HbAction*>(docLoader()->findObject("cnt:clearhistory"));
    connect(clearHistory, SIGNAL(triggered()), this, SLOT(clearHistory()));
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
Clear communications history
*/
void CntHistoryView::clearHistory()
{
    // Ask the use if they want to clear the history
    HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_clear_communications_history_with_1"),
                HbMessageBox::MessageTypeQuestion);
    
    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        // Clear comm history
        mHistoryModel->clearHistory();
    }
    delete note;
}

/*!
Once list item is pressed on the list view this slot handles the 
emitted signal
 */
void CntHistoryView::itemActivated(const QModelIndex &index)
{
    QVariant itemType = index.data(CntHistoryModel::ItemTypeRole);
    
    if (!itemType.isValid())
        return;
    
    // If the list item is a call log a call is made to that item
    if (itemType.toInt() == CntHistoryModel::CallLog) {
        // Make a call
        QVariant v = index.data(CntHistoryModel::PhoneNumberRole);
        if (!v.isValid())
            return;
        
        QString service("com.nokia.services.telephony");
        QString type("dial(QString)");
        XQServiceRequest snd(service, type, false);
        snd << v.toString();
        snd.send();
    }
}

/*!
Go back to previous view
*/
void CntHistoryView::showPreviousView()
{
    CntViewParameters viewParameters;
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewMgr->back(viewParameters);
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
