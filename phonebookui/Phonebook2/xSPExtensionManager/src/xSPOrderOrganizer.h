/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       xSP order organizer
*
*/


#ifndef __XSPORDERORGANIZER_H__
#define __XSPORDERORGANIZER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CxSPViewInfo;

class TxSPOrderUnit
	{
	public:
	
		/**
		 * Internalize the class data
		 *
		 * @param aStream Stream for internalizing
		 */	
		void InternalizeL( RReadStream& aStream );
		
		/**
		 * Externalize the class data
		 *
		 * @param aStream Stream for externalizing
		 */	
		void ExternalizeL( RWriteStream& aStream ) const;
		
	public: // data
	
		/// Own: xSP id
		TUint32 iId;
		/// Own: tab view old (unmapped) id
		TUint32 iViewId;	
	};

/**
 * xSP order organizer
 */
class xSPOrderOrganizer
    {
    public:
    
    	/**
		 *	Check and arrange xSP view order. Currently present extension
		 *	data is compared to the data from the previous start. Newly
		 *	installed extensions will go to the end of array.
		 *	
		 *	@param aViewIdMap array of view id mappings. Possibly
		 *			will be rearranged on return
		 *	@return error code
		 */
    	static TInt OrderCheck( RPointerArray<CxSPViewInfo>& aViewIdMap );
    	
    	/**
		 *	Reorganize xSP view order. Will be effective on Phonebook restart.
		 *	
		 *	@param aNewOrder xSP view ids in new order
		 *	@return error code
		 */
    	static TInt Reorganize( const RArray<TxSPOrderUnit>& aNewOrder );
    	
    private:
    
    	/**
		 *	Check and arrange xSP view order. Currently present extension
		 *	data is compared to the data from the previous start. Newly
		 *	installed extensions will go to the end of array.
		 *	
		 *	@param aViewIdMap array of view id mappings
		 */
    	static void OrderCheckL( RPointerArray<CxSPViewInfo>& aViewIdMap );
    
    	/**
    	 * Creates a new private folder defined by given parametres
    	 *
    	 * @param aSession file session to be used
    	 * @param aPath path of the folder to be created
    	 * @param aDrive Drive number to which the folder is to be created
    	 */
    	static TInt CreatePrivateFolder( RFs& aSession, 
    									TDes& aPath, 
    									TDriveNumber aDrive );
    	
    	/**
		 *	Makes new xSP view order. Currently present extension
		 *	data is compared to the data from the previous start. Newly
		 *	installed extensions will go to the end of array.
		 *	
		 *	@param aViewIdMap array of view id mappings
		 *	@param aNewOrder array placeholder for new order
		 */
    	static void MakeNewOrderL( RPointerArray<CxSPViewInfo>& aViewIdMap, 
    									RArray<TxSPOrderUnit>& aNewOrder );
    	    	
    	/**
		 *	Rearrange xSP views
		 *
		 *	@param aViewIdMap array of view id mappings
		 *	@param aNewOrder xSP view order
		 */
    	static void RearrangeExtensionsL( RPointerArray<CxSPViewInfo>& aViewIdMap,
    									const RArray<TxSPOrderUnit>& aNewOrder );
    	
    	/**
		 *	Write xSP view order to file
		 *
		 *	@param aNewOrder xSP view order
		 */
    	static void WriteOrderL( const RArray<TxSPOrderUnit>& aNewOrder );
    								
    	/**
		 *	Delete order file
		 *
		 *	@return error code
		 */
    	static TInt DeleteOrderFile();
    };


#endif // __XSPORDERORGANIZER_H__

// End of File
