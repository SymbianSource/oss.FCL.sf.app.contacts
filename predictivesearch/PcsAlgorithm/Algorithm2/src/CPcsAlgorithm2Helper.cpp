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
#include <PbkGlobalSettingFactory.h>
#include "FindUtilChineseECE.h"
#include "CPcsAlgorithm2.h"
#include "CPcsAlgorithm2Helper.h"
#include "CPcsAlgorithm2Utils.h"
#include "CPcsDebug.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsData.h"
#include "CPsQuery.h"
#include "CPsQueryItem.h"
#include "CPsDataPluginInterface.h"
#include "CPcsPoolElement.h"


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
    iMaxCount = 0;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::ConstructL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::ConstructL") );

    iAlgorithm = aAlgorithm;
    iKeyMap = iAlgorithm->GetKeyMap();

    iPbkSettings = PbkGlobalSettingFactory::CreatePersistentSettingL();
    iPbkSettings->ConnectL( MPbkGlobalSetting::EGeneralSettingCategory );
    iPbkSettings->RegisterObserverL( this );
    UpdateNameOrderL();
    
    PRINT ( _L("End CPcsAlgorithm2Helper::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper") );
    iSearchResultsArr.ResetAndDestroy();
    PRINT ( _L("End CPcsAlgorithm2Helper::~CPcsAlgorithm2Helper") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchSingleL
// Search function for input with both ITU-T and QWERTY mode
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchSingleL(const CPsSettings& aSettings,
                                         CPsQuery& aPsQuery, 
                                         TBool aIsSearchInGroup,
                                         const RArray<TInt>& aContactsInGroup,
                                         RPointerArray<CPsData>& aSearchResults,
                                         RPointerArray<CPsPattern>& aSearchSeqs)
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchSingleL") );

    //__LATENCY_MARK ( _L("CPcsAlgorithm2Helper::SearchSingleL") );

    iMaxCount = aSettings.MaxResults();
    
    // Create filtering helper for the required sort type
    TSortType sortType = aSettings.GetSortType();
    CPcsAlgorithm2FilterHelper* filterHelper =
            CPcsAlgorithm2FilterHelper::NewL(sortType);
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
    const TInt dataStoresCount = dataStores.Count();

    for (TInt dsIndex = 0; dsIndex < dataStoresCount; dsIndex++)
        {
        RPointerArray<CPsData>* temp = new (ELeave) RPointerArray<CPsData> ();
        CleanupStack::PushL( temp );
        iSearchResultsArr.AppendL( temp );
        CleanupStack::Pop( temp );

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
        TUint8 filteredDataMatch = CPcsAlgorithm2Utils::FilterDataFieldsL(requiredDataFields,
                                                                          supportedDataFields);

        // Perform filtering
        FilterResultsSingleL(filterHelper,
                             elements,
                             aPsQuery,
                             filteredDataMatch,
                             aIsSearchInGroup,
                             aContactsInGroup);

        // If alphabetical sorting, get the results for this datastore
        if (sortType == EAlphabetical)
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
    if (sortType == EAlphabetical)
        {
        // Form the complete searchResults array
        CPcsAlgorithm2Utils::FormCompleteSearchResultsL(iSearchResultsArr,
                                                        aSearchResults);
        }
    else
        {
        // Results are already sorted pattern based
        filterHelper->GetResults(aSearchResults);
        }

    // Get the sorted match sequence list
    filterHelper->GetPatternsL(aSearchSeqs);

    PRINT1 ( _L("Number of search results = %d"), aSearchResults.Count() );

    // Cleanup
    const TInt searchResultsArrCount = iSearchResultsArr.Count();
    for (TInt i = 0; i < searchResultsArrCount; i++)
        {
        iSearchResultsArr[i]->Reset();
        }
    iSearchResultsArr.ResetAndDestroy();

    CleanupStack::PopAndDestroy( filterHelper );

    //__LATENCY_MARKEND ( _L("CPcsAlgorithm2Helper::SearchSingleL") );

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchSingleL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SearchMatchSeqL
// Function to search matching sequences in the input text
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SearchMatchSeqL(CPsQuery& aPsQuery,
                                           const TDesC& aData,
                                           RPointerArray<TDesC>& aMatchSet,
                                           RArray<TPsMatchLocation>& aMatchLocation )
    {
    PRINT ( _L("Enter CPcsAlgorithm2Helper::SearchMatchSeqL") );

    RArray<TInt> matchPos;
    CleanupClosePushL( matchPos );
    RArray<TInt> matchLen;
    CleanupClosePushL( matchLen );
    if ( iAlgorithm->FindUtilECE()->MatchRefineL( aData, aPsQuery, matchPos, matchLen, ETrue ) )
        {
        ASSERT( matchPos.Count() == matchLen.Count() );
        const TInt matchPosCount = matchPos.Count();
        for ( TInt i = 0 ; i < matchPosCount ; ++i )
            {
            TPsMatchLocation tempLocation;
    
            // check for directionality of the text
            TBool found(EFalse);
            TPtrC matchingPart = aData.Mid( matchPos[i], matchLen[i] );
            TBidiText::TDirectionality dir = TBidiText::TextDirectionality(matchingPart, &found);
    
            tempLocation.index = matchPos[i];
            tempLocation.length = matchLen[i];
            tempLocation.direction = dir;
    
            // Add the match location to the data structure array
            aMatchLocation.Append(tempLocation);
            
            // Add the matched sequence to set, not allowing duplicates
            CPcsAlgorithm2Utils::AppendMatchToSeqL( aMatchSet, matchingPart );
            }
        }
    CleanupStack::PopAndDestroy( &matchLen );
    CleanupStack::PopAndDestroy( &matchPos );

    PRINT ( _L("End CPcsAlgorithm2Helper::SearchMatchSeqL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::FilterResultsSingleL
// Subset search function
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::FilterResultsSingleL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                                                RPointerArray<CPcsPoolElement>& aSearchSet,
                                                CPsQuery& aSearchQuery,
                                                TUint8 aFilteredDataMatch,
                                                TBool aIsSearchInGroup,
                                                const RArray<TInt>& aContactsInGroup)
    {
    PRINT ( _L("Enter CPcsAlgorithm2::FilterResultsSingleL") );

    TInt maxcount = 0;
    CFindUtilChineseECE* pFindUtilEce = iAlgorithm->FindUtilECE();

    // Assume that all the elements is aSearchSet are from the same database.
    // Get firstname index and lastname index for that database.
    // If both are found, then name fields are matched a bit differently compared
    // to rest of the fields.
    TInt fnIndex( KErrNotFound );
    TInt lnIndex( KErrNotFound );
    TBool fullNameSearch( EFalse );
    if ( aSearchSet.Count() )
        {
        TInt dbUriId = aSearchSet[0]->GetPsData()->UriId();
        CPcsCache* cache = iAlgorithm->GetCache( dbUriId );
        fnIndex = cache->GetFirstNameIndex();
        lnIndex = cache->GetLastNameIndex();

        // Ensure that firstname and lastname are among the fields to be searched
        TUint8 fnBitmask = 1 << fnIndex;
        TUint8 lnBitmask = 1 << lnIndex;
        if ( (aFilteredDataMatch & fnBitmask) && 
             (aFilteredDataMatch & lnBitmask) )
            {
            fullNameSearch = ETrue;
            // Remove firstname and lastname from the set of fields to search
            // so that they will not be searched twice.
            aFilteredDataMatch &= ~fnBitmask;
            aFilteredDataMatch &= ~lnBitmask;
            }
        }
    
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
        
        // Buffer for matched character sequence. Currently, we don't return
        // accurate pattern but just first character from the matching point.
        // Current clients don't actually use this data for anything.
        TBuf<1> matchingData;
        
        TBool matched = EFalse;

        // Parse thru each data field and filter the results
        // -------------------------------------------------
        
        // Name fields are handled separately to enable searching with query like
        // "LastnameFirstname". Searching fullname by query string 
        // only for the pool elements matching with the firstname or lastname
        if ( fullNameSearch && ( poolElement->IsDataMatch(fnIndex) || 
            poolElement->IsDataMatch(lnIndex) ))
            {
            HBufC* fullName = CreateNameBufLC( *psData, fnIndex, lnIndex );

            // FindUtil can take care of matching queries like "Firstname", "Lastname", 
            // and "LastnameFirstname".
            matched = pFindUtilEce->MatchRefineL( *fullName, aSearchQuery );
            if (matched)
                {
                matchingData = fullName->Left(1);
                psData->SetDataMatch( fnIndex );
                psData->SetDataMatch( lnIndex );
                }
            
            CleanupStack::PopAndDestroy( fullName );
            }

        // Find from the rest of the fields if no match found so far. 
        // Name fields are already removed from aFilteredDataMatch if we did separate full name search.
        const TInt dataElementCount = psData->DataElementCount();
        for ( TInt dataIndex = 0; dataIndex < dataElementCount && !matched ; dataIndex++ )
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
                TPtrC fieldData( *psData->Data(dataIndex) );
                matched = pFindUtilEce->MatchRefineL( fieldData, aSearchQuery );
                if (matched)
                    {
                    matchingData = fieldData.Left(1);
                    psData->SetDataMatch( dataIndex );
                    }
                }
            }


        // Add to results if match is found
        if ( matched )
            {
            RPointerArray<TDesC> tempMatchSeq;
            CleanupClosePushL( tempMatchSeq );

            // Wrap matched character sequence to array.
            matchingData.UpperCase();
            tempMatchSeq.AppendL(&matchingData);
            
            // Add the result
            aAlgorithmFilterHelper->AddL( psData, tempMatchSeq );
            maxcount++;
            
            // Cleanup the match sequence array as 
            // they are stored in pattern details structure
            CleanupStack::PopAndDestroy( &tempMatchSeq ); // Close
            
            // TODO: Match seqs could be extracted from actual
            //       match locations by using the other overload of
            //       CFindUtilChineseECE::MatchRefineL().
            //       Currently, match seq data is not used by clients.
            }
        
          if ( iMaxCount != -1 && maxcount > iMaxCount )
              {
              return;
              }
        }

    PRINT ( _L("End CPcsAlgorithm2Helper::FilterResultsSingleL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SortSearchSeqsL()
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SortSearchSeqsL(RPointerArray<TDesC>& aSearchSeqs)
    {
    // Sort the search seqs
    TLinearOrder<TDesC> rule( CPcsAlgorithm2Utils::MyCompareC );
    aSearchSeqs.Sort(rule);
    }

// ---------------------------------------------------------------------------
// CPcsAlgorithm2Helper::CreteNameBufLC
// Update name order according to Phonebook setting
// ---------------------------------------------------------------------------
HBufC* CPcsAlgorithm2Helper::CreateNameBufLC( const CPsData& aContactData, 
        TInt aFirstNameIndex, TInt aLastNameIndex ) const
    {
    const TDesC& firstName( *aContactData.Data(aFirstNameIndex) );
    const TDesC& lastName( *aContactData.Data(aLastNameIndex) );
    CFindUtilChineseECE* pFindUtilEce = iAlgorithm->FindUtilECE();
    HBufC* fullName = NULL;
    
    if ( pFindUtilEce->IsChineseWordIncluded( lastName ) || 
        pFindUtilEce->IsChineseWordIncluded( firstName ) )
        {
        fullName = HBufC::NewLC( lastName.Length() + firstName.Length()  + 1 );
        TPtr fullNamePtr = fullName->Des();
    
        // Form the full name according the Phonebook name order setting. Typically,
        // the order is always lastname-firstname in Chinese variants. However, at least
        // currently it is possible to change this from Contacts app if UI language has
        // been set to English.
        if ( iNameOrder == ELastnameFirstname )
            {
            fullNamePtr.Append( lastName );
            //Holds the first char of first name of a contact
            TBuf<10> firstChar;   
            firstChar.Zero();
            
            if(firstName.Length())
                {
                firstChar.Append(firstName[0]);   
                // There is no space between LastName and FirstName in Chinese Name
                // except that the firstChar of FirstName isn't Chinese character
                if ( !pFindUtilEce->IsChineseWordIncluded( firstChar ) )
                    {
                    fullNamePtr.Append( KSpace );
                    }
                fullNamePtr.Append( firstName ); 
                }
            }
        else
            {
            fullNamePtr.Append( firstName );
            //Holds the first char of last name of a contact
            TBuf<10> firstChar;   
            firstChar.Zero();
            
            if(lastName.Length())
                {
                firstChar.Append(lastName[0]);
                // There is no space between LastName and FirstName in Chinese Name
                // except that the firstChar of Lastname isn't Chinese character  
                if ( !pFindUtilEce->IsChineseWordIncluded( firstChar ) )
                    {
                    fullNamePtr.Append( KSpace );
                    }
                fullNamePtr.Append( lastName ); 
                }
            }
        }
    else
        {
        fullName = HBufC::NewLC( lastName.Length() + firstName.Length() + 1 );
        TPtr fullNamePtr = fullName->Des();
    
        // Form the full name according the Phonebook name order setting. Typically,
        // the order is always lastname-firstname in Chinese variants. However, at least
        // currently it is possible to change this from Contacts app if UI language has
        // been set to English.
        if ( iNameOrder == ELastnameFirstname )
            {
            fullNamePtr.Append( lastName );
            fullNamePtr.Append( KSpace );
            fullNamePtr.Append( firstName );
            }
        else
            {
            fullNamePtr.Append( firstName );
            fullNamePtr.Append( KSpace );
            fullNamePtr.Append( lastName );
            }
        }
    
    return fullName;
    }

// ---------------------------------------------------------------------------
// CPcsAlgorithm2Helper::UpdateNameOrderL
// Update name order according to Phonebook setting
// ---------------------------------------------------------------------------
void CPcsAlgorithm2Helper::UpdateNameOrderL()
    {
    /*
    * Phonebook name ordering flag, integer value, possible values:
    * 0: name order Lastname Firstname
    * 1: name order Firstname Lastname
    * 2: name order undefined
    */
    TInt nameOrderSetting;
    iPbkSettings->Get( MPbkGlobalSetting::ENameOrdering, nameOrderSetting );
    
    switch ( nameOrderSetting )
        {
        case 0:
            {
            iNameOrder = ELastnameFirstname;
            break;
            }
        case 1:
            {
            iNameOrder = EFirstnameLastname;
            break;
            }
        case 2:
        default:
            {
            // Decide name order based on UI language: lastname-firstname
            // for Chinese, firstname-lastname for the rest of languages.
            TLanguage uiLang = User::Language();
            if ( uiLang == ELangPrcChinese || 
                 uiLang == ELangHongKongChinese ||
                 uiLang == ELangTaiwanChinese )
                {
                iNameOrder = ELastnameFirstname;
                }
            else
                {
                iNameOrder = EFirstnameLastname;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CPcsAlgorithm2Helper::SettingChangedL
// From MPbkGlobalSettingObserver
// ---------------------------------------------------------------------------
void CPcsAlgorithm2Helper::SettingChangedL( MPbkGlobalSetting::TPbkGlobalSetting aKey )
    {
    if ( aKey == MPbkGlobalSetting::ENameOrdering )
        {
        UpdateNameOrderL();
        }
    }

// End of file


