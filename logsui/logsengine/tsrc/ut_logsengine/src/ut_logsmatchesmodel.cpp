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
#include "ut_logsmatchesmodel.h"
#include "logsmodel.h"
#include "logsmatchesmodel.h"
#include "logsevent.h"
#include "logscall.h"
#include "logscontact.h"
#include "logsdetailsmodel.h"
#include "logsmessage.h"
#include "logseventdata.h"
#include "qtcontacts_stubs_helper.h"
#include "logscntfinder.h"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(LogsEvent *)
Q_DECLARE_METATYPE(LogsCall *)
Q_DECLARE_METATYPE(LogsDetailsModel *)
Q_DECLARE_METATYPE(LogsMessage *)
Q_DECLARE_METATYPE(LogsContact *)

void UT_LogsMatchesModel::initTestCase()
{
}

void UT_LogsMatchesModel::cleanupTestCase()
{
}


void UT_LogsMatchesModel::init()
{
    mModel = new LogsModel();
    
    mMatchesModel = mModel->logsMatchesModel();
}

void UT_LogsMatchesModel::cleanup()
{
    delete mMatchesModel;
    mMatchesModel = 0;
    delete mModel;
    mModel = 0;
}

void UT_LogsMatchesModel::testConstructor()
{
    QVERIFY( mMatchesModel );
}

void UT_LogsMatchesModel::testRowCount()
{
    QVERIFY( mMatchesModel->rowCount(QModelIndex()) == 0 );
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 0); 
    mMatchesModel->mMatches.append(item);
    QVERIFY( mMatchesModel->rowCount(QModelIndex()) == 1 );
}

void UT_LogsMatchesModel::testData()
{
    QVERIFY( mModel->data(QModelIndex(), Qt::DisplayRole).isNull() );
    
    // Setup search results
    LogsCntEntry* entry = new LogsCntEntry(*this, 0);
    entry->setFirstName("Testing");
    mMatchesModel->mLogsCntFinder->mResults.append(entry);
    quint32 contactId = 2;
    entry = new LogsCntEntry(contactId);
    mMatchesModel->mLogsCntFinder->mResults.append(entry);
    quint32 contactId2 = 3;
    entry = new LogsCntEntry(contactId2);
    mMatchesModel->mLogsCntFinder->mResults.append(entry);
    
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 0); 
    LogsEvent event;
    event.setRemoteParty( "Testing" );
    item->setEvent(event);
    item->mFormattedCallerId = "formattedCallerId";
    mMatchesModel->mMatches.append(item);
    
    QtContactsStubsHelper::setContactId(contactId);
    LogsMatchesModelItemContainer* item2 = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 1); 
    mMatchesModel->mMatches.append(item2);
     
    // Display data for event match
    QVariant displayData = mMatchesModel->data(mMatchesModel->index(0), Qt::DisplayRole);
    QVERIFY ( displayData.type() == QVariant::StringList );
    QStringList list = displayData.toStringList();
    QVERIFY( list.count() == 2 );
    QVERIFY( list.at(0) == "formattedCallerId" );
    
    // Display data for contact match, getting it first time which causes
    // that data is fetched from contact
    QtContactsStubsHelper::setContactNames("first", "last");
    QVERIFY( mMatchesModel->mMatches.at(1)->mContactId == 0 );
    displayData = mMatchesModel->data(mMatchesModel->index(1), Qt::DisplayRole);
    QVERIFY ( displayData.type() == QVariant::StringList );
    list = displayData.toStringList();
    QVERIFY( list.count() == 2 );
    QVERIFY( list.at(0).length() > 0 && list.at(0) != "Testing" );
    QVERIFY( mMatchesModel->mMatches.at(1)->mContactId == contactId );
    
    // Data asked again for same item, optimized
    item2->mContactName = "formattedName";
    item2->mContactNumber = "formattedNumber";
    displayData = mMatchesModel->data(mMatchesModel->index(1), Qt::DisplayRole);
    QVERIFY ( displayData.type() == QVariant::StringList );
    QStringList list2 = displayData.toStringList();
    QVERIFY( list2.count() == 2 );
    QVERIFY( list2.at(0) == "formattedName" );
    QVERIFY( list2.at(1) == "formattedNumber" );
    
    // Decoration data for event match
    QVariant decorationData = mMatchesModel->data(mMatchesModel->index(0), Qt::DecorationRole);
    QVERIFY ( decorationData.canConvert< QList<QVariant> >() );
    
    //create and save contact
    QContact c;
    QVariant ret;	
    QtContactsStubsHelper::setContactId(contactId2);
    LogsMatchesModelItemContainer* item3 = new LogsMatchesModelItemContainer(
                *mModel, *mMatchesModel->mIconManager, 2); 
    item3->setContact(contactId2);
    mMatchesModel->mMatches.append(item3);
    ret = mMatchesModel->data(mMatchesModel->index(2), Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    
    //
    ret = mMatchesModel->data(mMatchesModel->index(2), Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    //	
    ret = mMatchesModel->data(mMatchesModel->index(2), Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
   
    ret = mMatchesModel->data(mMatchesModel->index(2), Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    
    // Details model for logs event
    QVariant details = mMatchesModel->data(mMatchesModel->index(0), LogsMatchesModel::RoleDetailsModel);
    LogsDetailsModel *detailsModel = qVariantValue<LogsDetailsModel *>( details );
    QVERIFY( detailsModel );
    delete detailsModel;
    detailsModel = 0;
    
    //Details model for contact match is null
    details = mMatchesModel->data(mMatchesModel->index(1), LogsMatchesModel::RoleDetailsModel);
    detailsModel = qVariantValue<LogsDetailsModel *>( details );
    QVERIFY( !detailsModel );
    delete detailsModel;
}

void UT_LogsMatchesModel::testDataAdded()
{
    // One event added
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 0); 
    mMatchesModel->mMatches.append(item);
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    mMatchesModel->eventsAdded(QModelIndex(), 0, 0);
    QVERIFY( mMatchesModel->mSearchEvents.count() == 1 );
    QVERIFY( mMatchesModel->mMatches.count() == 0 ); // cleared because of new events
    QVERIFY( mMatchesModel->mPrevSearchPattern.isEmpty() ); // Must be emptied, otherwise requery would not occur
    
    // Two more sequnetial events added
    item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 1); 
    mMatchesModel->mMatches.append(item);
    LogsEvent* event2 = new LogsEvent();
    LogsEvent* event3 = new LogsEvent();
    mModel->mEvents.insert(0, event2);
    mModel->mEvents.insert(0, event3);
    mMatchesModel->eventsAdded(QModelIndex(), 0, 1);
    QVERIFY( mMatchesModel->mSearchEvents.count() == 3 );  
    QVERIFY( mMatchesModel->mMatches.count() == 0 ); // cleared because of new events
    QVERIFY( mMatchesModel->mPrevSearchPattern.isEmpty() ); // Must be emptied, otherwise requery would not occur
}

void UT_LogsMatchesModel::testDataUpdated()
{
    // Nothing to update
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 0); 
    mMatchesModel->mMatches.append(item);
    mMatchesModel->eventsUpdated(mModel->index(0), mModel->index(0));
    QVERIFY( mMatchesModel->mMatches.count() == 1 ); // not cleared because nothing was updated
    
    // Search event to update
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    mMatchesModel->eventsAdded(QModelIndex(), 0, 0);
    mMatchesModel->eventsUpdated(mModel->index(0), mModel->index(0));
    QVERIFY( mMatchesModel->mSearchEvents.count() == 1 );  
    QVERIFY( mMatchesModel->mMatches.count() == 0 ); // cleared because of updated events
    QVERIFY( mMatchesModel->mPrevSearchPattern.isEmpty() ); // Must be emptied, otherwise requery would not occur
}

void UT_LogsMatchesModel::testDataRemoved()
{
    // Nothing to remove
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
            *mModel, *mMatchesModel->mIconManager, 1); 
    mMatchesModel->mMatches.append(item);
    mMatchesModel->eventsRemoved(QModelIndex(), 0, 0);
    QVERIFY( mMatchesModel->mMatches.count() == 1 ); // not cleared because nothing was removed
    
    // Existing search event removed
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    mMatchesModel->eventsAdded(QModelIndex(), 0, 0);
    mMatchesModel->eventsRemoved(QModelIndex(), 0, 0);
    QVERIFY( mMatchesModel->mSearchEvents.count() == 0 ); // cleared because of removed events
    QVERIFY( mMatchesModel->mMatches.count() == 0 ); // cleared because of removed events
    QVERIFY( mMatchesModel->mPrevSearchPattern.isEmpty() ); // Must be emptied, otherwise requery would not occur
}

void UT_LogsMatchesModel::testLogsMatches()
{
    // Query ready when no matching search events
    QSignalSpy spy(mMatchesModel, SIGNAL(modelReset()));
    
    mMatchesModel->logsMatches( "3" );
    mMatchesModel->doSearchQuery(); // simulate async completion
    LogsCntEntry* entry = new LogsCntEntry(*this, 0);
    mMatchesModel->mLogsCntFinder->mResults.append(entry);
    mMatchesModel->queryReady();
    QVERIFY( mMatchesModel->mPrevSearchPattern == "3" );
    QVERIFY( mMatchesModel->mCurrentSearchPattern == "3" );
    QVERIFY( spy.count() == 1 );
    
    // Query ready when matching search events
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    mMatchesModel->eventsAdded(QModelIndex(), 0, 0); // Causes immediate reset
    mMatchesModel->logsMatches( "4" );
    mMatchesModel->doSearchQuery(); // simulate async completion
    mMatchesModel->mLogsCntFinder->mResults.append(
            new LogsCntEntry(*mMatchesModel->mLogsCntFinder->mHistoryEvents.at(0)));
    mMatchesModel->queryReady();
    QVERIFY( spy.count() == 3 );
    QVERIFY( mMatchesModel->mMatches.count() == 1 );
    
    // Query ready with both event and contact matches
    mMatchesModel->logsMatches( "5" );
    mMatchesModel->doSearchQuery(); // simulate async completion
    mMatchesModel->mLogsCntFinder->mResults.append(
            new LogsCntEntry(*mMatchesModel->mLogsCntFinder->mHistoryEvents.at(0)));
    mMatchesModel->mLogsCntFinder->mResults.append( new LogsCntEntry(2) );
    mMatchesModel->queryReady();
    QVERIFY( spy.count() == 4 );
    QVERIFY( mMatchesModel->mMatches.count() == 2 );
    
    // Query ready without any matches
    mMatchesModel->logsMatches( "377778877" );
    mMatchesModel->doSearchQuery(); // simulate async completion
    qDeleteAll(mMatchesModel->mLogsCntFinder->mResults);
    mMatchesModel->mLogsCntFinder->mResults.clear();
    mMatchesModel->queryReady();
    QVERIFY( spy.count() == 5 );
    QVERIFY( mMatchesModel->mMatches.count() == 0 );
    
    // Test rapid queries, only last one should be done once async calls complete
    mMatchesModel->logsMatches( "1" );
    QVERIFY( mMatchesModel->mCurrentSearchPattern == "1" );
    QVERIFY( mMatchesModel->mPrevSearchPattern != "1" );
    mMatchesModel->logsMatches( "12" );
    QVERIFY( mMatchesModel->mCurrentSearchPattern == "12" );
    QVERIFY( mMatchesModel->mPrevSearchPattern != "12" );
    mMatchesModel->logsMatches( "123" );
    QVERIFY( mMatchesModel->mCurrentSearchPattern == "123" );
    QVERIFY( mMatchesModel->mPrevSearchPattern != "123" );
    
    mMatchesModel->doSearchQuery(); // simulate async completion
    QVERIFY( mMatchesModel->mPrevSearchPattern == "123" );
    QVERIFY( mMatchesModel->mLogsCntFinder->mCurrentPredictivePattern == "123" );
    
    mMatchesModel->mLogsCntFinder->mCurrentPredictivePattern.clear(); // Check that same query is not made many times
    mMatchesModel->doSearchQuery(); // simulate async completion
    QVERIFY( mMatchesModel->mPrevSearchPattern == "123" );
    QVERIFY( mMatchesModel->mLogsCntFinder->mCurrentPredictivePattern.isEmpty() );
    
    mMatchesModel->doSearchQuery(); // simulate async completion
    QVERIFY( mMatchesModel->mPrevSearchPattern == "123" );
    QVERIFY( mMatchesModel->mLogsCntFinder->mCurrentPredictivePattern.isEmpty() );
}

void UT_LogsMatchesModel::testCreateCall()
{
    // With event
    LogsMatchesModelItemContainer item(*mModel, *mMatchesModel->mIconManager, 0);    
    LogsEvent event;
    event.setEventType(LogsEvent::TypeVoiceCall);
    event.setNumber( "1234" );
    item.setEvent(event);
    QVariant var = mMatchesModel->createCall(item);
    LogsCall *call = qVariantValue<LogsCall *>( var );
    QVERIFY( call );
    delete call;
    
    // With contact, calling not supported
    LogsMatchesModelItemContainer item2(*mModel, *mMatchesModel->mIconManager, 1);  
    var = mMatchesModel->createCall(item2);
    call = qVariantValue<LogsCall *>( var );
    QVERIFY( !call );
    
    // With contact, calling supported
    LogsMatchesModelItemContainer item3(*mModel, *mMatchesModel->mIconManager, 2);  
    item3.setContact(2);
    var = mMatchesModel->createCall(item3);
    call = qVariantValue<LogsCall *>( var );
    QVERIFY( call );
    delete call;
    
}

void UT_LogsMatchesModel::testCreateMessage()
{
    // With event
    LogsMatchesModelItemContainer item(*mModel, *mMatchesModel->mIconManager, 0);    
    LogsEvent event;
    event.setNumber( "1234" );
    item.setEvent(event);
    QVariant var = mMatchesModel->createMessage(item);
    LogsMessage *message = qVariantValue<LogsMessage *>( var );
    QVERIFY( message );
    delete message;
    
    // With contact, messaging not supported
    LogsMatchesModelItemContainer item2(*mModel, *mMatchesModel->mIconManager, 1);  
    var = mMatchesModel->createMessage(item2);
    message = qVariantValue<LogsMessage *>( var );
    QVERIFY( !message );
    
    // With contact, messaging supported
    LogsMatchesModelItemContainer item3(*mModel, *mMatchesModel->mIconManager, 2);  
    item3.setContact(2);
    var = mMatchesModel->createMessage(item3);
    message = qVariantValue<LogsMessage *>( var );
    QVERIFY( message );
    delete message;
}

void UT_LogsMatchesModel::testCreateContact()
{
    // With event
    LogsMatchesModelItemContainer item(*mModel, *mMatchesModel->mIconManager, 0);    
    LogsEvent event;
    event.setEventType(LogsEvent::TypeVoiceCall);
    event.setNumber( "1234" );
    item.setEvent(event);
    QVariant var = mMatchesModel->createContact(item);
    LogsContact *contact = qVariantValue<LogsContact *>( var );
    QVERIFY( contact );
    delete contact;
    contact = 0;
    
    // With contact
    LogsMatchesModelItemContainer item2(*mModel, *mMatchesModel->mIconManager, 1);  
    item2.setContact(2);
    var = mMatchesModel->createContact(item2);
    contact = qVariantValue<LogsContact *>( var );
    QVERIFY( contact );
    delete contact;
}

void UT_LogsMatchesModel::testUpdateSearchEntry()
{
    // Remote name exists
    LogsEvent event1;
    event1.setRemoteParty("someparty");
    LogsCntEntry entry1(0);
    mMatchesModel->updateSearchEntry(entry1, event1);
    QVERIFY( entry1.firstName().at(0).text() == "someparty" );
    QVERIFY( entry1.phoneNumber().text() == "" );

    // Only number exists
    LogsEvent event2;
    event2.setNumber("55556666");
    LogsCntEntry entry2(0);
    mMatchesModel->updateSearchEntry(entry2, event2);
    QVERIFY( entry2.firstName().at(0).text() == "" );
    QVERIFY( entry2.phoneNumber().text() == "55556666" );

    // Only number starting with '+' exists
    LogsEvent event3;
    event3.setNumber("+77776666");
    LogsCntEntry entry3(0);
    mMatchesModel->updateSearchEntry(entry3, event3);
    QVERIFY( entry3.firstName().at(0).text() == "" );
    QVERIFY( entry3.phoneNumber().text() == "77776666" );

    // VoIP event with CS compatible url
    LogsEvent event4;
    event4.setEventType(LogsEvent::TypeVoIPCall);
    LogsEventData* eventData1 = new LogsEventData();
    eventData1->mRemoteUrl = "444222111";
    event4.setLogsEventData(eventData1);
    LogsCntEntry entry4(0);
    mMatchesModel->updateSearchEntry(entry4, event4);
    QVERIFY( entry4.firstName().at(0).text() == "" );
    QVERIFY( entry4.phoneNumber().text() == "444222111" );

    // VoIP event without CS compatible url
    LogsEvent event5;
    event5.setEventType(LogsEvent::TypeVoIPCall);
    LogsEventData* eventData2 = new LogsEventData();
    eventData2->mRemoteUrl = "testing@test.com";
    event5.setLogsEventData(eventData2);
    LogsCntEntry entry5(0);
    mMatchesModel->updateSearchEntry(entry5, event5);
    QVERIFY( entry5.firstName().at(0).text() == "testing@test.com" );
    QVERIFY( entry5.phoneNumber().text() == "" );
}

void UT_LogsMatchesModel::testGetFormattedCallerId()
{
    // Entry is not initialized, caller Id is empty
    LogsCntEntry entry(0);
    LogsMatchesModelItemContainer item(*mModel, *mMatchesModel->mIconManager, 0); 
    QVERIFY( item.getFormattedCallerId(entry).length() == 0 );
    
    // Entry is initialized, firstname is preffered over other data
    entry.setFirstName("  long firstname");
    entry.setLastName("verylong lastname");
    entry.setPhoneNumber(" number");
    QString callerId = item.getFormattedCallerId(entry);
    QVERIFY( callerId == "long firstname" );
    
    // Firstname is missing, phone number is used
    entry.setFirstName("");
    callerId = item.getFormattedCallerId(entry);
    QVERIFY( callerId == "number" );
}

void UT_LogsMatchesModel::testGetFormattedContactInfo()
{
    QString name;
    QString number;
    
    // Entry is not initialized, name and number are empty
    LogsCntEntry entry(0);
    LogsMatchesModelItemContainer item(*mModel, *mMatchesModel->mIconManager, 0);
    item.getFormattedContactInfo(entry, name, number);
    QVERIFY( name.length() == 0 && number.length() == 0 );
    
    // Entry is initialized, name and number are not empty
    entry.setFirstName("long firstname");
    entry.setLastName("long lastname");
    entry.setPhoneNumber("number");
    item.getFormattedContactInfo(entry, name, number);
    QVERIFY( name == "long firstname long lastname" );
    QVERIFY( number == "number" );
}
