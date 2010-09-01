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
* Description: Predictive Contact Search Algorithm 1 main class
*
*/

// INCLUDES
#include <VPbkEng.rsg>
#include <PtiEngine.h>
#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>

#include "CPcsAlgorithm2.h"
#include "CPcsAlgorithm2Helper.h"
#include "CPcsAlgorithm2MultiSearchHelper.h"
#include "CPcsAlgorithm2Utils.h"
#include "CPcsDebug.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsData.h"
#include "CWords.h"
#include "CPsQuery.h"
#include "CPsDataPluginInterface.h"
#include "CPcsDefs.h"
#include "FindUtilChineseECE.h"


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2* CPcsAlgorithm2::NewL()
    {
    PRINT ( _L("Enter CPcsAlgorithm2::NewL") );

    CPcsAlgorithm2* self = new (ELeave) CPcsAlgorithm2();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT ( _L("End CPcsAlgorithm2::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::CPcsAlgorithm2
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2::CPcsAlgorithm2()
    {
    PRINT ( _L("Enter CPcsAlgorithm2::CPcsAlgorithm2") );
    PRINT ( _L("End CPcsAlgorithm2::CPcsAlgorithm2") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::ConstructL()
    {
    PRINT ( _L("Enter CPcsAlgorithm2::ConstructL") );

    iCacheStatus = ECachingNotStarted; // Starting status
    iCacheError = KErrNone; // No error
    iCacheCount = 0; // No data

    iFindUtilECE = CFindUtilChineseECE::NewL(this);
    
    iPluginLauncher = CIdle::NewL( CActive::EPriorityStandard );

    // Define cache status property used to inform clients about the caching status.
    DefinePropertyL( EPsKeyCacheStatus );
    
    // Define cache error property used to inform client about the errors.
    DefinePropertyL( EPsKeyCacheError );
    
    // Define properties for notifying about cache updates
    DefinePropertyL( EPsKeyContactRemovedCounter );
    DefinePropertyL( EPsKeyContactModifiedCounter );
    DefinePropertyL( EPsKeyContactAddedCounter );

    // Initialize key map and pti engine
    TInt keyMapErr = KErrNone;
    TRAP( keyMapErr, iKeyMap = CPcsKeyMap::NewL( this ) );
    if (keyMapErr != KErrNone)
        {
        PRINT ( _L("**********************************************."));
        PRINT1 ( _L("CPcsAlgorithm2::ConstructL() KeyMap construction error. The keymap crashed with error code %d."), keyMapErr );
        PRINT ( _L("Please check the keypad/language for which keymap got crashed.") );
        PRINT ( _L("**********************************************."));
        User::Leave( keyMapErr ); // we can't go on without a key map; constructing cache needs it
        }
    
    // Initialize helpers
    iHelper = CPcsAlgorithm2Helper::NewL(this);
    iMultiSearchHelper = CPcsAlgorithm2MultiSearchHelper::NewL(this);

    if(!iPluginLauncher->IsActive())
        {
        iPluginLauncher->Start(TCallBack(CPcsAlgorithm2::DoLaunchPluginsL, this));
        }

    PRINT ( _L("End CPcsAlgorithm2::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::~CPcsAlgorithm2
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm2::~CPcsAlgorithm2()
    {
    PRINT ( _L("Enter CPcsAlgorithm2::~CPcsAlgorithm2") );

    // Clear TLS
    Dll::SetTls(NULL);

    // Cleanup cache
    iPcsCache.ResetAndDestroy();

    // Cleanup adapters interface and key handling
    delete iKeyMap;
    delete iPsDataPluginInterface;

    // Cleanup helpers
    delete iHelper;
    delete iMultiSearchHelper;

    delete iFindUtilECE;
    
    delete iPluginLauncher;

    PRINT ( _L("End CPcsAlgorithm2::~CPcsAlgorithm2") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::DefinePropertyL
// Define a P&S property with given key under the internal category 
// UID of PCS. Leave if definition fails for any other reason than
// key already existing. 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::DefinePropertyL( TPcsInternalKeyCacheStatus aPsKey )
    {
    TInt err = RProperty::Define( KPcsInternalUidCacheStatus, 
                                  aPsKey, 
                                  RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError(err);
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::RemoveSpacesL
// Remove leading and trailing spaces of search query
// ----------------------------------------------------------------------------
void  CPcsAlgorithm2::RemoveSpacesL(CPsQuery& aQuery)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::RemoveSpacesL") );

    // Remove all leading " "
    while ( aQuery.Count() > 0 )
        {
        CPsQueryItem& item = aQuery.GetItemAtL(0); // First
        if ( !item.Character().IsSpace() )
            {
            break;
            }
        aQuery.Remove(0);
        }

    // Remove all trailing " "
    while ( aQuery.Count() > 0 )
        {
        CPsQueryItem& item = aQuery.GetItemAtL(aQuery.Count()-1); // Last
        if ( !item.Character().IsSpace() )
            {
            break;
            }
        aQuery.Remove(aQuery.Count()-1);
        }
    
    PRINT ( _L("End CPcsAlgorithm2::RemoveSpacesL") );
    }    

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::ReplaceZeroWithSpaceL
// Replace first occurance of '0' in a sequence of '0's in ITU-T with space
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2::ReplaceZeroWithSpaceL(CPsQuery& aQuery)
    {
    PRINT ( _L("Enter CPcsAlgorithm1::ReplaceZeroWithSpaceL") );

       //PRINTQUERY ( _L("CPcsAlgorithm1::ReplaceZeroWithSpaceL (BEFORE): "), aQuery );

       TBool queryModified = EFalse;    

       /* In phones like E52 and E55, where the "0" and the " " characters are on
        * the same key, the "0"s have to be considered as possible separators.
        *
        * In phones like N97 and E72, where the "0" and the " " characters are on
        * different keys, the "0"s must not be considered as possible separators.
        */

       // Skip initial "0"s, they are not replaced into spaces
       TInt skipIndex = 0;
       while ( (skipIndex < aQuery.Count()) && 
               (aQuery.GetItemAtL(skipIndex).Character().GetNumericValue() == 0) )
       {
           skipIndex++;
       }
       
       // Replace remaining "0"s into spaces in case they are entered with a keyboard
       // that has "0" and " " on the same key.
       const TInt queryCount = aQuery.Count();
       for ( TInt index = skipIndex; index < queryCount; index++ )
       {
           CPsQueryItem& item = aQuery.GetItemAtL(index);

           if ( iKeyMap->GetSpaceAndZeroOnSameKey( item.Mode() ) &&
                item.Character().GetNumericValue() == 0 )
           {
               item.SetCharacter(KSpace);
               queryModified = ETrue;
           }
       }
       
       //PRINTQUERY ( _L("CPcsAlgorithm1::ReplaceZeroWithSpaceL (AFTER): "), aQuery );

       PRINT1 ( _L("CPcsAlgorithm1::ReplaceZeroWithSpaceL: Query modified (0=not, 1=yes): %d"), queryModified );

       PRINT ( _L("End CPcsAlgorithm1::ReplaceZeroWithSpaceL") );

       return  queryModified;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::PerformSearchL
// Search function for cache
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::PerformSearchL(const CPsSettings& aSettings, 
                                    CPsQuery& aQuery, 
                                    RPointerArray<CPsClientData>& aSearchResults,
                                    RPointerArray<CPsPattern>& aSearchSeqs)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::PerformSearchL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2::PerformSearchL") );
    
    // Check aSettings   
    RPointerArray<TDesC> searchUris;
    CleanupResetAndDestroyPushL( searchUris );
    aSettings.SearchUrisL(searchUris);
         
    if ( searchUris.Count() <= 0)
    {
        PRINT ( _L("searchUris.Count() <= 0, Leave from CPcsAlgorithm1::PerformSearchL") );
        User::Leave(KErrArgument); 
    }
    CleanupStack::PopAndDestroy( &searchUris ); // ResetAndDestroy

    // Local arrays to hold the search results 
    RPointerArray<CPsData> tempSearchResults;
    CleanupClosePushL( tempSearchResults );
    RPointerArray<CPsData> tempSearchResults1;
    CleanupClosePushL( tempSearchResults1 );

    // -------------------- Perform the basic search --------------------------

    RemoveSpacesL(aQuery);
    PRINTQUERY ( _L("CPcsAlgorithm2::PerformSearchL: 1st search query: "), aQuery );
    DoSearchL(aSettings, aQuery, tempSearchResults, aSearchSeqs);

    // ------------------------------------------------------------------------

    // ---- Perform new search after "0" replacement if query is not empty ----
    /* Examples:
     * - If the original search string is "Abc0" then we will search again with "Abc".
     * - If the original search string is "00" then we will not search again.
     */
    TBool queryModified = ReplaceZeroWithSpaceL(aQuery);
    RemoveSpacesL(aQuery);
    // Perform query again if query got modified. 
    if (queryModified)
        {
        PRINTQUERY ( _L("CPcsAlgorithm2::PerformSearchL: 2nd search query: "), aQuery );
        DoSearchL(aSettings, aQuery, tempSearchResults1, aSearchSeqs);


        // Sort rule        
        TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrder);

        // Avoid duplicates and add new results
        TIdentityRelation<CPsData> identityRule(CPsData::CompareById);
        const TInt tempSearchResults1Count = tempSearchResults1.Count();
        if (aSettings.GetSortType() != EAlphabetical)
            {
            TInt insertPos = 0;
            for (TInt i = 0; i < tempSearchResults1Count; i++)
                {
                if (tempSearchResults.Find(tempSearchResults1[i],
                                           identityRule) == KErrNotFound)
                    {
                    tempSearchResults.Insert(tempSearchResults1[i], insertPos);
                    insertPos++;
                    }
                }
            }
        else
            {
            for (TInt i = 0; i < tempSearchResults1Count; i++)
                {
                if (tempSearchResults.Find(tempSearchResults1[i],
                                           identityRule) == KErrNotFound)
                    {
                    tempSearchResults.InsertInOrderAllowRepeats(tempSearchResults1[i], 
                                                                rule);
                    }
                }
            }
        }
    // ------------------------------------------------------------------------

    // ---------------------- Write result objects to the stream --------------
    // Truncate the result set if required
    TInt maxNumToDisplay = aSettings.MaxResults();
    TInt resultSetCount = tempSearchResults.Count();
    TInt numToDisplay = 0;
    if ( maxNumToDisplay == -1 )
        {
        numToDisplay = resultSetCount;
        }
    else
        {
        numToDisplay = Min( maxNumToDisplay, resultSetCount );
        }

    // Copy desired number of results from tempSearchResults to the results stream
    for (TInt i = 0; i < numToDisplay; i++)
        {
        aSearchResults.Append(WriteClientDataL(*(tempSearchResults[i])));
        }
    // ------------------------------------------------------------------------

    // Cleanup local results array
    CleanupStack::PopAndDestroy( &tempSearchResults1 ); // Close, don't destroy
    CleanupStack::PopAndDestroy( &tempSearchResults );  // Close, don't destroy

    // __LATENCY_MARKEND ( _L("CPcsAlgorithm2::PerformSearchL") );

    PRINT ( _L("End CPcsAlgorithm2::PerformSearchL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SearchInputL
// Search function for input string
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SearchInputL(CPsQuery& aQuery, 
                                  TDesC& aData,
                                  RPointerArray<TDesC>& aMatchSet,
                                  RArray<TPsMatchLocation>& aMatchLocation)
    {
    // __LATENCY_MARK ( _L("CPcsAlgorithm2::SearchInputL: ") );
    PRINT ( _L("Enter CPcsAlgorithm2::SearchInputL") );

    // Print input query for debug
    PRINTQUERY ( _L("CPcsAlgorithm2::SearchInputL: Search query: "), aQuery );

    // Print received search data
    PRINT1 ( _L("Search data received = %S"), &aData);


    // -------------------- Perform the basic search --------------------------

    RemoveSpacesL(aQuery);
    DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);

    // ------------------------------------------------------------------------   

    // ---- Perform new search after "0" replacement if query is not empty ----
    /* Examples:
     * - If the original search string is "Abc0" then we will search again with "Abc".
     * - If the original search string is "00" then we will not search again.
     */
    TBool queryModified = ReplaceZeroWithSpaceL(aQuery);
    RemoveSpacesL(aQuery);
    // If query got modified and the search query still contains something
    // perform a multi search again
    if (queryModified && aQuery.Count() > 0 && aMatchSet.Count() == 0  && aMatchLocation.Count() == 0 )
        {
        DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);
        }
    // ------------------------------------------------------------------------

    // --- Remove overlapping items from aMatchLocation ---
    TInt i = 0;
    TBool incrementFirstCursor;
    while ( i < aMatchLocation.Count() )
        {
        incrementFirstCursor = ETrue;
        TInt j = i+1;
        while ( j < aMatchLocation.Count() )
            {
            if ( CPcsAlgorithm2Utils::MatchesOverlap( aMatchLocation[j], aMatchLocation[i] ) )
                {
                // Remove match location item with smallest length if 2 items have same index
                if ( aMatchLocation[j].length <= aMatchLocation[i].length )
                    {
                    aMatchLocation.Remove(j);
                    continue; // Do not increment j
                    }
                else
                    {
                    aMatchLocation.Remove(i);
                    incrementFirstCursor = EFalse; // Do not increment i
                    break;
                    }
                }
            j++;
            }
        if ( incrementFirstCursor )
            {
            i++;
            }
        }

    // --- Remove from aMatchSet items which no longer have corresponding item in aMatchLocation ---
    HBufC* dataUpper = HBufC::NewLC(aData.Length());
    dataUpper->Des().Copy(aData);
    dataUpper->Des().UpperCase(); // Get uppercase, as aMatchSet is in upper case

    TInt k = 0;
    while ( k < aMatchSet.Count() )
        {
        TBool keepMatch = EFalse;
        TInt startCursor = -1;

        TInt offsetCursor;
        while ( KErrNotFound != (offsetCursor = dataUpper->Mid(startCursor + 1).Find(*aMatchSet[k])) )
            {
            // startCursor = index of match item *aMatchSet[k] into search string aData
            startCursor = offsetCursor + startCursor + 1;
            const TInt matchLocationCount = aMatchLocation.Count();
            for ( TInt i = 0; i < matchLocationCount; i++ )
                {
                // If match item was found in the location list, then keep it
                if ( (aMatchLocation[i].index == startCursor) &&
                     (aMatchLocation[i].length == aMatchSet[k]->Length()) )
                    {
                    keepMatch = ETrue;
                    break;
                    }
                }
            }

        if ( keepMatch )
            {
            k++;
            }
        else
            {
            aMatchSet.Remove(k); // Do not increment k
            }
        }
    CleanupStack::PopAndDestroy( dataUpper );
    // --- Remove items End ---

    // Sort match set
    iHelper->SortSearchSeqsL(aMatchSet);

    PRINT ( _L("End CPcsAlgorithm2::SearchInputL") );
    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2::SearchInputL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SearchMatchStringL
// Search function for input string, result also as string
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SearchMatchStringL( CPsQuery& /*aSearchQuery*/,
                                         TDesC& /*aSearchData*/,
                                         TDes& /*aMatch*/ )
    {
    PRINT ( _L("Enter CPcsAlgorithm2::SearchMatchStringL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2::SearchMatchStringL") ); 
    
    // TODO: Implementation missing
    
    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2::SearchMatchStringL") );

    PRINT ( _L("End CPcsAlgorithm2::SearchMatchStringL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::DoSearchL
// Search function helper
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::DoSearchL( const CPsSettings& aSettings, 
                                CPsQuery& aQuery, 
                                RPointerArray<CPsData>& aSearchResults,
                                RPointerArray<CPsPattern>& aSearchSeqs )
    {
    PRINT ( _L("Enter CPcsAlgorithm2::DoSearchL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2::DoSearchL") ); 
    
    // -(0)----------------- Check if group search is required ---------------    
    RArray<TInt> contactsInGroup;
    CleanupClosePushL( contactsInGroup );
    RArray<TInt> groupIdArray;
    CleanupClosePushL( groupIdArray );

    // Create a new settings instance
    CPsSettings* tempSettings = aSettings.CloneL();
    CleanupStack::PushL( tempSettings );

    TBool isGroupSearch = IsGroupSearchL(*tempSettings, groupIdArray);

    if (isGroupSearch)
        {
        // Replace groups URI with contacts DB URI in new search settings
        ReplaceGroupsUriL(*tempSettings);

        // List of contacts in this group	
        GetContactsInGroupL(groupIdArray[0], contactsInGroup);
        }

    // -----------------------------------------------------------------------


    // Extract query list. 
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);
    CleanupResetAndDestroyPushL( queryList );

    // (1)-------------------- No query return all contacts -------------------
    if (queryList.Count() == 0)
        {
        GetAllContentsL(*tempSettings, aSearchResults);

        if (isGroupSearch)
            {
            FilterSearchResultsForGroupsL(contactsInGroup, aSearchResults);
            }
        }
    // ------------------------------------------------------------------------

    // (2)-------------------- Perform a single query search ------------------
    else if (queryList.Count() == 1)
        {
        CPsQuery* query = queryList[0];

        // Search results
        iHelper->SearchSingleL(*tempSettings, *query, isGroupSearch,
                               contactsInGroup, aSearchResults, aSearchSeqs);
        }
    // ------------------------------------------------------------------------

    // (3)-------------------- Perform a multi query search -------------------
    else // multiple query
        {
        PRINT ( _L("Query received is in multiple. Performing a multi search.") );

        // Search results
        iMultiSearchHelper->SearchMultiL(*tempSettings, queryList, isGroupSearch, 
                                         contactsInGroup, aSearchResults, aSearchSeqs);
        }
    // -------------------------------------------------------------------------	

    // Cleanup
    
    CleanupStack::PopAndDestroy( &queryList ); // ResetAndDestroy
    CleanupStack::PopAndDestroy( tempSettings );
    CleanupStack::PopAndDestroy( &groupIdArray ); // Close
    CleanupStack::PopAndDestroy( &contactsInGroup ); // Close

	//__LATENCY_MARKEND ( _L("CPcsAlgorithm2::DoSearchL") );

    PRINT ( _L("End CPcsAlgorithm2::DoSearchL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::DoSearchInputL
// Search function helper
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::DoSearchInputL(CPsQuery& aQuery, 
                                    const TDesC& aData,
                                    RPointerArray<TDesC>& aMatchSet,
                                    RArray<TPsMatchLocation>& aMatchLocation)
    {

    //__LATENCY_MARK ( _L("CPcsAlgorithm2::SearchInputL: ") );
    PRINT ( _L("Enter CPcsAlgorithm2::DoSearchInputL") );

    // Check if any seperator is there in the query
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);
    CleanupResetAndDestroyPushL( queryList );

    // No query    
    if (queryList.Count() == 0)
        {
        PRINT ( _L("Query received is empty") );
        CleanupStack::PopAndDestroy( &queryList ); // ResetAndDestroy
        return;
        }

    // Single query
    if (queryList.Count() == 1)
        {
        iHelper->SearchMatchSeqL(aQuery, aData, aMatchSet, aMatchLocation);
        }

    if (queryList.Count() > 1) // multiple query
        {
        PRINT ( _L("Query received is in multiple. Performing a multi search.") );

        // Search results
        iMultiSearchHelper->SearchMatchSeqMultiL(queryList, 
                                                 aData, 
                                                 aMatchSet,
                                                 aMatchLocation);
        }

    // Delete all the query elements
    CleanupStack::PopAndDestroy( &queryList ); // ResetAndDestroy
    PRINT ( _L("End CPcsAlgorithm2::DoSearchInputL") );
    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2::SearchInputL") );

    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::AddData
// Add a data element to the pool
// ----------------------------------------------------------------------------    
void CPcsAlgorithm2::AddData(TDesC& aDataStore, CPsData* aData)
    {
    TInt arrayIndex = GetCacheIndex(aDataStore);

    if (arrayIndex < 0)
        return;

    CPcsCache* cache = iPcsCache[arrayIndex];

    // Fill the data store index
    TInt dataStoreIndex = FindStoreUri(aDataStore);
    if (dataStoreIndex >= 0)
        {
        aData->SetUriId(dataStoreIndex);
        }
    else
        {
        PRINT(_L("CPcsAlgorithm2::AddDataL Unknown data store"));
        return;
        }
    TRAPD(err, cache->AddToCacheL(*aData));

    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::RemoveData
// Remove a data element from the pool
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::RemoveData(TDesC &aDataStore, TInt aItemId)
    {
    TInt arrayIndex = GetCacheIndex(aDataStore);

    if (arrayIndex < 0)
        return;

    CPcsCache* cache = iPcsCache[arrayIndex];
    TRAPD(err, cache->RemoveFromCacheL(aItemId));

    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        }
    }

// ---------------------------------------------------------------------
// CPcsAlgorithm2::RemoveAll
// Remove all the contacts from a datastore
// ---------------------------------------------------------------------
void CPcsAlgorithm2::RemoveAll(TDesC& aDataStore)
    {
    TInt dataStoreIndex = GetCacheIndex(aDataStore);

    if (dataStoreIndex < 0)
        return;

    CPcsCache* cache = iPcsCache[dataStoreIndex];
    cache->RemoveAllFromCache();
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetCacheIndex
// Return the cache index for a data store
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2::GetCacheIndex(const TDesC& aDataStore)
    {
    const TInt pcsCacheCount = iPcsCache.Count();
    for (int i = 0; i <pcsCacheCount; i++)
        {
        CPcsCache* cache = iPcsCache[i];

        if (cache->GetURI().CompareC(aDataStore) == 0)
            return i;
        }

    return -1;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::AddDataStore
// Adds a new store
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::AddDataStore(TDesC& aDataStore)
    {
    // Check if the datastore cache already exists
    TInt index = GetCacheIndex(aDataStore);
    if (index != -1)
        {
        // Already exists
        return;
        }

    // Create a new cache    
    CPcsCache* cache = NULL;
    TRAPD(err, cache = CPcsCache::NewL( this, aDataStore, *iKeyMap, iCacheCount));
    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        return;
        }

    // Increment the cachecount
    iCacheCount++;

    RArray<TInt> dataFields;
    TRAP(err, iPsDataPluginInterface->GetSupportedDataFieldsL(cache->GetURI(), dataFields));
    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        return;
        }
    cache->SetDataFields(dataFields);

    // Check if sort order is persisted already    
    RArray<TInt> sortOrder;
    TRAP(err, ReadSortOrderFromCenRepL(*(cache->GetUri()), sortOrder));
    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        return;
        }

    if (sortOrder.Count() == 0)
        {
        cache->SetSortOrder(dataFields); // Initial sort order	
        }
    else
        {
        cache->SetSortOrder(sortOrder); // Persisted sort order
        }

    sortOrder.Close();
    dataFields.Close();

    iPcsCache.Append(cache);

    TRAP(err, iPsDataPluginInterface->RequestForDataL(aDataStore));
    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        UpdateCachingStatus(aDataStore, ECachingCompleteWithErrors);
        return;
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::RemoveDataStore
// Removes an existing data store
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::RemoveDataStore(TDesC& aDataStore)
    {

    for (int i = 0; i < iPcsCache.Count(); i++)
        {
        CPcsCache* cache = iPcsCache[i];

        if (cache->GetURI().CompareC(aDataStore) == 0)
            {
            delete iPcsCache[i];
            iPcsCache.Remove(i);
            iCacheCount--; 
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::IsLanguageSupportedL
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2::IsLanguageSupportedL(TUint32 aLang)
    {
    return iKeyMap->IsLanguageSupportedL(aLang);
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetUriForIdL
// Get the URI string for this internal id
// ----------------------------------------------------------------------------
const TDesC& CPcsAlgorithm2::GetUriForIdL(TUint8 aUriId)
    {
    TBool found = EFalse;
    TInt i = 0;
    const TInt pcsCacheCount = iPcsCache.Count();
    for (i = 0; i < pcsCacheCount; i++)
        {
        if (iPcsCache[i]->GetUriId() == aUriId)
            {
            found = ETrue;
            break;
            }
        }

    if (!found)
        {
        User::Leave(KErrNotFound);
        }

    return *(iPcsCache[i]->GetUri());
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::FindStoreUri
// Checks if this store exists
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2::FindStoreUri(const TDesC& aDataStore)
    {
    const TInt pcsCacheCount = iPcsCache.Count();
    for ( TInt i = 0; i < pcsCacheCount; i++ )
        {
        if ( aDataStore.CompareC(*(iPcsCache[i]->GetUri())) == 0 )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::UpdateCachingStatus
// Update caching status
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::UpdateCachingStatus(TDesC& aDataStore, TInt aStatus)
{
    PRINT ( _L("Enter CPcsAlgorithm2::UpdateCachingStatus") );
    PRINT2 ( _L("CPcsAlgorithm2::UpdateCachingStatus: Request received for URI=%S with status=%d"),
             &aDataStore, aStatus );

    // Handle data store update events
    if ( aStatus == ECacheUpdateContactRemoved ||
         aStatus == ECacheUpdateContactModified ||
         aStatus == ECacheUpdateContactAdded )
        {
        HandleCacheUpdated( static_cast<TCachingStatus>(aStatus) );
        return;
        }

    // If not a cache update event, then this event is related to the initial
    // cache construction.

    // Check if any error occurred and update the cache error
    if ( aStatus < 0 )
    {
        SetCachingError(aDataStore, aStatus);
    }
    else
    {
        TInt index = FindStoreUri(aDataStore);
        iPcsCache[index]->UpdateCacheStatus(aStatus);
    }

    TCachingStatus status = ECachingNotStarted;
    TUint countNotStarted = 0;
    TUint countInProgress = 0;
    TUint countCompleted = 0;
    TUint countCompletedWithErrors = 0;
    TInt cacheCount = iPcsCache.Count();
    for ( TInt i = 0; i < cacheCount; i++ )
    {
        PRINT3 ( _L("CPcsAlgorithm2::UpdateCachingStatus: URI[%d]=%S, cache status=%d"),
                 i, &iPcsCache[i]->GetURI(), iPcsCache[i]->GetCacheStatus() );

        switch ( iPcsCache[i]->GetCacheStatus() )
        {
            case ECachingNotStarted:         
            {
                countNotStarted++;          
                break;
            }
            case ECachingInProgress:         
            {
                countInProgress++;         
                break;
            }
            case ECachingComplete:           
            {
                countCompleted++;           
                break;
            }
            case ECachingCompleteWithErrors: 
            {
                countCompletedWithErrors++; 
                break;
            }
            default:                         
            { 
                // Default completed state
                countCompleted++;           
                break;
            }
        }
    }

    // Calculate cumulative status according to single caches statuses
    if ( countCompleted > 0 && ( countCompleted + countNotStarted ) == cacheCount )
    {
        // If at least one caching is finished
        // set status to ECachingComplete or ECachingCompleteWithErrors
        // according to iCacheError
        status = ( iCacheError == KErrNone ) ? ECachingComplete : ECachingCompleteWithErrors;
    }
    else if ( countInProgress > 0 )
    {
        // Else if at least one caching is in progress,
        // set status to ECachingInProgress
        status = ECachingInProgress;
    }
    else if ( countCompletedWithErrors > 0 )
    {
        // Else if at least one caching is completed with errors, 
        //set status to ECachingCompleteWithErrors
        status = ECachingCompleteWithErrors;
    }
    else
    {
        // countNotStarted == cacheCount
        // status is set to default ECachingNotStarted
    }

    PRINT1 ( _L("CPcsAlgorithm2::UpdateCachingStatus: Cumulative caching status is %d"),
             status );

    // Check if status changed
    if ( status != iCacheStatus )
    {
        PRINT2 ( _L("CPcsAlgorithm2::UpdateCachingStatus: Cumulative caching changed: %d -> %d"),
                 iCacheStatus, status );

        iCacheStatus = status;
        RProperty::Set(KPcsInternalUidCacheStatus, EPsKeyCacheStatus, iCacheStatus );
    }

    PRINT( _L("End CPcsAlgorithm2::UpdateCachingStatus") );
}


// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SetCachingError
// Updates cachinge error
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SetCachingError(const TDesC& aDataStore, TInt aError)
    {
    PRINT2 ( _L("SetCachingError::URI %S ERROR %d"), &aDataStore, aError );

    iCacheError = aError;
    RProperty::Set( KPcsInternalUidCacheStatus, EPsKeyCacheError, iCacheError );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetAllContentsL
// Returns all the contents of a store
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::GetAllContentsL(const CPsSettings& aSettings,
                                     RPointerArray<CPsData>& aResults)
    {
    //__LATENCY_MARK ( _L("CPcsAlgorithm2::GetAllContentsL") );

    PRINT ( _L("Enter CPcsAlgorithm2::GetAllContentsL") );

    // To hold array of results from different data stores
    typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;
    RPointerArray<CPSDATA_R_PTR_ARRAY> searchResultsArr;
    CleanupResetAndDestroyPushL( searchResultsArr );
    // TODO: Here's still a potential memory leak if a leave happens. The child
    // arrays of searchResultsArr are not Reset in that case. The CPsData objects
    // may leak as well. Handling this safely is somewhat complicated because some of
    // the CPsData objects may already be transferred to ownership of aResults array
    // at the time the leave happens, and those items must not be deleted.
    
    // Get the data stores
    RPointerArray<TDesC> dataStores;
    CleanupResetAndDestroyPushL( dataStores );
    aSettings.SearchUrisL(dataStores);

    // Get all contacts for each data store
    const TInt dataStoresCount = dataStores.Count(); 
    for (TInt dsIndex = 0; dsIndex < dataStoresCount; dsIndex++)
        {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        searchResultsArr.Append(temp);

        TInt arrayIndex = GetCacheIndex(*(dataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }

        CPcsCache* cache = GetCache(arrayIndex);

        cache->GetAllContentsL(*(searchResultsArr[dsIndex]));
        }

    CleanupStack::PopAndDestroy( &dataStores ); // ResetAndDestroy

    // Merge the results from different data stores
    CPcsAlgorithm2Utils::FormCompleteSearchResultsL(searchResultsArr, aResults);

    // Cleanup the local arrays
    const TInt seaerchResultsArrCount = searchResultsArr.Count(); 
    for (TInt i = 0; i < seaerchResultsArrCount; i++)
        {
        searchResultsArr[i]->Reset();
        }

    CleanupStack::PopAndDestroy( &searchResultsArr ); // ResetAndDestroy

    PRINT1 ( _L("Number of results = %d"), aResults.Count() );

    PRINT ( _L("End CPcsAlgorithm2::GetAllContentsL") );

    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2::GetAllContentsL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::IsGroupSearchL
// Checks if a group search is required
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2::IsGroupSearchL(CPsSettings& aSettings,
                                     RArray<TInt>& aGroupIdArray)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::IsGroupSearchL") );

    // Get the groupIds in the seach settings
    aSettings.GetGroupIdsL(aGroupIdArray);

    // Get the current URIs defined in settings    
    RPointerArray<TDesC> searchUris;
    CleanupResetAndDestroyPushL( searchUris );
    aSettings.SearchUrisL(searchUris);

    if (aGroupIdArray.Count() && (searchUris.Count() > aGroupIdArray.Count()))
        {
        // There is an error, either there are more than one groups
        // or the settings contain a combination of group/non-group Uris
        aGroupIdArray.Close();
        User::Leave(KErrArgument);
        }

    CleanupStack::PopAndDestroy( &searchUris ); // ResetAndDestroy

    PRINT ( _L("End CPcsAlgorithm2::IsGroupSearchL") );

    if (aGroupIdArray.Count() == 1)
        return ETrue;

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::ReplaceGroupsUriL
// Replace groups uri to contacts uri
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::ReplaceGroupsUriL(CPsSettings& aSettings)
    {
    RPointerArray<TDesC> uri;
    CleanupResetAndDestroyPushL( uri );

    // Set contacts db uri
    HBufC* cntdb = KVPbkDefaultCntDbURI().AllocLC();
    uri.AppendL(cntdb);
    CleanupStack::Pop( cntdb ); // ownership transferred
    aSettings.SetSearchUrisL(uri);

    // Cleanup
    CleanupStack::PopAndDestroy( &uri ); // ResetAndDestroy
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::FilterSearchResultsForGroupsL
// Filters the results that belong to a group
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::FilterSearchResultsForGroupsL(RArray<TInt>& contactsInGroup, 
                                                   RPointerArray<CPsData>& aSearchResults)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::FilterSearchResultsForGroupsL") );

    // for each search result
    // Note: aSearchResults.Count() is to be checked everytime,
    //       since the elements are being removed dynamically.
    for (TInt j = 0; j < aSearchResults.Count(); j++)
        {
        TBool includeResult = EFalse;

        if (contactsInGroup.Find(aSearchResults[j]->Id()) != KErrNotFound)
            {
            includeResult = ETrue;
            }

        if (includeResult == EFalse)
            {
            aSearchResults.Remove(j);
            j--; // j is decremented, since that object is removed
            }
        }

    PRINT ( _L("End CPcsAlgorithm2::FilterSearchResultsForGroupsL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetContactsInGroupL
// Recover contacts that belong to a group
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::GetContactsInGroupL(TInt aGroupId, RArray<TInt>& aGroupContactIds)
    {
    // Clear results array
    aGroupContactIds.Reset();
    
    // Cache Index for group database
    TInt cacheIndex = GetCacheIndex(KVPbkDefaultGrpDbURI);
    
    // Get the groups contact ids 
    if (cacheIndex != -1)
        {
        RPointerArray<CPsData> groups;
        CleanupClosePushL( groups );

        // Get all groups
        iPcsCache[cacheIndex]->GetAllContentsL(groups);

        // Get all contacts in group
        const TInt groupsCount = groups.Count(); 
        for (TInt i = 0; i < groupsCount; i++)
            {
            if (groups[i]->Id() == aGroupId)
                {
                groups[i]->IntDataExt(aGroupContactIds); // All contacts in group
                break;
                }
            }

        CleanupStack::PopAndDestroy( &groups ); // Close
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetDataOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::GetDataOrderL(TDesC& aURI, RArray<TInt>& aDataOrder)
    {
    PRINT ( _L("End CPcsAlgorithm2::GetDataOrderL") );

    TInt arrayIndex = -1;

    if (CPcsAlgorithm2Utils::IsGroupUri(aURI))
        {
        // If search in a group uri, use contacts db
        arrayIndex = GetCacheIndex(KVPbkDefaultCntDbURI);
        }
    else
        {
        arrayIndex = GetCacheIndex(aURI);
        }

    if (arrayIndex < 0)
        {
        return;
        }

    CPcsCache* cache = iPcsCache[arrayIndex];

    aDataOrder.Reset();

    // Get the data fields for this cache
    cache->GetDataFields(aDataOrder);

    PRINT ( _L("End CPcsAlgorithm2::GetDataOrderL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetSortOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::GetSortOrderL(TDesC& aURI, RArray<TInt>& aDataOrder)
    {
    PRINT ( _L("End CPcsAlgorithm2::GetSortOrderL") );

    TInt arrayIndex = -1;

    if (CPcsAlgorithm2Utils::IsGroupUri(aURI))
        {
        // If search in a group uri, use contacts db
        arrayIndex = GetCacheIndex(KVPbkDefaultCntDbURI);
        }
    else
        {
        arrayIndex = GetCacheIndex(aURI);
        }

    if (arrayIndex < 0)
        {
        return;
        }

    CPcsCache* cache = iPcsCache[arrayIndex];

    aDataOrder.Reset();

    // Get the data fields for this cache
    cache->GetSortOrder(aDataOrder);

    PRINT ( _L("End CPcsAlgorithm2::GetSortOrderL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::ChangeSortOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::ChangeSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::ChangeSortOrderL.") );

    PRINT ( _L("CPcsAlgorithm2::ChangeSortOrderL. Sort order change received.") );
    PRINT1 ( _L("URI = %S"), &aURI );

    // If URI is search in a group URI return       
    if (CPcsAlgorithm2Utils::IsGroupUri(aURI))
        {
        PRINT ( _L("CPcsAlgorithm2::ChangeSortOrderL. Sort order change not supported.") );
        return;
        }

    // Check if a cache exists
    TInt arrayIndex = GetCacheIndex(aURI);
    if (arrayIndex < 0)
        {
        PRINT ( _L("CPcsAlgorithm2::ChangeSortOrderL. Cache for URI doesn't exist.") );
        return;
        }

    // Cache instance for this URI
    CPcsCache* cache = iPcsCache[arrayIndex];

    // Check if received sort order is same as before
    RArray<TInt> mySortOrder;
    cache->GetSortOrder(mySortOrder);

    if (aSortOrder.Count() == mySortOrder.Count())
        {
        TBool same = ETrue;
        const TInt mySourtOrderCount = mySortOrder.Count(); 
        for ( TInt i = 0; i < mySourtOrderCount ; i++ )
            {
            if (mySortOrder[i] != aSortOrder[i])
                {
                same = EFalse;
                break;
                }
            }

        if (same)
            {
            PRINT ( _L("CPcsAlgorithm2::ChangeSortOrderL. Same sort order received. Ignoring ...") );
            PRINT ( _L("End CPcsAlgorithm2::ChangeSortOrderL.") );
            mySortOrder.Reset();
            return;
            }
        }

    mySortOrder.Reset();

    PRINT ( _L("CPcsAlgorithm2::ChangeSortOrderL. New sort order received. Refreshing ...") );

    // Set the new sort order on the cache
    cache->SetSortOrder(aSortOrder);

    // Persist the changes in sort order
    WriteSortOrderToCenRepL(aURI, aSortOrder);

    // Request for data again
    TInt err = KErrNone;
    TRAP(err, cache->ResortdataInPoolsL());
    if (err != KErrNone)
        {
        PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL() Set Caching Error ") );
        SetCachingError(aURI, err);
        UpdateCachingStatus(aURI, ECachingCompleteWithErrors);
        return;
        }

    PRINT ( _L("End CPcsAlgorithm2::ChangeSortOrderL.") );
    }

// ---------------------------------------------------------------------------------
// Read the persisted sort order from the central repository
// Persisted sort order is of form URI Field1 Field2 Field3 .. FieldN (space delimited)
// ---------------------------------------------------------------------------------
void CPcsAlgorithm2::ReadSortOrderFromCenRepL(const TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::ReadSortOrderFromCenRepL.") );

    aSortOrder.Reset();

    CRepository *repository = CRepository::NewL(KCRUidPSSortOrder);

    // Read the sort order from cenrep
    TBuf<KCRMaxLen> str;

    for ( TInt i(KCenrepFieldsStartKey); 
          i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
          i++ )
        {
        TInt err = repository->Get(i, str);

        if (KErrNone != err)
            {
            break;
            }

        if (str != KNullDesC)
            {
            TLex lex(str);

            // Extract the URI
            TPtrC token = lex.NextToken();

            if (aURI.Compare(token) == 0)
                {
                // Extract the sort order
                token.Set(lex.NextToken());

                while (token.Length() != 0)
                    {
                    TLex lex1(token);

                    TInt intVal;
                    TInt err = lex1.Val(intVal);

                    if (KErrNone == err)
                        {
                        aSortOrder.Append(intVal);
                        }

                    // Next token
                    token.Set(lex.NextToken());
                    }

                break;
                }
            }

        }

    delete repository;

    PRINT ( _L("End CPcsAlgorithm2::ReadSortOrderFromCenRepL.") );
    }

// ---------------------------------------------------------------------------------
// Write the sort order into the central repository
// Persisted sort order is of form URI Field1 Field2 Field3 .. FieldN (space delimited)
// ---------------------------------------------------------------------------------
void CPcsAlgorithm2::WriteSortOrderToCenRepL(const TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::WriteSortOrderToCenRepL.") );

    CRepository* repository = CRepository::NewLC(KCRUidPSSortOrder);

    // Check if there an entry for this URI in cenrep
    TBuf<KCRMaxLen> str;
    TInt keyIndex = -1;

    for ( TInt i(KCenrepFieldsStartKey); 
          i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
          i++ )
        {
        TInt err = repository->Get(i, str);

        if (KErrNone != err)
            {
            PRINT ( _L("CPcsAlgorithm2::WriteSortOrderToCenRepL. cenrep error.") );
            return;
            }

        if (str != KNullDesC)
            {
            TLex lex(str);

            // Extract the URI
            TPtrC token = lex.NextToken();

            if (aURI.Compare(token) == 0)
                {
                keyIndex = i; // i has the key index for this URI
                break;
                }
            }
        }

    // No entry for this URI in cenrep
    // Find the next free location in cenrep
    if (keyIndex == -1)
        {
        // Find the next free key index
        for ( TInt i(KCenrepFieldsStartKey); 
              i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
              i++ )
            {
            TInt err = repository->Get(i, str);

            if (KErrNone != err)
                {
                PRINT ( _L("CPcsAlgorithm2::WriteSortOrderToCenRepL. cenrep error.") );
                return;
                }

            if (str == KNullDesC)
                {
                keyIndex = i; // i has the next free location
                break;
                }
            }
        }

    if (keyIndex == -1)
        {
        PRINT ( _L("CPcsAlgorithm2::WriteSortOrderToCenRepL. Persist limit violated.") );
        return;
        }

    // Persist the sort order
    HBufC* str1 = HBufC::NewLC(KCRMaxLen);
    TPtr ptr(str1->Des());

    // Append the URI
    ptr.Append(aURI);
    ptr.Append(KSpace);

    // Append the sort order fields
    const TInt sortOrderCount =  aSortOrder.Count();
    for (TInt j = 0; j < sortOrderCount; j++)
        {
        ptr.AppendNum(aSortOrder[j]);
        ptr.Append(KSpace);
        }

    // Write to persistent store
    TInt err = repository->Set(keyIndex, ptr);

    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy( str1 );

    CleanupStack::PopAndDestroy( repository );

    PRINT ( _L("End CPcsAlgorithm2::WriteSortOrderToCenRepL.") );
    }

// ---------------------------------------------------------------------------------
// WriteClientDataL.
// Write the content required by client
// ---------------------------------------------------------------------------------
CPsClientData* CPcsAlgorithm2::WriteClientDataL(CPsData& aPsData)
    {
    CPsClientData* clientData = CPsClientData::NewL();
    CleanupStack::PushL(clientData);

    // set Id
    clientData->SetId(aPsData.Id());

    // set Uri
    clientData->SetUriL(GetUriForIdL(aPsData.UriId()));

    // set pointer to the each data element
    const TInt dataElementCount = aPsData.DataElementCount(); 
    for (TInt i = 0; i <dataElementCount; i++)
        {
        clientData->SetDataL(i, *(aPsData.Data(i)));
        }

    // set data extension
    clientData->SetDataExtensionL(aPsData.DataExtension());

    // Set the Field match
    clientData->SetFieldMatch(aPsData.DataMatch());
    CleanupStack::Pop(clientData);

    return clientData;
    }

// ---------------------------------------------------------------------------------
// HandleCacheUpdated.
// ---------------------------------------------------------------------------------
void CPcsAlgorithm2::HandleCacheUpdated( TCachingStatus aStatus )
    {
    TInt psKey( KErrNotFound );
    
    switch ( aStatus )
        {
        case ECacheUpdateContactRemoved:
            psKey = EPsKeyContactRemovedCounter;
            break;
            
        case ECacheUpdateContactModified:
            psKey = EPsKeyContactModifiedCounter;
            break;
            
        case ECacheUpdateContactAdded:
            psKey = EPsKeyContactAddedCounter;
            break;
            
        default:
            break;
        }
    
    // Increment the relevant counter in P&S by one to signal the clients about
    // the cache update.
    if( psKey != KErrNotFound )
        {
        TInt counter( KErrNotFound );
        TInt err = RProperty::Get( KPcsInternalUidCacheStatus, psKey, counter );
        if ( !err )
            {
            counter++;
            RProperty::Set( KPcsInternalUidCacheStatus, psKey, counter );
            }
        }
    }

// ---------------------------------------------------------------------------------
// ReconstructCacheDataL.
// ---------------------------------------------------------------------------------
void CPcsAlgorithm2::ReconstructCacheDataL()
    {
    PRINT ( _L("Enter CPcsAlgorithm2::ReconstructCacheDataL.") );

    TInt err;
    TRAP( err, iKeyMap->ReconstructKeymapL());
    if (err != KErrNone)
        {    
        PRINT1 ( _L("keyMap ReconstructKeymapL, err =%d"),err );
        }
    
    for (TInt index = 0; index < iCacheCount; index++)
        {
        CPcsCache* cache = iPcsCache[index];

        HBufC* uri = cache->GetUri();
        // Clear the cache
        cache->RemoveAllFromCache();

        if (err != KErrNone)
            {
            SetCachingError(*uri, err);
            }
        //Update the caching status as ECachingInProgress, since now the caching
        // would be started again
        UpdateCachingStatus(*uri, ECachingInProgress);

        // Request for data again
        TRAP(err, iPsDataPluginInterface->RequestForDataL(*uri));
        PRINT1 ( _L("iPsDataPluginInterface->RequestForDataL, err =%d"),err );

        if (err != KErrNone)
            {
            SetCachingError(*uri, err);
            }
        }
    }

// ---------------------------------------------------------------------------------
// DoLaunchPluginsL.
// launch plugins by idle
// ---------------------------------------------------------------------------------
TInt CPcsAlgorithm2::DoLaunchPluginsL(TAny* aPtr)
    {
    CPcsAlgorithm2* ptr = (CPcsAlgorithm2*) aPtr;
    ptr->DoLaunchPluginsL();
    return EFalse;
    }

// ---------------------------------------------------------------------------------
// DoLaunchPluginsL.
// lauch plugins
// ---------------------------------------------------------------------------------
void CPcsAlgorithm2::DoLaunchPluginsL()
    {
    // Initialize available data adapters
    iPsDataPluginInterface = CPsDataPluginInterface::NewL(this, this);
    iPsDataPluginInterface->InstantiateAllPlugInsL();

    // Store the cache list in TLS
    // Required to support sort order changes in a memory efficient way
    // This avoids storing sort order information in the CPsData element
    // and storing it in CPcsCache. Refer CPcsAlgorithm2Utils::CompareDataBySortOrder
    // to see how this is being used.
    User::LeaveIfError(Dll::SetTls(&iPcsCache));

    // Initialize cache
    RPointerArray<TDesC> dataStores;
    CleanupClosePushL( dataStores );

    iPsDataPluginInterface->GetAllSupportedDataStoresL(dataStores);

    const TInt dataStoresCount = dataStores.Count();
    for (TInt dIndex = 0; dIndex < dataStoresCount; dIndex++)
        {
        AddDataStore(*(dataStores[dIndex]));
        }

    CleanupStack::PopAndDestroy( &dataStores ); // Close
    }

/**
* Returns the Adaptive Grid for one or more URI
* 
*/
 void CPcsAlgorithm2::GetAdaptiveGridL( const MDesCArray& /*aURIs*/,
                               const TBool /*aCompanyName*/,
                               TDes& /*aAdaptiveGrid*/ )
    {
     PRINT ( _L("Enter CPcsAlgorithm2::GetAdaptiveGridL") );


     PRINT ( _L("End CPcsAlgorithm2::GetAdaptiveGridL") );

    }
// End of file

