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
* Description:  Client side class to hold the result of predictive search.
*
*/

#ifndef __C_PS_CLIENT_DATA_H__
#define __C_PS_CLIENT_DATA_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <e32math.h>

//USER INCLUDES

// CLASS DECLARATION
/**
 * This is a client side class to hold the results of search
 *
 * @lib PsServerClientAPI.lib
 * @since S60 v3.2
 */
class CPsClientData: public CBase
{
	public:
	
		/**
		* Two phase construction
		* @return Instance of CPsClientData
		*/
		IMPORT_C static CPsClientData* NewL();	 		

		/**
		* Destructor
		*/
		IMPORT_C ~CPsClientData();
		
	public:
		
		/**
		* Makes a clone of the current object
		*
		* @return The clone
		*/
		IMPORT_C CPsClientData* CloneL() const;
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
		* Provides the URI of this object
		*
		* @return Unique URI of this object
		*/
		IMPORT_C HBufC* Uri() const;

		/**
		* Sets the unique URI of this object 
		* @param aUri The unique URI 
		*/
		IMPORT_C void SetUriL(const TDesC& aUri);
		
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
		* Returns the number of data elements
		*
		* @return Number of data elements
		*/
		IMPORT_C TInt DataElementCount() const;

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
		IMPORT_C void SetDataExtensionL(TAny* aDataExt);
		
		/**
		* Sets the contact as bookmarked
		*/
		IMPORT_C void SetMark();
		
		/**
		* Returns ETrue if the contact is bookmarked, else EFalse
		*/
		IMPORT_C TBool IsMarked() const;
		
		/**
		* Sets FieldMatch attribute
		*
		* @param aFieldMatch 
		*/
		IMPORT_C void SetFieldMatch(TUint8 aFieldMatch);		

		/**
		* Returns the Field match attribute
		*
		* @return FieldMatch Attribute
		*/
		IMPORT_C TUint8 FieldMatch() const; 
	
	private:    // functions
	
		/**
		* Default Constructor
		*/
		CPsClientData();

		/**
		* Second phase constructor
		*/
		void ConstructL();
		
		/**
		* Second phase constructor
		*/
		void ConstructL(const CPsClientData& aRhs);

	
	private:	// data members
				
		/**
		* Unique id 
		*/
		TInt iId;	

		/**
		* URI. Indicates the data source.
		*/
		HBufC* iUri;

		/**
		* Data fields
		*/
		RPointerArray<HBufC>* iData;
		
		/**
 		* Indicates in bit positions, what data fields 
		* are matched for predictive search
		*/	
		TUint8 iFiledMatches;

		/**
		* Extended Data field
		*/	
		TAny* iDataExtension; 
		
		/**
		* Field to indicate if a contact is bookmarked or not
		*/
		TBool iMarked;

};

#endif // __C_PS_CLIENT_DATA_H__
