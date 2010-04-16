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

// maptile database column names
_LIT( NCntColUid, "cntuid" );
_LIT( NCntColFilePath, "filepath" );
_LIT( NColSource, "source" );

// maptile lookup database name
_LIT( KMapTileLookupDatabaseName, "mylocationsmaptilelookup.db" );

// maptile database table name
_LIT( KMapTileLookupTable, "cntmaptilelookuptable" );

_LIT( KLookupDbPath, "c:\\mylocations\\" );

// uid column number
const TInt KColumncntUid = 1;
// source type column number
const TInt KColumnSource = 2;
// source type column number
const TInt KColumnFilePath = 3;


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
    
    // Landmark uid in the landmarks database
    TUint32 iLmId;
    
    // Uid of the Contact
    TUint32 icntUid; 
    
    // File Path
    TFileName iFilePath;
};

/**
 *  Defines uid source type
 */
enum TUidSourceType
{
    /** Uid Source type contacts default/prefered address */
    ESourceContactsPref  = 3,
    /** Uid Source type contacts work address */
    ESourceContactsWork,
    /** Uid Source type contacts home address */
    ESourceContactsHome,
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

private:
    
    // default constructor
    CLookupMapTileDatabase();
    
    // Second phase constructor
    void ConstructL( const TDesC& aLookupTableName );
    
private:
    
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

