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
    iFindUtil = CFindUtil::NewL();
    
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
    TRAP( keyMapErr, iKeyMap = CPcsKeyMap::NewL( this ));
    if (keyMapErr != KErrNone)
        {
        PRINT ( _L("**********************************************."));
        PRINT1 ( _L("CPcsAlgorithm2::ConstructL() KeyMap construction error. The keymap crashed with error code %d."), keyMapErr );
        PRINT ( _L("Please check the keypad/language for which keymap got crashed.") );
        PRINT ( _L("**********************************************."));
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
    delete iFindUtil;
    
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
// CPcsAlgorithm2::ReplaceZeroWithSpaceL
// Replace first occurance of '0' in a sequence of '0's in ITU-T with space
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2::ReplaceZeroWithSpaceL(CPsQuery& aQuery)
    {
    TChar space(KSpace); // ascii value for space

    TBool queryModified = EFalse;

    // Skip initial zeros in query
    TInt index = 0;
    for (index = 0; index < aQuery.Count()
            && aQuery.GetItemAtL(index).Character().GetNumericValue() == 0; ++index)
        {
        }

    if (aQuery.KeyboardModeL() != EQwerty)
        {
        for (TInt beg = index; beg < aQuery.Count(); ++beg)
            {
            CPsQueryItem& item = aQuery.GetItemAtL(beg);

            if (item.Character().GetNumericValue() == 0 && item.Mode()== EItut)
                {
                if (beg != 0)
                    {
                    CPsQueryItem& item1 = aQuery.GetItemAtL(beg - 1);
                    if (item1.Character().GetNumericValue() != 0
                            && !item1.Character().IsSpace())
                        {
                        item.SetCharacter(space);
                        queryModified = ETrue;
                        }
                    }
                else
                    {
                    item.SetCharacter(space);
                    queryModified = ETrue;
                    }
                }
            }
        }

    return queryModified;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::PerformSearchL
// Search function for cache
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::PerformSearchL(const CPsSettings& aSettings, CPsQuery& aQuery, 
                                    RPointerArray<CPsClientData>& aSearchResults,
                                    RPointerArray<CPsPattern>& aSearchSeqs)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::PerformSearchL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm2::PerformSearchL") );

    // Get the current language
    TLanguage lang = User::Language();

    // Check if the language is supported and the keyboard mode is not qwerty.

    // Local arrays to hold the search results 
    RPointerArray<CPsData> tempSearchResults;
    RPointerArray<CPsData> tempSearchResults1;

    // -------------------- Perform the basic search --------------------------

    DoSearchL(aSettings, aQuery, tempSearchResults, aSearchSeqs);

    // ------------------------------------------------------------------------

    // ------------------- Perform advanced search if needed ------------------
    // Substitute "0" with space
    TBool queryModified = ReplaceZeroWithSpaceL(aQuery);

    // If query got modified and the search query translated to more than 1 query
    // perform a multi search again
    if (queryModified)
        {
        // Split query
        RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);

        //  Perform seach again
        if (queryList.Count() > 1)
            {
            DoSearchL(aSettings, aQuery, tempSearchResults1, aSearchSeqs);
            }

        // Sort rule        
        TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrder);

        // Avoid duplicates and add new results
        TIdentityRelation<CPsData> identityRule(CPsData::CompareById);
        if (aSettings.GetSortType() != EAlphabetical)
            {
            TInt insertPos = 0;
            for (int i = 0; i < tempSearchResults1.Count(); i++)
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

            for (int i = 0; i < tempSearchResults1.Count(); i++)
                {
                if (tempSearchResults.Find(tempSearchResults1[i],
                                           identityRule) == KErrNotFound)
                    {
                    tempSearchResults.InsertInOrderAllowRepeats(tempSearchResults1[i], 
                                                                rule);
                    }
                }
            }

        queryList.ResetAndDestroy();
        }
    // ------------------------------------------------------------------------

    // ---------------------- Write result objects to the stream --------------
    // Truncate the result set if required
    TInt numToDisplay = aSettings.MaxResults();
    TInt resultSet = tempSearchResults.Count();

    if (resultSet > numToDisplay && numToDisplay != -1)
        {
        // Copy the top N contents from tempSearchResults to the results stream
        for (int i = 0; i < numToDisplay; i++)
            {
            aSearchResults.Append(WriteClientDataL(*(tempSearchResults[i])));
            }

        }
    else
        {
        // Copy all the contents from tempSearchResults to the results stream
        for (int i = 0; i < resultSet; i++)
            {
            aSearchResults.Append(WriteClientDataL(*(tempSearchResults[i])));
            }
        }
    // ------------------------------------------------------------------------

    // Cleanup local results array
    tempSearchResults.Reset(); // Don't destroy
    tempSearchResults1.Reset(); // Don't destroy

    __LATENCY_MARKEND ( _L("CPcsAlgorithm2::PerformSearchL") );

    PRINT ( _L("End CPcsAlgorithm2::PerformSearchL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SearchInputL
// Search function for input string
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SearchInputL(CPsQuery& aQuery, TDesC& aData,
                                  RPointerArray<TDesC>& aMatchSet,
                                  RArray<TPsMatchLocation>& aMatchLocation)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::SearchInputL") );

    // Get the current language
    TLanguage lang = User::Language();

    // Check if the language is supported and the keyboard mode is not qwerty.

    // Print input query for debug
    aQuery.PrintQuery();

    // Print received search data
    PRINT1 ( _L("Search data received = %S"), &aData);

    // -------------------- Perform the basic search --------------------------

    DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);

    // ------------------------------------------------------------------------   

    // ------------------- Perform advanced search if needed ------------------
    // Substitute "0" with space
    TBool queryModified = ReplaceZeroWithSpaceL(aQuery);

    // If query got modified and the search query translated to more than 1 query
    // perform a multi search again
    if (queryModified)
        {
        RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);

        if (queryList.Count() > 1)
            {
            DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);
            }

        queryList.ResetAndDestroy();
        }
    // ------------------------------------------------------------------------

    // Sort match set
    iHelper->SortSearchSeqsL(aMatchSet);

    PRINT ( _L("End CPcsAlgorithm2::SearchInputL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SearchMatchStringL
// Search function for input string, result also as string
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SearchMatchStringL( CPsQuery& /*aSearchQuery*/,
                            TDesC& /*aSearchData*/,
                            TDes& /*aMatch*/ )
    {
    //NOT IMPLEMENTED YET
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::DoSearchL
// Search function helper
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::DoSearchL(const CPsSettings& aSettings, CPsQuery& aQuery, 
                               RPointerArray<CPsData>& searchResults,
                               RPointerArray<CPsPattern>& searchSeqs)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::DoSearchL") );

    // Print query for debug
    aQuery.PrintQuery();

    // -(0)----------------- Check if group search is required ---------------    
    RArray<TInt> contactsInGroup;
    RArray<TInt> groupIdArray;

    // Create a new settings instance
    CPsSettings *tempSettings = aSettings.CloneL();

    TBool isGroupSearch = IsGroupSearchL(*tempSettings, groupIdArray);

    if (isGroupSearch)
        {
        // Replace groups URI with contacts DB URI in new search settings
        ReplaceGroupsUriL(*tempSettings);

        // List of contacts in this group	
        GetContactsInGroupL(groupIdArray[0], contactsInGroup);
        }

    groupIdArray.Close();

    // -----------------------------------------------------------------------


    // Extract query list. 
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);

    // -(1)--------------------- No query return all contacts ----------------    
    if (queryList.Count() == 0)
        {
        GetAllContentsL(*tempSettings, searchResults);

        if (isGroupSearch)
            {
            FilterSearchResultsForGroupsL(contactsInGroup, searchResults);
            }
        }
    // ------------------------------------------------------------------------


    // -(2)------------------------ Perform a single query search -------------
    if (queryList.Count() == 1)
        {
        TInt mode = aQuery.KeyboardModeL();

        CPsQuery* query = queryList[0];

        iFindUtilECE->SetKeyboardMode(mode);

        switch (mode)
            {
            case EItut:

                PRINT ( _L("Query received is in ITU-T mode") );

                // Search results
                iHelper->SearchITUL(*tempSettings, *query, isGroupSearch,
                                    contactsInGroup, searchResults, searchSeqs);

                break;

            case EQwerty:

                PRINT ( _L("Query received is in QWERTY mode") );

                // Search results
                iHelper->SearchQWERTYL(*tempSettings, *query, isGroupSearch,
                                       contactsInGroup, searchResults, searchSeqs);

                break;

            case EModeUndefined:

                PRINT ( _L("Query received is in Mixed mode. Keyboard swap happened.") );

                // Search results
                iHelper->SearchMixedL(*tempSettings, *query, isGroupSearch,
                                      contactsInGroup, searchResults, searchSeqs);

                break;
            }

        }
    // ------------------------------------------------------------------------


    // -(3)---------------------------- Perform a multi query search ----------
    if (queryList.Count() > 1) // multiple query
        {
        PRINT ( _L("Query received is in multiple. Performing a multi search.") );

        TInt mode = aQuery.KeyboardModeL();
        iFindUtilECE->SetKeyboardMode(mode);

        for (int i = 0; i < queryList.Count(); i++)
            {
            TPtrC queryPtr = queryList[i]->QueryAsStringLC();
            PRINT2 ( _L("Received Query, index = %d; value = %S"), i, &queryPtr );
            CleanupStack::PopAndDestroy();
            }

        // Search results
        iMultiSearchHelper->SearchMultiL(*tempSettings, queryList, isGroupSearch, 
                                         contactsInGroup, searchResults, searchSeqs, 
                                         mode);
        }
    // -------------------------------------------------------------------------	

    // Cleanup
    delete tempSettings;
    tempSettings = NULL;

    groupIdArray.Close();
    contactsInGroup.Close();
    queryList.ResetAndDestroy();

    PRINT ( _L("End CPcsAlgorithm2::DoSearchL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::DoSearchInputL
// Search function helper
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::DoSearchInputL(CPsQuery& aQuery, TDesC& aData,
                                    RPointerArray<TDesC>& aMatchSet,
                                    RArray<TPsMatchLocation>& aMatchLocation)
    {

    PRINT ( _L("Enter CPcsAlgorithm2::DoSearchInputL") );

    // Check if any seperator is there in the query
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);

    // No query    
    if (queryList.Count() == 0)
        {
        PRINT ( _L("Query received is empty") );
        return;
        }

    RPointerArray<HBufC> convertedQuery;
    iMultiSearchHelper->ConvertQueryToListL(queryList, convertedQuery);

    // Single query
    if (queryList.Count() == 1)
        {
        iHelper->SearchMatchSeqL(convertedQuery[0], aData, aMatchSet, aQuery,
                                 aMatchLocation);

        }

    if (queryList.Count() > 1) // multiple query
        {
        PRINT ( _L("Query received is in multiple. Performing a multi search.") );

        for (int i = 0; i < queryList.Count(); i++)
            {
            TPtrC queryPtr = queryList[i]->QueryAsStringLC();
            PRINT2 ( _L("Rceived Query, index = %d; value = %S"), i, &queryPtr );
            CleanupStack::PopAndDestroy();
            }

        // Search results
        iMultiSearchHelper->SearchMatchSeqMultiL(queryList, aData, aMatchSet,
                                                 aMatchLocation);

        }

    // Delete all the query elements
    queryList.ResetAndDestroy();
    convertedQuery.ResetAndDestroy();

    PRINT ( _L("End CPcsAlgorithm2::DoSearchInputL") );
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
    TRAPD(err, cache->RemoveAllFromCacheL());

    if (err != KErrNone)
        {
        SetCachingError(aDataStore, err);
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::GetCacheIndex
// Return the cache index for a data store
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2::GetCacheIndex(TDesC& aDataStore)
    {
    for (int i = 0; i < iPcsCache.Count(); i++)
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
TDesC& CPcsAlgorithm2::GetUriForIdL(TUint8 aUriId)
    {
    TBool found = EFalse;
    TInt i = 0;

    for (i = 0; i < iPcsCache.Count(); i++)
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
TInt CPcsAlgorithm2::FindStoreUri(TDesC& aDataStore)
    {
    for (int i = 0; i < iPcsCache.Count(); i++)
        {
        if (aDataStore.CompareC(*(iPcsCache[i]->GetUri())) == 0)
            {
            return i;
            }
        }

    return -1;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::UpdateCachingStatus
// Update caching status
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::UpdateCachingStatus(TDesC& aDataStore, TInt aStatus)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::UpdateCachingStatus") );

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
    TInt index = FindStoreUri(aDataStore);
    iPcsCache[index]->UpdateCacheStatus(aStatus);

    // Check if any error occurred
    // If so, update the cache status, Set the property and return
    if (aStatus < 0)
        {
        SetCachingError(aDataStore, aStatus);
        //return;
        }

    //store the index for firstname and lastname
    if (aStatus == ECachingComplete)
        {
        RArray<TInt> dataFields;
        iPcsCache[index]->GetDataFields(dataFields);

        for (int i = 0; i < dataFields.Count(); i++)
            {
            if (dataFields[i] == R_VPBK_FIELD_TYPE_FIRSTNAME)
                {
                iFirstNameIndex = i;
                }
            else if (dataFields[i] == R_VPBK_FIELD_TYPE_LASTNAME)
                {
                iLastNameIndex = i;
                }
            }
        }

    // No error occurred
    TCachingStatus status = ECachingComplete;
    TBool atLeastOneStoreCachingCompleteWithErrors(EFalse);
    for (TInt i = 0; i < iPcsCache.Count(); i++)
        {
        if (iPcsCache[i]->GetCacheStatus() == ECachingComplete)
            {
            continue;
            }
        else if (iPcsCache[i]->GetCacheStatus() == ECachingCompleteWithErrors)
            {
            atLeastOneStoreCachingCompleteWithErrors = ETrue;
            continue;
            }
        else
            {
            status = ECachingInProgress;
            break;
            }
        }

    if (status == ECachingComplete)
        {
        // See if any error occurred while caching
        // If so, change the status to ECachingCompleteWithErrors
        if ((iCacheError != KErrNone) || (atLeastOneStoreCachingCompleteWithErrors))
            {
            status = ECachingCompleteWithErrors;
            }
        }

    // Check if status changed
    if (status != iCacheStatus)
        {
        iCacheStatus = status;
        RProperty::Set(KPcsInternalUidCacheStatus, EPsKeyCacheStatus, iCacheStatus );
        }

    PRINT ( _L("End CPcsAlgorithm2::UpdateCachingStatus") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2::SetCachingError
// Updates cachinge error
// ----------------------------------------------------------------------------
void CPcsAlgorithm2::SetCachingError(TDesC& aDataStore, TInt aError)
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
    __LATENCY_MARK ( _L("CPcsAlgorithm2::GetAllContentsL") );

    PRINT ( _L("Enter CPcsAlgorithm2::GetAllContentsL") );

    // Get the data stores
    RPointerArray<TDesC> aDataStores;
    aSettings.SearchUrisL(aDataStores);

    // To hold array of results from different data stores
    typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;
    RPointerArray<CPSDATA_R_PTR_ARRAY> iSearchResultsArr;

    // Get all contacts for each data store
    for (int dsIndex = 0; dsIndex < aDataStores.Count(); dsIndex++)
        {
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData> ();
        iSearchResultsArr.Append(temp);

        TInt arrayIndex = GetCacheIndex(*(aDataStores[dsIndex]));
        if (arrayIndex < 0)
            {
            continue;
            }

        CPcsCache* cache = GetCache(arrayIndex);

        cache->GetAllContentsL(*(iSearchResultsArr[dsIndex]));
        }

    aDataStores.ResetAndDestroy();

    // Merge the results from different data stores
    CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iSearchResultsArr, aResults);

    // Cleanup the local arrays
    for (TInt i = 0; i < iSearchResultsArr.Count(); i++)
        {
        iSearchResultsArr[i]->Reset();
        delete iSearchResultsArr[i];
        iSearchResultsArr[i] = NULL;
        }

    iSearchResultsArr.Reset();

    PRINT1 ( _L("Number of results = %d"), aResults.Count() );

    PRINT ( _L("End CPcsAlgorithm2::GetAllContentsL") );

    __LATENCY_MARKEND ( _L("CPcsAlgorithm2::GetAllContentsL") );
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
    aSettings.SearchUrisL(searchUris);

    if (aGroupIdArray.Count() && (searchUris.Count() > aGroupIdArray.Count()))
        {
        // There is an error, either there are more than one groups
        // or the settings contain a combination of group/non-group Uris
        searchUris.ResetAndDestroy();
        aGroupIdArray.Close();
        User::Leave(KErrArgument);
        }

    searchUris.ResetAndDestroy();

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

    // Set contacts db uri
    HBufC* cntdb = HBufC::NewL(KBufferMaxLen);
    cntdb->Des().Copy(KVPbkDefaultCntDbURI);
    uri.Append(cntdb);
    aSettings.SetSearchUrisL(uri);

    // Cleanup
    uri.ResetAndDestroy();
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

    // Groups URI
    HBufC* groupURI = HBufC::NewL(50);
    groupURI->Des().Copy(KVPbkDefaultGrpDbURI);

    // Cache Index   
    TInt cacheIndex = GetCacheIndex(*groupURI);

    // Cleanup
    delete groupURI;
    groupURI = NULL;

    // Get the groups contact ids 
    if (cacheIndex != -1)
        {
        RPointerArray<CPsData> groups;

        // Get all groups
        iPcsCache[cacheIndex]->GetAllContentsL(groups);

        // Get all contacts in group
        for (TInt i = 0; i < groups.Count(); i++)
            {
            if (groups[i]->Id() == aGroupId)
                {
                groups[i]->IntDataExt(aGroupContactIds); // All contacts in group
                break;
                }
            }

        groups.Reset();
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
        TBuf<255> cntdb(KVPbkDefaultCntDbURI);
        arrayIndex = GetCacheIndex(cntdb);
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
        TBuf<255> cntdb(KVPbkDefaultCntDbURI);
        arrayIndex = GetCacheIndex(cntdb);
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
        for (int i = 0; i < mySortOrder.Count(); i++)
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
void CPcsAlgorithm2::ReadSortOrderFromCenRepL(TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::ReadSortOrderFromCenRepL.") );

    aSortOrder.Reset();

    CRepository *repository = CRepository::NewL(KCRUidPSSortOrder);

    // Read the sort order from cenrep
    TBuf<KCRMaxLen> str;

    for (TInt i(KCenrepFieldsStartKey); i < KCenrepFieldsStartKey
            + KCenrepNumberOfFieldsCount; i++)
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
void CPcsAlgorithm2::WriteSortOrderToCenRepL(TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::WriteSortOrderToCenRepL.") );

    CRepository *repository = CRepository::NewL(KCRUidPSSortOrder);

    // Check if there an entry for this URI in cenrep
    TBuf<KCRMaxLen> str;
    TInt keyIndex = -1;

    for (TInt i(KCenrepFieldsStartKey); i < KCenrepFieldsStartKey
            + KCenrepNumberOfFieldsCount; i++)
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
        for (TInt i(KCenrepFieldsStartKey); i < KCenrepFieldsStartKey
                + KCenrepNumberOfFieldsCount; i++)
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
    HBufC* str1 = HBufC::NewL(KCRMaxLen);
    TPtr ptr(str1->Des());

    // Append the URI
    ptr.Append(aURI);
    ptr.Append(KSpace);

    // Append the sort order fields
    for (int j = 0; j < aSortOrder.Count(); j++)
        {
        ptr.AppendNum(aSortOrder[j]);
        ptr.Append(KSpace);
        }

    // Write to persistent store
    TInt err = repository->Set(keyIndex, ptr);

    User::LeaveIfError(err);

    delete str1;

    delete repository;

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
    for (TInt i = 0; i < aPsData.DataElementCount(); i++)
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

    if ( psKey != KErrNotFound )
        {
        // Increment the relevant counter in P&S by one to signal the clients about
        // the cache update.
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
    
    for (int index = 0; index < iCacheCount; index++)
        {
        CPcsCache* cache = iPcsCache[index];

        HBufC * uri = cache->GetUri();
        // Clear the cache
        TRAP(err, cache->RemoveAllFromCacheL());   
        PRINT1 ( _L("cache RemoveAllFromCacheL, err =%d"),err );

        if (err != KErrNone)
            {
            SetCachingError(*uri, err);
            }
        //Update the caching status as ECachingInProgress, since now the caching
        // would be strated again
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

void CPcsAlgorithm2::Converter(const TDesC& aSourStr, TDes& aDestStr)
    {
    if (iKeyMap)
        {
        iKeyMap->GetNumericKeyString(aSourStr, aDestStr);
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

    iPsDataPluginInterface->GetAllSupportedDataStoresL(dataStores);

    for (int dIndex = 0; dIndex < dataStores.Count(); dIndex++)
        {
        AddDataStore(*(dataStores[dIndex]));
        }

    dataStores.Reset();
    }
// End of file

