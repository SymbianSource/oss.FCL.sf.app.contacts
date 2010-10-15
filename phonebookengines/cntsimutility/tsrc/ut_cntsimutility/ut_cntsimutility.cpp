/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QObject>
#include <qdebug.h>
#include <qtcontacts.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <mmtsy_names.h>
#include "ut_cntsimutility.h"
#include "cntsimutility.h"

bool  TestCntSimUtility::returnErrorInStub_1 = false;
bool  TestCntSimUtility::returnErrorInStub_2 = false;


TestCntSimUtility::TestCntSimUtility() :
m_SimStore(0)
{

}

TestCntSimUtility::~TestCntSimUtility()
{

}

void TestCntSimUtility::initTestCase()
{

    int error = KErrNone;
    
    // remove all contacts  
}

void TestCntSimUtility::cleanupTestCase()
{

    delete m_SimStore;
    m_SimStore = 0;
}

void TestCntSimUtility::TestStoreCreation()
{
    
}

void TestCntSimUtility::TestGetSimInfo()
{
    // ok case
    TestCntSimUtility::returnErrorInStub_1 = false;
    int error = KErrNotSupported;
    m_SimStore = new CntSimUtility(CntSimUtility::AdnStore, error);
    error = KErrNotSupported;
    CntSimUtility::SimInfo simInformation_adn = m_SimStore->getSimInfo(error);
    QVERIFY(KErrNone == error);
    outputSimInfo(simInformation_adn);
    
    // error case
    TestCntSimUtility::returnErrorInStub_1 = true;
    simInformation_adn = m_SimStore->getSimInfo(error);
    QVERIFY(KErrNone == KErrGeneral);
        
       
}
void TestCntSimUtility::outputSimInfo(CntSimUtility::SimInfo &results)
    {
    return;
    qDebug()<< "SIMUTILITY:Totalentries = "<< results.totalEntries; 
    qDebug()<< "SIMUTILITY:usedEntries = "<< results.usedEntries ;
    qDebug()<< "SIMUTILITY:maxNumLength = "<<        results.maxNumLength;
    qDebug()<< "SIMUTILITY:maxTextLength = "<<        results.maxTextLength;
    qDebug()<< "SIMUTILITY:maxSecondNames = "<<        results.maxSecondNames;
    qDebug()<< "SIMUTILITY:maxTextLengthSecondName = "<<        results.maxTextLengthSecondName ;
    qDebug()<< "SIMUTILITY:maxAdditionalNumbers = "<<        results.maxAdditionalNumbers ;
    qDebug()<< "SIMUTILITY:maxNumLengthAdditionalNumber = "<<        results.maxNumLengthAdditionalNumber;
    qDebug()<< "SIMUTILITY:maxTextLengthAdditionalNumber = "<<        results.maxTextLengthAdditionalNumber;
    qDebug()<< "SIMUTILITY:maxGroupNames = "<<        results.maxGroupNames ;
    qDebug()<< "SIMUTILITY:maxTextLengthGroupName = "<<results.maxTextLengthGroupName ;
    qDebug()<< "SIMUTILITY:maxEmailAddr = "<<results.maxEmailAddr ;
    qDebug()<< "SIMUTILITY:maxTextLengthEmailAddr = "<<results.maxTextLengthEmailAddr;
            
            
      
    }

void TestCntSimUtility::TestGetAvailableStores()
    {
    TestCntSimUtility::returnErrorInStub_1 = false;
    int error1 = KErrNotSupported;
    CntSimUtility::AvailableStores avalStores1 = m_SimStore->getAvailableStores(error1);
    
    
    TestCntSimUtility::returnErrorInStub_1 = true;
    int error2 = KErrNotSupported;
    CntSimUtility::AvailableStores avalStores2 = m_SimStore->getAvailableStores(error2);
    
    QVERIFY(error1 == KErrNone);
    QVERIFY(true == avalStores1.AdnStorePresent);
    QVERIFY(false == avalStores1.SdnStorePresent);
    QVERIFY(true == avalStores1.FdnStorePresent);
    QVERIFY(false == avalStores1.OnStorePresent);
    QVERIFY(avalStores1.SimPresent == true);

    QVERIFY(error2 == KErrNone);
    QVERIFY(false == avalStores2.AdnStorePresent);
    QVERIFY(false == avalStores2.SdnStorePresent);
    QVERIFY(false == avalStores2.FdnStorePresent);
    QVERIFY(false == avalStores2.OnStorePresent);
    QVERIFY(false == avalStores2.SimPresent);
    
    }
void TestCntSimUtility::TestVerifyPin2Code()
    {
    TestCntSimUtility::returnErrorInStub_1 = false;
    bool ret = m_SimStore->verifyPin2Code();
    QVERIFY(true == ret);
    
    }
void TestCntSimUtility::TestIsFdnActive()
    {
    TestCntSimUtility::returnErrorInStub_1 = false;
    bool isFdnActive = m_SimStore->isFdnActive();
    QVERIFY(isFdnActive == true);
   
    //check error case
   TestCntSimUtility::returnErrorInStub_1 = true;
   isFdnActive = m_SimStore->isFdnActive();
   QVERIFY(isFdnActive == false);
    
    }
void TestCntSimUtility::TestSetFdnStatus()
    {
    TestCntSimUtility::returnErrorInStub_1 = false;
    bool fdnActive = m_SimStore->isFdnActive();
    int err = m_SimStore->setFdnStatus(true);
    QVERIFY(err == KErrNone);         
    //error case
    TestCntSimUtility::returnErrorInStub_1 = true;
    err = m_SimStore->setFdnStatus(true);
    QVERIFY(err == KErrGeneral);
        
    }
void TestCntSimUtility::TestGetLastImportTime()
    {
    TestCntSimUtility::returnErrorInStub_1 = false;
    int error1 = KErrNotSupported;
    QDateTime datetime1 = m_SimStore->getLastImportTime(error1);
    
    TestCntSimUtility::returnErrorInStub_1 = true;
    int error2 = KErrNotSupported;
    QDateTime datetime = m_SimStore->getLastImportTime(error2);
    
    QVERIFY(KErrNone == error1);
           
    QVERIFY(KErrAccessDenied == error2);
    
    
          
    }

void TestCntSimUtility::TestSetLastImportTime()
    {
    
    TestCntSimUtility::returnErrorInStub_1 = false;
    int error1 = KErrNotSupported;
    m_SimStore->setLastImportTime(error1);
    TestCntSimUtility::returnErrorInStub_1 = false;
    int error2 = KErrNotSupported;
    m_SimStore->setLastImportTime(error2);

    
    QVERIFY(KErrNone == error1);   
    QVERIFY(KErrAccessDenied == error2);   
        
    }


void TestCntSimUtility::TestStartGetSimInfo()
    {
    connect(m_SimStore, SIGNAL(simInfoReady(CntSimUtility::SimInfo& simInfo, int error)), this, SLOT(simInfoReady(CntSimUtility::SimInfo& simInfo, int error)));
    bool ret = m_SimStore->startGetSimInfo();
    QVERIFY(true == ret);
    }

void TestCntSimUtility::TestStartGetAvailableStores()
    {
    connect(m_SimStore, SIGNAL(availableStoresReady(CntSimUtility::AvailableStores& availableStores, int error)), this, SLOT(availableStoresReady(CntSimUtility::AvailableStores& availableStores, int error)));
    bool ret = m_SimStore->startGetAvailableStores();
    QVERIFY(true == ret);
    }

void TestCntSimUtility::TestNotifyAdnCacheStatus()
    {
    connect(m_SimStore, SIGNAL(adnCacheStatusReady(CntSimUtility::CacheStatus&, int)), this, SLOT(getSimInfoAndUpdateUI(CntSimUtility::CacheStatus&, int)));
    bool ret = m_SimStore->notifyAdnCacheStatus();  
    QVERIFY(true == ret);
    }

    
void TestCntSimUtility::adnCacheStatusReady(CntSimUtility::CacheStatus& cacheStatus, int error)
    {
        
    QVERIFY(cacheStatus == RMmCustomAPI::ECacheReady);
    QVERIFY(KErrNone == error);
    
     }
     
void TestCntSimUtility::simInfoReady(CntSimUtility::SimInfo& simInfo, int error)
    {
    QVERIFY(KErrNone == error);
        
     }
     
void TestCntSimUtility::availableStoresReady(CntSimUtility::AvailableStores& availableStores, int error)
    {
    QVERIFY(KErrNone == error);
        
     }
     
