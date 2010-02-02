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
* Description:  Utility class to hold data for predictive search.
*                Used to marshal data between the client, server and 
*                data plugins.
*
*/

#ifndef __C_PS_DATA_H__
#define __C_PS_DATA_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <e32math.h>

// CLASS DECLARATION

/**
 * Utility class to store data for predictive search.
 * This is the basic data class that is used to marshal data between 
 * the client, server and data plugins.
 *
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPsData : public CBase
{

public:

	/**
	* Two phase construction
	* @return Instance of CPsData
	*/
	IMPORT_C static CPsData* NewL();	 		

	/**
	* Destructor
	*/
	IMPORT_C ~CPsData();

	/**
	* Provides the unique identifier of this object
	*
	* @return Unique identifier of this object
	*/
	IMPORT_C TInt Id() const;

	/**
	* Sets the unique identifier of this object 
	* @param aId The unique identifier
	*/
	IMPORT_C void SetId(const TInt aId);


	/**
	* Provides the unique URI identifier of this object
	*
	* @return Unique URI identifier of this object
	*/
	IMPORT_C TUint8 UriId() const;

	/**
	* Sets the unique URI identifier of this object 
	* @param aUriId The unique URI identifier
	*/
	IMPORT_C void SetUriId(const TUint8 aUriId);

	/**
	* Pointer to the data element at the specified location
	*
	* @param aIndex Index of data element
	* @param Pointer to data element
	*/
	IMPORT_C HBufC* Data(TInt aIndex) const;

	/**
	* Sets the data at the specified location
	*
	* @param aIndex Index of data element
	* @param aData Data descriptor to be searched
	*/
	IMPORT_C void SetDataL(const TInt aIndex,
	       const TDesC& aData);

	/**
	* Writes 'this' to the stream
	*
	* @param aStream WriteStream with externalized contents
	*/
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;		

	/**
	* Initializes 'this' from stream
	*
	* @param aStream ReadStream with data contents to be internalized
	*/
	IMPORT_C virtual void InternalizeL(RReadStream& aStream);

	/**
	* Method to compare two CPsData objects  
	*
	* @param aObject1 Instance 1 to be compared
	* @param aObject2 Instance 2 to be compared
	* @return 0 if aObject1 == aObject2
	*       -1 if aObject1 < aObject2
	*        1 if aObject1 > aObject2
	*/
	IMPORT_C static TInt CompareByData ( const CPsData& aObject1, 
		     const CPsData& aObject2 );

	/**
	* Method to compare two CPsData objects based on unique Id
	*
	* @param aObject1 Instance 1 to be compared
	* @param aObject2 Instance 2 to be compared
	* @return ETrue if aObject1 == aObject2
	*         else EFalse
	*/				     
	IMPORT_C static TBool CompareById(const CPsData& aObject1, 
		     const CPsData& aObject2);

	/**
	* Checks if data at array index is matched for predictive search
	*
	* @param aIndex Index of data element
	* @return TRUE if matched for predictive search
	*        FALSE if not matched for predictive search
	*/
	IMPORT_C TBool IsDataMatch(TInt aIndex);

	/**
	* Sets data at array index has matched for predictive search
	*
	* @param aIndex Index of data element
	*/
	IMPORT_C void SetDataMatch(TInt aIndex);		

	/**
	* Returns the data match attribute
	*
	* @return Data Match Attribute
	*/
	IMPORT_C TUint8 DataMatch(); 

	/**
	* Clear all the bits set in data matches
	*/
	IMPORT_C void ClearDataMatches();

	/**
	* Returns the number of data elements
	*
	* @return Number of data elements
	*/
	IMPORT_C TInt DataElementCount() const;

	/**
	* Returns the data extension
	*
	* @return Pointer to data extension if supported. Else NULL.
	*/
	IMPORT_C TAny* DataExtension() const;

	/**
	* Set the data extension
	*
	* @param aDataExt Data Extension for this object
	*/
	IMPORT_C void SetDataExtension(TAny* aDataExt);

	/**
	* Add element to the Integer Data Extention 
	*
	* @param aDataExt - Integer Data Extension to be added
	*/
	IMPORT_C void AddIntDataExtL(TInt aDataExt);

	/**
	* Remove element from the Integer Data Extention
	*
	* @param aIndex - Integer Data Extension array index 
	*/
	IMPORT_C void RemoveIntDataExt(TInt aIndex);

	/**
	* Retrieve Integer Data Extention Array
	*
	* @param aDataExtArray - Integer Data Extension array  
	*/

	IMPORT_C void IntDataExt(RArray<TInt>& aDataExtArray);

private:

	/**
	* Default Constructor
	*/
	CPsData();

	/**
	* Second phase constructor
	*/
	void ConstructL();		
			
private:	
				
	/**
	* Unique id 
	*/
	TInt iId;	

	/**
	* URI ID. Indicates the data source.
	*/
	TUint8 iUriId;

	/**
	* Indicates in bit positions, what data fields 
	* are matched for predictive search
	*/	
	TUint8 iDataMatches;		

	/**
	* Data fields
	*/
	RPointerArray<HBufC>* iData;

	/**
	* Extended Data field
	*/	
	TAny* iDataExtension; 

	/**
	* Extended Data field of Integers
	*/
	RArray<TInt>* iIntDataExt;
	
	/**
	* Garbage collector to fix performance issue
	*/
	RPointerArray<HBufC>* iGc;
};

#endif // __C_PS_DATA_H__

// End of file
