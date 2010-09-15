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
* Description: Supports initial search feature. 
*
*/

// INCLUDES
#include "FindUtilChineseECE.h"
#include "CPcsAlgorithm2MultiSearchHelper.h"
#include "CPcsAlgorithm2Utils.h"
#include "CPcsDefs.h"
#include <collate.h>
#include <biditext.h>


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2MultiSearchHelper* CPcsAlgorithm2MultiSearchHelper::NewL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::NewL") );

    CPcsAlgorithm2MultiSearchHelper* self = new (ELeave) CPcsAlgorithm2MultiSearchHelper();
    CleanupStack::PushL(self);
    self->ConstructL(aAlgorithm);
    CleanupStack::Pop(self);

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::CPcsAlgorithm2MultiSearchHelper
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2MultiSearchHelper::CPcsAlgorithm2MultiSearchHelper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::CPcsAlgorithm2") );
    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::CPcsAlgorithm2") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ConstructL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::ConstructL") );

    iAlgorithm = aAlgorithm;
    iKeyMap = iAlgorithm->GetKeyMap();
    iMaxCount = 0;
    
    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::~CPcsAlgorithm2MultiSearchHelper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm2MultiSearchHelper::~CPcsAlgorithm2MultiSearchHelper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::~CPcsAlgorithm2MultiSearchHelper") );
    iMultiSearchResultsArr.ResetAndDestroy();
    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::~CPcsAlgorithm2MultiSearchHelper") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::SearchMultiL
// Initial search feature.
// Flow of steps in initial search is explained below.
// (1) Extract the pool elements corresponding to one of the queries.
// (2) Always the first query is used here.
// (3) Get pool elements from all caches corresponding to the data stores.
// (4) Convert each query to mode specific form.
// (5) Parse each data element.
// (6) Check for every search query atleast one data element matches.
// (7) If NOT ignore the result.
// (8) If so perform an additional check that number of data matches is
//     atleast equal to number of search queries. This will ensure that same
//     data element has not matched for multiple queries.
// (9) Now include the element in the result.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::SearchMultiL(const CPsSettings& aSettings, 
                                                   RPointerArray<CPsQuery>& aPsQuery,
                                                   TBool aIsSearchInGroup, 
                                                   const RArray<TInt>& aContactsInGroup, 
                                                   RPointerArray<CPsData>& aSearchResults,
                                                   RPointerArray<CPsPattern>& aSearchSeqs)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );

    PRINTQUERYLIST ( _L("CPcsAlgorithm2MultiSearchHelper::SearchMultiL: "), aPsQuery );

    iMaxCount = aSettings.MaxResults();
    // Create CPcsAlgorithm2FilterHelper object to be used for filtering the results
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper = CPcsAlgorithm2FilterHelper::NewL(sortType);
    CleanupStack::PushL( filterHelper );
    RPointerArray<CPcsPoolElement> elements;
    CleanupClosePushL( elements );

    iMultiSearchResultsArr.ResetAndDestroy();

    // Get the data stores  
    RPointerArray<TDesC> dataStores;
    CleanupResetAndDestroyPushL( dataStores );
    aSettings.SearchUrisL(dataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    CleanupClosePushL( requiredDataFields );
    aSettings.DisplayFieldsL(requiredDataFields);

    // Search from cache based on first character of 1st item in query list
    const CPsQueryItem& firstCharItem = aPsQuery[0]->GetItemAtL(0);
    TInt cachePoolId = iKeyMap->PoolIdForCharacter( firstCharItem.Character(), firstCharItem.Mode() );

    // Get the elements from all the databases
    const TInt dataStoresCount = dataStores.Count(); 
    for (TInt dsIndex = 0; dsIndex < dataStoresCount; dsIndex++)
        {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iMultiSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(dataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(cachePoolId, elements);

        // Get the supported data fields for this data store
        RArray<TInt> supportedDataFields;
        CleanupClosePushL( supportedDataFields );
        cache->GetDataFields(supportedDataFields);

        // Get the filtered data fields for this data store
        TUint8 filteredDataMatch = CPcsAlgorithm2Utils::FilterDataFieldsL(
                requiredDataFields, supportedDataFields);

        // Filter the results now
        FilterResultsMultiL(filterHelper, 
                            elements, 
                            aPsQuery, 
                            filteredDataMatch, 
                            aIsSearchInGroup, 
                            aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore
        if (sortType == EAlphabetical)
            {
            filterHelper->GetResults(*(iMultiSearchResultsArr[dsIndex]));
            }

        elements.Reset();
        CleanupStack::PopAndDestroy( &supportedDataFields ); // Close
        }
    CleanupStack::PopAndDestroy( &requiredDataFields ); // Close
    CleanupStack::PopAndDestroy( &dataStores );         // ResetAndDestroy

    // If alphabetical sorting, merge the result sets of all datastores
    if (sortType == EAlphabetical)
        {
        // Form the complete searchResults array
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iMultiSearchResultsArr, 
                                                        aSearchResults);
        }
    else
        {
        // Results are already sorted patternbased
        filterHelper->GetResults(aSearchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(aSearchSeqs);

    PRINT1 ( _L("Number of search results = %d"), aSearchResults.Count() );

    // Cleanup
    for (TInt i = 0; i < iMultiSearchResultsArr.Count(); i++)
        {
        iMultiSearchResultsArr[i]->Reset();
        delete iMultiSearchResultsArr[i];
        iMultiSearchResultsArr[i] = NULL;
        }

    iMultiSearchResultsArr.Reset();
    CleanupStack::PopAndDestroy( &elements ); // Close
    CleanupStack::PopAndDestroy( filterHelper );

    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL
// Function adds matches, and locations based on multi query, and data
// Duplicate locations are allowed (as they are removed later anyway)
// Post condition locations in index order
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL( RPointerArray<CPsQuery>& aPsQuery,
                                                            const TDesC& aData,
                                                            RPointerArray<TDesC>& aMatchSeq,
                                                            RArray<TPsMatchLocation>& aMatchLocation )
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL") );

    TLex lex(aData);
    while ( !lex.Eos() ) // Search thru all words
        {
        TPtrC currentWord = lex.NextToken(); // next word

        const TInt psQueryCount = aPsQuery.Count(); 
        for ( TInt queryIndex = 0; queryIndex < psQueryCount; ++queryIndex )
            {
            CPsQuery* currentQuery = aPsQuery[queryIndex];

            RArray<TInt> matchPos;
            CleanupClosePushL( matchPos );
            RArray<TInt> matchLen;
            CleanupClosePushL( matchLen );
            
            if ( iAlgorithm->FindUtilECE()->MatchRefineL( currentWord, *currentQuery, matchPos, matchLen, ETrue ) )
                {
                // Some matches found. Add all of them to result array.
                ASSERT( matchPos.Count() == matchLen.Count() );
                
                TInt wordStartPos = lex.Offset() - currentWord.Length();
                const TInt matchPosCount =  matchPos.Count(); 
                for ( TInt i = 0 ; i < matchPosCount; ++i )
                    {
                    TPsMatchLocation newLocation = { wordStartPos + matchPos[i], matchLen[i], 
                            TBidiText::TextDirectionality(currentWord) };
                    aMatchLocation.AppendL( newLocation );

                    TPtrC matchPart = currentWord.Mid( matchPos[i], matchLen[i] );
                    CPcsAlgorithm2Utils::AppendMatchToSeqL( aMatchSeq, matchPart );
                    }
                }
                
            CleanupStack::PopAndDestroy( &matchLen );
            CleanupStack::PopAndDestroy( &matchPos );
            }
        }

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::LookupMatchL
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::LookupMatchL( CPsQuery& aSearchQuery,
    const TDesC& aData, TDes& aMatchedData )
    {
    _LIT( KSpace, " " );
    aMatchedData.Zero();
    RPointerArray<CPsQuery> queryList = MultiQueryL( aSearchQuery );
    CleanupResetAndDestroyPushL( queryList );
    // Convert the individual queries to string form
    RPointerArray<CPsQuery> mySearchQuery;
    CleanupResetAndDestroyPushL( mySearchQuery );

    // Remember a temporary copy of query list
    // Copy the content of searchQuery
    const TInt searchQueryCount = queryList.Count();
    for (TInt i = 0; i < searchQueryCount; i++ )
        {
        CPsQuery* tempQuery = CPsQuery::NewL();
        CleanupStack::PushL( tempQuery );
        iAlgorithm->FindUtilECE()->GetPartOfQueryL( *(queryList[i]), 0,
            queryList[i]->Count() - 1, *tempQuery );
        mySearchQuery.AppendL( tempQuery );
        CleanupStack::Pop( tempQuery ); // ownership transferred
        }

    // Sort the query items according to the length of each query 
    TLinearOrder<CPsQuery> rule( CPcsAlgorithm2Utils::CompareLength );
    mySearchQuery.Sort( rule );

    // To hold the match results
    RPointerArray<TDesC> tmpMatchSet;
    CleanupResetAndDestroyPushL( tmpMatchSet );

    TBool isMatch = ETrue;
    TInt wordMatches = 0;

    // Reset iWordMatches to zero 
    ClearWordMatches();

    // Check for each query atleast one data element matches
    // Loop from the last query so that longest match is seen first
    for (TInt queryIndex = mySearchQuery.Count() - 1; queryIndex >= 0; queryIndex-- )
        {
        TBool queryMatch = EFalse;
        CPsQuery* tmpPsQuery = mySearchQuery[queryIndex];

        TInt wordIndex = -1;
        TLex lex( aData );

        // First word
        TPtrC tmpData = lex.NextToken();

        // Search thru multiple words
        while (tmpData.Length() != 0 )
            {
            wordIndex++;

            // Compare the data against query
            TBool matched = iAlgorithm->FindUtilECE()->MatchRefineL( tmpData,
                *tmpPsQuery );

            if ( matched )
                {
                // Perform two checks.
                // 1. Ensure that the word is not matched against any previous query
                // 2. If it is the first match to the query
                TBool isWordMatch = IsWordMatch( 0, wordIndex );

                // Check if the current word is not matched to any query
                // For example, there is a contact named "abc a" and query is key2
                // The key2 could match the first and second 'a'. So it is required to 
                // check if the current word has aready been matched before.

                if ( !isWordMatch )
                    {
                    // Check if no word is matched for this query till now
                    if ( !queryMatch )
                        {
                        wordMatches++;
                        queryMatch = ETrue;
                        SetWordMap( 0, wordIndex );
                        // Extract matched character sequence and fill in temp array
                        TInt len = tmpPsQuery->Count();
                        if ( iAlgorithm->FindUtilECE()->IsChineseWordIncluded(
                            tmpData ) )
                            {
                            // A Chinese word could be matched by serveral keys
                            // It is hard to know the matched query length. So set it to 1
                            // as a trick result
                            len = 1;
                            }

                        TPtrC seq = tmpData.Left( len );
                        CPcsAlgorithm2Utils::AppendMatchToSeqL( tmpMatchSet,
                            seq );
                        }
                    }
                }

            // Next word
            tmpData.Set( lex.NextToken() );
            }

        // No data element matches the query. Ignore this result.
        if ( queryMatch == EFalse )
            {
            isMatch = EFalse;
            break;
            }
        }

    // If match add the element to the result set
    //  And before adding to the result set, check if there is atleast one match per query
    if ( isMatch && wordMatches >= mySearchQuery.Count() )
        {
        const TInt matchCount = tmpMatchSet.Count();
        for (TInt i = 0; i < matchCount; i++ )
            {
            aMatchedData.Append( *tmpMatchSet[i] );
            aMatchedData.Append( KSpace );
            }
        aMatchedData.TrimRight();
        }

    CleanupStack::PopAndDestroy( &tmpMatchSet ); // ResetAndDestroy
    CleanupStack::PopAndDestroy( &mySearchQuery ); // ResetAndDestroy
    CleanupStack::PopAndDestroy( &queryList ); // ResetAndDestroy
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL
// Subset search function. Refer the above function for more description.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper, 
                                                          RPointerArray<CPcsPoolElement>& aSearchSet,
                                                          RPointerArray<CPsQuery>& aSearchQuery, 
                                                          TUint8 aFilteredDataMatch,
                                                          TBool aIsSearchInGroup, 
                                                          const RArray<TInt>& aContactsInGroup)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );

    TInt maxcount = 0;
    
    // Convert the individual queries to string form
    RPointerArray<CPsQuery> mySearchQuery;
    CleanupResetAndDestroyPushL( mySearchQuery );

    // Remember a temporary copy of query list
    // Copy the content of searchQuery
    const TInt searchQueryCount = aSearchQuery.Count(); 
    for (TInt i=0; i < searchQueryCount; i++)
        {
        CPsQuery* tempQuery = CPsQuery::NewL();
        CleanupStack::PushL( tempQuery );
        iAlgorithm->FindUtilECE()->GetPartOfQueryL(
                *(aSearchQuery[i]), 0, aSearchQuery[i]->Count()-1, *tempQuery );
        mySearchQuery.AppendL(tempQuery);
        CleanupStack::Pop(tempQuery); // ownership transferred
        }

    // Sort the query items according to the length of each query 
    TLinearOrder<CPsQuery> rule(CPcsAlgorithm2Utils::CompareLength);
    mySearchQuery.Sort(rule);

    // To hold the match results
    RPointerArray<TDesC> tmpMatchSet;
    CleanupResetAndDestroyPushL( tmpMatchSet );

    // Parse thru each search set elements and filter the results
    const TInt searchSetCount = aSearchSet.Count(); 
    for (TInt index = 0; index < searchSetCount; index++)
        {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*> (aSearchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();

        // Skip the contact if we are doing a group search and contact doesn't belong to the group
        if ( aIsSearchInGroup && 
             aContactsInGroup.Find( psData->Id() ) == KErrNotFound )
            {
            continue;
            }
        
        TBool isMatch = ETrue;
        TInt wordMatches = 0;

        // Reset iWordMatches to zero 
        ClearWordMatches();

        // Check for each query atleast one data element matches
        // Loop from the last query so that longest match is seen first
        for (TInt queryIndex = mySearchQuery.Count() - 1; queryIndex >= 0; queryIndex--)
            {
            TBool queryMatch = EFalse;
            CPsQuery* tmpPsQuery = mySearchQuery[queryIndex];

            const TInt dataElementCount = psData->DataElementCount();
            for (TInt dataIndex = 0; dataIndex < dataElementCount; dataIndex++)
                {
                // Filter off data fields not required in search
                TUint8 bitIndex = 1 << dataIndex;
                TUint8 filter = bitIndex & aFilteredDataMatch;
                
                // Omit the data fields which is not required in search
                // or not matched with the pool element
                if ( filter == 0x0 ) 
                    {
                    // Move to next data
                    continue;
                    }

                TInt wordIndex = -1;

                TLex lex(psData->Data(dataIndex)->Des());

                // First word
                TPtrC tmpData = lex.NextToken();

                // Search thru multiple words
                while (tmpData.Length() != 0)
                    {
                    wordIndex++;
                    
                    // Compare the data against query
                    TBool matched = iAlgorithm->FindUtilECE()->MatchRefineL(tmpData, *tmpPsQuery);
                    
                    if (matched)
                        {
                        psData->SetDataMatch(dataIndex);

                        // Perform two checks.
                        // 1. Ensure that the word is not matched against any previous query
                        // 2. If it is the first match to the query
                        TBool isWordMatch = IsWordMatch(dataIndex, wordIndex);

                        // Check if the current word is not matched to any query
                        if (!isWordMatch)
                            {
                            // Check if no word is matched for this query till now
                            if (!queryMatch)
                                {
                                wordMatches++;
                                queryMatch = ETrue;
                                SetWordMap(dataIndex, wordIndex);
                                }

                            // Extract matched character sequence and fill in temp array
                            TInt len = tmpPsQuery->Count();
                            if (iAlgorithm->FindUtilECE()->IsChineseWordIncluded(tmpData))
                                {
                                len = 1;
                                }

                            TPtrC seq = tmpData.Left(len);
                            CPcsAlgorithm2Utils::AppendMatchToSeqL( tmpMatchSet, seq );
                            
                            // TODO: Match seqs could be extracted from actual
                            //       match locations by using the other overload of
                            //       CFindUtilChineseECE::MatchRefineL().
                            //       Currently, match seq data is not used by clients.
                            }
                        }

                    // Next word
                    tmpData.Set(lex.NextToken());
                    }
                }

            // No data element matches the query. Ignore this result.
            if (queryMatch == EFalse)
                {
                isMatch = EFalse;
                break;
                }
            }
        

        // If match add the element to the result set
        //  And before adding to the result set, check if there is atleast one match per query
        if ( isMatch && wordMatches >= mySearchQuery.Count() )
            {
            aAlgorithmFilterHelper->AddL(psData, tmpMatchSet);
            maxcount++;
            }
        
        if ( iMaxCount != -1 && maxcount > iMaxCount )
            {
            break;
            }
        
        // Cleanup the match sequence array as 
        // they are stored in pattern details structure
        tmpMatchSet.ResetAndDestroy();
        }

    CleanupStack::PopAndDestroy( &tmpMatchSet );   // ResetAndDestroy
    CleanupStack::PopAndDestroy( &mySearchQuery ); // ResetAndDestroy

    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::SetWordMap
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::SetWordMap(TInt aIndex, TInt aPosition)
    {
    TUint8 val = 1 << aPosition;
    iWordMatches[aIndex] |= val;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::IsWordMatch
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2MultiSearchHelper::IsWordMatch(TInt aDataIndex, TInt aWordIndex)
    {
    TUint8 val = 1 << aWordIndex;
    return (iWordMatches[aDataIndex] & val);
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::BitsSet32
// Helper funtion to count the number of bits set
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2MultiSearchHelper::BitsSet32(TUint32 aData)
    {
    TInt count = 0;

    for (count = 0; aData; aData &= (aData - 1))
        {
        count++;
        }

    return count;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ClearWordMatches
// Function to reset the iWordMatches
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ClearWordMatches()
    {
    for (TInt i = 0; i < MAX_DATA_FIELDS; i++)
        iWordMatches[i] = 0;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::MultiQuery
// Checks if the query object has multiple queries embedded.
// Seperator used is a space.
// Scans through each query character and creates a new query object on a space.
// Consequtive spaces are skipped.
// Returns an array of query objects.
// ----------------------------------------------------------------------------
RPointerArray<CPsQuery> CPcsAlgorithm2MultiSearchHelper::MultiQueryL(CPsQuery& aQuery)
    {
    RPointerArray<CPsQuery> query;

    const TInt textLength = aQuery.Count();

    for (TInt beg = 0; beg < textLength; ++beg)
        {
        // Skip separators before next word	                
        if (!aQuery.GetItemAtL(beg).Character().IsSpace())
            {
            // Scan the end of the word
            TInt end = beg;
            while ( end < textLength && !aQuery.GetItemAtL(end).Character().IsSpace() )
                {
                end++;
                }

            // Create a new query object and append
            CPsQuery* newQuery = CPsQuery::NewL();
            for (TInt i = beg; i < end; i++)
                {
                CPsQueryItem* item = CPsQueryItem::NewL();
                item->SetCharacter(aQuery.GetItemAtL(i).Character());
                item->SetMode(aQuery.GetItemAtL(i).Mode());
                newQuery->AppendL(*item);
                }
            query.Append(newQuery);

            // Scan for next word
            beg = end;
            }
        }

    return query;
    }

// End of file


