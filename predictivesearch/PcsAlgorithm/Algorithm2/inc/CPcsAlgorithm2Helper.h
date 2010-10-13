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

#ifndef C_PCS_ALGORITHM_2_HELPER
#define C_PCS_ALGORITHM_2_HELPER

// INCLUDES
#include <e32cmn.h>
#include <e32math.h>
#include <e32hashtab.h>

#include "CPcsPlugin.h"
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPcsAlgorithm2FilterHelper.h"
#include "mdatastoreobserver.h"

// FORWARD DECLARATION
class CPcsAlgorithm2;
class CPsData;
class CPsDataPluginInterface;

// CLASS DECLARATION
class CPcsAlgorithm2Helper : public CBase
    {
public:

    /**
     * Two phase construction
     */
    static CPcsAlgorithm2Helper* NewL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Destructor
     */
    virtual ~CPcsAlgorithm2Helper();

private:

    /**
     * Filter subset results for ITU-T Mode. Entire search query is 
     * entered in ITU-T mode.
     */
    void FilterResultsL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                        RPointerArray<CPcsPoolElement>& searchSet,
                        const TDesC& searchQuery, TBool isSearchInGroup,
                        RArray<TInt>& aContactsInGroup);
    /**
     * Filter subset results for Qwerty Mode. Entire search query is 
     * entered in Qwerty mode.
     */
    void FilterResultsQwertyL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                              RPointerArray<CPcsPoolElement>& searchSet,
                              const TDesC& searchQuery, TBool isSearchInGroup,
                              RArray<TInt>& aContactsInGroup);

    /**
     * Filter subset results for Mixed mode. Some portion of query is in 
     * ITU-T mode and some portion in Qwert mode.
     */
    void FilterResultsMixedL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                             RPointerArray<CPcsPoolElement>& searchSet, 
                             CPsQuery& searchQuery, TBool isSearchInGroup, 
                             RArray<TInt>& aContactsInGroup);

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

public:

    /**
     * Search Function for ITU-T style
     */
    void SearchITUL(const CPsSettings& aSettings, CPsQuery& aQuery,
                    TBool isGroupSearch, RArray<TInt>& aContactsInGroup,
                    RPointerArray<CPsData>& searchResults,
                    RPointerArray<CPsPattern>& searchSeqs);

    /**
     * Search Function for QWERTY style
     */
    void SearchQWERTYL(const CPsSettings& aSettings, CPsQuery& aQuery,
                       TBool isGroupSearch, RArray<TInt>& aContactsInGroup,
                       RPointerArray<CPsData>& searchResults,
                       RPointerArray<CPsPattern>& searchSeqs);

    /**
     * Search Function for mixed style
     * Some chars are entered in ITU-T and some in QWERTY
     */
    void SearchMixedL(const CPsSettings& aSettings, CPsQuery& aQuery,
                      TBool isGroupSearch, RArray<TInt>& aContactsInGroup,
                      RPointerArray<CPsData>& searchResults,
                      RPointerArray<CPsPattern>& searchSeqs);

    /**
     * Funciton to search matching sequences in the input text
     */
    void SearchMatchSeqL(HBufC* aQuery, TDesC& aData,
                         RPointerArray<TDesC>& aMatchSet, CPsQuery& aPsQuery, 
                         RArray<TPsMatchLocation>& aMatchLocation);

    /**
     * Sort search sequences
     */
    void SortSearchSeqsL(RPointerArray<TDesC>& aMatchSet);

private:

    /**
     * Constructs a bit pattern using the required/supported data fields
     * For example, 6, 4 and 27 are supported fields <-- 00000111
     *              6 and 4 are required fields      <-- 00000011
     * Bit pattern returned is 00000011.
     */
    TUint8 FilterDataFieldsL(RArray<TInt>& aRequiredDataFields,
                             RArray<TInt>& aSupportedDataFields);
private:

    /**
     * Constructor
     */
    CPcsAlgorithm2Helper();

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
    RPointerArray<CPSDATA_R_PTR_ARRAY> iSearchResultsArr;
    };

#endif // C_PCS_ALGORITHM_2_HELPER
