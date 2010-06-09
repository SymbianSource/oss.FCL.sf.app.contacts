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
* Description:  Predictive Contact Search Algorithm 1 main class
*
*/


#ifndef C_PCS_ALGORITHM_1_UTILS
#define C_PCS_ALGORITHM_1_UTILS

// INCLUDE FILES

#include <e32base.h>
#include <e32hashtab.h>


// FORWARD DECLARATION
class CPsData;
class CPsQuery;

typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;


// CLASS DECLARATION
class CPcsAlgorithm1Utils : public CBase
{
	public: 

		/**
		 * Merge function to merge the search results from different arrays
		 * into a single array in sorted order.
		 * It merges all the result sets that are there in iSearchResultsArr
         * into single result set. This is a wrapper funciton implemented that
         * works for all type of key board modes
		 */
		static void FormCompleteSearchResultsL(RPointerArray<CPSDATA_R_PTR_ARRAY>& aSearchResultsArr, 
										       RPointerArray<CPsData>& SearchResults);

        /**
         * Compare by character.
         */
		static TInt CompareByCharacter(const TChar& aFirst, const TChar& aSecond);

		/**
		 * Compare by length.
		 */
		static TInt CompareByLength(const HBufC& aFirst, const HBufC& aSecond);

		/**
		 * Compare strings exactly case sensitively.
		 */
		static TBool CompareExact(const TDesC& aFirst, const TDesC& aSecond);

		/**
		 * Compare strings with collate rules depending on locale.
		 */
		static TInt CompareCollate(const TDesC& aFirst, const TDesC& aSecond);

		/**
		 * Compare for keys and strings:
		 * - Case sensitive compare for keys,
		 * - Case insensitive and language dependent compare for Contact Data and Query.
		 */
		static TBool MyCompareKeyAndString(const TDesC& aContactString,
		                                   const TDesC& aQueryAsString,
		                                   CPsQuery& aPsQuery);

		/**
         * Case Sensitive compare for keys
         */
		static TBool MyCompareK(const TDesC& aLeft, const TDesC& aRight, CPsQuery& aPsQuery);

		/**
         * Customized CompareC
         */
        static TInt MyCompareC(const TDesC& aLeft, const TDesC& aRight);
		
        
        /**
         * TLinearOrder rule for comparison of data objects
         */
        static TInt CompareDataBySortOrderL(const CPsData& aObject1,
                                            const CPsData& aObject2);

        /**
         * Trim off all white spaces and special chars
         */
        static void MyTrim(TDes& aString);
        
        
        /**
         * Check if the input URI is of contact search in a group template form
         */
        static TBool IsGroupUri(TDesC& aURI);

        /**
         * Constructs a bit pattern using the required/supported data fields
         * For example, 6, 4 and 27 are supported fields <-- 00000111
         *              6 and 4 are required fields      <-- 00000011
         * Bit pattern returned is 00000011.
         */
        static TUint8 FilterDataFieldsL(const RArray<TInt>& aRequiredDataFields,
                                        const RArray<TInt>& aSupportedDataFields);
    
        /**
         * Create upper-case match sequence buffer from matching part and append it to
         * array unless there's equal sequence already present.
         */
        static void AppendMatchToSeqL( RPointerArray<TDesC>& aMatchSeq, const TDesC& aMatch );
};


// CleanupStack helpers for item owning RPointerArrays
template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef)
        { CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef)); }
private:
    inline static void ResetAndDestroy(TAny *aPtr)
        { static_cast<T*>(aPtr)->ResetAndDestroy(); }
    };

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
    { CleanupResetAndDestroy<T>::PushL(aRef); }

#endif // C_PCS_ALGORITHM_1_UTILS
    
