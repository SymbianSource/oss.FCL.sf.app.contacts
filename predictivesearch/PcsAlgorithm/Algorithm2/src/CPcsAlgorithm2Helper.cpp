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
* Description: Predictive Contact Search Algorithm 1 helper class
*
*/

// INCLUDES
#include <FindUtil.h>
#include "FindUtilChineseECE.h"
#include "CPcsAlgorithm2.h"
#include "CPcsAlgorithm2Helper.h"
#include "CPcsAlgorithm2Utils.h"
#include "CPcsDebug.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsData.h"
#include "CWords.h"
#include "CPsQuery.h"
#include "CPsQueryItem.h"
#include "CPsDataPluginInterface.h"
#include "CPcsPoolElement.h"

// Compare functions
TBool Compare1(const TDesC& aFirst, const TDesC& aSecond)
    {
    return aFirst == aSecond;
    }

TBool Compare2(const TDesC& aFirst, const TDesC& aSecond)
    {
    return CPcsAlgorithm2Utils::MyCompareC(aFirst, aSecond);
    }

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2Helper* CPcsAlgorithm2Helper::NewL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::NewL") );

    CPcsAlgorithm2Helper* self = new (ELeave) CPcsAlgorithm2Helper();
    CleanupStack::PushL(self);
    self->ConstructL(aAlgorithm);
    CleanupStack::Pop(self);

    PRINT ( _L("End CPcsAlgorithm2Helper::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::CPcsAlgorithm2Helper
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2Helper::CPcsAlgorithm2Helper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::CPcsAlgorithm2") );
    PRINT ( _L("End CPcsAlgorithm2Helper::CPcsAlgorithm2") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::ConstructL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::ConstructL") );

    iAlgorithm = aAlgorithm;
    keyMap = iAlgorithm->GetKeyMap();

    PRINT ( _L("End CPcsAlgorithm2Helper::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::CPcsAlgorithm2Helper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper") );
    iSearchResultsArr.ResetAndDestroy();
    PRINT ( _L("End CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchMixedL
// Search function for input with both ITU-T and QWERTY mode
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchMixedL(const CPsSettings& aSettings,
                                        CPsQuery& aPsQuery, TBool isSearchInGroup,
                                        RArray<TInt>& aContactsInGroup,
                                        RPointerArray<CPsData>& searchResults,
                                        RPointerArray<CPsPattern>& searchSeqs)
    {
    __LATENCY_MARK ( _L("CPcsAlgorithm2Helper::SearchMixedL") );

    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchMixedL") );

    // Create filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper =
            CPcsAlgorithm2FilterHelper::NewL(sortType);

    // Reset the result set array for new search
    iSearchResultsArr.ResetAndDestroy();

    // Get the data stores
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    aSettings.DisplayFieldsL(requiredDataFields);
    
    // Search from cache based on first character
     const CPsQueryItem& firstCharItem = aPsQuery.GetItemAtL(0);
     TInt numValue  = keyMap->PoolIdForCharacter( firstCharItem.Character() );

    // Perform search for each required data store
    RPointerArray<CPcsPoolElement> elements;

    for (int dsIndex = 0; dsIndex < aDataStores.Count(); dsIndex++)
        {

        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(aDataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(numValue, elements);

        // Perform filtering
        FilterResultsMixedL(filterHelper, elements, aPsQuery,
                            isSearchInGroup, aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore               
        if (sortType == EAlphabetical)
            {
            filterHelper->GetResults(*(iSearchResultsArr[dsIndex]));
            }

        elements.Reset();
        }

    aDataStores.ResetAndDestroy();
    requiredDataFields.Reset();

    // If alphabetical sorting, merge the result sets of all datastores
    if (sortType == EAlphabetical)
        {
        // Form the complete searchResults array
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        searchResults);
        }
    else
        {
        // Results are already sorted pattern based
        filterHelper->GetResults(searchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(searchSeqs);

    PRINT1 ( _L("Number of search results = %d"), searchResults.Count() );

    // Cleanup         
    for (TInt i = 0; i < iSearchResultsArr.Count(); i++)
        {
        iSearchResultsArr[i]->Reset();
        delete iSearchResultsArr[i];
        iSearchResultsArr[i] = NULL;
        }

    iSearchResultsArr.Reset();

    CleanupStack::PopAndDestroy(); // query
    delete filterHelper;

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchMixedL") );

    __LATENCY_MARKEND ( _L("CPcsAlgorithm2Helper::SearchMixedL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchITUL
// Search function for ITU-T style
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchITUL(const CPsSettings& aSettings,
                                      CPsQuery& aPsQuery, TBool isSearchInGroup,
                                      RArray<TInt>& aContactsInGroup,
                                      RPointerArray<CPsData>& searchResults,
                                      RPointerArray<CPsPattern>& searchSeqs)
    {
    __LATENCY_MARK ( _L("CPcsAlgorithm2Helper::SearchITUL") );

    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchITUL") );

    // Create filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper =
            CPcsAlgorithm2FilterHelper::NewL(sortType);

    // Convert the query to string
    TPtrC queryPtr = aPsQuery.QueryAsStringLC();

    // Search from cache based on first character
    const CPsQueryItem& firstCharItem = aPsQuery.GetItemAtL(0);
    TInt numValue  = keyMap->PoolIdForCharacter( firstCharItem.Character() );

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

    for (int dsIndex = 0; dsIndex < aDataStores.Count(); dsIndex++)
        {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(aDataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(numValue, elements);

        // Perform filtering
        FilterResultsL(filterHelper, elements, queryPtr,
                       isSearchInGroup, aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore               
        if (sortType == EAlphabetical)
            {
            filterHelper->GetResults(*(iSearchResultsArr[dsIndex]));
            }

        elements.Reset();
        }

    aDataStores.ResetAndDestroy();
    requiredDataFields.Reset();

    // If alphabetical sorting, merge the result sets of all datastores
    if (sortType == EAlphabetical)
        {
        // Merge the result sets of individual datastores alphabetically
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        searchResults);
        }
    else
        {
        // Results are already sorted pattern based 
        filterHelper->GetResults(searchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(searchSeqs);

    PRINT1 ( _L("Number of search results = %d"), searchResults.Count() );

    // Cleanup
    for (TInt i = 0; i < iSearchResultsArr.Count(); i++)
        {
        iSearchResultsArr[i]->Reset();
        delete iSearchResultsArr[i];
        iSearchResultsArr[i] = NULL;
        }
    iSearchResultsArr.Reset();

    CleanupStack::PopAndDestroy(); // query
    delete filterHelper;

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchITUL") );
    __LATENCY_MARKEND ( _L("CPcsAlgorithm2Helper::SearchITUL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchQWERTYL
// Search function for QWERTY style
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchQWERTYL(const CPsSettings& aSettings,
                                         CPsQuery& aPsQuery, TBool isSearchInGroup,
                                         RArray<TInt>& aContactsInGroup,
                                         RPointerArray<CPsData>& searchResults,
                                         RPointerArray<CPsPattern>& searchSeqs)
    {
    __LATENCY_MARK ( _L("CPcsAlgorithm2Helper::SearchQWERTYL") );
    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchQWERTYL") );

    // te filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper =
            CPcsAlgorithm2FilterHelper::NewL(sortType);

    // Convert the query to string
    TPtrC queryPtr = aPsQuery.QueryAsStringLC();

    // Search from cache based on first character
    const CPsQueryItem& firstCharItem = aPsQuery.GetItemAtL(0);
    TInt numValue  = keyMap->PoolIdForCharacter( firstCharItem.Character() );
    
    // Reset the result set array for new search
    iSearchResultsArr.ResetAndDestroy();

    // Get the data stores  
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);

    // Get the required display fields from the client
    RArray<TInt> requiredDataFields;
    aSettings.DisplayFieldsL(requiredDataFields);

    // Perform search for each of the required data stores
    RPointerArray<CPcsPoolElement> elements;

    for (int dsIndex = 0; dsIndex < aDataStores.Count(); dsIndex++)
        {

        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iSearchResultsArr.Append(temp);

        // Get the contents for this data store
        TInt arrayIndex = iAlgorithm->GetCacheIndex(*(aDataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }
        CPcsCache* cache = iAlgorithm->GetCache(arrayIndex);
        cache->GetContactsForKeyL(numValue, elements);

        // Perform filtering
        FilterResultsQwertyL(filterHelper, elements, queryPtr,
                             isSearchInGroup, aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore               
        if (sortType == EAlphabetical)
            {
            filterHelper->GetResults(*(iSearchResultsArr[dsIndex]));
            }

        elements.Reset();
        }
    aDataStores.ResetAndDestroy();
    requiredDataFields.Reset();

    // If alphabetical sorting, merge the result sets of all datastores
    if (sortType == EAlphabetical)
        {
        // Form the complete searchResults array
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        searchResults);
        }
    else
        {
        // Results are already sorted pattern based
        filterHelper->GetResults(searchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(searchSeqs);

    PRINT1 ( _L("Number of search results = %d"), searchResults.Count() );

    // Cleanup             
    for (TInt i = 0; i < iSearchResultsArr.Count(); i++)
        {
        iSearchResultsArr[i]->Reset();
        delete iSearchResultsArr[i];
        iSearchResultsArr[i] = NULL;
        }
    iSearchResultsArr.Reset();

    CleanupStack::PopAndDestroy(); // query
    delete filterHelper;

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchQWERTYL") );
    __LATENCY_MARKEND ( _L("CPcsAlgorithm2Helper::SearchQWERTYL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchMatchSeqL
// Funciton to search matching sequences in the input text
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchMatchSeqL(HBufC* /*aQuery*/, TDesC& aData,
                                           RPointerArray<TDesC>& /*aMatchSet*/, 
                                           CPsQuery& /*aPsQuery*/,
                                           RArray<TPsMatchLocation>& aMatchLocation)
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchMatchSeqL") );

    // Convert the data into words		     
    TLex lex(aData);

    // First word
    TPtrC token = lex.NextToken();

    TInt beg = lex.Offset() - token.Length(); // start index of match sequence

    // Search thru multiple words
    while (token.Length() != 0)
        {
        TPsMatchLocation tempLocation;

        // check for directionality of the text
        TBool found(EFalse);
        TBidiText::TDirectionality dir = TBidiText::TextDirectionality(token, &found);

        tempLocation.index = beg;
        tempLocation.length = 0;
        tempLocation.direction = dir;

        // Add the match location to the data structure array
        aMatchLocation.Append(tempLocation);

        // Next word
        token.Set(lex.NextToken());
        beg = lex.Offset() - token.Length(); // start index of next word
        }

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchMatchSeqL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::FilterResultsL
// Subset search function
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::FilterResultsL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper, 
                                          RPointerArray<CPcsPoolElement>& searchSet, 
                                          const TDesC& searchQuery,
                                          TBool isSearchInGroup, 
                                          RArray<TInt>& aContactsInGroup)
    {
    __LATENCY_MARK ( _L("CPcsAlgorithm2Helper::FilterResultsL") );
    PRINT ( _L("Enter CPcsAlgorithm2Helper::FilterResultsL") );

    // Convert the search condition to numeric key string
    TBuf<KPsQueryMaxLen> tmpSearchQuery;
    keyMap->GetNumericKeyString(searchQuery, tmpSearchQuery);
    
    //Holds the first char of first name of a contact
    TBuf<10> firstChar;   

    PRINT2 ( _L("Numeric Key String for %S = %S"), &searchQuery, &tmpSearchQuery );

    // Parse thru each search set elements and filter the results
    for (int index = 0; index < searchSet.Count(); index++)
        {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*> (searchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();
        RPointerArray<TDesC> tempMatchSeq;

        // Search thru multiple words
        TBuf<KBufferMaxLen> token;
        TBuf<KBufferMaxLen> firstName;
        TBuf<KBufferMaxLen> lastName;
        lastName.Append(psData->Data(iAlgorithm->GetLastNameIndex())->Des());
        firstName.Append(psData->Data(iAlgorithm->GetFirstNameIndex())->Des());

        CFindUtilChineseECE* pFindUtilEce = iAlgorithm->FindUtilECE();
        TBool matched  = 0;

        // If has Chinese word and the first name doesn't start with Chinese character, then should add a space
        // before the first name, otherwise intitial letter searching will not function
        if( pFindUtilEce->IsChineseWord(lastName) || pFindUtilEce->IsChineseWord(firstName))
            {
            token.Append(lastName);
            if (firstName.Length())
                {
                firstChar.Zero();
                firstChar.Append(firstName[0]);
                if (!pFindUtilEce->IsChineseWord(firstChar) )
                    {
                    token.Append(KSpace);
                    }
                token.Append(firstName);
                }

            if (token.Length() != 0)
                {
                matched = iAlgorithm->FindUtil()->Interface()->
                    MatchRefineL(token, tmpSearchQuery, ECustomConverter, iAlgorithm);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }
        else
            {
            // If contact name only has western word, then should send 
            // "first name","last name" and "last name + first name" to FindUtil to do the search
            const TInt lastnameLen = lastName.Length();
            const TInt firstnameLen = firstName.Length();
            if(lastnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->
                    MatchRefineL(lastName, tmpSearchQuery, ECustomConverter, iAlgorithm);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    }
                }

            if(!matched && firstnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->
                    MatchRefineL(firstName, tmpSearchQuery, ECustomConverter, iAlgorithm);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            
            token.Append(lastName);
            token.Append(firstName);
            if (!matched && lastnameLen && firstnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->
                    MatchRefineL(token, tmpSearchQuery, ECustomConverter, iAlgorithm);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }

        if (matched)
            {
            // Extract matched character sequence, don't need to be accurate for Chinese variant
            const TInt len = 1;  
            HBufC* seq = HBufC::NewLC(len);
            *seq = token.Mid(0, len);
            seq->Des().UpperCase();

            TIdentityRelation<TDesC> rule(Compare1);
            if (tempMatchSeq.Find(seq, rule) == KErrNotFound)
                {
                tempMatchSeq.Append(seq);
                CleanupStack::Pop();
                }
            else
                {
                CleanupStack::PopAndDestroy();
                }
                

            // Add the result        
            if (isSearchInGroup)
                {
                if (aContactsInGroup.Find(psData->Id()) != KErrNotFound)
                    {
                    aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                    }
                }
            else
                {
                aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                }
            }

        // Cleanup the match sequence array as 
        // they are stored in pattern details structure
        tempMatchSeq.ResetAndDestroy();
        }

    PRINT ( _L("End CPcsAlgorithm2Helper::FilterResultsL") );
    __LATENCY_MARKEND ( _L("CPcsAlgorithm2Helper::FilterResultsL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::FilterResultsQwertyL
// Subset search function 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::FilterResultsQwertyL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper, 
                                                RPointerArray<CPcsPoolElement>& searchSet, 
                                                const TDesC& searchQuery,TBool isSearchInGroup, 
                                                RArray<TInt>& aContactsInGroup)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::FilterResultsQwertyL") );

    TBuf<50> tmpSearchQuery = searchQuery;
    tmpSearchQuery.LowerCase();
    
    //Holds the first char of first name of a contact
    TBuf<10> firstChar;

    // Parse thru each search set elements and filter the results    
    for (int index = 0; index < searchSet.Count(); index++)
        {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*> (searchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();
        RPointerArray<TDesC> tempMatchSeq;

        // Parse thru each data and filter the results
        TBuf<KBufferMaxLen> token;
        TBuf<KBufferMaxLen> firstName;
        TBuf<KBufferMaxLen> lastName;
        lastName.Append(psData->Data(iAlgorithm->GetLastNameIndex())->Des());
        firstName.Append(psData->Data(iAlgorithm->GetFirstNameIndex())->Des());

        CFindUtilChineseECE* pFindUtilEce = iAlgorithm->FindUtilECE();
        TBool matched  = 0;

        // If has Chinese word and the first name doesn't start with Chinese character, then should add a space
        // before the first name, otherwise intitial letter searching will not function
        if( pFindUtilEce->IsChineseWord(lastName) || pFindUtilEce->IsChineseWord(firstName))
            {
            token.Append(lastName);
            if (firstName.Length())
                {
                firstChar.Zero();
                firstChar.Append(firstName[0]);
                if (!pFindUtilEce->IsChineseWord(firstChar) )
                    {
                    token.Append(KSpace);
                    }
                token.Append(firstName);
                }

            if (token.Length() != 0)
                {
                matched = iAlgorithm->FindUtil()->Interface()->MatchRefineL(token, tmpSearchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }
        else
            {
            // If contact name only has western word, then should send 
            // "first name","last name" and "last name + first name" to FindUtil to do the search
            const TInt lastnameLen = lastName.Length();
            const TInt firstnameLen = firstName.Length();
            if(lastnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->MatchRefineL(lastName, tmpSearchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    }
                }

            if(!matched && firstnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->MatchRefineL(firstName, tmpSearchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            
            token.Append(lastName);
            token.Append(firstName);
            if (!matched && lastnameLen && firstnameLen)
                {
                matched = iAlgorithm->FindUtil()->Interface()->MatchRefineL(token, tmpSearchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }
        
        if (matched)
            {
            // Extract matched character sequence, don't need to be accurate for Chinese variant
            const TInt len = 1;
            HBufC* seq = HBufC::NewLC(len);
            *seq = token.Mid(0, len);
            seq->Des().UpperCase();

            TIdentityRelation<TDesC> rule(Compare1);
            if (tempMatchSeq.Find(seq, rule) == KErrNotFound)
                {
                tempMatchSeq.Append(seq);
                CleanupStack::Pop();
                }
            else
                {
                CleanupStack::PopAndDestroy();
                }
            
            // Add the result
            if (isSearchInGroup)
                {
                if (aContactsInGroup.Find(psData->Id()) != KErrNotFound)
                    {
                    aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                    }
                }
            else
                {
                aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                }
            }

        // Cleanup the match sequence array as 
        // they are stored in pattern details structure
        tempMatchSeq.ResetAndDestroy();
        }

    PRINT ( _L("End CPcsAlgorithm2Helper::FilterResultsQwertyL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::FilterResultsMixedL
// Subset search function
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::FilterResultsMixedL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper, 
                                               RPointerArray<CPcsPoolElement>& searchSet, 
                                               CPsQuery& searchQuery, TBool isSearchInGroup,
                                               RArray<TInt>& aContactsInGroup)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::FilterResultsMixedL") );

    // Convert the search query to alpha numeric string
    TBuf<50> tmpSearchQuery;
    ExtractQueryL(searchQuery, tmpSearchQuery);
    tmpSearchQuery.LowerCase();
    TBuf<10> firstChar;

    // Parse thru each search set elements and filter the results    
    for (int index = 0; index < searchSet.Count(); index++)
        {
        CPcsPoolElement* poolElement = static_cast<CPcsPoolElement*> (searchSet[index]);
        CPsData* psData = poolElement->GetPsData();
        psData->ClearDataMatches();
        RPointerArray<TDesC> tempMatchSeq;

        // Parse thru each data and filter the results
        TBuf<255> token;
        TBuf<KBufferMaxLen> firstName;
        TBuf<KBufferMaxLen> lastName;
        lastName.Append(psData->Data(iAlgorithm->GetLastNameIndex())->Des());
        firstName.Append(psData->Data(iAlgorithm->GetFirstNameIndex())->Des());

        CFindUtilChineseECE* pFindUtilEce = iAlgorithm->FindUtilECE();
        TBool matched  = 0;

        // If has Chinese word and the first name doesn't start with Chinese character, then should add a space
        // before the first name, otherwise intitial letter searching will not function
        if( pFindUtilEce->IsChineseWord(lastName) || pFindUtilEce->IsChineseWord(firstName))
            {
            token.Append(lastName);
            if (firstName.Length())
                {
                firstChar.Zero();
                firstChar.Append(firstName[0]);
                if (!pFindUtilEce->IsChineseWord(firstChar) )
                    {
                    token.Append(KSpace);
                    }
                token.Append(firstName);
                }

            if (token.Length() != 0)
                {
                matched = pFindUtilEce->MatchRefineL(token, searchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }
        else
            {
            // If contact name only has western word, then should send 
            // "first name","last name" and "last name + first name" to FindUtil to do the search
            const TInt lastnameLen = lastName.Length();
            const TInt firstnameLen = firstName.Length();
            if(lastnameLen)
                {
                matched = pFindUtilEce->MatchRefineL(lastName, searchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    }
                }

            if(!matched && firstnameLen)
                {
                matched = pFindUtilEce->MatchRefineL(firstName, searchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            
            token.Append(lastName);
            token.Append(firstName);
            if (!matched && lastnameLen && firstnameLen)
                {
                matched = pFindUtilEce->MatchRefineL(token, searchQuery);

                if (matched)
                    {
                    psData->SetDataMatch(iAlgorithm->GetLastNameIndex());
                    psData->SetDataMatch(iAlgorithm->GetFirstNameIndex());
                    }
                }
            }

        if (matched)
            {
            // Extract matched character sequence, don't need to be accurate for Chinese variant
            const TInt len = 1;       
        
            HBufC* seq = HBufC::NewLC(len);
            *seq = token.Mid(0, len);
            seq->Des().UpperCase();

            TIdentityRelation<TDesC> rule(Compare1);
            if (tempMatchSeq.Find(seq, rule) == KErrNotFound)
                {
                tempMatchSeq.Append(seq);
                CleanupStack::Pop();
                }
            else
                {
                CleanupStack::PopAndDestroy();
                }
            
            // Add the result
            if (isSearchInGroup)
                {
                if (aContactsInGroup.Find(psData->Id()) != KErrNotFound)
                    {
                    aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                    }
                }
            else
                {
                aAlgorithmFilterHelper->AddL(psData, tempMatchSeq);
                }
            }

        // Cleanup the match sequence array as 
        // they are stored in pattern details structure
        tempMatchSeq.ResetAndDestroy();
        }

    PRINT ( _L("End CPcsAlgorithm2Helper::FilterResultsMixedL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SortSearchSeqsL()
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SortSearchSeqsL(RPointerArray<TDesC>& aSearchSeqs)
    {
    // Sort the search seqs
    TLinearOrder<TDesC> rule(Compare2);
    aSearchSeqs.Sort(rule);
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::ExtractQueryL()
// Required for mixed mode search.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::ExtractQueryL(CPsQuery& aQuery, TDes& aOutput)
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
// CPcsAlgorithm2Helper::ExtractQueryL()
// Required for mixed mode search.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::ExtractQueryL(TDesC& aInput, CPsQuery& aQuery, TDes& aOutput)
    {
    TInt len = -1;

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
// CPcsAlgorithm2Helper::FilterDataFieldsL()
// Constructs a bit pattern using the required/supported data fields
// For example, 6, 4 and 27 are supported fields <-- 00000111
//              6 and 4 are required fields      <-- 00000011
// Bit pattern returned is 00000011.
// ----------------------------------------------------------------------------
TUint8 CPcsAlgorithm2Helper::FilterDataFieldsL(RArray<TInt>& aRequiredDataFields, 
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


