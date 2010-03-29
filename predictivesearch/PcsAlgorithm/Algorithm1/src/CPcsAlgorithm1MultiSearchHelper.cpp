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
	                                     TBool isSearchInGroup,
	                                     RArray<TInt>& aContactsInGroup,
	                                     RPointerArray<CPsData>& searchResults,
	                                     RPointerArray<CPsPattern>& searchSeqs )
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL") );
   
    PRINTQUERYLIST ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL: "), aPsQuery );

    // Create CPcsAlgorithm1FilterHelper object to be used for filtering the results
	TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm1FilterHelper* filterHelper = CPcsAlgorithm1FilterHelper::NewL(sortType);
    RPointerArray<CPcsPoolElement> elements;
    
    iMultiSearchResultsArr.ResetAndDestroy();  
    
    // Get the data stores  
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);
          
    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    aSettings.DisplayFieldsL(requiredDataFields);

    // Search based on first character of 1st item in query list
    TInt numValue = iKeyMap->PoolIdForCharacter(aPsQuery[0]->GetItemAtL(0).Character());

    // Get the elements from all the databases
    for ( int dsIndex = 0; 
			dsIndex < aDataStores.Count(); 
			dsIndex++ )
	{
	    RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
	    iMultiSearchResultsArr.Append(temp);
	    
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

	    // Filter the results now
	    FilterResultsMultiL(filterHelper,
                            elements,
                            aPsQuery,
                            filteredDataMatch,
                            isSearchInGroup,
                            aContactsInGroup);	
	    
		// If alphabetical sorting, get the results for this datastore               
		if ( sortType == EAlphabetical )
		{
			filterHelper->GetResults(*(iMultiSearchResultsArr[dsIndex]));
		}
		
	    elements.Reset();
	    supportedDataFields.Reset();
	}
	aDataStores.ResetAndDestroy();
    requiredDataFields.Reset(); 
	
	// If alphabetical sorting, merge the result sets of all datastores
  	if ( sortType == EAlphabetical )
  	{
  		// Form the complete searchResults array
    	CPcsAlgorithm1Utils::FormCompleteSearchResultsL(iMultiSearchResultsArr,
    											   	    searchResults);
  	}
  	else
  	{
  		// Results are already sorted patternbased
  		filterHelper->GetResults(searchResults);
  	}
  
  	// Get the sorted match sequence list
	filterHelper->GetPatternsL(searchSeqs);
	
	PRINT1 ( _L("CPcsAlgorithm1MultiSearchHelper::SearchMultiL: Number of search results = %d"), searchResults.Count() ); 
             
	// Cleanup             
    for(TInt i = 0; i < iMultiSearchResultsArr.Count(); i++)
    {
    	iMultiSearchResultsArr[i]->Reset();
    	delete iMultiSearchResultsArr[i];
        iMultiSearchResultsArr[i] = NULL;
    }
    
    iMultiSearchResultsArr.Reset();
    delete filterHelper;
   
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
                AppendMatchToSeqL( aMatchSeq, currentWord.Left(newLocation.length) );
            }
        }
    }
    descriptorsQueryList.ResetAndDestroy();
    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::SearchMatchSeqMultiL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::AppendMatchToSeqL
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::AppendMatchToSeqL( 
        RPointerArray<TDesC>& aMatchSeq, const TDesC& aMatch  )
{
    HBufC* seq = aMatch.AllocLC();
    seq->Des().UpperCase();
    TIdentityRelation<TDesC> rule(CPcsAlgorithm1Utils::CompareExact);
    if ( aMatchSeq.Find(seq, rule) == KErrNotFound )
    {
        aMatchSeq.Append(seq);
        CleanupStack::Pop( seq );
    }
    else 
    {
        CleanupStack::PopAndDestroy( seq );
    }
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
        dataWordIndexes.AppendL( lex.Offset() -  currentWord.Length() );
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

    for ( int queryIndex = 0; queryIndex < aSearchQuery.Count(); queryIndex++ )
    {
        TBuf<KPsQueryMaxLen> dataWithKeys;
        iKeyMap->GetMixedKeyStringForQueryL( *aSearchQuery[queryIndex], dataWithKeys );

        HBufC* dWKToAppend = HBufC::NewL(KPsQueryMaxLen);
        TPtr tPtr(dWKToAppend->Des());
        tPtr.Copy(dataWithKeys);

        aQueryList.Append( dWKToAppend );
    }

    PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::ConvertQueryToListL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL
// Subset search function. Refer the above function for more description.
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL(
        CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
        RPointerArray<CPcsPoolElement>& searchSet,
        RPointerArray<CPsQuery>& searchQuery,
        TUint8 aFilteredDataMatch,
        TBool isSearchInGroup,
        RArray<TInt>& aContactsInGroup)
{
    PRINT ( _L("Enter CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

    // Convert the individual queries to string form
    RPointerArray<HBufC> queryList;    
    RPointerArray<HBufC> tempqueryList;   
    ConvertQueryToListL(searchQuery, queryList);
    
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
       
    // Parse thru each search set elements and filter the results    
	for ( int index = 0; index < searchSet.Count(); index++ )
	{
	    CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*>(searchSet[index]);
		CPsData* psData = poolElement->GetPsData();
		psData->ClearDataMatches();
		
		TBool isMatch = ETrue;		
		TUint8 wordMatches = 0;
		
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
	                iKeyMap->GetMixedKeyStringForDataL(*searchQuery[modeIndex], tmpData, data);

				    // Compare the data against query
                    if ( CPcsAlgorithm1Utils::MyCompareKeyAndString(data, *tmpQuery, *searchQuery[modeIndex]) )
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
							HBufC* seq = HBufC::NewL(len);
							*seq = tmpData.Mid(0, len);
							
							seq->Des().UpperCase();
							TIdentityRelation<TDesC> searchRule(CPcsAlgorithm1Utils::CompareExact);
							if ( tmpMatchSet.Find(seq, searchRule) == KErrNotFound )
							{
                                tmpMatchSet.Append(seq);
							}
							else
							{ 
                                delete seq;
								seq = NULL;
							}
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
		if (( isMatch ) && ( wordMatches >= queryList.Count()))
		{
			if ( isSearchInGroup )
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
	queryList.ResetAndDestroy();
	tempqueryList.Reset();

	__LATENCY_MARKEND ( _L("CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );

	PRINT ( _L("End CPcsAlgorithm1MultiSearchHelper::FilterResultsMultiL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::SetWordMap()
// ----------------------------------------------------------------------------
void CPcsAlgorithm1MultiSearchHelper::SetWordMap( TInt aIndex, TInt aPosition)
{
    TReal val;
	Math::Pow(val, 2, aPosition);

	iWordMatches[aIndex] |= (TUint8)val;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::IsWordMatch()
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1MultiSearchHelper::IsWordMatch(TInt aDataIndex, TInt aWordIndex)
{
    TReal val;
	Math::Pow(val, 2, aWordIndex);

	return(iWordMatches[aDataIndex] & (TUint8)val);
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
    TBool newWord = true;

    for (TInt i = 0; i < aQuery.Count(); i++ )
    {
        if ( aQuery.GetItemAtL(i).Character().IsSpace() )
        {
            newWord = true;
        }
        else
        {
            if ( newWord )
            {
                wordCount++;
                newWord = false;
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

// ----------------------------------------------------------------------------
// CPcsAlgorithm1MultiSearchHelper::FilterDataFieldsL()
// Constructs a bit pattern using the required/supported data fields
// For example, 6, 4 and 27 are supported fields <-- 00000111
//              6 and 4 are required fields      <-- 00000011
// Bit pattern returned is 00000011.
// ----------------------------------------------------------------------------
TUint8 CPcsAlgorithm1MultiSearchHelper::FilterDataFieldsL(RArray<TInt>& aRequiredDataFields,
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
