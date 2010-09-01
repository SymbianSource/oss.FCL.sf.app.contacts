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
void  CPcsAlgorithm1Helper::SearchSingleL( const CPsSettings& aSettings,
                                           CPsQuery& aPsQuery,
                                           TBool aIsSearchInGroup,
                                           RArray<TInt>& aContactsInGroup,
                                           RPointerArray<CPsData>& aSearchResults,
                                           RPointerArray<CPsPattern>& aSearchSeqs )
{
    PRINT ( _L("Enter CPcsAlgorithm1Helper::SearchSingleL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1Helper::SearchSingleL") );

    // Create filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm1FilterHelper* filterHelper = CPcsAlgorithm1FilterHelper::NewL(sortType);
    CleanupStack::PushL( filterHelper );
    
    // Search from cache based on first character
    const CPsQueryItem& firstCharItem = aPsQuery.GetItemAtL(0);
    TInt cachePoolId = iKeyMap->PoolIdForCharacter( firstCharItem.Character(), firstCharItem.Mode() );

    // Reset the result set array for new search
    iSearchResultsArr.ResetAndDestroy();

    // Get the data stores
    RPointerArray<TDesC> dataStores;
    CleanupResetAndDestroyPushL( dataStores );
    aSettings.SearchUrisL(dataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    CleanupClosePushL( requiredDataFields );
    aSettings.DisplayFieldsL(requiredDataFields);

    // Perform search for each required data store
    RPointerArray<CPcsPoolElement> elements;
    CleanupClosePushL( elements );

    for ( TInt dsIndex = 0; 
          dsIndex < dataStores.Count();
          dsIndex++ )
    {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        CleanupStack::PushL( temp );
        iSearchResultsArr.AppendL( temp );
        CleanupStack::Pop( temp );

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(dataStores[dsIndex]));
        if ( arrayIndex < 0 ) continue;
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(cachePoolId, elements);

        // Get the supported data fields for this data store
        RArray<TInt> supportedDataFields;
        CleanupClosePushL( supportedDataFields );
        cache->GetDataFields(supportedDataFields);

        // Get the filtered data fields for this data store
        TUint8 filteredDataMatch = CPcsAlgorithm1Utils::FilterDataFieldsL(requiredDataFields,
                                                                          supportedDataFields);

        // Perform filtering
        FilterResultsSingleL(filterHelper,
                             elements,
                             aPsQuery,
                             filteredDataMatch,
                             aIsSearchInGroup,
                             aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore
        if ( sortType == EAlphabetical )
        {
            filterHelper->GetResults(*(iSearchResultsArr[dsIndex]));
        }

        elements.Reset();
        CleanupStack::PopAndDestroy( &supportedDataFields ); // Close
    }

    CleanupStack::PopAndDestroy( &elements );           // Close
    CleanupStack::PopAndDestroy( &requiredDataFields ); // Close
    CleanupStack::PopAndDestroy( &dataStores );         // ResetAndDestroy

    // If alphabetical sorting, merge the result sets of all datastores
    if ( sortType == EAlphabetical )
    {
        // Merge the result sets of individual datastores alphabetically
        CPcsAlgorithm1Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        aSearchResults);
    }
    else
    {
        // Results are already sorted pattern based
        filterHelper->GetResults(aSearchResults);
    }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(aSearchSeqs);

    PRINT1 ( _L("CPcsAlgorithm1Helper::SearchSingleL: Number of search results = %d"), aSearchResults.Count() );

    // Cleanup
    for ( TInt i = 0; i < iSearchResultsArr.Count(); i++ )
    {
        iSearchResultsArr[i]->Reset();
    }
    iSearchResultsArr.ResetAndDestroy();

    CleanupStack::PopAndDestroy( filterHelper );

    __LATENCY_MARKEND ( _L("CPcsAlgorithm1Helper::SearchSingleL") );

    PRINT ( _L("End CPcsAlgorithm1Helper::SearchSingleL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::FilterResultsSingleL
// Subset search function
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::FilterResultsSingleL(CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
                                                 RPointerArray<CPcsPoolElement>& aSearchSet,
                                                 CPsQuery& aPsQuery,
                                                 TUint8 aFilteredDataMatch,
                                                 TBool aIsSearchInGroup,
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
    for ( TInt index = 0; index < aSearchSet.Count(); index++ )
    {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*>(aSearchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();
        
        // Skip this contact if performing group search and this contact doesn't
        // belong to the group
        if ( aIsSearchInGroup && aContactsInGroup.Find(psData->Id()) == KErrNotFound )
            {
            continue;
            }
        
        RPointerArray<TDesC> tempMatchSeq;
        CleanupResetAndDestroyPushL( tempMatchSeq );
        TBool isAdded = EFalse;

        // Parse thru each data and filter the results
        for ( TInt dataIndex = 0; dataIndex < psData->DataElementCount(); dataIndex++ )
        {
            // Filter off data fields not required in search
            TUint8 bitIndex = 1 << dataIndex;

            TUint8 filter = bitIndex & aFilteredDataMatch;
            if ( filter == 0x0 )
            {
                // Move to next data
                continue;
            }

            if ( poolElement->IsDataMatch(dataIndex) )
            {
                TLex lex( *psData->Data(dataIndex) );

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
                        TPtrC seq = token.Left(len);
                        CPcsAlgorithm1Utils::AppendMatchToSeqL( tempMatchSeq, seq );
                    }
                    // Next word
                    token.Set(lex.NextToken());
                }
            }
        }

        // Add the result
        if ( isAdded )
        {
            aAlgorithmFilterHelper->AddL(psData,tempMatchSeq);
        }

        // Cleanup the match sequence array as
        // they are stored in pattern details structure
        CleanupStack::PopAndDestroy( &tempMatchSeq ); // ResetAndDestroy
    }

    __LATENCY_MARKEND ( _L("CPcsAlgorithm1Helper::FilterResultsSingleL") );

    PRINT ( _L("End CPcsAlgorithm1Helper::FilterResultsSingleL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::SearchMatchSeqL
// Funciton to search matching sequences in the input text
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1Helper::SearchMatchSeqL(CPsQuery& aPsQuery,
                                            const TDesC& aData,
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
            TPtrC seq = token.Left(len);
            CPcsAlgorithm1Utils::AppendMatchToSeqL( aMatchSet, seq );
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

// End of file
