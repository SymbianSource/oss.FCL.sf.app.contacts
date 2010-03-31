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
* Description:  Supports initial search feature
*
*/


#ifndef C_PCS_ALGORITHM_1_MULTI_SEARCH_HELPER
#define C_PCS_ALGORITHM_1_MULTI_SEARCH_HELPER

// SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32math.h>

// USER INCLUDES
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsQuery.h"
#include "CPcsAlgorithm1.h"
#include "CPcsDebug.h"
#include "CPcsKeyMap.h"
#include "CPsQueryItem.h"
#include "CPcsPoolElement.h"
#include "CWords.h"
#include "CPcsAlgorithm1FilterHelper.h"

// FORWARD DECLARATION
class CPcsAlgorithm1;
class CPsData;
class CPsQuery;
class TMultiSearchMap;

#define MAX_DATA_FIELDS 8

// CLASS DECLARATION
class CPcsAlgorithm1MultiSearchHelper : public CBase
{
	public: 
	
	    /**
		 * Two phase construction
		 */
		static CPcsAlgorithm1MultiSearchHelper* NewL(CPcsAlgorithm1* aAlgorithm);

        /**
		 * Destructor
		 */
		virtual ~CPcsAlgorithm1MultiSearchHelper();

		
    private:
    	
		/**
		 * Filter subset results for multi query mode. 
		 */
        void  FilterResultsMultiL(CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
        						  RPointerArray<CPcsPoolElement>& aSearchSet,
		                          RPointerArray<CPsQuery>& aSearchQuery,
		                          TUint8 aFilteredDataMatch,
		                          TBool aIsGroupSearch,
		                          RArray<TInt>& aContactsInGroup);

    public:

        /**
		 * Search Function for multi query
		 * Initials search feature
		 */
		void  SearchMultiL(const CPsSettings& aSettings,
		                   RPointerArray<CPsQuery>& aQuery,
		                   TBool aIsGroupSearch,
		                   RArray<TInt>& aContactsInGroup,
		                   RPointerArray<CPsData>& aSearchResults,
		                   RPointerArray<CPsPattern>& aSearchSeqs );		

		/**
		 * Funtion to Search matching sequences for multi query
		 */                   
		void  SearchMatchSeqMultiL(RPointerArray<CPsQuery>& aPsQuery,
	                            const TDesC& aData,
	                            RPointerArray<TDesC>& aMatchSeq,
	                            RArray<TPsMatchLocation>& aMatchLocation );		                   			                    	                    

		/**
		 * Funtion to Search match string for multi query
		 */
		void LookupMatchL( CPsQuery& aSearchQuery,
		        const TDesC& aData,
		        TDes& aMatchedData );

    private:
    
        /**
		 * Set the bits corresponding to word matches
		 *
		 * @param aIndex Index of data element
		 * @param aPosition Word matched for predictive search
		 */  
        void  SetWordMap( TInt aIndex, TInt aPosition);
        
        /**
		 * Checks if data at data index and word at word index
		 * is matched for predictive search
		 *
		 * @param aDataIndex Index of data element
		 * @param aWordIndex Word index
		 * @return TRUE if word matched else FALSE
		 */
        TBool  IsWordMatch(TInt aDataIndex, TInt aWordIndex);
        
        /**
         * Resets iWordMatches member variable
         */
        void ClearWordMatches();
        
    public:
        
        /**
         * Utility function that counts no of bit set in 32 bit number
         */
        TInt BitsSet32(TUint32 aData);
        
        /**
         * Utility function to count the number of sub-queries (words) in the query
         */
        TInt CountMultiQueryWordsL(CPsQuery& aQuery);

        /**
         * Utility function to split the query into multiple queries
         */
        RPointerArray<CPsQuery> MultiQueryL(CPsQuery& aQuery);
        
        /**
         * Convert the search query (multiple) to a list
         */
        void  ConvertQueryToListL(RPointerArray<CPsQuery>& aSearchQuery,
                                 RPointerArray<HBufC>& aQueryList);
        
	private:
	
	    /**
	     * Constructor
	     */
		CPcsAlgorithm1MultiSearchHelper();
		
		/**
		 * 2nd phase construtor
		 */
		void ConstructL(CPcsAlgorithm1* aAlgorithm);        
		                                                                          
    private:
    
		/**
		 * Pointer to algorithm instance. Not owned.
		 */
		CPcsAlgorithm1* iAlgorithm;

		/**
		 * Pointer to key map instance. Not owned.
		 */
		CPcsKeyMap* iKeyMap;
	    
		/**
         * Array of result sets from different data stores.
         */
         typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;
         RPointerArray<CPSDATA_R_PTR_ARRAY> iMultiSearchResultsArr; 
     	
     	/**
     	 * Array of word macthes.
     	 */
     	TUint8 iWordMatches[MAX_DATA_FIELDS];		 
};

#endif // C_PCS_ALGORITHM_1_MULTI_SEARCH_HELPER
