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
*     Retrieving maptile path from lookup db
*
*/

#ifndef __MAPTILEDBLOOKUPTABLE_H__
#define __MAPTILEDBLOOKUPTABLE_H__

//Headers needed
#include <f32file.h> //RFs
#include <d32dbms.h>  //RDbNamedDatabase,RDbView 

#include "cntmaptileservice.h"

// maptile database column names
_LIT( NCntColUid, "cntuid" );
_LIT( NCntColFilePath, "filepath" );
_LIT( NColSource, "source" );
_LIT( NColMaptileStatus, "fetchingstatus" );

// maptile lookup database name
_LIT( KMapTileLookupDatabaseName, "mylocationsmaptilelookup.db" );

// maptile database table name
_LIT( KMapTileLookupTable, "cntmaptilelookuptable" );

_LIT( KLookupDbPath, "c:\\mylocations\\" );

// uid column number
const TInt KColumncntUid = 1;
// source type column number
const TInt KColumnSource = 2;
// maptile image path column number
const TInt KColumnFilePath = 3;
// maptile status column number
const TInt KColumnMapTileFetchingStatus = 4;


/**
 *  Maptile database lookup entry
 */
class TLookupItem                            
{
public:
    // Uid of the source entry
    TUint32 iUid; 
    
    // Source type
    TUint32 iSource;
    
    // File Path
    TFileName iFilePath;
    
    //Map tile fetching status
    TUint32 iFetchingStatus;    
};

/**
 * CLookupMapTileDatabase class.
 * This class handles all the operations related to maptile lookup database.
 *
 */
class CLookupMapTileDatabase : public CBase
{
public:
 
    /**
    * This is a static function, which creates and returns an instance of this class.
    */
    static CLookupMapTileDatabase* NewL( const TDesC& aLookupTableName );

    /**
    * This is a static function, which creates and returns an instance of this class. 
    * Pushes the created object to the cleanup stack.
    */
    static CLookupMapTileDatabase* NewLC( const TDesC& aLookupTableName );

    /**
    * Destructor
    */
    ~CLookupMapTileDatabase();

public:
  
   /**
    * Finds an entry in the lookup table.
    * @param[in/out] aLookupItem The lookup item to be found in the database.
    * The source iUid is passed in the lookup item
    */
    void FindEntryL( TLookupItem& aLookupItem );
    
   /**
    * Finds number of address associated with an contact id.
    * @param[in] aId The contact id .
    * @return Number of address a contact has.
    */
    int FindNumberOfAddressL( int& aId );

#ifdef CNTMAPTILESERVICE_UNIT_TEST
public:
#else     
private:
#endif  
    // default constructor
    CLookupMapTileDatabase();
    
    // Second phase constructor
    void ConstructL( const TDesC& aLookupTableName );

    
#ifdef CNTMAPTILESERVICE_UNIT_TEST
public:
#else     
private:
#endif  
    
    // Handle to the items database
    RDbNamedDatabase iItemsDatabase;
    
    // handle to the file session
    RFs iFsSession;
        
    // holds the database file name
    TFileName iDbFileName;
    
    // holds the info about database existence.
    TBool iDatabaseExists;

};

#endif  // __MAPTILEDBLOOKUPTABLE_H__`

// End of file

