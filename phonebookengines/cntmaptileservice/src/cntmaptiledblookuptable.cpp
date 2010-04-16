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
*     Retrieving maptile path from lookup db implementation
*
*/

#include <bautils.h>

#include "cntmaptiledblookuptable.h"

// select all from
_LIT( KSelectAllFrom, "SELECT * FROM " );

// string 'where'
_LIT( KStringWhere, " WHERE " );

// string ' = '
_LIT( KStringEqual, " = " );

// string 'And'
_LIT( KStringAnd, " AND " );


// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::CLookupMapTileDatabase()
// Default constructor.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase::CLookupMapTileDatabase()
{
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::~CLookupMapTileDatabase()
// Destructor.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase::~CLookupMapTileDatabase()
{
    // close the database
	iItemsDatabase.Close();
	
	// close the file session
	iFsSession.Close();
}
 
// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::~CLookupMapTileDatabase()
// Creates an object of this class and pushes to cleanup stack.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase* CLookupMapTileDatabase::NewLC( const TDesC& aLookupTableName )
{
    CLookupMapTileDatabase* self = new (ELeave) CLookupMapTileDatabase;
    CleanupStack::PushL(self);
    self->ConstructL( aLookupTableName );
    return self;
}


// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::NewL()
// Creates an object of this class.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase* CLookupMapTileDatabase::NewL( const TDesC& aLookupTableName )
{
    CLookupMapTileDatabase* self = CLookupMapTileDatabase::NewLC( aLookupTableName );
    CleanupStack::Pop( self );
    return self;
}
 

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::ConstructL()
// 2nd phase contructor.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::ConstructL( const TDesC& aLookupTableName )
{
    User::LeaveIfError( iFsSession.Connect() );
    
    iDbFileName.Copy( KLookupDbPath );
    iDbFileName.Append( aLookupTableName );
    
    iDatabaseExists = EFalse; 

    if( BaflUtils::FileExists( iFsSession, iDbFileName ) )
    {	
        // database exists 
        iDatabaseExists = ETrue; 
    }
}


// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::FindEntryL()
// Finds an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::FindEntryL( TLookupItem& aLookupItem )
{ 
 
    // used to check whether entry available or not
    TBool entryAvailable = EFalse;
  
    if ( iDatabaseExists )
    {
        // Create a query to find the item.
        TFileName queryBuffer;
        queryBuffer.Copy( KSelectAllFrom );
        queryBuffer.Append( KMapTileLookupTable );
        queryBuffer.Append( KStringWhere );
        queryBuffer.Append( NCntColUid );
        queryBuffer.Append( KStringEqual );
        queryBuffer.AppendNum( aLookupItem.icntUid );
        queryBuffer.Append( KStringAnd );
        queryBuffer.Append( NColSource );
        queryBuffer.Append( KStringEqual );
        queryBuffer.AppendNum( aLookupItem.iSource );
        
        TInt ret = iItemsDatabase.Open( iFsSession, iDbFileName );
        
        if( ret != KErrNone )
        {
            //if already opened , close and open again
            iItemsDatabase.Close();          
            User::LeaveIfError( iItemsDatabase.Open( iFsSession, iDbFileName ) );
        }
        User::LeaveIfError( iItemsDatabase.Begin() );
        
        // Create a view of the table with the above query.
        RDbView myView;
        myView.Prepare( iItemsDatabase, TDbQuery( queryBuffer ) );
        CleanupClosePushL( myView );
        myView.EvaluateAll();
        myView.FirstL();
    
        if( myView.AtRow() ) 
        {   
            // Item found. get the details.
            myView.GetL();      
            if( aLookupItem.icntUid == myView.ColUint( KColumncntUid ) )
            {
                aLookupItem.iFilePath.Copy( myView.ColDes16( KColumnFilePath ) );
                entryAvailable = ETrue;
            }      
        } 
    
        CleanupStack::PopAndDestroy( &myView ); // myView
        
        //Close the database
        iItemsDatabase.Close();
    }
    
    //No entry found 
    if( !entryAvailable )
    {
        User::Leave( KErrNotFound );
    }
}


// End of file

