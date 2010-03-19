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
#include "st_logscntfinder.h"
#include "logscntfinder.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>


void st_LogsCntFinder::initTestCase()
{
}

void st_LogsCntFinder::cleanupTestCase()
{
    
  
}


void st_LogsCntFinder::init()
{   
    m_finder = 0;
    //open symbian database
    m_manager = new QContactManager("symbian");
    int err = 0;
    
    qDebug() << "CContactDatabase::OpenL -> error =  " << err ;
    QVERIFY(err == 0);
    
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = m_manager->contacts();
    qDebug() << "contacts now before delete" << cnt_ids.count();
    
    m_manager->removeContacts(&cnt_ids);
    cnt_ids = m_manager->contacts();
    qDebug() << "contacts now " << cnt_ids.count();
    
    QVERIFY(0 == cnt_ids.count());
    
    m_finder = new LogsCntFinder();
    
}

void st_LogsCntFinder::cleanup()
{
    delete m_manager;
    m_manager = 0;
    delete m_finder;
    m_finder = 0;
}

void st_LogsCntFinder::createContacts_testKeymap()
{
    qDebug() << "st_LogsCntFinder::createContacts_testKeymap";
    /*Create contacts in Contacts DB for keymap testing
		Stefann    Yadira
		Jonn         Ennon
		Maria-Zola     Jones
		Levis         Augustin Zi
		Nancy       Csoma
		Olga          Baranik
		Petter       Harhai
		Queen      Fesko
		Rose        Galisin
		Sasha      Dofzin
		Tisha       Iatzkovits
		Wilda       Lazar
		Una Vivi   Kantsak
		*/
    createContact_one_Contact( QString("Stefann"), QString("Yadira "), QString("932472398") );
    createContact_one_Contact( QString("Jonn"), QString("Ennon"), QString("932472398") );
    createContact_one_Contact( QString("Maria-Zola"), QString("Jones"), QString("932472398") );
    createContact_one_Contact( QString("Levis"), QString("Augustin Zi"), QString("932472398") );
    createContact_one_Contact( QString("Nancy"), QString("Csoma"), QString("932472398") );
    createContact_one_Contact( QString("Olga"), QString("Baranik"), QString("932472398") );
    createContact_one_Contact( QString("Petter"), QString("Harhai"), QString("932472398") );
    createContact_one_Contact( QString("Queen"), QString("Fesko"), QString("932472398") );
    createContact_one_Contact( QString("Rose"), QString("Galisin"), QString("932472398") );
    createContact_one_Contact( QString("Sasha"), QString("Dofzin"), QString("932472398") );
    createContact_one_Contact( QString("Tisha"), QString("Iatzkovits"), QString("932472398") );
    createContact_one_Contact( QString("Wilda"), QString("Lazar"), QString("932472398") );
    createContact_one_Contact( QString("Una Vivi"), QString("Kantsak"), QString("932472398") );
   
    QList<QContactLocalId> cnt_ids = m_manager->contacts();
    cnt_ids = m_manager->contacts();
    int j = cnt_ids.count();
    
    qDebug() << "st_LogsCntFinder::createContacts_testKeymap. created " << j << " contacts";

    
}

void st_LogsCntFinder::createContacts()
{
    qDebug() << "st_LogsCntFinder::createContacts";
    
    createContact_one_Contact( QString("Aarne Adam"), QString("Bravonen"), QString("932472398") );
    createContact_one_Contact( QString("Daavid Faarao"), QString("Heikkinen"), QString("932472398") );
    createContact_one_Contact( QString("Gideon-Gustav"), QString("Jcholainen"), QString("932472398") );
    createContact_one_Contact( QString("Mike Matti"), QString("Kovemberlainen"), QString("932472398") );

    QList<QContactLocalId> cnt_ids = m_manager->contacts();
    cnt_ids = m_manager->contacts();
    int j = cnt_ids.count();
    
    qDebug() << "st_LogsCntFinder::createContacts. created " << j << " contacts";
}

void st_LogsCntFinder::createHistoryEvents()
{
  createLogEvent( QString("Stefann Albert"), QString("Fedrernn"), QString("932472398") );
  createLogEvent( QString("Jonn"), QString("Lennon"), QString("932472398") );
  createLogEvent( QString("Jim-Petter"), QString("Jones"), QString("932472398") );
  
}

void st_LogsCntFinder::createLogEvent(QString firstname, QString Lastname, 
                                      QString phnumber)
{
  LogsCntEntryHandle* dummy = 0;
  
  LogsCntEntry* logEvent = new LogsCntEntry( *dummy, 0 );
  logEvent->setFirstName( firstname );
  logEvent->setLastName( firstname );
  logEvent->setPhoneNumber( phnumber );
  
  m_finder->insertEntry(0, logEvent );
  
  
}


void st_LogsCntFinder::createContact_one_Contact(QString firstname, QString Lastname, 
                                                 QString phnumber)
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    QContact phonecontact;
    // Stefann Fedrernn +02644424423 ste.Fed@nokia.com
    // Contact details
    QContactName contactName;
    contactName.setFirst(firstname);
    contactName.setLast(Lastname);
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber(phnumber);
    phonecontact.saveDetail(&number);
    
    qDebug() << "st_LogsCntFinder::createContact_one_Contact about to save..";
    
    m_manager->saveContact(&phonecontact);
    qDebug() << "st_LogsCntFinder::createContact_one_Contact done";
   
}



//
// Tests
//

// Test basic predictive search, all records with names starting letters "J, K, L" are matched
void st_LogsCntFinder::testPredictiveSearchQuery()
{
    createContacts();
    m_finder->predictiveSearchQuery( QString("524") );
    
    QVERIFY( m_finder->resultsCount() == 1 );
    
    m_finder->predictiveSearchQuery( QString("5") );
    
    QVERIFY( m_finder->resultsCount() == 2 );
  
  
}

/* Test itut keymap predictive search, checking that press key "2", records with names starting letters "A, B, C" are matched;
Press key "3", records with names starting letters "D,E,F" are matched;
Press key "4", records with names starting letters "G,H,I" are matched;
Press key "5", records with names starting letters "J,K,L" are matched;
Press key "6", records with names starting letters "M,O,N" are matched;
Press key "7", records with names starting letters "P,Q,R,S" are matched;
Press key "8", records with names starting letters "T,U,V" are matched;
Press key "9", records with names starting letters "W,X,Y,Z" are matched;
Press key "0", records with names starting letters "space" etc. are matched;
Press key "1", records with names starting letters "-,Ä,Ö" etc. are matched;
*/
void st_LogsCntFinder::testKeymap()
{

   createContacts_testKeymap();
   
    for (int i = 2; i < 10; i++)
    {
        m_finder->predictiveSearchQuery( QString::number(i) );
        switch( i ) 
        {
            case 2:
                QVERIFY( m_finder->resultsCount() == 3 );
                const LogsCntEntry& data = m_finder->resultAt( 0 );
                QVERIFY( data.firstName().count() == 1 );
                //QVERIFY( data.firstName()[0].startsWith());
                //qDebug() << "found " << results << " matches:";
           break;     
           
           case 3:
           	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );
           break;

           case 4:
           			QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

           break;
              
           case 5:
           	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

             break;
             
           case 6:
           	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

             break;
             
           case 7:
           	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

             break;
             
           case 8:
          	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

             break;
             
             
           case 9:
           	    QVERIFY( m_finder->resultsCount() == 3 );
                QVERIFY( data.firstName().count() == 1 );

             break;
						
					 m_finder->predictiveSearchQuery( QString("709") );  
					 QVERIFY( m_finder->resultsCount() == 1 );           
      
	}
    }
	  
}

void st_LogsCntFinder::testPredictiveSearchQueryZero()
{
    createContacts();
    m_finder->predictiveSearchQuery( QString("52404") );
    
    QVERIFY( m_finder->resultsCount() == 1 );
  
  
}

void st_LogsCntFinder::testPredictiveSearchQueryLogs()
{
    createHistoryEvents();
    m_finder->predictiveSearchQuery( QString("5") );
    
    QVERIFY( m_finder->resultsCount() == 2 );
  
  
}



//QTEST_MAIN(st_LogsCntFinder); // on Emulator

int main(int argc, char *argv[]) //on HW
{
    QApplication app(argc, argv);
    
    st_LogsCntFinder st_logscntfinder;
    QString resultFileName = "c:/data/others/st_logscntfinder.txt";
    QStringList args_logsCntFinder( "st_logscntfinder");
    args_logsCntFinder << "-o" << resultFileName;
    QTest::qExec(&st_logscntfinder, args_logsCntFinder);
    return 0;   
}
