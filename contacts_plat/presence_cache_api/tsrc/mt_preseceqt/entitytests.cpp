/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache reader and writer.
*
*/

#include "entitytests.h"
#include <stdio.h>
#include <qtest.h>
#include <QObject>
#include <prcpresencebuddyinfo_qt.h>
#include <prcpresencereader_qt.h>
#include <prcpresencewriter_qt.h>

    QString EntityTests::K_Buddy = "Buddy";
    QString EntityTests::K_Atext = "Atext";
    QString EntityTests::K_Avalue = "Avalue";
    QString EntityTests::K_Newkey = "Newkey";
    QString EntityTests::K_Newvalue = "Newvalue";
    QString EntityTests::K_Avatar = "Avatar";
    QString EntityTests::K_Statustext =  "Statustext";
    
const int waitTime = 500;
EntityTests::EntityTests(QObject *parent) :
    QObject(parent)
{
   
}

void EntityTests::initTestCase()
{
    iWriter = NULL;
    iReader = NULL;
 
}

void EntityTests::cleanupTestCase()
{
  delete iReader;
  delete iWriter;
}

// -----------------------------------------------------------------------------
// ClientSrvTester::SaveBuddyL
// 
// -----------------------------------------------------------------------------
//
void EntityTests::saveBuddy(QMap<QString,QString>& map)
    {
    PrcPresenceBuddyInfoQt* buddy = createBuddyForWriting(map);
    iWriter->writePresence( *buddy ); 
    }


PrcPresenceBuddyInfoQt* EntityTests::createBuddyForWriting(QMap<QString,QString>& map)
{
    QString buddyId = map.value(K_Buddy);
    QString AText = map.value(K_Atext);
    QString AValue = map.value(K_Avalue); 
    QString avatar = map.value(K_Avatar);
    QString statusText = map.value(K_Statustext);
      
    QString newKey = map.value(K_Newkey);
    QString newValue = map.value(K_Newvalue);
     
    // TInt avValue = TesterAtoi( AValue );
    PrcPresenceBuddyInfoQt::AvailabilityValues intAval = getAvailbility(AText)     ;
    
    if ( !iWriter )
        {
        iWriter = PrcPresenceWriter::createWriter();
        }
        
    PrcPresenceBuddyInfoQt* buddy = PrcPresenceBuddyInfoQt::createInstance();
    
    // Another way to initiate buddy info
    // -------------------------------------
    PrcPresenceBuddyInfoQt* buddy2 = iWriter->newBuddyPresenceInfo();
    delete buddy2;
    buddy2 = NULL;    
    // -------------------------------------
    buddy->setIdentity( buddyId );
    buddy->setAvailability( intAval, AText);

    if ( statusText.length() )
        {
        buddy->setStatusMessage( statusText);
        }
    if ( newKey.length() || newValue.length())
        {
        buddy->setAnyField( newKey, newValue );
        } 
    if(avatar.length())
        {
        buddy->setAvatar(avatar);
        }
    return buddy;
}
PrcPresenceBuddyInfoQt::AvailabilityValues EntityTests::getAvailbility(QString aval)
    {
    if(aval == "available")
        return PrcPresenceBuddyInfoQt::PrcAvailable;
    else if(aval == "busy")
        return PrcPresenceBuddyInfoQt::PrcBusy;
    else if(aval == "notavailable")
           return PrcPresenceBuddyInfoQt::PrcNotAvailable;
      
    else return PrcPresenceBuddyInfoQt::PrcUnknownAvailability;
    
    }
void EntityTests::fetchAndVerifyBuddy( QMap<QString,QString>& map )
    {
   
    QString buddyId = map.value(K_Buddy);
    QString AText = map.value(K_Atext);
    QString AValue = map.value(K_Avalue);;
    QString avatar = map.value(K_Avatar);
    QString statusText = map.value(K_Statustext);
  QString newKey = map.value(K_Newkey);
    QString newValue = map.value(K_Newvalue);

    PrcPresenceBuddyInfoQt::AvailabilityValues intAval = getAvailbility (AText)    ;   
  
    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
    
    PrcPresenceBuddyInfoQt* buddy = iReader->presenceInfo(buddyId);   
    QString testbuddyId = buddy->buddyId();
    QVERIFY(testbuddyId == buddyId);
    
    PrcPresenceBuddyInfoQt::AvailabilityValues testavalibility = buddy->availability();
    QVERIFY (testavalibility == intAval );
    QString testAvalText =  buddy->availabilityText();
    QVERIFY( testAvalText ==AText );
    if ( statusText.length())
        {
        QString testStatusMsg = buddy->statusMessage();
        QVERIFY (testStatusMsg== statusText );
            
        }  
    if ( newKey.length() || newValue.length())
        {
        QString testnewVal = buddy->getAnyField( newKey );
        QVERIFY ( testnewVal== newValue );
        }
    
    if(avatar.length())
    {
    QString atr = buddy->avatar();
    }
    
   }

void  EntityTests::subscribeBuddy( QString& buddyId)
{
    
    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
           
    int error = iReader->subscribePresenceBuddyChange(buddyId);
}
void  EntityTests::unSubscribeBuddy( QString& buddyId)
{
    
    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
           
    iReader->unSubscribePresenceBuddyChange(buddyId);
}
void EntityTests::CheckServices(int nbrServices,int nbrBuddies)
    {
    int  val = 0;    
    
    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
    
    val = iReader->servicesCount();
    QVERIFY ( val == nbrServices );
            
    val = iReader->buddyCountInAllServices();
    QVERIFY( val == nbrBuddies );
            
    }
void  EntityTests::CheckOneService(QString service,int buddiesInService)
    {
    // Services: x Buddies: 0 BuddiesInService: 0 Service: sip

    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
    
    int val = iReader->buddyCountInService( service );
    QVERIFY( val == buddiesInService );
    
    }

void EntityTests::fetchAllBuddiesInService(QString service, int numberofbuddies  )
{

    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
    iNumberOfBuddiesInService = numberofbuddies;
    TInt ret = iReader->allBuddiesPresenceInService(service);
}
// Save and Fetch
void EntityTests::test1()
    {
    QMap<QString,QString> map;
    map.insert(K_Buddy,"sip:tester1@test.com");
    map.insert(K_Atext,"avaifsslable");
    map.insert(K_Avalue,"ONLIsfsNE");
    map.insert(K_Newkey,"x-locfsdfation");
    map.insert(K_Avatar,"caland");
    map.insert(K_Newvalue,"kitfsdschen");
    saveBuddy(map);
    fetchAndVerifyBuddy(map);
    
    
    
    //Some other tests
    PrcPresenceBuddyInfoQt* buddy = iReader->presenceInfo("sip:tester1@test.com"); 
    
    //Equals identity test 
    PrcPresenceBuddyInfoQt* buddy2 = iReader->presenceInfo("sip:tester1@test.com"); 
    bool ret = buddy->equalsIdentity(*buddy2);
    QVERIFY(true == ret);
    QList<QString> qtKeyList;
    buddy->getFieldKeys(qtKeyList);
    int cnt = qtKeyList.count();
    if(cnt > 0)
        {
        QString keyval = qtKeyList.at(0);
        buddy->removeField(keyval);
        }
        
    qtKeyList.clear();
    buddy->getFieldKeys(qtKeyList);
    if(cnt > 0)
        {
        QVERIFY((qtKeyList.count() )== (cnt - 1  )  );
        }
    
    //Change the identity of one buddy
    

    delete buddy;
    delete buddy2;
    
    }
// Save and Subscribe
void EntityTests::test2()
    {
    connect(iReader, SIGNAL(signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* )), 
                       this, SLOT(handlePresenceNotificationInClient(bool , PrcPresenceBuddyInfoQt* )));
      connect(iReader, SIGNAL(signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )), 
                      this, SLOT( handlePresenceReadInClient(bool,QList<PrcPresenceBuddyInfoQt*>)));

    QString testBuddyID = "sip:tester1@test.com";
    iMapDataForAsyncTest.clear();
    iMapDataForAsyncTest.insert(K_Buddy,testBuddyID);
    iMapDataForAsyncTest.insert(K_Atext,"available");
    iMapDataForAsyncTest.insert(K_Avalue,"OFFLINE");
    iMapDataForAsyncTest.insert(K_Newkey,"x-location");
    iMapDataForAsyncTest.insert(K_Newvalue,"garden");
    saveBuddy(iMapDataForAsyncTest);
    
    subscribeBuddy(testBuddyID);
    //change some data
    iMapDataForAsyncTest.remove(K_Avalue);
    iMapDataForAsyncTest.remove(K_Newvalue);
    iMapDataForAsyncTest.insert(K_Avalue,"ON_THE_LINE");
    iMapDataForAsyncTest.insert(K_Newvalue,"atdoor");
    saveBuddy(iMapDataForAsyncTest);
    QTest::qWait(waitTime);

    
    }

//Subscribe and Update partly
void EntityTests::test3()
    {
    connect(iReader, SIGNAL(signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* )), 
                           this, SLOT(handlePresenceNotificationInClient(bool , PrcPresenceBuddyInfoQt* )));
    connect(iReader, SIGNAL(signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )), 
                           this, SLOT( handlePresenceReadInClient(bool,QList<PrcPresenceBuddyInfoQt*>)));
    QString testBuddyID = "sip:tester1@test.com";
    QString testBuddyID2 = "xzsip:abctese@tt.com";
     iNotificationReceived = false;
    //Subscribe
    subscribeBuddy(testBuddyID);
    subscribeBuddy(testBuddyID2); //subscribe also to some dummy buddya
    //Now save and check if notification is received
    iMapDataForAsyncTest.clear();
    iMapDataForAsyncTest.insert(K_Buddy,testBuddyID);
    iMapDataForAsyncTest.insert(K_Atext,"available");
    iMapDataForAsyncTest.insert(K_Avalue,"In Transit");
    iMapDataForAsyncTest.insert(K_Newkey,"Y-location");
    iMapDataForAsyncTest.insert(K_Newvalue,"Gate");
    iMapDataForAsyncTest.insert(K_Statustext,"engaged");
    saveBuddy(iMapDataForAsyncTest);
    QTest::qWait(waitTime);
    QVERIFY(true == iNotificationReceived);
    unSubscribeBuddy(testBuddyID2);
    }

// Subscribe , save, and Unsubscribe ,save
void EntityTests::test4()
    {

    connect(iReader, SIGNAL(signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* )), 
                           this, SLOT(handlePresenceNotificationInClient(bool , PrcPresenceBuddyInfoQt* )));
    connect(iReader, SIGNAL(signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )), 
                          this, SLOT( handlePresenceReadInClient(bool,QList<PrcPresenceBuddyInfoQt*>)));
    QString testBuddyID = "sip:tester1@test.com";
     iNotificationReceived = false;
    //Subscribe
    subscribeBuddy(testBuddyID);
    //Now save and check if notification is received
    iMapDataForAsyncTest.clear();
    iMapDataForAsyncTest.insert(K_Buddy,testBuddyID);
    iMapDataForAsyncTest.insert(K_Atext,"available");
    iMapDataForAsyncTest.insert(K_Avalue,"In Transit");
    iMapDataForAsyncTest.insert(K_Newkey,"Y-location");
    iMapDataForAsyncTest.insert(K_Newvalue,"Gate");
    iMapDataForAsyncTest.insert(K_Statustext,"engaged");
    //Save the data to cache
    saveBuddy(iMapDataForAsyncTest);
    //We should recive notification after save
    QTest::qWait(waitTime);
    QVERIFY(true == iNotificationReceived);
    
    //Unsubscribe 
    unSubscribeBuddy(testBuddyID);
    //change some data and re-save
    iNotificationReceived = false;
    iMapDataForAsyncTest.remove(K_Atext);
    iMapDataForAsyncTest.insert(K_Atext,"UN_available");
    saveBuddy(iMapDataForAsyncTest);
    
    //We should not recive notification since we have unsubscribed
    QTest::qWait(waitTime);
    QVERIFY(false == iNotificationReceived);
    
    
    }
//title Subscribe and delete reader
void EntityTests::test5()
    {

    connect(iReader, SIGNAL(signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* )), 
                           this, SLOT(handlePresenceNotificationInClient(bool , PrcPresenceBuddyInfoQt* )));
    connect(iReader, SIGNAL(signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )), 
                          this, SLOT( handlePresenceReadInClient(bool,QList<PrcPresenceBuddyInfoQt*>)));
    QString testBuddyID = "sip:tester1@test.com";
     iNotificationReceived = false;
    //Subscribe
    subscribeBuddy(testBuddyID);
    //Now save and check if notification is received
    iMapDataForAsyncTest.clear();
    iMapDataForAsyncTest.insert(K_Buddy,testBuddyID);
    iMapDataForAsyncTest.insert(K_Atext,"available");
    iMapDataForAsyncTest.insert(K_Avalue,"In Home ");
    iMapDataForAsyncTest.insert(K_Newkey,"x-location");
    iMapDataForAsyncTest.insert(K_Newvalue,"Restaurant");
    iMapDataForAsyncTest.insert(K_Statustext,"Working hard");
    //Save the data to cache
    saveBuddy(iMapDataForAsyncTest);
    //We should recive notification after save
    QTest::qWait(waitTime);
    QVERIFY(true == iNotificationReceived);
    
    //Unsubscribe 
    if(iReader)
        {
        delete iReader;
        iReader = NULL;
        }
    
    //change some data and re-save
    iNotificationReceived = false;
    iMapDataForAsyncTest.remove(K_Atext);
    iMapDataForAsyncTest.insert(K_Atext,"UN_available");
    saveBuddy(iMapDataForAsyncTest);
    
    //We should not recive notification since we have unsubscribed
    QTest::qWait(waitTime);
    QVERIFY(false == iNotificationReceived);
    
    
    }

// Write multiple presence

void EntityTests::test6()
    {
    connect(iWriter, SIGNAL(signalPresenceWrite(bool)), 
            this, SLOT(handlePresencewriteInclient(bool)));
    QMap<QString,QString> map1;
    map1.insert(K_Buddy,"sip:tester1@test.com");
    map1.insert(K_Atext,"available");
    map1.insert(K_Avalue,"ONLINE");
    map1.insert(K_Newkey,"x-location");
    map1.insert(K_Newvalue,"kitchen");
    
    QMap<QString,QString> map2;
    map2.insert(K_Buddy,"sip:tester2@test.com");
    map2.insert(K_Atext,"huh");
    map2.insert(K_Avalue,"BUSY");
    map2.insert(K_Newkey,"x-location");
    map2.insert(K_Newvalue,"outdoor");
        
    PrcPresenceBuddyInfoQt* buddy1 = createBuddyForWriting(map1);
    PrcPresenceBuddyInfoQt* buddy2 = createBuddyForWriting(map2);
    iNotificationReceived = false;
    QList<PrcPresenceBuddyInfoQt*> multipleBuddy;
    multipleBuddy.append(buddy1);
    multipleBuddy.append(buddy2);
    if ( !iWriter )
        {
        iWriter = PrcPresenceWriter::createWriter();
        }
      
    iWriter->writeMultiplePresence(multipleBuddy);
    QTest::qWait(waitTime);
    QVERIFY(true == iNotificationReceived);
        
    //write and cancel by deleting the writer
    iNotificationReceived = false;
    iWriter->writeMultiplePresence(multipleBuddy);
    iWriter->cancelWrite();
    QTest::qWait(waitTime);
    QVERIFY(false == iNotificationReceived);
        
    }

//Check services count
void EntityTests::test7()
    {
    //Delete previously created services
    if ( !iWriter )
      {
      iWriter = PrcPresenceWriter::createWriter();
      }
    iWriter->deleteService("sip");
    QMap<QString,QString> map1;
    map1.insert(K_Buddy,"sip:tester1@test.com");
    map1.insert(K_Atext,"available");
    map1.insert(K_Avalue,"ONLINE");
    map1.insert(K_Newkey,"x-location");
    map1.insert(K_Newvalue,"kitchen");
    
    QMap<QString,QString> map2;
    map2.insert(K_Buddy,"msn:tester2@test.com");
    map2.insert(K_Atext,"huh");
    map2.insert(K_Avalue,"BUSY");
    map2.insert(K_Newkey,"x-location");
    map2.insert(K_Newvalue,"outdoor");
    
    QMap<QString,QString> map3;
    map3.insert(K_Buddy,"msn:t2@test.com");
    map3.insert(K_Atext,"huh");
    map3.insert(K_Avalue,"BUSY");
    map3.insert(K_Newkey,"x-location");
    map3.insert(K_Newvalue,"outdoor");
    saveBuddy(map1);
    saveBuddy(map2);
    saveBuddy(map3);
    
    CheckServices(2,3);
    CheckOneService( "sip",1);
    CheckOneService("msn",2);
    if ( !iWriter )
          {
          iWriter = PrcPresenceWriter::createWriter();
          }
    bool ret = iWriter->deletePresence("msn:t2@test.com");
    QVERIFY(true == ret);
    ret = iWriter->deletePresence("msn:tester2@test.com");
    QVERIFY(true == ret);
    ret = iWriter->deletePresence("sip:tester1@test.com");
    QVERIFY(true == ret);
    }
void EntityTests::test8()
    {
    connect(iReader, SIGNAL(signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )), 
               this, SLOT(handlePresenceReadInClient(bool , QList<PrcPresenceBuddyInfoQt*> )));

    //Delete previously created services
    if ( !iWriter )
      {
      iWriter = PrcPresenceWriter::createWriter();
      }
    iWriter->deleteService("sip");
    iWriter->deleteService("msn");

    QMap<QString,QString> map1;
    map1.insert(K_Buddy,"yahoo:tester123@test.com");
    map1.insert(K_Atext,"available");
    map1.insert(K_Avalue,"ONLINE");
    map1.insert(K_Newkey,"x-location");
    map1.insert(K_Newvalue,"kitchen");
    
    QMap<QString,QString> map2;
    map2.insert(K_Buddy,"msn:tester232@test.com");
    map2.insert(K_Atext,"huh");
    map2.insert(K_Avalue,"BUSY");
    map2.insert(K_Newkey,"x-location");
    map2.insert(K_Newvalue,"outdoor");
    
    QMap<QString,QString> map3;
    map3.insert(K_Buddy,"yahoo:tester2@test.com");
    map3.insert(K_Atext,"huh");
    map3.insert(K_Avalue,"BUSY");
    map3.insert(K_Newkey,"x-location");
    map3.insert(K_Newvalue,"outdoor");

    QMap<QString,QString> map4;
    map4.insert(K_Buddy,"yahoo:tester322@test.com");
    map4.insert(K_Atext,"huh");
    map4.insert(K_Avalue,"FREE");
    map4.insert(K_Newkey,"x-location");
    map4.insert(K_Newvalue,"kitchen");
    
    saveBuddy(map1);
    saveBuddy(map2);
    saveBuddy(map3);
    saveBuddy(map4);
    iNumberOfBuddiesInService = -1;
    
    iBuddiesInService.append("yahoo:tester123@test.com");
    iBuddiesInService.append("yahoo:tester2@test.com");
    iBuddiesInService.append("yahoo:tester322@test.com");
    iNotificationReceived = false;
    fetchAllBuddiesInService("yahoo",3);
    QTest::qWait(waitTime);
    QVERIFY(true == iNotificationReceived);
    
    //Fetch and cancel by deleting the object
    iNotificationReceived = false;
    fetchAllBuddiesInService("yahoo",3);
    bool ret = iReader->cancelRead();
    QVERIFY(true == ret);
    QTest::qWait(waitTime);
    QVERIFY(false == iNotificationReceived);
    
    //Delete reader and writer
    delete iReader;
    iReader = NULL;
    delete iWriter;
    iWriter = NULL;
    
    
    }
void EntityTests::handlePresenceNotificationInClient(bool success, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo)
    {

    if(true == success)
        {
        iNotificationReceived = true;
        }
    QString buddyId = iMapDataForAsyncTest.value(K_Buddy);
    QString AText = iMapDataForAsyncTest.value(K_Atext);
    QString AValue = iMapDataForAsyncTest.value(K_Avalue);;
    QString avatar = iMapDataForAsyncTest.value(K_Avatar);
    QString statusText = iMapDataForAsyncTest.value(K_Statustext);
  QString newKey = iMapDataForAsyncTest.value(K_Newkey);
    QString newValue = iMapDataForAsyncTest.value(K_Newvalue);

    PrcPresenceBuddyInfoQt::AvailabilityValues intAval = getAvailbility (AText);   
  
    if ( !iReader )
        {
        iReader = PrcPresenceReader::createReader();
        }
    
    
    QString testbuddyId = aPresenceBuddyInfo->buddyId();
    QVERIFY(testbuddyId == buddyId);
    
    PrcPresenceBuddyInfoQt::AvailabilityValues testavalibility = aPresenceBuddyInfo->availability();
    QVERIFY (testavalibility == intAval );
    QString testAvalText =  aPresenceBuddyInfo->availabilityText();
    QVERIFY( testAvalText ==AText );
    if ( statusText.length())
        {
        QString testStatusMsg = aPresenceBuddyInfo->statusMessage();
        QVERIFY (testStatusMsg== statusText );
            
        }  
    if ( newKey.length() || newValue.length())
        {
        QString testnewVal = aPresenceBuddyInfo->getAnyField( newKey );
        QVERIFY ( testnewVal== newValue );
        }
    }

void EntityTests::handlePresenceReadInClient(bool success, QList<PrcPresenceBuddyInfoQt*> buddyInfoList)
{
   int cnt = buddyInfoList.count(); 
   QVERIFY(iNumberOfBuddiesInService == cnt );
   if(true == success)
       {
       iNotificationReceived = true;
       }
   for(int i = 0 ; i< cnt ; i++)
       {
       PrcPresenceBuddyInfoQt* tempbuddy = buddyInfoList.at(i);
       QString identity = tempbuddy->buddyId();
       bool isPresent = iBuddiesInService.contains(identity);
       QVERIFY(true == isPresent);
       iBuddiesInService.removeOne(identity);
       }
    
}
void EntityTests::handlePresencewriteInclient(bool success)   
    {
    if(success == true)
        {
        iNotificationReceived = true;
        }
    
    }

/*!
 * A helper function for the test threads used in EntityTests::test9
 * Basically just creates a presence reader and records wether or not 
 * succesfull
 */   

TInt EntityTests::startupTestThreadFunction(TAny* any)
    {
    // 1. Add cleanup stack support.    
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
     
    // 2. Add support for active objects        
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;          
    CActiveScheduler::Install(activeScheduler);
     
    // 3. create reader, which will cause the server startup (provided it is not running)
    PrcPresenceReader* reader = PrcPresenceReader::createReader(); 
    
    // 4. If reader created correctly, increase provided results counter
    if (reader)
        {
        TInt* count = (TInt*) any;
        (*count)++;
        }
    
    delete reader;
           
    return(KErrNone);
    }

/*!
 * Special server startup test.
 * 
 * 1. This test first waits that the presence server timeouts and shuts down.
 * 2. Then two test threads are created which simultaneously try to create 
 *    presence reader instances.
 * 3. That causes two server instances trying to start at the same time and 
 *    the latter fails with KErrAlreadyExists
 * 4. We check that is handled correctly by checking that neither test thread 
 *    panics and succesfully creates the reader
 */
void EntityTests::test9()
    { 
    // this will hold the count of succesfully created readers
    TInt readerCount=0;
    
    // 1. Wait so the server time outs and shuts down.   
    //    (we want to start fresh for this test)
    User::After(3000000);   
     
    // 2. Create two test threads and start them
    _LIT(KMyThread1, "PresenceTestThread1");
    TBufC<48> threadName(KMyThread1); 
    RThread thread1;
    TRequestStatus thread1Status;

    _LIT(KMyThread2, "PresenceTestThread2");
    TBufC<48> threadName2(KMyThread2);
    RThread thread2;
    TRequestStatus thread2Status;
    
    TInt r=thread1.Create(KMyThread1, 
            EntityTests::startupTestThreadFunction,
            KDefaultStackSize, 
            NULL, 
            &readerCount);  
    
    TInt x=thread2.Create(KMyThread2, 
            EntityTests::startupTestThreadFunction,
            KDefaultStackSize,
            NULL,
            &readerCount);
    
    thread1.Logon(thread1Status); 
    thread2.Logon(thread2Status);   
    thread1.Resume();
    thread2.Resume();  
    
    // 3. When threads are ready, verify results
    User::WaitForRequest(thread1Status);
    User::WaitForRequest(thread2Status);
   
    // verify that neither thread paniced
    QVERIFY(thread1.ExitType() != EExitPanic);
    QVERIFY(thread2.ExitType() != EExitPanic);
    // verify that both succesfully created presence readers
    QVERIFY(readerCount == 2);

    thread1.Close();
    thread2.Close();  
    }
