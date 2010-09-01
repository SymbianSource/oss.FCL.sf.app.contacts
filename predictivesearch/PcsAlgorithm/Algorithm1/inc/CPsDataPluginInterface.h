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
* Description:  
*
*/

#ifndef CPS_DATA_PLUGIN_INTERFACE_H
#define CPS_DATA_PLUGIN_INTERFACE_H

// INCLUDE FILES
#include <coemain.h>

#include "CPcsDefs.h"
#include "CPsData.h"
#include "mdatastoreobserver.h"
#include "mstorelistobserver.h"

// FORWARD DECLARATION	
class CPsDataPlugin;

/**
* Class is used to instantiate the required plugins when the search has been
* asked. Acts as interface to all Plugins.
*/

class CPsDataPluginInterface: public CBase 
{
	public:
		
		/** 
		* Symbian OS 2 phased constructor.
		* @param aObserverForDataStore  An observer instance for data store
		* @param aStoreListObserver     A store list observer instance
		* @return	A pointer to the created instance of CPsDataPluginInterface.
		*/
		static CPsDataPluginInterface* NewL(MDataStoreObserver* aObserverForDataStore,
											MStoreListObserver* aStoreListObserver);

		/** 
		* Symbian OS 2 phased constructor.
		* @param aObserverForDataStore  An observer instance for data store
		* @param aStoreListObserver     A store list observer instance
		* @return	A pointer to the created instance of CPsDataPluginInterface.
		*/
		static CPsDataPluginInterface* NewLC(MDataStoreObserver* aObserverForDataStore,
											MStoreListObserver* aStoreListObserver);

		/**
		 * Destructor.
		 */      
		virtual ~CPsDataPluginInterface();		

		/**
		* Instantiates the required data store plug-ins known by the ecom framework.	
		*/
		void InstantiateAllPlugInsL();

		/**
		* Unloads all plugins
		* This should internally call  REComSession::DestroyedImplementation( iDtor_ID_Key )
		* and then REComSession::FinalClose()
		*/
		void UnloadPlugIns(); 
			
		/**
		* Lists all implementations which satisfy this ecom interface
		*
		* @param aImplInfoArray On return, contains the list of available implementations
		* 
		*/   
		 inline void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
		 {
		     REComSession::ListImplementationsL( KPsDataStoreInterfaceUid, aImplInfoArray );
		 }

         /**
         * Requests the DataStore for data 
         */
		 void  RequestForDataL(TDesC& aDataStore);                              
                                     
 
         /**
         * Retrieve all the supported URIs from data adapters
         */
         void GetAllSupportedDataStoresL(RPointerArray<TDesC>& aDataStores);
         
         
         /**
         * Retrieve the supported data fields from adapters
         */
         void GetSupportedDataFieldsL(TDesC& aUri, RArray<TInt>& aDataFields);

 
    private: 
      	
	    /** 
	     * Performs the first phase of two phase construction.
	     */
	     CPsDataPluginInterface();  
    	
 		/** 
		* Symbian OS 2 phased constructor.
		*/
       	void ConstructL(MDataStoreObserver* aObserverForDataStore,
						MStoreListObserver* aStoreListObserver);
		
        /**
		* Instantiates a data store plug-in, knowing the implementation uid.
		* @param aImpUid imp uID
		*/
	    CPsDataPlugin* InstantiatePlugInFromImpUidL( const TUid& aImpUid, 
	    											 MDataStoreObserver* aObserverForDataStore,
													 MStoreListObserver* aStoreListObserver );
	    
	  
	private: 
	  
		TUid iDtor_ID_Key;

		// List of plugins that this interface will interact with
		RPointerArray<CPsDataPlugin>  iPsDataPluginInstances; 
		
		/**
	    *  An observer instance used to
	    *  send the data from adapters to data cache
	    */
		MDataStoreObserver* iObserverForDataStore;
		
		/**
	    * An observer instance used to send the datastore to the adapter
	    */
		MStoreListObserver* iStoreListObserver;
};

#endif // CPS_DATA_PLUGIN_INTERFACE_H

//End of File
