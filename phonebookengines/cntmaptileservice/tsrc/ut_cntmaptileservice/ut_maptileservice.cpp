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
#include <QtTest/QtTest>
#include <QtGui>
#include <QString>
#include <qtcontacts.h>
#include <qcontactmanager.h>


#include "cntmaptileservice.h"


//Maximum maptile processing time
const int KMapTileFetchTime  = 150000;

QTM_USE_NAMESPACE


//Maptile test interface class
class MaptileServiceTest: public QObject
{
    Q_OBJECT
    
private slots:
    
    void checkLocationFeature();
    void getPreferredAddressMapTilePath();
    void getWorkAddressMapTilePath();
    void getHomeAddressMapTilePath();
    void checkInvalidContactId();
 
};


//Checks whether location feature enabled or disabled
void MaptileServiceTest::checkLocationFeature()
{
   
    QVERIFY( CntMapTileService::isLocationFeatureEnabled() == 1 );
}

//Checks the maptile path retrieval for preferred address
void MaptileServiceTest::getPreferredAddressMapTilePath()
{
    
    QContact* contact = new QContact();
    
    //Add contact name   
    QContactName* name = new QContactName();
    name->setFirst("Raj");
    contact->saveDetail( name );
    
    //Add address
    QContactAddress* address = new QContactAddress();
    address->setPostOfficeBox("87640");
    address->setStreet("Torstrasse");
    address->setPostcode("12345");
    address->setLocality("Berlin");
    address->setCountry("Germany");
    contact->saveDetail(address);
    
    //Save the contact
    QContactManager* contactManager = NULL;
    contactManager = new QContactManager("symbian");
    contactManager->saveContact( contact );
    
    //Wait till maptile operation complete
    QTest::qWait( KMapTileFetchTime );
    
   
    //Get the saved id
    QContactId savedId = contact->id();
    TUint32 contactId = savedId.localId();
    
    //Get the maptile 
    QString string = CntMapTileService::getMapTileImage( contactId, CntMapTileService::AddressPreference );
    
    //Construct the QPimap from reference bitmap
    QImage referenceBitmap( "c:\\maptiletest\\preferredaddressmap.png" );
    
    //Construct the QPixmap from new retrieved bitmap
    QImage retrievedBitmap( string );
    
      
    //delete the contact
    contactManager->removeContact( contactId );
    
    delete contact;
    delete name;
    delete address;
    delete contactManager;
    
}

//Checks the maptile path retrieval for work address
void MaptileServiceTest::getWorkAddressMapTilePath()
{
    
    QContact* contact = new QContact();
    
    //Set name
    QContactName* name = new QContactName();
    name->setFirst("Mike");
    contact->saveDetail(name);
    
    //Set address
    QContactAddress* address = new QContactAddress();
    address->setPostOfficeBox("2345");
    address->setPostcode("29834");
    address->setStreet("Domlur");
    address->setLocality("Bangalore");
    address->setCountry("India");
    address->setContexts(QContactDetail::ContextWork);
    contact->saveDetail(address);
    
    //Save the contact
    QContactManager* contactManager = NULL;
    contactManager = new QContactManager("symbian");
    contactManager->saveContact( contact );
      
    //Wait till maptile operation complete
    QTest::qWait( KMapTileFetchTime );
    
     
    //Get the saved id
    QContactId savedId = contact->id();
    TUint32 contactId = savedId.localId();
    
    //Get the maptile 
    QString string = CntMapTileService::getMapTileImage( contactId, CntMapTileService::AddressWork );
    
    //Construct the QPimap from already stored bitmap
    QImage referenceBitmap( "c:\\maptiletest\\workaddressmap.png" );
    
    //Construct the QPixmap from new retrieved bitmap
    QImage retrievedBitmap( string );
    
    //check results are same
    QVERIFY( retrievedBitmap == referenceBitmap );
    
        
    contactManager->removeContact( contactId );
    
    delete contact;
    delete name;
    delete address;
    delete contactManager;
    
}

//Checks the maptile path retrieval for home address
void MaptileServiceTest::getHomeAddressMapTilePath()
{
    
    QContact* contact = new QContact();
        
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    
    QContactAddress* address = new QContactAddress();
    address->setContexts(QContactDetail::ContextHome);
    address->setPostOfficeBox("81282");
    address->setStreet("Keilalahdentie");
    address->setPostcode("67890");
    address->setLocality("Espoo");
    address->setCountry("Finland");
    contact->saveDetail(address);
    
    //Save the contact
    QContactManager* contactManager = NULL;
    contactManager = new QContactManager("symbian");
    contactManager->saveContact( contact );
    
    //Wait till maptile operation complete
    QTest::qWait( KMapTileFetchTime );
    
    //Get the saved id
    QContactId savedId = contact->id();
    TUint32 contactId = savedId.localId();
    
    //Get the maptile 
    QString string = CntMapTileService::getMapTileImage( contactId, CntMapTileService::AddressHome );
    
    //Construct the QPimap from already stored bitmap
    QImage referenceBitmap( "c:\\maptiletest\\homeaddressmap.png" );
    
    //Construct the QPixmap from new retrieved bitmap
    QImage retrievedBitmap( string );
    
    //comapre the bitmaps
    QVERIFY( retrievedBitmap == referenceBitmap );
    
      
    contactManager->removeContact( contactId );
    
    delete contact;
    delete name;
    delete address;
    delete contactManager;
    
}

//Checks the maptile path retrieval returns NULL for invalid address
void  MaptileServiceTest::checkInvalidContactId()
{
    
    QContact* contact = new QContact();
   
    QContactName* name = new QContactName();
    name->setFirst("first");
    contact->saveDetail(name);
    
    //Add some invalid address
    QContactAddress* address = new QContactAddress();
    address->setPostOfficeBox("11111");
    address->setStreet("htrtfdsk");
    address->setPostcode("98989");
    address->setLocality("ghwdxnkwnn");
    address->setCountry("Fbsjwskws");
    contact->saveDetail(address);
    
    //Save the contact
    QContactManager* contactManager = NULL;
    contactManager = new QContactManager("symbian");
    contactManager->saveContact( contact );
    
    //Wait till maptile operation complete
    QTest::qWait( KMapTileFetchTime );
    
     
    //Get the saved id
    QContactId savedId = contact->id();
    TUint32 contactId = savedId.localId();
    
    //Get the maptile 
    QString string = CntMapTileService::getMapTileImage( contactId, CntMapTileService::AddressPreference );
    
    contactManager->removeContact( contactId );
    
    //Maptile path should be NULL for invalid address
    QVERIFY( string.isNull() );
    
    delete contact;
    delete name;
    delete address;
    delete contactManager;
    
  
}


QTEST_MAIN(MaptileServiceTest)
#include "ut_maptileservice.moc"


