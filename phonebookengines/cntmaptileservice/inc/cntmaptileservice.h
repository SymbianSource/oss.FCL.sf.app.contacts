/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Contact maptile service interface
*
*/

#ifndef _CNTMAPTILESERVICE_H_
#define _CNTMAPTILESERVICE_H_


#include <QString>
#include <QtGlobal>
#include <QObject>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QValueSpacePublisher;
class QValueSpaceSubscriber;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#ifdef  CNTMAPTILESERVICEDLL
#define CNTMAPTILESERVICE_EXPORT Q_DECL_EXPORT
#else
#define CNTMAPTILESERVICE_EXPORT Q_DECL_IMPORT
#endif

//Forward declaration
class TLookupItem;

// CLASS DECLARATION

/**
*  Maptile service class,  provides interface to get map tile image associated with 
*  contact. Also provides interface to check whether location feature is enabled or disabled.
*
*  Note: Location feature can be enabled or disabled by modifying conf\cntmaptileservice.confml file.
*/
class CNTMAPTILESERVICE_EXPORT CntMapTileService : public QObject
{
    Q_OBJECT
public:

    /** 
     * Maptile fetching status
     */
    enum MapTileStatus
    {
        /** Map tile fetching completed */
        MapTileFetchingCompleted = 0,
        /** Map tile fetching in progress */
        MapTileFetchingInProgress,
        /** Map  tile fetching n/w error */
        MapTileFetchingNetworkError,
        /** Map tile fetching invalid address */
        MapTileFetchingInvalidAddress,
        /** Map tile fetching unknown erro */
        MapTileFetchingUnknownError
    };

	
    /** 
     * Contact address types
     */
    enum ContactAddressType      
    {
        /** Address Type Pref */
        AddressPreference,
        /** Address type Home */
        AddressHome,
        /** Address type Work */
        AddressWork
    };
    
    
   /**
    * Default Constructor
    */  
    CntMapTileService();
    
   /**
    * Destructor
    */
    ~CntMapTileService();
    
   /**
    * Checks whether location feature enabled or disabled.
    * 
    * @return Returns true or false based on location feature setting.
    */
    bool isLocationFeatureEnabled();
            
   /**
    * Gets a maptile image associated with a contact id. Returns a maptile 
    * image path if it is available otherwise returns NULL.
    * 
    * @param contactId  Contact id     
    * @param sourceType Source address type( Preferred, Home , Work address )
    * @param imagePath  Maptile image path associated with the contact id
    * @param orientation Application current ui orientation.   
    *      
    * @return Returns the maptile fetching status.
    */
    int getMapTileImage( int Id, 
                         ContactAddressType sourceType, 
                         QString& imagePath,
                         Qt::Orientations orientation = Qt::Vertical );   
    
public slots: 
    /**
     * Receives maptile status information and emits the maptilFetchingStatusUpdate
     * signal.
     */
    void setMaptileStatus();
    
signals:
    /**
     * Signal to update the maptile fetchings status to contact application.
     * @param id  Contact id     
     * @param addressType Source address type( Preferred, Home , Work address )
     * @param status Maptile status for the associated address
     */
     void maptileFetchingStatusUpdate( int id, int addressType, int status );
    

#ifdef CNTMAPTILESERVICE_UNIT_TEST
public:
#else     
private:
#endif  
     
    /**
     * Publishes the contact address information to backend engine
     * @param id  Contact id     
     * @param sourceType Source address type( Preferred, Home , Work address )
     * @param addressCount Number of address associated with this contact
     */ 
     void publishValue( int id, ContactAddressType sourceType, int addressCount );
    
     /**
      * Reads the contact maptile information from maptile database.
      * @param id  Contact id     
      * @param sourceType Source address type( Preferred, Home , Work address )
      * @param aLookupItem Contains the maptile information of a contact entry.
      * @param aNoOfAddress Number of address associated with this contact.
      * 
      * @return Returns zero if successful or error.
      */ 
     int readEntryFromMaptileDataBase( int id, ContactAddressType sourceType,
                          TLookupItem& aLookupItem, int& aNoOfAddress  );

#ifdef CNTMAPTILESERVICE_UNIT_TEST
public:
#else     
private:
#endif  

    //The contact id for which maptile requested
    int mLastViewedContactId;
    //Maptile request publisher
    QValueSpacePublisher *mPublisher;
    //Maptile status request subscriber
    QValueSpaceSubscriber *mSubscriber;
    //Contact information stored as string 
    QString mContactEntryInfo;

};

#endif //_CNTMAPTILESERVICE_H_

