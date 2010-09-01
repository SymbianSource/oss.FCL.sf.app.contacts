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
* Description:  Identifies a contact in cache pools
*
*/

#ifndef C_PCS_ALGORITHM_2_MULTI_SEARCH_HELPER
#define C_PCS_ALGORITHM_2_MULTI_SEARCH_HELPER

// SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32math.h>

// USER INCLUDES
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsQuery.h"
#include "CPcsAlgorithm2.h"
#include "CPcsDebug.h"
#include "CPcsKeyMap.h"
#include "CPsQueryItem.h"
#include "CPcsPoolElement.h"
#include "CWords.h"
#include "CPcsAlgorithm2FilterHelper.h"

// FORWARD DECLARATION
class CPcsAlgorithm2;
class CPsData;
class CPsQuery;
class TMultiSearchMap;

#define MAX_DATA_FIELDS 8

// CLASS DECLARATION
class CPcsAlgorithm2MultiSearchHelper : public CBase
    {
public:

    /**
     * Two phase construction
     */
    static CPcsAlgorithm2MultiSearchHelper* NewL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Destructor
     */
    virtual ~CPcsAlgorithm2MultiSearchHelper();

private:

    /**
     * Filter subset results for multi query mode. 
     */
    void FilterResultsMultiL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                             RPointerArray<CPcsPoolElement>& aSearchSet,
                             RPointerArray<CPsQuery>& aSearchQuery, 
                             TUint8 aFilteredDataMatch,
                             TBool aIsGroupSearch, 
                             const RArray<TInt>& aContactsInGroup);

public:

    /**
     * Search Function for multi query
     * Initials search feature
     */
    void SearchMultiL(const CPsSettings& aSettings, 
                      RPointerArray<CPsQuery>& aQuery, 
                      TBool aIsGroupSearch, 
                      const RArray<TInt>& aContactsInGroup,
                      RPointerArray<CPsData>& aSearchResults,
                      RPointerArray<CPsPattern>& aSearchSeqs);

    /**
     * Funtion to Search matching sequences for multi query
     */
    void SearchMatchSeqMultiL(RPointerArray<CPsQuery>& aPsQuery,
                              const TDesC& aData, 
                              RPointerArray<TDesC>& aMatchSet, 
                              RArray<TPsMatchLocation>& aMatchLocation);

private:

    /**
     * Set the bits corresponding to word matches
     *
     * @param aIndex Index of data element
     * @param aPosition Word matched for predictive search
     */
    void SetWordMap(TInt aIndex, TInt aPosition);

    /**
     * Checks if data at data index and word at word index
     * is matched for predictive search
     *
     * @param aDataIndex Index of data element
     * @param aWordIndex Word index
     * @return TRUE if word matched else FALSE
     */
    TBool IsWordMatch(TInt aDataIndex, TInt aWordIndex);

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
     * Utility function to split the query into multiple queries
     */
    RPointerArray<CPsQuery> MultiQueryL(CPsQuery& aQuery);

private:

    /**
     * Constructor
     */
    CPcsAlgorithm2MultiSearchHelper();

    /**
     * 2nd phase construtor
     */
    void ConstructL(CPcsAlgorithm2* aAlgorithm);

private:

    /**
     * Pointer to algorithm instance. Not owned.
     */
    CPcsAlgorithm2* iAlgorithm;

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
	
    /**
     * Counter of the matched contacts 
     */
    TInt iMaxCount;
    };

#endif // C_PCS_ALGORITHM_2_MULTI_SEARCH_HELPER
