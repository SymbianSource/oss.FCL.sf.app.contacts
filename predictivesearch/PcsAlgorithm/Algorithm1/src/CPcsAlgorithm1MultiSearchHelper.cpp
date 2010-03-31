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
* Description:  Supports initial search feature. 
*
*/


// INCLUDES
#include "CPcsAlgorithm1MultiSearchHelper.h"
#include "CPcsAlgorithm1Utils.h"
#include "CPcsDefs.h"
#include <collate.h>
#include <biditext.h>


#ifdef _DEBUG
_LIT( KPanicMultiSearchHelper, "CPcsAlgorithm1MultiSearchHelper" );
static void Panic( TInt aReason )
    {
    User::Panic( KPanicMultiSearchHelper, aReason );
    }
#endif


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1MultiSearchHelper* CPcsAlgorithm1MultiSearchHelper::NewL(CPcsAlgorithm1* aAlgorithm)
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::NewL") );
    
	CPcsAlgorithm1MultiSearchHelper* self = new ( ELeave ) CPcsAlgorithm1MultiSearchHelper();
	CleanupStack::PushL( self );
	self->ConstructL(aAlgorithm);
	CleanupStack::Pop( self );

    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::NewL") );
    
	return self;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::CPcsAlgorithm1MultiSearchHelper
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1MultiSearchHelper::CPcsAlgorithm1MultiSearchHelper()
{		
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::CPcsAlgorithm1") );
    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::CPcsAlgorithm1") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::ConstructL(CPcsAlgorithm1* aAlgorithm)
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::ConstructL") );
    
    iAlgorithm = aAlgorithm;
    iKeyMap = iAlgorithm->GetKeyMap();
    
    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::ConstructL") );
} 	
	
// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::CPcsAlgorithm1MultiSearchHelper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm1MultiSearchHelper::~CPcsAlgorithm1MultiSearchHelper()
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::~CPcsAlgorithm1MultiSearchHelper") );        
    iMultiSearchResultsArr.ResetAndDestroy();          
    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::~CPcsAlgorithm1MultiSearchHelper") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::SearchMultiL
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
void  CPcsAlgorithm1MultiSearchHelper::SearchMultiL(const CPsSettings& aSettings,
	                                     RPointerArray<CPsQuery>& aPsQuery,
	                                     TBool aIsSearchInGroup,
	                                     RArray<TInt>& aContactsInGroup,
	                                     RPointerArray<CPsData>& aSearchResults,
	                                     RPointerArray<CPsPattern>& aSearchSeqs )
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );
   
    PRINTQUERYLIST ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL: "), aPsQuery );

    // Create CPcsAlgorithm1FilterHelper object to be used for filtering the results
	TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm1FilterHelper* filterHelper = CPcsAlgorithm1FilterHelper::NewL(sortType);
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
    for ( TInt dsIndex = 0; 
			dsIndex < dataStores.Count(); 
			dsIndex++ )
	{
	    RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
	    iMultiSearchResultsArr.Append(temp);
	    
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

	    // Filter the results now
	    FilterResultsMultiL(filterHelper,
	    			   elements, 
		               aPsQuery, 
		               filteredDataMatch,
		               aIsSearchInGroup,
		               aContactsInGroup);
	    
		// If alphabetical sorting, get the results for this datastore               
		if ( sortType == EAlphabetical )
		{
			filterHelper->GetResults(*(iMultiSearchResultsArr[dsIndex]));
		}
		
	    elements.Reset();
	    CleanupStack::PopAndDestroy( &supportedDataFields ); // Close
	}
    CleanupStack::PopAndDestroy( &requiredDataFields ); // Close
    CleanupStack::PopAndDestroy( &dataStores );         // ResetAndDestroy
	
	// If alphabetical sorting, merge the result sets of all datastores
  	if ( sortType == EAlphabetical )
  	{
  		// Form the complete searchResults array
    	CPcsAlgorithm1Utils::FormCompleteSearchResultsL(iMultiSearchResultsArr,
    											   	    aSearchResults);
  	}
  	else
  	{
  		// Results are already sorted patternbased
  		filterHelper->GetResults(aSearchResults);
  	}
  
  	// Get the sorted match sequence list
	filterHelper->GetPatternsL(aSearchSeqs);
	
	PRINT1 ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL: Number of search results = %d"), aSearchResults.Count() );
             
	// Cleanup             
    for(TInt i = 0; i < iMultiSearchResultsArr.Count(); i++)
    {
    	iMultiSearchResultsArr[i]->Reset();
    	delete iMultiSearchResultsArr[i];
        iMultiSearchResultsArr[i] = NULL;
    }
    
    iMultiSearchResultsArr.Reset();
    CleanupStack::PopAndDestroy( &elements ); // Close
    CleanupStack::PopAndDestroy( filterHelper );
   
    __LATENCY_MARKEND ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );

    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL
// Function adds matches, and locations based on multi query, and data
// Duplicate locations are allowed (as they are removed later anyway)
// Post condition locations in index order
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL(RPointerArray<CPsQuery>& aPsQuery,
                                                            const TDesC& aData,
                                                            RPointerArray<TDesC>& aMatchSeq,
                                                            RArray<TPsMatchLocation>& aMatchLocation )
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL") );
    RPointerArray<HBufC> descriptorsQueryList;
    CleanupResetAndDestroyPushL( descriptorsQueryList );
    ConvertQueryToListL(aPsQuery, descriptorsQueryList);
    TLex lex(aData);
    while ( !lex.Eos() ) // Search thru all words
    {
        TPtrC currentWord = lex.NextToken(); // next word

        TPsMatchLocation newLocation = { lex.Offset() - currentWord.Length(), //start index
                                         0, TBidiText::TextDirectionality(currentWord) };

        for ( TInt queryIndex = 0; queryIndex < aPsQuery.Count(); ++queryIndex )
        {
            HBufC* currentQuery = descriptorsQueryList[queryIndex];
            TBuf<KPsQueryMaxLen> convertedWord;

            // Convert the data to required form (mode specific)
            iKeyMap->GetMixedKeyStringForDataL(*aPsQuery[queryIndex], currentWord, convertedWord);

            if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(convertedWord, *currentQuery, *aPsQuery[queryIndex]) )
            {
                newLocation.length = currentQuery->Length();
                aMatchLocation.AppendL( newLocation );
                TPtrC matchPart = currentWord.Left(newLocation.length);
                CPcsAlgorithm1Utils::AppendMatchToSeqL( aMatchSeq, matchPart );
            }
        }
    }
    CleanupStack::PopAndDestroy( &descriptorsQueryList ); // ResetAndDestroy
    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::LookupMatchL
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::LookupMatchL( CPsQuery& aSearchQuery,
        const TDesC& aData,
        TDes& aMatchedData )
{
    RPointerArray<CPsQuery> queryList = MultiQueryL(aSearchQuery);
    TBuf<KPsQueryMaxLen> queryAsString = aSearchQuery.QueryAsStringLC();
    TBuf<KPsQueryMaxLen> convertedQuery;
    iKeyMap->GetMixedKeyStringForDataL( aSearchQuery, queryAsString, convertedQuery );
    
    RArray<TInt> dataWordIndexes;
    RArray<TInt> dataWordLengths;
    TLex lex( aData );
    while ( !lex.Eos() )
    {
        TPtrC currentWord = lex.NextToken();
        PRINT2( _L("idx len: %d %d"), lex.Offset() - currentWord.Length(), currentWord.Length() );
        dataWordIndexes.AppendL( lex.Offset() - currentWord.Length() );
        dataWordLengths.AppendL( currentWord.Length() );
    }

    RArray<TInt> queryIndexes;
    RArray<TPtrC> convertedQueriesAsDes;
    lex.Assign( queryAsString );
    while ( !lex.Eos() )
    {
        TPtrC currentWord = lex.NextToken();
        convertedQueriesAsDes.AppendL( 
                convertedQuery.Mid( lex.Offset() - currentWord.Length(), currentWord.Length()) );
        queryIndexes.AppendL( lex.Offset() - currentWord.Length() );
    }
    
    RPointerArray< RArray<TBool> > possibleMatches;
    for ( TInt i(0); i < queryList.Count(); ++i )
    {
        RArray<TBool>* matchesForCurrentQuery = new (ELeave) RArray<TBool>;
        possibleMatches.AppendL( matchesForCurrentQuery );
        CPsQuery* currentQuery = queryList[i];
        for ( TInt j(0); j < dataWordIndexes.Count(); j++ )
        {
            TPtrC currentDataWord = aData.Mid( dataWordIndexes[j], dataWordLengths[j] );
            RBuf convertedDataWord;
            convertedDataWord.CreateL( currentDataWord.Length() );
            CleanupClosePushL( convertedDataWord );

            iKeyMap->GetMixedKeyStringForDataL( *currentQuery, currentDataWord.Left(currentQuery->Count()), convertedDataWord );

            if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(convertedQueriesAsDes[i], convertedDataWord, *currentQuery) )
            {
                matchesForCurrentQuery->AppendL( ETrue );
            }
            else
            {
                matchesForCurrentQuery->AppendL( EFalse );
            }
            PRINT3( _L("CPcsAlgorithm1MultiSearchHelper::LookupMatchL: possibleMatches[%d][%d]=%d"),i,j, (*(possibleMatches[i]))[j] )
            CleanupStack::PopAndDestroy( &convertedDataWord );
        }
    }
    
    const TInt KUnapplied(-1);
    RArray<TInt> appliedMatches;
    appliedMatches.ReserveL( queryList.Count() );
    for ( TInt i(0); i < queryList.Count(); ++i )
    {
        appliedMatches.AppendL( KUnapplied );
    }
    
    //backtrack algorithm starts here to find fully applied match
    TInt currentQueryIndex(0);
    while ( currentQueryIndex < queryList.Count() && currentQueryIndex >= 0 )
    {
        TInt currentDataIndex = appliedMatches[ currentQueryIndex ] + 1;
        appliedMatches[ currentQueryIndex ] = KUnapplied;
        RArray<TBool>& matchesForCurrentQuery = *(possibleMatches[currentQueryIndex]);
        TBool doBacktrack( ETrue );
        while ( currentDataIndex < dataWordIndexes.Count() )
        {
            PRINT2(_L("CPcsAlgorithm1MultiSearchHelper::LookupMatchL: matchesForCurrentQuery[%d] = %d"), 
                    currentDataIndex, matchesForCurrentQuery[ currentDataIndex ] );
            
            if ( matchesForCurrentQuery[ currentDataIndex ] 
                     && (appliedMatches.Find( currentDataIndex ) == KErrNotFound) )
            {
                appliedMatches[ currentQueryIndex ] = currentDataIndex;
                doBacktrack = EFalse;
                break;
            }
            ++currentDataIndex;
        }
        if ( doBacktrack )
        {
            --currentQueryIndex;
        }
        else
        {
            ++currentQueryIndex;
        }
    }
    
    if ( currentQueryIndex >= 0 ) //found
    {
        aMatchedData = queryAsString;
        for ( TInt i(0); i < appliedMatches.Count(); ++i )
        {
            TInt matchedDataIndex = appliedMatches[i];
            TPtr resultFragment = aMatchedData.MidTPtr( 
                queryIndexes[ i ],
                convertedQueriesAsDes[i].Length() );
            resultFragment = aData.Mid(
                dataWordIndexes[ matchedDataIndex ],
                convertedQueriesAsDes[i].Length() );
        }
    }
    else
    {
        aMatchedData.Zero();
    }
    
    for ( TInt i(0); i < possibleMatches.Count(); ++i )
    {
        RArray<TBool>* pointerToDelete = possibleMatches[i];
        pointerToDelete->Close();
        delete pointerToDelete;
    }
    possibleMatches.Close();
    dataWordIndexes.Close();
    dataWordLengths.Close();

    queryIndexes.Close();
    convertedQueriesAsDes.Close();
    queryList.Close();
    appliedMatches.Close();
    
    CleanupStack::PopAndDestroy();  //result of queryAsStringLC
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::ConvertQueryToList
// Converts the multiple search queries to a list
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1MultiSearchHelper::ConvertQueryToListL(RPointerArray<CPsQuery>& aSearchQuery,
                                                           RPointerArray<HBufC>& aQueryList)
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::ConvertQueryToListL") );

    for ( TInt queryIndex = 0; queryIndex < aSearchQuery.Count(); queryIndex++ )
    {
        TBuf<KPsQueryMaxLen> dataWithKeys;
        iKeyMap->GetMixedKeyStringForQueryL( *aSearchQuery[queryIndex], dataWithKeys );

        HBufC* dWKToAppend = dataWithKeys.AllocLC();
        aQueryList.AppendL( dWKToAppend );
        CleanupStack::Pop( dWKToAppend ); // ownership transfered
    }

    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::ConvertQueryToListL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL
// Subset search function. Refer the above function for more description.
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL(
        CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
        RPointerArray<CPcsPoolElement>& aSearchSet,
        RPointerArray<CPsQuery>& aSearchQuery,
        TUint8 aFilteredDataMatch,
        TBool aIsSearchInGroup,
        RArray<TInt>& aContactsInGroup)
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

    // Convert the individual queries to string form
    RPointerArray<HBufC> queryList;
    CleanupResetAndDestroyPushL( queryList );
    RPointerArray<HBufC> tempqueryList;
    CleanupClosePushL( tempqueryList );
    ConvertQueryToListL(aSearchQuery, queryList);
    
    // Remember a temporary copy of query list
    // since we sort the queries
	for( TInt i = 0; i < queryList.Count(); i++)
	{
		tempqueryList.Append(queryList[i]);
	}
         
	// Sort the query items before we search them
	TLinearOrder<HBufC> rule( CPcsAlgorithm1Utils::CompareByLength );
	queryList.Sort(rule);
	
    // To hold the match results
    RPointerArray<TDesC> tmpMatchSet;
    CleanupResetAndDestroyPushL( tmpMatchSet );
       
    // Parse thru each search set elements and filter the results    
	for ( TInt index = 0; index < aSearchSet.Count(); index++ )
	{
	    CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*>(aSearchSet[index]);
		CPsData* psData = poolElement->GetPsData();
		psData->ClearDataMatches();
		
		TBool isMatch = ETrue;		
		TInt wordMatches = 0;
		
		// Reset iWordMatches to zero 
		ClearWordMatches();
		
		// Check for each query atleast one data element matches
		// Loop from the last query so that longest match is seen first
		for ( TInt queryIndex = queryList.Count() - 1; queryIndex >= 0; queryIndex-- )
		{
		    TBool queryMatch = EFalse;
		    HBufC* tmpQuery = queryList[queryIndex];
		    // Get the original query mode corresponding to this query
		    TInt modeIndex = tempqueryList.Find(tmpQuery);
		    
		    TInt dataCount = psData->DataElementCount();
		    __ASSERT_DEBUG( dataCount < MAX_DATA_FIELDS, Panic(KErrOverflow) );
		    
		    for ( TInt dataIndex = 0; dataIndex < dataCount; dataIndex++ )
		    {
		        // Filter off data fields not required in search
                TUint8 bitIndex = 1 << dataIndex;

                TUint8 filter = bitIndex & aFilteredDataMatch;
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
			    while ( tmpData.Length() != 0 ) 
			    {
			        wordIndex++;
			     	
			     	TBuf<KPsQueryMaxLen> data;

	                // Convert the data to required form (mode specific)
	                iKeyMap->GetMixedKeyStringForDataL(*aSearchQuery[modeIndex], tmpData, data);

				    // Compare the data against query
                    if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(data, *tmpQuery, *aSearchQuery[modeIndex]) )
				    {
				        psData->SetDataMatch(dataIndex);

				        // Perform two checks.
				        // 1. Ensure that the word is not matched against any previous query
				        // 2. If it is the first match to the query
				        TBool isWordMatch = IsWordMatch(dataIndex, wordIndex);

				        // Check if the current word is not matched to any query
		                if( !isWordMatch )
		                {
		                	// Check if no word is matched for this query till now
							if ( !queryMatch )
							{
								wordMatches++;
	                 			queryMatch = ETrue;
	                 			SetWordMap(dataIndex, wordIndex);
							}

					        // Extract matched character sequence and fill in temp array
							TInt len = tmpQuery->Length();
							TPtrC seq = tmpData.Left(len);
							CPcsAlgorithm1Utils::AppendMatchToSeqL( tmpMatchSet, seq );
						}
				    }
				     
				    // Next word
				    tmpData.Set(lex.NextToken());
			    }
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
		if ( isMatch && wordMatches >= queryList.Count() )
		{
			if ( aIsSearchInGroup )
			{
				if ( aContactsInGroup.Find(psData->Id()) != KErrNotFound )
				{
					aAlgorithmFilterHelper->AddL(psData,tmpMatchSet);
				}
			}
			else 
			{
				aAlgorithmFilterHelper->AddL(psData,tmpMatchSet);
			}
	    } 
	    
	    // Cleanup the match sequence array as 
		// they are stored in pattern details structure
		tmpMatchSet.ResetAndDestroy();
	}
	
	// Free the query list
	CleanupStack::PopAndDestroy( &tmpMatchSet );   // ResetAndDestroy
    CleanupStack::PopAndDestroy( &tempqueryList ); // Close
	CleanupStack::PopAndDestroy( &queryList );     // ResetAndDestroy

	__LATENCY_MARKEND ( _L("CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

	PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::SetWordMap()
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::SetWordMap(TInt aIndex, TInt aPosition)
{
    TUint8 val = 1 << aPosition;
    iWordMatches[aIndex] |= val;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::IsWordMatch()
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1MultiSearchHelper::IsWordMatch(TInt aDataIndex, TInt aWordIndex)
{
    TUint8 val = 1 << aWordIndex;
    return (iWordMatches[aDataIndex] & val);
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::ClearWordMatches
// Function to reset the iWordMatches
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::ClearWordMatches()
{
	for( TInt i = 0; i < MAX_DATA_FIELDS; i++)
	{
        iWordMatches[i] = 0;
	}
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::CountMultiQueryWordsL

// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1MultiSearchHelper::CountMultiQueryWordsL(CPsQuery& aQuery)
{    
    TInt wordCount = 0;
    TBool newWord = ETrue;

    for (TInt i = 0; i < aQuery.Count(); i++ )
    {
        if ( aQuery.GetItemAtL(i).Character().IsSpace() )
        {
            newWord = ETrue;
        }
        else
        {
            if ( newWord )
            {
                wordCount++;
                newWord = EFalse;
            }
        }
    }

    PRINT1 ( _L("CPcsAlgorithm1MultiSearchHelper::CountMultiQueryWords: Number of words in search query: %d"), wordCount );

    return wordCount;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::MultiQuery
// Checks if the query object has multiple queries embedded.
// Seperator used is a space.
// Scans through each query character and creates a new query object on a space.
// Consequtive spaces are skipped.
// Returns an array of query objects.
// ----------------------------------------------------------------------------
RPointerArray<CPsQuery> CPcsAlgorithm1MultiSearchHelper::MultiQueryL(CPsQuery& aQuery)
{
    RPointerArray<CPsQuery> query;
    
	const TInt textLength = aQuery.Count();
	
    for (TInt beg = 0; beg < textLength; beg++)
    {
        // Skip separators before next word
        if ( !aQuery.GetItemAtL(beg).Character().IsSpace() )
        {
            // Scan the end of the word
            TInt end = beg+1;
            while (end < textLength &&
                   !aQuery.GetItemAtL(end).Character().IsSpace())
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
