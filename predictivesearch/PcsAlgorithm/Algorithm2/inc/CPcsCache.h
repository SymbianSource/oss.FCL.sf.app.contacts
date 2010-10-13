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

#ifndef __CPCS_CACHE_ALGORITHM2_H__
#define __CPCS_CACHE_ALGORITHM2_H__

// INCLUDE FILES
#include <e32base.h>
#include <e32hashtab.h>

#include "CPsData.h"
#include "CPcsKeyMap.h"
#include "CPcsPoolElement.h"
#include "TCacheInfo.h"
#include "CPcsDefs.h"

class CPcsAlgorithm2;
// CLASS DECLARATION
class CPcsCache : public CBase
    {

public:

    /**
     * Two phase construction
     */
    static CPcsCache* NewL(CPcsAlgorithm2* aAlgorithm, TDesC& aURI, 
                           CPcsKeyMap& aKeyMap, TUint8 aUriId);

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
    void RemoveAllFromCacheL();

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
     * Returns the URI for this cache
     */
    HBufC* GetUri();
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
    void ConstructL(CPcsAlgorithm2* aAlgorithm, TDesC& aURI,
                    CPcsKeyMap& aKeyMap, TUint8 aUriId);

    /**
     * Utility function to add a contact to the pool
     */
    void AddToPoolL(TInt& aInfo, CPsData& data);

    /**
     * Utility function 
     */
    void SetPoolMap(TInt& aPoolMap, TInt aPoolId);

    /**
     * Utility function 
     */
    TBool GetPoolMap(TInt& aPoolMap, TInt arrayIndex);

    /**
     * Utility function
     */
    void ComputeIndexOrder();
private:

    CPcsAlgorithm2* iAlgorithm;

    /*
     * Array of key maps
     */
    typedef RPointerArray<CPcsPoolElement> R_PTR_ARRAY;
    RPointerArray<R_PTR_ARRAY> keyArr;

    /*
     * Hashmaps to remember the location of a contact in the pools
     */
    RHashMap<TInt, TInt> cacheInfo;

    /*
     * Master pool of all contacts in this cache
     */
    RPointerArray<CPsData> masterPool;
    
    /*
    * Master pool backup of all contacts in this cache when sortorder changed
    */
    RPointerArray<CPsData> masterPoolBackup;  

    /*
     * Not owned
     */
    CPcsKeyMap* keyMap;

    /**
     * Owned. Refer to the database URI this cache is for.
     */
    HBufC* iURI;

    /**
     * Internal URI id for this cache
     */
    TInt8 iUriId;

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

#endif // __CPCS_CACHE_ALGORITHM2_H__
// End of file
