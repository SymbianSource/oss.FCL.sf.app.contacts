/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       xSP order organizer
*
*/


// INCLUDE FILES
#include <coemain.h>
#include <s32file.h>
#include "xSPOrderOrganizer.h"
#include "CxSPViewInfo.h"
#include "MxSPFactory.h"

// CONSTANTS
namespace
	{
	_LIT( KExtOrderFile, "xSPorder.dat" );
	}

// ==================== MEMBER FUNCTIONS ====================

void TxSPOrderUnit::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteUint32L( iId );
	aStream.WriteUint32L( iViewId );
	}

void TxSPOrderUnit::InternalizeL( RReadStream& aStream )
	{
	iId = aStream.ReadUint32L();
	iViewId = aStream.ReadUint32L();
	}	

TInt xSPOrderOrganizer::CreatePrivateFolder( RFs& aSession, 
												TDes& aPath, 
												TDriveNumber aDrive )
	{
	_LIT( KColon, ":" );
	TChar drive;
	TInt err = aSession.DriveToChar( aDrive, drive );							
	if( !err )
		{
		err = aSession.CreatePrivatePath( aDrive );
		if( !err )
			{
			TFileName path;			
			aSession.PrivatePath( path );
			aPath.Zero();
			aPath.Append( drive );
			aPath.Append( KColon );
			aPath.Append( path );														
			}		
		}	
	return err;
	}
	
TInt xSPOrderOrganizer::OrderCheck( RPointerArray<CxSPViewInfo>& aViewIdMap )
	{
	TInt result( KErrNone );		
	TRAP( result, OrderCheckL( aViewIdMap ) );
	if( result )
		{
		// error occured, as a preventive procedure delete the
		// order file, deletion error code is ignored
		DeleteOrderFile();
		}				
	return result;	
	}
	
TInt xSPOrderOrganizer::Reorganize( const RArray<TxSPOrderUnit>& aNewOrder )
	{
	TInt result( KErrNone );
	TRAP( result, WriteOrderL( aNewOrder ) );
	if( result )
		{
		// error occured, as a preventive procedure delete the
		// order file, deletion error code is ignored
		DeleteOrderFile();
		}
	return result;
	}
	
void xSPOrderOrganizer::OrderCheckL( RPointerArray<CxSPViewInfo>& aViewIdMap )
	{
	RArray<TxSPOrderUnit> newOrder;
	CleanupClosePushL( newOrder );
	MakeNewOrderL( aViewIdMap, newOrder );
	WriteOrderL( newOrder );
	RearrangeExtensionsL( aViewIdMap, newOrder );
	CleanupStack::PopAndDestroy(); // newOrder
	}	

void xSPOrderOrganizer::MakeNewOrderL( RPointerArray<CxSPViewInfo>& aViewIdMap, 
											RArray<TxSPOrderUnit>& aNewOrder )
	{
	// read extOrder array from file
	RFs& fs = CCoeEnv::Static()->FsSession();
	TFileName orderFile;
	User::LeaveIfError( CreatePrivateFolder( fs, orderFile, EDriveC ) );
	orderFile.Append( KExtOrderFile );	
	RFileReadStream readStream;
	TInt err = readStream.Open( fs, orderFile, EFileRead );
	if( !err )
		{
		CleanupClosePushL( readStream );
		TUint32 count = readStream.ReadUint32L();
		for( TUint32 i = 0; i < count; i++ )
			{
			TxSPOrderUnit next;
			readStream >> next;
			User::LeaveIfError( aNewOrder.Append( next ));
			}
		CleanupStack::PopAndDestroy(); // readStream
		}
			
	// add new installed extensions to aNewOrder array
	for( TInt i = 0; i < aViewIdMap.Count(); i++ )
		{
		const CxSPViewInfo& viewInfo = *aViewIdMap[i];
		if( viewInfo.TabView() )
			{
			TBool found( EFalse );
			for( TInt j = 0; j < aNewOrder.Count() && !found; j++ )
				{
				if( viewInfo.Id() == aNewOrder[j].iId &&
					viewInfo.OldViewId() == aNewOrder[j].iViewId )
					{					
					found = ETrue;
					}
				}
			if( !found )
				{
				TxSPOrderUnit next;
				next.iId = viewInfo.Id();
				next.iViewId = viewInfo.OldViewId();
				User::LeaveIfError( aNewOrder.Append( next ) );
				}
			}		
		}
		
	// remove uninstalled extensions from aNewOrder array
	for( TInt i = 0; i < aNewOrder.Count();  )
		{
		TxSPOrderUnit next = aNewOrder[i];
		TBool found( EFalse );
		for( TInt j = 0; j < aViewIdMap.Count() && !found; j++ )
			{
			const CxSPViewInfo& viewInfo = *aViewIdMap[j];
			if( viewInfo.Id() == next.iId && viewInfo.OldViewId() == next.iViewId )
				{
				found = ETrue;
				}			
			}
		if( found )
			{
			i++;
			}
		else
			{
			aNewOrder.Remove( i );
			}
		}								
	}
	
void xSPOrderOrganizer::WriteOrderL( const RArray<TxSPOrderUnit>& aNewOrder )
	{	
	RFs& fs = CCoeEnv::Static()->FsSession();
	TFileName orderFile;
	User::LeaveIfError( CreatePrivateFolder( fs, orderFile, EDriveC ) );
	orderFile.Append( KExtOrderFile );	
	// write aNewOrder array to file
	RFileWriteStream writeStream;
	TInt err = writeStream.Replace( fs, orderFile, EFileWrite );
	if( !err )
		{
		CleanupClosePushL( writeStream );
		TUint32 count = aNewOrder.Count();
		writeStream.WriteUint32L( count );
		for( TUint32 i = 0; i < count; i++ )
			{
			writeStream << aNewOrder[i]; 
			}		
		CleanupStack::PopAndDestroy(); // writeStream
		}		
	}
	
void xSPOrderOrganizer::RearrangeExtensionsL( RPointerArray<CxSPViewInfo>& aViewIdMap,
    											const RArray<TxSPOrderUnit>& aNewOrder )
	{
	// organize extension order
	for( TInt i = 0; i < aNewOrder.Count(); i++ )
		{
		TxSPOrderUnit next = aNewOrder[i];
		for( TInt j = 0; j < aViewIdMap.Count(); j++ )
			{
			const CxSPViewInfo* viewInfo = aViewIdMap[j];
			if( next.iId == viewInfo->Id() &&
				next.iViewId == viewInfo->OldViewId() )
				{
				aViewIdMap.Remove( j );
				aViewIdMap.Compress();
				aViewIdMap.InsertL( viewInfo, i );
				}							
			}
		}
	}
	
TInt xSPOrderOrganizer::DeleteOrderFile()
	{
	TInt result( KErrNone );
	TFileName orderFile;
	RFs& fs = CCoeEnv::Static()->FsSession();
	TInt err = CreatePrivateFolder( fs, orderFile, EDriveC );
	if( !err )
		{
		orderFile.Append( KExtOrderFile );
		result = fs.Delete( orderFile );							
		}
	else
		{
		result = err;
		}			
	return result;
	}

// end of file
