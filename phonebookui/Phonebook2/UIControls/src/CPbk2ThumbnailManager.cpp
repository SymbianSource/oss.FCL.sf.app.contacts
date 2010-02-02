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
* Description: Thumbnail manager for the social phonebook.  
*
*/


// class header
#include "CPbk2ThumbnailManager.h"

// external includes 
#include <CPbk2ImageManager.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <TPbk2ImageManagerParams.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <barsread.h>	//tresource reader
#include <gulicon.h>  

//INTERNAL
#include "CPbk2ContactViewListBox.h"
#include "CPbk2IconArray.h"


// Virtual Phonebook
#include <MVPbkBaseContact.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactViewBase.h>

//Pbk2
#include <Pbk2UIControls.rsg>
#include <Pbk2UID.h>

// Default size for thumbnail images
#define KDefaultThumbnailSize TSize(36,36)
// icon offset. Thumbnail indexing starts from 10000 in CPbk2IconArray, so that there is 0 - 9999 indexes free for 
// phonebook's own icons
const TInt KIconIndexOffset = 10000;
// loading limit for thumbnails
const TInt KLoadingLimit = 100;
// loading queue limit
const TInt KQueueLimit = 25;
// Denotes Start Index for an item in the any Queue
const TInt KStartIndex = 0;

/*
 * Helper class for mapping between contact link + image + index on listbox
 */
NONSHARABLE_CLASS( CPbk2TmItem ) : public CBase
	{
public:	// constructor & destructor
	
	// Takes ownership of items
	static CPbk2TmItem* NewL( MVPbkContactLink* aLink, 
							  TInt aListboxIndex );
	~CPbk2TmItem();
	
public:	//new functions
	
	/*
	 * Returns contact link
	 */
	inline MVPbkContactLink* GetLink();
		
	
	/*
	 * Returns corresponding bitmap
	 */
	inline CFbsBitmap* GetBitmap();
		
	/*
	 * Returns index on listbox
	 */
	inline TInt	GetListboxIndex() const;
		
	
	/*
	 * Returns index on Pbk2IconArray
	 */
	inline TInt GetIconArrayIndex() const;
		
	
	/*
	 * Returns icon id
	 */
	inline const TPbk2IconId& GetIconId() const;
		
	/*
	 * Getter for has thumbnail flag
	 */
	inline TBool HasThumbnail();
	
	/*
	 * deletes link and bitmap 
	 */
	inline void DeleteBitmap();
	
	/*
	 * Setter for bitmap, deletes old one if set
	 */
	inline void SetBitmap( CFbsBitmap* aBitmap );
		
	
	/*
	 * Setter for listbox index
	 */
	inline void SetListboxIndex( TInt aIndex );
		
	/*
	 * Setter for Pbk2IconArray index
	 */
	inline void SetIconArrayIndexAndId( TInt aIndex, TPbk2IconId aId );
		
	/*
	 * Setter for has thumbnail flag
	 */
	inline void SetHasThumbnail( TBool aValue );
	
	/*
	 * Compare function for sorting
	 */
	static TInt CompareByListboxIndex( 
			const CPbk2TmItem& aItem1, const CPbk2TmItem& aItem2 );
	
private:	// constructor
	
	CPbk2TmItem( MVPbkContactLink* aLink, 
				 TInt aListboxIndex );
	
private:	//data
	// OWN: contact link
	MVPbkContactLink* 	iLink;
	// OWN: thumbnail
	CFbsBitmap* 		iBitmap;
	// index to listbox
	TInt				iIndexOnListbox;
	// index to icon array
	TInt				iPbkIconArrayIndex;
	// icon id
	TPbk2IconId 		iIconId;
	// informs whether the item has thumbnail image on contact database
	TBool				iHasThumbnail;
	};


// --------------------------------------------------------------------------
// CPbk2TmItem::NewL
// --------------------------------------------------------------------------
//
CPbk2TmItem* CPbk2TmItem::NewL( MVPbkContactLink* aLink, 
								TInt aListboxIndex )
    {
    CPbk2TmItem* self = new( ELeave )CPbk2TmItem( aLink, 
												  aListboxIndex );
    return self;	
    }

// --------------------------------------------------------------------------
// CPbk2TmItem::CPbk2TmItem
// --------------------------------------------------------------------------
//
CPbk2TmItem::CPbk2TmItem( MVPbkContactLink* aLink, 
						  TInt aListboxIndex ):	
    iLink( aLink ),
	iIndexOnListbox( aListboxIndex ),
	iHasThumbnail( EFalse )	
	{
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::~CPbk2TmItem
// --------------------------------------------------------------------------
//
CPbk2TmItem::~CPbk2TmItem()
	{
	delete iLink;
	delete iBitmap;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::CompareByListboxIndex
// --------------------------------------------------------------------------
//
TInt CPbk2TmItem::CompareByListboxIndex( 
		const CPbk2TmItem& aItem1, const CPbk2TmItem& aItem2 )
	{
	TInt i = aItem1.GetListboxIndex();
	TInt j = aItem2.GetListboxIndex();
	return i-j;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::GetLink
// --------------------------------------------------------------------------
//
inline MVPbkContactLink* CPbk2TmItem::GetLink()
	{
	return iLink;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::GetBitmap
// --------------------------------------------------------------------------
//
inline CFbsBitmap*	CPbk2TmItem::GetBitmap()
	{
	return iBitmap;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::GetListboxIndex
// --------------------------------------------------------------------------
//
inline TInt CPbk2TmItem::GetListboxIndex() const
	{
	return iIndexOnListbox;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::GetIconArrayIndex
// --------------------------------------------------------------------------
//
inline TInt CPbk2TmItem::GetIconArrayIndex() const
	{
	return iPbkIconArrayIndex;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::HasThumbnail
// --------------------------------------------------------------------------
//
inline TBool CPbk2TmItem::HasThumbnail()
	{
	return iHasThumbnail;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::GetIconId
// --------------------------------------------------------------------------
//
inline const TPbk2IconId& CPbk2TmItem::GetIconId() const
	{
	return iIconId;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::ClearValues
// --------------------------------------------------------------------------
//
inline void CPbk2TmItem::DeleteBitmap( )
	{
	delete iBitmap;
	iBitmap = NULL;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::SetBitmap
// --------------------------------------------------------------------------
//
inline void CPbk2TmItem::SetBitmap( CFbsBitmap* aBitmap )
	{
	delete iBitmap;
	iBitmap = aBitmap;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::SetListboxIndex
// --------------------------------------------------------------------------
//
inline void CPbk2TmItem::SetListboxIndex( TInt aIndex )
	{
	iIndexOnListbox = aIndex;
	}


// --------------------------------------------------------------------------
// CPbk2TmItem::SetIconArrayIndexAndId
// --------------------------------------------------------------------------
//
inline void CPbk2TmItem::SetIconArrayIndexAndId( TInt aIndex, TPbk2IconId aId )
	{
	iPbkIconArrayIndex = aIndex;
	iIconId = aId;
	}

// --------------------------------------------------------------------------
// CPbk2TmItem::SetHasThumbnail
// --------------------------------------------------------------------------
//
inline void CPbk2TmItem::SetHasThumbnail( TBool aValue )
	{
	iHasThumbnail = aValue;
	}


/*********************************** THUMBNAIL MANAGER ************************************/


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager* CPbk2ThumbnailManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ThumbnailManager* CPbk2ThumbnailManager::NewL( CVPbkContactManager& aContactManager )
   {
   CPbk2ThumbnailManager* self = new (ELeave) CPbk2ThumbnailManager( aContactManager );
   CleanupStack::PushL(self);
   self->ConstructL();
   CleanupStack::Pop();
   return self;
   }


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::CPbk2ThumbnailManager
// --------------------------------------------------------------------------
//
CPbk2ThumbnailManager::CPbk2ThumbnailManager( 
    CVPbkContactManager& aContactManager ) :
        iState( EIdle ),
        iContactManager( aContactManager ),
        iDefaultIconIndex( KErrNotFound )
	{
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::~CPbk2ThumbnailManager()
// --------------------------------------------------------------------------
//
CPbk2ThumbnailManager::~CPbk2ThumbnailManager()
	{
	delete iInProgressItemToBeRemoved;
	delete iThumbOperation;
    delete iManager;
    delete iRetrieveOperation;
    delete iStoreContact;
    iContactThumbnails.ResetAndDestroy();
    iLoadingQueue.Reset();
    iPriorityArray.Reset();
    }


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ConstructL()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ConstructL()
	{
	// image manager
	iManager = CPbk2ImageManager::NewL( iContactManager );
	// read file type for thumbnail field
	ReadFieldTypeL();
	//reset thumbnail array
	iContactThumbnails.Reset();
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::SetObserver()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::SetObserver( MPbk2ThumbnailManagerObserver& aObserver )
	{
	iObserver = &aObserver;
	}
	
// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::RemoveObserver()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::RemoveObserver()
	{
	iObserver = NULL;
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ThumbnailCount()	
// --------------------------------------------------------------------------
//
TInt CPbk2ThumbnailManager::ThumbnailCount()	
	{
	return iContactThumbnails.Count();
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::GetPbkIconIndex()
// --------------------------------------------------------------------------
//
TInt CPbk2ThumbnailManager::GetPbkIconIndex( TInt aListboxIndex, const MVPbkBaseContact& aContactLink )
	{
	TBool reLoadThumbnail = EFalse;
	TInt arrIndex = iDefaultIconIndex;
	
	// check that requested thumbnail is allready added to thumbnail array
	if( aListboxIndex < iContactThumbnails.Count() )
		{
		CPbk2TmItem* item = iContactThumbnails[ aListboxIndex ];
		
        // This is because when find is in use, the indexes are not mapped
		// directly to thumbnail managers indexes
		if( !item->GetLink()->RefersTo( aContactLink ) )
		    {
            // try to find correct one from the array based on contact link
            item = FindItem( aContactLink );
		    }
		
		if( item )
		    {
            // if item has a thumbnail image, but it is not loaded yet ( queue )
            if( item->HasThumbnail() && !item->GetBitmap() )
                {
                reLoadThumbnail = ETrue;
                }
            else
                {
                arrIndex = item->GetIconArrayIndex();
                }
		    }
			
		//if item and thumbnail index was not found ( queue limit ), reload thumbnail
		if( reLoadThumbnail )
			{
			TBool reOrderItem = ETrue;
			TInt res = iLoadingQueue.Find( item );
			//remove from array if duplicate
			if( res != KErrNotFound )
				{
				// if item's position is 0 or 1, dont reorder
				if( res <= 1 )
					{
					reOrderItem = EFalse;
					}
				// else remove item from the array for reordering
				else
					{
					iLoadingQueue.Remove( res );
					}
				}
			
			// if item is to be reordered
			if( reOrderItem )
				{
				// if there are more than 2 items
				if( iLoadingQueue.Count() > 2 && iState == ELoading )
					{
					// insert to second position, first one is under loading
					iLoadingQueue.Insert( item, 1 );
					}
				// else append to first or second
				else
					{
					iLoadingQueue.Append( item );
					}
				}
			
			//if idle, start loading
			if( iState == EIdle )
				{
				StartLoading();
				}
			}
		}
	return arrIndex;
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::FindItem()
// --------------------------------------------------------------------------
//
CPbk2TmItem* CPbk2ThumbnailManager::FindItem( const MVPbkBaseContact& aContactLink )
	{
	const TInt count = iContactThumbnails.Count();
	for( TInt i = 0; i < count; ++i )
		{
		if( iContactThumbnails[ i ]->GetLink()->RefersTo( aContactLink ) )
			{
			return iContactThumbnails[i];
			}
		}
	// shouldn't come here ever
	return NULL;
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::SetDefaultIconId()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::SetDefaultIconId( TPbk2IconId aDefaultIconId )
	{
	if( iIconArray )
		{
		iDefaultIconIndex = iIconArray->FindIcon( aDefaultIconId );
		}
	iDefaultIconId = aDefaultIconId;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::GetDefaultIconIndex()
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2ThumbnailManager::GetDefaultIconId()
	{
	return iDefaultIconId;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::SetPbkIconArray()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::SetPbkIconArray( CPbk2IconArray* aIconArray )
	{
	iIconArray = aIconArray;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::RemoveIconArray()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::RemoveIconArray()
	{
	// no more icon array
	iIconArray = NULL;
	const TInt count = iContactThumbnails.Count();
	
	// set all the indexes to default because there is no items in icon array to refer to
	for( TInt i = 0; i < count; ++i )
		{
		iContactThumbnails[i]->SetIconArrayIndexAndId( iDefaultIconIndex, iDefaultIconId );
		}
	iIconIdCounter = 0;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::IncreaseIndexes()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::IncreaseIndexes( TInt aListboxIndex )
	{
	TInt i;
	TInt index = 0;
	const TInt count = iContactThumbnails.Count();
	// go through all contacts
	for( i = 0; i < count; ++i )
		{
		index = iContactThumbnails[i]->GetListboxIndex();
		// increase by one all the indexes that are bigger than newly added index
		if( index  >= aListboxIndex )
			{
			iContactThumbnails[i]->SetListboxIndex( index+1 );
			}
		}
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::DecreaseIndexes()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::DecreaseIndexes( TInt aListboxIndex )
	{
	TInt i;
	const TInt count = iContactThumbnails.Count();
	TInt index = 0;
	
	// go through items
	for( i = 0; i < count; ++i )
		{
		index = iContactThumbnails[i]->GetListboxIndex();
		// decrease all indexes that were bigger than removed index
		if( index > aListboxIndex )
			{
			iContactThumbnails[i]->SetListboxIndex( index-1 );
			}
		}
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::Reset()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::Reset()
    {
    // clear cache
    iContactThumbnails.ResetAndDestroy();
    iPriorityArray.Reset();
    
    // reset operations and queue
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    delete iThumbOperation;
    iThumbOperation = NULL;
    iState = EIdle;
    iLoadingQueue.Reset();
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::SetThumbnailIconSize()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::SetThumbnailIconSize( TSize aSize )
	{
	iIconSize = aSize;
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::GetThumbnailIconSize()
// --------------------------------------------------------------------------
//
const TSize& CPbk2ThumbnailManager::GetThumbnailIconSize()
	{
	return iIconSize;
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::LoadThumbnailL()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::LoadThumbnailL( const MVPbkContactLink& aContactLink, TInt aListboxIndex )
	{
	// check if the thumbnail is already loaded
	if( aListboxIndex < iContactThumbnails.Count() &&
		iContactThumbnails[aListboxIndex]->GetLink()->IsSame( aContactLink ) )
		{
		CFbsBitmap* tmp = iContactThumbnails[ aListboxIndex ]->GetBitmap();
		// if there is icon for the contact allready loaded
		if( tmp )
			{
			//inform observer 
			if( iObserver )
				{
				//if icon array exists
				if( iIconArray )
					{
					//create icon and pass it to the array
					CGulIcon* icon = CGulIcon::NewL( DuplicateBitmapL( tmp ) );
					// counter to add icon ids
					TPbk2IconId iconID( TUid::Uid(KPbk2UID3 ), 
							KIconIndexOffset + iIconIdCounter );
					iIconIdCounter++;
					iIconArray->AppendIconL( icon, iconID );
					// store the index
					iContactThumbnails[ aListboxIndex ]->SetIconArrayIndexAndId( iIconArray->FindIcon( iconID ), iconID );
					iObserver->ThumbnailLoadingComplete( KErrNone,iContactThumbnails[ aListboxIndex]->GetListboxIndex() );
					}
				}
			}
		else
			{
			// has a default icon
			iObserver->ThumbnailLoadingComplete( KErrNotFound,iContactThumbnails[ aListboxIndex]->GetListboxIndex() );
			}
		}
	// new contact, add new CPbk2TmItem to thumbnail array
	else
		{
		// create new contact item. Thumbnail is not yet loaded so it's set to NULL
		CPbk2TmItem* item = CPbk2TmItem::NewL( aContactLink.CloneLC(), aListboxIndex );
		CleanupStack::Pop();
		// set default icon index
		item->SetIconArrayIndexAndId( iDefaultIconIndex,iDefaultIconId );
		item->SetHasThumbnail( ETrue );
		
		
		// if thumbnail to be loaded is added middle of the list (new contact), handle indexes
		const TInt lastIndex = iContactThumbnails.Count() - 1;
		if( lastIndex >= 0 && 
				aListboxIndex <= iContactThumbnails[ lastIndex ]->GetListboxIndex() )
			{
			// make room for the new thumbnail. Function does nothing if priority array is not full 
			// This is because we need to get the thumbnail showing right away when new contact is added
			MakeRoomForNextThumbnail();
			// increase indexes after this added index
			IncreaseIndexes( aListboxIndex );
			}
		
		// add item
		TLinearOrder<CPbk2TmItem> sorter( CPbk2TmItem::CompareByListboxIndex );
		iContactThumbnails.InsertInOrderL( item, sorter );
		
		// also add item to loading queue. This is because the actual array is sorted between thumbnail loading
		// list doesn't own the items
		// if added item's listbox index is smaller than already added, add to start end on loading queue
		const TInt queueCount = iLoadingQueue.Count();
		if( aListboxIndex <  queueCount && queueCount > 2  )
			{
			//add to second place, first one is under loading
			iLoadingQueue.Insert( item, 1 );
			}
		// just add last place on the list
		else
			{
			iLoadingQueue.Append( item );
			}
		
		//if idle, start loading
		if( iState == EIdle )
			{
			StartLoading();
			}
		}
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::RemoveThumbnail()
// --------------------------------------------------------------------------
//
TInt CPbk2ThumbnailManager::RemoveThumbnail( const MVPbkContactLink& aContactLink, TInt aListboxIndex  )
	{
	TInt i;
	const TInt count = iContactThumbnails.Count();
	CPbk2TmItem* item = NULL;
	TInt listBoxIndex = 0;
	
	for( i = 0; i < count; ++i )
		{
		item = iContactThumbnails[i];
		if( item->GetLink()->IsSame( aContactLink ) )
			{
			// store listbox index before updating
			listBoxIndex = item->GetListboxIndex();
		
            // handle indexes
		    DecreaseIndexes( aListboxIndex );

			// remove item from the list
			iContactThumbnails.Remove( i );
			
			// check that the icon is not a default icon
			if( item->GetIconArrayIndex() != iDefaultIconIndex )
				{
				if( iIconArray )
					{
					//inform icon array to remove the icon
					iIconArray->RemoveIcon( item->GetIconId() );
					}
				//update indexes after that
				UpdateIconIndexes();
				}
			
			//remove item from priority array if in it
			const TInt index = iPriorityArray.Find( item->GetLink() );
			//remove if found
			if( index != KErrNotFound )
				{
				iPriorityArray.Remove( index );
				}
			
			//The item might be even in the loading queue
			//Remove from loading queue
			//remove item from priority array if in it
            const TInt loadIndex = iLoadingQueue.Find( item );
            //remove if found
            if( loadIndex != KErrNotFound )
                {
                iLoadingQueue.Remove( loadIndex );
                }
			
			// inform observer
			if( iObserver )
				{
				iObserver->ThumbnailRemoved( *item->GetLink(),listBoxIndex );
				}
			
			//The item at 0th position, denotes the current item
			//whose thumbnail load is in progress.			
			if ( KStartIndex == loadIndex && !iInProgressItemToBeRemoved )
			    {
			    //Remove it when its safe
			    iInProgressItemToBeRemoved = item;
			    }
			else
			    {
			    //can be safely deleted immediately
			    delete item;
			    }
			return KErrNone;
			}
		}
	return KErrNotFound;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::UpdateIconIndexes()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::UpdateIconIndexes(  )
	{
	// if icon array exists
	if( iIconArray )
		{
		const TInt count = iContactThumbnails.Count();
		for( TInt i = 0; i < count; ++i )
			{
			if( !( iDefaultIconId == iContactThumbnails[ i ]->GetIconId() ) ) 
				{
				// icon is removed from the CPbk2IconArray, update indexes
				TPbk2IconId id = iContactThumbnails[ i ]->GetIconId();	
				iContactThumbnails[ i ]->SetIconArrayIndexAndId( iIconArray->FindIcon( id ), id );
				}
			}
		}
	}



// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::DuplicateBitmapL()
// --------------------------------------------------------------------------
//
CFbsBitmap* CPbk2ThumbnailManager::DuplicateBitmapL( CFbsBitmap* aSourceBitmap ) 
	{
	//Target bitmap.
	CFbsBitmap* dstbitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL( dstbitmap );

	//Get the handle to source bitmap
	TInt srchandle = aSourceBitmap->Handle();

	//Duplicate the bitmap handle. Increases the RefCount of bitmap 
	//managed but Fbs Server
	User::LeaveIfError( dstbitmap->Duplicate( srchandle ) );
	CleanupStack::Pop( dstbitmap );
	return dstbitmap;
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::StartLoading()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::StartLoading( )
	{
	//its safe to delete the item here
	if ( iInProgressItemToBeRemoved )
	    {
	    delete iInProgressItemToBeRemoved;
	    iInProgressItemToBeRemoved = NULL;
	    }
	
    TInt count = iLoadingQueue.Count();
    
    // limit loading queue
    while( count > KQueueLimit )
        {
        iLoadingQueue.Remove( --count );
        }
        
    if( count )
		{
		// manager is loading
		iState = ELoading;
		// take first item from the queue
		CPbk2TmItem* item = iLoadingQueue[KStartIndex];
		// take first item from the loading queue
		TRAPD( err, iRetrieveOperation = 
			iContactManager.RetrieveContactL( *item->GetLink(), *this ) );
		if( err )
			{
			// error occured -> remove item from the queue and continue
			iLoadingQueue.Remove( KStartIndex );
			//load next one
			StartLoading();
			}
		}
		
	// no more items
	else
		{
		// loading queue is empty, inform observer
		if( iObserver )
			{
			iObserver->LoadingFinished();
			}
		// default state
		iState = EIdle;
		}
	}

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ , MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;
    
    // store contact found, fetch thumbnail for it
    DoLoadThumbnail( *iStoreContact );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/ ,TInt /*aError*/ )
    {
    if( iObserver && !iInProgressItemToBeRemoved && iLoadingQueue.Count() )
    	{
    	//inform observer
    	//We should not inform the observer with wrong index,
    	//hence check for iInProgressItemToBeRemoved
    	iObserver->ThumbnailLoadingComplete( KErrNotFound, iLoadingQueue[ KStartIndex ]->GetListboxIndex() );
    	
    	//remove item from the queue
    	iLoadingQueue.Remove( KStartIndex );
    	}    
    //next
    StartLoading();
    }


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::DoLoadThumbnail()
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::DoLoadThumbnail( 
		MVPbkBaseContact& aContact )
	{
    TInt err = KErrNotFound;

    //if the item in progress has been removed then it will be filled in iInProgressItemToBeRemoved
    //If such condition arises then we need not load the thumbnail at all
    
    // check if the contact has an image
	if( !iInProgressItemToBeRemoved && iManager->HasImage( aContact, *iFieldType ) && iLoadingQueue.Count() )
		{
		// contact has a thumbnail
		iLoadingQueue[ KStartIndex ]->SetHasThumbnail( ETrue ); 
		
		/*
		 * If priority array is full, this function removes first item from the array and 
		 * frees all the allocations related.
		 */
		MakeRoomForNextThumbnail();
			
		// if there is room, load thumbnail
		if( iPriorityArray.Count() <= KLoadingLimit )
			{
			//add current item to priority array
			iPriorityArray.Append( iLoadingQueue[ KStartIndex ]->GetLink() );
			
			// start loading picture for the current item
			delete iThumbOperation;
			iThumbOperation = NULL;
			// Define parameters for thumbnail
			TPbk2ImageManagerParams params;
			// if iconsize is not set
			if( iIconSize.iHeight == 0 &&  iIconSize.iWidth == 0 )
				{
				iIconSize = KDefaultThumbnailSize;
				}
			TInt useCropping = 0x0008;
			// set params
			params.iSize = iIconSize;
			params.iFlags = TPbk2ImageManagerParams::EScaleImage |
							TPbk2ImageManagerParams::EKeepAspectRatio |
							useCropping;	//CROP IMAGE	//TODO change value
			// fetch the image
			TRAP( err, iThumbOperation =
				iManager->GetImageAsyncL( &params, aContact, *iFieldType, *this ) );
			}
		// no room at this moment
		else
			{
			//has image, but cannot load it
			err = KErrNone;
			//remove item from the queue
			iLoadingQueue.Remove( KStartIndex );
			// start loading next one
			StartLoading();
			}
		}
	
	// contact has no image or error occured, move to next contact
	if( err )
		{
		if ( iLoadingQueue.Count() )
		    {
            if( iObserver )
                {
                //inform observer
                iObserver->ThumbnailLoadingComplete( KErrNotFound, iLoadingQueue[ KStartIndex ]->GetListboxIndex() );
                }
            //no thumbnail
            iLoadingQueue[ KStartIndex ]->SetHasThumbnail( EFalse );
            // set default icon index and id
            iLoadingQueue[ KStartIndex ]->SetIconArrayIndexAndId( iDefaultIconIndex, iDefaultIconId );
            //remove item from the queue
            iLoadingQueue.Remove( KStartIndex );
		    }
		// start loading next one
		StartLoading();
		}
	}


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::MakeRoomForNextThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::MakeRoomForNextThumbnail()
	{
	// if the loading limit for thumbnails is reached and optimizing is on, make room for next thumbnail
	if( iPriorityArray.Count() >= KLoadingLimit )
		{
		// remove first one from the queue
		CPbk2TmItem* rem = NULL;
		const TInt count = iContactThumbnails.Count();
		for( TInt i = 0; i < count; ++i )
			{
			if( iContactThumbnails[ i ]->GetLink()->IsSame( *iPriorityArray[ KStartIndex ] ) )
				{
				rem = iContactThumbnails[i];
				break;
				}
			}
		// if item was found
		if( rem )
			{
			// remove from priority array
			iPriorityArray.Remove( 0 );
			
			//remove icon from Pbk2IconArray
			if( iIconArray )
				{
				iIconArray->RemoveIcon( rem->GetIconId() );
				}
			// remove memory allocations from heap
			rem->DeleteBitmap();
			//set default icon id and index
			rem->SetIconArrayIndexAndId( iDefaultIconIndex,iDefaultIconId );
			//update indexes for other items
			UpdateIconIndexes();
			}
		}
	}



// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::Pbk2ImageGetComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::Pbk2ImageGetComplete
        ( MPbk2ImageOperation& /*aOperation*/, CFbsBitmap* aBitmap )
    {
    //if the item in progress has been removed then it will be filled in iInProgressItemToBeRemoved
    //If such condition arises then we need not load the thumbnail at all
    
    // store loaded bitmap
    if ( !iInProgressItemToBeRemoved && iLoadingQueue.Count() )
        {
        iLoadingQueue[ KStartIndex ]->SetBitmap( aBitmap );
        
        // inform the observer
        if( iObserver )
            {
            // if icon array exists
            if( iIconArray )
                {
                //create icon and pass it to the array
                TPbk2IconId iconID( TUid::Uid( KPbk2UID3 ), iIconIdCounter + KIconIndexOffset );
                iIconIdCounter++;
                
                TRAPD( err, AppendIconL( iconID, aBitmap ) );	
                if( err == KErrNone )
                    {
                    // set icond index and id
                    iLoadingQueue[ KStartIndex ]->SetIconArrayIndexAndId( iIconArray->FindIcon( iconID ), iconID );
                    // inform observer
                    iObserver->ThumbnailLoadingComplete( KErrNone, iLoadingQueue[ KStartIndex ]->GetListboxIndex() );
                    }
                }
            }
        
        //remove item from the queue
        iLoadingQueue.Remove( KStartIndex );
        }
    else
        {
        //None to take ownership. so delete here
        delete aBitmap;
        }   
    
    // delete operation
    delete iThumbOperation;
    iThumbOperation = NULL;
    // next
    StartLoading();
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::Pbk2ImageGetFailed
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::Pbk2ImageGetFailed
        ( MPbk2ImageOperation& /*aOperation*/, TInt /*aError*/ )
    {
    delete iThumbOperation;
    iThumbOperation = NULL;
    
    if( iObserver && !iInProgressItemToBeRemoved && iLoadingQueue.Count() )
    	{
    	//inform observer
    	//We should not inform the observer with wrong index,
    	//hence check for iInProgressItemToBeRemoved
    	iObserver->ThumbnailLoadingComplete( KErrNotFound, iLoadingQueue[ KStartIndex ]->GetListboxIndex() );
    	
    	//remove item from the queue
    	iLoadingQueue.Remove( KStartIndex );    	    
    	}
    
    // next
    StartLoading();
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ContactViewReady( MVPbkContactViewBase& aView )
    {
    TRAP_IGNORE( DoContactViewReadyL( aView ) );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::DoContactViewReadyL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::DoContactViewReadyL( MVPbkContactViewBase& aView )
    {   
    // get contact count
    const TInt contactCount = aView.ContactCountL();
    const TInt thumbnailCount = iContactThumbnails.Count();
    
    if( thumbnailCount == 0 )
        {
        // first time initialization
        for( TInt i = 0; i < contactCount; ++i )
            {
            CPbk2TmItem* item = CPbk2TmItem::NewL( aView.CreateLinkLC( i ), i );
            CleanupStack::Pop(); // link
            CleanupStack::PushL( item );
            // set default icon index
            item->SetIconArrayIndexAndId( iDefaultIconIndex,iDefaultIconId );
            item->SetHasThumbnail( ETrue );
            
            // add item
            iContactThumbnails.AppendL( item );
            CleanupStack::Pop( item );
            }
        }
    else
        {
        // there is items in the listbox that are not loaded yet. If there is favorite contacts, 
        // those are added afterwards to the list. normal items are added first.
        const TInt itemCount = contactCount - thumbnailCount;
        for( TInt i = 0; i < itemCount; ++i )
            {            
            MVPbkContactLink* link = aView.CreateLinkLC( i );
            // if link exists and some thumbnails are not loaded
            if( link  )
                {
                // add to thumb manager and load thumbnail
                LoadThumbnailL( *link,i );
                // created link
                CleanupStack::PopAndDestroy();
                }
            }
        }  
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ContactViewUnavailable( 
    MVPbkContactViewBase& /*aView*/ )
    {
    // ignored
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ContactAddedToView(
    MVPbkContactViewBase& /*aView*/, 
    TInt aIndex, 
    const MVPbkContactLink& aContactLink )
    {
    TRAP_IGNORE( LoadThumbnailL( aContactLink, aIndex ) );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ContactRemovedFromView(
    MVPbkContactViewBase& /*aView*/, 
    TInt aIndex, 
    const MVPbkContactLink& aContactLink )
    {
    RemoveThumbnail( aContactLink, aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ContactViewError(
    MVPbkContactViewBase& /*aView*/, 
    TInt /*aError*/, 
    TBool /*aErrorNotified*/ )
    {
    // ignored
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::ReadFieldTypeL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::ReadFieldTypeL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( reader, R_PBK2_THUMBNAIL_FIELD_TYPE );
    const TInt count = reader.ReadInt16();

    TVPbkFieldTypeMapping mapping( reader );
    iFieldType = mapping.FindMatch( iContactManager.FieldTypes() );
    if( !iFieldType )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); // CreateResourceReaderLC
    }


// --------------------------------------------------------------------------
// CPbk2ThumbnailManager::AppendIconL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailManager::AppendIconL( 
    TPbk2IconId& aIcon, CFbsBitmap* aBitmap )
    {
    // create icon
    CGulIcon* icon = CGulIcon::NewLC();
    icon->SetBitmap( DuplicateBitmapL( aBitmap ) );
    iIconArray->AppendIconL( icon, aIcon );
    CleanupStack::Pop( icon );
    }

// end of file
