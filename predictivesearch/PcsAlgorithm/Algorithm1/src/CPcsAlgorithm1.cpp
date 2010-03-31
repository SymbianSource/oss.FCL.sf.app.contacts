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
* Description:  Predictive Contact Search Algorithm 1 main class
*
*/


// INCLUDES
#include "CPcsAlgorithm1.h"
#include "CPcsAlgorithm1Helper.h"
#include "CPcsAlgorithm1MultiSearchHelper.h"
#include "CPcsAlgorithm1Utils.h"
#include "CPcsDebug.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsData.h"
#include "CWords.h"
#include "CPsQuery.h"
#include "CPsDataPluginInterface.h"
#include "CPcsDefs.h"

const TText KSpace = ' ';

// UID used for Publish and Subscribe mechanism
// This should be same as the one defined in CPsPropertyHandler.cpp
const TUid KCStatus = {0x2000B5B6};

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1* CPcsAlgorithm1::NewL()
{
    PRINT ( _L("Enter CPcsAlgorithm1::NewL") );
    
	CPcsAlgorithm1* self = new ( ELeave ) CPcsAlgorithm1();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

    PRINT ( _L("End CPcsAlgorithm1::NewL") );
    
	return self;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::CPcsAlgorithm1
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm1::CPcsAlgorithm1()
{		
    PRINT ( _L("Enter CPcsAlgorithm1::CPcsAlgorithm1") );
    PRINT ( _L("End CPcsAlgorithm1::CPcsAlgorithm1") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::ConstructL()
{
    PRINT ( _L("Enter CPcsAlgorithm1::ConstructL") );
    
    iCacheStatus = ECachingNotStarted;  // Starting status
    iCacheError = KErrNone;             // No error
        
    iPluginLauncher = CIdle::NewL( CActive::EPriorityStandard );
    
    // Define cache status property used to inform clients about the caching status.
    TInt err = RProperty::Define(KCStatus,0, RProperty::EInt);      
    if ( err != KErrAlreadyExists )
    {
    	User::LeaveIfError(err);
    }
    
    // Define cache error property used to inform client about the errors.
    err = RProperty::Define(KCStatus,1, RProperty::EInt);
    if ( err != KErrAlreadyExists )
    {
    	User::LeaveIfError(err);
    }
    
    // Initialize key map and pti engine
    //iKeyMap = CPcsKeyMap::NewL();
    TInt keyMapErr = KErrNone;
    TRAP( keyMapErr, iKeyMap = CPcsKeyMap::NewL());
    if ( keyMapErr != KErrNone )
    {
        PRINT ( _L("**********************************************."));
        PRINT1( _L("CPcsAlgorithm1::ConstructL() KeyMap construction error. The keymap crashed with error code %d."),keyMapErr );
        PRINT ( _L("Please check the keypad/language for which keymap got crashed.") );
        PRINT ( _L("**********************************************."));
        User::Leave( keyMapErr ); // we can't go on without a key map; constructing cache needs it
    }
    
    // Initialize helpers
    iHelper = CPcsAlgorithm1Helper::NewL(this);
    iMultiSearchHelper = CPcsAlgorithm1MultiSearchHelper::NewL(this);
    
    if(!iPluginLauncher->IsActive())
        {
        iPluginLauncher->Start(TCallBack(CPcsAlgorithm1::DoLaunchPluginsL, this));
        }
     
    PRINT ( _L("End CPcsAlgorithm1::ConstructL") );
} 
	
	
// ----------------------------------------------------------------------------
// CPcsAlgorithm1::~CPcsAlgorithm1
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm1::~CPcsAlgorithm1()
{
    PRINT ( _L("Enter CPcsAlgorithm1::~CPcsAlgorithm1") );
    
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
    
    delete iPluginLauncher;

    PRINT ( _L("End CPcsAlgorithm1::~CPcsAlgorithm1") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::RemoveSpacesL
// Remove leading and trailing spaces of search query
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1::RemoveSpacesL(CPsQuery& aQuery)
{
    PRINT ( _L("Enter CPcsAlgorithm1::RemoveSpacesL") );

    //PRINTQUERY ( _L("CPcsAlgorithm1::RemoveSpacesL (BEFORE): "), aQuery );

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
    
    //PRINTQUERY ( _L("CPcsAlgorithm1::RemoveSpacesL (AFTER): "), aQuery );
    
    PRINT ( _L("End CPcsAlgorithm1::RemoveSpacesL") );
}    

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::ReplaceZeroWithSpaceL
// Replace all "0"s in a search query with " "s if those characters are on
// the same key.
// ----------------------------------------------------------------------------
TBool  CPcsAlgorithm1::ReplaceZeroWithSpaceL(CPsQuery& aQuery)
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
    for ( TInt index = skipIndex; index < aQuery.Count(); index++ )
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
// CPcsAlgorithm1::PerformSearchL
// Search function for cache
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1::PerformSearchL(const CPsSettings& aSettings,
                                     CPsQuery& aQuery,
                                     RPointerArray<CPsClientData>& aSearchResults,
                                     RPointerArray<CPsPattern>& aSearchSeqs)
{
	PRINT ( _L("Enter CPcsAlgorithm1::PerformSearchL") );
	
	__LATENCY_MARK ( _L("CPcsAlgorithm1::PerformSearchL") );

	// Local arrays to hold the search results 
	RPointerArray<CPsData> tempSearchResults;
	CleanupClosePushL( tempSearchResults );
    RPointerArray<CPsData> tempSearchResultsIni;
    CleanupClosePushL( tempSearchResultsIni );
    RPointerArray<CPsData> tempSearchResultsMod;
    CleanupClosePushL( tempSearchResultsMod );

    // ----------------------- Perform the basic search -----------------------
	/* Even before replacing zeroes with spaces the query can have multiple words
	 * Some phones support infact double keyboards with possibility to type a space in qwerty mode
	 * A space in qwerty mode can even be typed disabling temporarily PCS
	 */
    RemoveSpacesL(aQuery);
    PRINTQUERY ( _L("CPcsAlgorithm1::PerformSearchL: 1st search query: "), aQuery );
	DoSearchL(aSettings, aQuery, tempSearchResultsIni, aSearchSeqs );
    // ------------------------------------------------------------------------

    // ---- Perform new search after "0" replacement if query is not empty ----
    /* Examples:
     * - If the original search string is "Abc0" then we will search again with "Abc".
     * - If the original search string is "00" then we will not search again.
     * - If the original search string is "00Zxc" then we will search again with "Zxc".
     */
    TBool isQueryModified = ReplaceZeroWithSpaceL(aQuery);
    RemoveSpacesL(aQuery);
    if ( isQueryModified && (aQuery.Count() > 0) )
    {
        PRINTQUERY ( _L("CPcsAlgorithm1::PerformSearchL: 2nd search query: "), aQuery );
        DoSearchL(aSettings, aQuery, tempSearchResultsMod, aSearchSeqs );
    }
    // ------------------------------------------------------------------------

    // ------------------ Merge and sort the search results -------------------
	if ( tempSearchResultsIni.Count() + tempSearchResultsMod.Count() > 0)
	{
		// Sort rule        
		TLinearOrder<CPsData> rule( CPcsAlgorithm1Utils::CompareDataBySortOrderL );

		// Avoid duplicates and add new results
		TIdentityRelation<CPsData> identityRule(CPsData::CompareById);

        for ( TInt i = 0; i < tempSearchResultsIni.Count(); i++ )
        {
            if ( tempSearchResults.Find( tempSearchResultsIni[i],identityRule ) == KErrNotFound )
            {
                if (aSettings.GetSortType() == EAlphabetical)
                {
                    tempSearchResults.InsertInOrderAllowRepeats(tempSearchResultsIni[i], rule);
                }
                else
                {
                    tempSearchResults.AppendL(tempSearchResultsIni[i]);
                }
            }
        }

        for ( TInt i = 0; i < tempSearchResultsMod.Count(); i++ )
        {
            if ( tempSearchResults.Find( tempSearchResultsMod[i],identityRule ) == KErrNotFound )
            {
                if (aSettings.GetSortType() == EAlphabetical)
                {
                    tempSearchResults.InsertInOrderAllowRepeats(tempSearchResultsMod[i], rule);
                }
                else
                {
                    tempSearchResults.AppendL(tempSearchResultsMod[i]);
                }
            }
        }
	}
	// ------------------------------------------------------------------------

    // ------------------ Write result objects to the stream ------------------
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
    CleanupStack::PopAndDestroy( &tempSearchResultsMod ); // Close, don't destroy
    CleanupStack::PopAndDestroy( &tempSearchResultsIni ); // Close, don't destroy
    CleanupStack::PopAndDestroy( &tempSearchResults );    // Close, don't destroy

	__LATENCY_MARKEND ( _L("CPcsAlgorithm1::PerformSearchL") );

	PRINT ( _L("End CPcsAlgorithm1::PerformSearchL") );			
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::SearchInputL
// Search function for input string
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1::SearchInputL(CPsQuery& aQuery,
                                   TDesC& aData,
                                   RPointerArray<TDesC>& aMatchSet,
                                   RArray<TPsMatchLocation>& aMatchLocation )
{
    PRINT ( _L("Enter CPcsAlgorithm1::SearchInputL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1::SearchInputL") ); 

    // ----------------------- Perform the basic search -----------------------
    /* Even before replacing zeroes with spaces the query can have multiple words
     * Some phones support infact double keyboards with possibility to type a space in qwerty mode
     * A space in qwerty mode can even be typed disabling temporarily PCS
     */
    RemoveSpacesL(aQuery);
    DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);
    PRINTQUERY    ( _L("CPcsAlgorithm1::SearchInputL: 1st search: "), aQuery );
    PRINT1        ( _L("CPcsAlgorithm1::SearchInputL: 1st search: Search Data: %S"), &aData );
    PRINTMATCHSET ( _L("CPcsAlgorithm1::SearchInputL: 1st search: "), aMatchSet );
    PRINTMATCHLOC ( _L("CPcsAlgorithm1::SearchInputL: 1st search: "), aMatchLocation );
    // ------------------------------------------------------------------------
    
    // ---- Perform new search after "0" replacement if query is not empty ----
    /* Examples:
     * - If the original search string is "Abc0" then we will search again with "Abc".
     * - If the original search string is "00" then we will not search again.
     */
    TBool isQueryModified = ReplaceZeroWithSpaceL(aQuery);
    RemoveSpacesL(aQuery);
    if ( isQueryModified && (aQuery.Count() > 0) )
    {
        DoSearchInputL(aQuery, aData, aMatchSet, aMatchLocation);
        PRINTQUERY    ( _L("CPcsAlgorithm1::SearchInputL: 2nd search: "), aQuery );
        PRINT1        ( _L("CPcsAlgorithm1::SearchInputL: 2nd search: Search Data: %S"), &aData );
        PRINTMATCHSET ( _L("CPcsAlgorithm1::SearchInputL: 2nd search: "), aMatchSet );
        PRINTMATCHLOC ( _L("CPcsAlgorithm1::SearchInputL: 2nd search: "), aMatchLocation );
    }
    /* In cases like we have a contact "Nik0 Abc" and the search query is "Nik0" we do:
     * - a 1st search with "Nik0" that matches "Nik0" (4 chars) of "Nik0 Abc",
     * - a 2nd search with "Nik" that matches "Nik" (3 chars) of "Nik0 Abc".
     * We have to remove from aMatchLocation the occurrence that has a shorter match length.
     * We have to remove the match ("Nik" in this case) even from aMatchSet if there is not
     * any other match for it than the one in "Nik0".
     */

    // --- Remove duplicate items from aMatchLocation ---
    TInt i = 0;
    TBool incrementFirstCursor;
    while ( i < aMatchLocation.Count() )
    {
        incrementFirstCursor = ETrue;
        TInt j = i+1;
        while ( j < aMatchLocation.Count() )
        {
            if (aMatchLocation[j].index == aMatchLocation[i].index)
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

    // --- Remove duplicate items from aMatchSet ---
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
            for ( TInt i = 0; i < aMatchLocation.Count(); i++ )
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
        // --- Remove items End ---

    }
    CleanupStack::PopAndDestroy( dataUpper );
    // ------------------------------------------------------------------------

    // Sort match set
	iHelper->SortSearchSeqsL(aMatchSet);
	
    PRINTQUERY    ( _L("CPcsAlgorithm1::SearchInputL: Final: "), aQuery );
    PRINT1        ( _L("CPcsAlgorithm1::SearchInputL: Final: Search Data: %S"), &aData );
    PRINTMATCHSET ( _L("CPcsAlgorithm1::SearchInputL: Final: "), aMatchSet );
    PRINTMATCHLOC ( _L("CPcsAlgorithm1::SearchInputL: Final: "), aMatchLocation );
	
	__LATENCY_MARKEND ( _L("CPcsAlgorithm1::SearchInputL") );

    PRINT ( _L("End CPcsAlgorithm1::SearchInputL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::SearchMatchStringL
// Search function for input string, result also as string
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::SearchMatchStringL( CPsQuery& aSearchQuery,
                            TDesC& aSearchData,
                            TDes& aMatch )
    {
    PRINT ( _L("Enter CPcsAlgorithm1::SearchMatchStringL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1::SearchMatchStringL") ); 
    
    // ---------------------- Perform the initial search ----------------------
    iMultiSearchHelper->LookupMatchL( aSearchQuery, aSearchData, aMatch );
    PRINTQUERY ( _L("CPcsAlgorithm1::SearchMatchStringL: 1st search: "), aSearchQuery );
    PRINT1     ( _L("CPcsAlgorithm1::SearchMatchStringL: 1st search: Search Data: %S"), &aSearchData );
    PRINT1     ( _L("CPcsAlgorithm1::SearchMatchStringL: 1st search: Result: %S"), &aMatch );
    // ------------------------------------------------------------------------
    
    // ---- Perform new search after "0" replacement if query is not empty ----
    /* Examples:
     * - If the original search string is "Abc0" then we will search again with "Abc".
     * - If the original search string is "00" then we will not search again.
     */
    if ( aMatch.Length() <= 0 )
    {
        TBool isQueryModified = ReplaceZeroWithSpaceL(aSearchQuery);
        if ( isQueryModified && (aSearchQuery.Count() > 0) )
        {
            iMultiSearchHelper->LookupMatchL( aSearchQuery, aSearchData, aMatch );
            PRINTQUERY ( _L("CPcsAlgorithm1::SearchMatchStringL: 2nd search: "), aSearchQuery );
            PRINT1     ( _L("CPcsAlgorithm1::SearchMatchStringL: 2nd search: Search Data: %S"), &aSearchData );
            PRINT1     ( _L("CPcsAlgorithm1::SearchMatchStringL: 2nd search: Result: %S"), &aMatch );            
        }
    }
    
    __LATENCY_MARKEND ( _L("CPcsAlgorithm1::SearchMatchStringL") );

    PRINT ( _L("End CPcsAlgorithm1::SearchMatchStringL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::DoSearchL
// Search function helper
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1::DoSearchL(const CPsSettings& aSettings,
								CPsQuery& aQuery,
								RPointerArray<CPsData>& aSearchResults,
								RPointerArray<CPsPattern>& aSearchSeqs )
{
    PRINT ( _L("Enter CPcsAlgorithm1::DoSearchL") );

    __LATENCY_MARK ( _L("CPcsAlgorithm1::DoSearchL") ); 
    
    // -(0)----------------- Check if group search is required ---------------    
    RArray<TInt> contactsInGroup;
    CleanupClosePushL( contactsInGroup );
    RArray<TInt> groupIdArray;
    CleanupClosePushL( groupIdArray );
    
    // Create a new settings instance
    CPsSettings* tempSettings = aSettings.CloneL();
    CleanupStack::PushL( tempSettings );
    
    TBool isGroupSearch = IsGroupSearchL(*tempSettings, groupIdArray);
    
    if ( isGroupSearch )
    {    
    	// Replace groups URI with contacts DB URI in new search settings
    	ReplaceGroupsUriL(*tempSettings);
        	
        // List of contacts in this group	
    	GetContactsInGroupL ( groupIdArray[0], contactsInGroup );
    }
    
    // -----------------------------------------------------------------------
        
    // Extract query list. 
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);
    CleanupResetAndDestroyPushL( queryList );
    PRINTQUERYLIST ( _L("CPcsAlgorithm1::DoSearchL: "), queryList );

    // (1)-------------------- No query return all contacts -------------------
    if ( queryList.Count() == 0 )
    {
    	GetAllContentsL(*tempSettings, aSearchResults);
    	
    	if ( isGroupSearch ) 
    	{
    		FilterSearchResultsForGroupsL( contactsInGroup, aSearchResults );
    	}
    }
    // ------------------------------------------------------------------------

    // (2)-------------------- Perform a single query search ------------------
    else if ( queryList.Count() == 1 ) // single query
    {
        PRINT ( _L("CPcsAlgorithm1::DoSearchL: Query received is Single. Performing a SingleSearch") );

        iHelper->SearchSingleL(*tempSettings, *queryList[0], isGroupSearch,
                               contactsInGroup, aSearchResults, aSearchSeqs);
    }
    // ------------------------------------------------------------------------

    // (3)-------------------- Perform a multi query search -------------------
    else // multiple query
    {
    	PRINT ( _L("CPcsAlgorithm1::DoSearchL: Query received is Multiple. Performing a MultiSearch") );

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

	__LATENCY_MARKEND ( _L("CPcsAlgorithm1::DoSearchL") );

	PRINT ( _L("End CPcsAlgorithm1::DoSearchL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::DoSearchInputL
// Search function helper
// ----------------------------------------------------------------------------
void  CPcsAlgorithm1::DoSearchInputL(CPsQuery& aQuery,
		                             const TDesC& aData,
		                             RPointerArray<TDesC>& aMatchSet,
		                             RArray<TPsMatchLocation>& aMatchLocation )
{
    PRINT ( _L("Enter CPcsAlgorithm1::DoSearchInputL") );

    TInt queryWords = iMultiSearchHelper->CountMultiQueryWordsL(aQuery);

    // No query    
    if ( queryWords == 0 )
    {
    	PRINT ( _L("CPcsAlgorithm1::DoSearchInputL: Query received is empty") );
    	return;
    }
    
    RPointerArray<CPsQuery> queryList = iMultiSearchHelper->MultiQueryL(aQuery);
    CleanupResetAndDestroyPushL( queryList );
    
    PRINTQUERYLIST ( _L("CPcsAlgorithm1::DoSearchInputL: "), queryList );

    if ( queryList.Count() == 1 ) // Single query
    {
        PRINT ( _L("CPcsAlgorithm1::DoSearchInputL: Query received is single. Performing a single search.") );
        iHelper->SearchMatchSeqL(aQuery, 
                             	 aData, 
                              	 aMatchSet,
                             	 aMatchLocation);
    }
    else // multiple query
    {
    	PRINT ( _L("CPcsAlgorithm1::DoSearchInputL: Query received is Multiple. Performing a MultiSearch") );
    	iMultiSearchHelper->SearchMatchSeqMultiL(queryList, 
		                                         aData,
		                                         aMatchSet,
		                                         aMatchLocation);
    }

	// Delete all the query elements
	CleanupStack::PopAndDestroy( &queryList ); // ResetAndDestroy

	PRINT ( _L("End CPcsAlgorithm1::DoSearchInputL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::AddData
// Add a data element to the pool
// ----------------------------------------------------------------------------    
void CPcsAlgorithm1::AddData(TDesC& aDataStore, CPsData* aData)
{
    TInt arrayIndex = GetCacheIndex(aDataStore);
    
    if ( arrayIndex < 0 ) return;
	
	CPcsCache* cache = iPcsCache[arrayIndex];
	
	// Fill the data store index
	TInt dataStoreIndex = FindStoreUri(aDataStore);
    if ( dataStoreIndex >= 0 )
    {
    	aData->SetUriId(dataStoreIndex);
    }
    else 
    {
    	PRINT(_L("CPcsAlgorithm1::AddData: Unknown data store"));
    	return;
    }
    
	TRAPD(err, cache->AddToCacheL(*aData));
	
	if ( err != KErrNone )
	{
		SetCachingError(aDataStore, err);
	}
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::RemoveData
// Remove a data element from the pool
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::RemoveData(TDesC &aDataStore, TInt aItemId)
{
    TInt arrayIndex = GetCacheIndex(aDataStore);
    
    if ( arrayIndex < 0 ) return;
	
	CPcsCache* cache = iPcsCache[arrayIndex];
	
	TRAPD(err, cache->RemoveFromCacheL(aItemId));
	
	if ( err != KErrNone )
	{
		SetCachingError(aDataStore, err);
	}
}

// ---------------------------------------------------------------------
// CPcsAlgorithm1::RemoveAll
// Remove all the contacts from a datastore
// ---------------------------------------------------------------------
void CPcsAlgorithm1::RemoveAll(TDesC& aDataStore)
{
	TInt dataStoreIndex = GetCacheIndex(aDataStore);
	
	if(dataStoreIndex < 0) return;
	
    CPcsCache* cache = iPcsCache[dataStoreIndex];
    	
	cache->RemoveAllFromCache();
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetCacheIndex
// Return the cache index for a data store
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1::GetCacheIndex(const TDesC& aDataStore)
{
    for ( int i = 0; i < iPcsCache.Count(); i++ )
    {
    	CPcsCache* cache = iPcsCache[i];
    	
    	if ( cache->GetURI().CompareC(aDataStore) == 0 ) return i;
    }
    
	return -1;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::AddDataStore
// Adds a new store
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::AddDataStore(TDesC& aDataStore)
{
    // Check if the datastore cache already exists
    TInt index = GetCacheIndex(aDataStore);
    if ( index != -1 )
    {
    	// Already exists
    	return;
    }

    // Create a new cache
    CPcsCache* cache = NULL;
	TRAPD(err, cache = CPcsCache::NewL(aDataStore, *iKeyMap, (TUint8) iPcsCache.Count()));
	if ( err != KErrNone )
	{
		SetCachingError(aDataStore, err);
		return;
	}
	
    RArray<TInt> dataFields;
    TRAP(err, iPsDataPluginInterface->GetSupportedDataFieldsL(cache->GetURI(), dataFields));
	if ( err != KErrNone )
	{
		SetCachingError(aDataStore, err);
		return;
	}		    
    cache->SetDataFields(dataFields); 
    
    // Check if sort order is persisted already    
    RArray<TInt> sortOrder;
    TRAP(err, ReadSortOrderFromCenRepL(cache->GetURI(), sortOrder));
    if ( err != KErrNone )
	{
		SetCachingError(aDataStore, err);
		return;
	}
    
    if ( sortOrder.Count() == 0 )
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
	if ( err != KErrNone )
	{
		PRINT ( _L("CPcsAlgorithm1::AddDataStore() Set Caching Error ") );
		SetCachingError(aDataStore, err);
		UpdateCachingStatus(aDataStore,ECachingCompleteWithErrors);
		return;
	}		
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::RemoveDataStore
// Removes an existing data store
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::RemoveDataStore(TDesC& aDataStore)
{
    for ( int i = 0; i < iPcsCache.Count(); i++ )
    {
    	CPcsCache* cache = iPcsCache[i];
    	
    	if ( cache->GetURI().CompareC(aDataStore) == 0 ) 
    	{
    		delete iPcsCache[i];
    		iPcsCache.Remove(i);
    	}
    }
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::IsLanguageSupportedL
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1::IsLanguageSupportedL(TUint32 aLang)
{
    return iKeyMap->IsLanguageSupported(aLang);
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetUriForIdL
// Get the URI string for this internal id
// ----------------------------------------------------------------------------
const TDesC& CPcsAlgorithm1::GetUriForIdL(TUint8 aUriId)
{
    TBool found = EFalse;
    TInt i = 0;
    
    for ( i = 0; i < iPcsCache.Count(); i++ )
    {
    	if ( iPcsCache[i]->GetUriId() == aUriId ) 
    	{    	   
    	   found = ETrue;
    	   break;
    	}
    }	
	
	if ( ! found )
	{
		User::Leave(KErrNotFound);
	}
	
	return iPcsCache[i]->GetURI(); 
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::FindStoreUri
// Checks if this store exists
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1::FindStoreUri ( const TDesC& aDataStore )
{
    for ( int i = 0; i < iPcsCache.Count(); i++ )
    {
        if ( aDataStore.CompareC(iPcsCache[i]->GetURI()) == 0 ) 
    	{
    	   return i;   
    	}
    }

    return -1;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::UpdateCachingStatus
// Update caching status
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::UpdateCachingStatus(TDesC& aDataStore, TInt aStatus)
{
	PRINT ( _L("Enter CPcsAlgorithm1::UpdateCachingStatus") );
	
	TInt index = FindStoreUri(aDataStore);
	iPcsCache[index]->UpdateCacheStatus(aStatus);
	
	// Check if any error occurred
	// If so, update the cache status, Set the property and return
	if( aStatus < 0)
	{
		SetCachingError(aDataStore, aStatus);
		//return;
	}
	
	// No error occurred
	TCachingStatus status = ECachingComplete;
	TBool atLeastOneStoreCachingCompleteWithErrors(EFalse);
	for ( TInt i = 0; i < iPcsCache.Count(); i++ )
	{
		if( iPcsCache[i]->GetCacheStatus() == ECachingComplete)
		  {
	      continue;
	      }	
		else if ( iPcsCache[i]->GetCacheStatus() == ECachingCompleteWithErrors)
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
	
	if ( status == ECachingComplete )
	{
		// See if any error occurred while caching
		// If so, change the status to ECachingCompleteWithErrors
		if (( iCacheError != KErrNone ) || (atLeastOneStoreCachingCompleteWithErrors))
			status = ECachingCompleteWithErrors;
	}
	
	// Check if status changed
	if ( status != iCacheStatus )
	{
		iCacheStatus = status;
		RProperty::Set(KCStatus,0,iCacheStatus );
	}
	
	PRINT ( _L("End CPcsAlgorithm1::UpdateCachingStatus") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::SetCachingError
// Updates cachinge error
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::SetCachingError(const TDesC& aDataStore, TInt aError)
{
	PRINT2 ( _L("SetCachingError::URI %S ERROR %d"), &aDataStore, aError );

	iCacheError = aError;
	RProperty::Set( KCStatus, 1, iCacheError );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetAllContentsL
// Returns all the contents of a store
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::GetAllContentsL ( const CPsSettings& aSettings,
                                       RPointerArray<CPsData>& aResults )
{
	__LATENCY_MARK ( _L("CPcsAlgorithm1::GetAllContentsL") );
    
    PRINT ( _L("Enter CPcsAlgorithm1::GetAllContentsL") );   
    
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
    for ( TInt dsIndex = 0; 
          dsIndex < dataStores.Count(); 
          dsIndex++ )
    {	        	
        RPointerArray<CPsData> *temp = new (ELeave) RPointerArray<CPsData>();
        searchResultsArr.Append(temp);
        
        TInt arrayIndex = GetCacheIndex(*(dataStores[dsIndex]));
        
		if ( arrayIndex < 0 ) continue;
		
		CPcsCache* cache = GetCache(arrayIndex);
        	      
        cache->GetAllContentsL(*(searchResultsArr[dsIndex]));
    }
    
    CleanupStack::PopAndDestroy( &dataStores ); // ResetAndDestroy
  
    // Merge the results from different data stores
    CPcsAlgorithm1Utils::FormCompleteSearchResultsL(searchResultsArr,
    												aResults);
  
    // Cleanup the local arrays
    for(TInt i = 0; i < searchResultsArr.Count(); i++)
    {
    	searchResultsArr[i]->Reset();
    }
    CleanupStack::PopAndDestroy( &searchResultsArr ); // ResetAndDestroy

    PRINT1 ( _L("Number of results = %d"), aResults.Count() );
   
    PRINT ( _L("End CPcsAlgorithm1::GetAllContentsL") );
    
    __LATENCY_MARKEND ( _L("CPcsAlgorithm1::GetAllContentsL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::IsGroupSearchL
// Checks if a group search is required
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1::IsGroupSearchL ( CPsSettings& aSettings,
                                       RArray<TInt>& aGroupIdArray )
{
    PRINT ( _L("Enter CPcsAlgorithm1::IsGroupSearchL") );   
    
    // Get the groupIds in the seach settings
    aSettings.GetGroupIdsL(aGroupIdArray);
     
    // Get the current URIs defined in settings    
    RPointerArray<TDesC> searchUris;
    CleanupResetAndDestroyPushL( searchUris );
    aSettings.SearchUrisL(searchUris);
    
    if ( aGroupIdArray.Count() && (searchUris.Count() > aGroupIdArray.Count() ) )
    {
    	// There is an error, either there are more than one groups
    	// or the settings contain a combination of group/non-group Uris
    	aGroupIdArray.Close();
    	User::Leave(KErrArgument); 
    }
    
    CleanupStack::PopAndDestroy( &searchUris ); // ResetAndDestroy
        
    PRINT ( _L("End CPcsAlgorithm1::IsGroupSearchL") );    
    
    if ( aGroupIdArray.Count() == 1 )
        return ETrue;
    
    return EFalse;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::ReplaceGroupsUriL
// Replace groups uri to contacts uri
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::ReplaceGroupsUriL( CPsSettings& aSettings )
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
// CPcsAlgorithm1::FilterSearchResultsForGroupsL
// Filters the results that belong to a group
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::FilterSearchResultsForGroupsL(RArray<TInt>& contactsInGroup, 
                                                   RPointerArray<CPsData>& aSearchResults)
{
	PRINT ( _L("Enter CPcsAlgorithm1::FilterSearchResultsForGroupsL") );   
   
	// for each search result
	// Note: aSearchResults.Count() is to be checked everytime,
	//       since the elements are being removed dynamically.
	for ( TInt j = 0 ; j < aSearchResults.Count(); j++ )
	{
		TBool includeResult = EFalse;

		if ( contactsInGroup.Find(aSearchResults[j]->Id()) != KErrNotFound )
		{
			includeResult = ETrue;
		}

		if ( includeResult == EFalse )
		{
			aSearchResults.Remove(j);
			j--; // j is decremented, since that object is removed
		}
	}
	        
	PRINT ( _L("End CPcsAlgorithm1::FilterSearchResultsForGroupsL") );    
}


// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetContactsInGroupL
// Recover contacts that belong to a group
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::GetContactsInGroupL( TInt aGroupId, 
                                          RArray<TInt>& aGroupContactIds )
{	    
    // Clear results array
    aGroupContactIds.Reset();
    
    // Cache Index for group database
    TInt cacheIndex = GetCacheIndex(KVPbkDefaultGrpDbURI);
    
    // Get the groups contact ids 
	if ( cacheIndex != -1 )
	{
		RPointerArray<CPsData> groups;
		CleanupClosePushL( groups );
		
		// Get all groups
		iPcsCache[cacheIndex]->GetAllContentsL(groups);
	
	    // Get all contacts in group
		for ( TInt i = 0; i < groups.Count(); i++ )
		{
			if ( groups[i]->Id() == aGroupId )
			{
				groups[i]->IntDataExt(aGroupContactIds); // All contacts in group
				break;
			}		
		}
		
		CleanupStack::PopAndDestroy( &groups ); // Close
	}
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetDataOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::GetDataOrderL ( TDesC& aURI,
                                     RArray<TInt>& aDataOrder )
{
    PRINT ( _L("End CPcsAlgorithm1::GetDataOrderL") );

    TInt arrayIndex = -1;     
    
    if ( CPcsAlgorithm1Utils::IsGroupUri(aURI) )
    {
        // If search in a group uri, use contacts db
        arrayIndex = GetCacheIndex(KVPbkDefaultCntDbURI);
    }
    else 
    {		
		arrayIndex = GetCacheIndex(aURI);
    }
    
    if ( arrayIndex < 0 ) return;
        
    CPcsCache* cache = iPcsCache[arrayIndex];
    
    aDataOrder.Reset();
    
    // Get the data fields for this cache
    cache->GetDataFields(aDataOrder);
    
    PRINT ( _L("End CPcsAlgorithm1::GetDataOrderL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::GetSortOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::GetSortOrderL ( TDesC& aURI,
                                     RArray<TInt>& aDataOrder )
{
    PRINT ( _L("End CPcsAlgorithm1::GetSortOrderL") );

    TInt arrayIndex = -1;
    
    if ( CPcsAlgorithm1Utils::IsGroupUri(aURI) )
    {
        // If search in a group uri, use contacts db
        TBuf<255> cntdb(KVPbkDefaultCntDbURI);
        arrayIndex = GetCacheIndex(cntdb);
    }
    else 
    {
		arrayIndex = GetCacheIndex(aURI);
    }
    
    if ( arrayIndex < 0 ) return;
    
    CPcsCache* cache = iPcsCache[arrayIndex];
    
    aDataOrder.Reset();
    
    // Get the data fields for this cache
    cache->GetSortOrder(aDataOrder);
    
    PRINT ( _L("End CPcsAlgorithm1::GetSortOrderL") );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1::ChangeSortOrderL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm1::ChangeSortOrderL ( TDesC& aURI,
                                        RArray<TInt>& aSortOrder )
{
    PRINT ( _L("Enter CPcsAlgorithm1::ChangeSortOrderL.") );
    
    PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL. Sort order change received.") );
    PRINT1 ( _L("URI = %S"), &aURI );
    
    // If URI is search in a group URI return
    if ( CPcsAlgorithm1Utils::IsGroupUri(aURI) )
    {
       PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL. Sort order change not supported.") );
       return;	
    }
    
    // Check if a cache exists
    TInt arrayIndex = GetCacheIndex(aURI);
    if ( arrayIndex < 0 )
    {
        PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL. Cache for URI doesn't exist.") );
    	return;
    }
    
    // Cache instance for this URI
    CPcsCache* cache = iPcsCache[arrayIndex];
        
    // Check if received sort order is same as before
    RArray<TInt> mySortOrder;
    cache->GetSortOrder(mySortOrder);
    
    if ( aSortOrder.Count() == mySortOrder.Count() )    
    {
        TBool same = ETrue;
        for ( TInt i = 0; i < mySortOrder.Count(); i++ )	
        {
            if ( mySortOrder[i] != aSortOrder[i] )
            {
                same = EFalse;
                break;
            }
        }
         
        if ( same )
        {
            PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL. Same sort order received. Ignoring ...") );
            PRINT ( _L("End CPcsAlgorithm1::ChangeSortOrderL.") );
            mySortOrder.Reset();
            return;
        }
    }
    
    mySortOrder.Reset();
    
    PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL. New sort order received. Refreshing ...") );
	// Set the new sort order on the cache
	cache->SetSortOrder(aSortOrder);
	
    // Persist the changes in sort order
    WriteSortOrderToCenRepL(aURI, aSortOrder);
	
	// Resort data
	TInt err = KErrNone; 
	TRAP(err, cache->ResortdataInPoolsL());
	if ( err != KErrNone )
	{
		PRINT ( _L("CPcsAlgorithm1::ChangeSortOrderL() Set Caching Error ") ); 
		SetCachingError(aURI, err);
		UpdateCachingStatus(aURI,ECachingCompleteWithErrors);
		return;
	}
	
	PRINT ( _L("End CPcsAlgorithm1::ChangeSortOrderL.") );
}

// ---------------------------------------------------------------------------------
// Read the persisted sort order from the central repository
// Persisted sort order is of form URI Field1 Field2 Field3 .. FieldN (space delimited)
// ---------------------------------------------------------------------------------
void CPcsAlgorithm1::ReadSortOrderFromCenRepL(const TDesC& aURI, 
                                              RArray<TInt>& aSortOrder)
{
    PRINT ( _L("Enter CPcsAlgorithm1::ReadSortOrderFromCenRepL.") );

    aSortOrder.Reset();
    
	CRepository *repository = CRepository::NewL( KCRUidPSSortOrder );

    // Read the sort order from cenrep
    TBuf<KCRMaxLen> str;
    
    for ( TInt i(KCenrepFieldsStartKey); 
          i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
          i++ )
    {
	    TInt err = repository->Get( i, str );
	    
	    if ( KErrNone != err )
	    {
		    break;
	    }
	    
	    if (str != KNullDesC)
	    {		    
		    TLex lex(str);
		    
		    // Extract the URI
		    TPtrC token = lex.NextToken();
		    
		    if ( aURI.Compare(token) == 0 )
		    {
		        // Extract the sort order
		        token.Set(lex.NextToken());
		        
				while ( token.Length() != 0 )
				{	
				    TLex lex1(token);
				    
				    TInt intVal;				    
				    TInt err = lex1.Val(intVal);
				    
					if ( KErrNone == err )
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
    
    PRINT ( _L("End CPcsAlgorithm1::ReadSortOrderFromCenRepL.") );
}

// ---------------------------------------------------------------------------------
// Write the sort order into the central repository
// Persisted sort order is of form URI Field1 Field2 Field3 .. FieldN (space delimited)
// ---------------------------------------------------------------------------------
void CPcsAlgorithm1::WriteSortOrderToCenRepL(const TDesC& aURI, 
                                             RArray<TInt>& aSortOrder)
{   
    PRINT ( _L("Enter CPcsAlgorithm1::WriteSortOrderToCenRepL.") );

    CRepository *repository = CRepository::NewLC( KCRUidPSSortOrder );

	// Check if there an entry for this URI in cenrep
	TBuf<KCRMaxLen> str;
	TInt keyIndex = -1;

	for ( TInt i(KCenrepFieldsStartKey); 
	  		   i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
	           i++ )
	{
		TInt err = repository->Get( i, str );

		if ( KErrNone != err )
		{
		    PRINT ( _L("CPcsAlgorithm1::WriteSortOrderToCenRepL. cenrep error.") );
		    return;
		}

		if (str != KNullDesC)
		{		    
		    TLex lex(str);
		    
		    // Extract the URI
		    TPtrC token = lex.NextToken();
		    
		    if ( aURI.Compare(token) == 0 )
		    {
		         keyIndex = i; // i has the key index for this URI
		         break;
		    }
		}
	}
	
	// No entry for this URI in cenrep
	// Find the next free location in cenrep
	if ( keyIndex == -1 )
	{
		// Find the next free key index
		for ( TInt i(KCenrepFieldsStartKey); 
		  		   i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; 
		           i++ )
	    {
		    TInt err = repository->Get( i, str );

			if ( KErrNone != err )
			{
			    PRINT ( _L("CPcsAlgorithm1::WriteSortOrderToCenRepL. cenrep error.") );
			    return;
			}

			if (str == KNullDesC)
			{
			    keyIndex = i; // i has the next free location
			    break;
			}
	    }
	}
	
	if ( keyIndex == -1 )
	{
		PRINT ( _L("CPcsAlgorithm1::WriteSortOrderToCenRepL. Persist limit violated.") );
		return;
	}

    // Persist the sort order
	HBufC* str1 = HBufC::NewLC(KCRMaxLen);
	TPtr ptr(str1->Des());

	// Append the URI
	ptr.Append(aURI);
	ptr.Append(KSpace);

	// Append the sort order fields
	for ( TInt j = 0; j < aSortOrder.Count(); j++ )
	{
		ptr.AppendNum(aSortOrder[j]);
	    ptr.Append(KSpace);
	}

	// Write to persistent store
	TInt err = repository->Set(keyIndex, ptr); 

	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy( str1 );
  
    CleanupStack::PopAndDestroy( repository );
     
    PRINT ( _L("End CPcsAlgorithm1::WriteSortOrderToCenRepL.") );
}

// ---------------------------------------------------------------------------------
// WriteClientDataL.
// Write the content required by client
// ---------------------------------------------------------------------------------
CPsClientData* CPcsAlgorithm1::WriteClientDataL( CPsData& aPsData )
{
    CPsClientData* clientData = CPsClientData::NewL();
	CleanupStack::PushL(clientData);
	
	// set Id
	clientData->SetId(aPsData.Id());
	
	// set Uri
	clientData->SetUriL(GetUriForIdL(aPsData.UriId()));
	
	// set pointer to the each data element
	for(TInt i = 0; i < aPsData.DataElementCount(); i++)
	{
		clientData->SetDataL(i, *(aPsData.Data(i)));
	}
	
	// set data extension
	clientData->SetDataExtensionL(aPsData.DataExtension());
	    
	// Set the Field match
	clientData->SetFieldMatch(aPsData.DataMatch())  ;
	CleanupStack::Pop(clientData);
	
	return clientData;
}

// ---------------------------------------------------------------------------------
// DoLaunchPluginsL.
// launch plugins by idle
// ---------------------------------------------------------------------------------
TInt CPcsAlgorithm1::DoLaunchPluginsL(TAny* aPtr)
    {
    CPcsAlgorithm1* ptr = (CPcsAlgorithm1*) aPtr;
    ptr->DoLaunchPluginsL();
    return EFalse;
    }

// ---------------------------------------------------------------------------------
// DoLaunchPluginsL.
// lauch plugins
// ---------------------------------------------------------------------------------
void CPcsAlgorithm1::DoLaunchPluginsL()
    {
    // Initialize available data adapters
    iPsDataPluginInterface = CPsDataPluginInterface::NewL(this, this);    
    iPsDataPluginInterface->InstantiateAllPlugInsL();
    
    // Store the cache list in TLS
    // Required to support sort order changes in a memory efficient way
    // This avoids storing sort order information in the CPsData element
    // and storing it in CPcsCache. Refer CPcsAlgorithm1Utils::CompareDataBySortOrderL
    // to see how this is being used.
    User::LeaveIfError( Dll::SetTls(&iPcsCache) );
    
    // Initialize cache
    RPointerArray<TDesC> dataStores;
    CleanupClosePushL( dataStores );
    
    iPsDataPluginInterface->GetAllSupportedDataStoresL(dataStores);
        
    for ( TInt dIndex = 0; dIndex < dataStores.Count(); dIndex++ )
        {
        AddDataStore(*(dataStores[dIndex]));
        }
    
    CleanupStack::PopAndDestroy( &dataStores ); // Close
    }
// End of file

