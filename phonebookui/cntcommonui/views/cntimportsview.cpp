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
#include "cntimportsview.h"
#include "cntsimengine.h"

#include "cntglobal.h"
#include <cntdebug.h>

#include <hbaction.h>
#include <hbview.h>
#include <hbframebackground.h>
#include <hblistview.h>
#include <hbextendedlocale.h>
#include <hbparameterlengthlimiter.h>
#include <hblistviewitem.h>
#include <hbdevicenotificationdialog.h>
#include <hbprogressdialog.h>

#include <QTimer>
#include <QStandardItemModel>
#include <QCoreApplication>

const char *CNT_IMPORT_UI_XML = ":/xml/contacts_sim.docml";

/*!
Constructor
*/
CntImportsView::CntImportsView() :
    mViewManager(NULL),
    mSimEngine(NULL),
    mListView(NULL),
    mView(NULL),
    mSoftkey(NULL),
    mModel(NULL),
    mImportSimPopup(NULL),
    mEngine(NULL)
{
    CNT_ENTRY

    bool ok = false;
    mDocumentLoader.load(CNT_IMPORT_UI_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_sim.docml");
    }
    
    mSimEngine = new CntSimEngine(*this, *mView);
    connect(mSimEngine, SIGNAL(showNamesView()), this, SLOT(showPreviousView()));
    connect(mSimEngine, SIGNAL(closePopup()), this, SLOT(closeImportPopup()));

    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));

    CNT_EXIT
}

/*!
Destructor
*/
CntImportsView::~CntImportsView()
{
    CNT_ENTRY
    
    mView->deleteLater();
    delete mModel;
    delete mSimEngine;
    
    CNT_EXIT
}

/*
Activates a default view
*/
void CntImportsView::activate(const CntViewParameters aArgs)
{
    CNT_ENTRY
   Q_UNUSED(aArgs);
    //back button
          
    if (mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
            
    mViewManager = &mEngine->viewManager();  
    QContactManager *ContactSymbianManager = &mEngine->contactManager(SYMBIAN_BACKEND);
 
    mModel = new QStandardItemModel();

    mSimEngine->init(*mEngine);
    
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mListView->setUniformItemSizes(true);
    
    connect(mListView, SIGNAL(activated (const QModelIndex&)),
                                      this,  SLOT(onListViewActivated(const QModelIndex&)));
       
    
    QStandardItem *importItem = new QStandardItem();
    QStringList simList;
        
    mSimEngine->checkEntries(simList, *importItem);
    
    importItem->setData(simList, Qt::DisplayRole);
    importItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);   
     
    mModel->insertRow(0,importItem);
        
    mListView->setModel(mModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(userCancelsImport()));
    CNT_EXIT
}

void CntImportsView::deactivate()
{
    
}

/*!
Returns the contact manager
*/
QContactManager *CntImportsView::contactSymbianManager()
{
    if (mSimEngine)
    {
        return mSimEngine->contactSymbianManager();
    }
    
    return NULL;
}

/*!
Activates a previous view
*/
void CntImportsView::showPreviousView()
{
    CNT_ENTRY
    
    CntViewParameters args;
    mViewManager->back(args);
    
    CNT_EXIT
}

/*!
Close the import popup
*/
void CntImportsView::closeImportPopup()
{
    CNT_ENTRY
    
    if (mImportSimPopup != NULL)
    {
        mImportSimPopup->close();
        mImportSimPopup = NULL;
    }
    
    CNT_EXIT
}

/*!
Called when list item is activated
*/
void CntImportsView::onListViewActivated(const QModelIndex &index)
{
    CNT_ENTRY
    
    int row = index.row();
    if ( row == 0 ) // row 0 has "Imports from SIM" 
    {
        if (!mSimEngine->startSimImport())
        {
            //show error note
            mSimEngine->simInfoErrorMessage(KErrAccessDenied);
        }
        else
        {
            //start popup and progress
            showWaitNote();
        }
    }

    CNT_EXIT
}

/*!
Called when importing is cancelled
*/
void CntImportsView::userCancelsImport()
{   
    CNT_ENTRY
    
    QString results;
    
    if(mSimEngine->userCancelsImport(results))
    {
        HbDeviceNotificationDialog::notification(QString(),results);
    }
    
    CNT_EXIT
}

/*!
Display the wait note
*/
void CntImportsView::showWaitNote()
{
    CNT_ENTRY
    
    mImportSimPopup = new HbProgressDialog(HbProgressDialog::WaitDialog);    
    mImportSimPopup->setDismissPolicy(HbPopup::NoDismiss);
    mImportSimPopup->setTimeout(HbPopup::NoTimeout);
    mImportSimPopup->setBackgroundFaded(true);
    mImportSimPopup->setAttribute(Qt::WA_DeleteOnClose, true);
    
    mImportSimPopup->setHeadingText(hbTrId("txt_phob_title_import_contacts"));
    mImportSimPopup->setIcon(HbIcon("qtg_large_sim"));
    mImportSimPopup->setText(hbTrId("txt_phob_info_importing_contacts_from_sim"));
    
    connect(mImportSimPopup, SIGNAL(cancelled()), mSimEngine, SLOT(stopSimImport()));

    mImportSimPopup->show();
    
    CNT_EXIT
}

/*!
Set preferred details for the given contact
*/
void CntImportsView::setPreferredDetails( QContact *aContact )
{
    CNT_ENTRY
    
    mSimEngine->setPreferredDetails(*aContact);
    
    CNT_EXIT
}

void CntImportsView::setListBoxItemText(QString& aPrimary, QString& aSecondary)
{
    CNT_ENTRY
    
    QList<QStandardItem*> importItems = mModel->takeRow(0);
    QStandardItem* importItem = NULL;
    
    if (importItems.count() > 0)
    {
        importItem = importItems.at(0);
    }
    
    if (importItem != NULL)
    {
        QStringList simList;
        
        simList << aPrimary;
        simList << aSecondary;
        
        importItem->setData(simList, Qt::DisplayRole);
        importItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);
        mModel->insertRow(0, importItem);
        mListView->reset();
    }
    
    CNT_EXIT
}

void CntImportsView::setListBoxItemEnabled(bool aEnabled)
{
    CNT_ENTRY
    
    QList<QStandardItem*> importItems = mModel->takeRow(0);
    QStandardItem* importItem = NULL;
    
    if ( !importItems.isEmpty() )
    {
       importItem = importItems.first();
    }
    
    if (importItem != NULL)
    {
       importItem->setEnabled(aEnabled);
    }
    
    CNT_EXIT
}

// EOF
