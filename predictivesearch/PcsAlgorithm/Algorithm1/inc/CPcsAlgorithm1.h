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
* Description:  Predictive Contact Search Algorithm 1 main class
*
*/


#ifndef C_PCS_ALGORITHM_1
#define C_PCS_ALGORITHM_1

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

// FORWARD DECLARATION
class CPsData;
class CPsClientData;
class CPsDataPluginInterface;
class CPsQuery;
class CPcsAlgorithm1Helper;
class CPcsAlgorithm1MultiSearchHelper;

// CLASS DECLARATION
class CPcsAlgorithm1 : public CPcsPlugin,
                       public MDataStoreObserver,
                       public MStoreListObserver
{
	public: 
	
		/**
		* Two phase construction
		*/
		static CPcsAlgorithm1* NewL();

		/**
		* Destructor
		*/
		virtual ~CPcsAlgorithm1();


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
		TInt GetCacheIndex(const TDesC& aDataStore);	

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
		}
        
		
	private:
	
		/**
		* Constructor
		*/
		CPcsAlgorithm1();

		/**
		* 2nd phase construtor
		*/
		void ConstructL();
			
    private:		

        /**
        * Remove leading and trailing spaces of search query
        */
        void RemoveSpacesL ( CPsQuery& aQuery );

		/**
		* Replace occurance of "0" in ITU-T mode with space
		*/
		TBool ReplaceZeroWithSpaceL ( CPsQuery& aQuery );
			
		/**
		* Search function helper
		*/
		void DoSearchL ( const CPsSettings& aPcsSettings,
						 CPsQuery& aCondition,
						 RPointerArray<CPsData>& searchResults,
	                     RPointerArray<CPsPattern>& searchSeqs );
		
		/**                
		* Search function helper
		*/
		void DoSearchInputL ( CPsQuery& aQuery,
						 	  const TDesC& aData,
							  RPointerArray<TDesC>& aSearchSeqs,
							  RArray<TPsMatchLocation>& aMatchLocation );
		
		/**
		* Returns the index corresponding a URI in iDataStoreUri
		*/
		TInt FindStoreUri ( const TDesC& aDataStoreUri );

		/**
		* Function to return all cached content
		*/
		void GetAllContentsL ( const CPsSettings& aPcsSettings,
		                       RPointerArray<CPsData>& aSearchResults );
		    
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
		void FilterSearchResultsForGroupsL ( RArray<TInt>& aGroupContactIds,
							 	             RPointerArray<CPsData>& aSearchResults );

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
		void ReadSortOrderFromCenRepL ( const TDesC& aURI, 
		                                RArray<TInt>& aSortOrder );

		/** Write sort order for a data store to the central repository
		* @param aURI - The data store URI for which sort order is to be persisted
		* @param aSortOrder - The sort order to be persisted	
		*/
		void WriteSortOrderToCenRepL ( const TDesC& aURI, 
		                               RArray<TInt>& aSortOrder );	
		                         								
		/**
		* Utility function that sets the caching error value
		*/
		void SetCachingError ( const TDesC& aDataStore, 
		                       TInt aError );

		/**
		* Write the content required by client 
		*/
		CPsClientData* WriteClientDataL ( CPsData& aPsData );
			
		/**
		* Function to return data base URI for an internal identifier
		*/
		const TDesC& GetUriForIdL( TUint8 aUriId );
		
        /**
        * launch plugins by idle
        */
         void DoLaunchPluginsL();
        
        /**
        * launch plugins
        */
         static TInt DoLaunchPluginsL(TAny* aPtr);

    private:
    
		/**
		* Instance of contacts cache
		* Own
		*/
		RPointerArray<CPcsCache> iPcsCache; 

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
		CPsDataPluginInterface* iPsDataPluginInterface;

		/**
		* Instance of algorithm helper. Owned
		*/
		CPcsAlgorithm1Helper* iHelper;
		CPcsAlgorithm1MultiSearchHelper* iMultiSearchHelper;

		
		/**
		* Global that keeps the updated status of the caching
		*/
		TCachingStatus iCacheStatus;
		
		/**
		* Global that stores any error that occurs while caching
		*/
        TInt iCacheError;

        /**
        * plugin launcher, make the plugin instantiation async
        */
        CIdle* iPluginLauncher;
};
	

#endif // C_PCS_ALGORITHM_1
