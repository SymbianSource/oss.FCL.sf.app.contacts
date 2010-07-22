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
*     Maptile service implementation
*
*/


#include <centralrepository.h> 
#include <locationservicedefines.h>
#include <qvaluespacepublisher.h>
#include <qvaluespacesubscriber.h>
#include <qfile.h>
#include "cntmaptileservice.h"
#include "cntmaptiledblookuptable.h"

// CONSTANTS
// Maptile interface uid
const TUid KUidMapTileInterface = { 0x2002C3A8 };
// Central Repository Key IDs
const TInt KEnableLocationFeature           = 0x1;

const char *CNT_MAPTILE_STATUS_RECEIVER = "/maptilestatuspublisher/name";
const char *CNT_MAPTILE_STATUS_PUBLISHER = "/cntmaptilepublisher";

const char*  CNT_PORTRAIT_MAPTILE_IMAGE = "_Vertical";
const char*  CNT_LANDSCAPE_MAPTILE_IMAGE = "_Contact_Horizontal";

// -----------------------------------------------------------------------------
// CntMapTileService::CntMapTileService()
// Default constructor
// -----------------------------------------------------------------------------
//
CntMapTileService::CntMapTileService()
{
    //publisher
    mPublisher = NULL;
    
    //subscriber
    mSubscriber = new QValueSpaceSubscriber( CNT_MAPTILE_STATUS_RECEIVER );
    //Connect for maptile status change key
    QObject::connect(mSubscriber, SIGNAL(contentsChanged()), this, SLOT(setMaptileStatus()));

}


// -----------------------------------------------------------------------------
// CntMapTileService::~CntMapTileService()
// Destructor
// -----------------------------------------------------------------------------
//
CntMapTileService::~CntMapTileService()
{
    if( mSubscriber )
    {
        delete mSubscriber;
        mSubscriber = NULL;
    }
    
    if( mPublisher )
    {
        delete mPublisher;
        mPublisher = NULL;
    }
}
// -----------------------------------------------------------------------------
// CntMapTileService::isLocationFeatureEnabled()
// Checks whether location feature is enabled or disabled
// -----------------------------------------------------------------------------
//
bool CntMapTileService::isLocationFeatureEnabled()
{ 
    //Create the centrep with uid 0x2002C3A8
    bool enableLocationFeature = false;
    
    CRepository* centralRepository = NULL;  

    TRAPD( err, centralRepository = CRepository::NewL( KUidMapTileInterface ) );

    if ( KErrNone == err )
    {
        TInt repValue;
      
        //Get the Location feature flag 
        TInt ret = centralRepository->Get( KEnableLocationFeature , repValue );
      
        if ( ret == KErrNone && repValue == 1 )
        {
            enableLocationFeature  = true;
        }
           
         delete centralRepository;
    }
           
    return   enableLocationFeature;
    
}


// -----------------------------------------------------------------------------
// CntMapTileService::getMapTileImage()
// Gets the maptile image path associated with a contact.
// -----------------------------------------------------------------------------
//
int CntMapTileService::getMapTileImage( 
                                 int contactId , 
                                 ContactAddressType sourceType, 
                                 QString& imagePath,
                                 Qt::Orientations orientation )    
{
    
    TLookupItem lookupItem;
    
    int addressCount = 0;
    int maptileStatus = MapTileFetchingUnknownError;
    
    //Read the entry from maptile database
    int error = readEntryFromMaptileDataBase( contactId, sourceType, lookupItem, addressCount );
    
    //if entry available returns the file path otherwise NULL. 
    if ( KErrNone == error  )
    {
        maptileStatus = lookupItem.iFetchingStatus;
        
        if( maptileStatus == MapTileFetchingCompleted )
        {
            //Get the image path
            QString imageFile((QChar*)lookupItem.iFilePath.Ptr(),
                    lookupItem.iFilePath.Length());
            if( orientation == Qt::Vertical )
            {
                imageFile.append( CNT_PORTRAIT_MAPTILE_IMAGE );
            }
            else
            { 
                imageFile.append( CNT_LANDSCAPE_MAPTILE_IMAGE );
            }
            imagePath = imageFile;
            
            //Check if File exists
            if ( !QFile::exists( imagePath ) )
            {
                imagePath.clear();
                maptileStatus = MapTileFetchingUnknownError;
            }
        }
        else if( maptileStatus == MapTileFetchingNetworkError ||
                    maptileStatus == MapTileFetchingInProgress )
        {
            //Publish the contact id for maptile processing
           	publishValue( contactId, sourceType, addressCount );
        }

    }
    else if ( KErrNotFound == error )
    {
        //If entry is not found , it will be a newly added entry.
        publishValue( contactId, sourceType, addressCount );
        maptileStatus = MapTileFetchingInProgress;
    }
   
    //Return the maptile status
    return maptileStatus;
}


// -----------------------------------------------------------------------------
// CntMapTileService::setMaptileStatus()
// Emits the maptile status changed event to contact application
// -----------------------------------------------------------------------------
//
void CntMapTileService::setMaptileStatus()
{
    QString imagePath;
    ContactAddressType addressType = AddressPreference;
    
    QStringList subPath = mSubscriber->subPaths();
    QVariant value = mSubscriber->value(subPath.at(0));
    
    //Subscriber Protocol : [contactid-addresstype-maptilestatus]
    QStringList text = value.toString().split("-");
    int id = text.at(0).toInt();
    int status = text.at(2).toInt();
    
    switch( text.at(1).toInt() )
    {
        case ESourceContactsPref:
            addressType = AddressPreference;
            break;
        case ESourceContactsWork:
            addressType = AddressWork;
            break;        
        case ESourceContactsHome:
            addressType = AddressHome;
            break;
        default:
            break;
    }
  
    //Emit the maptile status signal 
    int type = addressType;
    if( mLastViewedContactId == id )
    {
        emit maptileFetchingStatusUpdate( mLastViewedContactId, type , status );
    }
}


// -----------------------------------------------------------------------------
// CntMapTileService::publishValue()
// Publish the the contact id and address for which maptile to be processed.
// -----------------------------------------------------------------------------
//
void CntMapTileService::publishValue( int id, ContactAddressType sourceType, int addressCount )
{
    
    mLastViewedContactId = id;
    int addressType = ESourceInvalid;
    
    switch( sourceType )
    {
        case AddressPreference:
            addressType = ESourceContactsPref;
            break;
        case AddressWork:
            addressType = ESourceContactsWork;
            break;
        case AddressHome:
            addressType = ESourceContactsHome;
            break;
    }
    
    if ( !mPublisher )
    {
        /* Constructs a QValueSpacePublisher that publishes values under path /mypublisher*/
        mPublisher = new QValueSpacePublisher( CNT_MAPTILE_STATUS_PUBLISHER, this);
    }
       
    /* Publisher protocol [appid-addresstype-count] */
    mContactEntryInfo.clear();
    mContactEntryInfo.append( QVariant(id).toString());
    mContactEntryInfo.append( QChar('-') );
    mContactEntryInfo.append( QVariant(addressType).toString());
    mContactEntryInfo.append( QChar('-') );
    mContactEntryInfo.append( QVariant(addressCount).toString() );
    
    mPublisher->setValue("name", mContactEntryInfo.toAscii() );
    mPublisher->sync();
    
}

// -----------------------------------------------------------------------------
// CntMapTileService::readEntryFromMaptileDataBase()
// Read the entry from maptile database
// -----------------------------------------------------------------------------
//
int CntMapTileService::readEntryFromMaptileDataBase( 
            int id, ContactAddressType sourceType, TLookupItem& aLookupItem, int& aNoOfAddress )
{
    
    //Maptile database  instance
    CLookupMapTileDatabase* mapTileDatabase = NULL;

    TRAPD( err, mapTileDatabase = CLookupMapTileDatabase::NewL(
            KMapTileLookupDatabaseName ) );

    if ( KErrNone == err )
    {
        int appId = id;
        TRAP( err, aNoOfAddress = mapTileDatabase->FindNumberOfAddressL(appId) );
       
        if( err != KErrNone )
        {
            aNoOfAddress = 0;
        }
            
        aLookupItem.iUid = id;
        switch( sourceType )
        {
           case AddressPreference:
               aLookupItem.iSource = ESourceContactsPref;
               break;
           case AddressWork:
               aLookupItem.iSource = ESourceContactsWork;
               break;
           case AddressHome:
               aLookupItem.iSource = ESourceContactsHome;
               break;
           default: 
               break;
        }
   
        TRAP( err , mapTileDatabase->FindEntryL( aLookupItem ) );
        
        //delet the database instance
        delete mapTileDatabase;
           
    }
    
    return err;
}

// End of file

