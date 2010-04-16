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


#include "cntmaptileservice.h"
#include "cntmaptiledblookuptable.h"

// CONSTANTS
// Maptile interface uid
const TUid KUidMapTileInterface = { 0x2002C3A8 };
// Central Repository Key IDs
const TInt KEnableLocationFeature           = 0x1;


// -----------------------------------------------------------------------------
// MapTileService::isLocationFeatureEnabled()
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
// MapTileService::getMapTileImage()
// Gets the maptile image path associated with a contact.
// -----------------------------------------------------------------------------
//
QString CntMapTileService::getMapTileImage( int contactId , ContactAddressType sourceType )    
{
    //Image file to return;
   
    //Maptile database  instance
    CLookupMapTileDatabase* mapTileDatabase = NULL;
   
    TRAPD( err, mapTileDatabase = CLookupMapTileDatabase::NewL(
            KMapTileLookupDatabaseName ) );
    if ( KErrNone == err )
    {
        TLookupItem lookupItem;
        lookupItem.icntUid = contactId;
        switch( sourceType )
        {
           case AddressPreference:
               lookupItem.iSource = ESourceContactsPref;
               break;
           case AddressWork:
               lookupItem.iSource = ESourceContactsWork;
               break;
           case AddressHome:
               lookupItem.iSource = ESourceContactsHome;
               break;
           default: 
               break;
        }
       
        TRAP( err , mapTileDatabase->FindEntryL( lookupItem ) );
       
        //delet the database instance
        delete mapTileDatabase;
       
        //if entry available returns the file path otherwise NULL. 
        if ( KErrNone == err  )
        {
           //Get the image path
            QString imageFile((QChar*)lookupItem.iFilePath.Ptr(),
                    lookupItem.iFilePath.Length());
            return imageFile;
        }
    }
    
    return QString();
}

// End of file

