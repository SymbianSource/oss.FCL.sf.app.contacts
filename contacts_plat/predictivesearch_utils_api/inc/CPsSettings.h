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
* Description:  Utility class to store settings for predictive search.
*                Used to set the desired data stores to search and 
*                the display fields for predictive search.
*
*/

#ifndef __CPS_SETTINGS_H__
#define __CPS_SETTINGS_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

// CLASS DECLARATION

/**
 * Utility class to store settings for predictive search.
 *
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPsSettings : public CBase
{
	public:

		/**
		* Two phase construction.
		*
		* @return Instance of CPsSettings
		*/
		IMPORT_C static CPsSettings* NewL();		

		/**
		* Destructor.
		*/
		IMPORT_C ~CPsSettings();

		/**
		* Creates a clone of the settings
		*/
		IMPORT_C CPsSettings*  CloneL() const;

		/**
		* Sets the list of URIs to search from.
		*
		* @param aSearchUri The list of database URIs to be searched. 
		*/
		IMPORT_C void SetSearchUrisL(const RPointerArray<TDesC>& aSearchUri);

		/**
		* Sets the list of fields to display.
		*
		* @param aDisplayFields The list of fields to display.
		*/
		IMPORT_C void SetDisplayFieldsL(const RArray<TInt>& aDisplayFields);

		/**
		* Sets the maximum number of elements (contacts) to be given to client.
		* If number is set to -1: The entire search result will be returned.
		* Otherwise: Top N search results will be returned
		*
		* @param aMaxResults The maximum number of elements.
		*/	
		IMPORT_C void SetMaxResults(const TInt aMaxResults);

		/**
		* Returns the list of URIs configured to search from.
		*
		* @return aSearchUri - Contains the Uris that will be searched
		*/
		IMPORT_C void SearchUrisL(RPointerArray<TDesC>& aSearchUri) const;

		/**
		* Sets the sort type based on which sorting will be done
		*/
		IMPORT_C void SetSortType(const TSortType aSortType);
		
		/*
		* Returns the list of fields to display.
		*
		* @return aDisplayFields - Contains the fileds that will be displayed 
		*/
		IMPORT_C void DisplayFieldsL(RArray<TInt>& aDisplayFields) const;

		/**
		* Returns the maximum number of elements (contacts) to be given to client.
		*
		* @return TInt The maximum number of elements.
		*/	
		IMPORT_C TInt MaxResults() const;

		/**
		* Returns the GroupId from the settings
		*
		* @param - aGroupIdArray - Array of GroupIds
		* @return - none 
		*/	
		IMPORT_C void GetGroupIdsL(RArray<TInt>& aGroupIdArray);
		
		/**
		* Returns the sort type
		*/
		IMPORT_C TSortType GetSortType() const;
		
		/**
		* Writes 'this' to the stream.
		*
		* @param aStream WriteStream with externalized contents.
		*/
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;		

		/**
		* Initializes 'this' from stream.
		*
		* @param aStream ReadStream with data contents to be internalized.
		*/
		IMPORT_C virtual void InternalizeL(RReadStream& aStream);

	private:

		/**
		* Default Constructor
		*/
		CPsSettings();

		/**
		* Second phase constructor
		*/
		void ConstructL();		
			
	private:	
			
		/**
		* Database URIs
		*/
		RPointerArray<TDesC> iSearchUri;

		/**
		* Display fields
		*/
		RArray<TInt> iDisplayFields;

		/**
		* Maximum number of results that will be displayed to the client
		* (Top N contacts will be displayed)
		*/
		TInt iMaxResults;
		/**
		* Sort type whether to sort based on alphabets or on match patterns
		*/
		TSortType iSortType;
	
};

#endif // __CPS_SETTINGS_H__

// End of file
