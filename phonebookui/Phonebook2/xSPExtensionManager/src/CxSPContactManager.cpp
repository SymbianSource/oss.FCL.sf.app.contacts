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
*       Contact manager.
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <s32mem.h>
#include <CPbk2ViewState.h>
#include <Pbk2ViewId.hrh>
#include "CxSPContactManager.h"
#include "MxSPFactory.h"
#include <MVPbkContactLink.h>
#include <MVPbkStreamable.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>

// CONSTANTS
namespace
	{
	_LIT( KExtContactMapFile, "xSPcontactMap.dat" );
	const TInt KExpandSize = 4; // needed data buffer size
	}

// ==================== MEMBER FUNCTIONS ====================

CxSPPbkContactMap::CxSPPbkContactMap()
	{	
	}
        
CxSPPbkContactMap::~CxSPPbkContactMap()
	{
	delete iPbkContactLink;
	}

void CxSPPbkContactMap::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteUint32L( iId );
	aStream.WriteInt32L( ixSPContactId );
	HBufC8* buffer = CxSPContactManager::LinkToBufferL( iPbkContactLink );
	CleanupStack::PushL( buffer );
	aStream.WriteInt32L( buffer->Length() );
	aStream.WriteL( *buffer, buffer->Length() );
	CleanupStack::PopAndDestroy(); // buffer		
	}

void CxSPPbkContactMap::InternalizeL( RReadStream& aStream,
										const MVPbkContactStoreList& aContactStores )
	{
	iId = aStream.ReadUint32L();
	ixSPContactId = aStream.ReadInt32L();
	TInt length = aStream.ReadInt32L();
	HBufC8* bufLink = HBufC8::NewLC( length );
	TPtr8 ptr = bufLink->Des();
	aStream.ReadL( ptr, length );		
	iPbkContactLink = CxSPContactManager::BufferToLinkL( *bufLink, aContactStores );	
	CleanupStack::PopAndDestroy(); // bufLink	
	}	

CxSPContactManager::CxSPContactManager()
    {
    }
    
void CxSPContactManager::ConstructL( CArrayPtrFlat<MxSPFactory>& aFactories,
										CVPbkContactManager& aVPbkContactManager )
    {
    MVPbkContactStoreList& contactStoreList = aVPbkContactManager.ContactStoresL();    
    RestoreArrayL( KExtContactMapFile, aFactories, contactStoreList );	
	// Register this instance as the contact manager of all xSP factories
	for( TInt i = 0; i < aFactories.Count(); i++ )
		{
		aFactories[i]->RegisterContactManager( *this );
		}
    } 
         
CxSPContactManager::~CxSPContactManager()
    {
    iContactMap.ResetAndDestroy(); 
    iContactMap.Close();
    delete iState;
    }

CxSPContactManager* CxSPContactManager::NewL( CArrayPtrFlat<MxSPFactory>& aFactories,
												CVPbkContactManager& aVPbkContactManager )
    {
    CxSPContactManager* self = new (ELeave) CxSPContactManager;
    CleanupStack::PushL(self);
    self->ConstructL( aFactories, aVPbkContactManager );
    CleanupStack::Pop(self);
    return self;
    }
    
void CxSPContactManager::MapContactL( TUint32 aId, const MVPbkContactLink& aPbkContactLink,
        								TInt32 axSPContactId )
	{
	TInt index = FindMap( aId, aPbkContactLink, axSPContactId );
	if( index == KErrNotFound )
		{
		CxSPPbkContactMap* map = new (ELeave) CxSPPbkContactMap;
		CleanupStack::PushL( map );		
		map->iId = aId;
		map->ixSPContactId = axSPContactId;
		map->iPbkContactLink = aPbkContactLink.CloneLC();
		CleanupStack::Pop(); // map->iPbkContactLink
		User::LeaveIfError( iContactMap.Append( map ) );		
		CleanupStack::Pop(); // map		
		SaveArrayL( KExtContactMapFile );
		}
	}
	
void CxSPContactManager::UnmapContactL( TUint32 aId, const MVPbkContactLink& aPbkContactLink,
        								TInt32 axSPContactId )
	{
	TInt index = FindMap( aId, aPbkContactLink, axSPContactId );
	if( index != KErrNotFound )
		{
		CxSPPbkContactMap* map = iContactMap[index];
		delete map;
		iContactMap.Remove( index );		
		SaveArrayL( KExtContactMapFile );
		}
	}
		
void CxSPContactManager::GetMappedPbkContactsL( TUint32 aId, TInt32 axSPContactId,
        							RPointerArray<MVPbkContactLink>& aPbkContactLinks ) const
	{
	CleanupClosePushL( aPbkContactLinks );
	TInt count = iContactMap.Count();
	for( TInt i = 0; i < count; i++ )
		{
		const CxSPPbkContactMap* map = iContactMap[i];
		if( map->iId == aId && map->ixSPContactId == axSPContactId && map->iPbkContactLink )
			{
			User::LeaveIfError( aPbkContactLinks.Append( map->iPbkContactLink ) );
			}
		}
	CleanupStack::Pop();
	}		

void CxSPContactManager::GetMappedxSPContactsL( TUint32 aId, 
												const MVPbkContactLink& aPbkContactLink,
        										RArray<TInt32>& axSPContactIds ) const
	{
	CleanupClosePushL( axSPContactIds );
	const TInt count = iContactMap.Count();
	for( TInt i = 0; i < count; i++ )
		{
		const CxSPPbkContactMap* map = iContactMap[i];
		if( map->iId == aId && map->iPbkContactLink )
			{
			if( map->iPbkContactLink->IsSame( aPbkContactLink ) )
				{
				User::LeaveIfError( axSPContactIds.Append( map->ixSPContactId ) );
				}
			}			
		}
	CleanupStack::Pop();
	}        										
	
void CxSPContactManager::ViewActivatedL( const TVwsViewId& aPrevViewId,
                         				TUid aCustomMessageId,
                         				const TDesC8& aCustomMessage )
	{
	if( aCustomMessageId == CPbk2ViewState::Uid() && 
    						aPrevViewId.iViewUid.iUid == EPbk2NamesListViewId )
		{
		delete iState;
		iState = NULL;
		iState = CPbk2ViewState::NewL( aCustomMessage );
		}								
	}
			
const CPbk2ViewState* CxSPContactManager::NamesListState() const
	{
	return iState;
	}		  	   
 
TInt CxSPContactManager::CreatePrivateFolder( RFs& aSession, 
											  TDes& aPath, 
											  TDriveNumber aDrive ) const
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
 
TInt CxSPContactManager::FindMap( TUint32 aId, 
									const MVPbkContactLink& aPbkContactLink, 
									TInt32 axSPContactId ) const
	{
	TInt index( KErrNotFound );
	TInt count = iContactMap.Count();
	for( TInt i = 0; i < count && index == KErrNotFound; i++ )
		{
		const CxSPPbkContactMap* map = iContactMap[i];
		if( map->iId == aId && map->ixSPContactId == axSPContactId && map->iPbkContactLink )
			{
			if( map->iPbkContactLink->IsSame( aPbkContactLink ) )
				{
				index = i;
				}
			}			
		}
	return index;
	}
	
void CxSPContactManager::SaveArrayL( const TDesC& aFileName )
	{
	RFs fs; // CSI: 76 #
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	TFileName orderFile;
	User::LeaveIfError( CreatePrivateFolder( fs, orderFile, EDriveC ) );
	orderFile.Append( aFileName );
	RFileWriteStream writeStream;
	TInt err = writeStream.Replace( fs, orderFile, EFileWrite );
	if( !err )
		{
		CleanupClosePushL( writeStream );		
		TUint32 count = iContactMap.Count();
		writeStream.WriteUint32L( count );
		for( TUint32 i = 0; i < count; i++ )
			{
			writeStream << *iContactMap[i];
			}			
		CleanupStack::PopAndDestroy(); // writeStream
		}
	CleanupStack::PopAndDestroy(); // fs
	}
	
void CxSPContactManager::RestoreArrayL( const TDesC& aFileName,
										CArrayPtrFlat<MxSPFactory>& aFactories,
										MVPbkContactStoreList& aContactStoreList )
	{
	// read the array from file
    RFs fs; // CSI: 76 #
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );	
	TFileName orderFile;
	User::LeaveIfError( CreatePrivateFolder( fs, orderFile, EDriveC ) );
	orderFile.Append( aFileName );
	RFileReadStream readStream;
	TInt err = readStream.Open( fs, orderFile, EFileRead );
	if( !err )
		{
		CleanupClosePushL( readStream );
		TUint32 count = readStream.ReadUint32L();
		for( TUint32 i = 0; i < count; i++ )
			{
			CxSPPbkContactMap* map = new (ELeave) CxSPPbkContactMap;
			CleanupStack::PushL( map );
			map->InternalizeL( readStream, aContactStoreList );
			
			// only use mappings from the present xSP extensions
			// removed xSP extension mappings are not used
			TBool found( EFalse );
			for( TInt j = 0; j < aFactories.Count() && !found; j++ )
				{
				if( map->iId == aFactories[j]->Id() )
					{
					found = ETrue;
					}
				}			
			if( found )
				{
				User::LeaveIfError( iContactMap.Append( map ) );
				CleanupStack::Pop(); // map	
				}
			else
				{
				CleanupStack::PopAndDestroy(); // map
				}			
			}
		CleanupStack::PopAndDestroy(); // readStream
		}		
	CleanupStack::PopAndDestroy(); // fs
	}	
	
HBufC8* CxSPContactManager::LinkToBufferL( const MVPbkContactLink* aLink )
	{
	HBufC8* buffer = NULL;	
	if( aLink )
		{
		const MVPbkStreamable* streamable = aLink->Streamable();
		if( streamable )
			{
			CBufFlat* buf = CBufFlat::NewL( KExpandSize );
			CleanupStack::PushL( buf );
			RBufWriteStream writeStream( *buf );
			CleanupClosePushL( writeStream );
			streamable->ExternalizeL( writeStream );
			writeStream.CommitL();
			CleanupStack::PopAndDestroy(); // writeStream				
			TPtr8 ptr = buf->Ptr( 0 );
			buffer = ptr.AllocL();	// CSI: 35 #
			CleanupStack::PopAndDestroy(); // buf			
			}
		}
	if( !buffer )
		{
		buffer = KNullDesC8().AllocL();
		}
	return buffer;
	}
	
MVPbkContactLink* CxSPContactManager::BufferToLinkL( const TDesC8& aBuffer,
									const MVPbkContactStoreList& aContactStores )
	{
	MVPbkContactLink* contactLink = NULL;
	if( aBuffer.Length() )
		{
		CBufFlat* buf = CBufFlat::NewL( KExpandSize );
		CleanupStack::PushL( buf );
		buf->InsertL( 0, aBuffer );
		RBufReadStream readStream( *buf );
		CleanupClosePushL( readStream );	
		CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC( readStream, aContactStores );
		TInt count = array->Count();												
		if( count == 1 ) // should contain only single contact link
			{
			const MVPbkContactLink& link = array->At( 0 );
			contactLink = link.CloneLC();
			CleanupStack::Pop(); // contactLink
			}						
		CleanupStack::PopAndDestroy( 3 ); // array, readStream, buf	
		}
	return contactLink;
	}			
        								
// End of File
