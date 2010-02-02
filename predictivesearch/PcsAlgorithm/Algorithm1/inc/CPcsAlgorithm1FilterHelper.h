/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class to filter search results.
*
*/


#ifndef C_PCS_ALGORITHM_1_FILTER_HELPER
#define C_PCS_ALGORITHM_1_FILTER_HELPER

// SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32math.h>
#include <e32hashtab.h>

// USER INCLUDES
#include "CPcsDefs.h"
#include "CPsData.h"
#include "CPcsAlgorithm1Utils.h"
#include "CPsPattern.h"

/*
* Internal structure to store the match patterns
* and mapping pool index
*/
struct TPsSubPattern
{
	HBufC* matchPattern;   // match pattern
	TInt   firstOccurenceInThisPattern; // First occurence of this pattern in this patterns

};

struct TPsPatternDetails
{
	HBufC* matchPattern;   // match pattern
	TInt   firstOccurence; // First occurence of this pattern in result set
	TInt   poolIndex; 
	RPointerArray<TPsSubPattern> subPatternArray;    
	
};

// CLASS DECLARATION
class CPcsAlgorithm1FilterHelper : public CBase
{
	public: 
	
	    /**
		 * Two phase construction
		 */
		static CPcsAlgorithm1FilterHelper* NewL(TSortType aSortType);

        /**
		 * Destructor
		 */
		virtual ~CPcsAlgorithm1FilterHelper();

	public:
	
		/**
		* Adds the result 
		*/
		void AddL(CPsData* psData, RPointerArray<TDesC>& aPatternSequence);
		
		/**
		* Returns the final list of results based on the given sort type
		*/
		void GetResults(RPointerArray<CPsData>& aSearchResults);
		
		/**
		* Returns the final match sequence list
		*/
		void GetPatternsL(RPointerArray<CPsPattern>& aPatternSet);
		
	private:
	
		/**
	     * Constructor
	     */
		CPcsAlgorithm1FilterHelper();
		
		/**
		 * 2nd phase construtor
		 */
		void ConstructL(TSortType aSortType);   
		
		/**
		* Adds each result into respective pool based on match pattern
		*/
		TInt AddToPoolL(CPsData* psData, RPointerArray<TDesC>& aPatternSequence);
		
		/**
		* Searches for for aSeq in iMatchPatternDetails and returns index 
        * where input sequence is found. Returns -1 if not found
		*/
		TInt FindSequence(TDesC* aSeq);
		
		
		/**
		* Searches for for seq in subpatterns of iMatchPatternDetails[aPatternIndex]
        * and returns index where input sequence is found. 
        * Returns -1 if not found
		*/
		TInt FindSubSequence(TDesC* aSeq,TInt aPatternIndex);
		
		/**
		* Creates the Matchpattern aSeq in  iMatchPatternDetails.
        * Returns the index of the sequence in iMatchPatternDetails
        */
		TInt CreateMatchPatternDetailsAndPoolsL(TDesC* aSeq);
		
		/**
		* Searches for all the subsequences of aSeq and updates the 
		* aFirstOccrVal in iMatchPatternDetails.
        */
		void UpdateForSubSequencesL(TDesC& aSeq,TInt aFirstOccrVal);

	private:
		
		/**
		* Own. Array of TPsPatternDetails structure
		*/
		RPointerArray<TPsPatternDetails> iMatchPatternDetails;
		
		/**
		* Keeps track number of results
		*/
		TInt32 iResultCount;
		
		/**
		* Own. Array of result pools based on match patterns
		*/
        RPointerArray<CPSDATA_R_PTR_ARRAY> iMatchPatternPools;
        
        /**
        * Sort type, either alphabetical or pattern based
        */
        TSortType iSortType;
};

#endif /* C_PCS_ALGORITHM_1_FILTER_HELPER */

// END OF FILE
