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

#ifndef CNTIMPORTSVIEW_H_
#define CNTIMPORTSVIEW_H_

#include <QObject>
#include <hbpushbutton.h>
#include <hblistview.h>

#include "cntactionmenubuilder.h"
#include <hbdocumentloader.h>
#include "cntsimutility.h"

#include <cntabstractview.h>

class HbView;
class HbAction;
class QStandardItemModel;
class HbDialog;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntImportsView : public QObject, public CntAbstractView
{

Q_OBJECT

public slots:

    void onListViewActivated(const QModelIndex &index);
    void stopSimImport();
    void importFetchResultReceivedADN();
    void importFetchResultReceivedSDN();
    void adnCacheStatusReady(CntSimUtility::CacheStatus& cacheStatus, int error);
    void endKeyPressed();
     
public:
CntImportsView();
    ~CntImportsView();

public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return importsView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
	void simInfoErrorMessage(int infoError);
    bool startSimImport();
    void fetchSDNContacts();

protected:
    void timerEvent(QTimerEvent *event);
    
private:
    void showWaitNote();
    void showSimImportResults() const;
    void setPreferredDetails( QContact *aContact );
        
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void showPreviousView();
        
#ifdef PBK_UNIT_TEST
public :
#else
private :
#endif  
   // QContact* mContact;
    CntAbstractViewManager* mViewManager;
	QContactManager *mContactSimManagerADN;
	QContactManager *mContactSimManagerSDN;
	QContactManager *mContactSymbianManager;
    QContactFetchRequest *mFetchRequestADN;
    QContactFetchRequest *mFetchRequestSDN;
    HbListView              *mListView;
    HbDocumentLoader mDocumentLoader;
    HbView* mView; // own
    HbAction* mSoftkey;
	QStandardItemModel* mModel;
    CntSimUtility *mAdnSimUtility;
    CntSimUtility *mSdnSimUtility;
    bool mAdnEntriesPresent;
    HbDialog *mImportSimPopup;
    int mSaveCount;
    QList<QContact> mSaveSimContactsList;
    QList<QContact> mSaveSimContactsListSDN;
    QTimer *mTimer;
    bool mFetchIsDone;
    int mTimerId;
    bool mAdnStorePresent;
    bool mSdnStorePresent;
    bool mSimPresent;
    int mAdnStoreEntries;
    int mSdnStoreEntries;
    bool mSimError;
    bool mWaitingForAdnCache;
    bool mImportInProgress;
    
    CntAbstractEngine* mEngine;
};

#endif /* CNTIMPORTSVIEW_H_ */
