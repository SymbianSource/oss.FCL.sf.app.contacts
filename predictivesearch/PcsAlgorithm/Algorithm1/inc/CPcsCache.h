/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used to cache the contacts in memory
*
*/

#ifndef __CPCS_CACHE_H__
#define __CPCS_CACHE_H__

// INCLUDE FILES
#include <e32base.h>
#include <e32hashtab.h>

#include "CPsData.h"
#include "CPcsKeyMap.h"
#include "CPcsPoolElement.h"
#include "TCacheInfo.h"
#include "CPcsDefs.h"

// CLASS DECLARATION
class CPcsCache : public CBase
{

	public:

		/**
		* Two phase construction
		*/
		static CPcsCache* NewL(const TDesC& aURI, CPcsKeyMap& aKeyMap, TUint8 aUriId);

		/**
		* Virtual destructor
		*/
		virtual ~CPcsCache();

		/**
		* Returns the database URI this cache support
		*/
		TDesC& GetURI();

		/**
		* Get CPcsPoolElement for a key id
		*/
		void GetContactsForKeyL(TInt aKeyId, RPointerArray<CPcsPoolElement>& aData);

		/**
		* Get all cached content
		*/
		void GetAllContentsL(RPointerArray<CPsData>& aData);

		/**
		* Add a data element to the cache
		*/	    
		void AddToCacheL(CPsData& aData);

		/**
		* Removes a data element from the cache
		*/
		void RemoveFromCacheL(TInt aItemId);	    

		/**
		* Removes all data elements from the cache
		*/
		void RemoveAllFromCache();

		/**
		* Sets data fields to the cache
		*/
		void SetDataFields(RArray<TInt>& aDataFields);

		/**
		* Gets data fields from the cache
		*/
		void GetDataFields(RArray<TInt>& aDataFields);

		/**
		* Updates the caching status for this cache
		*/
		void UpdateCacheStatus(TInt aStatus);

		/**
		* Returns the cache status for this cache
		*/
		TInt GetCacheStatus();

		/**
		* Returns the UriId
		*/
		TUint8 GetUriId();

	    /**
	    * Sets sort order for data fields
	    */
	    void SetSortOrder(RArray<TInt>& aSortOrder);
	    
	    /**
	    * Gets sort order for data fields
	    */
	    void GetSortOrder(RArray<TInt>& aSortOrder);
	    
	    /**
	    * Get the index order corresponding to the sort order
	    */	
		void GetIndexOrder(RArray<TInt>& aIndexOrder);
		
        /**
         * Resort data according to the new sortorder
         */
        void ResortdataInPoolsL();

	private:

		/**
		* Constructor
		*/
		CPcsCache();

		/**
		* Second phase constructor
		*/
		void ConstructL(const TDesC& aURI, CPcsKeyMap& aKeyMap, TUint8 aUriId);

		/**
		* Utility function to add a contact to the pool
		*/
		void AddToPoolL(TUint64& aPoolMap, CPsData& aData);

		/**
		* Utility function 
		*/
		void SetPoolMap(TUint64& aPoolMap, TInt aArrayIndex);

		/**
		* Utility function 
		*/
		TBool GetPoolMap(TUint64& aPoolMap, TInt aArrayIndex);

        /**
         * Utility function
         */
        void ComputeIndexOrder();
        
	private:    

		/*
		* Array of key maps
		*/
		typedef RPointerArray<CPcsPoolElement> R_PTR_ARRAY;
		RPointerArray<R_PTR_ARRAY> iKeyArr;

		/*
		* Hashmaps to remember the location of a contact in the pools
		*/
		RHashMap<TInt, TUint64> iCacheInfo;

		/*
		* Master pool of all contacts in this cache
		*/
		RPointerArray<CPsData> iMasterPool;	
		
	    /*
	    * Master pool backup of all contacts in this cache when sortorder changed
	    */
	    RPointerArray<CPsData> iMasterPoolBackup;

		/*
		* Not owned
		*/
		CPcsKeyMap* iKeyMap;

		/**
		* Owned. Refer to the database URI this cache is for.
		*/
		HBufC* iURI;

		/**
		* Internal URI id for this cache
		*/
		TUint8 iUriId;

		/**
		* Current status of this cache
		*/
		TInt iCacheStatus;

		/**
		* List of data fields (mapped to VPbk ids) supported by this cache
		*/
		RArray<TInt> iDataFields;
		/**
		* Sort order of data fields expected by the clients
		*/
		RArray<TInt> iSortOrder;
		
		/**
		* Order of indexes determined by correlating iDataFields and iSortOrder
		* Each CPsData data element fields need to be compared in this order
		* to get the client expected sort order for this cache.
		*/
		RArray<TInt> iIndexOrder;
		
};


#endif // __CPCS_CACHE_H__

// End of file