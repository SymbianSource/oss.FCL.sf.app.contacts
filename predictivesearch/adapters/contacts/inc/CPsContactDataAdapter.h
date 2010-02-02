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
* Description:  Predictive Contact Search Adapter
*
*/


#ifndef C_PS_CONTACT_ADAPTER_H
#define C_PS_CONTACT_ADAPTER_H

// SYSTEM INCLUDES
#include <e32hashtab.h>

//USER INCLUDES
#include "cpcscontactfetch.h"
#include "CPsDataPlugin.h"
#include "CPsData.h"

// FORWARD DECLARATION
class CPsData;

// CLASS DECLARATION

/**
 *  This class is the data adapter for the phone/sim/group contact. 
 *  This class manages data retrival from the virtual phonebook
 *  @lib pscontactsadapter.lib
 */
class CPsContactDataAdapter : public CPsDataPlugin 
{
	public: 
	
	    /**
		 * Two phase construction
		 */
		static CPsContactDataAdapter* NewL(TAny* aPsDataPluginParameters);

        /**
		 * Destructor
		 */
		virtual ~CPsContactDataAdapter();
		
	public:
		
		/**
	    * Requests for data from this store
	    * The adapters need to pass an instance of the observer. The data 
	    * to the algorithm is provided via the observer callback 
	    * @param aDataStoreURI - The store from which data is requested
	    */
		void  RequestForDataL(TDesC& aDataStoreURI );
		
		/**
	    * Checks if the given data store is supported.
	    * @param aDataStoreURI - data store
	    * @return True if this store is supported        
	    */  
	    TBool IsDataStoresSupportedL( TDesC& aDataStoreURI ) ;
	          
	    /**
	    * Gets the supported data stores URIs
	    * 
	    * @param aDataStores supported data stores URIs
	    *         
	    */  
	    void GetSupportedDataStoresL( RPointerArray<TDesC> &aDataStoresURIs );
	    
		/**
	    * Gets the supported data fields.
	    * @param aDataFields supported data fields.         
	    */ 
		void GetSupportedDataFieldsL(RArray<TInt> &aDataFields );
	
	private:
	
	    /**
	    * Constructor
	    */
		CPsContactDataAdapter();
		
		/**
		* 2nd phase constructor
		*/
		void ConstructL(MDataStoreObserver* aObserverForDataStore,
						MStoreListObserver* aStoreListObserver);
	
	private:
	
	    /**
	    * Instance of CPcsContactFetch for managing 
	    * data stores of this adapter
	    *
	    * Own
	    */
	    CPcsContactFetch* iContactFetcher;	
	    
	    /**
	    * Supported Data stores of this adapter
	    */
	    RPointerArray<TDesC> iSupportedDataStores;
	    
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
	

#endif // C_PS_CONTACT_ADAPTER_H
