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

//User Include
#include "psclienttestsuitedefs.h"

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
        static CTestSuiteInputData* NewL(CStifItemParser& aItem,CPSRequestHandler& aPsClientHandler);

        /**
        * Destructor.
        */
        virtual ~CTestSuiteInputData();

	public: // Functions

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
		* Pointer to the data element at the specified location
		*
		* @param aIndex Index of data element
		* @param aInputCacheData Pointer to data element
		*/
		void InputCacheData(TInt aIndex, cacheData& aInputCacheData) const;

		/**
		* Adds the cachedata to the array of input cache data
		*
		* @param aInputCacheData
		*/
		void AppendInputCacheDataL(const cacheData& aInputCacheData);

		/**
		* Returns the number of Input Cache Data elements
		*
		* @return Number of Input Cache Data elements
		*/
		TInt InputCacheDataCount() const;

		/**
		* Returns the array of cachedata of the result data
		*
		* @param aResultData Pointer to data element
		*/
		void ResultData(RPointerArray<cacheData>& aResultData) const;

		/**
		* Appends cachedata to the the aResultData
		*
		* @param aResultData
		*/
		void AppendResultDataL(const cacheData& aResultData);

		/**
		* Returns the number of ResultData elements
		*
		* @return Number of ResultData elements
		*/
		TInt ResultDataCount() const;

		/**
		* Returns the sort type
		*
		* @return sort Type
		*/
		TSortType GetSortType();

		/**
		* Returns the pointer to the uri at the specified location
		*
		* @param aIndex Index of data element
		* @return Pointer to Cache Uri
		*/
		TDesC& CacheUris(TInt aIndex) const;

		/**
		* Sets the cache uris
		*
		* @param aCacheUri Cache Uri descriptor to be searched
		*/
		void ApendCacheUrisL(RPointerArray<TDesC>& aCacheUriArray);

		/**
		* Returns the number of Cache Uris (data stores)
		*
		* @return Number of Cache Uris (data stores)
		*/
		TInt CacheUrisCount() const;

		/*
		* Returns the Input language (as specified in the configuration file)
		*/
		TLanguage LanguageInput();

		/*
		* Returns the expected result for the  Input language
		* (as specified in the configuration file)
		*/
		TBool LanguageInputExpectedResult();
		/*
		* Parses the input data
		*
		* @param aItem - The input test case name and parmeters
		*/
		void ParseInputL(CStifItemParser& aItem);

		/*
		* Creates contacts in the phonebook. The contact data is taken from iInputCacheData
		*/
		void CreateContactsL();

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

		/*
		* compares the search results with expected results
		*
		* @param aSettings - The search results
		*/
		TInt CompareResWithExpResL(RPointerArray<CPsClientData>& searchResults);
		TInt ExactMatchCompareInputDataWithResultDataL(RPointerArray<CPsClientData>& searchResults);

		/*
		* Deletes all the created contacts
		*/
		void DeleteAllCreatedContactsL();

		/*
		* Deletes contacts with specific ids
		*/
		void DeleteContactsWithIdsL(RArray<TInt>& aContactIdArray);

		/*
		* Returns the input search string
		*/
		TDesC& InputSearchString() const;

		/*
        * Returns the input search string
        */
        const TDesC& ExpectedResultString() const;

        /*
        * Returns the input boolean
        */
        TBool InputBoolean() const;

		/*
		* Return the group name to be searched (as specified in configuration file)
		*/
		TDesC& GroupToBeSearched() const;

	    /*
        * Tests if Id returns correct vpbklink
        *
        * @param aPsClientHandler -  PsClientHandler instance
        * @param srchId -  Id to be tested\
        * @return - ETrue if Id returns correct link, else EFalse
        */
        TBool TestVpbkLinkForIdL(CPSRequestHandler& aPsClientHandler,const CPsClientData& aSearchResult);

		/**
        * Returns ths sort order
        *
        * @param aSortOrder - The sort order
        */
	    void SortOrder(RArray<TInt>& aSortOrder);

	    /*
	    * Return array of expected result locations data
	    */
	    const RArray<TPsMatchLocation>& ExpectedLocations();

private: // functions

		/**
		* Default Constructor
		*/
		CTestSuiteInputData();

		/**
		* Second phase constructor
		*/
		void ConstructL(CStifItemParser& aItem,CPSRequestHandler& aPsClientHandler);

private:  // Private functions for internal Parsing data
		void ParseInputForContactsDataL(TDesC& aInput, TInt aType);
	    void AddSingleContactDataL(TDesC& aInput, TInt aType);
	    void ParseInputForMatchLocationsL(TDesC& aInput);
	    void ParseInputForDataStoresUriL(TDesC& aInput);
	    void GetCharacterSeparatedDataL(TDesC& aInput, TChar aSeparator, RPointerArray<TDesC> &aSeparatedData);
	    void ParseInputForSearchQueryL(TDesC& aInput);
        TBool CompareInputDataWithResultDataL(CPsClientData& aResData,cacheData aInputData);
        void ParseInputForDisplayFieldsL(TDesC& aInput);
	    void ParseInputInputSearchStringL(TDesC& aInput);
        void ParseInputExpectedResultStringL(TDesC& aInput);
        void ParseInputInputBoolean(TDesC& aInput);
	    void ParseInputForLanguageSupportL(TDesC& aInput);
	    void ParseInputForAddingGroupsL(TDesC& aInput);
	    void ParseInputForSearchWithinGroupL(TDesC& aInput);
		void ParseInputForSortOrderL(TDesC& aInput);
		TInt GetFieldIdFromVpbkIdentifierL(TDesC& aIdentifierAsString);
		void ConvUtf2UniL( const TDesC& aOriginal, TDes& aRes );

private: // data

        /*
		* Array of cache data elements
		*/
		RPointerArray<cacheData>  iInputCacheData;

		/*
		* Array of cache data elements of the expected results
		*/
		RPointerArray<cacheData>  iResultData;

		/*
		 * Array of expected result locations data
		 */
        RArray<TPsMatchLocation> iExpectedLocations;

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
		* The contacts handler. Performs the operations related to virtual phone book
		*/
		CPcsTestContactsHandler * iContactHandler;

		/*
		* The client handler
		*/
		CPSRequestHandler* iPsClientHandler;

    	/*
		* Flag indicating if the contacts have been created
		*/
    	TBool iContactAlreadyCreated;

    	/*
		* The list of display fields
		*/
    	RArray<TInt> iDisplayFields;

    	/*
		* The input search string
		*/
    	HBufC* iInputSearchString;

        /*
        * The expected result string
        */
        HBufC* iExpectedResultString;

        /*
        * The input boolean
        */
        TBool iInputBoolean;

    	/*
		* The input language id
		*/
    	TLanguage iLangId;

    	/*
		* Flag indicating if the language is supported
		*/
    	TBool iIsLangSupported;

    	/*
		* The group to be searched
		*/
    	HBufC* iGroupToBeSearched;

    	/*
		* The sort Type
		*/
    	TSortType iSortType;

    	/*
    	* The sort order
    	*/
    	RArray<TInt> iSortOrder;

    	/*
    	* The max number of results
    	*/
    	TInt iMaxNumberOfResults;

};

#endif // TEST_SUITE_INPUT_DATA_H

// End of file
