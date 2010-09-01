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

#ifndef CPBK2THUMBNAILMANAGER_H_
#define CPBK2THUMBNAILMANAGER_H_

// external includes
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ImageOperationObservers.h>
#include <MVPbkContactViewObserver.h>
#include <TPbk2IconId.h>
#include "MPbk2FilteredViewStack.h"

//FORWARD declaration
class CPbk2ImageManager;
class CVPbkContactManager;
class MVPbkContactOperationBase;
class MVPbkContactLink;
class MVPbkStoreContact;
class MVPbkBaseContact;
class MVPbkFieldType;
class CPbk2TmItem;
class CPbk2IconArray;

//internal state of the loader
enum TTMLoadingState
	{
	ELoading,
	EIdle
	};

// state for determining whether to optimize or not
enum TTMOptimizationState
	{
	EOptimize,
	ENoOptimization
	};

/*
 * Observer class for the manager
 */
class MPbk2ThumbnailManagerObserver
	{
public:
	
	/*
	 * This is called after all thumbnails from the loading queue is loaded.
	 */
	virtual void LoadingFinished() = 0;
	
	/*
	 * Informs observer that loading of thumbnail is complete
	 * Param@   aError, KErrNone if success, KErrNotFound if not found.
	 * param@	index on listbox or KErrNotFound if not found
	 */
	virtual void ThumbnailLoadingComplete( TInt aError, TInt aIndex ) = 0;
	
	/*
	 * Informs the observer that thumbnail has been deleted from the manager's array. Listbox index is the index
	 * that is given when thumbnail is asked to be loaded
	 */
	virtual void ThumbnailRemoved( const MVPbkContactLink& aLink, TInt aIndex ) = 0;
	
	};


/*
 * Class for loading contact thumbnails
 */
NONSHARABLE_CLASS( CPbk2ThumbnailManager ) : public CBase,
											 public MVPbkSingleContactOperationObserver,
											 public MPbk2ImageGetObserver,
											 public MVPbkContactViewObserver,
											 public MPbk2FilteredViewStackObserver
	{
public:	// constructor & destructor
	
	IMPORT_C static CPbk2ThumbnailManager* NewL( CVPbkContactManager& aContactManager );
	virtual ~CPbk2ThumbnailManager();
	
	
public:	// new functions
	
	/*
	 * Sets observer for the manager
	 */
	void SetObserver( MPbk2ThumbnailManagerObserver& aObserver );
	
	/*
	 * Removes observer
	 */
	void RemoveObserver();
	
	
	/*
	 * Returns icon index from Pbk2IconArray for double listbox index. 
	 * If icon is not set, default icon index is returned
	 */
	TInt GetPbkIconIndexL( TInt aListboxIndex, const MVPbkContactLink& aContactLink );
	
	/*
	 * Setter for default icon ID
	 */
	void SetDefaultIconId( TPbk2IconId aDefaultIconId );
		
	/*
	 * Returns default icon id
	 */
	const TPbk2IconId& GetDefaultIconId();
	
	/*
	 * Setter for Pbk2IconArray. Thumbnail manager doesn't own the array.
	 */
	void SetPbkIconArray( CPbk2IconArray* aIconArray );
	
	/*
	 * Removes icon array from the manager. Doesn't destroy the arrray
	 */
	void RemoveIconArray();
	
	/*
	 * Returns number of loaded thumbnails
	 */
	TInt ThumbnailCount();	

	/**
	 * Reset thumbnail manager. Clear cache and cancel all ongoing operations.
	 * NOTICE: Does not remove icons from icon array.
	 */
	void Reset();
	
	/*
	 * Setter for thumbnail icon size
	 */
	void SetThumbnailIconSize( TSize aSize );
	
	/*
	 * Getter for thumbnail icon size
	 */
	const TSize& GetThumbnailIconSize();
	
	/**
	 * Set view for thumbnail manager. Thumbnail manager will start observing the 
	 * view and update it's internal state accordingly. Remove view by setting it
	 * to NULL.
	 */
	void SetContactViewL( MPbk2FilteredViewStack* aView );
	
private:	// new functions
	
    /**
     * Removes contact from loader's array and deletes the thumbnail. 
     */
    void RemoveThumbnail( const MVPbkContactLink& aContactLink, TInt aListboxIndex  );
	
	/*
	 * reset listbox indexes to match the array order
	 */
	void ResetIndexes();
	
	/*
	 * Removes allready loaded thumbnail from the last position of the priorization array. 
	 * Removes also image from Pbk2IconArray and iContactThumbnails. 
	 */
	void MakeRoomForNextThumbnail();
	
	/*
	 * Starts loading of the contact thumbnails. 
	 */
	void StartLoading();
	
	/*
	 * Loads the thumbnail for the contact
	 */
	void DoLoadThumbnail( MVPbkBaseContact& aContact );
	
	/*
	 * Duplicates the bitmap handle
	 */
	CFbsBitmap* DuplicateBitmapL( CFbsBitmap* aSourceBitmap ); 
	
	/*
	 * Reads file type for the contact image
	 */
	void ReadFieldTypeL();
	
	/**
	 * Append bitmap into icon array
	 */
	void AppendIconL( TPbk2IconId& aIcon, CFbsBitmap* aBitmap );
	
	/*
	 * Icon indexes must be updated after icon(s) are removed from the icon array
	 */
	void UpdateIconIndexes( );

	/**
	 * Handle view ready events
	 */
	void DoContactViewReadyL( MVPbkContactViewBase& aView );

	/**
	 * Create empty thumbnail item array reflecting the aView. 
	 */
	void PreCreateThumbnailArrayL( MVPbkContactViewBase& aView );

    /**
     * Flush cached thumbnails and cancel all ongoing and scheduled operations.
     * Remove icons from icon array if available.
     */
    void FlushCache();
    
private: // From MVPbkSingleContactOperationObserver
   void VPbkSingleContactOperationComplete(
		   MVPbkContactOperationBase& aOperation,
		   MVPbkStoreContact* aContact );
   
   void VPbkSingleContactOperationFailed(
		   MVPbkContactOperationBase& aOperation,
		   TInt aError );

private: // From MPbk2ImageGetObserver
	void Pbk2ImageGetComplete(
			MPbk2ImageOperation& aOperation,
			CFbsBitmap* aBitmap );
	void Pbk2ImageGetFailed(
			MPbk2ImageOperation& aOperation,
			TInt aError );

private: // From MVPbkContactViewObserver
    void ContactViewReady( MVPbkContactViewBase& aView );
    void ContactViewUnavailable( MVPbkContactViewBase& aView );
    void ContactAddedToView(
        MVPbkContactViewBase& aView, 
        TInt aIndex, 
        const MVPbkContactLink& aContactLink );
    void ContactRemovedFromView(
        MVPbkContactViewBase& aView, 
        TInt aIndex, 
        const MVPbkContactLink& aContactLink );
    void ContactViewError(
        MVPbkContactViewBase& aView, 
        TInt aError, 
        TBool aErrorNotified );

private: // From MPbk2FilteredViewStackObserver
    void TopViewChangedL( MVPbkContactViewBase& aOldView );
    void TopViewUpdatedL();
    void BaseViewChangedL();
    void ViewStackError( TInt aError );
    void ContactAddedToBaseView( 
        MVPbkContactViewBase& aBaseView,
        TInt aIndex,
        const MVPbkContactLink& aContactLink );


private:	//constructors
	CPbk2ThumbnailManager( CVPbkContactManager& aContactManager );
	void ConstructL();
	
private:	//data
	
	//OWN: thumbnail size
	TSize							iIconSize;
	
	// internal state
	TTMLoadingState					iState;
	
	/// Own:	thumbnails
	RPointerArray<CPbk2TmItem>		iContactThumbnails;
	
	/// Own:	Loading queue for the thumbnails
	RPointerArray<CPbk2TmItem>		iLoadingQueue;
	
	/// Own: Thumbnail manager
	CPbk2ImageManager* 				iManager;
	
	// ref:
	CVPbkContactManager& 			iContactManager;
	
	 /// Own: Retrieve operation
	MVPbkContactOperationBase* 		iRetrieveOperation;
	
	/// Own: Store contact
	MVPbkStoreContact* 				iStoreContact;
	
	/// Own: Asynchronous thumbnail operation
	MPbk2ImageOperation* 			iThumbOperation;
	
	// REF: observer for the loader
	MPbk2ThumbnailManagerObserver*	iObserver;
	
	/// Ref: Field type
	const MVPbkFieldType* 			iFieldType;
	
	//REF:	listbox icon array
	CPbk2IconArray*					iIconArray;
	
	// default icon index and id
	TInt							iDefaultIconIndex;
	TPbk2IconId						iDefaultIconId;
	// icon index counter
	TInt							iIconIdCounter;
	// loading priority.
	RArray<MVPbkContactLink*>		iPriorityArray;
	/// Own: Holds the item whose thumbnail load is in progress
	/// This item needs to be removed when its safe
	CPbk2TmItem*                    iInProgressItemToBeRemoved;
	/// Not own. Filtered view of contacts.
	MPbk2FilteredViewStack*         iView;
	/// Flag whether cache is clean. Used to prevent unnecessary work when cleaning
	/// is requested repeatedly
	TBool iIsCacheClean;
	};

#endif /* CPBK2THUMBNAILMANAGER_H_ */
