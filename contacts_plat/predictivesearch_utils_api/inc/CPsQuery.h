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
* Description:  Utility class to hold the search query for predictive search.
*                Holds an array of CPsQueryItem objects.
*
*/

#ifndef __CPS_QUERY_H__
#define __CPS_QUERY_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

// CLASS FORWARD DECLARATION
class CPsQueryItem;

// CLASS DECLARATION

/**
 * Utility class to hold the search sequence for predictive search. The Max length of search sequence = KPsQueryMaxLen.
 *
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPsQuery : public CBase
{
	public:

		/**
		* Two phase construction
		*
		* @return Instance of CPsQuery
		*/
		IMPORT_C static CPsQuery* NewL();	 		

		/**
		* Destructor
		*/
		IMPORT_C ~CPsQuery();

		/**
		* Insert a query item to the search sequence (if the current length < KPsQueryMaxLen) 
		*
		* @param aQueryItem The object pointer to be inserted
		* @param aIndex     The position within the array where the object 
		*                   pointer is to be inserted
		*/
		IMPORT_C void InsertL(const CPsQueryItem& aQueryItem, TInt aIndex);

		/**
		* Appends a query item to the search sequence (if the current length < KPsQueryMaxLen)
		*
		* @param aQueryItem The object pointer to be appended
		*/
		IMPORT_C void AppendL(const CPsQueryItem& aQueryItem);

		/**
		* Returns the query item at the specified index
		*
		* @param aIndex  
		* @return Instance of CPsQueryItem
		*/
		IMPORT_C CPsQueryItem& GetItemAtL(TInt aIndex);

		/**
		* Returns the search sequence as a single string (This does not 
		*   return the keyboard mode)
		*  
		* @return Entire search query as a string
		*/
		IMPORT_C TDesC& QueryAsStringLC();

		/**
		* Removes the query item at the specified index
		*
		* @param aIndex The position within the array from where the 
		*               object pointer is to be removed.
		*/
		IMPORT_C void Remove(TInt aIndex);

		/**
		* Deletes the entire search query
		*/
		IMPORT_C void Reset();

		/**
		* Returns the keyboard input mode for the search query
		*		if all the keys are entered in the same mode
		*		then returns the mode e.g. ITU-T, QWERTY
		*      else returns unspecified mode
		*  
		* @return TKeyboardModes
		*/
		IMPORT_C TKeyboardModes KeyboardModeL();

		/**
		* Returns the length of the query
		*  
		* @return number of CPsQueryItem in the array
		*/
		IMPORT_C TInt Count();

		/**
		* Prints the query as array of query items
		* Used only for debugging. 
		*/	 
		IMPORT_C void PrintQuery();

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


	private:

		/**
		* Default Constructor
		*/
		CPsQuery();

		/**
		* Second phase constructor
		*/
		void ConstructL();		
					
		private:	
					
		/**
		* The search query.
		*/

		RPointerArray<CPsQueryItem> iSearchQuery;

};

#endif // __CPS_QUERY_H__

// End of file
