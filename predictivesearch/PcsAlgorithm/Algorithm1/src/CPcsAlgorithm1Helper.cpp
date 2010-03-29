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
* Description:  Predictive Contact Search Algorithm 1 helper class
*
*/


// INCLUDES

#include "CPcsAlgorithm1.h"
#include "CPcsAlgorithm1Helper.h"
#include "CPcsAlgorithm1Utils.h"
#include "CPcsDebug.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsData.h"
#include "CWords.h"
#include "CPsQuery.h"
#include "CPsQueryItem.h"
#include "CPsDataPluginInterface.h"
#include "CPcsPoolElement.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1Helper* CPcsAlgorithm1Helper::NewL(CPcsAlgorithm1* aAlgorithm)
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::NewL") );

    CPcsAlgorithm1Helper* self = new ( ELeave ) CPcsAlgorithm1Helper();
    CleanupStack::PushL( self );
    self->ConstructL(aAlgorithm);
    CleanupStack::Pop( self );

    PRINT ( _L("End CPcsAlgorithm1Helper::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::CPcsAlgorithm1Helper
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1Helper::CPcsAlgorithm1Helper()
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::CPcsAlgorithm1") );
    PRINT ( _L("End CPcsAlgorithm1Helper::CPcsAlgorithm1") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm1Helper::ConstructL(CPcsAlgorithm1* aAlgorithm)
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::ConstructL") );

    iAlgorithm = aAlgorithm;
    iKeyMap = iAlgorithm->GetKeyMap();

    PRINT ( _L("End CPcsAlgorithm1Helper::ConstructL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::CPcsAlgorithm1Helper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm1Helper::~CPcsAlgorithm1Helper()
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::~CPcsAlgorithm1Helper") );
    iSearchResultsArr.ResetAndDestroy();
    PRINT ( _L("End CPcsAlgorithm1Helper::~CPcsAlgorithm1Helper") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::SearchSingleL
// Search function for query in ITU-T mode, QWERTY mode, or Mixed (ITU-T and QWERTY) mode.
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::SearchSingleL(const CPsSettings& aSettings,
                                       CPsQuery& aPsQuery,
                                       TBool isSearchInGroup,
                                       RArray<TInt>& aContactsInGroup,
                                       RPointerArray<CPsData>& searchResults,
                                       RPointerArray<CPsPattern>& searchSeqs )
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::SearchSingleL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1Helper::SearchSingleL") );

    // Create filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm1FilterHelper* filterHelper = CPcsAlgorithm1FilterHelper::NewL(sortType);

    // Search based on first character
    TInt numValue = iKeyMap->PoolIdForCharacter(aPsQuery.GetItemAtL(0).Character());

    // Reset the result set array for new search
    iSearchResultsArr.ResetAndDestroy();

    // Get the data stores
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    aSettings.DisplayFieldsL(requiredDataFields);

    // Perform search for each required data store
    RPointerArray<CPcsPoolElement> elements;

    for ( int dsIndex = 0;
          dsIndex < aDataStores.Count();
          dsIndex++ )
    {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(aDataStores[dsIndex]));
        if ( arrayIndex < 0 ) continue;
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(numValue,elements);

        // Get the supported data fields for this data store
        RArray<TInt> supportedDataFields;
        cache->GetDataFields(supportedDataFields);

        // Get the filtered data fields for this data store
        TUint8 filteredDataMatch = FilterDataFieldsL(requiredDataFields,
                                                     supportedDataFields);

        // Perform filtering
        FilterResultsSingleL(filterHelper,
                             elements,
                             aPsQuery,
                             filteredDataMatch,
                             isSearchInGroup,
                             aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore
        if ( sortType == EAlphabetical )
        {
            filterHelper->GetResults(*(iSearchResultsArr[dsIndex]));
        }

        elements.Reset();
        supportedDataFields.Reset();
    }

    aDataStores.ResetAndDestroy();
    requiredDataFields.Reset();

    // If alphabetical sorting, merge the result sets of all datastores
    if ( sortType == EAlphabetical )
    {
        // Merge the result sets of individual datastores alphabetically
        CPcsAlgorithm1Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        searchResults);
    }
    else
    {
        // Results are already sorted pattern based
        filterHelper->GetResults(searchResults);
    }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(searchSeqs);

    PRINT1 ( _L("CPcsAlgorithm1Helper::SearchSingleL: Number of search results = %d"), searchResults.Count() );

    // Cleanup
    for ( TInt i = 0; i < iSearchResultsArr.Count(); i++ )
    {
        iSearchResultsArr[i]->Reset();
        delete iSearchResultsArr[i];
        iSearchResultsArr[i] = NULL;
    }
    iSearchResultsArr.Reset();

    delete filterHelper;

    __LATENCY_MARKEND ( _L("CPcsAlgorithm1Helper::SearchSingleL") );

    PRINT ( _L("End CPcsAlgorithm1Helper::SearchSingleL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::FilterResultsSingleL
// Subset search function
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::FilterResultsSingleL(CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
                                                 RPointerArray<CPcsPoolElement>& searchSet,
                                                 CPsQuery& aPsQuery,
                                                 TUint8 aFilteredDataMatch,
                                                 TBool isSearchInGroup,
                                                 RArray<TInt>& aContactsInGroup)
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::FilterResultsSingleL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1Helper::FilterResultsSingleL") );

    // Convert the search query to char-and-key string
    TBuf<KPsQueryMaxLen> queryAsDes;
    iKeyMap->GetMixedKeyStringForQueryL( aPsQuery, queryAsDes );

    PRINTQUERY ( _L("CPcsAlgorithm1Helper::FilterResultsSingleL: "), aPsQuery );

    PRINT1 ( _L("CPcsAlgorithm1Helper::FilterResultsSingleL: char-and-key string for the query is \"%S\""),
             &queryAsDes );      

    // Parse thru each search set elements and filter the results
    for ( TInt index = 0; index < searchSet.Count(); index++ )
    {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*>(searchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();
        RPointerArray<TDesC> tempMatchSeq;
        TBool isAdded = EFalse;

        // Parse thru each data and filter the results
        for ( TInt dataIndex = 0; dataIndex < psData->DataElementCount(); dataIndex++ )
        {
            // Filter off data fields not required in search
            TReal bitIndex;
            Math::Pow(bitIndex, 2, dataIndex);

            TUint8 filter = (TUint8)bitIndex & aFilteredDataMatch;
            if ( filter == 0x0 )
            {
                // Move to next data
                continue;
            }

            if ( poolElement->IsDataMatch(dataIndex) )
            {
                TLex lex(psData->Data(dataIndex)->Des());

                // First word
                TPtrC token = lex.NextToken();

                // Search thru multiple words
                while ( token.Length() != 0 )
                {
                    // Convert the data to char-and-key string
                    TBuf<KPsQueryMaxLen> dataWithKeys;
                    iKeyMap->GetMixedKeyStringForDataL( aPsQuery, token, dataWithKeys );

                    if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(dataWithKeys, queryAsDes, aPsQuery) )
                    {
                        psData->SetDataMatch(dataIndex);
                        isAdded = ETrue;

                        // Extract matched character sequence
                        TInt len = queryAsDes.Length();
                        HBufC* seq = HBufC::NewLC(len);
                        *seq = token.Mid(0, len);
                        seq->Des().UpperCase();

                        TIdentityRelation<TDesC> rule(CPcsAlgorithm1Utils::CompareExact);
                        if ( tempMatchSeq.Find(seq, rule) == KErrNotFound )
                        {
                            tempMatchSeq.Append(seq);
                            CleanupStack::Pop();
                        }
                        else
                        {
                            CleanupStack::PopAndDestroy();
                        }
                    }
                    // Next word
                    token.Set(lex.NextToken());
                }
            }
        }

        // Add the result
        if ( isAdded )
        {
            if ( isSearchInGroup )
            {
                if ( aContactsInGroup.Find(psData->Id()) != KErrNotFound )
                {
                    aAlgorithmFilterHelper->AddL(psData,tempMatchSeq);
                }
            }
            else
            {
                aAlgorithmFilterHelper->AddL(psData,tempMatchSeq);
            }
        }

        // Cleanup the match sequence array as
        // they are stored in pattern details structure
        tempMatchSeq.ResetAndDestroy();
    }

    __LATENCY_MARKEND ( _L("CPcsAlgorithm1Helper::FilterResultsSingleL") );

    PRINT ( _L("End CPcsAlgorithm1Helper::FilterResultsSingleL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::SearchMatchSeqL
// Funciton to search matching sequences in the input text
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::SearchMatchSeqL(CPsQuery& aPsQuery,
                                            TDesC& aData,
                                            RPointerArray<TDesC>& aMatchSet,
                                            RArray<TPsMatchLocation>& aMatchLocation )
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::SearchMatchSeqL") );

    HBufC* queryAsDes = HBufC::NewLC(aPsQuery.Count());
    TPtr queryAsDesPtr(queryAsDes->Des());
    iKeyMap->GetMixedKeyStringForQueryL( aPsQuery, queryAsDesPtr );

    // Convert the data into words
    TLex lex(aData);

    // First word
    TPtrC token = lex.NextToken();

    TInt beg = lex.Offset() - token.Length();  // start index of match sequence

    // Search thru multiple words
    while ( token.Length() != 0 )
    {
        HBufC* data = HBufC::NewLC(token.Length());
        TPtr dataPtr(data->Des());
        iKeyMap->GetMixedKeyStringForDataL( aPsQuery, token, dataPtr );

        if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(dataPtr, *queryAsDes, aPsQuery) )
        {
            TInt len = queryAsDes->Length();

            TPsMatchLocation tempLocation;

            // check for directionality of the text
            TBool found(EFalse);
            TBidiText::TDirectionality dir = TBidiText::TextDirectionality(token, &found);

            tempLocation.index = beg;
            tempLocation.length = len;
            tempLocation.direction = dir;

            // Add the match location to the data structure array
            aMatchLocation.Append(tempLocation);

            // Add the sequence to the match sequence
            HBufC* seq = HBufC::NewLC(len);
            *seq = token.Mid(0, len);
            seq->Des().UpperCase();

            TIdentityRelation<TDesC> rule(CPcsAlgorithm1Utils::CompareExact);
            if ( aMatchSet.Find(seq, rule) == KErrNotFound )
            {
                aMatchSet.Append(seq);
                CleanupStack::Pop();
            }
            else
            {
                CleanupStack::PopAndDestroy();
            }
        }

        // Next word
        token.Set(lex.NextToken());
        beg = lex.Offset() - token.Length();  // start index of next word
        CleanupStack::PopAndDestroy(); //data
    }

    CleanupStack::PopAndDestroy( queryAsDes );

    PRINT ( _L("End CPcsAlgorithm1Helper::SearchMatchSeqL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::SortSearchSeqsL()
//
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::SortSearchSeqsL(RPointerArray<TDesC>& aSearchSeqs)
{
    // Sort the search seqs
    TLinearOrder<TDesC> rule( CPcsAlgorithm1Utils::CompareCollate );
    aSearchSeqs.Sort(rule);
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::FilterDataFieldsL()
// Constructs a bit pattern using the required/supported data fields
// For example, 6, 4 and 27 are supported fields <-- 00000111
//              6 and 4 are required fields      <-- 00000011
// Bit pattern returned is 00000011.
// ----------------------------------------------------------------------------
TUint8 CPcsAlgorithm1Helper::FilterDataFieldsL(RArray<TInt>& aRequiredDataFields,
                                               RArray<TInt>& aSupportedDataFields)
{
    TUint8 filteredMatch = 0x0;

    for ( int i = 0; i < aSupportedDataFields.Count(); i++ )
    {
        for ( int j = 0; j < aRequiredDataFields.Count(); j++ )
        {
            if ( aSupportedDataFields[i] == aRequiredDataFields[j] )
            {
                TReal val;
                Math::Pow(val, 2, i);

                filteredMatch |= (TUint8)val;
            }
        }
    }

    return filteredMatch;
}

// End of file
