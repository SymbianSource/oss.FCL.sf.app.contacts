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
* Description:  ECom data adapters plugin interface definition
*
*/


#ifndef C_PS_DATAPLUGIN_H
#define C_PS_DATAPLUGIN_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// USER INCLUDES
#include <mdatastoreobserver.h>
#include <mstorelistobserver.h>
// CLASS DECLARATION

// Structure to hold PsData Plugin observers. 
// Need structure to pass as a parameter to ECOM interface
struct TPsDataPluginParams
{
	// Constructor
	TPsDataPluginParams( MDataStoreObserver* aDataStoreObserver, MStoreListObserver* aStoreListObserver )
	    : iDataStoreObserver( aDataStoreObserver ), iStoreListObserver( aStoreListObserver ) {}
	// Data
	MDataStoreObserver* iDataStoreObserver;
	MStoreListObserver* iStoreListObserver;
};

/**
* Data plug-ins (Ecom) interface definition.
* This class acts as an interface for all the data stores (which will be 
* searched by the PS Algorithm). The Data stores have to inherit from 
* this class.
* 
* @since S60 v3.2
*/
class CPsDataPlugin: public CBase
{

	public: 

		/**
		* Ecom interface static factory method implementation.
		* @param aImpUid Ecom's implementation uid
		* @param aObserverForDataStore  An observer instance for data store
		* @param aStoreListObserver     A store list observer instance
		* @return A pointer to the created instance of CPsDataPlugin
		*/
		static inline CPsDataPlugin* NewL( TUid aImpUid, MDataStoreObserver* aObserverForDataStore,
													MStoreListObserver* aStoreListObserver );

		/**
		* Ecom interface static factory method implementation.
		*
		* @param aImpUid Ecom's implementation uid
		* @param aObserverForDataStore  An observer instance for data store
		* @param aStoreListObserver     A store list observer instance
		* @return A pointer to the created instance of CPsDataPlugin
		*/
		static inline CPsDataPlugin* NewLC( TUid aImpUid, MDataStoreObserver* aObserverForDataStore,
													MStoreListObserver* aStoreListObserver );

		/**
		* Destructor
		*/
		virtual ~CPsDataPlugin();

	public: 

		/**
		* Gets the plugin id.
		*
		* @return Id of the plugin.
		*/   
		inline TUid PluginId() const;

		/**
		* Gets the supported data stores URIs
		* Implementation needs to be provided by the data adapters
		* 
		* @param aDataStores supported data stores URIs
		*         
		*/  
		virtual void GetSupportedDataStoresL( RPointerArray<TDesC> &aDataStoresURIs ) = 0;

		/**
		* Checks if the given data store is supported.
		* Implementation needs to be provided by the data adapters
		* 
		* @param aDataStoreURI data store
		* @return True if this store is supported        
		*/  
		virtual TBool IsDataStoresSupportedL( TDesC& aDataStoreURI ) = 0;

		/**
		* Gets the supported data fields
		* Implementation needs to be provided by the data adapters
		* 
		* @param aDataStores supported data fields
		*         
		*/  
		virtual void GetSupportedDataFieldsL( RArray<TInt>& aDataFields ) = 0;

		/**
		* Requests for data from this store
		* Implementation needs to be provided by the data adapters
		* The adapters need to pass an instance of the observer. The data 
		* to the algorithm is provided via the observer callback 
		*
		* @param aObserverForDataStore An observer instance used to
		*/      
		virtual  void  RequestForDataL(TDesC& aDataStoreURI )=0;      	    
		               
	private:    

		/**
		* The plugin id
		*/
		TUid iPluginId;

		/**
		* Used internally to create ECOM implementation
	    */
	    TUid iDtor_ID_Key;
    
};

#include "CPsDataPlugin.inl"

#endif		// C_PS_DATAPLUGIN_H

// End of File

