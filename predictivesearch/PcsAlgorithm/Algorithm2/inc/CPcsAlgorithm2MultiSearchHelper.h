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
                             RPointerArray<CPcsPoolElement>& searchSet,
                             RPointerArray<CPsQuery>& searchQuery, TUint8 aFilteredDataMatch,
                             TBool isGroupSearch, RArray<TInt>& aContactsInGroup, 
                             TInt keyboardMode);

    /**
     * Convert the search query to a string.
     * If the mode is ITU-T, character is converted to numeric.
     * If the mode is Qwerty, character is retained as is.
     */
    void ExtractQueryL(CPsQuery& aPsQuery, TDes& aOutput);

    /**
     * Convert the input search query to a string.
     * Mode is referred in the CPsQuery.
     * If the mode is ITU-T, character is converted to numeric.
     * If the mode is Qwerty, character is retained as is.
     */
    void ExtractQueryL(TDesC& aInput, CPsQuery& aPsQuery, TDes& aOutput);
    /**
     * Converts the input data to the key board mode specified by the query.
     * Mode is referred in the CPsQuery.
     * If the mode is ITU-T, data is converted to numeric.
     * If the mode is Qwerty, data is retained as is.
     */
    void ConvertdDataToKeyBoardModeL(CPsQuery* aQuery, TPtrC aInputData,
                                     TBuf<KPsQueryMaxLen>& aOutputData);

public:

    /**
     * Search Function for multi query
     * Initials search feature
     */
    void SearchMultiL(const CPsSettings& aSettings, RPointerArray<CPsQuery>& aQuery, 
                      TBool isGroupSearch, RArray<TInt>& aContactsInGroup,
                      RPointerArray<CPsData>& searchResults,
                      RPointerArray<CPsPattern>& searchSeqs, TInt keyboardMode);

    /**
     * Funtion to Search matching sequences for multi query
     */
    void SearchMatchSeqMultiL(RPointerArray<CPsQuery>& aPsQuery,
                              TDesC& aData, RPointerArray<TDesC>& aMatchSet, 
                              RArray<TPsMatchLocation>& aMatchLocation);

private:

    /**
     * Constructs a bit pattern using the required/supported data fields
     * For example, 6, 4 and 27 are supported fields <-- 00000111
     *              6 and 4 are required fields      <-- 00000011
     * Bit pattern returned is 00000011.
     */
    TUint8 FilterDataFieldsL(RArray<TInt>& aRequiredDataFields,
                             RArray<TInt>& aSupportedDataFields);
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

    /**
     * Convert the search query (multiple) to a list
     */
    void ConvertQueryToListL(RPointerArray<CPsQuery>& aSearchQuery,
                             RPointerArray<HBufC>& aQueryList);

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
    CPcsKeyMap* keyMap;

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

#endif // C_PCS_ALGORITHM_2_MULTI_SEARCH_HELPER
