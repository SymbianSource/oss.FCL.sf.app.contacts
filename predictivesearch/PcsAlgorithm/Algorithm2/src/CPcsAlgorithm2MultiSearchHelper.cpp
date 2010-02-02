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

// Compare functions
TBool Compare2(const HBufC& aFirst, const HBufC& aSecond)
    {
    TPtrC t1(aFirst);
    TPtrC t2(aSecond);
    return (t1.Length() > t2.Length());
    }

TBool Compare3(const TDesC& aFirst, const TDesC& aSecond)
    {
    return aFirst == aSecond;
    }

TBool Compare4(const CPsQuery& aFirst, const CPsQuery& aSecond)
    {
    CPsQuery& first = const_cast<CPsQuery&> (aFirst);
    CPsQuery& second = const_cast<CPsQuery&> (aSecond);
    
    return (first.Count() > second.Count());
    }
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
    keyMap = iAlgorithm->GetKeyMap();

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::CPcsAlgorithm2MultiSearchHelper
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
                                                   TBool isSearchInGroup, 
                                                   RArray<TInt>& aContactsInGroup, 
                                                   RPointerArray<CPsData>& searchResults,
                                                   RPointerArray<CPsPattern>& searchSeqs,
                                                   TInt keyboardMode)
    {
    __LATENCY_MARK ( _L("CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );

    // Create CPcsAlgorithm2FilterHelper object to be used for filtering the results
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper = CPcsAlgorithm2FilterHelper::NewL(sortType);
    RPointerArray<CPcsPoolElement> elements;

    // Get the initial search elements using the first key of first query
    TBuf<50> numericKeyStr;
    TPtrC queryPtr = aPsQuery[0]->QueryAsStringLC();
    keyMap->GetNumericKeyString(queryPtr, numericKeyStr);
    CleanupStack::PopAndDestroy();

    iMultiSearchResultsArr.ResetAndDestroy();

    // Get the data stores  
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    aSettings.DisplayFieldsL(requiredDataFields);

    // Search based on first key str
    TInt numValue = keyMap->PoolIdForCharacter(numericKeyStr[0]);
    // Get the elements from all the databases
    for (int dsIndex = 0; dsIndex < aDataStores.Count(); dsIndex++)
        {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iMultiSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(aDataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(numValue, elements);

        // Get the supported data fields for this data store
        RArray<TInt> supportedDataFields;
        cache->GetDataFields(supportedDataFields);

        // Get the filtered data fields for this data store    		
        TUint8 filteredDataMatch = FilterDataFieldsL(requiredDataFields, supportedDataFields);

        // Filter the results now
        FilterResultsMultiL(filterHelper, elements, aPsQuery, filteredDataMatch, 
                            isSearchInGroup, aContactsInGroup, keyboardMode);

        // If alphabetical sorting, get the results for this datastore               
        if (sortType == EAlphabetical)
            {
            filterHelper->GetResults(*(iMultiSearchResultsArr[dsIndex]));
            }

        elements.Reset();
        supportedDataFields.Reset();
        }
    aDataStores.ResetAndDestroy();
    requiredDataFields.Reset();

    // If alphabetical sorting, merge the result sets of all datastores
    if (sortType == EAlphabetical)
        {
        // Form the complete searchResults array
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iMultiSearchResultsArr, searchResults);
        }
    else
        {
        // Results are already sorted patternbased
        filterHelper->GetResults(searchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(searchSeqs);

    PRINT1 ( _L("Number of search results = %d"), searchResults.Count() );

    // Cleanup             
    for (TInt i = 0; i < iMultiSearchResultsArr.Count(); i++)
        {
        iMultiSearchResultsArr[i]->Reset();
        delete iMultiSearchResultsArr[i];
        iMultiSearchResultsArr[i] = NULL;
        }

    iMultiSearchResultsArr.Reset();
    delete filterHelper;

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );
    __LATENCY_MARKEND ( _L("CPcsAlgorithm2MultiSearchHelper::SearchMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::SearchInputMultiL
// Function to search match sequences for multi query
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL(RPointerArray<CPsQuery>& aPsQuery, 
                                                           TDesC& aData, 
                                                           RPointerArray<TDesC>& aMatchSet,
                                                           RArray<TPsMatchLocation>& aMatchLocation)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL") );
    RPointerArray<HBufC> queryList;
    ConvertQueryToListL(aPsQuery, queryList);

    RPointerArray<HBufC> tempqueryList;
    // Remember a temporary copy of query list
    // since we sort the queries
    for (TInt i = 0; i < queryList.Count(); i++)
        {
        tempqueryList.Append(queryList[i]);
        }
    // To hold the match results
    RPointerArray<TDesC> tmpMatchSet;
    TBool isMatch = ETrue;
    TUint32 wordMatches = 0;

    // Sort the query items before we search them
    TLinearOrder<HBufC> rule(Compare2);
    queryList.Sort(rule);

    // Check for each query atleast one data element matches
    // Loop from the last query so that longest match is seen first
    for (TInt queryIndex = queryList.Count() - 1; queryIndex >= 0; queryIndex--)
        {
        TBool queryMatch = EFalse;
        HBufC* tmpQuery = queryList[queryIndex];
        // Get the original query mode corresponding to this query
        TInt modeIndex = tempqueryList.Find(tmpQuery);

        TLex lex(aData);

        // First word
        TPtrC tmpData = lex.NextToken();

        TInt beg = lex.Offset() - tmpData.Length(); // start index of match sequence

        TInt wordIndex = -1;
        // Search thru multiple words
        while ((tmpData.Length() != 0) && (!queryMatch))
            {
            wordIndex++;

            TPtr ptr = tmpQuery->Des();

            // Perform two checks.
            // 1. Ensure that the word is not matched against any previous query
            // 2. If it is the first match to the query
            TBool isWordMatch = EFalse;
            TReal val;
            Math::Pow(val, 2, wordIndex);
            isWordMatch = wordMatches & (TUint) val;

            if (!isWordMatch)
                {
                // Check if no word is matched till now for this query
                if (!queryMatch)
                    {
                    queryMatch = ETrue;
                    //set the word match bit
                    TReal val;
                    Math::Pow(val, 2, wordIndex);
                    wordMatches |= (TUint) val;
                    }

                TPsMatchLocation tempLocation;
                // check for directionality of the text
                TBool found(EFalse);
                TBidiText::TDirectionality dir = TBidiText::TextDirectionality(tmpData, &found);

                tempLocation.index = beg;
                tempLocation.length = 0;
                tempLocation.direction = dir;

                // Add the match location to the data structure array
                aMatchLocation.Append(tempLocation);
                }
            }
        // Next word
        tmpData.Set(lex.NextToken());
        beg = lex.Offset() - tmpData.Length(); // start index of next word  


        // No data element matches the query. Ignore this result.
        if (queryMatch == EFalse)
            {
            isMatch = EFalse;
            break;
            }
        }

    // Count the number of bits set
    TInt matchCount = 0;
    matchCount = BitsSet32(wordMatches);

    // If match add the element to the result set
    // Before adding to the result set, check if there is atleast one match per query
    // Number of bits set in word matches is atleast equal to total number of queries.
    if ((isMatch) && (matchCount >= queryList.Count()))
        {

        // Include the match sequences in the final results
        for (int i = 0; i < tmpMatchSet.Count(); i++)
            {
            TIdentityRelation<TDesC> rule(Compare3);
            CleanupStack::PushL(tmpMatchSet[i]);
            if (aMatchSet.Find(tmpMatchSet[i], rule) == KErrNotFound)
                {
                aMatchSet.Append(tmpMatchSet[i]);
                CleanupStack::Pop();
                }
            else
                {
                CleanupStack::PopAndDestroy();
                }
            }

        // Reset tmp match set
        tmpMatchSet.Reset();
        }
    else
        {
        tmpMatchSet.ResetAndDestroy();
        }

    // Free the query list
    queryList.ResetAndDestroy();
    tempqueryList.Reset();

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::SearchMatchSeqMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ConvertQueryToList
// Converts the multiple search queries to a list
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ConvertQueryToListL(RPointerArray<CPsQuery>& aSearchQuery, 
                                                          RPointerArray<HBufC>& aQueryList)
    {
    for (int queryIndex = 0; queryIndex < aSearchQuery.Count(); queryIndex++)
        {
        CPsQuery* query = aSearchQuery[queryIndex];

        HBufC* tmpSearchQuery = HBufC::NewL(KPsQueryMaxLen);
        TPtr ptr = tmpSearchQuery->Des();

        if (query->KeyboardModeL() == EItut) // ITU
            {
            keyMap->GetNumericKeyString(query->QueryAsStringLC(), ptr);
            CleanupStack::PopAndDestroy();
            }
        else if (query->KeyboardModeL() == EQwerty) // QWERTY
            {
            ptr = query->QueryAsStringLC();
            ptr.LowerCase();
            CleanupStack::PopAndDestroy();
            }
        else // UNDEFINED
            {
            ExtractQueryL(*query, ptr);
            ptr.LowerCase();
            }
        aQueryList.Append(tmpSearchQuery);
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ConvertdDataToKeyBoardModeL
// Converts the input data to the key board mode specified by the query
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ConvertdDataToKeyBoardModeL(CPsQuery* aQuery, 
                                                                  TPtrC aInputData, 
                                                                  TBuf<KPsQueryMaxLen>& aOutputData)
    {
    if (aQuery->KeyboardModeL() == EItut)
        {
        keyMap->GetNumericKeyString(aInputData, aOutputData);
        }
    else if (aQuery->KeyboardModeL() == EQwerty)
        {
        aOutputData = aInputData;
        aOutputData.LowerCase();
        }
    else
        {
        ExtractQueryL(aInputData, *aQuery, aOutputData);
        aOutputData.LowerCase();
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL
// Subset search function. Refer the above function for more description.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper, 
                                                          RPointerArray<CPcsPoolElement>& searchSet,
                                                          RPointerArray<CPsQuery>& searchQuery, 
                                                          TUint8 aFilteredDataMatch,
                                                          TBool isSearchInGroup, 
                                                          RArray<TInt>& aContactsInGroup,
                                                          TInt keyboardMode)
    {
    PRINT ( _L("Enter CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );

    // Convert the individual queries to string form
    RPointerArray<CPsQuery> mySearchQuery;

    // Remember a temporary copy of query list
    // Copy the content of searchQuery
    for (TInt i=0; i<searchQuery.Count(); i++)
        {
        CPsQuery* tempQuery = CPsQuery::NewL();
        iAlgorithm->FindUtilECE()->GetPartOfQueryL(*(searchQuery[i]), 0, 
                                                   searchQuery[i]->Count()-1, *tempQuery);
        mySearchQuery.Append(tempQuery);
        }

    // Sort the query items according to the length of each query 
    TLinearOrder<CPsQuery> rule(Compare4);
    mySearchQuery.Sort(rule);

    // To hold the match results
    RPointerArray<TDesC> tmpMatchSet;

    // Parse thru each search set elements and filter the results    
    for (int index = 0; index < searchSet.Count(); index++)
        {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*> (searchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();

        TBool isMatch = ETrue;
        TUint8 wordMatches = 0;

        // Reset iWordMatches to zero 
        ClearWordMatches();

        // Check for each query atleast one data element matches
        // Loop from the last query so that longest match is seen first
        for (TInt queryIndex = mySearchQuery.Count() - 1; queryIndex >= 0; queryIndex--)            
            {
            TBool queryMatch = EFalse;
            CPsQuery* tmpPsQuery = mySearchQuery[queryIndex];

            for (TInt dataIndex = 0; dataIndex < psData->DataElementCount(); dataIndex++)
                {
                // Filter off data fields not required in search
                TReal bitIndex;
                Math::Pow(bitIndex, 2, dataIndex);

                TUint8 filter = (TUint8) bitIndex & aFilteredDataMatch;
                if (filter == 0x0)
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
                    
                    TPtrC queryPtr = tmpPsQuery->QueryAsStringLC();
                    TBool matched = EFalse;
                    
                    if (keyboardMode == EModeUndefined)
                        {
                        matched = iAlgorithm->FindUtilECE()->MatchRefineL(tmpData, *tmpPsQuery);
                        }
                    else if (keyboardMode == EItut)
                        {
                        matched = iAlgorithm->FindUtil()->Interface()->
                                  MatchRefineL(tmpData, queryPtr, ECustomConverter, iAlgorithm);
                        }
                    else // Qwerty
                        {
                        matched = iAlgorithm->FindUtil()->Interface()->MatchRefineL(tmpData, queryPtr);
                        
                        }
                    
                    CleanupStack::PopAndDestroy(); // queryPtr
                    
                    // Compare the data against query
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
                            if (iAlgorithm->FindUtilECE()->IsChineseWord(tmpData))
                                {
                                len = 1;
                                }

                            HBufC* seq = HBufC::NewL(len);
                            *seq = tmpData.Mid(0, len);

                            seq->Des().UpperCase();
                            TIdentityRelation<TDesC> searchRule(Compare3);
                            if (tmpMatchSet.Find(seq, searchRule) == KErrNotFound)
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
            if (queryMatch == EFalse)
                {
                isMatch = EFalse;
                break;
                }
            }

        // If match add the element to the result set
        //  And before adding to the result set, check if there is atleast one match per query
        if ((isMatch) && (wordMatches >= mySearchQuery.Count()))
            {
            if (isSearchInGroup)
                {
                if (aContactsInGroup.Find(psData->Id()) != KErrNotFound)
                    {
                    aAlgorithmFilterHelper->AddL(psData, tmpMatchSet);
                    }
                }
            else
                {
                aAlgorithmFilterHelper->AddL(psData, tmpMatchSet);
                }
            }

        // Cleanup the match sequence array as 
        // they are stored in pattern details structure
        tmpMatchSet.ResetAndDestroy();
        }

    mySearchQuery.Reset();

    PRINT ( _L("End CPcsAlgorithm2MultiSearchHelper::FilterResultsMultiL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::SetWordMap()
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::SetWordMap(TInt aIndex, TInt aPosition)
    {
    TReal val;
    Math::Pow(val, 2, aPosition);

    iWordMatches[aIndex] |= (TUint8) val;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::IsWordMatch()
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2MultiSearchHelper::IsWordMatch(TInt aDataIndex, TInt aWordIndex)
    {
    TReal val;
    Math::Pow(val, 2, aWordIndex);

    return (iWordMatches[aDataIndex] & (TUint8) val);
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ExtractQueryL()
// Extracts the query as a string. If the mode of query item is ITU numeric
// character is used. Else the character is used.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ExtractQueryL(CPsQuery& aQuery, TDes& aOutput)
    {
    for (int i = 0; i < aQuery.Count(); i++)
        {
        if (aQuery.GetItemAtL(i).Mode() == EItut)
            {
            TBuf<KPsQueryMaxLen> outBuf;
            keyMap->GetNumericKeyString(aQuery.QueryAsStringLC(), outBuf);
            aOutput.Append(outBuf[i]);
            CleanupStack::PopAndDestroy();
            }
        else
            {
            aOutput.Append(aQuery.GetItemAtL(i).Character());
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2MultiSearchHelper::ExtractQueryL()
// Converts the input data refering the modes in the query.
// If the mode of query item is ITU numeric character is used. Else the character
// is used.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2MultiSearchHelper::ExtractQueryL(TDesC& aInput, CPsQuery& aQuery, TDes& aOutput)
    {
    TInt len = -1;

    // Always loop thru the lowest length
    if (aInput.Length() > aQuery.Count())
        {
        len = aQuery.Count();
        }
    else
        {
        len = aInput.Length();
        }

    for (int i = 0; i < len; i++)
        {
        if (aQuery.GetItemAtL(i).Mode() == EItut)
            {
            TBuf<KPsQueryMaxLen> outBuf;
            keyMap->GetNumericKeyString(aInput, outBuf);
            aOutput.Append(outBuf[i]);
            }
        else
            {
            aOutput.Append(aInput[i]);
            }
        }
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
// CPcsAlgorithm2MultiSearchHelper::BitsSet32
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
            for (; end < textLength && !aQuery.GetItemAtL(end).Character().IsSpace(); ++end)
                {
                }

            // Create a new query object and append
            CPsQuery* newQuery = CPsQuery::NewL();
            for (int i = beg; i < end; i++)
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
// CPcsAlgorithm2MultiSearchHelper::FilterDataFieldsL()
// Constructs a bit pattern using the required/supported data fields
// For example, 6, 4 and 27 are supported fields <-- 00000111
//              6 and 4 are required fields      <-- 00000011
// Bit pattern returned is 00000011.
// ----------------------------------------------------------------------------
TUint8 CPcsAlgorithm2MultiSearchHelper::FilterDataFieldsL(RArray<TInt>& aRequiredDataFields, 
                                                          RArray<TInt>& aSupportedDataFields)
    {
    TUint8 filteredMatch = 0x0;

    for (int i = 0; i < aSupportedDataFields.Count(); i++)
        {
        for (int j = 0; j < aRequiredDataFields.Count(); j++)
            {
            if (aSupportedDataFields[i] == aRequiredDataFields[j])
                {
                TReal val;
                Math::Pow(val, 2, i);

                filteredMatch |= (TUint8) val;
                }
            }
        }

    return filteredMatch;
    }

// End of file


