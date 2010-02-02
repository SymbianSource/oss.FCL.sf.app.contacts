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
* Description:  Parser of the data received from configuration file
*
*/


#ifndef TEST_SUITE_INPUT_DATA_H
#define TEST_SUITE_INPUT_DATA_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <CPsQuery.h>
#include <CPsSettings.h>
#include <StifParser.h>
#include <CPsClientData.h>
#include <CPsRequestHandler.h>
#include <CPcsDefs.h>

// USER INCLUDES
#include "psutilstestsuitedefs.h"

//Forward Declaration
class CPcsTestContactsHandler;

/**
 *  This class performs parsing of the data input from the configuration file
 *
 *  @since S60 v3.2
 */
class CTestSuiteInputData :public CBase
{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestSuiteInputData* NewL(CStifItemParser& aItem);

        /**
        * Destructor.
        */
        virtual ~CTestSuiteInputData();	
   
	public: // Functions
		/**
		* Returns the unique id
		*
		* @return Id 
		*/
		TInt Id();
		
		/**
		* Returns the contacts data
		*
		* @param aDataArray  The contact data
		*/
		void Data(RPointerArray<TDesC> & aDataArray);
		
		/**
		* Provides the Keyboard mode of this object
		*
		* @return Keyboard mode of this object
		*/
		TDesC& KeyboardMode() const;

		/**
		* Sets the Keyboard mode of this object 
		*
		* @param aKeyboardMode The Keyboard mode 
		*/
		void SetKeyboardModeL(const TDesC& aKeyboardMode);
		
		/**
		* Returns the search query, as specified in the configuration parameter
		*
		* @return Query
		*/
		TDesC& Query() const;

		/**
		* Sets the search query 
		*
		* @param aQuery	The Query 
		*/
		void SetQueryL(const TDesC& aQuery);
		
		/**
		* Provides the Sort Type of this object
		*
		* @return Sort Type of this object
		*/
		TSortType SortType() const;
		
		/**
		* Sets the Sort Type of this object 
		*
		* @param aSortType The Sort Type
		*/
		void SetSortType(TSortType aSortType);
		
		/**
		* Returns the pointer to the uri at the specified location
		*
		* @param aIndex Index of data element
		* @return Pointer to Cache Uri
		*/
		TDesC& CacheUriAt(TInt aIndex) const;

		/**
		* Returns the Cache Uris
		*
		* @param aUriArray  The Cache Uris
		*/
		void CacheUris(RPointerArray<TDesC> & aUriArray);
	
		/**
		* Sets the cache uris
		*
		* @param aCacheUri Cache Uri descriptor to be searched
		*/
		void ApendCacheUrisL(RPointerArray<TDesC>& aCacheUriArray);  
		
		/**
		* Returns the Display Fields
		*
		* @param aDisplayFields  The Display Fields
		*/
		void DisplayFields(RArray<TInt>& aDisplayFields);
		
		/**
		* Returns the Group Ids
		*
		* @param aGroupId  The Group Ids
		*/
		void GroupIds(RArray<TInt>& aGroupId);
	
		/*
		* Parses the input data 
		* 
		* @param aItem - The input test case name and parmeters
		*/
		void ParseInputL(CStifItemParser& aItem);
		
	
		/*
		* Creates the search query
		* 
		* @param aPsQuery - The search query as needed by the ps engine 
		* @param aQueryBuf - The input search query 
		*/
		void CreateSearchQueryL(CPsQuery& aPsQuery,TDesC& aQueryBuf);
	
		/*
		* Performs the search settings
		* 
		* @param aSettings - The search settings
		*/
		void PerformSearchSettingsL(CPsSettings& aSettings);
		TInt CompareByDataL( const CPsClientData& aObject1, const CPsClientData& aObject2 );

		
		

private: // functions
	
		/**
		* Default Constructor
		*/
		CTestSuiteInputData();

		/**
		* Second phase constructor
		*/
		void ConstructL(CStifItemParser& aItem);
		
		
private:  //  Private functions for internal Parsing data
		    
	    void ParseInputForDataStoresUriL(TDesC& aInput);
	    void GetCharacterSeparatedDataL(TDesC& aInput,TChar aSeparator, RPointerArray<TDesC> &aSeparatedData);
	    void ParseInputForSearchQueryL(TDesC& aInput);
        
	    void ParseInputForDisplayFieldsL(TDesC& aInput);

		void ParseInputForIdL(TDesC& aInput);
		void ParseInputForData(TDesC& aInput);
		void ParseInputForSortTypeL(TDesC& aInput);
		void ParseInputForGroupIdsL(TDesC& aInput);

private:  // data
		
		/*
		* The contact id
		*/
		TInt iId;
		
		/* 
		* Array of cache uris to be searched 
		*/
		RPointerArray<TDesC>  iCacheUris;
		
		/* 
		* Keyboard mode 
		*/
		HBufC* iKeyboardModeBuf; 
		
		/* 
		* The search query
		*/
		HBufC* iQueryBuf;
		

    	/* 
		* The list of display fields
		*/
    	RArray<TInt> iDisplayFields;
    	
    	
    	/* 
		* The input contact Data
		*/
    	RPointerArray<TDesC>  iDataArray;
		
		/* 
		* The input sort type
		*/
    	TSortType  iSortType;
    	
    	/* 
		* The list of group ids
		*/
    	RArray<TInt> iGroupIds;
    	

};

#endif // TEST_SUITE_INPUT_DATA_H

// End of file
