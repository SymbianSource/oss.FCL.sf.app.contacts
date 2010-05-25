/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Identifies a contact in cache pools
*
*/

#ifndef C_PCS_ALGORITHM_2
#define C_PCS_ALGORITHM_2

// INCLUDES
#include <e32cmn.h>
#include <s32mem.h>
#include <e32hashtab.h>
#include <e32property.h>
#include <centralrepository.h>

#include "PSAlgorithmInternalCRKeys.h"
#include "CPcsPlugin.h"
#include "CPsData.h"
#include "CPsClientData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsQuery.h"
#include "CPsPattern.h"
#include "mdatastoreobserver.h"
#include "mstorelistobserver.h"
#include <FindUtil.h>

const TInt KSpace = 32;

// FORWARD DECLARATION
class CPsData;
class CPsClientData;
class CPsDataPluginInterface;
class CPsQuery;
class CPcsAlgorithm2Helper;
class CPcsAlgorithm2MultiSearchHelper;
class CPtiEngine;
class CRepository;
class CFindUtilChineseECE;
class CFindUtil;

// CLASS DECLARATION
class CPcsAlgorithm2 : public CPcsPlugin,
public MDataStoreObserver,
public MStoreListObserver,
public MFindStringConverter
    {
public: 

    /**
     * Two phase construction
     */
    static CPcsAlgorithm2* NewL();

    /**
     * Destructor
     */
    virtual ~CPcsAlgorithm2();


public:

    // ------------------------ From base class CPcsPlugin ----------------

    /**
     * Search Function for cache
     */
    void PerformSearchL(const CPsSettings& aPcsSettings,
        CPsQuery& aCondition,
        RPointerArray<CPsClientData>& aData,
        RPointerArray<CPsPattern>& aPattern);							

    /**
     * Search Function for input string
     */
    void SearchInputL(CPsQuery& aSearchQuery,
        TDesC& aSearchData,
        RPointerArray<TDesC>& aMatchSet,
        RArray<TPsMatchLocation>& aMatchLocation );  

    
    /**
    * Performs search on a input string, and return result also as a string 
    */
    void  SearchMatchStringL( CPsQuery& aSearchQuery,
                                TDesC& aSearchData,
                                TDes& aMatch );     
    
    /**
     * Returns ETrue if this language is supported
     */
    TBool IsLanguageSupportedL(const TUint32 aLanguage);

    /**
     * Returns supported data fields for a cache
     */		                     
    void GetDataOrderL( TDesC& aURI, RArray<TInt>& aDataOrder );

    /**
     * Returns supported sort order for a cache
     */		                     
    void GetSortOrderL( TDesC& aURI, RArray<TInt>& aDataOrder );

    /**
     * Sets the sort order for a cache
     */
    void ChangeSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder);


    // --------------------------------------------------------------------

public:

    // --------------- From base class MDataStoreObserver -----------------

    /** 
     * Add a contact to the cache
     */
    void AddData ( TDesC& aDataStore, CPsData* aData);

    /** 
     * Remove a contact from the cache based on contact id		
     */
    void RemoveData ( TDesC& aDataStore, TInt aItemId );

    /**
     *  Removes all the contacts from a particular datastore
     */
    void RemoveAll ( TDesC& aDataStore);

    /**
     * Updates the caching status for a particular datastore
     */
    void UpdateCachingStatus(TDesC& aDataStore, TInt aStatus);

    // --------------------------------------------------------------------

public:

    // ----------------- From base class MStoreListObserver ---------------

    /**
     * This method is called whenever any new store has been added
     */
    void AddDataStore ( TDesC& aDataStore);

    /**
     * This method is called whenever a data store is removed
     */
    void RemoveDataStore ( TDesC& aDataStore);

    // --------------------------------------------------------------------

public:	

    /**
     * Returns the array index of cache (in iPcsCache) for a datastore
     */
    TInt GetCacheIndex(TDesC& aDataStore);	

    /** 
     * Return the cache instance at a specific array index
     */
    inline CPcsCache* GetCache(TInt aIndex) 
        {
        return iPcsCache[aIndex];
        }

    /**
     * Returns the key map instance
     */
    inline CPcsKeyMap* GetKeyMap() 
        { 
        return iKeyMap; 
        };


private:

    /**
     * Constructor
     */
    CPcsAlgorithm2();

    /**
     * 2nd phase construtor
     */
    void ConstructL();

private:

    /**
     * Define a P&S property with given key under the internal category 
     * UID of PCS. Leave if definition fails for any other reason than
     * key already existing.
     */
    void DefinePropertyL( TPcsInternalKeyCacheStatus aPsKey );

    /**
     * Replace occurance of "0" in ITU-T mode with space
     */
    TBool  ReplaceZeroWithSpaceL(CPsQuery& aQuery);

    /**
     * Search function helper
     */
    void  DoSearchL(const CPsSettings& aPcsSettings,
        CPsQuery& aCondition,
        RPointerArray<CPsData>& searchResults,
        RPointerArray<CPsPattern>& searchSeqs );		                       

    /**                
     * Search function helper
     */
    void  DoSearchInputL(CPsQuery& aQuery,
        TDesC& aData,
        RPointerArray<TDesC>& searchSeqs,
        RArray<TPsMatchLocation>& aMatchLocation );	 

    /**
     * Returns the index corresponding a URI in iDataStoreUri
     */
    TInt FindStoreUri ( TDesC& aDataStoreUri );

    /**
     * Function to return all cached content
     */
    void GetAllContentsL(const CPsSettings& aPcsSettings,	                    
        RPointerArray<CPsData>& searchResults );

    /**
     * Checks if search is on groups
     * Return ETrue if there is a valid group URI        
     */
    TBool IsGroupSearchL ( CPsSettings& aSettings,
        RArray<TInt>& aGroupIdArray );						 

    /**
     * Utility function to replace groups uri with that of contacts uri
     */
    void ReplaceGroupsUriL ( CPsSettings& aSettings );							 

    /**
     * Filters the search results array for groups
     */
    void FilterSearchResultsForGroupsL(RArray<TInt>& aGroupContactIds,
        RPointerArray<CPsData>& aSearchResults);

    /**
     * Get the list of contact ids that belong to a group.
     */							 	           
    void GetContactsInGroupL ( TInt aGroupId, 
        RArray<TInt>& aGroupContactIds );

    /**
     * Read sort order for a data store from the central repository
     * @param aURI - The data store URI for which sort order is required
     * @param aSortOrder - The persisted sort order from the cenrep
     */
    void ReadSortOrderFromCenRepL ( TDesC& aURI, 
        RArray<TInt>& aSortOrder );

    /** Write sort order for a data store to the central repository
     * @param aURI - The data store URI for which sort order is to be persisted
     * @param aSortOrder - The sort order to be persisted	
     */
    void WriteSortOrderToCenRepL ( TDesC& aURI, 
        RArray<TInt>& aSortOrder );	

    /**
     * Utility function that sets the caching error value
     */                             
    void SetCachingError ( TDesC& aDataStore, 
        TInt aError );	

    /**
     * Write the content required by client 
     */
    CPsClientData* WriteClientDataL ( CPsData& aPsData );	

    /**
     * Function to return data base URI for an internal identifier
     */		                     
    TDesC& GetUriForIdL( TUint8 aUriId );	

    /**
     * Inform clients about update happened on the cache
     */
    void HandleCacheUpdated( TCachingStatus aStatus );
    
    /**
    * launch plugins by idle
    */
     void DoLaunchPluginsL();
    
    /**
    * launch plugins
    */
     static TInt DoLaunchPluginsL(TAny* aPtr);

public:    

    inline TInt GetFirstNameIndex() 
        { 
        return iFirstNameIndex; 
        }

    inline TInt GetLastNameIndex()
        { 
        return iLastNameIndex; 
        }

    inline CFindUtilChineseECE* FindUtilECE()
        {
        return iFindUtilECE;
        }

    inline CFindUtil* FindUtil()
        {
        return iFindUtil;
        }

    void ReconstructCacheDataL();

public: // From MFindStringConverter
    void Converter(const TDesC& aSourStr, TDes& aDestStr);

private:

    /**
     * Instance of contacts cache
     * Own
     */
    RPointerArray<CPcsCache>   iPcsCache; 

    /**
     * Keeps the count of caches
     */
    TUint8 iCacheCount; 

    /**
     * Instance of key map
     * Own.
     */
    CPcsKeyMap* iKeyMap;

    /**
     * Data plugin interface. Owned
     */
    CPsDataPluginInterface *iPsDataPluginInterface;

    /**
     * Instance of algorithm helper. Owned
     */
    CPcsAlgorithm2Helper* iHelper;
    CPcsAlgorithm2MultiSearchHelper* iMultiSearchHelper;


    /**
     * Global that keeps the updated status of the caching
     */
    TCachingStatus iCacheStatus;

    /**
     * Global that stores any error that occurs while caching
     */
    TInt iCacheError;

    // Own: Find util used to match contacts and construct character grid
    CFindUtilChineseECE* iFindUtilECE;

    // Own: Find util used to match contacts and construct character grid
    CFindUtil* iFindUtil;

    // keep the index for Firstname and Lastname
    TInt iFirstNameIndex;
    TInt iLastNameIndex;
    
    /**
    * plugin laucher, make the plugin instantiation async
    */
    CIdle* iPluginLauncher;

    };


#endif // C_PCS_ALGORITHM_2
