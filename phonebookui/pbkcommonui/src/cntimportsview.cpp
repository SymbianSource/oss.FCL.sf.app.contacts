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
#include "cntglobal.h"
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmenu.h>
#include <hbframebackground.h>
#include <QStandardItemModel>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <hblistview.h>
#include <QTimer>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbprogressbar.h>
#include <QGraphicsLinearLayout>
#include <hbframebackground.h>
#include <hbabstractviewitem.h>
#include <hbextendedlocale.h>
#include <hbparameterlengthlimiter.h>
#include <hblistviewitem.h>
#include <hbstringutil.h>

const char *CNT_IMPORT_UI_XML = ":/xml/contacts_sim.docml";
const int KTimerValue = 1; // used as 1 msec timer for saving ADN contacts from SIM   

CntImportsView::CntImportsView() : mContactSimManagerADN(0),
    mContactSimManagerSDN(0),    
    mFetchRequestADN(0),
    mFetchRequestSDN(0),
    mListView(0),
    mAdnSimUtility(0),
    mSdnSimUtility(0),
    mAdnEntriesPresent(0),
    mImportSimPopup(0),
    mSaveCount(0),
    mFetchIsDone(0),
    mTimerId(0),
    mAdnStorePresent(0),
    mSdnStorePresent(0),
    mSimPresent(0),
    mAdnStoreEntries(0),
    mSdnStoreEntries(0),
    mSimError(0),
    mWaitingForAdnCache(0)
{
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

    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
      
}

void CntImportsView::deactivate()
{
}

   
CntImportsView::~CntImportsView()
{
    mView->deleteLater();
    delete mFetchRequestADN;
    delete mFetchRequestSDN;
    delete mModel;
    delete mAdnSimUtility;
    delete mSdnSimUtility;
}

/*!
Activates a previous view
*/
void CntImportsView::showPreviousView()
{
    CntViewParameters args;
    mViewManager->back(args);
}

/*
Activates a default view
*/
void CntImportsView::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    Q_UNUSED(aArgs);
    //back button
    HbMainWindow* window = mView->mainWindow();
    if (mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
            
    mViewManager = aMgr;  
    mContactSymbianManager = mViewManager->contactManager(SYMBIAN_BACKEND);
   
    // Sim Utility info fetch
    int getSimInfoError(0);
    int storesError(0);
    int getSdnSimInfoError(0);
    
    
    //ADN store
    int error = -1;
    mAdnSimUtility = new CntSimUtility(CntSimUtility::AdnStore, error);
    
    if (error != 0) 
    {
        delete mAdnSimUtility; 
        mAdnSimUtility = 0;
        mSimError = true;
    }
    else
    {
        // check what stores are there
        CntSimUtility::AvailableStores stores = mAdnSimUtility->getAvailableStores(storesError);
        if(!storesError)
        {
            mSimPresent = stores.SimPresent;
            mAdnStorePresent = stores.AdnStorePresent; 
            mSdnStorePresent = stores.SdnStorePresent;
        }
        
        //check there are ADN contacts 
        if (mAdnStorePresent)
        {
            CntSimUtility::SimInfo simInfo = mAdnSimUtility->getSimInfo(getSimInfoError);
            if (!getSimInfoError)
            {
                // sim entries are present
                mAdnStoreEntries = simInfo.usedEntries;
                if (mAdnStoreEntries > 0) 
                { 
                    mAdnEntriesPresent = true;
                }
            }
            else
            {
                if (getSimInfoError == KErrNotReady)
                {
                    //ADN store is not accessible, e.g. because of active FDN
                    //Or ADN cache is not ready yet, so wait for this  
                    mAdnStorePresent = true;
                    mAdnEntriesPresent = true;
                    mWaitingForAdnCache = true;
                    connect(mAdnSimUtility, SIGNAL(adnCacheStatusReady(CntSimUtility::CacheStatus&, int)), this, SLOT(adnCacheStatusReady(CntSimUtility::CacheStatus&, int)));
                    if (!mAdnSimUtility->notifyAdnCacheStatus()) {
                        mAdnStorePresent = false;
                        mAdnEntriesPresent = false;
                        mWaitingForAdnCache = false;
                    }
                }
                else
                {
                    simInfoErrorMessage(getSimInfoError);
                }
            }
        }
    }
    
    //SDN store
     if (mSdnStorePresent && !mWaitingForAdnCache)
     {
        int sdnError = -1;
        mSdnSimUtility = new CntSimUtility(CntSimUtility::SdnStore, sdnError);
        if (sdnError != 0) 
        {
            delete mSdnSimUtility; 
            mSdnSimUtility = 0;
        }
        else
        {
            //get number of SDN contacts 
            CntSimUtility::SimInfo sdnSimInfo = mSdnSimUtility->getSimInfo(getSdnSimInfoError);
            if (!getSdnSimInfoError)
            {
                // sim entries are present
                mSdnStoreEntries = sdnSimInfo.usedEntries;
            }
        }
     }
    // end SDN store
    
    
    
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mListView->setUniformItemSizes(true);
    
    connect(mListView, SIGNAL(activated (const QModelIndex&)),
                                      this,  SLOT(onListViewActivated(const QModelIndex&)));
       
    mModel = new QStandardItemModel();
    
    QStandardItem *importSimItem = new QStandardItem();
    QStringList simList;
    
    QString simImport(hbTrId("txt_phob_dblist_import_from_sim"));
    QString simNoContacts(hbTrId("txt_phob_dblist_import_from_1_val_no_sim_contacts"));
    QString simNoCard(hbTrId("txt_phob_dblist_import_from_ovi_val_no_sim_card"));
    
    // check if SIM card is there 
    if (mSimError)
    {
        simList << simImport;
        importSimItem->setEnabled(false);
    }
    else if (!(mAdnStorePresent || mSdnStorePresent))
    {
        // Both stores are not present
        // disable the QStandardItem
        if (!mSimPresent) 
        {
            simList << simImport << simNoCard;
        }
        else
        {
            simList << simImport << simNoContacts;
        }
        importSimItem->setEnabled(false);
    }
    else if (!(mAdnEntriesPresent || mSdnStorePresent))
    {
        // Sim card is present
        // No ADN entries are there
        // no SDN entries are there
        simList << simImport << simNoContacts;
        importSimItem->setEnabled(false);
    }
    else
    {   
        // SIM card is present
        //ADN entries or SDN entries are there
        simList << simImport;
        int error = 0;
        QDateTime date = mAdnSimUtility->getLastImportTime(error);
        if (error == 0) {
            HbExtendedLocale locale = HbExtendedLocale::system();
            QString dateStr = locale.format(date.date(), r_qtn_date_usual);
            QString dateStrLocaleDigits = HbStringUtil::convertDigits(dateStr); 
            QString dateStrFull = 
                HbParameterLengthLimiter(hbTrId("txt_phob_dblist_import_from_1_val_updated_1")).arg(dateStrLocaleDigits);
            simList << dateStrFull;
        }
    }
    importSimItem->setData(simList, Qt::DisplayRole);
    importSimItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);   
     
    mModel->insertRow(0,importSimItem);
    
    QStandardItem *importDeviceItem = new QStandardItem();
    QStringList deviceList;
    QString deviceImport(hbTrId("txt_phob_dblist_import_from_device"));
    deviceList << deviceImport;
    importDeviceItem->setData(deviceList, Qt::DisplayRole);
    importDeviceItem->setData(HbIcon("qtg_large_data_import"), Qt::DecorationRole);
    importDeviceItem->setEnabled(false);
       
    mModel->insertRow(1,importDeviceItem);
    
    mListView->setModel(mModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
}

void CntImportsView::simInfoErrorMessage(int infoError)
{
    Q_UNUSED(infoError);
    QString errorMessage;
    errorMessage.append(hbTrId("txt_phob_info_sim_card_error"));
    HbNotificationDialog::launchDialog(errorMessage);
    mSimError = true;
}

void CntImportsView::onListViewActivated(const QModelIndex &index)
{
    int row = index.row();
    if ( row == 0 ) // row 0 has "Imports from SIM" 
    {
       if (!startSimImport())
       {
           //show error note
           simInfoErrorMessage(KErrAccessDenied);
       }
       else
       {
           //start popup and progress
           showWaitNote();
       }
    }        
}


bool CntImportsView::startSimImport()
{

    bool started = false;

    delete mFetchRequestADN;
    mContactSimManagerADN = mViewManager->contactManager(SIM_BACKEND_ADN);
    mFetchRequestADN = new QContactFetchRequest;
    mFetchRequestADN->setManager(mContactSimManagerADN);   
    
    delete mFetchRequestSDN;
    mContactSimManagerSDN = mViewManager->contactManager(SIM_BACKEND_SDN);
    mFetchRequestSDN = new QContactFetchRequest;
    mFetchRequestSDN->setManager(mContactSimManagerSDN);        
        
    if (mWaitingForAdnCache)
    {
        //show progress dialog and wait when ADN cache is ready
        started = true;
    }
    else
    {
        if(mAdnStorePresent)
        {
            connect(mFetchRequestADN, SIGNAL(resultsAvailable()), this, SLOT(importFetchResultReceivedADN()));
        }
      
        if(mSdnStorePresent)
        {
            connect(mFetchRequestSDN, SIGNAL(resultsAvailable()), this, SLOT(importFetchResultReceivedSDN()));
        }
        
        if(mAdnEntriesPresent > 0 && mContactSimManagerADN->error() == QContactManager::NoError) 
        {
            // ADN contacts are there, start fetch
            mFetchRequestADN->start();
            started = true;
        }
        else if (mSdnStorePresent && mContactSimManagerSDN->error() == QContactManager::NoError)        
        {
            //start fetch of SDN contacts since ADN contacts are not there
            mFetchRequestSDN->start();
            started = true;
        }
    }
    return started;
}  

void CntImportsView::stopSimImport()
{
    mImportSimPopup->close();
    if (mFetchIsDone)
    {
        // indicates that timer has been started
        killTimer(mTimerId);
    }
    mSaveSimContactsList.clear(); 
    mSaveSimContactsListSDN.clear(); 
    
    showSimImportResults();
    
    if (mFetchRequestADN != NULL)
    {
        mFetchRequestADN->cancel();
    }
    if (mFetchRequestSDN != NULL)
    {
        mFetchRequestSDN->cancel();
    }
    
    // save import time
    int error = 0;
    mAdnSimUtility->setLastImportTime(error);
    
    //update sim import row with last import time
    if (error == 0) {
        QList<QStandardItem*> importSimItems = mModel->takeRow(0);
        QStandardItem* importSimItem = 0;
        if (importSimItems.count() > 0) {
            importSimItem = importSimItems.at(0);
        }
        
        if (importSimItem != 0) {
            QDateTime date = mAdnSimUtility->getLastImportTime(error);
            if (error == 0) {
                QStringList simList;
                QString simImport(hbTrId("txt_phob_dblist_import_from_sim"));
                simList << simImport;
                
                HbExtendedLocale locale = HbExtendedLocale::system();
                QString dateStr = locale.format(date.date(), r_qtn_date_usual);
                QString dateStrLocaleDigits = HbStringUtil::convertDigits(dateStr);
                QString dateStrFull = 
                    HbParameterLengthLimiter(hbTrId("txt_phob_dblist_import_from_1_val_updated_1")).arg(dateStrLocaleDigits);
                simList << dateStrFull;
                
                importSimItem->setData(simList, Qt::DisplayRole);
                importSimItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);
            }
            mModel->insertRow(0, importSimItem);
            mListView->reset();
        }
    }
}

void CntImportsView::importFetchResultReceivedADN()
{
    //save import time
    int error = 0;
    mAdnSimUtility->setLastImportTime(error);

    QList<QContact> simContactsList = mFetchRequestADN->contacts();
    if (simContactsList.isEmpty())
    {
        // No sim contacts, jump to fetching SDN contacts
        fetchSDNContacts();
    }
    else
    {
        // save ADN contacts
    
        int count = simContactsList.count();
        foreach(QContact contact, simContactsList) 
        {
            if (contact.localId() > 0) 
            {
            //delete local id before saving to different storage
               QScopedPointer<QContactId> contactId(new QContactId());
               contactId->setLocalId(0);
               contactId->setManagerUri(QString());
               contact.setId(*contactId);
    
               // custom label contains name information, save it to the first name 
               QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
               if (names.count() > 0) 
               {
                   QContactName name = static_cast<QContactName>(names.at(0));
                   name.setFirstName(name.customLabel());
                   name.setCustomLabel(QString());
                   contact.saveDetail(&name);
               }
                 
               //update phone numbers to contain default subtype
               QList<QContactDetail> numbers = contact.details(QContactPhoneNumber::DefinitionName);
               for (int i = 0; i < numbers.count(); i++) 
               {
                   QContactPhoneNumber number = static_cast<QContactPhoneNumber>(numbers.at(i));
                   number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                   contact.saveDetail(&number);
               }
               
               //remove sync target details, it's read-only. 
                QList<QContactDetail> syncTargets = contact.details(QContactSyncTarget::DefinitionName);
                for (int j = 0; j < syncTargets.count(); j++) 
                {
                    QContactSyncTarget syncTarget = static_cast<QContactSyncTarget>(syncTargets.at(j));
                    contact.removeDetail(&syncTarget);
                }
               
               contact.setType(QContactType::TypeContact);
               setPreferredDetails(&contact);
               mSaveSimContactsList.append(contact);
                
            }
        }
        // save the list synchronously because async cancelling of save request is  
        // not supported in symbian backend at the time of this implementation
        
        if (!(mSaveSimContactsList.isEmpty()))
        {
            // indicates that there is one or more sim contact that has been fetched
            // create a timer and start saving synchronously one by one
            // when cancel is pressed, kill the timer
            
            mTimerId = startTimer(KTimerValue); // starting a 1 msec timer
            mFetchIsDone = true;
            mSaveCount = 0;    
        }
    }
}

void CntImportsView::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (!(mSaveCount >= mSaveSimContactsList.count())) // while mSaveCount is less than or equal to mSaveSimContactsList.count()
    {
        QContact unSavedContact = mSaveSimContactsList.at(mSaveCount);
        mContactSymbianManager->saveContact(&unSavedContact);
        mSaveCount++;
        //use another counter to check number of actual saved if saveContact() fails
    }
    else
    {
        // saving complete
        killTimer(mTimerId);
        mSaveSimContactsList.clear();
        
        // ADN contacts fetching and saving is done
        // Now start SDN fetching and save
        fetchSDNContacts();
    }
}

void CntImportsView::showWaitNote()
{
    mImportSimPopup = new HbDialog();    
    mImportSimPopup->setDismissPolicy(HbPopup::NoDismiss);
    mImportSimPopup->setTimeout(HbPopup::NoTimeout);
    mImportSimPopup->setBackgroundFaded(true);
    mImportSimPopup->setAttribute(Qt::WA_DeleteOnClose, true);
    
    HbLabel *headingLabel = new HbLabel(mImportSimPopup);
    headingLabel->setPlainText(hbTrId("txt_phob_title_import_contacts")); 
    mImportSimPopup->setHeadingWidget(headingLabel);
    
    QGraphicsLinearLayout *containerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);
    
    HbLabel *icon = new HbLabel(mImportSimPopup);
    icon->setIcon(HbIcon("qtg_large_sim"));  
    
    HbLabel *simText= new HbLabel(mImportSimPopup);
    simText->setPlainText(hbTrId("txt_phob_info_importing_contacts_from_sim")); 
    simText->setTextWrapping(Hb::TextWordWrap);
    simText->setElideMode(Qt::ElideNone);
    
    HbProgressBar *progressBar = new HbProgressBar(mImportSimPopup);
    progressBar->setRange(0,0); 
    
    HbPushButton *stopButton = new HbPushButton(mImportSimPopup);
    stopButton->setText(hbTrId("txt_phob_button_cancel"));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopSimImport()));
    
    QGraphicsLinearLayout *containerLayout1 = new QGraphicsLinearLayout(Qt::Horizontal);
    containerLayout1->addItem(icon);
    containerLayout1->addItem(simText);
    
    QGraphicsWidget *containerWidget = new QGraphicsWidget;    
    containerLayout->addItem(containerLayout1);
    containerLayout->addItem(progressBar);
    containerLayout->addItem(stopButton);  
    containerWidget->setLayout(containerLayout);
    mImportSimPopup->setContentWidget(containerWidget);
    mSaveCount = 0;
    mImportSimPopup->open();
}

void CntImportsView::showSimImportResults() const
{   
    QString results = hbTrId("txt_phob_dpophead_ln_contacts_imported").arg(mSaveCount).arg(mAdnStoreEntries + mSdnStoreEntries);
    HbNotificationDialog::launchDialog(results);
}

void CntImportsView::fetchSDNContacts()
{
   if (mSdnStorePresent && mContactSimManagerSDN->error() == QContactManager::NoError)
    {
        mFetchRequestSDN->start();
    }
    else
    {
        // no SDN contacts to fetch
        showSimImportResults();
        mImportSimPopup->close();
        // Importing finished, go back to NamesView
        showPreviousView();
    }
}

void CntImportsView::importFetchResultReceivedSDN()
{
    //save import time
    int error = 0;
    mAdnSimUtility->setLastImportTime(error);

    QList<QContact> simContactsListSDN = mFetchRequestSDN->contacts();
    if (simContactsListSDN.isEmpty())
    {
        //No sdn contacts present
        showSimImportResults();
        mSaveSimContactsListSDN.clear(); 
        mImportSimPopup->close();
        // Importing finished, go back to NamesView
        showPreviousView();
    }
    else
    {
    // SAVE SDN CONTACTS
        int count = simContactsListSDN.count();
        foreach(QContact contact, simContactsListSDN) 
        {
            if (contact.localId() > 0) 
            {
            //delete local id before saving to different storage
               QScopedPointer<QContactId> contactId(new QContactId());
               contactId->setLocalId(0);
               contactId->setManagerUri(QString());
               contact.setId(*contactId);
    
               //custom label contains name information, save it to the first name 
              QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
              if (names.count() > 0) 
              {
                  QContactName name = static_cast<QContactName>(names.at(0));
                  name.setFirstName(name.customLabel());
                  name.setCustomLabel(QString());
                  contact.saveDetail(&name);
              }
                
              //update phone numbers to contain default subtype
              QList<QContactDetail> numbers = contact.details(QContactPhoneNumber::DefinitionName);
              for (int i = 0; i < numbers.count(); i++) 
              {
                  QContactPhoneNumber number = static_cast<QContactPhoneNumber>(numbers.at(i));
                  number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                  contact.saveDetail(&number);
              }
              
              //remove sync target details, it's read-only. 
              QList<QContactDetail> syncTargets = contact.details(QContactSyncTarget::DefinitionName);
              for (int j = 0; j < syncTargets.count(); j++) 
              {
                  QContactSyncTarget syncTarget = static_cast<QContactSyncTarget>(syncTargets.at(j));
                  contact.removeDetail(&syncTarget);
              }
              
               contact.setType(QContactType::TypeContact);
               setPreferredDetails(&contact);
               mSaveSimContactsListSDN.append(contact);
                
            }
        }
        // save the list synchronously because async cancelling of save request is  
        // not supported in symbian backend at the time of this implementation
        
        if (!(mSaveSimContactsListSDN.isEmpty()))
        {
            // indicates that there is one or more SDN sim contact that has been fetched
            QMap<int, QContactManager::Error> errorMap;
            mContactSymbianManager->saveContacts(&mSaveSimContactsListSDN,&errorMap);
            // check number of contacts really saved
            mSaveCount = mSaveCount + mSaveSimContactsListSDN.count();
        }
        
        // no more SDN contacts to fetch
        showSimImportResults();
        mSaveSimContactsListSDN.clear(); 
        mImportSimPopup->close();
        // Importing finished, go back to NamesView
        showPreviousView();
    }   
}

void CntImportsView::adnCacheStatusReady(CntSimUtility::CacheStatus& cacheStatus, int error)
{
    mWaitingForAdnCache = false;
    //update ADN store info...
    if (error != 0 || cacheStatus == CntSimUtility::ECacheFailed)
    {
        mAdnStorePresent = false;
        mAdnEntriesPresent = false;
    }
    else
    {
        //check if there are ADN contacts
        mAdnStorePresent = true;
        mAdnEntriesPresent = false;
        int getSimInfoError = -1;
        CntSimUtility::SimInfo simInfo = mAdnSimUtility->getSimInfo(getSimInfoError);
        if (!getSimInfoError)
        {
            // sim entries are present
            mAdnStoreEntries = simInfo.usedEntries;
            if (mAdnStoreEntries > 0) 
            { 
                mAdnEntriesPresent = true;
            }
        }
    }
    
    //and start SIM contacts import, if user tapped "SIM import"
    if(mImportSimPopup != NULL && mImportSimPopup->isActive())
    {
        if (!startSimImport())
        {
            //dismiss wait note
            mImportSimPopup->close();
            //and show error note
            simInfoErrorMessage(KErrAccessDenied);
        }
    }
}

void CntImportsView::setPreferredDetails( QContact *aContact )
{
    QList<QContactPhoneNumber> numberList( aContact->details<QContactPhoneNumber>() );
    //set preferred number for call if there is only one phone number
    if ( aContact->preferredDetail("call").isEmpty() && numberList.count() == 1 )
    {
        aContact->setPreferredDetail( "call", numberList.first() );
    }
    //set preferred number for message if there is only one mobile phone number
    if ( aContact->preferredDetail("message").isEmpty() && numberList.count() == 1 )
    {      
        aContact->setPreferredDetail( "message", numberList.first() );
    }
    //set preferred number for message if there is only one email address
    QList<QContactEmailAddress> emailList( aContact->details<QContactEmailAddress>() );
    if ( aContact->preferredDetail("email").isEmpty() && emailList.count() == 1 )
    {      
        aContact->setPreferredDetail( "email", emailList.first() );
    }
}

// EOF
