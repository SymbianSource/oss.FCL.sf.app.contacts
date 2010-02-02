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


#ifndef M_STORE_LIST_OBSERVER_H
#define M_STORE_LIST_OBSERVER_H

//  SYSTEM INCLUDES
#include <e32std.h>

//  USER INCLUDES

/**
 * An observer interface for addition/deletion of Data Stores for 
 * the Predictive Search. Predictive Search Alogrithm will receive 
 * events from the Data Stores via this interface.
 * 
 * @since S60 v3.2
 */ 
class MStoreListObserver
{

	public:

		/**
		* This method should be called by the Data Adapter whenever 
		* it has any new store that can be processed by the Algorithm
		* 
		* @param aDataStore The new database URI
		* 
		* @return           None 
		*
		*/
		virtual void AddDataStore ( TDesC& aDataStore ) = 0;

		/**
		* This method should be called by the Adapter when it wants to
		* remove any data Store from the  Algorithm cache. The removed store 
		* will be completely removed and will not be serached
		* 
		* @param aDataStore The database URI to be removed
		* @return - None 
		*/
		virtual void RemoveDataStore ( TDesC& aDataStore ) = 0;

};
    
#endif	// M_STORE_LIST_OBSERVER_H

// End of File

