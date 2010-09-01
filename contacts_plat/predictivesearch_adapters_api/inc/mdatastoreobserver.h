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
* Description:  An observer interface for Data Stores of the Predictive Search.
*
*/


#ifndef M_DATASTORE_OBSERVER_H
#define M_DATASTORE_OBSERVER_H

//  SYSTEM INCLUDES
#include <e32std.h>

//  USER INCLUDES
#include <CPsData.h>
#include <CPcsDefs.h>

/**
 * An observer interface for Data Stores of the Predictive Search.
 * Predictive Search Alogrithm will receive events from the Data Stores 
 * via this interface.
 * 
 * @since S60 v3.2
 */ 
class MDataStoreObserver
{

	public:

		/**
		* This method should be called by the Data Store whenever it has
		* any data ready that can be processed by the Algorithm
		* 
		* @param aDataStore The database URI
		* @param aData      The Data to be sent to the algorithm 
		*                   (to be stored in cache.)
		* @return           None 
		*
		* NOTE: The format of the data should be as defined in CPsData. 
		*/
		virtual void AddData ( TDesC& aDataStore, CPsData* aData ) = 0;

		/**
		* This method should be called by the Data Store when it wants to
		* remove any data from the  Algorithm cache. The removed data will 
		* not be searched in further searches by the algorithm.
		* 
		* @param aDataStore The database URI
		* @param aItemId    The unique Id of the item to be removed
		* @return - None 
		*     
		* NOTE : Each entry in the cache is identified by a unique Id 
		* (unique for that Data Store)
		*/
		virtual void RemoveData ( TDesC& aDataStore, TInt aItemId ) = 0;	

		/**
		* Removes all the contacts for a particular datastore. Equivalent
		* of clearing the cache for a datastore.
		*
		* @param aDataStore The database URI
		*/
		virtual void RemoveAll ( TDesC& aDataStore ) = 0;

		/**
		* Updates the caching status of a particular data store
		*
		* @param aDataStore The database URI
		* @param aStatus    status of the cache
		*/
		virtual void UpdateCachingStatus(TDesC& aDataStore, TInt aStatus) = 0;
		
};
    
#endif	// M_DATASTORE_OBSERVER_H

// End of File

